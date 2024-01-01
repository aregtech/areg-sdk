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
            FILETIME now { 0, 0 };
            ::GetSystemTimeAsFileTime( &now );

            String::formatString( out_buffer, length, strFormat
                                  , prefix != nullptr ? prefix : NEUtilities::DEFAULT_GENERATED_NAME.data()
                                  , spec
                                  , now.dwHighDateTime
                                  , spec
                                  , now.dwLowDateTime);
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


    inline void _convWinFileTime2AregFileTime( const FILETIME & winTime, NEUtilities::sFileTime & aregTime )
    {
        aregTime.ftLowDateTime  = winTime.dwLowDateTime;
        aregTime.ftHighDateTime = winTime.dwHighDateTime;
    }
    
    //!< Converts areg 64-bits time structure to windows 64-bits time.
    inline void _convAregFileTime2WinFileTime( const NEUtilities::sFileTime & IN aregTime, FILETIME & OUT winTime )
    {
        winTime.dwLowDateTime   = aregTime.ftLowDateTime;
        winTime.dwHighDateTime  = aregTime.ftHighDateTime;
    }

    //!< Converts windows 64-bit file structure to areg 64-bit time.
    inline void _convWinFileTime2AregTime( const FILETIME & IN winTime, TIME64 & OUT aregTime)
    {
        aregTime = MACRO_MAKE_64( winTime.dwHighDateTime, winTime.dwLowDateTime );
    }

    //!< Converts areg 64-bits time structure to 64-bits time value.
    inline TIME64 _convWinFileTime2AregTime( const FILETIME & winTime )
    {
        return MACRO_MAKE_64( winTime.dwHighDateTime, winTime.dwLowDateTime );
    }

    //!< Converts 64-bits time value to areg 64-bit time structure.
    inline void _convAregTime2WinFileTime(const TIME64 & IN aregTime, FILETIME & OUT winTime)
    {
        winTime.dwLowDateTime   = MACRO_64_LO_BYTE32(aregTime);
        winTime.dwHighDateTime  = MACRO_64_HI_BYTE32(aregTime);
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

    bool _osConvToLocalTime( const TIME64 & inUtcTime, sSystemTime & outLocalTime )
    {
        NEUtilities::sSystemTime utcTime;
        convToSystemTime( inUtcTime, utcTime );
        return convToLocalTime( utcTime, outLocalTime );
    }


    void _osSystemTimeNow( NEUtilities::sSystemTime & OUT sysTime, bool localTime )
    {
        SYSTEMTIME utc{ 0 };
        ::GetSystemTime( &utc );

        if ( localTime )
        {
            TIME_ZONE_INFORMATION tzi{ 0 };
            SYSTEMTIME local{ 0 };
            GetTimeZoneInformation( &tzi );
            _convWinSysTime2AregSysTime( SystemTimeToTzSpecificLocalTime( &tzi, &utc, &local ) ? local : utc, sysTime );
        }
        else
        {
            NEUtilities::_convWinSysTime2AregSysTime( utc, sysTime );
        }
    }

    void _osSystemTimeNow( NEUtilities::sFileTime & OUT fileTime, bool localTime )
    {
        SYSTEMTIME utc{ 0 };
        SYSTEMTIME * src = &utc;
        ::GetSystemTime( src );

        if ( localTime )
        {
            SYSTEMTIME local{ 0 };
            TIME_ZONE_INFORMATION tzi{ 0 };
            GetTimeZoneInformation( &tzi );
            if ( SystemTimeToTzSpecificLocalTime( &tzi, &utc, &local ) )
            {
                src = &local;
            }
        }

        FILETIME ft{ 0 };
        ::SystemTimeToFileTime( src, &ft );

        NEUtilities::_convWinFileTime2AregFileTime( ft, fileTime );
    }

    TIME64 _osSystemTimeNow( void )
    {
        SYSTEMTIME st{ 0 };
        ::GetSystemTime( &st );

        FILETIME ft{ 0 };
        ::SystemTimeToFileTime( &st, &ft );

        return _convWinFileTime2AregTime( ft );
    }

    TIME64 _osConvToTime( const NEUtilities::sSystemTime & IN sysTime )
    {
        SYSTEMTIME st{ 0 };
        NEUtilities::_convAregSysTime2WinSysTime( sysTime, st );

        FILETIME ft{ 0 };
        ::SystemTimeToFileTime( &st, &ft );

        return _convWinFileTime2AregTime( ft );
    }

    void _osConvToSystemTime( const TIME64 & IN timeValue, NEUtilities::sSystemTime & OUT sysTime )
    {
        FILETIME ft{ 0 };
        NEUtilities::_convAregTime2WinFileTime( timeValue, ft );

        SYSTEMTIME st{ 0 };
        ::FileTimeToSystemTime( &ft, &st );

        NEUtilities::_convWinSysTime2AregSysTime( st, sysTime );
    }

    void _osConvToSystemTime( const NEUtilities::sFileTime & IN fileTime, NEUtilities::sSystemTime & OUT sysTime )
    {
        FILETIME ft{ 0 };
        NEUtilities::_convAregFileTime2WinFileTime( fileTime, ft );

        SYSTEMTIME st{ 0 };
        ::FileTimeToSystemTime( &ft, &st );

        NEUtilities::_convWinSysTime2AregSysTime( st, sysTime );
    }

    void _osConvToFileTime( const NEUtilities::sSystemTime & IN sysTime, NEUtilities::sFileTime & OUT fileTime )
    {
        SYSTEMTIME st{ 0 };
        NEUtilities::_convAregSysTime2WinSysTime( sysTime, st );

        FILETIME ft{ 0 };
        ::SystemTimeToFileTime( &st, &ft );

        NEUtilities::_convWinFileTime2AregFileTime( ft, fileTime );
    }

} // namespace

#endif // WINDOWS
