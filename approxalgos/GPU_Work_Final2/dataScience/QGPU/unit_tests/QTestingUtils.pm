package QTestingUtils;

use strict;
use warnings;
# use diagnostics;
# use Carp;
use Exporter;

our $VERSION     = 1.00;
our @ISA         = qw(Exporter);
our @EXPORT      = ();
our @EXPORT_OK   = qw(urlencode runcurl load_reg filesizeIxNr);
our %EXPORT_TAGS = ( DEFAULT => [qw(&urlencode &runcurl &load_reg &filesizeIxNr)],
                     ALL     => [qw(&urlencode &runcurl &load_reg &filesizeIxNr)]);

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

sub runcurl {
    my ($instr) = @_;
    my $debug = 0;
    if ($debug) {
        my $cmd = $instr;
        print ("curl --url localhost:8080/gq?COMMAND=" . $cmd . "\n");
    } else {
        # my $cmd = `qtils urlencode "$instr"`;
        my $cmd = urlencode($instr);
        # print ($instr . "\n");
        # print ("curl --url localhost:8080/gq?COMMAND=" . $cmd . "\n");
        if (1) {
            system ("curl --url localhost:8080/gq?COMMAND=" . $cmd . "\n") == 0
                or die "FAILURE on [$instr], stopped at";
        } else {
            my $syscall = "curl --url localhost:8080/gq?COMMAND=" . $cmd . "\n";
            return qx/$syscall/;
        }
    }
}

sub filesizeIxNr {
    my ($Ix, $Nr) = @_;
    $Ix =~ s/^I//;
    return int($Ix) * int($Nr);
}

sub load_reg {
    my ($regname, $q_type, $data_size, $file) = @_;
    my $n_size = int($data_size);
    my $sizeof = $q_type;
    $sizeof =~ s/I//;
    $sizeof = int($sizeof);
    my $instr = "gq load filesz=" . ($n_size * $sizeof) .
        " nR=" . $n_size .
        " fldtype=" . $q_type .
        " filename=" . $file .
        " tbl=T1 h_fld=" . $regname . " d_fld=" . $regname;
    runcurl($instr);
}

1;

__END__
