/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/base/private/win32/NEUtilitiesWin32.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Windows OS specific hidden utility methods.
 *
 ************************************************************************/

#include "areg/base/NEUtilities.hpp"

#ifdef  _WINDOWS

#include "areg/base/String.hpp"
#include "areg/base/NEMemory.hpp"

#include <windows.h>

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
            FILETIME now = { 0, 0 };
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
        aregTime.stYear         = winTime.wYear;
        aregTime.stMonth        = winTime.wMonth;
        aregTime.stDayOfWeek    = winTime.wDayOfWeek;
        aregTime.stDay          = winTime.wDay;
        aregTime.stHour         = winTime.wHour;
        aregTime.stMinute       = winTime.wMinute;
        aregTime.stSecond       = winTime.wSecond;
        aregTime.stMillisecs    = winTime.wMilliseconds;
        aregTime.stMicrosecs    = 0;
    }

    //!< Converts areg specific time structure to Windows system time.
    inline void _convAregSysTime2WinSysTime( const NEUtilities::sSystemTime & IN aregTime, SYSTEMTIME & OUT winTime )
    {
        winTime.wYear           = aregTime.stYear;
        winTime.wMonth          = aregTime.stMonth;
        winTime.wDayOfWeek      = aregTime.stDayOfWeek;
        winTime.wDay            = aregTime.stDay;
        winTime.wHour           = aregTime.stHour;
        winTime.wMinute         = aregTime.stMinute;
        winTime.wSecond         = aregTime.stSecond;
        winTime.wMilliseconds   = aregTime.stMillisecs;
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
        aregTime = MACRO_MAKE_64(winTime.dwHighDateTime, winTime.dwLowDateTime);
    }

    //!< Converts areg 64-bits time structure to 64-bits time value.
    inline TIME64 _convWinFileTime2AregTime( const FILETIME & winTime )
    {
        return static_cast<TIME64>(MACRO_MAKE_64(winTime.dwHighDateTime, winTime.dwLowDateTime));
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

        TIME_ZONE_INFORMATION tzi;
        NEMemory::memZero( reinterpret_cast<void *>(&tzi), sizeof( TIME_ZONE_INFORMATION ) );
        if ( TIME_ZONE_ID_UNKNOWN != GetTimeZoneInformation( &tzi ) )
        {
            SYSTEMTIME utc, local;
            NEMemory::zeroElement<SYSTEMTIME>( utc );
            NEMemory::zeroElement<SYSTEMTIME>( local );

            _convAregSysTime2WinSysTime( inUtcTime, utc );
            if ( SystemTimeToTzSpecificLocalTime( &tzi, &utc, &local ) )
            {
                _convWinSysTime2AregSysTime( local, outLocalTime );
                outLocalTime.stMicrosecs = inUtcTime.stMicrosecs;
                result = true;
            }
        }

        return result;
    }

    bool _osConvToLocalTime( const TIME64 & inUtcTime, sSystemTime & outLocalTime )
    {
        NEUtilities::sSystemTime sysTime;
        convToSystemTime( inUtcTime, sysTime );
        return convToLocalTime( sysTime, outLocalTime );
    }


    void _osSystemTimeNow( NEUtilities::sSystemTime & OUT sysTime, bool localTime )
    {
        SYSTEMTIME st;
        NEMemory::zeroElement<SYSTEMTIME>( st );
        ::GetSystemTime( &st );

        if ( localTime )
        {
            TIME_ZONE_INFORMATION tzi;
            NEMemory::memZero( reinterpret_cast<void *>(&tzi), sizeof( TIME_ZONE_INFORMATION ) );
            SYSTEMTIME local;
            NEMemory::zeroElement<SYSTEMTIME>( local );
            if ( (TIME_ZONE_ID_UNKNOWN != GetTimeZoneInformation( &tzi )) && SystemTimeToTzSpecificLocalTime( &tzi, &st, &local ) )
            {
                _convWinSysTime2AregSysTime( local, sysTime );
            }
        }
        else
        {
            NEUtilities::_convWinSysTime2AregSysTime( st, sysTime );
        }
    }

    void _osSystemTimeNow( NEUtilities::sFileTime & OUT fileTime, bool localTime )
    {
        SYSTEMTIME st, local;
        SYSTEMTIME * src = &st;
        NEMemory::zeroElement<SYSTEMTIME>( st );
        ::GetSystemTime( &st );

        if ( localTime )
        {
            TIME_ZONE_INFORMATION tzi;
            NEMemory::memZero( reinterpret_cast<void *>(&tzi), sizeof( TIME_ZONE_INFORMATION ) );
            NEMemory::zeroElement<SYSTEMTIME>( local );
            if ( (TIME_ZONE_ID_UNKNOWN != GetTimeZoneInformation( &tzi )) && SystemTimeToTzSpecificLocalTime( &tzi, &st, &local ) )
            {
                src = &local;
            }
        }

        FILETIME ft;
        NEMemory::zeroElement<FILETIME>( ft );
        ::SystemTimeToFileTime( src, &ft );

        NEUtilities::_convWinFileTime2AregFileTime( ft, fileTime );
    }

    TIME64 _osSystemTimeNow( void )
    {
        SYSTEMTIME st;
        NEMemory::zeroElement<SYSTEMTIME>( st );
        ::GetSystemTime( &st );

        FILETIME ft;
        NEMemory::zeroElement<FILETIME>( ft );
        ::SystemTimeToFileTime( &st, &ft );

        return _convWinFileTime2AregTime( ft );
    }

    TIME64 _osConvToTime( const NEUtilities::sSystemTime & sysTime )
    {
        SYSTEMTIME st;
        NEUtilities::_convAregSysTime2WinSysTime( sysTime, st );

        FILETIME ft;
        NEMemory::zeroElement<FILETIME>( ft );
        ::SystemTimeToFileTime( &st, &ft );

        return _convWinFileTime2AregTime( ft );
    }

    void _osConvToSystemTime( const TIME64 & IN timeValue, NEUtilities::sSystemTime & OUT sysTime )
    {
        FILETIME ft;
        NEUtilities::_convAregTime2WinFileTime( timeValue, ft );

        SYSTEMTIME st;
        NEMemory::zeroElement<SYSTEMTIME>( st );
        ::FileTimeToSystemTime( &ft, &st );

        NEUtilities::_convWinSysTime2AregSysTime( st, sysTime );
    }

    void _osConvToSystemTime( const NEUtilities::sFileTime & IN fileTime, NEUtilities::sSystemTime & OUT sysTime )
    {
        FILETIME ft;
        NEUtilities::_convAregFileTime2WinFileTime( fileTime, ft );

        SYSTEMTIME st;
        NEMemory::zeroElement<SYSTEMTIME>( st );
        ::FileTimeToSystemTime( &ft, &st );

        NEUtilities::_convWinSysTime2AregSysTime( st, sysTime );
    }

    void _osConvToFileTime( const NEUtilities::sSystemTime & IN sysTime, NEUtilities::sFileTime & OUT fileTime )
    {
        SYSTEMTIME st;
        NEUtilities::_convAregSysTime2WinSysTime( sysTime, st );

        FILETIME ft;
        NEMemory::zeroElement<FILETIME>( ft );
        ::SystemTimeToFileTime( &st, &ft );

        NEUtilities::_convWinFileTime2AregFileTime( ft, fileTime );
    }

} // namespace

#endif // _WINDOWS
