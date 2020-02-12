#!/usr/bin/env perl

use File::Slurp; ### sudo apt install -y libfile-slurp-perl
# # use List::MoreUtils qw(uniq);
# # use constant EXIT_CODE_SUCCESS => 0; # SUCCESS

if ($#ARGV + 1 < 1) { die "Not enough args. Usage: my2smf.pl file"; }
my $if = $ARGV[0];
my $of = $if; $of =~ s/.my$/.smf/;
# print "Converting $if to $of\n";

### Read file
#my @if = grep { /^\s*\S.*$/ } read_file($if, chomp => 1);
my @if = grep { /^.*$/ } read_file($if, chomp => 1);

# # ### Debug, dump file:
# # # print "f1 ($ARGV[0]):\n"; for (@f1) { print " $_\n"; } print "\n";

my ($minx,$miny,$minz,$maxx,$maxy,$maxz);
$minz = 0; $maxz = 0;
my $iv = 0; ### vertex index
### For each line $i
for (my $i = 0; $i <= $#if; $i ++) {
    # print "$if[$i]\n";
    if ($if[$i] =~ m"^G1 X(\S+) Y(\S+)") {
        # print "$if[$i]\n";
        my ($x,$y) = ($1,$2);
        $iv = $iv + 1;
        if ($iv eq 1) { ($minx,$miny,$maxx,$maxy) = ($x,$y,$x,$y); }
        if ($x < $minx) { $minx = $x; }
        if ($y < $miny) { $miny = $y; }
        if ($x > $maxx) { $maxx = $x; }
        if ($y > $maxy) { $maxy = $y; }
    }
    if ($if[$i] =~ m" Z(\S+)\s") {
        my $z = $1;
        if ($z > $maxz) { $maxz = $z; }
    }
}
print "MIN: $minx,$miny,$minz\n";
print "MAX: $maxx,$maxy,$maxz\n";
