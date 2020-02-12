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

my ($minx,$miny,$minz);
my $iv = 0; ### vertex index
### For each line $i
for (my $i = 0; $i <= $#if; $i ++) {
    # print "$if[$i]\n";
    if ($if[$i] =~ m"^\s*v\s+(\S+)\s+(\S+)\s+(\S+)\s+(\S+)\s*") { ### vertex
        my ($vname,$x,$y,$z) = ($1,$2,$3,$4);
        $iv = $iv + 1;
        if ($iv eq 1) { ($minx,$miny,$minz) = ($x,$y,$z); }
        # print "vertex $vh{$vname}: $vname, $x, $y, $z\n";
        if ($x < $minx) { $minx = $x; }
        if ($y < $miny) { $miny = $y; }
        if ($z < $minz) { $minz = $z; }
    }
}
#print "$minx,$miny,$minz\n";

my %vh;
$iv = 0; ### vertex index
### For each line $i
for (my $i = 0; $i <= $#if; $i ++) {
    # print "$if[$i]\n";
    if ($if[$i] =~ m"^\s*v\s+(\S+)\s+(\S+)\s+(\S+)\s+(\S+)\s*") { ### vertex
        my ($vname,$x,$y,$z) = ($1,$2,$3,$4);
        $iv = $iv + 1;
        $vh{$vname} = $iv;
        # print "vertex $vh{$vname}: $vname, $x, $y, $z\n";
        my $newx = $x - $minx;
        my $newy = $y - $miny;
        my $newz = $z - $minz;
        print "v $newx $newy $newz\n";
    } elsif ($if[$i] =~ m"^\s*([^v]\w+)\s+(\w+)\s+(\w+)\s*(#.*)?$") { ### 3-point face
        my ($v1,$v2,$v3) = ($1,$2,$3);
        # print "3-point face $if[$i]\n";
        print "f $vh{$v1} $vh{$v2} $vh{$v3}\n";
        #print "f $v3 $v2 $v1\n";
    } elsif ($if[$i] =~ m"^\s*([^v]\w+)\s+(\w+)\s+(\w+)\s+(\w+)\s*(#.*)?$") { ### 4-point face
        my ($v1,$v2,$v3,$v4) = ($1,$2,$3,$4);
        # print "4-point face $if[$i]\n";
        print "f $vh{$v1} $vh{$v2} $vh{$v3}\n";
        print "f $vh{$v1} $vh{$v3} $vh{$v4}\n";
        #print "f $v3 $v2 $v1\n";
        #print "f $v4 $v3 $v1\n";
    } elsif ($if[$i] =~ m"^\s*([^v]\w+)\s+(\w+)\s+(\w+)\s+(\w+)\s+(\w+)\s*(#.*)?$") { ### 5-point face
        my ($v1,$v2,$v3,$v4,$v5) = ($1,$2,$3,$4,$5);
        # # print "5-point face $if[$i]\n";
        print "f $vh{$v1} $vh{$v2} $vh{$v3}\n";
        print "f $vh{$v1} $vh{$v3} $vh{$v4}\n";
        print "f $vh{$v1} $vh{$v4} $vh{$v5}\n";
        #print "f $v3 $v2 $v1\n";
        #print "f $v4 $v3 $v1\n";
        #print "f $v5 $v4 $v1\n";
    } else {
        print "$if[$i]\n";
    }

}
