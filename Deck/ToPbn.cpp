/** ======================================================================+
 + Copyright @2020-2025 Arjun Ray
 + Released under MIT License
 + see https://mit-license.org
 +========================================================================*/

#include "Deck.h"
#include "CharBuffer.h"

#include <ctype.h>
#include <string>
#include <iostream>

void process_arg( char const* arg, int dlr, std::ostream& os )
{
    //!> read into Deck representation
    Deck            _deck;
    if ( !_deck.load_hex( arg ) )
    {
        std::cerr << "Error loading HEX string\n";
        return;
    }
    //!> write to PBN representation
    os << Utility::CharBuffer<80>(&Deck::store_pbn_s, _deck, dlr).get() << std::endl;
}

int main( int ac, char const* av[] )
{
    int     _dlr{2}; // north
    if ( ac < 2 )
    {
        std::cerr << "Reading from STDIN\n";
        std::string     _line;
        while ( std::getline( std::cin, _line ) )
        {
            process_arg( _line.c_str(), _dlr++, std::cout );
        }
    }
    else
    for ( int _ndx{1}; _ndx < ac; ++_ndx )
    {
        process_arg( av[_ndx], _dlr++, std::cout );
    }

    return 0;
}
