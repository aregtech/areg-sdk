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

#include <Windows.h>
#include <chrono>

namespace NEUtilities {

    /**
     * \brief   Generates a name, sets the timestamp of now.
     *
     * \param   prefix          The prefix to add to name.
     * \param   out_buffer[out] On output, this contains the result.
     * \param   length          The length of output buffer.
     * \param   specChar        Special character to add when generating name.
     * \return Generated name.
     **/
    const char * _osGenerateName( const char * prefix, char * out_buffer, int length, const char * specChar )
    {
        constexpr char const strFormat[] { "%s%s%08x%s%08x" };

        if ( out_buffer != nullptr )
        {
            *out_buffer = '\0';
            const char * spec = specChar != nullptr ? specChar : NECommon::DEFAULT_SPECIAL_CHAR.data();
            NEMath::uLargeInteger time{};
            auto now{ std::chrono::high_resolution_clock::now().time_since_epoch() };
            time.quadPart = std::chrono::duration_cast<std::chrono::microseconds>(now).count();

            String::formatString( out_buffer, length, strFormat
                                  , prefix != nullptr ? prefix : NEUtilities::DEFAULT_GENERATED_NAME.data()
                                  , spec
                                  , time.u.highPart
                                  , spec
                                  , time.u.lowPart);
        }

        return out_buffer;
    }

    //!< Converts Win system time to the areg specific time structure.
    inline void _convWinSysTime2AregSysTime( const SYSTEMTIME & IN winTime, NEUtilities::sSystemTime & OUT aregTime )
    {
        aregTime.stYear         = static_cast<int>(winTime.wYear);
        aregTime.stMonth        = static_cast<int>(winTime.wMonth);
        aregTime.stDayOfWeek    = static_cast<int>(winTime.wDayOfWeek);
        aregTime.stDay          = static_cast<int>(winTime.wDay);
        aregTime.stHour         = static_cast<int>(winTime.wHour);
        aregTime.stMinute       = static_cast<int>(winTime.wMinute);
        aregTime.stSecond       = static_cast<int>(winTime.wSecond);
        aregTime.stMillisecs    = static_cast<int>(winTime.wMilliseconds);
        aregTime.stMicrosecs    = 0;
    }

    //!< Converts areg specific time structure to Windows system time.
    inline void _convAregSysTime2WinSysTime( const NEUtilities::sSystemTime & IN aregTime, SYSTEMTIME & OUT winTime )
    {
        winTime.wYear           = static_cast<WORD>(aregTime.stYear);
        winTime.wMonth          = static_cast<WORD>(aregTime.stMonth);
        winTime.wDayOfWeek      = static_cast<WORD>(aregTime.stDayOfWeek);
        winTime.wDay            = static_cast<WORD>(aregTime.stDay);
        winTime.wHour           = static_cast<WORD>(aregTime.stHour);
        winTime.wMinute         = static_cast<WORD>(aregTime.stMinute);
        winTime.wSecond         = static_cast<WORD>(aregTime.stSecond);
        winTime.wMilliseconds   = static_cast<WORD>(aregTime.stMillisecs);
    }

    uint64_t _osGetTickCount( void )
    {
        return static_cast<uint64_t>(::GetTickCount64( ));
    }


    bool _osConvToLocalTime( const sSystemTime & inUtcTime, sSystemTime & outLocalTime )
    {
        bool result = false;

        TIME_ZONE_INFORMATION tzi{ 0 };
        GetTimeZoneInformation( &tzi );
        SYSTEMTIME utc{ 0 };
        SYSTEMTIME local{ 0 };

        _convAregSysTime2WinSysTime( inUtcTime, utc );
        if ( SystemTimeToTzSpecificLocalTime( &tzi, &utc, &local ) )
        {
            _convWinSysTime2AregSysTime( local, outLocalTime );
            outLocalTime.stMicrosecs = inUtcTime.stMicrosecs;
            result = true;
        }
        else
        {
            _convWinSysTime2AregSysTime( utc, outLocalTime );
            outLocalTime.stMicrosecs = inUtcTime.stMicrosecs;
        }

        return result;
    }

    bool _osConvToLocalTime( const TIME64 & inUtcTime, NEUtilities::sSystemTime & outLocalTime )
    {
        NEUtilities::sSystemTime utcTime;
        convToSystemTime( inUtcTime, utcTime );
        return convToLocalTime( utcTime, outLocalTime );
    }


    void _osSystemTimeNow( NEUtilities::sSystemTime & OUT sysTime, bool localTime )
    {
        struct timespec ts { 0 };
        struct tm now { 0 };
        std::timespec_get(&ts, TIME_UTC);
        if (localTime)
        {
            ::localtime_s(&now, &ts.tv_sec);
        }
        else
        {
            gmtime_s(&now, &ts.tv_sec);
        }

        unsigned short milli = static_cast<unsigned short>((ts.tv_nsec / MILLISEC_TO_NS));
        unsigned short micro = static_cast<unsigned short>((ts.tv_nsec % MILLISEC_TO_NS) / MICROSEC_TO_NS);

        ASSERT(milli < 1000);
        ASSERT(micro < 1000);

        sysTime.stYear = now.tm_year + 1900;
        sysTime.stMonth = now.tm_mon + 1;
        sysTime.stDayOfWeek = now.tm_wday;
        sysTime.stDay = now.tm_mday;
        sysTime.stHour = now.tm_hour;
        sysTime.stMinute = now.tm_min;
        sysTime.stSecond = now.tm_sec;
        sysTime.stMillisecs = milli;
        sysTime.stMicrosecs = micro;
        sysTime.stTimeType = localTime ? NEUtilities::eTime::TimeLocal : NEUtilities::eTime::TimeUtc;
    }

    TIME64 _osConvToTime( const NEUtilities::sSystemTime & IN sysTime )
    {
        struct tm gmt {};
        NEUtilities::convToTm(sysTime, gmt);
        std::chrono::seconds secs{ ::mktime(&gmt) };
        std::chrono::microseconds ums{ sysTime.stMillisecs * NEUtilities::MILLISEC_TO_MICROSECS + sysTime.stMicrosecs };
        return (ums + secs).count();
    }

    void _osConvToSystemTime( const TIME64 & IN timeValue, NEUtilities::sSystemTime & OUT sysTime )
    {
        std::chrono::microseconds ums{ timeValue };
        std::chrono::seconds secs{ std::chrono::duration_cast<std::chrono::seconds>(ums) };
        std::chrono::microseconds remain{ ums - secs };
        time_t tmSecs{ secs.count() };

        struct tm gmt {};
        if (::gmtime_s(&gmt, &tmSecs) == 0)
        {
            convToSystemTime(gmt, sysTime);
            sysTime.stMillisecs = static_cast<int32_t>(remain.count() / NEUtilities::MILLISEC_TO_MICROSECS);
            sysTime.stMicrosecs = static_cast<int32_t>(remain.count() - (sysTime.stMillisecs * NEUtilities::MILLISEC_TO_MICROSECS));

            ASSERT(sysTime.stMillisecs < 1000);
            ASSERT(sysTime.stMicrosecs < 1000);
        }
    }

} // namespace

#endif // WINDOWS
