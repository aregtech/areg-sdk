/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/private/win32/NEUtilitiesWin32.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Windows OS specific hidden utility methods.
 *
 ************************************************************************/

#include "areg/base/NEUtilities.hpp"

#ifdef  WINDOWS

#include "areg/base/String.hpp"
#include "areg/base/NEMemory.hpp"

#include <chrono>
#include <time.h>
#include <Windows.h>

namespace NEUtilities
{
    uint64_t _osGetTickCount(void)
    {
        return static_cast<uint64_t>(::GetTickCount64());
    }

    TIME64 _osSystemTimeNow(void)
    {
        struct timespec ts { };
        return (std::timespec_get(&ts, TIME_UTC) != 0
                ? (static_cast<TIME64>(ts.tv_sec) * NEUtilities::SEC_TO_MICROSECS) + (static_cast<TIME64>(ts.tv_nsec) / NEUtilities::MICROSEC_TO_NS)
                : 0uLL);
    }

    void _osSystemTimeNow( NEUtilities::sSystemTime & OUT sysTime, bool localTime )
    {
        struct timespec ts { };
        struct tm now { };
        if (std::timespec_get(&ts, TIME_UTC) != 0)
        {
            if (localTime)
            {
                ::localtime_s(&now, &ts.tv_sec);
            }
            else
            {
                gmtime_s(&now, &ts.tv_sec);
            }

            unsigned short milli = static_cast<unsigned short>(static_cast<TIME64>(ts.tv_nsec) / MILLISEC_TO_NS);
            unsigned short micro = static_cast<unsigned short>(static_cast<TIME64>(ts.tv_nsec) % MILLISEC_TO_NS) / MICROSEC_TO_NS;

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
        localtime_s(&utcTime, &_timer);
    }

    bool _osConvToLocalTime(const TIME64& IN utcTime, sSystemTime& OUT localTime)
    {
        bool result = false;

        time_t secs;
        unsigned short milli, micro;
        NEUtilities::convMicrosecs(utcTime, secs, milli, micro);

        struct tm tmLocal { };
        if (RETURNED_OK == localtime_s(&tmLocal, &secs))
        {
            NEUtilities::convToSystemTime(tmLocal, localTime);
            localTime.stMillisecs = milli;
            localTime.stMicrosecs = micro;

            result = true;
        }

        return result;
    }

    bool _osConvToLocalTm(const TIME64& IN utcTime, struct tm & OUT localTm)
    {
        time_t secs = static_cast<time_t>(utcTime / NEUtilities::SEC_TO_MICROSECS);
        return (RETURNED_OK == localtime_s(&localTm, &secs));
    }

    void _osConvToSystemTime(const TIME64& IN timeValue, NEUtilities::sSystemTime& OUT sysTime)
    {
        time_t secs;
        unsigned short milli, micro;
        NEUtilities::convMicrosecs(timeValue, secs, milli, micro);

        struct tm gmt {};
        if (RETURNED_OK == ::gmtime_s(&gmt, &secs))
        {
            convToSystemTime(gmt, sysTime);
            sysTime.stMillisecs = milli;
            sysTime.stMicrosecs = micro;
        }
    }
    
    void _osConvToTm(const TIME64& IN timeValue, tm& OUT time)
    {
        time_t secs{ static_cast<time_t>(timeValue / NEUtilities::SEC_TO_MICROSECS) };
        ::gmtime_s(&time, &secs);
    }

} // namespace

#endif // WINDOWS
