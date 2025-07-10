/** ======================================================================+
 + Copyright @2020-2021 Arjun Ray
 + Released under MIT License
 + see https://mit-license.org
 +========================================================================*/

#include "Contract.h"

#include <regex>
#include <unordered_map>
#include <array>

namespace bridge
{
namespace
{
    inline
    int as_integer( Contract::Level level )
    {
        switch ( level )
        {
        default:
        case Contract::Level::L1 : return 1;
        case Contract::Level::L2 : return 2;
        case Contract::Level::L3 : return 3;
        case Contract::Level::L4 : return 4;
        case Contract::Level::L5 : return 5;
        case Contract::Level::L6 : return 6;
        case Contract::Level::L7 : return 7;
        }
    }

    inline
    int slam( Contract::Level level, bool vul )
    {
        switch ( level )
        {
        case Contract::Level::L7 : return vul ? 1500 : 1000;
        case Contract::Level::L6 : return vul ?  750 :  500;
        default: return 0;
        }
    }
}

    int
    Contract::score( int tricks ) const
    {
        if ( level_ == Level::L0 || tricks < 0 || tricks > 13 ) { return 0; }
        int _diff(tricks - 6 - as_integer( level_ ));
        return _diff < 0 ? -minus_( -_diff ) : plus_( _diff );
    }

    int
    Contract::minus_( int ut ) const
    {
        switch ( dbld_ )
        {
        case Dbld::NO   :
            return ut * (vul_ ? 100 : 50);
        case Dbld::YES  :
            return vul_ ? 200 + (ut - 1) * 300 : 100 + (ut - 1) * 200 + (ut > 3 ? ut - 3 : 0) * 100;
        case Dbld::AGAIN:
            return vul_ ? 400 + (ut - 1) * 600 : 200 + (ut - 1) * 400 + (ut > 3 ? ut - 3 : 0) * 200;
        default: return 0;
        }
    }

    int
    Contract::trickscore_( int tricks ) const
    {
        switch ( rank_ )
        {
        case Rank::NT : return 40 + 30 * (tricks - 1);
        case Rank::MN : return 20 * tricks;
        case Rank::MJ : return 30 * tricks;
        default       : return 0;
        }
    }
    
    int
    Contract::overtricks_( int ot ) const
    {
        switch ( dbld_ )
        {
        case Dbld::NO   :
            switch ( rank_ )
            {
            case Rank::NT :
            case Rank::MJ : return 30 * ot;
            case Rank::MN : return 20 * ot;
            }
        case Dbld::YES  : return ot * (vul_ ? 200 : 100);
        case Dbld::AGAIN: return ot * (vul_ ? 400 : 200);
        default         : return 0;
        }
    }
    
    int
    Contract::plus_( int ot ) const
    {
        // trickscore
        int _ts;
        int _xtra;
        switch ( dbld_ )
        {
        case Dbld::NO   : 
            _ts = trickscore_( as_integer( level_ ) );
            _xtra = 0;
            break;
        case Dbld::YES  :
            _ts = 2 * trickscore_( as_integer( level_ ) );
            _xtra = 50;
            break;
        case Dbld::AGAIN:
            _ts = 4 * trickscore_( as_integer( level_ ) );
            _xtra = 100;
            break;
        }
        // overtricks
        int _otb = overtricks_( ot );
        // bonuses
        int _game = (_ts < 100 ? 50 : vul_ ? 500 : 300);
        // total
        return _ts + _xtra + _otb + _game + slam( level_, vul_ );
    }

#define GOAL(B)  (7 + (B - 1) / 5)

    Contract::Level
    Contract::get_level( int bid )
    {
        switch ( GOAL(bid) )
        {
        case  7: return Contract::Level::L1;
        case  8: return Contract::Level::L2;
        case  9: return Contract::Level::L3;
        case 10: return Contract::Level::L4;
        case 11: return Contract::Level::L5;
        case 12: return Contract::Level::L6;
        default:
        case 13: return Contract::Level::L7;
        }
    }

    Contract::Rank
    Contract::get_rank( int bid )
    {
        switch ( bid % 5 )
        {
        default:
        case 0: return Contract::Rank::NT;
        case 1:
        case 2: return Contract::Rank::MN;
        case 3:
        case 4: return Contract::Rank::MJ;
        }
    }

    Contract::Dbld
    Contract::get_dbld( int dbld )
    {
        switch ( dbld )
        {
        default:
        case 0: return Contract::Dbld::NO;
        case 1: return Contract::Dbld::YES;
        case 2: return Contract::Dbld::AGAIN;
        }
    }

namespace
{
    std::regex      contractPattern("^([1-7])([CDHNScdhns])([xX]([xX])?)?$");

    std::unordered_map<std::string, Contract::Level> levelMap =
    {
        { "1", Contract::Level::L1 },
        { "2", Contract::Level::L2 },
        { "3", Contract::Level::L3 },
        { "4", Contract::Level::L4 },
        { "5", Contract::Level::L5 },
        { "6", Contract::Level::L6 },
        { "7", Contract::Level::L7 }
    };

    std::unordered_map<std::string, Contract::Rank> rankMap =
    {
        { "C", Contract::Rank::MN },
        { "D", Contract::Rank::MN },
        { "H", Contract::Rank::MJ },
        { "N", Contract::Rank::NT },
        { "S", Contract::Rank::MJ },
        { "c", Contract::Rank::MN },
        { "d", Contract::Rank::MN },
        { "h", Contract::Rank::MJ },
        { "n", Contract::Rank::NT },
        { "s", Contract::Rank::MJ },
    };

    std::array<std::array<bool, 2>, 4> sidevul =
    {{
        {{ false, false }},
        {{ true,  false }},
        {{ false, true  }},
        {{ true,  true  }},
    }};

    std::array<char const*, 20> outcome =
    {
        "-13", "-12", "-11", "-10",
         "-9",  "-8",  "-7",  "-6",
         "-5",  "-4",  "-3",  "-2",
         "-1",   "=",  "+1",  "+2",
         "+3",  "+4",  "+5",  "+6"
    };

}

    bool
    Contract::is_vul( int spec, int side )
    {
        return sidevul[spec - 1][side % 2];
    }

    std::string
    Contract::summary( std::string const& spec, int by, int tricks )
    {
        auto    _isl([]( char const& l){ return l > '0' && l < '8'; });
        return spec + " WNES"[by] + (_isl( spec[0] ) ? outcome[tricks + 6 - (spec[0] - '1')] : "?");
    }

    Contract::Contract(std::string const& spec)
    : Contract()
    {
        std::cmatch     _m;
        if ( std::regex_match( spec.c_str(), _m, contractPattern ) )
        {
            level_ = levelMap[_m.str( 1 )];
            rank_  = rankMap[_m.str( 2 )];
            dbld_  = _m.length( 4 ) > 0
                   ? Contract::Dbld::AGAIN
                   : _m.length( 3 ) > 0
                   ? Contract::Dbld::YES
                   : Contract::Dbld::NO;
        }
    }

} // namespace bridge
