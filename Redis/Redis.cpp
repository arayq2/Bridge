/** ======================================================================+
 + Copyright @2020-2021 Arjun Ray
 + Released under MIT License
 + see https://mit-license.org
 +========================================================================*/

#include "Redis.h"
#include "hiredis/hiredis.h"

#include <cstddef> // std::size_t

namespace redis
{
//=========================================================================
    Reply::~Reply() noexcept
    {
        if ( reply_ ) { freeReplyObject( reply_ ); }
    }

//=========================================================================
    Context::~Context() noexcept
    {
        if ( ctxt_ ) { redisFree( ctxt_ ); }
    }

    Context::Context(ContextOptions const& options)
    : ctxt_(redisConnect( options.host_, options.port_ ))
    {}

    template<typename... Args>
    Reply
    Context::command(char const* fmt, Args&&... args)
    {
        return redisCommand( ctxt_, fmt, std::forward<Args>(args)... );
    }

    Reply
    Context::invoke( char const* verb, char const* channel, char const* msg )
    {
        return command( "%s %s %s", verb, channel, msg );
    }

    Reply
    Context::invoke( char const* verb, char const* channel, char const* msg, std::size_t len )
    {
        return command( "%s %s %b", verb, channel, msg, len );
    }

    Reply
    Context::publish( char const* channel, char const* msg )
    {
        return command( "PUBLISH %s %s", channel, msg );
    }

    Reply
    Context::publish( char const* channel, char const* msg, std::size_t len )
    {
        return command( "PUBLISH %s %b", channel, msg, len );
    }

    Reply
    Context::lpush( char const* channel, char const* msg )
    {
        return command( "LPUSH %s %s", channel, msg );
    }

    Reply
    Context::lpush( char const* channel, char const* msg, std::size_t len )
    {
        return command( "LPUSH %s %b", channel, msg, len );
    }


} // namespace redis
