/** ======================================================================+
 + Copyright @2020-2021 Arjun Ray
 + Released under MIT License
 + see https://mit-license.org
 +========================================================================*/

#include "utility/RQueue.h"
#include <cstdlib>

namespace
{
    char const* safe_getenv( char const* key, char const* alt )
    {
        char const* _ptr(::getenv( key ));
        return _ptr ? _ptr : alt; 
    }

    redis::ContextOptions   options(safe_getenv("REDIS_SERVER", "172.17.0.1"));
}

    RQueue::RQueue()
    : rq_(options)
    {}

    bool
    RQueue::lpush( std::string const& key, std::string const& data )
    {
        return !!rq_.lpush( key.c_str(), data.c_str() );
    }
