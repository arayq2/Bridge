/** ======================================================================+
 + Copyright @2020-2025 Arjun Ray
 + Released under MIT License
 + see https://mit-license.org
 +========================================================================*/

#include "Movement.h"
#include <cstdlib>
#include <iostream>

    using namespace Bridge;

    std::ostream&
    operator<<( std::ostream& os, Seating const& seating )
    {
        for ( auto const& _matchups : seating )
        {
            for ( auto const& _matchup : _matchups )
            {
                os << _matchup.tableno_ << ":[" << _matchup.nspr_ << "][" << _matchup.ewpr_ << "]\n";
            }
            os << "=====" << std::endl;
        }
        return os;
    }

    std::ostream&
    operator<<( std::ostream& os, Movement const& movement )
    {
        int     _pr{0};
        os << std::boolalpha;
        for ( auto const& _itin : movement )
        {
            os << ++_pr << ":";
            for ( auto const& _pl : _itin )
            {
                os << "[" << _pl.table_ << ":" << _pl.ew_ << "]"; 
            }
            os << std::endl;
        }
        return os;
    }

    void
    usage( char const* pgm )
    {
        std::cerr << "Usage: " << pgm << " <tables> <rounds> ['Howell' | 'Mitchell']\n"; 
    }

    int main( int ac, char* av[] )
    {
        if ( ac < 3 ) { usage( av[0] ); return 1; }

        int         _tables(::atoi(av[1]));
        int         _rounds(::atoi(av[2]));
        bool        _how(ac < 4 ? false : (av[3][0] == 'H' || av[3][0] == 'h'));

        Seating     _seating(_rounds, Matchups(_tables));
        fill_seating( _seating, _how );
        std::cout << _seating;

        Movement    _movement(2 * _tables, Itinerary(_rounds));
        fill_movement( _movement, _seating );
        std::cout << _movement;

        return 0;
    }
