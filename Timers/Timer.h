/** ======================================================================+
 + Copyright @2019-2025 Arjun Ray
 + Released under MIT License
 + see https://mit-license.org
 +========================================================================*/

#pragma once

#ifndef TIMERSYS_TIMER_H
#define TIMERSYS_TIMER_H

#include "Scheduler.h"
#include "Timers.h"
#include "TimerAgent.h"

#include <vector>
#include <mutex>

    /**
     * @class Timer
     * @brief Timer subsystem.
     * Uses TimerAgent callbacks to inform about fired timers.
     * In replay mode, timers are not set, only simulated.
     */
    class Timer
    {
    private:
        struct Notifier
        {
            using item_type = unsigned int;
            Timer*      timer_;
            ~Notifier() noexcept = default;
            Notifier(Timer* timer) : timer_(timer) {}
            void operator()( item_type& item ){ timer_->on_timeout_( item ); }
        };
        friend struct Notifier;
        //
        using Alarm  = Utility::Scheduler<Notifier>;
        using Swatch = typename Alarm::TimeOut; // Stopwatch
        using Timers = std::vector<Swatch>;
        //
        using Mutex  = std::mutex;
        using Guard  = std::lock_guard<Mutex>;
        //
        enum TimerState : unsigned int;

    public:
#define ENUMDEFS(en,eq,...)   en eq,
        enum Type : unsigned int
        {
            ALL_TIMERS(ENUMDEFS)
            NUM_TIMERS /* number of timers - must be last */
        };
#undef ENUMDEFS
        //
        ~Timer() noexcept = default;
        explicit Timer(TimerAgent*);

        void set( Type type );
        void set( Type type, unsigned int millis );
        void kill( Type type );
        bool extend( Type type, unsigned int millis );
        bool extend( Type type ); // use default setting
        long time_left( Type type );
        void processed( Type type );
        //
        Timer& reinit( Type type, unsigned int millis );
        Timer& extra( Type type, unsigned int millis ); // extra time, if allowed
        Timer& restart();
        Timer& reset_all(); // for new deal
        Timer& set_replay( bool replay );
        void replay_fired( unsigned int ); // only in replay mode
        //
        void to_js( nlohmann::json& js ) const;
        void fr_js( nlohmann::json const& js );
        void show_states( nlohmann::json& js ) const;
        //
        class Restarter
        {
        public:
            ~Restarter() noexcept;
            explicit Restarter(Timer&);

        private:
            Timer&      timer_;
            Timer::Type item_;  // ACTIVITY
        };

    private:
        Mutex       mx_;     // to protect timerStates_
        TimerAgent* aptr_;
        Alarm       alarm_;
        TimerState  timerStates_[Type::NUM_TIMERS];
        unsigned    extra_[Type::NUM_TIMERS]; // extra time allowed
        Timers      timers_;
        bool        replay_{false};
        //
        void on_timeout_( unsigned int& ); // called by Notifier
        char const* get_timer_name_( Type ) const;
        char const* get_timer_state_name_( Type ) const;
        //
        bool to_none_( Type );
        bool to_set_( Type );
        bool to_killed_( Type );
        bool to_fired_( Type );
        bool to_extend_( Type );
        //
        void replay_set_( Type type );
        void replay_set_( Type type, unsigned int millis );
        void replay_kill_( Type type );
        bool replay_extend_( Type type, unsigned int millis );
        long replay_time_left_( Type type );
    };

    inline
    void to_json( nlohmann::json& j, Timer const& timer ) { timer.to_js( j ); }
    inline
    void from_json( nlohmann::json const& j, Timer& timer ) { timer.fr_js( j ); }

#endif // TIMERSYS_TIMER_H
