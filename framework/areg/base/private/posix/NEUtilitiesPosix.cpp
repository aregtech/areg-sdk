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
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG POSIX specific hidden utility methods.
 *
 ************************************************************************/

#include "areg/base/NEUtilities.hpp"

#if defined(_POSIX) || defined(POSIX)

#include "areg/base/String.hpp"
#include "areg/base/NEMemory.hpp"
#include "areg/base/GEMacros.h"
#include <time.h>
#include <unistd.h>

namespace NEUtilities
{
    /**
     * \brief   When converting the time in seconds to the time structure,
     *          we are loosing an information, so that we get a difference.
     *          With this method we compute the difference in seconds to balance.
     *          The method is only for internal use and should not be used outside
     *          of this file.
     **/
    inline int _internalTimeDifference( void )
    {
        struct timespec relTime;
        ::clock_gettime( CLOCK_REALTIME, &relTime );
        time_t secs = static_cast<time_t>(relTime.tv_sec);

        struct tm tmGmt{0};
        gmtime_r(&secs, &tmGmt);
        tmGmt.tm_isdst   = -1;
        time_t secsConv  = std::mktime( &tmGmt );
        int64_t timeDiff = static_cast<int64_t>(secs) - static_cast<int64_t>(secsConv);
        return static_cast<int>(timeDiff);
    }

    /**
     * \brief   Generates a name, sets the timestamp of now.
     * 
     * \param   prefix          The prefix to add to name.
     * \param   out_buffer[out] On output, this contains the result.
     * \param   length          The length of output buffer.
     * \param   specChar        Special character to add when generating name.
     * \return Generated name.
     **/
    const char * _osGenerateName( const char * prefix, char * OUT out_buffer, int length, const char * specChar )
    {
        constexpr char strFormat[]   { "%s%s%08x%s%08x" };
        
        if ( out_buffer != nullptr )
        {
            const char * spec = specChar != nullptr ? specChar : NECommon::DEFAULT_SPECIAL_CHAR.data();
            *out_buffer = '\0';
            struct timespec now;
            ::clock_gettime(CLOCK_MONOTONIC, &now);
            String::formatString( out_buffer, length, strFormat
                                , prefix != nullptr ? prefix : NEUtilities::DEFAULT_GENERATED_NAME.data()
                                , spec
                                , now.tv_sec
                                , spec, now.tv_nsec);
        }

        return out_buffer;
    }

    /**
     * \brief   Converts given time in microseconds into the time in seconds, milliseconds and microseconds.
     * 
     * \param   quad[in]        The time in microseconds to parse and extract.
     * \param   outSecs[out]    On output, this contains the time in seconds.
     * \param   outMilli[out]   On output, this contains the remaining time in milliseconds.
     * \param   outMicro[out]   On output, this contains the remaining time in microseconds.
     **/
    inline void _convertMicrosecs(const TIME64 & IN quad, time_t & OUT outSecs, unsigned short & OUT outMilli, unsigned short & OUT outMicro)
    {
        time_t secs = static_cast<time_t>(quad / SEC_TO_MICROSECS);
        time_t rest = static_cast<time_t>(quad % SEC_TO_MICROSECS);
        outSecs     = secs;
        outMilli    = static_cast<unsigned short>(rest / MILLISEC_TO_MICROSECS);
        outMicro    = static_cast<unsigned short>(rest % MILLISEC_TO_MICROSECS);
    }


    uint64_t _osGetTickCount( void )
    {
        struct timespec ts;
        ::clock_gettime( CLOCK_MONOTONIC, &ts );

        TIME64 result = (ts.tv_sec * SEC_TO_MILLISECS) + ts.tv_nsec / MILLISEC_TO_NS;
        return result;
    }

    bool _osConvToLocalTime( const TIME64 & IN utcTime, sSystemTime & OUT localTime )
    {
        bool result = false;

        time_t secs;
        unsigned short milli, micro;
        _convertMicrosecs( utcTime, secs, milli, micro );

        struct tm tmLocal{0};
        if ( ::localtime_r( &secs, &tmLocal ) != nullptr )
        {
            localTime.stYear = tmLocal.tm_year + 1900;
            localTime.stMonth = tmLocal.tm_mon + 1;
            localTime.stDay = tmLocal.tm_mday;
            localTime.stHour = tmLocal.tm_hour;
            localTime.stMinute = tmLocal.tm_min;
            localTime.stSecond = tmLocal.tm_sec;
            localTime.stDayOfWeek = tmLocal.tm_wday;
            localTime.stMillisecs = milli;
            localTime.stMicrosecs = micro;

            result = true;
        }

        return result;
    }

    bool _osConvToLocalTime( const sSystemTime & IN utcTime, sSystemTime & OUT localTime )
    {
        TIME64 quad = NEUtilities::convToTime( utcTime );
        return _osConvToLocalTime(quad, localTime);
    }

    void _osSystemTimeNow( NEUtilities::sSystemTime & OUT sysTime, bool localTime )
    {
        struct timespec ts { 0 };
        struct tm now { 0 };

        ::clock_gettime( CLOCK_REALTIME, &ts );
        if ( localTime )
        {
            ::localtime_r( &ts.tv_sec, &now );
        }
        else
        {
            ::gmtime_r( &ts.tv_sec, &now );
        }

        // unsigned short milli = static_cast<unsigned short>(ts.tv_nsec / MILLISEC_TO_NS);
        // unsigned short micro = static_cast<unsigned short>((ts.tv_nsec - (milli * MILLISEC_TO_MICROSECS)) / MICROSEC_TO_NS);

        unsigned short milli = static_cast<unsigned short>((ts.tv_nsec / MILLISEC_TO_NS));
        unsigned short micro = static_cast<unsigned short>((ts.tv_nsec % MILLISEC_TO_NS) / MICROSEC_TO_NS);

        sysTime.stYear = now.tm_year + 1900;
        sysTime.stMonth = now.tm_mon + 1;
        sysTime.stDayOfWeek = now.tm_wday;
        sysTime.stDay = now.tm_mday;
        sysTime.stHour = now.tm_hour;
        sysTime.stMinute = now.tm_min;
        sysTime.stSecond = now.tm_sec;
        sysTime.stMillisecs = milli;
        sysTime.stMicrosecs = micro;
    }

    void _osSystemTimeNow( NEUtilities::sFileTime & OUT fileTime, bool localTime )
    {
        struct timespec ts { 0 };
        ::clock_gettime( CLOCK_REALTIME, &ts );

        if ( localTime )
        {
            struct tm now { 0 };
            ::localtime_r( &ts.tv_sec, &now );
            time_t local = ::mktime( &now );
            TIME64 quad = static_cast<TIME64>(local * SEC_TO_MICROSECS) + static_cast<TIME64>(ts.tv_nsec / MICROSEC_TO_NS);
            fileTime.ftLowDateTime = MACRO_64_LO_BYTE32( quad );
            fileTime.ftHighDateTime = MACRO_64_HI_BYTE32( quad );
        }
        else
        {
            TIME64 quad = static_cast<TIME64>(ts.tv_sec * SEC_TO_MICROSECS) + static_cast<TIME64>(ts.tv_nsec / MICROSEC_TO_NS);
            fileTime.ftLowDateTime = MACRO_64_LO_BYTE32( quad );
            fileTime.ftHighDateTime = MACRO_64_HI_BYTE32( quad );
        }
    }

    TIME64 _osSystemTimeNow( void )
    {
        struct timespec ts { 0 };
        ::clock_gettime( CLOCK_REALTIME, &ts );

        TIME64 quad = static_cast<TIME64>(ts.tv_sec) * SEC_TO_MICROSECS + static_cast<TIME64>(ts.tv_nsec) / MICROSEC_TO_NS;
        return quad;
    }

    TIME64 _osConvToTime( const NEUtilities::sSystemTime & sysTime )
    {
        TIME64 quad = 0;

        struct tm tmSys;
        tmSys.tm_year = sysTime.stYear - 1900;
        tmSys.tm_mon = sysTime.stMonth - 1;
        tmSys.tm_mday = sysTime.stDay;
        tmSys.tm_hour = sysTime.stHour;
        tmSys.tm_min = sysTime.stMinute;
        tmSys.tm_sec = sysTime.stSecond;
        tmSys.tm_isdst = -1;

        time_t secs = std::mktime( &tmSys );
        secs += _internalTimeDifference();

        if ( secs != static_cast<time_t>(-1) )
        {
            quad = (SEC_TO_MICROSECS * secs) + (MILLISEC_TO_MICROSECS * sysTime.stMillisecs) + sysTime.stMicrosecs;
        }

        return quad;
    }

    void _osConvToSystemTime( const TIME64 & IN timeValue, NEUtilities::sSystemTime & OUT sysTime )
    {
        NEMemory::memZero( &sysTime, sizeof( NEUtilities::sSystemTime ) );

        // time_t secs = static_cast<time_t>(timeValue / SEC_TO_MICROSECS);
        // time_t rest = timeValue - (secs * SEC_TO_MICROSECS);
        // unsigned short milli = static_cast<unsigned short>(rest / MILLISEC_TO_MICROSECS);
        // unsigned short micro = static_cast<unsigned short>(rest - (milli * MILLISEC_TO_MICROSECS));

        time_t secs{ 0 };
        unsigned short milli{ 0 }, micro{ 0 };
        _convertMicrosecs( timeValue, secs, milli, micro );

        struct tm utc{ 0 };
        if ( ::gmtime_r( &secs, &utc ) != nullptr )
        {
            sysTime.stYear = utc.tm_year + 1900;
            sysTime.stMonth = utc.tm_mon + 1;
            sysTime.stDay = utc.tm_mday;
            sysTime.stHour = utc.tm_hour;
            sysTime.stMinute = utc.tm_min;
            sysTime.stSecond = utc.tm_sec;
            sysTime.stDayOfWeek = utc.tm_wday;
            sysTime.stMillisecs = milli;
            sysTime.stMicrosecs = micro;
        }
    }

    void _osConvToSystemTime( const NEUtilities::sFileTime & IN fileTime, NEUtilities::sSystemTime & OUT sysTime )
    {
        TIME64 quad = MACRO_MAKE_64( fileTime.ftHighDateTime, fileTime.ftLowDateTime );
        NEUtilities::convToSystemTime( quad, sysTime );
    }

    void _osConvToFileTime( const NEUtilities::sSystemTime & IN sysTime, NEUtilities::sFileTime & OUT fileTime )
    {
        TIME64 quad = NEUtilities::convToTime( sysTime );
        fileTime.ftLowDateTime = MACRO_64_LO_BYTE32( quad );
        fileTime.ftHighDateTime = MACRO_64_HI_BYTE32( quad );
    }

}   // namespace

#endif // defined(_POSIX) || defined(POSIX)
