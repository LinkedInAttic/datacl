#!/usr/bin/env perl

BEGIN { push @INC, (".", ".."); }

use strict;
use warnings;
# use diagnostics;
# use Carp;

use QTestingUtils qw(runcurl load_reg filesizeIxNr);

my $datapath = "/home/ssachdev/approxalgos/GPU_Work_Final2/dataScience/QGPU/unit_tests/datagen2/data";

my $src_link = "$datapath/500e6_sorted_buckets_I4.bin";
my $dst_link = "$datapath/50e6_sorted_buckets_I4.bin";

runcurl("gq load nR=" . int(500e6) . " fldtype=I4 filename=" . $src_link . " tbl=T1 h_fld=src_link d_fld=src_link");
runcurl("gq add_fld nR=" . int(500e6) . " fldtype=I4 filename=dummy tbl=T1 h_fld=src_data d_fld=src_data");
runcurl("gq mk_idx d_fld=src_data");
runcurl("gq load nR=" . int(50e6) . " fldtype=I4 filename=" . $dst_link . " tbl=T1 h_fld=dst_link d_fld=dst_link");
runcurl("gq add_fld nR=" . int(50e6) . " fldtype=I4 filename=dummy tbl=T1 h_fld=dst_data d_fld=dst_data");
runcurl("gq add_fld nR=" . int(50e6) . " fldtype=I4 filename=dummy tbl=T1 h_fld=dst_nn d_fld=dst_nn");

runcurl("gq join d_ls=src_link d_vs=src_data d_ld=dst_link d_vd=dst_data d_vd_nn=dst_nn op=min");
runcurl("gq print_reg_data d_fld=dst_data n=10 mode=head");
runcurl("gq print_reg_data d_fld=dst_data n=10 mode=tail");

runcurl("gq join d_ls=src_link d_vs=src_data d_ld=dst_link d_vd=dst_data d_vd_nn=dst_nn op=max");
runcurl("gq print_reg_data d_fld=dst_data n=10 mode=head");
runcurl("gq print_reg_data d_fld=dst_data n=10 mode=tail");

runcurl("gq join d_ls=src_link d_vs=src_data d_ld=dst_link d_vd=dst_data d_vd_nn=dst_nn op=sum");
runcurl("gq print_reg_data d_fld=dst_data n=10 mode=head");
runcurl("gq print_reg_data d_fld=dst_data n=10 mode=tail");

runcurl("gq join d_ls=src_link d_vs=src_data d_ld=dst_link d_vd=dst_data d_vd_nn=dst_nn op=cnt");
runcurl("gq print_reg_data d_fld=dst_data n=10 mode=head");
runcurl("gq print_reg_data d_fld=dst_data n=10 mode=tail");

runcurl("gq del_fld d_fld=src_link");
runcurl("gq del_fld d_fld=src_data");
runcurl("gq del_fld d_fld=dst_link");
runcurl("gq del_fld d_fld=dst_data");
runcurl("gq del_fld d_fld=dst_nn");
