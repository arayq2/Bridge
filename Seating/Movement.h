/** ======================================================================+
 + Copyright @2020-2025 Arjun Ray
 + Released under MIT License
 + see https://mit-license.org
 +========================================================================*/
#pragma once

#ifndef BRIDGE_MOVEMENT_H
#define BRIDGE_MOVEMENT_H

#include "nlohmann/json.hpp"

#include <vector>

    /**
     * Structures to manage movements.
     */

namespace Bridge
{
    struct Matchup // SeatInfo
    {
        //int     round_;
        int     tableno_{0};
        int     nspr_{0};
        int     ewpr_{0};
        int     iids_[2]{0}; // [ns, ew]

        Matchup() = default;
        Matchup( int tableno, int nspr, int ewpr, int nsiid, int ewiid ) : tableno_(tableno), nspr_(nspr), ewpr_(ewpr), iids_{nsiid, ewiid} {}
        Matchup( int tableno, int nspr, int ewpr ) : Matchup{ tableno, nspr, ewpr, nspr, ewpr } {}
   };

    using Matchups = std::vector<Matchup>; // size = #tables
    using Seating  = std::vector<Matchups>; // size = #rounds

    inline
    void to_json( nlohmann::json& js, Matchup const& mu )
    {
        js["tableno"] = mu.tableno_;
        js["nspr"]    = mu.nspr_;
        js["ewpr"]    = mu.ewpr_;
        js["iids"]    = mu.iids_;
    }

    inline
    void from_json( nlohmann::json const& js, Matchup& mu )
    {
        js.at( "tableno" ).get_to( mu.tableno_ );
        js.at( "nspr" ).get_to( mu.nspr_ );
        js.at( "ewpr" ).get_to( mu.ewpr_ );
        js.at( "iids" ).get_to( mu.iids_ );
    }

    void init_seating( Matchups& matchups ); // assign pair numbers
    //
    void new_round_mitchell( Matchups& next, Matchups const& prev );
    void new_round_howell( Matchups& next, Matchups const& prev );
    //
    void fill_seating( Seating& seating, bool howell = false );

//-------------------------------------------------------------------------
    struct Placement
    {
        //int     round_;
        int     table_{0};
        bool    ew_{false};
    };

    using Itinerary = std::vector<Placement>; // size = #rounds
    using Movement  = std::vector<Itinerary>; // size = #pairs

    inline
    void to_json( nlohmann::json& js, Placement const& pl )
    {
        js["table"] = pl.table_;
        js["ew"]    = pl.ew_;
    }

    inline
    void from_json( nlohmann::json const& js, Placement& pl )
    {
        js.at( "table" ).get_to( pl.table_ );
        js.at( "ew" ).get_to( pl.ew_ );
    }

    inline
    bool operator==( Placement const& lhs, Placement const& rhs )
    {
        return lhs.table_ == rhs.table_ && lhs.ew_ == rhs.ew_;
    }

    inline
    bool operator!=( Placement const& lhs, Placement const& rhs )
    {
        return lhs.table_ != rhs.table_ || lhs.ew_ != rhs.ew_;
    }

    void fill_movement( Movement& movement, Seating const& seating );
    int find_pair( Movement const&, int round, int table, bool ew );

//-------------------------------------------------------------------------
    /*
     * Detailed information on move at end of round. 
     */

    struct MoveSeat
    {
        int         seat_{0};
        std::string plid_;
        int         iid_{0};
        std::string tableid_;
        //
        void reset() { *this = MoveSeat(); }
        void to_js( nlohmann::json& js ) const;
        void fr_js( nlohmann::json const& js );
    };

    inline
    void to_json( nlohmann::json& j, MoveSeat const& ns ) { ns.to_js( j ); }
    inline
    void from_json( nlohmann::json const& j, MoveSeat& ns ) { ns.fr_js( j ); }

    using Move = std::array<MoveSeat, 4>;

    struct MoveTable
    {
        int         no_{0};
        std::string id_;  // table id
        Move        move_;
        //
        void to_js( nlohmann::json& js ) const;
        void fr_js( nlohmann::json const& js );
    };

    inline
    void to_json( nlohmann::json& js, MoveTable const& mt ) { mt.to_js( js ); }
    inline
    void from_json( nlohmann::json const& js, MoveTable& mt ) { mt.fr_js( js ); }

    using MoveTables = std::vector<MoveTable>; // size = #tables

} // namespace Bridge

#endif // BRIDGE_MOVEMENT_H
