/** ======================================================================+
 + Copyright @2020-2025 Arjun Ray
 + Released under MIT License
 + see https://mit-license.org
 +========================================================================*/

#include "Movement.h"

namespace Bridge
{
    void
    init_seating( Matchups& matchups )
    {
        int     _tno{1};
        int     _ew{1};
        int     _ns{2};

        for ( auto& _matchup : matchups )
        {
            _matchup = {_tno++, _ns, _ew};
            _ns += 2;
            _ew += 2;
        }
    }

    void
    new_round_mitchell( Matchups& next, Matchups const& prev )
    {
        std::size_t _max{next.size() - 1};
        for ( std::size_t _tbl = 0; _tbl < next.size(); ++_tbl )
        {
            next[_tbl] = {prev[_tbl].tableno_, prev[_tbl].nspr_, prev[_tbl ? _tbl - 1 : _max].ewpr_};
        }
    }

    void
    new_round_howell( Matchups& next, Matchups const& prev )
    {   // clockwise rotation, highest NS stationary (could be a dummy pair!)
        std::size_t _max{next.size() - 1};

        if ( _max > 1 )
        {
            next[0] = {prev[0].tableno_, prev[1].nspr_, prev[0].nspr_};
            for ( std::size_t _tbl = 1; _tbl < _max - 1; ++_tbl )
            {
                next[_tbl] = {prev[_tbl].tableno_, prev[_tbl + 1].nspr_, prev[_tbl - 1].ewpr_};
            }
            next[_max - 1] = {prev[_max - 1].tableno_, prev[_max].ewpr_, prev[_max - 2].ewpr_};
        }
        else { next[0] = {prev[0].tableno_, prev[1].ewpr_, prev[0].nspr_}; }
        next[_max] = {prev[_max].tableno_, prev[_max].nspr_, prev[_max - 1].ewpr_};
    }

    void
    fill_seating( Seating& seating, bool ishowell )
    {
        std::size_t _lround(0);

        init_seating( seating[0] );
        while ( ++_lround < seating.size() )
        {
            ishowell
            ? new_round_howell( seating[_lround], seating[_lround - 1] )
            : new_round_mitchell( seating[_lround], seating[_lround - 1] )
            ;
        }
    }

    void
    fill_movement( Movement& movement, Seating const& seating )
    {
        int     _lround{0};
        for ( auto const& _matchups : seating )
        {
            for ( auto const& _matchup : _matchups )
            {
                movement[_matchup.nspr_ - 1][_lround] = {_matchup.tableno_, false};
                movement[_matchup.ewpr_ - 1][_lround] = {_matchup.tableno_, true};
            }
            ++_lround;
        }
    }

    int
    find_pair( Movement const& movement, int lrd, int table, bool ew )
    {
        for ( std::size_t _lpr{0}; _lpr < movement.size(); ++_lpr )
        {
            auto const& _pl(movement[_lpr][lrd]);
            if ( _pl.table_ == table && _pl.ew_ == ew ) { return _lpr; }
        }
        return -1; // should not happen
    }

} // namespace Bridge
