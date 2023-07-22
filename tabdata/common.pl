
$0 =~ s/^.*\/([^\/]+)$/$1/;

$IFS = "\t";
@Header = ();
%Header = ();
$OptNoHeader = 0;

use Getopt::Long qw/:config no_ignore_case/;

GetOptions(
	'H|no-header' => \$OptNoHeader,
) or die "Usage: $0 [-H | --no-header] ...\n";

sub process_header
{
	$HeaderLine = @_[0];
	chomp $HeaderLine;
	@Header = split $IFS, $HeaderLine;
	%Header = ();
	for my $idx (0..$#Header)
	{
		$Header{$Header[$idx]} = $idx;
	}
}

1;
