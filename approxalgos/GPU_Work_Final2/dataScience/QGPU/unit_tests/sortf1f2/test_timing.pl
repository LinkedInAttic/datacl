#!/usr/bin/env perl

BEGIN { push @INC, (".", ".."); }

use strict;
use warnings;
# use diagnostics;
# use Carp;

use QTestingUtils qw(runcurl load_reg filesizeIxNr);

for my $q_type ( "I4" ) {
    print "=========================\n";
    print "RUNNING: " . $q_type . "\n";
    for my $data_size ( "1e5", "1e6", "1e7", "1e8", "2e8", "1e9" ) {
        print "-------------------------\n";
        print "DATA SIZE: " . $data_size . "\n";
        print "-------------------------\n";

        runcurl("gq add_fld filesz=" . filesizeIxNr($q_type, $data_size) .
                " nR=" . int($data_size) .
                " fldtype=" . $q_type .
                " filename=dummy tbl=T1 h_fld=a d_fld=a");
        runcurl("gq add_fld filesz=" . filesizeIxNr($q_type, $data_size) .
                " nR=" . int($data_size) .
                " fldtype=" . $q_type .
                " filename=dummy tbl=T1 h_fld=b d_fld=b");

        runcurl("gq mk_idx d_fld=a");
        runcurl("gq mk_idx d_fld=b");
        runcurl("gq print_reg_data d_fld=a n=10 mode=head");
        runcurl("gq print_reg_data d_fld=a n=10 mode=tail");

        runcurl("gq sortf1f2 d_fld1=a d_fld2=b srttype=D_");
        runcurl("gq print_reg_data d_fld=a n=10 mode=head");
        runcurl("gq print_reg_data d_fld=a n=10 mode=tail");
        runcurl("gq print_reg_data d_fld=b n=10 mode=head");
        runcurl("gq print_reg_data d_fld=b n=10 mode=tail");

        if (0) {
            # this is just to do manual correctness  check
            # do not use this for timing since it uses too much extra memory
            runcurl("gq add_fld filesz=" . filesizeIxNr($q_type, $data_size) .
                    " nR=" . int($data_size) .
                    " fldtype=" . $q_type .
                    " filename=dummy tbl=T1 h_fld=c d_fld=c");
            runcurl("gq mk_idx d_fld=c");
            runcurl("gq sortf1f2 d_fld1=b d_fld2=c srttype=A_");
            runcurl("gq print_reg_data d_fld=b n=10 mode=head");
            runcurl("gq print_reg_data d_fld=b n=10 mode=tail");
            runcurl("gq print_reg_data d_fld=c n=10 mode=head");
            runcurl("gq print_reg_data d_fld=c n=10 mode=tail");
        }
    }
}
