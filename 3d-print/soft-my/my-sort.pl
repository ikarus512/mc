#!/usr/bin/env perl

use File::Slurp; ### sudo apt install -y libfile-slurp-perl
# # use List::MoreUtils qw(uniq);
# # use constant EXIT_CODE_SUCCESS => 0; # SUCCESS

if ($#ARGV + 1 < 1) { die "Not enough args. Usage: my2smf.pl file"; }
my $if = $ARGV[0];
my ($suffix, $shiftx, $shifty, $shiftz) = ($ARGV[1], $ARGV[2], $ARGV[3], $ARGV[4]);
$shiftx =~ s/^.//;
$shifty =~ s/^.//;
$shiftz =~ s/^.//;
my $of = $if; $of =~ s/.my$/.smf/;
# print "Converting $if to $of\n";

### Read file
#my @if = grep { /^\s*\S.*$/ } read_file($if, chomp => 1);
my @if = grep { /^.*$/ } read_file($if, chomp => 1);

# # ### Debug, dump file:
# # # print "f1 ($ARGV[0]):\n"; for (@f1) { print " $_\n"; } print "\n";

my ($minx,$miny,$minz);
my $iv = 0; ### vertex index
### For each line $i
for (my $i = 0; $i <= $#if; $i ++) {
    if ($if[$i] =~ m"^\s*v\s+(\S+)\s+(\S+)\s+(\S+)\s+(\S+)\s*") { ### vertex
        print "$if[$i]\n";
    }
}
#print "$minx,$miny,$minz\n";

my %vh;
$iv = 0; ### vertex index
### For each line $i
for (my $i = 0; $i <= $#if; $i ++) {
    # print "$if[$i]\n";
    if ($if[$i] =~ m"^\s*v\s+(\S+)\s+(\S+)\s+(\S+)\s+(\S+)\s*" || # vertex
        $if[$i] =~ m"^\s*#" ||                                    # comment
        $if[$i] =~ m"^\s*$"                                       # empty line
    ) { ### vertex
    } else {
        print "$if[$i]\n";
    }
}
