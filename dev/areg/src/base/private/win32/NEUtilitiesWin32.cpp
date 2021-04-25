/************************************************************************
 * \file        areg/src/base/private/win32/NEUtilitiesWin32.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Windows OS specific hidden utility methods.
 *
 ************************************************************************/

#include "areg/src/base/NEUtilities.hpp"

#ifdef  _WINDOWS

#include "areg/src/base/CEString.hpp"
#include "areg/src/base/NEMemory.hpp"

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

            CEString::PrintString( out_buffer, length, formatStr,
                prefix != NULL ? prefix : NEUtilities::DEFAULT_GENERATED_NAME,
                tick8, spec, tick7, spec, tick6, spec, tick5, spec, tick4, spec, tick3, spec, tick2, spec, tick1 );
        }
    }

    inline void convertWinSysTime2AregSysTime( const SYSTEMTIME & winTime, NEUtilities::sSystemTime & aregTime )
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

    inline void convertAregSysTime2WinSysTime( const NEUtilities::sSystemTime & aregTime, SYSTEMTIME & winTime )
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

    inline void convertWinFileTime2AregFileTime( const FILETIME & winTime, NEUtilities::sFileTime & aregTime )
    {
        uint64_t quad = MACRO_MAKE_64(winTime.dwHighDateTime, winTime.dwLowDateTime);
        quad /= MICROSEC_TO_100NS;
        quad -= WIN_TO_POSIX_EPOCH_BIAS_MICROSECS;

        
        aregTime.ftLowDateTime  = MACRO_64_LO_BYTE32(quad);
        aregTime.ftHighDateTime = MACRO_64_HI_BYTE32(quad);
    }

    inline void convertAregFileTime2WinFileTime( const NEUtilities::sFileTime & aregTime, FILETIME & winTime )
    {
        uint64_t quad = MACRO_MAKE_64(aregTime.ftHighDateTime, aregTime.ftLowDateTime);
        quad += WIN_TO_POSIX_EPOCH_BIAS_MICROSECS;
        quad *= MICROSEC_TO_100NS;

        winTime.dwLowDateTime   = MACRO_64_LO_BYTE32(quad);
        winTime.dwHighDateTime  = MACRO_64_HI_BYTE32(quad);
    }

    inline void convertWinFileTime2AregTime( const FILETIME & winTime, TIME64 & aregTime)
    {
        aregTime = MACRO_MAKE_64(winTime.dwHighDateTime, winTime.dwLowDateTime);
        aregTime /= MICROSEC_TO_100NS;
        aregTime -= WIN_TO_POSIX_EPOCH_BIAS_MICROSECS;
    }

    inline TIME64 convertWinFileTime2AregTime( const FILETIME & winTime )
    {
        TIME64 result = MACRO_MAKE_64(winTime.dwHighDateTime, winTime.dwLowDateTime);
        result /= MICROSEC_TO_100NS;
        result -= WIN_TO_POSIX_EPOCH_BIAS_MICROSECS;

        return result;
    }

}

AREG_API uint64_t NEUtilities::GetTickCount( void )
{
    return static_cast<uint64_t>(::GetTickCount( ));
}

AREG_API bool NEUtilities::ToLocalTime( const sSystemTime &inUtcTime, sSystemTime & outLocalTime )
{
    bool result = false;

    TIME_ZONE_INFORMATION tzi;
    NEMemory::ZeroBuffer(reinterpret_cast<void *>(&tzi), sizeof(TIME_ZONE_INFORMATION));
    if (TIME_ZONE_ID_UNKNOWN != GetTimeZoneInformation(&tzi))
    {
        SYSTEMTIME utc, local;
        NEMemory::ZeroData<SYSTEMTIME>(utc);
        NEMemory::ZeroData<SYSTEMTIME>(local);

        convertAregSysTime2WinSysTime( inUtcTime, utc );
        if (SystemTimeToTzSpecificLocalTime(&tzi, &utc, &local))
        {
            convertWinSysTime2AregSysTime(local, outLocalTime);
            result = true;
        }
    }

    return result;
}

AREG_API bool NEUtilities::ToLocalTime( const TIME64 & inUtcTime, sSystemTime & outLocalTime )
{
    NEUtilities::sSystemTime sysTime;
    ConvertToSystemTime( inUtcTime, sysTime );
    return ToLocalTime(sysTime, outLocalTime);
}

AREG_API void NEUtilities::GetSystemTimeNow( NEUtilities::sSystemTime & out_sysTime )
{
    SYSTEMTIME st;
    NEMemory::ZeroData<SYSTEMTIME>(st);
    ::GetSystemTime( &st );
    NEUtilities::convertWinSysTime2AregSysTime(st, out_sysTime);
}

AREG_API void NEUtilities::GetSystemTimeNow( NEUtilities::sFileTime & out_fileTime )
{
    SYSTEMTIME st;
    NEMemory::ZeroData<SYSTEMTIME>( st );
    ::GetSystemTime( &st );

    FILETIME ft;
    NEMemory::ZeroData<FILETIME>(ft);
    ::SystemTimeToFileTime( &st, &ft );

    NEUtilities::convertWinFileTime2AregFileTime( ft, out_fileTime );
}

AREG_API TIME64 NEUtilities::GetSystemTimeNow( void )
{
    SYSTEMTIME st;
    NEMemory::ZeroData<SYSTEMTIME>( st );
    ::GetSystemTime( &st );

    FILETIME ft;
    NEMemory::ZeroData<FILETIME>( ft );
    ::SystemTimeToFileTime( &st, &ft );

    return convertWinFileTime2AregTime(ft);
}

AREG_API TIME64 NEUtilities::ConvertSystemTime( const NEUtilities::sSystemTime & sysTime )
{
    SYSTEMTIME st;
    NEUtilities::convertAregSysTime2WinSysTime(sysTime, st);

    FILETIME ft;
    NEMemory::ZeroData<FILETIME>( ft );
    ::SystemTimeToFileTime( &st, &ft );

    return convertWinFileTime2AregTime(ft);
}


AREG_API void NEUtilities::ConvertToSystemTime( const TIME64 &  timeValue, NEUtilities::sSystemTime & out_sysTime )
{
    uint64_t quad = timeValue * MICROSEC_TO_100NS;
    quad += WIN_TO_POSIX_EPOCH_BIAS_MICROSECS;

    FILETIME ft;
    ft.dwLowDateTime    = MACRO_64_LO_BYTE32(quad);
    ft.dwHighDateTime   = MACRO_64_HI_BYTE32(quad);

    SYSTEMTIME st;
    NEMemory::ZeroData<SYSTEMTIME>(st);
    ::FileTimeToSystemTime(&ft, &st);

    NEUtilities::convertWinSysTime2AregSysTime(st, out_sysTime);
}

AREG_API void NEUtilities::ConvertFileTimeToSystemTime( const NEUtilities::sFileTime & fileTime, NEUtilities::sSystemTime & out_sysTime )
{
    FILETIME ft;
    NEUtilities::convertAregFileTime2WinFileTime(fileTime, ft);

    SYSTEMTIME st;
    NEMemory::ZeroData<SYSTEMTIME>(st);
    ::FileTimeToSystemTime(&ft, &st);

    NEUtilities::convertWinSysTime2AregSysTime(st, out_sysTime);
}

AREG_API void NEUtilities::ConvertSystemTimeToFileTime( const NEUtilities::sSystemTime & sysTime, NEUtilities::sFileTime & out_fileTime )
{
    SYSTEMTIME st;
    NEUtilities::convertAregSysTime2WinSysTime(sysTime, st);

    FILETIME ft;
    NEMemory::ZeroData<FILETIME>( ft );
    ::SystemTimeToFileTime(&st, &ft);

    NEUtilities::convertWinFileTime2AregFileTime(ft, out_fileTime);
}

#endif // _WINDOWS
