
use Data::Dumper;
use Getopt::Long;
use Perl::Tokenizer;

# common defaults
$IFS = '\s+';
$OFS = "\t";

sub strip
{
	my $res = $_[0];
	chomp $res;
	$res =~ s/^\s*//;
	$res =~ s/\s*$//;
	return $res;
}

sub parse_column_headers
{
	# parse the first line as column headers
	my $headers = $_[0];
	my @COLUMNS = split /$IFS/, strip $headers;
	return @COLUMNS;
}

sub parse_record
{
	my $record_raw = shift;
	my @column_names = @_;
	
	my %FIELD = ();
	my @FIELDS = split /$IFS/, strip $record_raw;
	my $col_num = 0;
	
	for my $col_name (@column_names)
	{
		$FIELD{$col_name} = $FIELDS[$col_num];
		$col_num++;
	}
	return %FIELD;
}

sub transform_to_perl_expression
{
	my $field_var_name = $_[0];
	my $tdat_expr = $_[1];
	my @perl_expr;
	
	perl_tokens {
		my ($token_type, $start, $end) = @_;
		my $token = substr $tdat_expr, $start, $end-$start;
		if($token_type eq 'bare_word')
		{
			$token = '$'.$field_var_name.'{'."'".$token."'".'}';
		}
		push @perl_expr, $token;
	} $tdat_expr;
	
	my $perl_expr = join '', @perl_expr;
	return $perl_expr;
	
	# TODO: support "name.sub" style column names too
	# TODO: support "column name" style column names too
}

sub vardump
{
	my $terse = $Data::Dumper::Terse;
	$Data::Dumper::Terse = 1;
	my $ret = Dumper @_;
	$Data::Dumper::Terse = $terse;
	return $ret;
}

1;
