/** ======================================================================+
 + Copyright @2020-2021 Arjun Ray
 + Released under MIT License
 + see https://mit-license.org
 +========================================================================*/
#pragma once

#ifndef UTILITY_REDIS_H
#define UTILITY_REDIS_H


#include <utility> // std::swap, std::forward

extern "C"
{
    struct redisReply;
    struct redisContext;
}

namespace redis
{
    struct ContextOptions
    {
        char const* host_{"127.0.0.1"};
        int         port_{6379};
        bool        nonblock_{false};
        bool        unix_{false};
        //
        ContextOptions(char const* host = "127.0.0.1", int port = 6379, bool nonblock = false, bool unix = false)
        : host_(host)
        , port_(port)
        , nonblock_(nonblock)
        , unix_(unix)
        {}
    };

    class Context;
    class Reply
    {
    public:
        ~Reply() noexcept;
        // move semantics only
        Reply(Reply&& other) : reply_(other.reply_) { other.reply_ = nullptr; }
        Reply& operator=( Reply&& rhs )
        {
            std::swap( reply_, rhs.reply_ );
            return *this;
        }
        //
        explicit operator bool() { return !!reply_; }
        //
        redisReply* operator->() { return reply_; }

    private:
        redisReply*     reply_{nullptr};

        Reply(void* ptr) : reply_(reinterpret_cast<redisReply*>(ptr)) {}
        Reply(redisReply* reply) : reply_(reply) {}
        friend class Context;
    };

    class Context
    {
    public:
        ~Context() noexcept;
        //
        Context(ContextOptions const& options = ContextOptions());
        // move semantics only
        Context(Context&& other) : ctxt_(other.ctxt_) { other.ctxt_ = nullptr; }

        Reply invoke( char const* verb, char const* channel, char const* msg );
        Reply invoke( char const* verb, char const* channel, char const* msg, std::size_t len );
        Reply lpush( char const* channel, char const* msg );
        Reply lpush( char const* channel, char const* msg, std::size_t len );
        Reply publish( char const* channel, char const* msg );
        Reply publish( char const* channel, char const* msg, std::size_t len );

    private:
        redisContext*   ctxt_{nullptr};

        template<typename... Args>
        Reply command( char const* fmt, Args&&... args );
    };


} // namespace redis

#endif // UTILITY_REDIS_H
