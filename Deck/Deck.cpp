
#include "Deck.h"
#include "Deal.h"
#include "CharBuffer.h"

#include <unordered_map>
#include <algorithm>

// seat [ENSWensw]
// suit [CDHScdhs]
// card [AJKQTajkqt2-9]

namespace
{
    enum {
        WEST = 1, NORTH, EAST, SOUTH
    };

    constexpr std::array<int, 5>  clockwise = { 0, 2, 3, 4, 1 };

    using CharMap = std::unordered_map<char, int>;

    CharMap seatMap =
    {
        {'S', 4}, {'s', 4}, {'W', 1}, {'w', 1},
        {'N', 2}, {'n', 2}, {'E', 3}, {'e', 3}
    };

    CharMap suitMap =
    {
        {'S', 0}, {'s', 0}, {'H', 1}, {'h', 1},
        {'D', 2}, {'d', 2}, {'C', 3}, {'c', 3}
    };

    CharMap cardMap =
    {
        {'A', 0}, {'a', 0}, {'K', 1}, {'k', 1}, {'Q', 2},
        {'q', 2}, {'J', 3}, {'j', 3}, {'T', 4}, {'t', 4},
        {'9', 5}, {'8', 6}, {'7', 7}, {'6', 8}, {'5', 9},
        {'4', 10}, {'3', 11}, {'2', 12}
    };

    constexpr char const hexTable[] = "0123456789ABCDEF";

    //!> Map hex digit to a pair of 2-bits of seats
    std::unordered_map<char, std::array<int, 2>> hexDecoder =
    {
        { '0', { 4, 4 } },
        { '1', { 1, 4 } },
        { '2', { 2, 4 } },
        { '3', { 3, 4 } },
        { '4', { 4, 1 } },
        { '5', { 1, 1 } },
        { '6', { 2, 1 } },
        { '7', { 3, 1 } },
        { '8', { 4, 2 } },
        { '9', { 1, 2 } },
        { 'A', { 2, 2 } },
        { 'B', { 3, 2 } },
        { 'C', { 4, 3 } },
        { 'D', { 1, 3 } },
        { 'E', { 2, 3 } },
        { 'F', { 3, 3 } },
        { 'a', { 2, 2 } },
        { 'b', { 3, 2 } },
        { 'c', { 4, 3 } },
        { 'd', { 1, 3 } },
        { 'e', { 2, 3 } },
        { 'f', { 3, 3 } },
    };

    std::unordered_map<char, int> alphaDecoder =
    {
        { 'A',  0 }, { 'B',  1 }, { 'C',  2 }, { 'D',  3 },
        { 'E',  4 }, { 'F',  5 }, { 'G',  6 }, { 'H',  7 },
        { 'I',  8 }, { 'J',  9 }, { 'K', 10 }, { 'L', 11 },
        { 'M', 12 }, { 'N', 13 }, { 'O', 14 }, { 'P', 15 },
        { 'Q', 16 }, { 'R', 17 }, { 'S', 18 }, { 'T', 19 },
        { 'U', 20 }, { 'V', 21 }, { 'W', 22 }, { 'X', 23 },
        { 'Y', 24 }, { 'Z', 25 }, { 'a', 26 }, { 'b', 27 },
        { 'c', 28 }, { 'd', 29 }, { 'e', 30 }, { 'f', 31 },
        { 'g', 32 }, { 'h', 33 }, { 'i', 34 }, { 'j', 35 },
        { 'k', 36 }, { 'l', 37 }, { 'm', 38 }, { 'n', 39 },
        { 'o', 40 }, { 'p', 41 }, { 'q', 42 }, { 'r', 43 },
        { 's', 44 }, { 't', 45 }, { 'u', 46 }, { 'v', 47 },
        { 'w', 48 }, { 'x', 49 }, { 'y', 50 }, { 'z', 51 },
    };

    constexpr char const alphaTable[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

}

    bool /* static */
    Deck::verify_hex( char const* hex )
    {
        std::array<int, 4>  _counts{0, 0, 0, 0};
        char const*         _end = hex + 26;

        do {
            ++_counts[hexDecoder[*hex][0] % 4];
            ++_counts[hexDecoder[*hex][1] % 4];
        } while ( ++hex < _end );

        for ( auto const& _count : _counts ) if ( _count != 13 ) { return false; }
        return true;
        //return std::find_if( _counts.begin(), _counts.end(),
        //    []( int num ) -> bool { return num != 13; } ) == _counts.end();
    }

    bool
    Deck::load_hex( char const* hex )
    {
        char const* _end = hex + 26;

        int* s = &deck_[0];
        int* h = &deck_[13];
        int* d = &deck_[26];
        int* c = &deck_[39];

        while ( hex < _end )
        {
            *s++ = hexDecoder[*hex][0];
            *h++ = hexDecoder[*hex++][1];
            *d++ = hexDecoder[*hex][0];
            *c++ = hexDecoder[*hex++][1];
        }
        return true;
    }

    char*
    Deck::store_hex( char* hex, std::size_t len ) const
    {
        if ( len > 0 && len < 27 ) { return hex; }
        char const* _end = hex + 26;

        int const* s = &deck_[0];
        int const* h = &deck_[13];
        int const* d = &deck_[26];
        int const* c = &deck_[39];

        while ( hex < _end )
        {
            *hex++ = hexTable[(*s++ % 4) | (*h++ % 4) << 2];
            *hex++ = hexTable[(*d++ % 4) | (*c++ % 4) << 2];
        }
        *hex = 0;
        return hex;
    }

    bool
    Deck::load_pbn_s( char const* pbn )
    {
        int     _seat{4}; //!< south
        int     _suit{0}; //!< spades
        int     _spaces{0};

        switch ( *pbn )
        {
        case 'E': case 'e': _seat = 3; break;
        case 'N': case 'n': _seat = 2; break;
        case 'S': case 's': _seat = 4; break;
        case 'W': case 'w': _seat = 1; break;
        default: return false;
        }
        //
        for ( pbn += 2; *pbn; ++pbn )
        switch ( *pbn )
        {
        case '.':
            _suit += 13;
            if ( _suit == 52 ) { return false; }
            break;
        case ' ':
            if ( ++_spaces == 4 ) { return false; }
            _seat = clockwise[_seat];
            _suit = 0;
            break;
        case 'A': case 'a': case 'K': case 'k': case 'Q': case 'q':
        case 'J': case 'j': case 'T': case 't': case '9': case '8':
        case '7': case '6': case '5': case '4': case '3': case '2':
            deck_[_suit + cardMap[*pbn]] = _seat;
        default:
            break;
        }
        return true;
    }

    bool
    Deck::load_lin_s( char const* lin )
    {
        int     _seat{4}; //!< south
        int     _suit{0}; //!< spades
        int     _commas{0};
        //
        ++lin; //!< skip dealer
        for ( ; *lin; ++lin )
        switch ( *lin )
        {
        case 'S': case 's': _suit =  0; break;
        case 'H': case 'h': _suit = 13; break;
        case 'D': case 'd': _suit = 26; break;
        case 'C': case 'c': _suit = 39; break;
        case ',':
            if ( ++_commas == 4 ) { return false; }
            _seat = clockwise[_seat];
            break;
        case 'A': case 'a': case 'K': case 'k': case 'Q': case 'q':
        case 'J': case 'j': case 'T': case 't': case '9': case '8':
        case '7': case '6': case '5': case '4': case '3': case '2':
            deck_[_suit + cardMap[*lin]] = _seat;
        default:
            break;
        }
         //!> fill East?
        if ( std::count( begin(), end(), 0 ) == 13
          && std::count( begin(), end(), 3 ) == 0 )
        {
            for ( auto& _slot : deck_ ) { if ( _slot == 0 ) { _slot = 3; } }
        }
        else
        if ( _commas != 3 ) { return false; }
        return true;
    }

    bool
    Deck::load_bhg( char const* bhg, int seat ) // usually North (2) first
    {
        char const* _end(bhg + 52);
        int         _pslot(0);
        
        for ( ; bhg < _end; ++bhg )
        {
            int _slot(alphaDecoder[*bhg]);
            if ( _slot < _pslot ) { seat = clockwise[seat]; }
            deck_[_slot] = seat;
            _pslot = _slot;
        }
        return true;
    }

    /* ctor */
    Deck::Deck(char const* input, Format fmt)
    {
        load_( input, fmt ); //!< need error reporting
    }

    bool
    Deck::load_( char const* input, Format fmt )
    {
        switch ( fmt )
        {
        case Format::HEX: return load_hex( input );
        case Format::LIN: return load_lin_s( input );
        case Format::PBN: return load_pbn_s( input );
        case Format::BHG: return load_bhg( input );
        default: return false;
        }
    }

    int
    Deck::count( int q ) const
    {
        int     _count{0};
        for ( auto const& _slot : deck_ )
        {
            if ( _slot == q ) { ++_count; }
        }
        return _count;
    }
//=========================================================================

namespace
{
    char const* pbnFmt = "%c:%s %s %s %s";
    char const* linFmt = "%c%s,%s,%s";
    char const* linDlr = "1234";
    char const* hrFmt  = "%s\n%s\n%s\n%s\n%c %c";
    char const* hrFmt2 = "%s %s %s %s %c %c";
    char const* hrDlr  = "SWNE";
    char const* hrVul  = "B-NE";
}
    using Buf20 = Utility::CharBuffer<20>;


    char*
    Deck::store_record( char* buf, std::size_t len, int dlr, int vul ) const
    {
        return store_record_x( buf, len, dlr, vul, false );
    }

    char*
    Deck::store_record_x( char* buf, std::size_t len, int dlr, int vul, bool oneline ) const
    {
        Deal const  _deal(deck_.begin());
        Buf20       _w(&Deal::store_pbn, _deal[0]);
        Buf20       _n(&Deal::store_pbn, _deal[1]);
        Buf20       _e(&Deal::store_pbn, _deal[2]);
        Buf20       _s(&Deal::store_pbn, _deal[3]);

        return buf + ::snprintf( buf, len, (oneline ? hrFmt2 : hrFmt), _n.get(), _e.get(), _s.get(), _w.get(), hrDlr[dlr % 4], hrVul[vul % 4] );
    }

    char*
    Deck::store_pbn_s( char* buf, std::size_t len, int dlr ) const
    {
        Deal const  _deal(deck_.begin());
        Buf20       _w(&Deal::store_pbn, _deal[0]);
        Buf20       _n(&Deal::store_pbn, _deal[1]);
        Buf20       _e(&Deal::store_pbn, _deal[2]);
        Buf20       _s(&Deal::store_pbn, _deal[3]);

        switch ( dlr % 4 )
        {
        case 0: return buf + ::snprintf( buf, len, pbnFmt, 'S', _s.get(), _w.get(), _n.get(), _e.get());
        case 1: return buf + ::snprintf( buf, len, pbnFmt, 'W', _w.get(), _n.get(), _e.get(), _s.get());
        case 2: return buf + ::snprintf( buf, len, pbnFmt, 'N', _n.get(), _e.get(), _s.get(), _w.get());
        case 3: return buf + ::snprintf( buf, len, pbnFmt, 'E', _e.get(), _s.get(), _w.get(), _n.get());
        default: return buf;
        }
    }

    char*
    Deck::store_lin_s( char* buf, std::size_t len, int dlr ) const
    {
        Deal const  _deal(deck_.begin());
        Buf20       _s(&Deal::store_lin, _deal[3]);
        Buf20       _w(&Deal::store_lin, _deal[0]);
        Buf20       _n(&Deal::store_lin, _deal[1]);

        return buf + ::snprintf( buf, len, linFmt, linDlr[dlr % 4], _s.get(), _w.get(), _n.get());
    }

    char*
    Deck::store_bhg( char* buf, std::size_t len, int seat ) const
    {
        if ( len > 0 && len < 53 ) { return buf; }
        for ( int _count(4); _count--; seat = clockwise[seat] )
        for ( int _slot(0); _slot < 52; ++_slot )
        if ( deck_[_slot] == seat ) { *buf++ = alphaTable[_slot]; }
        *buf = '\0';
        return buf;
    }

    void /* static */
    Deck::get_hcp( int* hcp, char const* hex )
    {
        for ( int _pts{4}; _pts > 0; --_pts )
        {
            hcp[hexDecoder[*hex][0] % 4]   += _pts;
            hcp[hexDecoder[*hex++][1] % 4] += _pts;
            hcp[hexDecoder[*hex][0] % 4]   += _pts;
            hcp[hexDecoder[*hex++][1] % 4] += _pts;
        }
    }
