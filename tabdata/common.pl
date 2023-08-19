
$0 =~ s/^.*\/([^\/]+)$/$1/;

$FS = "\t";
@Header = ();
%Header = ();
$OptNoHeader = 0;

use Getopt::Long qw/:config no_ignore_case pass_through require_order no_getopt_compat/;
use Pod::Usage;

GetOptions(
	'H|no-header' => \$OptNoHeader,
	'help|?' => sub{ pod2usage(-exitval=>0, -verbose=>99); },
) or pod2usage(-exitval=>2, -verbose=>99);

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
	$HeaderLine = @_[0];
	chomp $HeaderLine;
	@Header = split $FS, $HeaderLine;
	%Header = ();
	for my $idx (0..$#Header)
	{
		$Header{$Header[$idx]} = $idx;
	}
}

1;
