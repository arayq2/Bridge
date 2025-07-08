/** ======================================================================+
 + Copyright @2020-2025 Arjun Ray
 + Released under MIT License
 + see https://mit-license.org
 +========================================================================*/

#include "Deck.h"
#include "Deal.h"
#include "CharBuffer.h"

#include <ctype.h>
#include <string>
#include <iostream>

void show_hcp( std::ostream& os, char const* hex )
{
    int _hcp[4]{0};
    Deck::get_hcp( _hcp, hex );
    os << "West: "   << _hcp[1]
       << " North: " << _hcp[2]
       << " East: "  << _hcp[3]
       << " South: " << _hcp[0]
       << " NS: " << (_hcp[0] + _hcp[2])
       << " EW: " << (_hcp[1] + _hcp[3])
       << std::endl;
}

void show_deck( std::ostream& os, Deck const& deck )
{
    nlohmann::json  _cards;
    _cards["cards"] = deck;
    os << _cards.dump() << std::endl;
    //
    Deal(deck.begin()).visualize( os );
    //
    Buffer32        _hex{&Deck::store_hex, deck};
    os << _hex.get() << std::endl;
    show_hcp( os, _hex.get() );
    os << Buffer64{&Deck::store_bhg, deck, 2}.get() << std::endl;
}

bool is_hex( char const* str )
{
    char const* _end(str + 26);
    do {
        if ( !::isxdigit( *str ) ) { return false; }
    } while ( ++str < _end );
    return true;
}

void process_arg( std::ostream& os, char const* arg )
{
    Deck            _deck;
    if ( is_hex( arg ) )
    {
        if ( !Deck::verify_hex( arg ) )
        {
            std::cerr << "Not a valid HEX deal-string.\n";
            return;
        }
        if ( !_deck.load_hex( arg ) )
        {
            std::cerr << "Error loading HEX deal-string.\n";
            return;
        }
    }
    else
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
    if ( ::isalpha( *arg ) )
    {
        if ( !_deck.load_bhg( arg ) )
        {
            std::cerr << "Error loading BHG deal-string.\n";
            return;
        }
    }
    else
    {
        std::cerr << "Unknown format: " << arg << "\n";
        return;
    }
    //
    os << "[" << arg << "]" << std::endl;
    show_deck( os, _deck );
}

int main( int ac, char const* av[] )
{
    if ( ac < 2 )
    {
        std::cerr << "Reading from STDIN\n";
        std::string     _line;
        while ( std::getline( std::cin, _line ) )
        {
            process_arg( std::cout, _line.c_str() );
        }
    }
    else
    for ( int _ndx{1}; _ndx < ac; ++_ndx )
    {
        process_arg( std::cout, av[_ndx] );
    }

    return 0;
}
