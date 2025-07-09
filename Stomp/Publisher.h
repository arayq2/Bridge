/** ======================================================================+
 + Copyright @2023-2025 Arjun Ray
 + Released under MIT License
 + see https://mit-license.org
 +========================================================================*/
#pragma once

#ifndef STOMP_PUBLISHER_H
#define STOMP_PUBLISHER_H

#include "nlohmann/json.hpp"

#include <string>

    // forward declaration
    namespace Stomp { class EndPoint; }

    /**
     * @struct Publisher
     * @brief  Basic output interface for JSON messages.
     *         Topics vs Queues may be distinguished by the implementation.
     */
    struct Publisher
    {
        virtual ~Publisher() {}

        // assumes an implementation defined default destination
        virtual void publish( nlohmann::json const& js ) = 0;
        virtual void publish( std::string const& msg ) {}

        // explicit destination
        virtual void publish( Stomp::EndPoint const& dest, nlohmann::json const& js ) = 0;
        virtual void publish( Stomp::EndPoint const& dest, std::string const& msg ) {};

        // for request/response scenarios
        virtual void publish_once( Stomp::EndPoint const& dest, nlohmann::json const& js ) = 0;
        virtual void publish_once( Stomp::EndPoint const& dest, std::string const& msg ) {};
    };

#endif // STOMP_PUBLISHER_H
