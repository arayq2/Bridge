
Scoring routines.

1. Supported modes: Matchpoints and Cross-Imps.

The input is a vector of pointers to a templated type. This allows assembly from disparate sources, and external sorting.  Typically this vector is assembled on the fly on the basis of the underlying data structure carrying the bridge scores and space for match scores.

The second template parameter is a struct defining two methods:
(1) A comparison function between two pointers.
(2) An assignment function for the computed score.


2. Score utility.

Use the syntax  [1..7][NSHDC](X|R)?v?=|+n|-n

letters are not case sensitive.

For example,

    2nrv+2  should yield 1680


 