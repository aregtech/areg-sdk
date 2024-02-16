/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/private/NEUtilitiesPosix.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG POSIX specific hidden utility methods.
 *
 ************************************************************************/

#include "areg/base/NEUtilities.hpp"

#if defined(_POSIX) || defined(POSIX)

#include "areg/base/String.hpp"
#include "areg/base/NEMemory.hpp"
#include "areg/base/GEMacros.h"

#include <chrono>
#include <time.h>
#include <unistd.h>

namespace NEUtilities
{
    uint64_t _osGetTickCount( void )
    {
        struct timespec ts;
        ::clock_gettime( CLOCK_MONOTONIC, &ts );

        TIME64 result = (ts.tv_sec * SEC_TO_MILLISECS) + ts.tv_nsec / MILLISEC_TO_NS;
        return result;
    }

    TIME64 _osSystemTimeNow(void)
    {
        struct timespec ts { 0 };
        return (RETURNED_OK == ::clock_gettime(CLOCK_REALTIME, &ts)
                ? static_cast<TIME64>((ts.tv_sec * NEUtilities::SEC_TO_MICROSECS) + (ts.tv_nsec / NEUtilities::MICROSEC_TO_NS))
                : 0LL);
    }

    void _osSystemTimeNow( NEUtilities::sSystemTime & OUT sysTime, bool localTime )
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

            unsigned short milli = static_cast<unsigned short>((ts.tv_nsec / MILLISEC_TO_NS));
            unsigned short micro = static_cast<unsigned short>((ts.tv_nsec % MILLISEC_TO_NS) / MICROSEC_TO_NS);
            ASSERT(milli < 1000);
            ASSERT(micro < 1000);

            NEUtilities::convToSystemTime(now, sysTime);
            sysTime.stMillisecs = milli;
            sysTime.stMicrosecs = micro;
        }
    }

    void _osMakeTmLocal(struct tm& IN OUT utcTime)
    {
        NEMemory::memSet(&utcTime, sizeof(struct tm), 0);
        time_t _timer = mktime(&utcTime);
        localtime_r(&_timer, &utcTime);
    }

    bool _osConvToLocalTime(const TIME64& IN utcTime, sSystemTime& OUT localTime)
    {
        bool result = false;

        time_t secs;
        unsigned short milli, micro;
        NEUtilities::convMicrosecs(utcTime, secs, milli, micro);

        struct tm tmLocal { 0 };
        if (::localtime_r(&secs, &tmLocal) != nullptr)
        {
            NEUtilities::convToSystemTime(tmLocal, localTime);
            localTime.stMillisecs = milli;
            localTime.stMicrosecs = micro;

            result = true;
        }

        return result;
    }

    bool _osConvToLocalTm(const TIME64& IN utcTime, struct tm& OUT localTm)
    {
        time_t secs = static_cast<time_t>(utcTime / NEUtilities::SEC_TO_MICROSECS);
        return (::localtime_r(&secs, &localTm) != nullptr);
    }

    void _osConvToSystemTime(const TIME64& IN timeValue, NEUtilities::sSystemTime& OUT sysTime)
    {
        time_t secs;
        unsigned short milli, micro;
        NEUtilities::convMicrosecs(timeValue, secs, milli, micro);

        struct tm gmt {};
        if (::gmtime_r(&secs, &gmt) != nullptr)
        {
            convToSystemTime(gmt, sysTime);
            sysTime.stMillisecs = milli;
            sysTime.stMicrosecs = micro;
        }
    }

    void _osConvToTm(const TIME64& IN timeValue, tm& OUT time)
    {
        time_t secs{ static_cast<time_t>(timeValue / NEUtilities::SEC_TO_MICROSECS) };
        ::gmtime_r(&secs, &time);
    }

} // namespace

#endif // defined(_POSIX) || defined(POSIX)
