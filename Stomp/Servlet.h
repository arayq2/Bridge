/** ======================================================================+
 + Copyright @2020-2025 Arjun Ray
 + Released under MIT License
 + see https://mit-license.org
 +========================================================================*/

#pragma once

#ifndef STOMP_SERVLET_H
#define STOMP_SERVLET_H

#include "PublisherImpl.h"

#include <vector>
#include <utility>
#include <memory>

namespace Stomp
{

    /**
     * @class ServletMethods
     * @brief Policy class for template method pattern.
     * Client class must implement these entry points to override defaults.
     */
    template<typename Client>
    struct ServletMethods
    {
        //!> called before enabling inbound messages
        static void
        on_init( Client* cp, Publisher& publisher )
        {
            cp->on_init( publisher );
        }

        static void
        on_start( Client* cp, Publisher& publisher )
        {
            cp->on_start( publisher );
        }

        static void
        on_message( Client* cp, std::string const& message, EndPoint const& info, Publisher& publisher )
        {
            cp->on_message( message, info, publisher );
        }

        static void
        on_error( Client* cp, std::string const& message, std::string const& errmsg, Publisher& publisher )
        {
            cp->on_error( message, errmsg, publisher );
        }
    };

    /**
     * @class Servlet
     * @brief class for servlet pattern.
     */
    template<typename Client, typename Methods = ServletMethods<Client>>
    class Servlet
    {
    public:
        ~Servlet() {}

        Servlet(StompAgent& agent, Client& client, EndPoint const& state)
        : agent_(agent)
        , cp_(&client)
        , publisher_(agent_, state)
        {
            Methods::on_init( cp_, publisher_ );
        }

        Servlet(StompAgent& agent, Client& client, EndPoint const& state, EndPoint const& event)
        : Servlet(agent, client, state)
        {
            subscribe( event );
        }

        void start()
        {   // optional delayed start
            Methods::on_start( cp_, publisher_ );
        }

        void on_message( std::string const& msg, EndPoint const& info )
        {
            Methods::on_message( cp_, msg, info, publisher_ );
        }

        void subscribe( std::string const& topic, bool isQueue )
        {
            subscribe( EndPoint{topic, isQueue} );
        }
        void subscribe( EndPoint const& ep )
        {
            agent_.subscribe( ep, make_callback(&Servlet::on_message, this) ); //!< may cause delivery of cached message
        }
        // STOMP has no protocol to manage brokers (no release functionality)
        bool unsubscribe( std::string const& target, bool isQueue = false, bool release = false )
        {
            return agent_.unsubscribe( {target, isQueue} );
        }

    private:
        StompAgent&     agent_;
        Client*         cp_;
        TPublisher      publisher_; // topic target by default

        Servlet(Servlet const&) = delete;
        Servlet& operator=( Servlet const& ) = delete;
    };

} // namespace Stomp

#endif // STOMP_SERVLET_H
