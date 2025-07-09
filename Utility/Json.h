/** ======================================================================+
 + Copyright @2020-2025 Arjun Ray
 + Released under MIT License
 + see https://mit-license.org
 +========================================================================*/
#pragma once

#ifndef UTILITY_JSON_H
#define UTILITY_JSON_H

#include "nlohmann/json.hpp"

namespace
{
    inline std::string
    js_str( nlohmann::json const& js, char const* key, std::string const& alt = std::string() )
    {
        return js.find( key ) != js.end() ? js[key].get<std::string>() : alt;
    }

    inline int
    js_int( nlohmann::json const& js, char const* key, int alt = 0 )
    {
        return js.find( key ) != js.end() ? js[key].get<int>() : alt;
    }

    inline bool
    js_bool( nlohmann::json const& js, char const* key, bool alt = false )
    {
        return js.find( key ) != js.end() ? js[key].get<bool>() : alt;
    }

    inline std::size_t
    js_szt( nlohmann::json const& js, char const* key, std::size_t alt = 0 )
    {
        return js.find( key ) != js.end() ? js[key].get<std::size_t>() : alt;
    }

    inline unsigned int
    js_uint( nlohmann::json const& js, char const* key, unsigned int alt = 0 )
    {
        return js.find( key ) != js.end() ? js[key].get<unsigned int>() : alt;
    }

}

#define FLD_INT(J,K)        J[K].get<int>()
#define FLD_STR(J,K)        J[K].get<std::string>()
#define FLD_BOOL(J,K)       J[K].get<bool>()
#define FLD_SZT(J,K)        J[K].get<std::size_t>()
#define FLD_UINT(J,K)       J[K].get<unsigned int>()
#define FLD_DOUBLE(J,K)     J[K].get<double>()

#define JS_PRESENT(J,K)     (J.find(K) != J.end())
#define JS_MISSING(J,K)     (J.find(K) == J.end())

#define LOAD_INT(F,J,K)     do if ( JS_PRESENT(J,K) ) { F = FLD_INT(J,K); } while ( false )
#define LOAD_STR(F,J,K)     do if ( JS_PRESENT(J,K) ) { F = FLD_STR(J,K); } while ( false )
#define LOAD_BOOL(F,J,K)    do if ( JS_PRESENT(J,K) ) { F = FLD_BOOL(J,K); } while ( false )
#define LOAD_SZT(F,J,K)     do if ( JS_PRESENT(J,K) ) { F = FLD_SZT(J,K); } while ( false )
#define LOAD_UINT(F,J,K)    do if ( JS_PRESENT(J,K) ) { F = FLD_UINT(J,K); } while ( false )
#define LOAD_DOUBLE(F,J,K)  do if ( JS_PRESENT(J,K) ) { F = FLD_DOUBLE(J,K); } while ( false )

#define GET_INT(F,J,K,A)    F = JS_PRESENT(J,K) ? FLD_INT(J,K) : A
#define GET_STR(F,J,K,A)    F = JS_PRESENT(J,K) ? FLD_STR(J,K) : A
#define GET_BOOL(F,J,K,A)   F = JS_PRESENT(J,K) ? FLD_BOOL(J,K) : A
#define GET_SZT(F,J,K,A)    F = JS_PRESENT(J,K) ? FLD_SZT(J,K) : A
#define GET_UINT(F,J,K,A)   F = JS_PRESENT(J,K) ? FLD_UINT(J,K) : A
#define GET_DOUBLE(F,J,K,A) F = JS_PRESENT(J,K) ? FLD_DOUBLE(J,K) : A

#endif // UTILITY__H
