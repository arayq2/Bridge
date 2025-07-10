/** ======================================================================+
 + Copyright @2020-2021 Arjun Ray
 + Released under MIT License
 + see https://mit-license.org
 +========================================================================*/
#pragma once

#ifndef BRIDGE_CONTRACT_H
#define BRIDGE_CONTRACT_H

#include <string>

namespace bridge
{
    /**
     * @class Contract
     * @brief Captures specification of a bid in Contract Bridge.
     * Enum classes are used to enforce valid values only.
     * Level is the only enum class needing numeric equivalents.
     */
    class Contract
    {
    public:
        enum class Level : int
        {
            L0, L1, L2, L3, L4, L5, L6, L7
        };

        enum class Rank : int
        {
            NT, MN, MJ
        };

        enum class Dbld : int
        {
            NO, YES, AGAIN
        };

        ~Contract() noexcept = default;

        Contract(Level l, Rank r, Dbld d, bool v = false)
        : level_(l)
        , rank_(r)
        , dbld_(d)
        , vul_(v)
        {}

        Contract(Level l, Rank r, bool v = false)
        : Contract(l, r, Dbld::NO, v)
        {}

        Contract()
        : Contract(Level::L0, Rank::NT) // as good as any default
        {}

        explicit
        Contract(std::string const&); // parse from string

        // Any number outside [0,13] returns 0
        int score( int tricks ) const;

        Contract& level( Level l ) { level_ = l; return *this; }
        Contract& rank( Rank r )   { rank_  = r; return *this; }
        Contract& dbld( Dbld d )   { dbld_  = d; return *this; }
        Contract& vul( bool v )    { vul_   = v; return *this; }
        Contract& clear() { *this = Contract(); return *this; }

        Level level() const { return level_; }
        Rank  rank() const  { return rank_; }
        Dbld  dbld() const  { return dbld_; }
        bool  vul() const   { return vul_; }

        static Level get_level( int bid );
        static Rank  get_rank( int bid );
        static Dbld  get_dbld( int dbld );
        static bool  is_vul( int spec, int side );
        static std::string summary( std::string const& spec, int by, int tricks );

    private:
        Level   level_;
        Rank    rank_;
        Dbld    dbld_;
        bool    vul_;
        //
        int minus_( int ut ) const; // undertricks
        int plus_( int ot = 0 ) const;  // overtricks
        int trickscore_( int ) const;
        int overtricks_( int ) const;
    };

} // namespace bridge

#endif // BRIDGE_CONTRACT_H
