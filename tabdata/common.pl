#!/usr/bin/env perl

$0 =~ s/^.*\/([^\/]+)$/$1/;

# Note: review escape_tabdata subroutine before you think about changing $FS and $RS fundamental variables.
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
$OptWarnNonNumericRValue = 1;
$OptFailNonNumericRValue = 1;

no if ($] >= 5.018), 'warnings' => 'experimental::smartmatch';
use Getopt::Long qw/:config no_ignore_case bundling pass_through require_order no_getopt_compat no_auto_abbrev/;
use Pod::Usage;

GetOptions(
	'h|header' => sub { $OptShowHeader = 1; },
	'H|no-header' => sub { $OptShowHeader = 0; },
	
	'i|ignore-non-existing-columns' => sub { $OptFailBadColumnNames = 0; $OptWarnBadColumnNames = 0; },
	'w|warn-non-existing-columns' => sub { $OptFailBadColumnNames = 0; $OptWarnBadColumnNames = 1; },
	'strict-columns' => sub { $OptWarnBadColumnNames = 1; $OptFailBadColumnNames = 1; $OptFailBadNegativeColumnNames = 1; },
	
	'N|no-fail-non-numeric' => sub { $OptFailNonNumericRValue = 0; },
	'W|no-warn-non-numeric' => sub { $OptWarnNonNumericRValue = 0; },
	
	'm|min-column-spacing=i' => \$OptMinColumnSpacing,
	'c|max-columns=i' => \$OptMaxColumns,
	
	's|separator=s' => \$OptSeparatorRegexp,
	'g|multiline-glue|multivalue-glue=s' => \$OptMultilineGlue,
	
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

sub sys_read_line
{
	# read data from STDIN until $RS (usually newline) or EOF, whichever comes first.
	# and return with data.
	# $RS is consumed by not returned.
	# useful if the rest of STDIN will not be processed by the current process.
	
	my $line = '';
	my $c;
	$line .= $c while sysread(STDIN, $c, 1) and $c ne $RS;
	return $line;
}

sub read_record
{
	my $fd = shift;
	my $line = <$fd>;
	chomp $line;
	my @record = split $FS, $line;
	return @record;
}

sub escape_tabdata
{
	my $arbitrary_data = shift;
	# Note, may be wrong if $FS or $RS are changed.
	my $tabdata = $arbitrary_data =~ s/[\t\n\r\e\\]/'\\'.{"\t"=>'t', "\n"=>'n', "\r"=>'r', "\e"=>'e', "\\"=>'\\'}->{$&}/ger;
	return $tabdata;
}

sub unescape_tabdata
{
	my $tabdata = shift;
	my $raw = $tabdata =~ s/\\[tnre\\]/eval "\"$&\""/ger;
	return $raw;
}

1;
