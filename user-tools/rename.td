#!/usr/bin/perl -w
#
#  This script was developed by Robin Barker (Robin.Barker@npl.co.uk),
#  from Larry Wall's original script eg/rename from the perl source.
#
#  Then further developed by A Hrubak and renamed to rename.td,
#  because he was not aware of newer versions, like file-rename(1p) (alias prename).
#
#  This script is free software; you can redistribute it and/or modify it
#  under the same terms as Perl itself.
#
# Larry(?)'s RCS header:
#  RCSfile: rename,v   Revision: 4.1   Date: 92/08/07 17:20:30 
#
# $RCSfile: rename,v $$Revision: 1.5 $$Date: 1998/12/18 16:16:31 $
#
# $Log: rename,v $
# Revision 1.5  1998/12/18 16:16:31  rmb1
# moved to perl/source
# changed man documentation to POD
#
# Revision 1.4  1997/02/27  17:19:26  rmb1
# corrected usage string
#
# Revision 1.3  1997/02/27  16:39:07  rmb1
# added -v
#
# Revision 1.2  1997/02/27  16:15:40  rmb1
# *** empty log message ***
#
# Revision 1.1  1997/02/27  15:48:51  rmb1
# Initial revision
#

use strict;

use Getopt::Long;
Getopt::Long::Configure('bundling');

my $verbose = 0;
my ($no_act, $force, $op);

die "Usage: rename [-v] [-n] [-f] perlexpr [filenames]\n"
unless GetOptions(
	'v|verbose+' => \$verbose,
	'n|no-act|dry-run|dryrun' => \$no_act,
	'f|force' => \$force,
) and $op = shift;

if (!@ARGV) {
    print STDERR "reading filenames from STDIN\n" if $verbose;
    @ARGV = <STDIN>;
    chomp(@ARGV);
}

$header_emitted = 0;
$highest_errno = 0;

sub emit_header
{
	return if $header_emitted;
	print join "\t", "STATUS", "OLD", "NEW";
	print "\n";
}

for (@ARGV)
{
    my $was = $_;
    eval $op;
    die $@ if $@;
    
    my $status;
    
    if($was eq $_)
    {
        $status = "KEEP" if $verbose >= 2;
    }
    elsif (-e $_ and !$force)
    {
    	$status = "SKIP";
    }
    else
    {
    	if ($no_act)
        {
        	$status = "WOULD";
        }
        else
        {
            if(rename $was, $_)
            {
            	$status = "OK" if $verbose;
            }
            else
            {
            	my $errno = int $!;
            	$status = "ERR $errno";
            	$highest_errno = $errno if $errno > $highest_errno;
            }
        }
    }
    
    if(defined $status)
    {
    	emit_header;
	    print join "\t", $status, $was, $_;
    	print "\n";
    }
}

exit $highest_errno;

__END__

=head1 NAME

rename.td - rename multiple files by a Perl expression

=head1 SYNOPSIS

rename.td S<[ B<-v>[B<v>] ]> S<[ B<-n> ]> S<[ B<-f> ]> I<perlexpr> S<[ I<files> ]>

cat files.list | rename.td S<[ B<-v>[B<v>] ]> S<[ B<-n> ]> S<[ B<-f> ]> I<perlexpr>

=head1 DESCRIPTION

B<rename.td> renames the files supplied according to the rule specified as the first argument.
The I<perlexpr> argument is a Perl expression which is expected to modify the C<$_>
string in Perl for at least some of the filenames specified.
If a given filename is not modified by the expression, it will not be renamed.
If no filenames are given on the command line, filenames will be read via standard input.

For example, to rename all files matching C<*.bak> to strip the extension,
you might say

	rename.td 's/\.bak$//' *.bak

To translate uppercase names to lower, you'd use

	rename.td 'y/A-Z/a-z/' *

=head1 OPTIONS

=over 8

=item B<-v>, B<--verbose>

Verbose: print names of files successfully renamed.

=item B<-vv>

Verbose extra: print names of files of which name is not changed.

=item B<-n>, B<--dry-run>, B<--no-act>

No Action: show what files would have been renamed, or skipped.

=item B<-f>, B<--force>

Force: overwrite existing files.

=back

=head1 OUTPUT

Output Tab-delimited fields line-by-line.
First line is the headers.
Each subsequent line describes a file in this way:

=over 4

=item 1st field - status

=over 8

=item B<KEEP> - no change in file name, shown in B<-vv> mode

=item B<SKIP> - destination already exists, not in B<--force> mode

=item B<WOULD> - would be attempted to rename, in B<--dry-run> mode

=item B<OK> - successfully renamed

=item B<ERR I<nnn>> - error happened during rename, error code is I<nnn>

=back

=item 2nd field - old file name

=item 3rd field - new file name

=back

=head1 EXIT STATUS

Zero when all rename succeeded, otherwise the highest error number of all the failed renames, if any.
See rename(2) for these error numbers.

=head1 ENVIRONMENT

No environment variables are used.

=head1 CREDITS

Larry Wall (author of the original)

Robin Barker 

=head1 SEE ALSO

mv(1), perl(1), rename(2), file-rename(1p) (prename(1)), rename.ul (rename(1)), renamemanual(1)

=head1 DIAGNOSTICS

If you give an invalid Perl expression you'll get a syntax error.

=head1 BUGS

The original C<rename> did not check for the existence of target filenames,
so had to be used with care.  I hope I've fixed that (Robin Barker).

=cut
