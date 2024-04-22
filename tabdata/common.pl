#!/usr/bin/env perl

$0 =~ s/^.*\/([^\/]+)$/$1/;

$FS = "\t";
$RS = $/ = "\n";
@Header = ();
%Header = ();
$OptShowHeader = 1;
$OptWarnBadColumnNames = 1;
$OptFailBadColumnNames = 1;
$OptFailBadNegativeColumnNames = 0;
$OptAddExtraColumns = 1;
$OptMinColumnSpacing = 2;
$OptMaxColumns = undef;
@OptPredefColumns = ();

no if ($] >= 5.018), 'warnings' => 'experimental::smartmatch';
use Getopt::Long qw/:config no_ignore_case bundling pass_through require_order no_getopt_compat/;
use Pod::Usage;

GetOptions(
	'h|header' => sub { $OptShowHeader = 1; },
	'H|no-header' => sub { $OptShowHeader = 0; },
	
	'i|ignore-nonexisting-columns' => sub { $OptFailBadColumnNames = 0; $OptWarnBadColumnNames = 0; },
	'w|warn-nonexisting-columns' => sub { $OptFailBadColumnNames = 0; $OptWarnBadColumnNames = 1; },
	'strict-columns' => sub { $OptWarnBadColumnNames = 1; $OptFailBadColumnNames = 1; $OptFailBadNegativeColumnNames = 1; },
	
	'm|min-column-spacing=i' => \$OptMinColumnSpacing,
	'c|max-columns=i' => \$OptMaxColumns,
	
	's|separator=s' => \$OptSeparatorRegexp,
	
	'x|extra-columns' => sub { $OptAddExtraColumns = 1; },
	'X|no-extra-columns' => sub { $OptAddExtraColumns = 0; },
	
	'C|columns=s@' => \@OptPredefColumns,
	
	'help|?' => sub{ pod2usage(-exitval=>0, -verbose=>99); },
) or pod2usage(-exitval=>2, -verbose=>99);

# TODO separate GetOptions() per td-* tool

if('--' ~~ @ARGV and $ARGV[0] ne '--')
{
	# at least 1 unknown option remained in @ARGV
	pod2usage(-exitval=>2, -verbose=>99, -msg=>"$0: unknown parameter: $ARGV[0]");
}

# when pass_through option is set:
# Note that the options terminator (default "--"), if present, will also be passed through in @ARGV.
shift @ARGV if $ARGV[0] eq '--';


sub process_header
{
	$HeaderLine = $_[0];
	chomp $HeaderLine;
	@Header = split $FS, $HeaderLine;
	%Header = ();
	for my $idx (0..$#Header)
	{
		$Header{$Header[$idx]} = $idx;
	}
}

sub read_record
{
	my $fd = shift;
	my $line = <$fd>;
	chomp $line;
	my @record = split $FS, $line;
	return @record;
}

1;
