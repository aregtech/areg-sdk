/************************************************************************
 * \file        areg/base/private/win32/NEUtilitiesWin32.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Windows OS specific hidden utility methods.
 *
 ************************************************************************/

#include "areg/base/NEUtilities.hpp"

#ifdef  _WINDOWS

#include "areg/base/String.hpp"
#include "areg/base/NEMemory.hpp"

#include <windows.h>

namespace NEUtilities {

    void _generateName( const char * prefix, char * out_buffer, int length, const char * specChar )
    {
        static const char * formatStr = "%s{%03d%s%03d%s%03d%s%03d%s%03d%s%03d%s%03d%s%03d}";
        if ( out_buffer != NULL )
        {
            *out_buffer = '\0';
            const char * spec = specChar != NULL ? specChar : NEUtilities::DEFAULT_SPECIAL_CHAR;
            FILETIME now = { 0, 0 };
            ::GetSystemTimeAsFileTime( &now );

            int tick1 = static_cast<int>((now.dwHighDateTime >>  0) & 0xFF);
            int tick2 = static_cast<int>((now.dwHighDateTime >>  8) & 0xFF);
            int tick3 = static_cast<int>((now.dwHighDateTime >> 16) & 0xFF);
            int tick4 = static_cast<int>((now.dwHighDateTime >> 24) & 0xFF);
            int tick5 = static_cast<int>((now.dwLowDateTime  >>  0) & 0xFF);
            int tick6 = static_cast<int>((now.dwLowDateTime  >>  8) & 0xFF);
            int tick7 = static_cast<int>((now.dwLowDateTime  >> 16) & 0xFF);
            int tick8 = static_cast<int>((now.dwLowDateTime  >> 24) & 0xFF);

            String::formatString( out_buffer, length, formatStr,
                prefix != NULL ? prefix : NEUtilities::DEFAULT_GENERATED_NAME,
                tick8, spec, tick7, spec, tick6, spec, tick5, spec, tick4, spec, tick3, spec, tick2, spec, tick1 );
        }
    }

    inline void _convWinSysTime2AregSysTime( const SYSTEMTIME & winTime, NEUtilities::sSystemTime & aregTime )
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

    inline void _convAregSysTime2WinSysTime( const NEUtilities::sSystemTime & aregTime, SYSTEMTIME & winTime )
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
        
        // uint64_t quad = MACRO_MAKE_64(winTime.dwHighDateTime, winTime.dwLowDateTime);
        // quad /= MICROSEC_TO_100NS;
        // quad -= WIN_TO_POSIX_EPOCH_BIAS_MICROSECS;        
        // aregTime.ftLowDateTime  = MACRO_64_LO_BYTE32(quad);
        // aregTime.ftHighDateTime = MACRO_64_HI_BYTE32(quad);
    }

    inline void _convAregFileTime2WinFileTime( const NEUtilities::sFileTime & aregTime, FILETIME & winTime )
    {
        winTime.dwLowDateTime   = aregTime.ftLowDateTime;
        winTime.dwHighDateTime  = aregTime.ftHighDateTime;

        // uint64_t quad = MACRO_MAKE_64(aregTime.ftHighDateTime, aregTime.ftLowDateTime);
        // quad += WIN_TO_POSIX_EPOCH_BIAS_MICROSECS;
        // quad *= MICROSEC_TO_100NS
        // winTime.dwLowDateTime   = MACRO_64_LO_BYTE32(quad);
        // winTime.dwHighDateTime  = MACRO_64_HI_BYTE32(quad);
    }

    inline void _convWinFileTime2AregTime( const FILETIME & winTime, TIME64 & aregTime)
    {
        aregTime = MACRO_MAKE_64(winTime.dwHighDateTime, winTime.dwLowDateTime);
        // aregTime /= MICROSEC_TO_100NS;
        // aregTime -= WIN_TO_POSIX_EPOCH_BIAS_MICROSECS;
    }

    inline TIME64 _convWinFileTime2AregTime( const FILETIME & winTime )
    {
        TIME64 result = MACRO_MAKE_64(winTime.dwHighDateTime, winTime.dwLowDateTime);
        // result /= MICROSEC_TO_100NS;
        // result -= WIN_TO_POSIX_EPOCH_BIAS_MICROSECS;

        return result;
    }

    inline void _convAregTime2WinFileTime(const TIME64 & IN aregTime, FILETIME & OUT winTime)
    {
        winTime.dwLowDateTime   = MACRO_64_LO_BYTE32(aregTime);
        winTime.dwHighDateTime  = MACRO_64_HI_BYTE32(aregTime);
    }
}

AREG_API uint64_t NEUtilities::getTickCount( void )
{
    return static_cast<uint64_t>(::GetTickCount( ));
}

AREG_API bool NEUtilities::convToLocalTime( const sSystemTime &inUtcTime, sSystemTime & outLocalTime )
{
    bool result = false;

    TIME_ZONE_INFORMATION tzi;
    NEMemory::zeroBuffer(reinterpret_cast<void *>(&tzi), sizeof(TIME_ZONE_INFORMATION));
    if (TIME_ZONE_ID_UNKNOWN != GetTimeZoneInformation(&tzi))
    {
        SYSTEMTIME utc, local;
        NEMemory::zeroData<SYSTEMTIME>(utc);
        NEMemory::zeroData<SYSTEMTIME>(local);

        _convAregSysTime2WinSysTime( inUtcTime, utc );
        if (SystemTimeToTzSpecificLocalTime(&tzi, &utc, &local))
        {
            _convWinSysTime2AregSysTime(local, outLocalTime);
            outLocalTime.stMicrosecs    = inUtcTime.stMicrosecs;
            result = true;
        }
    }

    return result;
}

AREG_API bool NEUtilities::convToLocalTime( const TIME64 & inUtcTime, sSystemTime & outLocalTime )
{
    NEUtilities::sSystemTime sysTime;
    convToSystemTime( inUtcTime, sysTime );
    return convToLocalTime(sysTime, outLocalTime);
}

AREG_API void NEUtilities::systemTimeNow( NEUtilities::sSystemTime & out_sysTime, bool localTime )
{
    SYSTEMTIME st;
    NEMemory::zeroData<SYSTEMTIME>(st);
    ::GetSystemTime( &st );

    if (localTime)
    {
        TIME_ZONE_INFORMATION tzi;
        NEMemory::zeroBuffer(reinterpret_cast<void *>(&tzi), sizeof(TIME_ZONE_INFORMATION));
        SYSTEMTIME local;
        NEMemory::zeroData<SYSTEMTIME>(local);
        if ((TIME_ZONE_ID_UNKNOWN != GetTimeZoneInformation(&tzi)) && SystemTimeToTzSpecificLocalTime(&tzi, &st, &local))
        {
            _convWinSysTime2AregSysTime(local, out_sysTime);
        }
    }
    else
    {
        NEUtilities::_convWinSysTime2AregSysTime(st, out_sysTime);
    }
}

AREG_API void NEUtilities::systemTimeNow( NEUtilities::sFileTime & out_fileTime, bool localTime )
{
    SYSTEMTIME st, local;
    SYSTEMTIME *src = &st;
    NEMemory::zeroData<SYSTEMTIME>( st );
    ::GetSystemTime( &st );

    if (localTime)
    {
        TIME_ZONE_INFORMATION tzi;
        NEMemory::zeroBuffer(reinterpret_cast<void *>(&tzi), sizeof(TIME_ZONE_INFORMATION));
        NEMemory::zeroData<SYSTEMTIME>(local);
        if ((TIME_ZONE_ID_UNKNOWN != GetTimeZoneInformation(&tzi)) && SystemTimeToTzSpecificLocalTime(&tzi, &st, &local))
        {
            src = &local;
        }
    }

    FILETIME ft;
    NEMemory::zeroData<FILETIME>(ft);
    ::SystemTimeToFileTime( src, &ft );

    NEUtilities::_convWinFileTime2AregFileTime( ft, out_fileTime );
}

AREG_API TIME64 NEUtilities::systemTimeNow( void )
{
    SYSTEMTIME st;
    NEMemory::zeroData<SYSTEMTIME>( st );
    ::GetSystemTime( &st );

    FILETIME ft;
    NEMemory::zeroData<FILETIME>( ft );
    ::SystemTimeToFileTime( &st, &ft );

    return _convWinFileTime2AregTime(ft);
}

AREG_API TIME64 NEUtilities::convToTime( const NEUtilities::sSystemTime & sysTime )
{
    SYSTEMTIME st;
    NEUtilities::_convAregSysTime2WinSysTime(sysTime, st);

    FILETIME ft;
    NEMemory::zeroData<FILETIME>( ft );
    ::SystemTimeToFileTime( &st, &ft );

    return _convWinFileTime2AregTime(ft);
}


AREG_API void NEUtilities::convToSystemTime( const TIME64 &  timeValue, NEUtilities::sSystemTime & out_sysTime )
{
    FILETIME ft;
    NEUtilities::_convAregTime2WinFileTime(timeValue, ft);

    SYSTEMTIME st;
    NEMemory::zeroData<SYSTEMTIME>(st);
    ::FileTimeToSystemTime(&ft, &st);

    NEUtilities::_convWinSysTime2AregSysTime(st, out_sysTime);
}

AREG_API void NEUtilities::convToSystemTime( const NEUtilities::sFileTime & fileTime, NEUtilities::sSystemTime & out_sysTime )
{
    FILETIME ft;
    NEUtilities::_convAregFileTime2WinFileTime(fileTime, ft);

    SYSTEMTIME st;
    NEMemory::zeroData<SYSTEMTIME>(st);
    ::FileTimeToSystemTime(&ft, &st);

    NEUtilities::_convWinSysTime2AregSysTime(st, out_sysTime);
}

AREG_API void NEUtilities::convToFileTime( const NEUtilities::sSystemTime & sysTime, NEUtilities::sFileTime & out_fileTime )
{
    SYSTEMTIME st;
    NEUtilities::_convAregSysTime2WinSysTime(sysTime, st);

    FILETIME ft;
    NEMemory::zeroData<FILETIME>( ft );
    ::SystemTimeToFileTime(&st, &ft);

    NEUtilities::_convWinFileTime2AregFileTime(ft, out_fileTime);
}

#endif // _WINDOWS
