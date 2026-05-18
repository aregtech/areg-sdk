/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/UtilityDefsPosix.cpp
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
#include "areg/base/String.hpp"
#include "areg/base/MemoryDefs.hpp"
#include "areg/base/areg_macros.h"

#include <chrono>
#include <time.h>
#include <unistd.h>

namespace areg::os {

uint64_t _os_tick_count()
{
    struct timespec ts;
    ::clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts);
    return ((ts.tv_sec * SEC_TO_MILLISECS) + (ts.tv_nsec / MILLISEC_TO_NS));
}

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

void _os_make_tm_local(struct tm& utcTime)
{
    areg::mem_set(&utcTime, sizeof(struct tm), 0);
    time_t _timer = mktime(&utcTime);
    localtime_r(&_timer, &utcTime);
}

bool _os_to_local_time(const TIME64& utcTime, CalendarTime& localTime)
{
    bool result = false;

    time_t secs;
    uint16_t milli, micro;
    conv_microsecs(utcTime, secs, milli, micro);

    struct tm tmLocal { 0 };
    if (::localtime_r(&secs, &tmLocal) != nullptr)
    {
        to_system_time(tmLocal, localTime);
        localTime.stMillisecs = milli;
        localTime.stMicrosecs = micro;

        result = true;
    }

    return result;
}

bool _os_to_local_tm(const TIME64& utcTime, struct tm& localTm)
{
    time_t secs = static_cast<time_t>(utcTime / SEC_TO_MICROSECS);
    return (::localtime_r(&secs, &localTm) != nullptr);
}

void _osto_system_time(const TIME64& timeValue, CalendarTime& sysTime)
{
    time_t secs;
    uint16_t milli{ 0 };
    uint16_t micro{ 0 };
    conv_microsecs(timeValue, secs, milli, micro);

    struct tm gmt {};
    if (::gmtime_r(&secs, &gmt) != nullptr)
    {
        to_system_time(gmt, sysTime);
        sysTime.stMillisecs = milli;
        sysTime.stMicrosecs = micro;
    }
}

void _os_to_tm(const TIME64& timeValue, tm& time)
{
    time_t secs{ static_cast<time_t>(timeValue / SEC_TO_MICROSECS) };
    ::gmtime_r(&secs, &time);
}

} // namespace areg::os

#endif // defined(_POSIX) || defined(POSIX)
