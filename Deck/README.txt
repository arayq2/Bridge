
Deck formats.


0.  Overview.

We use two formats for the representation of a deck of cards. One is for internal tracking of the individual plays. The other is for an external representation of the full deal.


1.  Internal format.

We use a 52-array of integers. The slots are enumerated 0..51, and are allotted in groups of 13 to the 4 suits in rank order (SHDC). Within each group, the 13 slots are associated with the cards in rank order (A..2).  Thus, slot 0 represents the SA, slot 51 the C2, and for example, slot 20 represents the H7.

    card = 14 - (slot mod 13),  suit = slot div 13

The integer values held in the slots are the seat positions of the cards

    0 = Not in play, 1 = West, 2 = North, 3 = East, 4 = South.
    
"Not in play" typically means "already played".  This enables the encoding of any particular layout situation during the course of a deal.

We associate two more integer arrays to track the actual sequence of plays. 

    a.  a 13-array of the opening seat for each trick.
    b.  a 13-array of 4-arrays of the cards played in order in each trick.

These two arrays can be combined into a consolidated 13-array of 5-arrays, with the first slot of each 5-array encoding the seat of the leader to the trick.

(strictly speaking, the leader array can be reconstructed from the trick array and the initial full layout.)

Note that a 0 in the trick array can't be the SA unless slot 0 in the layout array is 0 (i.e., "already played".) 


2.  External format.

This is for the full deal only. It is a 26-character string, consisting of a 13-byte array in hexadecimal format. The 13 bytes represent the cards in rank order (A..2). Within each byte, the four suits are allotted two bits each in rank order (SHDC). The two bits encode the seat of the corresponding card, in the usual clockwise order starting with 0 for South.

    0 = South, 1 = West, 2 = North, 3 = East

This string can also function as a unique index, as it is in fact a full information representation.

(Since the number of possible deals is a tad under 2^96, we would need at least 12 bytes for any full information representation. In that light, an easily processable 13 byte representation is quite compact.)

[Note that this format can be extended to include a trailing (or leading) dealer-and-vulnerability indicator: a hyphen separator and a hex digit encoding the 16 possible combinations (with 0 corresponding to the 16th in standard order, dealer West and E/W vulnerable).]

3. Layout for visibility.

A third format is suitable for display purposes, and for translations to other external formats.  The essential data structure is a 4-array (seats) of 4-arrays (suits) of 14-arrays of char (cards). The seats and suits are in canonical order (WNES and SHDC respectively.) There are thus 16 holdings as strings. See Deal.h.

