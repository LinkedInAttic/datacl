#!/usr/bin/env perl

BEGIN { push @INC, (".", ".."); }

use strict;
use warnings;
# use diagnostics;
# use Carp;

use QTestingUtils qw(runcurl load_reg filesizeIxNr);

for my $q_type ( qw(I8) ) {
    print "=========================\n";
    print "RUNNING: " . $q_type . "\n";
    for my $data_size ( qw(1e5 1e6 1e7 1e8 2e8 3e8) ) {
        print "-------------------------\n";
        print "DATA SIZE: " . $data_size . "\n";
        print "-------------------------\n";

        runcurl("gq add_fld filesz=" . filesizeIxNr($q_type, $data_size) .
                " nR=" . int($data_size) .
                " fldtype=" . $q_type .
                " filename=dummy tbl=T1 h_fld=a d_fld=a");
        runcurl("gq mk_idx d_fld=a");
        runcurl("gq fop op=sort d_fld=a order=dsc");
        runcurl("gq print_reg_data d_fld=a n=10 mode=head");
        runcurl("gq print_reg_data d_fld=a n=10 mode=tail");
        runcurl("gq fop op=sort d_fld=a order=asc");
        runcurl("gq print_reg_data d_fld=a n=10 mode=head");
        runcurl("gq print_reg_data d_fld=a n=10 mode=tail");
    }
}
