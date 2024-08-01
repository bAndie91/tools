###########################################
package Throttler;
###########################################
use strict;
use warnings;
use Log::Log4perl qw(:easy);

our $VERSION    = "0.08";
our $DB_VERSION = "1.1";

###########################################
sub new {
###########################################
    my($class, %options) = @_;

    my $self = {
        db_version      => $DB_VERSION,
        backend         => "Memory",
        backend_options => {},
        reset           => 0,
        %options,
    };

    if($self->{db_file}) {
        # legacy option, translate
        $self->{backend_options} = {
            db_file => $self->{db_file},
        };
        $self->{backend} = "YAML";
    }

    my $backend_class = "Throttler::Backend::$self->{backend}";

    $self->{db} = $backend_class->new( 
            %{ $self->{backend_options} } );

    $self->{changed} = 0;

    bless $self, $class;

    my $create = 1;

    if( $self->{ db }->exists() ) {
        DEBUG "Backend store exists";
        $self->lock();
        $self->{data} = $self->{ db }->load();

        $create = 0;

        if($self->{data}->{chain} and
           ($self->{data}->{chain}->{max_items} != $options{max_items} or
            $self->{data}->{chain}->{interval} != $options{interval})) {
            $self->{changed} = 1;
            $create = 1;
        }

        if($options{reset} or !$self->{ db }->backend_store_ok() ) {
            $create = 1;
        }
        $self->unlock();
    }
    
    if($create) {
        $self->{ db }->create( \%options ) or
            LOGDIE "Creating backend store failed";

          # create bucket chain
        $self->create( {
            max_items => $options{max_items},
            interval  => $options{interval},
        });

        $self->{db}->save( $self->{data} );
    }

    return $self;
}

###########################################
sub create {
###########################################
    my($self, $options) = @_;

    if( $self->{changed} ) {
        ERROR "Bucket chain parameters have changed ",
              "(max_items: $self->{data}->{chain}->{max_items}/",
              "$options->{max_items} ",
              "(interval: $self->{data}->{chain}->{interval}/",
              "$options->{interval})", ", throwing old chain away";
        $self->{changed} = 0;
    }

    DEBUG "Creating bucket chain max_items=$options->{max_items} ",
          "interval=$options->{interval}";

    $self->{data}->{chain} = Throttler::BucketChain->new(
            max_items => $options->{max_items},
            interval  => $options->{interval},
            );
}

###########################################
sub lock {
###########################################
    my($self) = @_;
    DEBUG "Trying to get lock ($$)";
    $self->{db}->lock();
    DEBUG "Lock on ($$)";
}

###########################################
sub unlock {
###########################################
    my($self) = @_;
    DEBUG "Lock off";
    $self->{db}->unlock();
}

###########################################
sub current_value {
###########################################
    my($self, %options) = @_;

    $self->{data} = $self->{db}->load();
    my $ret = $self->{data}->{chain}->current_value(%options);

    return $ret;
}

###########################################
sub try_push {
###########################################
    my($self, %options) = @_;

    if(exists $options{key}) {
        DEBUG "Pushing key $options{key}";
    } else {
        DEBUG "Pushing keyless item";
    }

    $self->lock();

    $self->{data} = $self->{db}->load();
    my $ret = $self->{data}->{chain}->try_push(%options);
    $self->{db}->save( $self->{data} );

    $self->unlock();
    return $ret;
}

###########################################
sub reset_key {
###########################################
    my($self, %options) = @_;

    if(exists $options{key}) {
        DEBUG "Resetting count for $options{key}";
    } else {
        DEBUG "Resetting count for keyless item";
    }

    $self->lock();

    $self->{data} = $self->{db}->load();
    my $ret = $self->{data}->{chain}->reset_key(%options);
    $self->{db}->save( $self->{data} );

    $self->unlock();
    return $ret;
}

###########################################
sub buckets_dump {
###########################################
    my($self) = @_;
    $self->lock();
    $self->{data} = $self->{db}->load();
    my $ret = $self->{data}->{chain}->as_string();
    $self->unlock();
    return $ret;
}

###########################################
sub buckets_rotate {
###########################################
    my($self) = @_;
    my $ret = $self->{data}->{chain}->rotate();
    return $ret;
}

package Throttler::Range;

###########################################
sub new {
###########################################
    my($class, $start, $stop) = @_;

    my $self = {
        start => $start,
        stop  => $stop,
    };
    bless $self, $class;
}

###########################################
sub min {
###########################################
    my($self) = @_;
    return $self->{start};
}

###########################################
sub max {
###########################################
    my($self) = @_;
    return $self->{stop};
}

###########################################
sub member {
###########################################
    my($self, $time) = @_;

    return ($time >= $self->{start} and $time <= $self->{stop});
}

###########################################
package Throttler::BucketChain;
###########################################
use Log::Log4perl qw(:easy);

our $DEFAULT_KEY = "_default";

###########################################
sub new {
###########################################
    my($class, %options) = @_;

    my $self = {
        max_items => undef,
        interval  => undef,
        %options,
    };

    if(!$self->{max_items} or
       !$self->{interval}) {
        LOGDIE "Both max_items and interval need to be defined";
    }

    if(!$self->{nof_buckets}) {
        $self->{nof_buckets} = 10;
    }

    if($self->{nof_buckets} > $self->{interval}) {
        $self->{nof_buckets} = $self->{interval};
    }

    bless $self, $class;

    $self->reset();

    return $self;
}

###########################################
sub reset {
###########################################
    my($self) = @_;

    $self->{buckets} = [];

    my $bucket_time_span = int ($self->{interval} / 
                                $self->{nof_buckets});

    $self->{bucket_time_span} = $bucket_time_span;

    my $time_start = time() -
        ($self->{nof_buckets}-1) * $bucket_time_span;

    for(1..$self->{nof_buckets}) {
        my $time_end = $time_start + $bucket_time_span - 1;
        DEBUG "Creating bucket ", hms($time_start), " - ", hms($time_end);
        push @{$self->{buckets}}, { 
            time  => Throttler::Range->new($time_start, $time_end),
            count => {},
        };
        $time_start = $time_end + 1;
    }

    $self->{head_bucket_idx} = 0;
    $self->{tail_bucket_idx} = $#{$self->{buckets}};
}

###########################################
sub first_bucket {
###########################################
    my($self) = @_;

    $self->{current_idx} = $self->{head_bucket_idx};
    return $self->{buckets}->[ $self->{current_idx} ];
}

###########################################
sub last_bucket {
###########################################
    my($self) = @_;

    $self->{current_idx} = $self->{tail_bucket_idx};
    return $self->{buckets}->[ $self->{current_idx} ];
}

###########################################
sub next_bucket {
###########################################
    my($self) = @_;

    return undef if $self->{current_idx} == $self->{tail_bucket_idx};

    $self->{current_idx}++;
    $self->{current_idx} = 0 if $self->{current_idx} > $#{$self->{buckets}};

    return $self->{buckets}->[ $self->{current_idx} ];
}

###########################################
sub as_string {
###########################################
    my($self) = @_;

    my @t;
    push @t, ["#", "idx", ("Time: " . hms(time)), "Key", "Count"];

    my $count = 1;

    for(my $b = $self->first_bucket(); $b; $b = $self->next_bucket()) {
        my $span = hms($b->{time}->min) . " - " . hms($b->{time}->max);
        my $idx  = $self->{current_idx};
        my $count_string = $count;

        if(! scalar keys %{$b->{count}}) {
            push @t, [$count_string, $idx, $span, "", ""];
        }

        foreach my $key (sort keys %{$b->{count}}) {
            push @t, [$count_string, $idx, $span, $key, $b->{count}->{$key}];
            $span = "";
            $count_string = "";
            $idx = "";
        }

        $count++;
    }
    return join('', map {"$_\n"} map {join "\t", @$_} @t);
}

###########################################
sub hms {
###########################################
    my($time) = @_;

    my ($sec,$min,$hour) = localtime($time);
    return sprintf "%02d:%02d:%02d", 
           $hour, $min, $sec;
}

###########################################
sub bucket_add {
###########################################
    my($self, $time) = @_;

      # ... and append a new one at the end
    my $time_start = $self->{buckets}->
                      [$self->{tail_bucket_idx}]->{time}->max + 1;
    my $time_end   = $time_start + $self->{bucket_time_span} - 1;

    DEBUG "Adding bucket: ", hms($time_start), " - ", hms($time_end);

    $self->{tail_bucket_idx}++;
    $self->{tail_bucket_idx} = 0 if $self->{tail_bucket_idx} >
                                    $#{$self->{buckets}};
    $self->{head_bucket_idx}++;
    $self->{head_bucket_idx} = 0 if $self->{head_bucket_idx} >
                                    $#{$self->{buckets}};

    $self->{buckets}->[ $self->{tail_bucket_idx} ] = { 
          time  => Throttler::Range->new($time_start, $time_end),
          count => {},
    };
}

###########################################
sub rotate {
###########################################
    my($self, $time) = @_;
    $time = time() unless defined $time;

    # If the last bucket handles a time interval that doesn't cover
    # $time, we need to rotate the bucket brigade. The first bucket
    # will be cleared and re-used as the new last bucket of the chain.

    DEBUG "Rotating buckets time=", hms($time), " ", 
          "head=", $self->{head_bucket_idx};

    if($self->last_bucket->{time}->{stop} >= $time) {
        # $time is still covered in the bucket brigade, we're golden
        DEBUG "Rotation not necessary (", 
              hms($self->last_bucket->{time}->{stop}),
              " - ", hms($time), ")";
        return 1;
    }

      # If we're too far off, just dump all buckets and re-init
    if($self->{buckets}->[ $self->{tail_bucket_idx} ]->{time}->max <
       $time - $self->{interval}) {
        DEBUG "Too far off, resetting (", hms($time), " >> ",
              hms($self->{buckets}->[ $self->{head_bucket_idx} ]->{time}->min),
              ")";
        $self->reset();
        return 1;
    }

    while($self->last_bucket()->{time}->min <= $time) {
        $self->bucket_add();
    }

    DEBUG "After rotation: ",
          hms($self->{buckets}->[ $self->{head_bucket_idx} ]->{time}->min),
          " - ",
          hms($self->{buckets}->[ $self->{tail_bucket_idx} ]->{time}->max),
          " (covers ", hms($time), ")";
}

###########################################
sub bucket_find {
###########################################
    my($self, $time) = @_;

    DEBUG "Searching bucket for time=", hms($time);

        # Search in the newest bucket first, chances are it's there
    my $last_bucket = $self->last_bucket();
    if($last_bucket->{time}->member($time)) {
        DEBUG hms($time), " covered by last bucket";
        return $last_bucket;
    }

    for(my $b = $self->first_bucket(); $b; $b = $self->next_bucket()) {
        if($b->{time}->member($time)) {
            DEBUG "Found bucket ", hms($b->{time}->min), 
                  " - ", hms($b->{time}->max);
            return $b;
        }
    }

    DEBUG "No bucket found for time=", hms($time);
    return undef;
}

###########################################
sub current_value {
###########################################
    my($self, %options) = @_;

    my $key = $DEFAULT_KEY;
    $key = $options{key} if defined $options{key};

    DEBUG "Getting current value for key=", $key;

    my $val = 0;

    for(0..$#{$self->{buckets}}) {
        $val += $self->{buckets}->[$_]->{count}->{$key} if
                exists $self->{buckets}->[$_]->{count}->{$key};
    }

    return $val;
}

###########################################
sub try_push {
###########################################
    my($self, %options) = @_;

    my $key = $DEFAULT_KEY;
    $key = $options{key} if defined $options{key};

    my $time = time();
    $time = $options{time} if defined $options{time};

    my $count = 1;
    $count = $options{count} if defined $options{count};

    my $force = 0;
    $force = $options{force} if defined $options{force};

    DEBUG "Trying to push $key ", hms($time), " $count";

    my $b = $self->bucket_find($time);

    if(!$b) {
       $self->rotate($time);
       $b = $self->bucket_find($time);
    }

    # Determine the total count for this key
    my $val = $self->current_value(%options);

    if($val >= $self->{max_items}) {
        if ($force) {
            DEBUG "Increasing (force) counter $key by $count ",
                  "($val|$self->{max_items})";
            $b->{count}->{$key} += $count;
        } else {
            DEBUG "Not increasing counter $key by $count (already at max)";
        }
        return 0;
    } else {
        DEBUG "Increasing counter $key by $count ",
              "($val|$self->{max_items})";
        $b->{count}->{$key} += $count;
        return 1;
    }

    LOGDIE "Time $time is outside of bucket range\n", $self->as_string;
    return undef;
}

###########################################
sub reset_key {
###########################################
    my ($self, %options) = @_;

    my $key = $DEFAULT_KEY;
    $key = $options{key} if defined $options{key};

    DEBUG "Resetting $key";

    my $total = 0;
    for(0..$#{$self->{buckets}}) {
        if (exists $self->{buckets}->[$_]->{count}->{$key}) {
            $total += $self->{buckets}->[$_]->{count}->{$key};
            $self->{buckets}->[$_]->{count}->{$key} = 0;
        }
    }

    return $total;
}

###########################################
package Throttler::Backend::Base;
###########################################

###########################################
sub new {
###########################################
    my($class, %options) = @_;

    my $self = { 
        %options,
    };

    bless $self, $class;
    $self->init();
    return $self;
}

sub exists { 0 }
sub create { 1 }
#sub save   { }
#sub load   { }
sub init   { }
sub lock   { }
sub unlock { }
sub backend_store_ok { 1 }

###########################################
package Throttler::Backend::Memory;
###########################################
use base 'Throttler::Backend::Base';

###########################################
sub save {
###########################################
    my($self, $data) = @_;
    $self->{data} = $data;
}

###########################################
sub load {
###########################################
    my($self) = @_;
    return $self->{data};
}

###########################################
package Throttler::Backend::YAML;
###########################################
use base 'Throttler::Backend::Base';
use Log::Log4perl qw(:easy);
use Fcntl qw(:flock);

###########################################
sub init {
###########################################
    my($self) = @_;

    require YAML;
    $YAML::LoadBlessed = 1;
}

###########################################
sub backend_store_ok {
###########################################
    my($self) = @_;

    # Legacy instances used DBM::Deep, but those data stores will be 
    # replaced by YAML backends. If we reuse a backend data store, make
    # sure it's a YAML file and not a DBM::Deep blob.
    if(! -f $self->{db_file} ) {
        return 1;
    }

    eval {
        $self->load();
    };

    if($@) {
        ERROR "$self->{db_file} apparently isn't a YAML file, we'll ",
              "have to dump it and rebuild the bucket chain in YAML";
        return 0;
    }

    return 1;
}

###########################################
sub exists {
###########################################
    my($self) = @_;

    return -f $self->{db_file};
}

###########################################
sub save {
###########################################
    my($self, $data) = @_;

    DEBUG "Saving YAML file $self->{db_file}";
    YAML::DumpFile( $self->{db_file}, $data );
}

###########################################
sub load {
###########################################
    my($self) = @_;

    DEBUG "Loading YAML file $self->{db_file}";
    return YAML::LoadFile( $self->{db_file} );
}

###########################################
sub lock {
###########################################
    my($self) = @_;

    open $self->{fh}, "+<", $self->{db_file} or 
        LOGDIE "Can't open $self->{db_file} for locking: $!";
    flock $self->{fh}, LOCK_EX;
}

###########################################
sub unlock {
###########################################
    my($self) = @_;
    flock $self->{fh}, LOCK_UN;
}

1;

__END__

=head1 NAME

Throttler - Limit data throughput

=head1 SYNOPSIS

    use Throttler;

    ### Simple: Limit throughput to 100 per hour

    my $throttler = Throttler->new(
        max_items => 100,
        interval  => 3600,
    );

    if($throttler->try_push()) {
        print "Item can be pushed\n";
    } else {
        print "Item needs to wait\n";
    }

    ### Advanced: Use a persistent data store and throttle by key:

    my $throttler = Throttler->new(
        max_items => 100,
        interval  => 3600,
        backend   => "YAML",
        backend_options => {
            db_file => "/tmp/mythrottle.yml",
        },
    );

    if($throttler->try_push(key => "somekey")) {
        print "Item can be pushed\n";
    }

=head1 DESCRIPTION

C<Throttler> helps solving throttling tasks like "allow a single
IP only to send 100 emails per hour". It provides an optionally persistent
data store to keep track of what happened before and offers a simple
yes/no interface to an application, which can then focus on performing
the actual task (like sending email) or suppressing/postponing it.

When defining a throttler, you can tell it to keep its
internal data structures in memory:

      # in-memory throttler
    my $throttler = Throttler->new(
        max_items => 100,
        interval  => 3600,
    );

However, if the data structures need to be maintained across different
invocations of a script or several instances of scripts using the
throttler, using a persistent database is required:

      # persistent throttler
    my $throttler = Throttler->new(
        max_items => 100,
        interval  => 3600,
        backend   => "YAML",
        backend_options => {
            db_file => "/tmp/mythrottle.yml",
        },
    );

The call above will reuse an existing backend store, given that the
C<max_items> and C<interval> settings are compatible and leave the
stored counter bucket chain contained therein intact. To specify that
the backend store should be rebuilt and all counters be reset, use 
the C<reset =E<gt> 1> option of the Throttler object constructor.

In the simplest case, C<Throttler> just keeps track of single 
events. It allows a certain number of events per time frame to succeed
and it recommends to block the rest:

    if($throttler->try_push()) {
        print "Item can be pushed\n";
    } else {
        print "Item needs to wait\n";
    }

the C<force =E<gt> 1> option of the try_push() method will cause the
counter to be incremented regardless of threshold for use in scenarios
where max_items is a threshold rather than throttle condition:

    if($throttler->try_push('force' => 1)) {
        print "Item can be pushed\n";
    } else {
        print "Counter incremented, Item needs to wait\n";
    }

When throttling different categories of items, like attempts to send
emails by IP address of the sender, a key can be used:

    if($throttler->try_push( key => "192.168.0.1" )) {
        print "Item can be pushed\n";
    } else {
        print "Item needs to wait\n";
    }

In this case, each key will be tracked separately, even if the quota
for one key is maxed out, other keys will still succeed until their
quota is reached.

=head2 HOW IT WORKS

To keep track of what happened within the specified time frame, 
C<Throttler> maintains a round-robin data store, either in 
memory or on disk. It splits up the controlled time interval into
buckets and maintains counters in each bucket:

    1 hour ago                     Now
      +-----------------------------+
      | 3  | 7  | 0  | 0  | 4  | 1  |
      +-----------------------------+
       4:10 4:20 4:30 4:40 4:50 5:00

To decide whether to allow a new event to happen or not, C<Throttler>
adds up all counters (3+7+4+1 = 15) and then compares the result 
to the defined threshold. If the event is allowed, the corresponding 
counter is increased (last column):

    1 hour ago                     Now
      +-----------------------------+
      | 3  | 7  | 0  | 0  | 4  | 2  |
      +-----------------------------+
       4:10 4:20 4:30 4:40 4:50 5:00

While time progresses, old buckets are expired and then reused
for new data. 10 minutes later, the bucket layout would look like this:

    1 hour ago                     Now
      +-----------------------------+
      | 7  | 0  | 0  | 4  | 2  | 0  |
      +-----------------------------+
       4:20 4:30 4:40 4:50 5:00 5:10

=head2 LOCKING

When used with a persistent data store, C<Throttler> protects
competing applications from clobbering the database by using the locking
mechanism offered with C<DBM::Deep>. Both the C<try_push()> and the
C<buckets_dump> function already perform locking behind the scenes.

If you see a need to lock the data store yourself, i.e. when trying to 
push counters for several keys simultaneously, use

    $throttler->lock();

and

    $throttler->unlock();

to protect the data store against competing applications.

=head2 RESETTING

Sometimes, you may need to reset a specific counter, e.g. if an IP
address has been unintentionally throttled:

    my $count = $throttler->reset_key(key => "192.168.0.1");

The C<reset_key> method returns the total number of attempts so far.

=head2 ADVANCED USAGE

By default, C<Throttler> will decide on the number of buckets by 
dividing the time interval by 10. It won't handle sub-seconds, though,
so if the time interval is less then 10 seconds, the number of buckets
will be equal to the number of seconds in the time interval.

If the default bucket allocation is unsatisfactory, you can specify 
it yourself:

    my $throttler = Throttler->new(
        max_items   => 100,
        interval    => 3600,
        nof_buckets => 42,
    );

Mainly for debugging and testing purposes, you can specify a different
time than I<now> when trying to push an item:

    if($throttler->try_push(
          key  => "somekey",
          time => time() - 600 )) {
        print "Item can be pushed in the past\n";
    }

Also for debugging and testing purposes, you can obtain the current
value of an item:

    my $val = $throttler->current_value(key => "somekey");

Speaking of debugging, there's a utility method C<buckets_dump> which
returns a string containing lines with tab-separated cells in them
representing what's in each bucket.

So the code

    use Throttler;
    
    my $throttler = Throttler->new(
        interval  => 3600,
        max_items => 10,
    );

    $throttler->try_push(key => "foobar");
    $throttler->try_push(key => "foobar");
    $throttler->try_push(key => "barfoo");
    print $throttler->buckets_dump();

will print out something like

   	#	idx	Time: 14:43:00	Key	Count	
	1	0	13:49:00 - 13:54:59			
	2	1	13:55:00 - 14:00:59			
	3	2	14:01:00 - 14:06:59			
	4	3	14:07:00 - 14:12:59			
	5	4	14:13:00 - 14:18:59			
	6	5	14:19:00 - 14:24:59			
	7	6	14:25:00 - 14:30:59			
	8	7	14:31:00 - 14:36:59			
	9	8	14:37:00 - 14:42:59			
	10	9	14:43:00 - 14:48:59	barfoo	1	
				foobar	2	

and allow for further investigation.

=head1 LICENSE

Copyright 2007 by Mike Schilli, all rights reserved.
This program is free software, you can redistribute it and/or
modify it under the same terms as Perl itself.

=head1 AUTHOR

2007, Mike Schilli <cpan@perlmeister.com>
