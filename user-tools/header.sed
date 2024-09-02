#!/bin/sh

exec sed -e '/^\r\?$/q' --separate "$@"


true <<EOF

=pod

=head1 NAME

header.sed - Echo the input stream up to the first empty line (usual end-of-header marker)

=cut

EOF
