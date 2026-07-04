/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/base/private/win32/UtilityDefsWin32.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Windows OS specific hidden utility methods.
 *
 ************************************************************************/

#ifdef  _WIN32

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/UtilityDefs.hpp"

#include <time.h>

namespace areg::os {

TIME64 _os_system_time_now()
{
    struct timespec ts { };
#ifndef _MINGW
    return (timespec_get(&ts, TIME_UTC) != 0
            ? (static_cast<TIME64>(ts.tv_sec) * areg::SEC_TO_MICROSECS) + (static_cast<TIME64>(ts.tv_nsec) / areg::MICROSEC_TO_NS)
            : 0uLL);
#else   // _MINGW
    return (areg::RETURNED_OK == ::clock_gettime(CLOCK_REALTIME, &ts)
                ? static_cast<TIME64>((ts.tv_sec * areg::SEC_TO_MICROSECS) + (ts.tv_nsec / areg::MICROSEC_TO_NS))
                : 0LL);
#endif  // _MINGW
}

void _os_system_time_now( areg::CalendarTime & sysTime, bool localTime )
{
    struct timespec ts { };
    struct tm now { };
#ifndef _MINGW
    if (timespec_get(&ts, TIME_UTC) != 0)
#else   // _MINGW
    if (areg::RETURNED_OK == ::clock_gettime( CLOCK_REALTIME, &ts ))
#endif  // _MINGW
    {
        if (localTime)
        {
            ::localtime_s(&now, &ts.tv_sec);
        }
        else
        {
            gmtime_s(&now, &ts.tv_sec);
        }

        uint16_t milli = static_cast<uint16_t>(static_cast<TIME64>(ts.tv_nsec) / MILLISEC_TO_NS);
        uint16_t micro = static_cast<uint16_t>(static_cast<TIME64>(ts.tv_nsec) % MILLISEC_TO_NS) / MICROSEC_TO_NS;

        ASSERT(milli < 1000);
        ASSERT(micro < 1000);

        to_system_time(now, sysTime);
        sysTime.stMillisecs = milli;
        sysTime.stMicrosecs = micro;
    }
}

} // namespace areg::os

#endif // _WIN32
