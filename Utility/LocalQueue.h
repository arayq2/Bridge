/** ======================================================================+
 + Copyright @2015-2021 Arjun Ray
 + Released under MIT License: see https://mit-license.org
 +========================================================================*/
#pragma once

#ifndef UTILITY_LOCALQUEUE_H
#define UTILITY_LOCALQUEUE_H

#include "BasicQueue.h"
#include <thread>
#include <utility>

namespace Utility
{
    /**
     * @class LocalQueue
     * @brief Multi-producer, single-consumer queue.
     */
    template<typename Handler>
    class LocalQueue
    {
        using Item   = typename Handler::item_type;
        using Queue  = BasicQueue<Item>;
        using Worker = std::thread;
    public:
        ~LocalQueue() noexcept
        {
            stop_();
            worker_.join();
        }
        //
        template<typename... Args>
        LocalQueue(Args&&... args)
        : handler_(std::forward<Args>(args)...)
        , worker_([this]() -> void { queue_.pump( handler_ ); })
        {}

        bool put( Item& item )
        {
            return queue_.put( std::move(item) );
        }

        bool put( Item&& item )
        {
            return queue_.put( std::move(item) );
        }

        void stop()
        {
            stop_();
        }

    private:
        Queue   queue_;
        Handler handler_;
        Worker  worker_;
        bool    stopped_{false};

        void stop_()
        {
            if ( !stopped_ )
            {
                stopped_ = true;
                queue_.stop();
            }
        }
    };
} // namespace Utility

#endif // UTILITY_LOCALQUEUE_H
