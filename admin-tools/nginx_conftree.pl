#!/usr/bin/env perl

use Data::Dumper;
use Getopt::Long;

$output = "plain";


if($ENV{'FCGI_ROLE'}) {
	print "Content-Type: text/plain\r\n\r\n";
}


GetOptions(
    'o|output=s'        => \$output,
    'f|file=s'		=> \$nginx_conf,
)
or die "Usage: $0 [-f config_file] [-o <plain|dot|merge>]\n";

undef $/;


if(not $nginx_conf) {
	open $fh, '-|', "nginx -V 2>&1";
	$_ = <$fh>;
	close $fh;

	/--conf-path=(\S+)/ and $nginx_conf = $1 or die "No '--conf-path' compile option found for nginx.\n";
}

($nginx_conf_root) = ($nginx_conf =~ /^(.*)\/+[^\/]+$/);


sub readconffile {
	my $filespec = shift;
	my $indent = shift;
	my $caller = shift;

	$filespec = "$nginx_conf_root/$filespec" if $filespec !~ /^\//;
	
	for my $file (glob $filespec) {
		my ($rel_file) = $file;
		$rel_file =~ s/^\Q$nginx_conf_root\E\/+//;
		
		if($output eq "plain") {
			printf "%s%s\n", "  "x$indent, $rel_file;
		}
		if($output eq "dot" and $caller) {
			printf "\"%s\" -> \"%s\";\n", $caller, $rel_file;
		}
		
		open my $fh, '<', $file or warn "$file: $!\n";
		my $conf = <$fh>;
		close $fh;
		
		if($output eq "merge") {
			print "\n#!", "#" x $indent, " file: $file\n";
		}

		while(1) {
			if($conf =~ /(^|;)\s*include\s+(\S+?)\s*;/sm) {
				if($output eq "merge") {
					print $`.$1;
				}
				
				my $child = $2;
				$conf = $';
				readconffile($child, $indent+1, $rel_file);
			}
			else {
				if($output eq "merge") {
					print $conf;
				}
				last;
			}
		}
		
		if($output eq "merge") {
			print "\n#!", "#" x $indent, " end of file: $file\n";
		}
	}
}

if($output eq "dot") {
	print "
	digraph \"nginx \" {
		rankdir=LR;
		concentrate=true;
";
}

readconffile($nginx_conf, 0);

if($output eq "dot") {
	print "
	}
";
}

