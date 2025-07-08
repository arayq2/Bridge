/** ======================================================================+
 + Copyright @2020-2025 Arjun Ray
 + Released under MIT License
 + see https://mit-license.org
 +========================================================================*/

#include "Deck.h"
#include "Deal.h"
#include "CharBuffer.h"

#include <string>
#include <iostream>

void show_hcp( std::ostream& os, char const* hex )
{
    int _hcp[4]{0};
    Deck::get_hcp( _hcp, hex );
    os << "West: "  << _hcp[1]
       << " North: " << _hcp[2]
       << " East: "  << _hcp[3]
       << " South: " << _hcp[0]
       << " NS: " << (_hcp[0] + _hcp[2])
       << " EW: " << (_hcp[1] + _hcp[3])
       << std::endl;
}

void process_hex( std::ostream& os, char const* hex )
{
    if ( !Deck::verify_hex( hex ) )
    {
        std::cerr << hex << " is not a valid deal string\n";
        return;
    }
    Deck            _deck(hex);
    nlohmann::json  _cards;
    _deck.to_js( _cards, "cards" );
    os << _cards.dump() << "\n" << hex << std::endl;
    
    Deal(_deck.begin()).visualize( os );
    show_hcp( os, hex );
    int const            _dlr{2};
    int const            _vul{1};
    os << Buffer128(&Deck::store_record, _deck, _dlr, _vul).get() << "\n";
    os << Buffer64(&Deck::store_bhg, _deck, 2).get() << std::endl;

    if ( Buffer32{&Deck::store_hex, _deck}.ncmp( hex, 26 ) )
    {
        std::cerr << "Round trip failed!\n";
    }
}

int main( int ac, char const* av[] )
{
    if ( ac < 2 )
    {
        std::cerr << "Reading from STDIN\n";
        std::string     _line;
        while ( std::getline( std::cin, _line ) )
        {
            process_hex( std::cout, _line.c_str() );
        }
    }
    else
    for ( int _ndx{1}; _ndx < ac; ++_ndx )
    {
        process_hex( std::cout, av[_ndx] );
    }

    return 0;
}
