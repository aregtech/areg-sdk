/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/UtilityDefsPosix.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg POSIX specific hidden utility methods.
 *
 ************************************************************************/

#if defined(_POSIX) || defined(POSIX)

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/UtilityDefs.hpp"
#include "areg/base/CommonDefs.hpp"
#include "areg/base/areg_macros.h"

#include <time.h>

namespace areg::os {

TIME64 _os_system_time_now()
{
    struct timespec ts { 0 };
    return (areg::RETURNED_OK == ::clock_gettime(CLOCK_REALTIME, &ts)
            ? static_cast<TIME64>((ts.tv_sec * areg::SEC_TO_MICROSECS) + (ts.tv_nsec / areg::MICROSEC_TO_NS))
            : 0LL);
}

void _os_system_time_now( areg::CalendarTime & sysTime, bool localTime )
{
    struct timespec ts { 0 };
    struct tm now { 0 };

    if (RETURNED_OK == ::clock_gettime( CLOCK_REALTIME, &ts ))
    {
        if ( localTime )
        {
            ::localtime_r( &ts.tv_sec, &now );
        }
        else
        {
            ::gmtime_r( &ts.tv_sec, &now );
        }

        uint16_t milli = static_cast<uint16_t>((ts.tv_nsec / MILLISEC_TO_NS));
        uint16_t micro = static_cast<uint16_t>((ts.tv_nsec % MILLISEC_TO_NS) / MICROSEC_TO_NS);
        ASSERT(milli < 1000);
        ASSERT(micro < 1000);

        to_system_time(now, sysTime);
        sysTime.stMillisecs = milli;
        sysTime.stMicrosecs = micro;
    }
}

} // namespace areg::os

#endif // defined(_POSIX) || defined(POSIX)
