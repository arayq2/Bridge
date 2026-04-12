/** ======================================================================+
 + Copyright @2019-2025 Arjun Ray
 + Released under MIT License
 + see https://mit-license.org
 +========================================================================*/

#include "Timer.h"
#include "DefaultKeys.h"

#include "Logging.h"

    using json = nlohmann::json;

#define ALL_TIMERSTATES(X) \
    X(NONE,=0,) \
    X(SET,,) \
    X(KILLED,,) \
    X(FIRED,,) \
    X(DONTFIRE,,)

#define ENUMDEFS(en,eq,...)     en eq,
    enum Timer::TimerState : unsigned int
    {
        ALL_TIMERSTATES(ENUMDEFS)
    };
#undef ENUMDEFS

//---------------------------------------------------------------------------
#define TS_STRINGS(en,...)  case TimerState::en : return #en;
    char const*
    Timer::get_timer_state_name_( Timer::Type etype ) const
    {
        if ( etype < Type::NUM_TIMERS )
        switch ( timerStates_[etype] )
        {
        ALL_TIMERSTATES(TS_STRINGS)
        default: break;
        }
        //
        return "Unknown";
    }
#undef TS_STRINGS

#define T_STRINGS(en,...)  case Type::en : return #en;
    char const*
    Timer::get_timer_name_( Timer::Type etype ) const
    {
        if ( etype < Type::NUM_TIMERS )
        switch ( etype )
        {
        ALL_TIMERS(T_STRINGS)
        default: break;
        }
        //
        return "Unknown";
    }
#undef T_STRINGS

    bool
    Timer::to_none_( Timer::Type type )
    {
        Guard   _gd(mx_);
        timerStates_[type] = Timer::TimerState::NONE;
        return true;
    }

    bool
    Timer::to_set_( Timer::Type type )
    {
        Guard   _gd(mx_);
        if ( timerStates_[type] != Timer::TimerState::SET )
        {
            timerStates_[type] = Timer::TimerState::SET;
            return true;
        }
        return false;
    }

    bool
    Timer::to_killed_( Timer::Type type )
    {
        Guard   _gd(mx_);
        if ( timerStates_[type] == Timer::TimerState::SET )
        {
            timerStates_[type] = Timer::TimerState::KILLED;
            return true;
        }
        return false;
    }

    bool
    Timer::to_fired_( Timer::Type type )
    {
        Guard   _gd(mx_);
        switch ( timerStates_[type] )
        {
        case Timer::TimerState::NONE:
        case Timer::TimerState::KILLED:
        case Timer::TimerState::DONTFIRE:
            return false;
        default: break;
        }
        timerStates_[type] = Timer::TimerState::FIRED;
        return true;
    }

    bool
    Timer::to_extend_( Timer::Type type )
    {
        Guard   _gd(mx_);
        if ( timerStates_[type] == Timer::TimerState::SET )
        {
            timerStates_[type] = Timer::TimerState::DONTFIRE;
            return true;
        }
        return false;
    }

//---------------------------------------------------------------------------
    void
    Timer::set( Timer::Type type )
    {
        if ( replay_ ) { replay_set_( type ); return; }
        if ( type < Type::NUM_TIMERS && to_set_( type ) )
        {
            long _ms = timers_[type].set( type );
            if ( _ms <= 0 )
            {
                LOG_FMT_INFO(nullptr, "Request to set [%s] timer not done - default time is 0 ms", get_timer_name_( type ));
                to_none_( type );
            }
            else
            {
                if (type != Timer::Type::ACTIVITY) // too verbose if we include ACTIVITY timer in logs
                    LOG_FMT_INFO(nullptr, "[%s] timer set for [%ld] ms", get_timer_name_( type ), _ms);
            }
        }
        else
        {
            LOG_FMT_WARN(nullptr, "Request to set [%s] timer ignored - was already set", get_timer_name_( type ));
        }
    }

    void
    Timer::set( Timer::Type type, unsigned int millis )
    {
        if ( replay_ ) { replay_set_( type, millis ); return; }
        if ( type < Type::NUM_TIMERS && to_set_( type ) )
        {
            timers_[type].set( type, millis );
            if (type != Timer::Type::ACTIVITY) // too verbose if we include ACTIVITY timer in logs
                LOG_FMT_INFO(nullptr, "[%s] timer set for [%ld] ms", get_timer_name_( type ), millis);
        }
    }

    void
    Timer::kill( Timer::Type type )
    {
        if ( replay_ ) { replay_kill_( type ); return; }
        if ( type < Type::NUM_TIMERS && to_killed_( type ) )
        {
            timers_[type].cancel();
            if (type != Timer::Type::ACTIVITY) // too verbose if we include ACTIVITY timer in logs
                LOG_FMT_INFO(nullptr, "[%s] timer killed", get_timer_name_( type ));
        }
    }

    bool
    Timer::extend( Timer::Type type, unsigned int millis )
    {
        if ( replay_ ) { return replay_extend_( type, millis ); }
        if ( type < Type::NUM_TIMERS && to_extend_( type ) )
        {
            auto _newTime = timers_[type].extend( millis );
            if ( _newTime > 0 )
            {
                LOG_FMT_INFO(nullptr, "[%s] timer extended by [%d] ms. Time left = [%ld] ms",
                    get_timer_name_( type ), millis, _newTime);
                to_set_( type ); // dismiss temporary freeze
                return true;
            }
        }
        LOG_FMT_INFO(nullptr, "Request to add time to [%s] timer ignored - was in [%s] state",
            get_timer_name_( type ), get_timer_state_name_( type ));
        return false;
    }

    bool
    Timer::extend( Timer::Type type )
    {
        if ( replay_ ) { return replay_extend_( type, extra_[type] ); }
        return extend( type, extra_[type] );
    }

    long
    Timer::time_left( Timer::Type type )
    {
        if ( replay_ ) { return replay_time_left_( type ); }
        if ( type < Type::NUM_TIMERS ) { return timers_[type].timeleft(); }
        return 0;
    }

    void
    Timer::processed( Timer::Type type )
    {
        to_none_( type );
    }

    Timer&
    Timer::reinit( Timer::Type type, unsigned int millis )
    {
        if ( type < Type::NUM_TIMERS ) { timers_[type].reinit( millis ); }
        return *this;
    }

    Timer&
    Timer::extra( Timer::Type type, unsigned int millis )
    {
        if ( type < Type::NUM_TIMERS ) { extra_[type] = millis; }
        return *this;
    }

    void
    Timer::on_timeout_( unsigned int& type )
    {
        Timer::Type     _type(static_cast<Timer::Type>(type));
        if ( !to_fired_( _type ) )
        {
            LOG_FMT_WARN(nullptr, "[%s] timer not in SET state, cannot fire", get_timer_name_( _type ));
            return;
        }
        json            _expiry;
        _expiry[KEY_TYPE]  = TYPE_TIMER;
        _expiry[KEY_TIMER] = type;

        switch ( type )
        {
        case Type::ACTIVITY: _expiry[KEY_ACTION] = TTimer::Actions::TO_ACTIVITY; // special handling
            _expiry[TYPE_EXTERNAL] = false;
            aptr_->inactive( _expiry );
            return;
        case Type::CLAIM   : _expiry[KEY_ACTION] = TTimer::Actions::TO_CLAIM; break;
        case Type::UNDO    : _expiry[KEY_ACTION] = TTimer::Actions::TO_UNDO; break;
        case Type::PHASE   : _expiry[KEY_ACTION] = TTimer::Actions::TO_PHASE; break;
        case Type::TRICK   : _expiry[KEY_ACTION] = TTimer::Actions::TO_TRICK; break;
        case Type::ROUND   : _expiry[KEY_ACTION] = TTimer::Actions::TO_ROUND; break;
        case Type::NBCO    : _expiry[KEY_ACTION] = TTimer::Actions::TO_NBCO; break;
        case Type::SLOW    : _expiry[KEY_ACTION] = TTimer::Actions::TO_SLOW; break;
        default: break;
        }
        LOG_FMT_INFO( nullptr, "[%s] timer expired", get_timer_name_( _type ) );
        aptr_->timeout( _expiry );
    }

//---------------------------------------------------------------------------
    Timer::Timer(TimerAgent* aptr)
    : aptr_(aptr)
    , alarm_(Notifier(this))
    , timerStates_{TimerState::NONE}
    , extra_{0}
    {
        timers_.reserve( Type::NUM_TIMERS );
        for ( unsigned int _i{0}; _i < NUM_TIMERS; ++_i )
        {
            timers_.emplace_back( alarm_ );
        }
    }

    Timer&
    Timer::restart()
    {
        uint _timer{0};
        for ( auto& _timerState : timerStates_ )
        {
            if ( _timer != Type::ACTIVITY ) // ACTIVITY handled elsewhere
            {
                Timer::Type _type(static_cast<Timer::Type>(_timer));
                if ( _timerState == Timer::TimerState::SET )
                {
                    LOG_FMT_INFO(nullptr, "Restarting [%s] timer", get_timer_name_( _type ));
                    timers_[_timer].set( _type );
                }
                else
                if ( _timerState == Timer::TimerState::FIRED )
                {
                    LOG_FMT_INFO(nullptr, "Refiring (suspended) [%s] timer", get_timer_name_( _type ));
                    on_timeout_( _timer ); // simulate the event
                }
                else
                {
                    _timerState = Timer::TimerState::NONE;
                }
            }
            _timer++;
        }
        return *this;
    }

    Timer&
    Timer::reset_all()
    {
        uint _timer{0};
        for ( auto& _timerState : timerStates_ )
        {
            if ( _timer != Type::ACTIVITY ) // ACTIVITY handled elsewhere
            {
                Timer::Type _type(static_cast<Timer::Type>(_timer));
                if ( _timerState == TimerState::SET ) { kill( _type ); }
                _timerState = Timer::TimerState::NONE;
            }
            _timer++;
        }
        return *this;
    }

//---------------------------------------------------------------------------
    Timer&
    Timer::set_replay( bool replay )
    {
        replay_ = replay;
        // set states?
        return *this;
    }

    void
    Timer::replay_fired( unsigned int type )
    {
        if ( replay_ && type < Type::NUM_TIMERS )
        {
            timerStates_[type] = Timer::TimerState::FIRED;
        }
    }

    void
    Timer::replay_set_( Type type )
    {
        if ( timerStates_[type] != Timer::TimerState::SET )
        {
            timerStates_[type] = Timer::TimerState::SET;
        }
    }

    void
    Timer::replay_set_( Type type, unsigned int millis )
    {
        if ( timerStates_[type] != Timer::TimerState::SET )
        {
            timerStates_[type] = Timer::TimerState::SET;
        }
    }

    void
    Timer::replay_kill_( Type type )
    {
        if ( timerStates_[type] == Timer::TimerState::SET )
        {
            timerStates_[type] = Timer::TimerState::KILLED;
        }
    }

    bool
    Timer::replay_extend_( Type type, unsigned int millis )
    {
        return extra_[type] != 0;
    }

    long
    Timer::replay_time_left_( Type type )
    {
        return timerStates_[type] == Timer::TimerState::SET ? 1000L : 0L;
    }

//---------------------------------------------------------------------------
    void
    Timer::to_js( json& js ) const
    {
        js["states"] = timerStates_;
    }

    void
    Timer::fr_js( json const& js )
    {
        js["states"].get_to( timerStates_ );
    }

    void
    Timer::show_states( json& js ) const
    {
        js = timerStates_;
    }

//===========================================================================
    Timer::Restarter::~Restarter() noexcept
    {
        timer_.set( item_ );
    }

    Timer::Restarter::Restarter(Timer& timer)
    : timer_(timer)
    , item_(Timer::Type::ACTIVITY)
    {
        timer_.kill( item_ );
    }
