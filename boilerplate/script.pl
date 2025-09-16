#!/usr/bin/env perl

=pod

=head1 NAME

scriptname - short description

=head1 SYNOPSIS

scriptname [I<OPTIONS>] [--] I<COMMAND> [I<ARGS>]

=head1 DESCRIPTION

=head1 OPTIONS

=over 4

=item -v, --verbose

=back

=head1 ENVIRONMENT

=head1 LIMITATIONS

=head1 SEE ALSO

=cut


use File::stat;
use Cwd qw/getcwd realpath/;
use Data::Dumper;
use Date::Parse;
use DateTime::Format::Strptime;
use Encode qw/decode encode decode_utf8 encode_utf8/;
use utf8;
use open ':std', ':utf8';  # mark stdio as utf8 but not verify
use open ':utf8';
use Errno qw/:POSIX/;
use Fcntl qw/:flock :seek F_GETFL F_SETFL O_NONBLOCK F_GETFD F_SETFD FD_CLOEXEC/;
use File::Basename;
use File::Temp qw/tempfile/;
use Getopt::Long qw/:config no_ignore_case no_bundling no_getopt_compat no_auto_abbrev pass_through permute/;
use IPC::Run qw/run/;
use List::MoreUtils qw/all any none/;
use Pod::Usage;
use POSIX qw/:sys_wait_h strftime/;
use Socket qw/AF_UNIX AF_INET SOCK_STREAM pack_sockaddr_in inet_aton sockaddr_un/;
no if ($] >= 5.018), 'warnings' => 'experimental::smartmatch';

$OptVerbose = 0;

GetOptions(
	'v|verbose!' => \$OptVerbose,
	'help' => sub { pod2usage(-exitval=>0, -verbose=>99); },
	'<>' => sub { unshift @ARGV, $_[0]; die '!FINISH'; },
) or pod2usage(-exitval=>2, -verbose=>99);


