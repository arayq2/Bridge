/** ======================================================================+
 + Copyright @2020-2025 Arjun Ray
 + Released under MIT License
 + see https://mit-license.org
 +========================================================================*/
#pragma once

#ifndef BRIDGE_DECK_H
#define BRIDGE_DECK_H

#include "nlohmann/json.hpp"
#include <array>

    /**
     * locations of all cards in a std::array<int, 52>, using
     * SHDC <-> [0-12][13-25][26-38][39-51] and
     * WNES <-> {1}{2}{3}{4} ({0} = out of play)
     */
    class Deck
    {
        enum { DECKSIZE = 52 };
    public:
        enum class Format : int { HEX, LIN, PBN, BHG };
        ~Deck() noexcept = default;
        //
        Deck() = default;
        explicit
        Deck(char const*, Format = Format::HEX);
        //
        bool load_pbn_s( char const* );
        bool load_lin_s( char const* );
        bool load_hex( char const* );
        bool load_bhg( char const*, int = 2 ); // North first
        //
        char* store_record( char* buf, std::size_t len, int dlr, int vul ) const;
        char* store_record_x( char* buf, std::size_t len, int dlr, int vul, bool oneline ) const;
        char* store_pbn_s( char* pbn, std::size_t len, int dlr ) const;
        char* store_lin_s( char* lin, std::size_t len, int dlr ) const;
        char* store_hex( char*, std::size_t = 0 ) const;
        char* store_bhg( char*, std::size_t = 0, int seat = 2 ) const; //North first
        //
        int&       operator[]( std::size_t ndx )       { return deck_[ndx]; }
        int const& operator[]( std::size_t ndx ) const { return deck_[ndx]; }
        //
        int*       begin()       { return deck_.begin(); }
        int const* begin() const { return deck_.begin(); }
        int*       end()       { return deck_.end(); }
        int const* end() const { return deck_.end(); }
        //
        void to_js( nlohmann::json& j ) const { j = deck_; }
        void fr_js( nlohmann::json const& j ) { j.get_to( deck_ ); }
        //
        void to_js( nlohmann::json& j, char const* key ) const { j[key] = deck_; }
        void fr_js( nlohmann::json const& j, char const* key) { j.at( key ).get_to( deck_ ); }
        static bool verify_hex( char const* );
        int count( int = 0 ) const;
        //needs a 4-array (at least) passed in for point counts
        static void get_hcp( int* hcp, char const* hex );

    private:
        std::array<int, DECKSIZE>   deck_{0};

        bool load_( char const* input, Format fmt );
    };

    inline void to_json( nlohmann::json& js, Deck const& deck ) { deck.to_js( js ); }
    inline void from_json( nlohmann::json const& js, Deck& deck ) { deck.fr_js( js ); }


#endif // BRIDGE_DECK_H
