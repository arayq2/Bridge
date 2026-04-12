/** ======================================================================+
 + Copyright @2019-2025 Arjun Ray
 + Released under MIT License
 + see https://mit-license.org
 +========================================================================*/

#pragma once

#ifndef TIMERSYS_TIMERAGENT_H
#define TIMERSYS_TIMERAGENT_H

#include "nlohmann/json.hpp"

#include <functional>

    /**
     * @struct TimerAgent
     * @brief Callbacks for the Timer subsystem.
     */
    struct TimerAgent
    {
        using Callback = std::function<void(nlohmann::json&)>;
        Callback timeout;   // for inbound queue
        Callback inactive;  // to signal exit
    };

#endif // TIMERSYS_TIMERAGENT_H
