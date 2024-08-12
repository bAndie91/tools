#!/usr/bin/env perl

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


use constant { STAT_DEV=>0, STAT_INODE=>1, STAT_PERM=>2, STAT_NLINKS=>3, STAT_UID=>4, STAT_GID=>5, STAT_RDEV=>6, STAT_SIZE=>7, STAT_ATIME=>8, STAT_MTIME=>9, STAT_CTIME=>10, STAT_BLOCKSIZE=>11, STAT_BLOCKS=>12, };
use Cwd qw/getcwd realpath/;
use Data::Dumper;
use Date::Parse;
use DateTime::Format::Strptime;
use Encode qw/decode encode decode_utf8 encode_utf8/;
use Errno qw/:POSIX/;
use Fcntl qw/:flock :seek F_GETFL F_SETFL O_NONBLOCK F_GETFD F_SETFD FD_CLOEXEC/;
use File::Basename;
use File::Temp qw/tempfile/;
use Getopt::Long qw/:config no_ignore_case no_bundling no_getopt_compat no_auto_abbrev require_order/;
use IPC::Run qw/run/;
use List::MoreUtils qw/all any none/;
use Pod::Usage;
use POSIX;
use Socket qw/AF_UNIX AF_INET SOCK_STREAM pack_sockaddr_in inet_aton sockaddr_un/;
no if ($] >= 5.018), 'warnings' => 'experimental::smartmatch';

$OptVerbose = 0;

GetOptions(
	'v|verbose!' => \$OptVerbose,
	'help' => sub { pod2usage(-exitval=>0, -verbose=>99); },
	'<>' => sub { unshift @ARGV, @_[0]; die '!FINISH'; },
) or pod2usage(-exitval=>2, -verbose=>99);


