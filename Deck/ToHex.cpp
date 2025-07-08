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

void process_arg( char const* arg, std::ostream& os )
{
    //!> read into Deck representation
    Deck            _deck;
    if ( ::isdigit( *arg ) )
    {
        if ( !_deck.load_lin_s( arg ) )
        {
            std::cerr << "Error loading LIN deal-string.\n";
            return;
        }
    }
    else
    if ( arg[1] == ':' )
    {
        if ( !_deck.load_pbn_s( arg ) )
        {
            std::cerr << "Error loading PBN deal-string.\n";
            return;
        }
    }
    else
    {
        std::cerr << "Unknown format: [" << arg << "]\n";
        return;
    }
    //!> write to hex string representation
    os << Buffer32(&Deck::store_hex, _deck).get() << std::endl;
}

int main( int ac, char const* av[] )
{
    if ( ac < 2 )
    {
        std::cerr << "Reading from STDIN\n";
        std::string     _line;
        while ( std::getline( std::cin, _line ) )
        {
            process_arg( _line.c_str(), std::cout );
        }
    }
    else
    for ( int _ndx{1}; _ndx < ac; ++_ndx )
    {
        process_arg( av[_ndx], std::cout );
    }

    return 0;
}
