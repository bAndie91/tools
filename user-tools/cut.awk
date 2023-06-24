#!/bin/bash

true <<EOF
=pod

=head1 NAME

cut.awk - Output only the selected fields from the input stream, parameters follow awk(1) syntax

=head1 SEE ALSO

awk-cut(1)

=cut

EOF


fields=''

for fnum
do
	fields=$fields${fields:+,}\$$fnum
done

exec awk "{print $fields}"
