/** ======================================================================+
 + Copyright @2023-2024 Arjun Ray
 + Released under MIT License
 + see https://mit-license.org
 +========================================================================*/
#pragma once

#ifndef STOMP_PUBLISHERIMPL_H
#define STOMP_PUBLISHERIMPL_H

#include "Publisher.h"
#include "StompAgent.h"

#include <utility>

    /**
     * @class PublisherImpl
     * @brief Implementation of Publisher interface using Stomp::StompAgent.
     *        Convenience typedefs of QPublisher and TPublisher for default target types.
     */
    template<bool ISQUEUE = false>
    class PublisherImpl
    : public Publisher
    {
    public:
        ~PublisherImpl() noexcept { if ( purge_ ) { agent_->purge( target_ ); } }

        PublisherImpl(Stomp::StompAgent& agent, Stomp::EndPoint const& target, bool purgeWhenDone = false)
        : agent_(&agent)
        , target_(target)
        , purge_(purgeWhenDone)
        {}

        PublisherImpl(Stomp::StompAgent& agent, char const* target, bool purgeWhenDone = false)
        : agent_(&agent)
        , target_{target, ISQUEUE}
        , purge_(purgeWhenDone)
        {}

        void publish( nlohmann::json const& js ) override
        {
            publish( target_, js );
        }
        void publish( Stomp::EndPoint const& dest, nlohmann::json const& js ) override
        {
            agent_->publish( dest, js.dump() );
        }
        //
        void publish( std::string const& msg ) override
        {
            publish( target_, msg );
        }
        void publish( Stomp::EndPoint const& dest, std::string const& msg ) override
        {
            agent_->publish( dest, msg );
        }

        void publish_once( Stomp::EndPoint const& dest, nlohmann::json const& js ) override
        {
            publish_once( dest, js.dump() );
        }
        void publish_once( Stomp::EndPoint const& dest, std::string const& msg ) override
        {
            agent_->publish( dest, msg );
        }

        void set_target( std::string const& target, bool isqueue = ISQUEUE )
        {
            target_ = {target, isqueue};
        }

        // for implementation needs
        void purge( Stomp::EndPoint const& dest, bool keep = false )
        {
            agent_->purge( dest, keep );
        }

    private:
        Stomp::StompAgent*  agent_{nullptr};
        Stomp::EndPoint     target_; // default destination
        bool                purge_{false};
    };

    using QPublisher = PublisherImpl<true>;
    using TPublisher = PublisherImpl<false>;

#endif // STOMP_PUBLISHERIMPL_H
