/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/win32/UtilityDefsWin32.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Windows OS specific hidden utility methods.
 *
 ************************************************************************/

#include "areg/base/UtilityDefs.hpp"

#ifdef  _WIN32

#include "areg/base/String.hpp"
#include "areg/base/MemoryDefs.hpp"

#ifndef NOMINMAX
    #define NOMINMAX
#endif // !NOMINMAX
#include <chrono>
#include <time.h>
#include <Windows.h>

namespace areg
{
    uint64_t _osGetTickCount()
    {
        return static_cast<uint64_t>(::GetTickCount64());
    }

    TIME64 _osSystemTimeNow()
    {
        struct timespec ts { };
#ifndef _MINGW
        return (timespec_get(&ts, TIME_UTC) != 0
                ? (static_cast<TIME64>(ts.tv_sec) * SEC_TO_MICROSECS) + (static_cast<TIME64>(ts.tv_nsec) / MICROSEC_TO_NS)
                : 0uLL);
#else   // _MINGW
        return (RETURNED_OK == ::clock_gettime(CLOCK_REALTIME, &ts)
                    ? static_cast<TIME64>((ts.tv_sec * SEC_TO_MICROSECS) + (ts.tv_nsec / MICROSEC_TO_NS))
                    : 0LL);
#endif  // _MINGW
    }

    void _osSystemTimeNow( CalendarTime & sysTime, bool localTime )
    {
        struct timespec ts { };
        struct tm now { };
#ifndef _MINGW
        if (timespec_get(&ts, TIME_UTC) != 0)
#else   // _MINGW
        if (RETURNED_OK == ::clock_gettime( CLOCK_REALTIME, &ts ))
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

            convToSystemTime(now, sysTime);
            sysTime.stMillisecs = milli;
            sysTime.stMicrosecs = micro;
        }
    }

    void _osMakeTmLocal(struct tm& utcTime)
    {
        memSet(&utcTime, sizeof(struct tm), 0);
        time_t _timer = mktime(&utcTime);
        localtime_s(&utcTime, &_timer);
    }

    bool _osConvToLocalTime(const TIME64& utcTime, CalendarTime& localTime)
    {
        bool result = false;

        time_t secs;
        uint16_t milli, micro;
        convMicrosecs(utcTime, secs, milli, micro);

        struct tm tmLocal { };
        if (RETURNED_OK == localtime_s(&tmLocal, &secs))
        {
            convToSystemTime(tmLocal, localTime);
            localTime.stMillisecs = milli;
            localTime.stMicrosecs = micro;

            result = true;
        }

        return result;
    }

    bool _osConvToLocalTm(const TIME64& utcTime, struct tm & localTm)
    {
        time_t secs = static_cast<time_t>(utcTime / SEC_TO_MICROSECS);
        return (RETURNED_OK == localtime_s(&localTm, &secs));
    }

    void _osConvToSystemTime(const TIME64& timeValue, CalendarTime& sysTime)
    {
        time_t secs;
        uint16_t milli, micro;
        convMicrosecs(timeValue, secs, milli, micro);

        struct tm gmt {};
        if (RETURNED_OK == ::gmtime_s(&gmt, &secs))
        {
            convToSystemTime(gmt, sysTime);
            sysTime.stMillisecs = milli;
            sysTime.stMicrosecs = micro;
        }
    }
    
    void _osConvToTm(const TIME64& timeValue, tm& time)
    {
        time_t secs{ static_cast<time_t>(timeValue / SEC_TO_MICROSECS) };
        ::gmtime_s(&time, &secs);
    }

} // namespace

#endif // _WIN32
