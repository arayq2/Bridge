/** ======================================================================+
 + Copyright @2019-2025 Arjun Ray
 + Released under MIT License
 + see https://mit-license.org
 +========================================================================*/

#ifndef TIMERSYS_TIMERS_H
#define TIMERSYS_TIMERS_H

#define ALL_TIMERS(X)   \
    X(ACTIVITY,=0,19) \
    X(CLAIM,,7) \
    X(UNDO,,9) \
    X(PHASE,,11) \
    X(TRICK,,15) \
    X(ROUND,,13) \
    X(NBCO,,17) \
    X(SLOW,,16)

namespace TTimer
{ 
#define ENUM_TOS(en,eq,to,...)  TO_##en = to,
    enum Actions = int
    {
        ALL_TIMERS(ENUM_TOS)
    };
#undef ENUM_TOS
}

#define DFLTVALS()

#undef DFLTVALS

// fallback hardcoded values
#define TIME_ACTIVITY   1'800'000
#define TIME_CLAIMTO    45000
#define TIME_UNDOTO     15000
#define TIME_PHASE      3000
#define TIME_PERBOARD   420000
#define TIME_NBCO       180000
#define TIME_CLAIMEXT   5000
#define TIME_RDEXTRA    120000
//#define TIME_RDSLACK    60000

#endif // TIMERSYS_TIMERS_H
