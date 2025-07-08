
#include "Deal.h"
#include "CharBuffer.h"

#include <ostream>
#include <ctype.h>


    namespace
    {
        char const* Suits  = "SHDC";
        char const* format = " %c |%-14s|%-14s|%-14s|%-14s\n";
    }

    void
    Deal::visualize( std::ostream& os ) const
    {
        Utility::CharBuffer<128>    _buffer;
        os << _buffer.format( format, ' ', "WEST", "NORTH", "EAST", "SOUTH" ).get();
        for ( int _suit{0}; _suit < 4; ++_suit )
        {
            os << _buffer.format( format, Suits[_suit], 
                                  deal_[0][_suit].get( "-" ), deal_[1][_suit].get( "-" ),
                                  deal_[2][_suit].get( "-" ), deal_[3][_suit].get( "-" )).get();
        }
        os.flush();
    }

    namespace
    {
        using Pair    = std::pair<int, int>;
        using Decoder = std::array<Pair, 16>;
        
        // Hex string has seats 0..3 <=> S..E
        // Deal has seats 0..3 <=> W..S (ROL 1) 
        // Thus Hex 0..3 <=> Deal 3..2
        Decoder decoder = 
        {{
            {3, 3}, {0, 3}, {1, 3}, {2, 3},
            {3, 0}, {0, 0}, {1, 0}, {2, 0},
            {3, 1}, {0, 1}, {1, 1}, {2, 1},
            {3, 2}, {0, 2}, {1, 2}, {2, 2}
        }};

        int to_hexit( char const letter )
        {
            return letter > '/' && letter < ':'
                 ? letter - '0'
                 : letter > '@' && letter < 'G'
                 ? letter - '7'
                 : letter > '`' && letter < 'g'
                 ? letter - 'W'
                 : -1;
        }

        char Cards[] = "AKQJT98765432";
    }

    Deal&
    Deal::layout( char const* hex )
    {
        char const* _end = hex + 26;
        int         _index{0};
        int         _suit{0};
        while ( hex < _end )
        {
            auto    _pr = decoder[to_hexit( *hex++ )];
            deal_[_pr.first][_suit++].add( Cards[_index] );
            deal_[_pr.second][_suit++].add( Cards[_index] );
            if ( _suit == 4 )
            {
                _suit = 0;
                ++_index;
            }
        }
        return *this;
    }

    Deal&
    Deal::layout( int const* deck )
    {
        for ( int _suit{0}; _suit < 4; ++_suit )
        {
            int     _offset{_suit * 13};
            for ( int _slot{0}; _slot < 13; ++_slot )
            {
                int _seat{deck[_offset + _slot]};
                if ( _seat )
                {
                    deal_[_seat - 1][_suit].add( Cards[_slot] );
                }
            }
        }
        return *this;
    }

    Deal&
    Deal::reset()
    {
        *this = Deal();
        return *this;
    }

//=========================================================================

namespace
{
    char const* pbnFmt = "%s.%s.%s.%s";
    char const* linFmt = "S%sH%sD%sC%s";
    char const* hrFmt2 = "%s %s %s %s %c %c";
    char const* hrDlr  = "SWNE";
    char const* hrVul  = "B-NE";
}

    char* /* static */
    Deal::store_pbn( char* buf, std::size_t len, Hand const& hand )
    {
        return buf + ::snprintf( buf, len, pbnFmt, hand[0].get(), hand[1].get(), hand[2].get(), hand[3].get() );
    }

    char* /* static */
    Deal::store_lin( char* buf, std::size_t len, Hand const& hand )
    {
        return buf + ::snprintf( buf, len, linFmt, hand[0].get(), hand[1].get(), hand[2].get(), hand[3].get() );
    }

    char* /* static */
    Deal::store_hrdv( char* buf, std::size_t len, char const* hex, int dlr, int vul )
    {
        Deal const  _deal(hex);
        Buffer32    _w(&Deal::store_pbn, _deal[0]);
        Buffer32    _n(&Deal::store_pbn, _deal[1]);
        Buffer32    _e(&Deal::store_pbn, _deal[2]);
        Buffer32    _s(&Deal::store_pbn, _deal[3]);
        return buf + ::snprintf( buf, len, hrFmt2, _n.get(), _e.get(), _s.get(), _w.get(), hrDlr[dlr % 4], hrVul[vul % 4] );
    }
