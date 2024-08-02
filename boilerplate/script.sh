#!/bin/bash

true <<'EOF'
=pod

=head1 NAME

name - description

=head1 SYNOPSIS

name [I<OPTIONS>] [--]

=head1 DESCRIPTION

=head1 OPTIONS

=over 4

=item -v, --verbose

=back

=head1 LIMITATIONS

=head1 SEE ALSO

=cut

EOF


set -e
set -o pipefail
set -u

. /usr/lib/tool/bash-utils


opt_verbose=no


while [ $# != 0 ]
do
	case "$1" in
	--help)
		pod2text "$0"
		exit 0
		;;
	-v|--verbose)
		opt_verbose=yes
		;;
	--)
		shift
		break
		;;
	-*)
		errx -1 "unknown option: $1"
		;;
	*)
		break
		;;
	esac
	shift
done


