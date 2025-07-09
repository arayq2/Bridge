/** ======================================================================+
 + Copyright @2020-2025 Arjun Ray
 + Released under MIT License
 + see https://mit-license.org
 +========================================================================*/

#include "Session.h"

    using IntVec = std::vector<int>;

namespace
{
    std::array<IntVec, 15> seedingOrder =
    {{
/* 1*/  { 1 },
/* 2*/  { 1,  2 },
/* 3*/  { 1,  3,  2 },
/* 4*/  { 1,  3,  4,  2 },
/* 5*/  { 5,  1,  3,  4,  2 },
/* 6*/  { 5,  1,  3,  6,  4,  2 },
/* 7*/  { 5,  1,  7,  3,  6,  4,  2 },
/* 8*/  { 5,  1,  7,  3,  6,  4,  8,  2 },
/* 9*/  { 5,  9,  1,  7,  3,  6,  4,  8,  2 },
/*10*/  { 5,  9,  1,  7,  3,  6, 10,  4,  8,  2 },
/*11*/  { 5,  9,  1,  7, 11,  3,  6, 10,  4,  8,  2 },
/*12*/  { 5,  9,  1,  7, 11,  3,  6, 10,  4,  8, 12,  2 },
/*13*/  { 5,  9, 13,  1,  7, 11,  3,  6, 10,  4,  8, 12,  2 },
/*14*/  { 5,  9, 13,  1,  7, 11,  3,  6, 10, 14,  4,  8, 12,  2 },
/*15*/  { 5,  9, 13,  1,  7, 11, 15,  3,  6, 10, 14,  4,  8, 12,  2 },
    }};

    bool
    validate( Field const& field, size_t tps )
    {
        for ( auto const& _sec : field )
        {
            if ( _sec.tbls_.size() != tps && _sec.tbls_.size() != tps + 1 ) { return false; }
        }
        return true;
    }
}

    /**
     * @function snake()
     * @brief assign internal pair numbers.
     * Pair numbers are assumed to be sorted (by master point rankings).
     * Sections have at most 15 tables.
     * Sections have either tps or tps + 1 tables.
     * The snake is run for tps table order in all sections,
     * and then a final pass for the (tps + 1) table sections.
     */
    int
    snake( Field& field, IntVec const& order, int ndx )
    {
        if ( !validate( field, order.size() ) ) { return ndx; }

        bool            _ew(false);

        for ( int const _pos : order ) // seeding order for tables
        {
            for ( auto _itr(field.begin()); _itr != field.end(); ++_itr )
            {
                _itr->tbls_[_pos - 1].iid_[_ew] = ndx++;
                _ew = !_ew; 
            }
            for ( auto _ritr(field.rbegin()); _ritr != field.rend(); ++_ritr )
            {
                _ritr->tbls_[_pos - 1].iid_[_ew] = ndx++;
                _ew = !_ew; 
            }
            _ew = !_ew;
        }

        size_t          _tps(order.size()); // tables per section

        for ( auto _itr(field.begin()); _itr != field.end(); ++_itr )
        {
            if ( _itr->tbls_.size() == _tps + 1 )
            {
                _itr->tbls_[_tps].iid_[_ew] = ndx++;
                _ew = !_ew; 
            }
        }
        for ( auto _ritr(field.rbegin()); _ritr != field.rend(); ++_ritr )
        {
            if ( _ritr->tbls_.size() == _tps + 1 )
            {
                _ritr->tbls_[_tps].iid_[_ew] = ndx++;
                _ew = !_ew; 
            }
        }

        return ndx;
    }

    int
    snake( Field& field, int tps )
    {
        return snake( field, seedingOrder[tps - 1], 1 ); 
    }
