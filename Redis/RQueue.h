/** ======================================================================+
 + Copyright @2020-2021 Arjun Ray
 + Released under MIT License
 + see https://mit-license.org
 +========================================================================*/
#pragma once

#ifndef UTILITY_RQUEUE_H
#define UTILITY_RQUEUE_H

#include "Redis.h"
#include <string>

    struct RQueue
    {
        redis::Context  rq_;
        
        ~RQueue() noexcept = default;
        RQueue();
        
        bool lpush( std::string const& key, std::string const& data );
    };


#endif // UTILITY_RQUEUE_H
