/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
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

        struct tm * conv = ::localtime( &secs );
        if ( conv != nullptr )
        {
            localTime.stYear = conv->tm_year + 1900;
            localTime.stMonth = conv->tm_mon + 1;
            localTime.stDay = conv->tm_mday;
            localTime.stHour = conv->tm_hour;
            localTime.stMinute = conv->tm_min;
            localTime.stSecond = conv->tm_sec;
            localTime.stDayOfWeek = conv->tm_wday;
            localTime.stMillisecs = milli;
            localTime.stMicrosecs = micro;

            result = true;
        }

        return result;
    }

    bool _osConvToLocalTime( const sSystemTime & IN utcTime, sSystemTime & OUT localTime )
    {
        bool result = false;
        TIME64 quad = NEUtilities::convToTime( utcTime );

        // time_t secs = static_cast<time_t>(quad / SEC_TO_MICROSECS);
        // time_t rest = quad - (secs * SEC_TO_MICROSECS);
        // unsigned short milli = static_cast<unsigned short>(rest / MILLISEC_TO_MICROSECS);
        // unsigned short micro = static_cast<unsigned short>(rest - (milli * MILLISEC_TO_MICROSECS));

        time_t secs;
        unsigned short milli, micro;
        _convertMicrosecs( quad, secs, milli, micro );

        struct tm * conv = ::localtime( &secs );
        if ( conv != nullptr )
        {
            localTime.stYear = conv->tm_year + 1900;
            localTime.stMonth = conv->tm_mon + 1;
            localTime.stDay = conv->tm_mday;
            localTime.stHour = conv->tm_hour;
            localTime.stMinute = conv->tm_min;
            localTime.stSecond = conv->tm_sec;
            localTime.stDayOfWeek = conv->tm_wday;
            localTime.stMillisecs = milli;
            localTime.stMicrosecs = micro;

            result = true;
        }

        return result;
    }

    void _osSystemTimeNow( NEUtilities::sSystemTime & OUT sysTime, bool localTime )
    {
        struct timespec ts;
        struct tm now;

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
        struct timespec ts;
        ::clock_gettime( CLOCK_REALTIME, &ts );

        if ( localTime )
        {
            struct tm now;
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
        struct timespec ts;
        ::clock_gettime( CLOCK_REALTIME, &ts );

        TIME64 quad = ts.tv_sec * SEC_TO_MICROSECS + ts.tv_nsec / MICROSEC_TO_NS;
        return quad;
    }

    TIME64 _osConvToTime( const NEUtilities::sSystemTime & sysTime )
    {
        TIME64 quad = 0;

        struct tm sys;
        sys.tm_year = sysTime.stYear - 1900;
        sys.tm_mon = sysTime.stMonth - 1;
        sys.tm_mday = sysTime.stDay;
        sys.tm_hour = sysTime.stHour;
        sys.tm_min = sysTime.stMinute;
        sys.tm_sec = sysTime.stSecond;
        sys.tm_wday = sysTime.stDayOfWeek;
        sys.tm_isdst = 0;

        time_t secs = ::mktime( &sys );
        if ( secs != static_cast<time_t>(-1) )
        {
            quad = (secs * SEC_TO_MICROSECS) + (sysTime.stMillisecs * MILLISEC_TO_MICROSECS) + sysTime.stMicrosecs;
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

        time_t secs;
        unsigned short milli, micro;
        _convertMicrosecs( timeValue, secs, milli, micro );

        struct tm * conv = ::gmtime( &secs );
        if ( conv != nullptr )
        {
            sysTime.stYear = conv->tm_year + 1900;
            sysTime.stMonth = conv->tm_mon + 1;
            sysTime.stDay = conv->tm_mday;
            sysTime.stHour = conv->tm_hour;
            sysTime.stMinute = conv->tm_min;
            sysTime.stSecond = conv->tm_sec;
            sysTime.stDayOfWeek = conv->tm_wday;
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
