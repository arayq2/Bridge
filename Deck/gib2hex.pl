#!/usr/bin/perl

# gib2hex.pl
# Deal format transform: GIB (PBN) -> HEX

# Hex Encoding
#
# Hex strings are 26 characters, representing 13 bytes, low nybble first.
# Each byte is a rank, with 2-bit chunks for SHDC, in normal order.
# Chunk values are seats 0-3, for SWNE, in clockwise order.
# Thus each 2-bit chunk is a card by position and its location by value.
# See hex2gib.pl for inverse transform.

use strict;

# Lookups

my @Cards = split //, "AKQJT98765432";

my @Encode = (
    ['0', '4', '8', 'C'],
    ['1', '5', '9', 'D'],
    ['2', '6', 'A', 'E'], 
    ['3', '7', 'B', 'F']
);

my %Seats = ( 'S' => 0, 'W' => 1, 'N' => 2, 'E' => 3 );
my @Next  = (1..3,0);

# Helpers

sub to_hex
{
    my ($seat, $deal) = @_;
    my %Ranks = (); # {rank} => [S,H,D,C] (4-array of seats)
    my $suit  = 0;

    for my $card (split //, $deal)
    {
        if ( $card eq '.' ) #Next suit
        {
            ++$suit;
            next;
        }
        if ( $card eq ' ' ) # Next seat
        {
            $seat = $Next[$seat];
            $suit = 0;
            next;
        }
        $Ranks{uc $card}->[$suit] = $seat;
    }
    # translate, in order, each rank of four seats into a pair of hex chars
    return join '', map { $Encode[$_->[0]][$_->[1]], $Encode[$_->[2]][$_->[3]] } @Ranks{@Cards};
}

sub convert
{
    my $deal = shift;
    my ($dealer, $cards) = $deal =~ /(\w:)?(.*)/;
    my $seat = $Seats{uc substr( $dealer, 0, 1 )} || 2; # North first by default
    print to_hex $seat, $cards;
}

# Main

local ($\) = ("\n");

if ( @ARGV > 0 )
{
    convert $_ for @ARGV
}
else
{
    while ( <> )
    {
        chomp;
        convert $_;
    }
}
