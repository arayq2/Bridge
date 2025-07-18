/** ======================================================================+
 + Copyright @2023-2025 Arjun Ray
 + Released under MIT License
 + see https://mit-license.org
 +========================================================================*/
#pragma once

#ifndef STOMP_FEEDER_H
#define STOMP_FEEDER_H

#include "StompAgent.h"
#include "LocalQueue.h"

#include <unordered_map>
    
namespace Stomp
{
    /**
     * @file Feeder.h
     * @brief Generic input system using local queue buffering.
     * Each broker topic/queue has an associated info item as an identifier.
     * This is propagated to the client by the consumer (handler) of the local queue.
     */
    template<typename Client, typename Info>
    class Feeder
    {
        struct Input
        {
            std::string     payload_;
            Info            info_;
        };
        //
        struct Handler
        {   // takes item from queue and sends it to client
            using item_type = Input;
            Client*     cl_;
            ~Handler() noexcept = default;
            Handler(Client* cl) : cl_(cl) {}
            void operator()( item_type& it ) { cl_->on_input( it.payload_, it.info_ ); }
        };
        //
        using Queue    = Utility::LocalQueue<Handler>;
        using Map      = std::unordered_map<std::string, Info>;

    public:
        ~Feeder() noexcept { stop_(); }
        Feeder(StompAgent& agent, Client* client)
        : agent_(agent)
        , queue_(client)
        {}

        void subscribe( std::string const& topic, Info const& info, bool release = false )
        {
            subscribe( EndPoint{topic, false}, info, release );
        }

        void subscribe( EndPoint const& ep, Info const& info, bool release = false )
        {
            if ( ep.isQ_ )
            {
                qmap_.insert( {ep.dest_, info} );
            }
            else
            {
                tmap_.insert( {ep.dest_, info} );
            }
            agent_.subscribe( {ep.dest_, ep.isQ_}, make_callback( &Feeder::on_message, this ) );
        }

        void unsubscribe( EndPoint const& ep, bool release = false )
        {
            agent_.unsubscribe( {ep.dest_, ep.isQ_} );
            ep.isQ_ ? qmap_.erase( ep.dest_ ) : tmap_.erase( ep.dest_ );
        }

        //!> also the entry point for internal event queueing
        void on_message( std::string const& msg, EndPoint const& source )
        {
            Info    _info(source.isQ_ ? qmap_[source.dest_] : tmap_[source.dest_]);                
            queue_.put( {msg, _info} );
        }

    private:
        StompAgent&     agent_;
        Queue           queue_;
        Map             tmap_;
        Map             qmap_;

        void stop_() noexcept
        {
            for ( auto& _item : qmap_ ) { agent_.unsubscribe( {_item.first, true} ); }
            for ( auto& _item : tmap_ ) { agent_.unsubscribe( {_item.first, false} ); }
        }
    };

} // namespace Stomp

#endif // STOMP_FEEDER_H
