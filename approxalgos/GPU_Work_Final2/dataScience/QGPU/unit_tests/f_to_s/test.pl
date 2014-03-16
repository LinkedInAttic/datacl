#!/usr/bin/perl -w

BEGIN { push @INC, (".", ".."); }

use strict;
use warnings;
# use diagnostics;
# use Carp;

# use QTestingUtils qw(:DEFAULT);
use QTestingUtils qw(runcurl load_reg filesizeIxNr);

runcurl("gq add_fld nR=10000 fldtype=I4 filename=dummy tbl=T1 h_fld=f1 d_fld=f1");
runcurl("gq mk_idx d_fld=f1");
runcurl("gq f_to_s d_fld1=f1 op=sum");

runcurl("gq add_fld nR=10000 fldtype=I4 filename=dummy tbl=T1 h_fld=f2 d_fld=f2");
runcurl("gq f1s1opf2 d_fld1=f1 scalar=-1 op=* d_fld2=f2");
runcurl("gq f_to_s d_fld1=f2 op=sum");

runcurl("gq del_fld d_fld=f1");
runcurl("gq del_fld d_fld=f2");
