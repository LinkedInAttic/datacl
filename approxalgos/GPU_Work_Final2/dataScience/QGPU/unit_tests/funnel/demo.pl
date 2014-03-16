#!/usr/bin/perl -w

BEGIN { push @INC, (".", ".."); }

use strict;
use warnings;
# use diagnostics;
# use Carp;

my $datapath = "/home/ssachdev/approxalgos/GPU_Work_Final2/dataScience/QGPU/unit_tests/data/FUNNELS";
my $session = "$datapath/same_as_prev.I1.bin";
my $pageid = "$datapath/pid.I2.bin";
my $pagedb = "$datapath/funnels.csv";

my $nR = -s $session;

my $topcount = 10;

my $numpages = 0;

my %pagename;

my @steps = ();

sub urlencode {
    my ($cmd) = @_;
    my @result;
    foreach (split(//, $cmd)) {
        if ($_ =~ m/[0-9a-zA-Z_=-]/) {
            push @result, $_;
        } elsif ($_ eq " ") {
            push @result, "+";
        } else {
            push @result, sprintf "%%%02x", ord($_);
        }
    }
    return join "", @result;
}

sub load_pagename {
    open(my $fh, "<", $pagedb)
        or die "cannot open < $pagedb : $!";
    while (<$fh>) {
        my @fields = split /,/;
        $pagename{$fields[0]} = $fields[1];
        ++$numpages;
    }
    close($fh)
        or warn "failed to close $pagedb : $!";
}

sub runcurl {
    my ($instr) = @_;
    my $cmd = urlencode($instr);
    my $syscall = "curl -s --url localhost:8080/gq?COMMAND=" . $cmd;
    my $result = qx/$syscall/;
    chomp $result;
    if ($result =~ m/^\{ \"msg\" : \"(.*)\" \}/) {
        $result = $1;
    }
    return $result;
}

sub init_registers {
    runcurl("gq del_fld d_fld=ALL_FLDS");
    runcurl("gq list_free_mem");

    runcurl("gq load fldtype=I1 filename=$session tbl=T1 h_fld=session d_fld=session");
    runcurl("gq list_free_mem");

    runcurl("gq load fldtype=I2 filename=$pageid tbl=T1 h_fld=pageid d_fld=pageid");
    runcurl("gq list_free_mem");

    runcurl("gq add_fld nR=$nR fldtype=I1 filename=dummy tbl=T1 h_fld=selection d_fld=selection");
    runcurl("gq list_free_mem");

    runcurl("gq add_fld nR=$nR fldtype=I1 filename=dummy tbl=T1 h_fld=next_selection d_fld=next_selection");
    runcurl("gq list_free_mem");

    runcurl("gq add_fld nR=$numpages fldtype=I4 filename=dummy tbl=T1 h_fld=histogram d_fld=histogram");
    runcurl("gq list_free_mem");

    runcurl("gq add_fld nR=$numpages fldtype=I4 filename=dummy tbl=T1 h_fld=idx d_fld=idx");
    runcurl("gq list_free_mem");
}

sub histogram_pareto {
    runcurl("gq mk_idx d_fld=idx");
    runcurl("gq sortf1f2 d_fld1=histogram d_fld2=idx srttype=D_");
    my $xs = runcurl("gq print_reg_data d_fld=idx n=$topcount mode=head");
    my $freqs = runcurl("gq print_reg_data d_fld=histogram n=$topcount mode=head");
    my @x = split ' ', $xs;
    my @freq = split ' ', $freqs;
    if ($freq[0] == 0) {
        print "no next pgid\n";
    } else {
        for (my $i = 0; ($i < $topcount) && ($freq[$i] > 0); ++$i) {
            print "pgid: $x[$i] , freq: $freq[$i] , pg:$pagename{$x[$i]}\n";
        }
    }
}

sub start_funnel {
    my ($page) = @_;

    if ($page) {
        print ">>>  start from page $page\n";
        runcurl("gq f1s1opf2 d_fld1=pageid scalar=$page op=== d_fld2=selection");
        runcurl("gq funnel_count d_key=pageid d_distance=1 d_same_session=session d_curr_funnel=selection d_result=histogram");
    } else {
        print ">>> start from session-start-page\n";
        runcurl("gq f1s1opf2 d_fld1=session scalar=0 op=== d_fld2=selection");
        runcurl("gq count d_fld1=pageid d_cfld=selection d_fld2=histogram");
    }
    my $cnt = runcurl("gq f_to_s d_fld=selection op=sum");
    print "cnt(" . ($page || "session_start-page") . ") = " . "$cnt\n";

    histogram_pareto();
}

sub next_page {
    my ($page) = @_;

    runcurl("gq funnel d_key=pageid d_value=$page d_same_session=session d_prev_funnel=selection d_result=next_selection");
    my $cnt = runcurl("gq f_to_s d_fld=next_selection op=sum");
    print "cnt($page) = $cnt\n";
    runcurl("gq funnel_count d_key=pageid d_distance=1 d_same_session=session d_curr_funnel=next_selection d_result=histogram");
    histogram_pareto();
    runcurl("gq swap_flds d_fld1=selection d_fld2=next_selection");
}

load_pagename();
init_registers();

my $usage = "help | ? | reset | lookup [pgid] | start [pgid] | follow [pgid] | gq [args] | topcount [number] | path | retrace [path] | quit | exit";
my $prompt = ">>> ";
print "USAGE: $usage\n$prompt";

while (<STDIN>) {
    chomp;
    my @fields = split / /, $_;
    if (!defined $fields[0]) {
        next;
    }
    if ($fields[0] =~ m/^[q|Q|e|E]/) {
        last;
    } elsif ($fields[0] =~ m/^[h|H]|\?/) {
        print "USAGE: $usage\n";
    } elsif ($fields[0] eq "reset") {
        @steps = ();
        init_registers();
        print ">> reset and ready to start\n";
    } elsif ($fields[0] eq "lookup") {
        if (!defined $fields[1]) {
            print ">>> USAGE: lookup [page-id]\n";
        } else {
            print "> page id $fields[1] --> $pagename{$fields[1]}\n";
        }
    } elsif ($fields[0] eq "start") {
        @steps = ();
        push @steps, $fields[1] || "session-start";
        print ">> start " . ($fields[1] || "") . "\n";
        start_funnel($fields[1] || "");
    } elsif (($fields[0] eq "follow") ||
             ($fields[0] eq "next") ||
             ($fields[0] =~ m/^[f|F|n|N]/)) {
        if (!defined $fields[1]) {
            print ">>> USAGE: follow [page-id]\n";
        } else {
            push @steps, $fields[1];
            print ">> follow $fields[1]\n";
            next_page($fields[1]);
        }
    } elsif ($fields[0] eq "gq") {
        print ">> $_\n";
        print "> " . runcurl($_) . "\n";
    } elsif ($fields[0] eq "topcount") {
        if (!defined $fields[1]) {
            print ">>> USAGE: topcount [number-for-pareto]\n";
        } else {
            $topcount = $fields[1];
            print "> topcount = $topcount\n";
        }
    } elsif ($fields[0] eq "path") {
        my $steps = join ',', @steps;
        print "> path = " . $steps . "\n";
    } elsif ($fields[0] eq "retrace") {
        $fields[1] = $fields[1] || join(',', @steps);
        print "> retrace $fields[1]\n";
        my @follow = split ',', $fields[1];
        my $first = shift @follow;
        @steps = ($first);
        my $startpg = $first eq "session-start" ? "" : $first;
        print "start $startpg\n";
        start_funnel($startpg);
        foreach my $next (@follow) {
            print "> follow $next\n";
            push @steps, $next;
            next_page($next);
        }
    } else {
        print ">>> $fields[0] not a valid command\n";
        print "USAGE: $usage\n";
    }
    print "$prompt";
}
