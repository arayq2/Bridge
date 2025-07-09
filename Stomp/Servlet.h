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
        on_message( Client* cp, nlohmann::json& event, std::string const& info, Publisher& publisher )
        {
            cp->on_message( event, info, publisher );
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
     * Payloads are assumed to be JSON strings.
     */
    template<typename Client, typename Methods = ServletMethods<Client>>
    class Servlet
    {
    public:
        ~Servlet() {}

        Servlet(Stomp::StompAgent& agent, Client& client, Stomp::EndPoint const& state, bool purge = false)
        : agent_(agent)
        , cp_(&client)
        , publisher_(agent_, state, purge)
        {
            Methods::on_init( cp_, publisher_ );
        }

        Servlet(Stomp::StompAgent& agent, Client& client, Stomp::EndPoint const& state, Stomp::EndPoint const& event, bool purgeState = false)
        : Servlet(agent, client, state, purgeState)
        {
            subscribe( event );
        }

        void start()
        {   // optional delayed start
            Methods::on_start( cp_, publisher_ );
        }

        //!> parse text into JSON object and pass to client
        void on_message( std::string const& msg, Stomp::EndPoint const& info )
        {
            try {
                auto    _evt(nlohmann::json::parse( msg ));
                Methods::on_message( cp_, _evt, info.dest_, publisher_ );
            }
            catch ( nlohmann::json::parse_error const& e )
            {
                Methods::on_error( cp_, msg, e.what(), publisher_ );
            }
        }

        void subscribe( std::string const& topic, bool isQueue )
        {
            subscribe( Stomp::EndPoint{topic, isQueue} );
        }
        void subscribe( ams::EndPoint const& ep )
        {
            subscribe( Stomp::EndPoint{ep.dest_, !ep.isTopic_} );
        }
        void subscribe( Stomp::EndPoint const& ep )
        {
            agent_.subscribe( ep, Stomp::make_callback(&Servlet::on_message, this) ); //!< may cause delivery of cached message
        }
        // STOMP has no protocol to manage brokers (no release functionality)
        bool unsubscribe( std::string const& target, bool isQueue = false, bool release = false )
        {
            return agent_.unsubscribe( {target, isQueue} );
        }

    private:
        Stomp::StompAgent&  agent_;
        Client*             cp_;
        TPublisher          publisher_; // topic target by default

        Servlet(Servlet const&) = delete;
        Servlet& operator=( Servlet const& ) = delete;
    };

#endif // STOMP_SERVLET_H
