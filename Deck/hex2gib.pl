#!/usr/bin/perl

# hex2gib.pl
# Deal format transform: HEX -> GIB (PBN)

# Hex Encoding
#
# Hex strings are 26 characters, representing 13 bytes, low nybble first.
# Each byte is a rank, with 2-bit chunks for SHDC, in normal order.
# Chunk values are seats 0-3, for SWNE, in clockwise order.
# Thus each 2-bit chunk is a card by position and its location by value.
# See gib2hex.pl for inverse transform.

use strict;

# Lookups

my %Decode = (
    '0' => [2, 2], '1' => [3, 2], '2' => [0, 2], '3' => [1, 2],
    '4' => [2, 3], '5' => [3, 3], '6' => [0, 3], '7' => [1, 3],
    '8' => [2, 0], '9' => [3, 0], 'A' => [0, 0], 'B' => [1, 0],
    'C' => [2, 1], 'D' => [3, 1], 'E' => [0, 1], 'F' => [1, 1],
);

my @Cards = split //, "AKQJT98765432";

# Helpers

sub to_gib
{
    # Predefine all slots, else voids are undef, when we want them empty.
    # Array levels: First, seats (NESW); Second, suits (SHDC); Third, cards.
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

    my @pbn = map { join '.', map { join '', @$_ } @$_ } @$Layout; 
    return wantarray ? @pbn : \@pbn;
}

sub convert
{
    my @deal = to_gib shift;
    print 'N:', join ' ', @deal; # North first
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
