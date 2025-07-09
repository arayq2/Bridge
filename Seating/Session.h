/** ======================================================================+
 + Copyright @2023-2025 Arjun Ray
 + Released under MIT License
 + see https://mit-license.org
 +========================================================================*/
#pragma once

#ifndef BRIDGE_SESSION_H
#define BRIDGE_SESSION_H

#include "CharBuffer.h"
#include "Json.h"

#include <vector>
#include <string>

// --------------------------------------------------------------------
    /**
     * @struct Specs: Session dimensions.
     * Max 225 tables, as algorithm is different for > 225
     */
    struct Specs
    {
        int     sections_; // number of sections
        int     tps_;      // (minimal) tables per section
        int     extra_;    // how many with tps_ + 1 tables
        //
        ~Specs() = default;
        explicit
        Specs(int total)
        : sections_(1 + (total - 1) / 15)
        , tps_(total / sections_)
        , extra_(total % sections_)
        {}
        Specs(int secs, int xtra)
        : sections_(secs)
        , tps_(14)
        , extra_(xtra)
        {}
        int total() const { return extra_ + tps_ * sections_; }
    };

// --------------------------------------------------------------------
    /**
     * @struct TableSpec: Session set up, who is seated where.
     * Major purpose is to convey table ids to front end.
     * The rest is informational.
     */
    struct TableSpec
    {
        std::string tableid_;
        int         tableno_{0}; // in-section number
        int         ewno_{0}; // in-section number
        int         nsno_{0}; // in-section number
        int         iid_[2]{0}; // Session-wide numbers [ns,ew]
        //
        TableSpec() = default;
        TableSpec(char const* id, int no, int ew, int ns)
        : tableid_(id)
        , tableno_(no)
        , ewno_(ew)
        , nsno_(ns)
        {}

        void
        to_js( nlohmann::json& js ) const
        {
            js["tableid"] = tableid_;
            js["tableno"] = tableno_;
            js["ewno"]    = ewno_;
            js["nsno"]    = nsno_;
            js["iids"]    = iid_;
        }

        void
        fr_js( nlohmann::json const& js )
        {
            js["tableid"].get_to<std::string>( tableid_ );
            js["tableno"].get_to<int>( tableno_ );
            js["ewno"].get_to<int>( ewno_ );
            js["nsno"].get_to<int>( nsno_ );
            js["iids"].get_to<int>( iid_ );
        }
    };

    inline
    void to_json( nlohmann::json& js, TableSpec const& a ) { a.to_js( js ); }

    inline
    void from_json( nlohmann::json const& js, TableSpec& a ) { a.fr_js( js ); }

    using Tables = std::vector<TableSpec>;

// --------------------------------------------------------------------
    /**
     * @struct SectionSpec: Aggregates tables in a section.
     */
    struct SectionSpec
    {
        int             ndx_{0};   // section index, 0-based logical
        std::string     sid_;   // base id for section
        Tables          tbls_;
        //
        //void set( std::string const& id, int size );
        std::size_t size() const { return tbls_.size(); }
     };
    inline
    void to_json( nlohmann::json& js, SectionSpec const& s )
    {
        js["index"]     = s.ndx_;
        js["section"]   = s.sid_;
        js["tables"]    = s.tbls_;
        js["secsize"]   = s.tbls_.size();
    }

    inline
    void from_json( nlohmann::json const& js, SectionSpec & s )
    {
        js["index"].get_to<int>( s.ndx_ );
        js["section"].get_to<std::string>( s.sid_ );
        js["tables"].get_to<Tables>( s.tbls_ );
    }

    using Field = std::vector<SectionSpec>;

// --------------------------------------------------------------------
    /**
     * @struct Session: Aggregates sections in a session.
     */
    struct Session
    {
        std::string     id_;  // session id
        int             total_{0};
        Field           field_;
        //
        std::size_t numsecs() const { return field_.size(); }
        std::size_t size() const { return total_; }
        ~Session() = default;
        Session() = default;
        Session(std::string const& tid, Specs const& specs);
    };

    inline
    void to_json( nlohmann::json& js, Session const& s )
    {
        js["sessid"]   = s.id_;
        js["total"]    = s.total_;
        js["field"]    = s.field_;
    }

    inline
    void from_json( nlohmann::json const& js, Session& s )
    {
        js["sessid"].get_to<std::string>( s.id_ );
        js["total"].get_to<int>( s.total_ );
        js["field"].get_to<Field>( s.field_ );
    }

// --------------------------------------------------------------------
    /**
     * @struct Packet: Informational
     */
    struct Packet
    {
        bool            status_{false};
        std::string     corrId_;
        std::string     hostName_;
        //
        Session         session_;
    };

    inline
    void to_json( nlohmann::json& js, Packet const& p )
    {
        js["status"]   = p.status_;
        js["corrid"]   = p.corrId_;
        js["hostname"] = p.hostName_;
        js["session"]  = p.session_;
    }

    inline
    void from_json( nlohmann::json const& js, Packet& p )
    {
        js["status"].get_to<bool>( p.status_ );
        js["corrid"].get_to<std::string>( p.corrId_ );
        js["hostname"].get_to<std::string>( p.hostName_ );
        js["session"].get_to<Session>( p.session_ );
    }

    // setup functions
    extern void init_section( SectionSpec& sec, int size, int num, char const* sid );
    extern void init_field( Field& field, int tables, int tps, char const* rid );
    extern void init_session( Session& session, int tables, int tps, char const* tid );
    extern int snake( Field& field, int tps );

#endif // BRIDGE_SESSION_H
