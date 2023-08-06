/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        units/DateTimeTest.Cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Google test dummy file.
 *              Created as a fist file to integrate GTest in AREG
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/DateTime.hpp"
#include "areg/base/String.hpp"
#include "areg/base/NEUtilities.hpp"

#ifdef WINDOWS

    #include <Windows.h>

namespace
{
    //!< Converts Win system time to the areg specific time structure.
    inline void _convWinSysTime2AregSysTime( const SYSTEMTIME & IN winTime, NEUtilities::sSystemTime & OUT aregTime )
    {
        aregTime.stYear = static_cast<int>(winTime.wYear);
        aregTime.stMonth = static_cast<int>(winTime.wMonth);
        aregTime.stDayOfWeek = static_cast<int>(winTime.wDayOfWeek);
        aregTime.stDay = static_cast<int>(winTime.wDay);
        aregTime.stHour = static_cast<int>(winTime.wHour);
        aregTime.stMinute = static_cast<int>(winTime.wMinute);
        aregTime.stSecond = static_cast<int>(winTime.wSecond);
        aregTime.stMillisecs = static_cast<int>(winTime.wMilliseconds);
        aregTime.stMicrosecs = 0;
    }

    //!< Converts areg specific time structure to Windows system time.
    inline void _convAregSysTime2WinSysTime( const NEUtilities::sSystemTime & IN aregTime, SYSTEMTIME & OUT winTime )
    {
        winTime.wYear = static_cast<WORD>(aregTime.stYear);
        winTime.wMonth = static_cast<WORD>(aregTime.stMonth);
        winTime.wDayOfWeek = static_cast<WORD>(aregTime.stDayOfWeek);
        winTime.wDay = static_cast<WORD>(aregTime.stDay);
        winTime.wHour = static_cast<WORD>(aregTime.stHour);
        winTime.wMinute = static_cast<WORD>(aregTime.stMinute);
        winTime.wSecond = static_cast<WORD>(aregTime.stSecond);
        winTime.wMilliseconds = static_cast<WORD>(aregTime.stMillisecs);
    }
}
#endif  // WINDOWS

TEST( DateTimeTest, TestNow )
{
    NEUtilities::sSystemTime sysTime;
    DateTime date( DateTime::getNow( ) );
    ASSERT_TRUE( date.getTime( ) != 0 );

    date.convToSystemTime( sysTime );

    ASSERT_TRUE( sysTime.stYear >= 2023 );
    ASSERT_TRUE( (sysTime.stMonth >= 1) && (sysTime.stMonth <= 12) );
    ASSERT_TRUE( (sysTime.stDayOfWeek >= 0) && (sysTime.stDayOfWeek <= 6));
    ASSERT_TRUE( (sysTime.stDay >= 1) && (sysTime.stDay <= 31));
    ASSERT_TRUE( (sysTime.stHour >= 0) && (sysTime.stHour <= 23) );
    ASSERT_TRUE( (sysTime.stMinute >= 0) && (sysTime.stMinute <= 59) );
    ASSERT_TRUE( (sysTime.stSecond >= 0) && (sysTime.stSecond <= 59) );
    ASSERT_TRUE( (sysTime.stMillisecs >= 0) && (sysTime.stMillisecs <= 999) );
    ASSERT_TRUE( (sysTime.stMicrosecs >= 0) && (sysTime.stMicrosecs <= 999) );

}

TEST( DateTimeTest, TestLocalTimeWin32 )
{
    DateTime date( DateTime::getNow( ) );
    ASSERT_TRUE( date.getTime( ) != 0 );

    NEUtilities::sSystemTime utcTime;
    NEUtilities::convToSystemTime( date.getTime( ), utcTime );

    EXPECT_GE( utcTime.stYear, 2023 ) << "UTC Year:   " << utcTime.stYear << std::endl;

    EXPECT_GE( utcTime.stMonth, 1 ) << "UTC Month:  " << utcTime.stMonth << std::endl;
    EXPECT_LE( utcTime.stMonth, 12 ) << "UTC Month:  " << utcTime.stMonth << std::endl;

    EXPECT_GE( utcTime.stDayOfWeek, 0 ) << "UTC DoF:    " << utcTime.stDayOfWeek << std::endl;
    EXPECT_LE( utcTime.stDayOfWeek, 6 ) << "UTC DoF:    " << utcTime.stDayOfWeek << std::endl;

    EXPECT_GE( utcTime.stDay, 1 ) << "UTC Day:    " << utcTime.stDay << std::endl;
    EXPECT_LE( utcTime.stDay, 31 ) << "UTC Day:    " << utcTime.stDay << std::endl;

    EXPECT_GE( utcTime.stHour, 0 ) << "UTC Hour:   " << utcTime.stHour << std::endl;
    EXPECT_LE( utcTime.stHour, 23 ) << "UTC Hour:   " << utcTime.stHour << std::endl;

    EXPECT_GE( utcTime.stMinute, 0 ) << "UTC Minute: " << utcTime.stMinute << std::endl;
    EXPECT_LE( utcTime.stMinute, 59 ) << "UTC Minute: " << utcTime.stMinute << std::endl;

    EXPECT_GE( utcTime.stSecond, 0 ) << "UTC Second: " << utcTime.stSecond << std::endl;
    EXPECT_LE( utcTime.stSecond, 59 ) << "UTC Second: " << utcTime.stSecond << std::endl;

    EXPECT_GE( utcTime.stMillisecs, 0 ) << "UTC MSec:   " << utcTime.stMillisecs << std::endl;
    EXPECT_LE( utcTime.stMillisecs, 999 ) << "UTC MSec:   " << utcTime.stMillisecs << std::endl;

    EXPECT_GE( utcTime.stMicrosecs, 0 ) << "UTC MuSec:  " << utcTime.stMicrosecs << std::endl;
    EXPECT_LE( utcTime.stMicrosecs, 999 ) << "UTC MuSec:  " << utcTime.stMicrosecs << std::endl;

    NEUtilities::sSystemTime localTime{ };
    TIME_ZONE_INFORMATION tzi{ 0 };
    if ( TIME_ZONE_ID_UNKNOWN != GetTimeZoneInformation( &tzi ) )
    {
        SYSTEMTIME utc{ 0 };
        SYSTEMTIME local{ 0 };

        _convAregSysTime2WinSysTime( utcTime, utc );
        if ( SystemTimeToTzSpecificLocalTime( &tzi, &utc, &local ) )
        {
            _convWinSysTime2AregSysTime( local, localTime );
            localTime.stMicrosecs = utcTime.stMicrosecs;
        }
    }
    else
    {
        EXPECT_GT( -1, 1 ) << "TIME_ZONE_ID_UNKNOWN" << std::endl;
    }


    EXPECT_GE( localTime.stYear, 2023 ) << "Local Year:   " << localTime.stYear << std::endl;

    EXPECT_GE( localTime.stMonth, 1 ) << "Local Month:  " << localTime.stMonth << std::endl;
    EXPECT_LE( localTime.stMonth, 12 ) << "Local Month:  " << localTime.stMonth << std::endl;

    EXPECT_GE( localTime.stDayOfWeek, 0 ) << "Local DoF:    " << localTime.stDayOfWeek << std::endl;
    EXPECT_LE( localTime.stDayOfWeek, 6 ) << "Local DoF:    " << localTime.stDayOfWeek << std::endl;

    EXPECT_GE( localTime.stDay, 1 ) << "Local Day:    " << localTime.stDay << std::endl;
    EXPECT_LE( localTime.stDay, 31 ) << "Local Day:    " << localTime.stDay << std::endl;

    EXPECT_GE( localTime.stHour, 0 ) << "Local Hour:   " << localTime.stHour << std::endl;
    EXPECT_LE( localTime.stHour, 23 ) << "Local Hour:   " << localTime.stHour << std::endl;

    EXPECT_GE( localTime.stMinute, 0 ) << "Local Minute: " << localTime.stMinute << std::endl;
    EXPECT_LE( localTime.stMinute, 59 ) << "Local Minute: " << localTime.stMinute << std::endl;

    EXPECT_GE( localTime.stSecond, 0 ) << "Local Second: " << localTime.stSecond << std::endl;
    EXPECT_LE( localTime.stSecond, 59 ) << "Local Second: " << localTime.stSecond << std::endl;

    EXPECT_GE( localTime.stMillisecs, 0 ) << "Local MSec:   " << localTime.stMillisecs << std::endl;
    EXPECT_LE( localTime.stMillisecs, 999 ) << "Local MSec:   " << localTime.stMillisecs << std::endl;

    EXPECT_GE( localTime.stMicrosecs, 0 ) << "Local MuSec:  " << localTime.stMicrosecs << std::endl;
    EXPECT_LE( localTime.stMicrosecs, 999 ) << "Local MuSec:  " << localTime.stMicrosecs << std::endl;
}

TEST( DateTimeTest, TestFormatISO8601 )
{
    DateTime date( DateTime::getNow( ) );
    ASSERT_TRUE( date.getTime( ) != 0 );

    NEUtilities::sSystemTime utcTime;
    NEUtilities::convToSystemTime( date.getTime( ), utcTime );

    EXPECT_GE( utcTime.stYear, 2023 )   << "UTC Year:   " << utcTime.stYear  << std::endl;

    EXPECT_GE( utcTime.stMonth, 1 )     << "UTC Month:  " << utcTime.stMonth << std::endl;
    EXPECT_LE( utcTime.stMonth, 12 )    << "UTC Month:  " << utcTime.stMonth << std::endl;

    EXPECT_GE( utcTime.stDayOfWeek, 0 ) << "UTC DoF:    " << utcTime.stDayOfWeek << std::endl;
    EXPECT_LE( utcTime.stDayOfWeek, 6 ) << "UTC DoF:    " << utcTime.stDayOfWeek << std::endl;

    EXPECT_GE( utcTime.stDay, 1 )       << "UTC Day:    " << utcTime.stDay << std::endl;
    EXPECT_LE( utcTime.stDay, 31)       << "UTC Day:    " << utcTime.stDay << std::endl;

    EXPECT_GE( utcTime.stHour, 0 )      << "UTC Hour:   " << utcTime.stHour << std::endl;
    EXPECT_LE( utcTime.stHour, 23 )     << "UTC Hour:   " << utcTime.stHour << std::endl;

    EXPECT_GE( utcTime.stMinute, 0 )    << "UTC Minute: " << utcTime.stMinute << std::endl;
    EXPECT_LE( utcTime.stMinute, 59 )   << "UTC Minute: " << utcTime.stMinute << std::endl;

    EXPECT_GE( utcTime.stSecond, 0 )    << "UTC Second: " << utcTime.stSecond << std::endl;
    EXPECT_LE( utcTime.stSecond, 59 )   << "UTC Second: " << utcTime.stSecond << std::endl;

    EXPECT_GE( utcTime.stMillisecs, 0 ) << "UTC MSec:   " << utcTime.stMillisecs << std::endl;
    EXPECT_LE( utcTime.stMillisecs, 999)<< "UTC MSec:   " << utcTime.stMillisecs << std::endl;

    EXPECT_GE( utcTime.stMicrosecs, 0 ) << "UTC MuSec:  " << utcTime.stMicrosecs << std::endl;
    EXPECT_LE( utcTime.stMicrosecs,999) << "UTC MuSec:  " << utcTime.stMicrosecs << std::endl;

    NEUtilities::sSystemTime localTime{ };
    NEUtilities::convToLocalTime( utcTime, localTime );
     
    EXPECT_GE( localTime.stYear, 2023 )   << "Local Year:   " << localTime.stYear  << std::endl;

    EXPECT_GE( localTime.stMonth, 1 )     << "Local Month:  " << localTime.stMonth << std::endl;
    EXPECT_LE( localTime.stMonth, 12 )    << "Local Month:  " << localTime.stMonth << std::endl;

    EXPECT_GE( localTime.stDayOfWeek, 0 ) << "Local DoF:    " << localTime.stDayOfWeek << std::endl;
    EXPECT_LE( localTime.stDayOfWeek, 6 ) << "Local DoF:    " << localTime.stDayOfWeek << std::endl;

    EXPECT_GE( localTime.stDay, 1 )       << "Local Day:    " << localTime.stDay << std::endl;
    EXPECT_LE( localTime.stDay, 31)       << "Local Day:    " << localTime.stDay << std::endl;

    EXPECT_GE( localTime.stHour, 0 )      << "Local Hour:   " << localTime.stHour << std::endl;
    EXPECT_LE( localTime.stHour, 23 )     << "Local Hour:   " << localTime.stHour << std::endl;

    EXPECT_GE( localTime.stMinute, 0 )    << "Local Minute: " << localTime.stMinute << std::endl;
    EXPECT_LE( localTime.stMinute, 59 )   << "Local Minute: " << localTime.stMinute << std::endl;

    EXPECT_GE( localTime.stSecond, 0 )    << "Local Second: " << localTime.stSecond << std::endl;
    EXPECT_LE( localTime.stSecond, 59 )   << "Local Second: " << localTime.stSecond << std::endl;

    EXPECT_GE( localTime.stMillisecs, 0 ) << "Local MSec:   " << localTime.stMillisecs << std::endl;
    EXPECT_LE( localTime.stMillisecs, 999)<< "Local MSec:   " << localTime.stMillisecs << std::endl;

    EXPECT_GE( localTime.stMicrosecs, 0 ) << "Local MuSec:  " << localTime.stMicrosecs << std::endl;
    EXPECT_LE( localTime.stMicrosecs,999) << "Local MuSec:  " << localTime.stMicrosecs << std::endl;

    struct tm conv { 0 };
    NEUtilities::convToTm( localTime, conv );

    int year = 2023 - 1900;
    EXPECT_GE( conv.tm_year, year ) << "TM Year:   " << conv.tm_year << std::endl;

    EXPECT_GE( conv.tm_mon, 0 )     << "TM Month:  " << conv.tm_mon << std::endl;
    EXPECT_LE( conv.tm_mon, 11 )    << "TM Month:  " << conv.tm_mon << std::endl;

    EXPECT_GE( conv.tm_wday, 0 )    << "TM DoF:    " << conv.tm_wday << std::endl;
    EXPECT_LE( conv.tm_wday, 6 )    << "TM DoF:    " << conv.tm_wday << std::endl;

    EXPECT_GE( conv.tm_mday, 1 )    << "TM Day:    " << conv.tm_mday << std::endl;
    EXPECT_LE( conv.tm_mday, 31)    << "TM Day:    " << conv.tm_mday << std::endl;

    EXPECT_GE( conv.tm_hour, 0 )    << "TM Hour:   " << conv.tm_hour << std::endl;
    EXPECT_LE( conv.tm_hour, 23 )   << "TM Hour:   " << conv.tm_hour << std::endl;

    EXPECT_GE( conv.tm_min, 0 )     << "TM Minute: " << conv.tm_min << std::endl;
    EXPECT_LE( conv.tm_min, 59 )    << "TM Minute: " << conv.tm_min << std::endl;

    EXPECT_GE( conv.tm_sec, 0 )     << "TM Second: " << conv.tm_sec << std::endl;
    EXPECT_LE( conv.tm_sec, 59 )    << "TM Second: " << conv.tm_sec << std::endl;


    NEUtilities::sSystemTime sysTime;
    NEUtilities::convToLocalTime( date.getTime( ), sysTime );

    EXPECT_GE( sysTime.stYear, 2023 )   << "Year:   " << sysTime.stYear  << std::endl;

    EXPECT_GE( sysTime.stMonth, 1 )     << "Month:  " << sysTime.stMonth << std::endl;
    EXPECT_LE( sysTime.stMonth, 12 )    << "Month:  " << sysTime.stMonth << std::endl;

    EXPECT_GE( sysTime.stDayOfWeek, 0 ) << "DoF:    " << sysTime.stDayOfWeek << std::endl;
    EXPECT_LE( sysTime.stDayOfWeek, 6 ) << "DoF:    " << sysTime.stDayOfWeek << std::endl;

    EXPECT_GE( sysTime.stDay, 0 )       << "Day:    " << sysTime.stDay << std::endl;
    EXPECT_LE( sysTime.stDay, 31)       << "Day:    " << sysTime.stDay << std::endl;

    EXPECT_GE( sysTime.stHour, 0 )      << "Hour:   " << sysTime.stHour << std::endl;
    EXPECT_LE( sysTime.stHour, 23 )     << "Hour:   " << sysTime.stHour << std::endl;

    EXPECT_GE( sysTime.stMinute, 0 )    << "Minute: " << sysTime.stMinute << std::endl;
    EXPECT_LE( sysTime.stMinute, 59 )   << "Minute: " << sysTime.stMinute << std::endl;

    EXPECT_GE( sysTime.stSecond, 0 )    << "Second: " << sysTime.stSecond << std::endl;
    EXPECT_LE( sysTime.stSecond, 59 )   << "Second: " << sysTime.stSecond << std::endl;

    EXPECT_GE( sysTime.stMillisecs, 0 ) << "MSec:   " << sysTime.stMillisecs << std::endl;
    EXPECT_LE( sysTime.stMillisecs, 999)<< "MSec:   " << sysTime.stMillisecs << std::endl;

    EXPECT_GE( sysTime.stMicrosecs, 0 ) << "MuSec:  " << sysTime.stMicrosecs << std::endl;
    EXPECT_LE( sysTime.stMicrosecs,999) << "MuSec:  " << sysTime.stMicrosecs << std::endl;

    String timestamp = date.formatTime( DateTime::TIME_FORMAT_ISO8601_OUTPUT );
    ASSERT_FALSE( timestamp.isEmpty( ) );

    char buf[ 128 ];
    String::formatString( buf, 128, "%04u-", sysTime.stYear );
    ASSERT_EQ( timestamp.findFirst( buf ), NEString::START_POS );

    String::formatString( buf, 128, "-%02u-", sysTime.stMonth );
    ASSERT_TRUE( timestamp.isValidPosition(timestamp.findFirst( buf )) );

    String::formatString( buf, 128, "-%02u", sysTime.stDay );
    ASSERT_TRUE( timestamp.isValidPosition(timestamp.findFirst( buf )) );

    String::formatString( buf, 128, " %02u:", sysTime.stHour );
    ASSERT_TRUE( timestamp.isValidPosition( timestamp.findFirst( buf ) ) );

    String::formatString( buf, 128, ":%02u:", sysTime.stMinute );
    ASSERT_TRUE( timestamp.isValidPosition( timestamp.findFirst( buf ) ) );

    String::formatString( buf, 128, ":%02u,", sysTime.stSecond );
    ASSERT_TRUE( timestamp.isValidPosition( timestamp.findFirst( buf ) ) );

    String::formatString( buf, 128, ",%03u", sysTime.stMillisecs );
    ASSERT_TRUE( timestamp.isValidPosition( timestamp.findFirst( buf ) ) );
}
