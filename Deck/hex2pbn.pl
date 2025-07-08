#!/usr/bin/perl

use strict;

my @Order = qw/ Event Site Date Board West 
                North East South Dealer Vulnerable
                Deal Scoring Declarer Contract Result /;
my @Vul = qw/ None NS EW All /;
my @Dlr = qw/ N E S W /;

my %Decode = (
    '0' => [2, 2], '1' => [3, 2], '2' => [0, 2], '3' => [1, 2],
    '4' => [2, 3], '5' => [3, 3], '6' => [0, 3], '7' => [1, 3],
    '8' => [2, 0], '9' => [3, 0], 'A' => [0, 0], 'B' => [1, 0],
    'C' => [2, 1], 'D' => [3, 1], 'E' => [0, 1], 'F' => [1, 1],
);

my @Cards = split //, "AKQJT98765432";

sub to_pbn
{
    my $Layout = [[[],[],[],[]],[[],[],[],[]],[[],[],[],[]],[[],[],[],[]]];
    my $index  = 0; # ranks: 0 - Ace, 12 - Deuce
    my $suit   = 0; # suits: 0 - Spades, 3 - Clubs
    
    # Add current rank to suit slots for each pair of seats (from hex char)
    for my $pair (map { $Decode{uc $_} } split //, shift)
    {
        push @{$Layout->[$pair->[0]][$suit++]}, $Cards[$index];
        push @{$Layout->[$pair->[1]][$suit++]}, $Cards[$index];
        if ( $suit == 4 )
        {
            $suit = 0;
            ++$index;
        }
    }

    return 'N:' . join ' ', map { join '.', map { join '', @$_ } @$_ } @$Layout; 
}

sub make_pbn
{
    my ($lbd, $hex) = @_;
    my %Recap;
    #@Recap{@Order}     = ('?') x 15;
    $Recap{Board}      = $lbd + 1;
    $Recap{Dealer}     = $Dlr[$lbd % 4];
    $Recap{Vulnerable} = $Vul[($lbd + int($lbd / 4)) % 4];
    $Recap{Deal}       = to_pbn $hex;
    print for map { qq/[$_ "$Recap{$_}"]/ } @Order;
    print "";
}

local ($\) = ("\r\n");

print "% PBN 2.1";
print "% EXPORT";
print "%";

my $count = 0;

while ( <> )
{
    chomp;
    my ($num, $deal) = $_ =~ /(?:(\d+)\s+)?([0-9A-Fa-f]{26})/;
    die "Bad input" unless $deal;
    $num ||= $count++;
    make_pbn $num, $deal;
}
