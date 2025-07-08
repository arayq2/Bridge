/** ======================================================================+
 + Copyright @2020-2025 Arjun Ray
 + Released under MIT License
 + see https://mit-license.org
 +========================================================================*/
#pragma once

#ifndef BRIDGE_DEAL_H
#define BRIDGE_DEAL_H

#include <array>
#include <iosfwd>

    class Holding
    {
    public:
        ~Holding() noexcept = default;
        Holding() = default;

        Holding& add( char card )
        {
            vec_[ndx_++] = card;
            return *this;
        }

        char const* get( char const* vs = "" ) const { return ndx_ ? &vec_[0] : vs; }
        std::size_t length() const { return ndx_; }

    private:
        std::size_t             ndx_{0};
        std::array<char, 14>    vec_{0};
    };

    using Hand = std::array<Holding, 4>; //!< S H D C


    class Deal
    {
    public:
        ~Deal() noexcept = default;
        Deal() = default;
        Deal(int const* deck) { layout( deck ); }
        Deal(char const* hex) { layout( hex ); }
        //
        Deal& reset();
        Deal& layout( int const* ); //!< usually from class Deck
        Deal& layout( char const* ); //!< hex string
        void visualize( std::ostream& ) const;
        //
        Hand&       operator[]( std::size_t index )       { return deal_[index % 4]; }
        Hand const& operator[]( std::size_t index ) const { return deal_[index % 4]; }

        static char* store_pbn( char* buf, std::size_t len, Hand const& hand );
        static char* store_lin( char* buf, std::size_t len, Hand const& hand );
        //!> GIB hand record format (PBN), encoded dealer and vulnerability
        static char* store_hrdv( char* buf, std::size_t len, char const* hex, int dlr, int vul );

    private:
        std::array<Hand, 4>     deal_; //!< W N E S
    };

#endif // BRIDGE_DEAL_H
