#!/usr/bin/perl -w

BEGIN { push @INC, (".", ".."); }

use strict;
use warnings;
# use diagnostics;
# use Carp;

# use QTestingUtils qw(:DEFAULT);
use QTestingUtils qw(runcurl load_reg filesizeIxNr);

my $datapath = "/home/ssachdev/approxalgos/GPU_Work_Final2/dataScience/QGPU/unit_tests/datagen2/data";
my $session = "$datapath/same_session_I1.bin";
my $pageid = "$datapath/pageid_I4.bin";

runcurl("gq load filesz=16 nR=16 fldtype=I1 filename=$session tbl=T1 h_fld=session d_fld=session");
runcurl("gq load filesz=64 nR=16 fldtype=I4 filename=$pageid tbl=T1 h_fld=pageid d_fld=pageid");
runcurl("gq add_fld filesz=16 nR=16 fldtype=I1 filename=dummy tbl=T1 h_fld=pid1 d_fld=pid1");
runcurl("gq add_fld filesz=16 nR=16 fldtype=I1 filename=dummy tbl=T1 h_fld=pid2 d_fld=pid2");
runcurl("gq f1s1opf2 d_fld1=pageid scalar=1 op=== d_fld2=pid1");
runcurl("gq funnel d_key=pageid d_value=2 d_same_session=session d_prev_funnel=pid1 d_result=pid2");

runcurl("gq add_fld filesz=40 nR=10 fldtype=I4 filename=dummy tbl=T1 h_fld=s1 d_fld=nextfldcnt");
runcurl("gq funnel_count d_key=pageid d_distance=2 d_same_session=session d_curr_funnel=pid1 d_result=nextfldcnt");
runcurl("gq add_fld filesz=40 nR=10 fldtype=I4 filename=dummy tbl=T1 h_fld=idx d_fld=idx");
runcurl("gq mk_idx d_fld=idx");
runcurl("gq sortf1f2 d_fld1=nextfldcnt d_fld2=idx srttype=D_");

runcurl("gq print_reg_data d_fld=session n=16 mode=head");
runcurl("gq print_reg_data d_fld=pageid n=16 mode=head");
runcurl("gq print_reg_data d_fld=pid1 n=16 mode=head");
runcurl("gq print_reg_data d_fld=nextfldcnt n=10 mode=head");
runcurl("gq print_reg_data d_fld=idx n=10 mode=head");
