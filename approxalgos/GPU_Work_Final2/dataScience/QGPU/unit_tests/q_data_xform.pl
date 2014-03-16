#!/usr/bin/env perl

use strict;
use warnings;

my ($mode, $qtype, $gentype, $count) = @ARGV;

my %pack_code = ("I1" => 'c', "I2" => 's', "I4" => 'i', "I8" => 'q');

my %pack_size = ("I1" => 1, "I2" => 2, "I4" => 4, "I8" => 8);

my %make_op = ("allones" => sub { return 1; },
               "all10s" => sub { return 10; },
               "evenones" => sub { return ($_[0] + 1) % 2; },
               "incr_mod100" => sub { return $_[0] % 100; },
               "sorted_buckets_2000" => sub {
                   return 2 * int(($_[0]/$_[1]) * 2000);
               },
               "sorted_buckets_25M" => sub {
                   return 2 * int(($_[0]/$_[1]) * 25000000);
               },
               "backwards" => sub { return $_[1] - ($_[0] + 1); },
               "forwards" => sub { return $_[0]; },
               "backwards2" => sub { return 2*($_[1] - ($_[0] + 1)); },
               "mult2" => sub { return 2*($_[0] + 1); },
               "mult3" => sub { return 3*($_[0] + 1); },
               "pos_neg" => sub { my $half = $_[1]/2; return (($_[0] < $half) ? $_[0] : ($half - $_[0])); },
               "pos_neg_alt" => sub { return (($_[0] % 2) ? -$_[0] : $_[0]); }
    );

my %mode = ("convert2asc" =>
            sub {
                my $size = $pack_size{$qtype};
                my $code = $pack_code{$qtype};
                while (read(STDIN, my $value, $size)) {
                    my $number = unpack($code, $value);
                    print "$number\n";
                }
            },
            "convert2bin" =>
            sub {
                my $size = $pack_size{$qtype};
                my $code = $pack_code{$qtype};
                while (<STDIN>) {
                    chomp;
                    s/,|;/ /g;
                    foreach (split) {
                        print pack($code, $_);
                    }
                }
            },
            "make" =>
            sub {
                $count or die "need positive count value, stopped at";
                my $code = $pack_code{$qtype};
                ($code) or die "unknown data type $qtype, stopped at";
                my $operator = $make_op{$gentype};
                ($operator) or die "do not know how to make $gentype, stopped at";
                for (my $i = 0; $i < $count; ++$i) {
                    print pack($code, $operator->($i, $count));
                }
            });

my $usage_mode = join "|", sort keys %mode;
my $usage_types = join "|", sort keys %pack_code;
my $usage_ops = join "|", sort keys %make_op;

my $usage = "usage: $0 <$usage_mode> <$usage_types> [$usage_ops] [count]";

$qtype or die $usage;
my $action = $mode{$mode};
($action) or die "do not know how to $mode : $usage, stopped at";

eval {
    &$action;
};

if ($@) {
    my $swansong = "$@$usage";
    die $swansong;
}
