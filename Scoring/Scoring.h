/** ======================================================================+
 + Copyright @2020-2025 Arjun Ray
 + Released under MIT License
 + see https://mit-license.org
 +========================================================================*/
#pragma once

#ifndef BRIDGE_SCORING_H
#define BRIDGE_SCORING_H

#include <vector>
#include <array>
#include <algorithm>

    /**
     * @file Scoring.h: Scoring routines.
     * Input is a vector of pointers to a templated type.
     * This allows assembly from disparate sources, and external sorting.
     * The second template parameter is a struct defining two methods:
     * (1) A comparison function between two pointers.
     * (2) An assignment function for the computed score.
     */
namespace bridge
{
    // for documentation only.  With these, mpt_score() does nothing.
    template<typename Cell>
    struct MPTCellMethods
    {
        // first < second
        static bool less( Cell const*, Cell const* ) { return false; }
        // max is the highest possible score
        static void award( Cell*, int score, int max ) {}
    };

    /**
     * @function mpt_score: Matchpoint scoring.
     */
    template<typename Cell, typename Methods = MPTCellMethods<Cell>>
    bool mpt_score( std::vector<Cell*>& cv )
    {
        // sanity checks?
        // added by PM 1/23 - allow a size of 1: if no comparisons, score as an average
        if ( cv.size() == 0 ) { return false; }
        if ( cv.size() == 1 ) 
        { 
            Methods::award( *cv[0], 1, 2 );
            return true; 
        }
 
        int         _max{2 * ((int)cv.size() - 1)};
        int         _worse{0};
        int         _ties{0};

        // sort lowest to highest and then fox+hound pattern to detect ties 
        std::sort( cv.begin(), cv.end(), Methods::less );
        auto        _fox{cv.begin()};
        auto        _hound{cv.begin()};
        auto        _fill([&]() -> void
        {
            int     _score(_ties + 2 * _worse);
            do {
                Methods::award( *_hound, _score, _max );
            } while ( ++_hound != _fox );
        });

        //
        while ( ++_fox != cv.end() )
        {
            if ( Methods::less( *_hound, *_fox ) )
            {
                _fill();
                _worse += 1 + _ties;
                _ties = 0;
            }
            else { ++_ties; }
        }
        _fill();
        //
	    return true;
    }

namespace
{
    std::array<int, 24> scale =
    {
          10,   40,   80,  120,  160,  210,
         260,  310,  360,  420,  490,  590,
         740,  890, 1090, 1290, 1490, 1740,
        1990, 2240, 2490, 2990, 3490, 3990
    };
    
    inline
    int raw_imps( int datum )
    {
        return datum < 0
        ? -(std::lower_bound( scale.begin(), scale.end(), -datum ) - scale.begin())
        :  (std::lower_bound( scale.begin(), scale.end(),  datum ) - scale.begin());
    }
}
    // for documentation only.  With these, imp_score() does nothing.
    template<typename Cell>
    struct IMPCellMethods
    {
        // first - second
        static int diff( Cell const*, Cell const* ) { return 0; }
        // num is the number of comparisons
        static void award( Cell*, int score, int num ) {}
    };

    /**
     * @function imp_score: Cross-IMPs scoring.
     */
    template<typename Cell, typename Methods = IMPCellMethods<Cell>>
    bool imp_score( std::vector<Cell*>& cv )
    {
        // sanity checks?
        int                 _max((int)cv.size());
        if ( _max < 2 ) { return false; }
        std::vector<int>    _scores(cv.size(), 0); // scratch
        //
        for ( int _hound{0}; _hound < _max; ++_hound )
        {
            for ( int _fox{_hound + 1}; _fox < _max; ++_fox )
            {
                int     _imps{raw_imps( Methods::diff( cv[_hound], cv[_fox] ) )};
                _scores[_hound] += _imps;
                _scores[_fox]   -= _imps;
            }
            Methods::award( cv[_hound], _scores[_hound], _max - 1 );
        }
        //
		return true;
	}

    // for documentation only.  With these, dat_score() does nothing.
    template<typename Cell>
    struct DATCellMethods
    {
        // first < second
        static bool less( Cell const*, Cell const* ) { return false; }
        // first - second
        static int datum( Cell const*, int ) { return 0; }
        // num is the number of comparisons
        static void award( Cell*, int score, int num ) {}
    };

    /**
     * @function dat_score: Datum-IMPs scoring (Butler).
     */
    template<typename Cell, typename Methods = DATCellMethods<Cell>>
    bool dat_score( std::vector<Cell*>& cv )
    {
       // this is obsolete, and there are variations 
       return false;
    }
} // namespace bridge

#endif // BRIDGE_SCORING_H
