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


namespace Stomp
{
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
        ~PublisherImpl() noexcept = default;

        PublisherImpl(StompAgent& agent, EndPoint const& target)
        : agent_(&agent)
        , target_(target)
        {}

        PublisherImpl(StompAgent& agent, char const* target)
        : agent_(&agent)
        , target_{target, ISQUEUE}
        {}

        void publish( nlohmann::json const& js ) override
        {
            publish( target_, js );
        }
        void publish( EndPoint const& dest, nlohmann::json const& js ) override
        {
            agent_->publish( dest, js.dump() );
        }
        //
        void publish( std::string const& msg ) override
        {
            publish( target_, msg );
        }
        void publish( EndPoint const& dest, std::string const& msg ) override
        {
            agent_->publish( dest, msg );
        }

        void publish_once( EndPoint const& dest, nlohmann::json const& js ) override
        {
            publish_once( dest, js.dump() );
        }
        void publish_once( EndPoint const& dest, std::string const& msg ) override
        {
            agent_->publish( dest, msg );
        }

        void set_target( std::string const& target, bool isqueue = ISQUEUE )
        {
            target_ = {target, isqueue};
        }

        // for implementation needs
        void purge( EndPoint const& dest, bool keep = false )
        {
            //agent_->purge( dest, keep );
        }

    private:
        StompAgent* agent_{nullptr};
        EndPoint    target_; // default destination
    };

    using QPublisher = PublisherImpl<true>;
    using TPublisher = PublisherImpl<false>;

} // namespace Stomp

#endif // STOMP_PUBLISHERIMPL_H
