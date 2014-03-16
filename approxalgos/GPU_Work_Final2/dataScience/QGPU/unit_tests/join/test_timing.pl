#!/usr/bin/env perl

BEGIN { push @INC, (".", ".."); }

use strict;
use warnings;
# use diagnostics;
# use Carp;

use QTestingUtils qw(runcurl load_reg filesizeIxNr);

my $datapath = "/home/ssachdev/approxalgos/GPU_Work_Final2/dataScience/QGPU/unit_tests/datagen2/data";

sub get_filename {
    my ($q_type, $data_size, $mode) = @_;
    my $file = $datapath . "/" . $data_size . "_" . $mode . "_" . $q_type . ".bin";
    return $file;
}

for my $q_type ( "I4" ) {
    print "=========================\n";
    print "RUNNING: " . $q_type . "\n";
    for my $data_size ( "1e5", "1e6", "1e7", "1e8", "1e9" ) {
        print "-------------------------\n";
        print "DATA SIZE: " . $data_size . "\n";
        print "-------------------------\n";

        load_reg("src_link", $q_type, $data_size, get_filename($q_type, $data_size, "sorted_buckets"));

        runcurl("gq add_fld filesz=" . filesizeIxNr("I4", $data_size) .
                " nR=" . int($data_size) .
                " fldtype=I4 filename=dummy tbl=T1 h_fld=src_data d_fld=src_data");
        runcurl("gq mk_idx d_fld=src_data");

        load_reg("dst_link", $q_type, "4e3", get_filename("I4", "4e3", "sorted_buckets"));

        runcurl("gq add_fld filesz=" . filesizeIxNr("I4", "4000") .
                " nR=" . 4000 .
                " fldtype=I4 filename=dummy tbl=T1 h_fld=dst_data d_fld=dst_data");

        runcurl("gq add_fld filesz=" . filesizeIxNr("I1", 4000) .
                " nR=" . 4000 .
                " fldtype=I4 filename=dummy tbl=T1 h_fld=dst_nn d_fld=dst_nn");

        runcurl("gq join d_ls=src_link d_vs=src_data d_ld=dst_link d_vd=dst_data d_vd_nn=dst_nn op=min");

        runcurl("gq print_reg_data d_fld=dst_data n=10 mode=head");
        runcurl("gq print_reg_data d_fld=dst_data n=10 mode=tail");
    }
}
