#!/usr/bin/perl -w

BEGIN { push @INC, (".", ".."); }

use strict;
use warnings;
# use diagnostics;
# use Carp;

# use QTestingUtils qw(:DEFAULT);
use QTestingUtils qw(runcurl load_reg filesizeIxNr);

my $dir = "/dev/shm/ssachdev";

#foreach my $qtype (qw(I1 I4)) {
foreach my $qtype (qw(I4)) {
    foreach my $sz (qw(1e5 1e6 1e7 1e8 2e8 1e9)) {
#    foreach my $sz (qw(1e5 1e6 1e7)) {
#    foreach my $sz (qw(1e9)) {
        my $size = int($sz);
        my $name = "$sz.$qtype";
        runcurl("gq list_free_mem");
        runcurl("gq add_fld nR=$size fldtype=$qtype filename=$dir/register.$name tbl=T1 h_fld=$name d_fld=$name");
        runcurl("gq list_free_mem");
        runcurl("gq mk_idx d_fld=$name");
        runcurl("gq store d_fld=$name");
        runcurl("gq list_free_mem");
        runcurl("gq del_fld d_fld=$name");
        runcurl("gq list_free_mem");
        runcurl("gq load nR=$size fldtype=$qtype filename=$dir/register.$name tbl=T1 h_fld=$name.in d_fld=$name.in");
        runcurl("gq list_free_mem");
        runcurl("gq del_fld d_fld=$name.in");
        runcurl("gq list_free_mem");
    }
}
