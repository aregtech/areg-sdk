/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        units/DateTimeTest.Cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, AREG framework unit test file.
 *              Tests of DateTime object.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/DateTime.hpp"

#include "areg/base/NEMemory.hpp"
#include "areg/base/NEMath.hpp"
#include "areg/base/NEUtilities.hpp"
#include "areg/base/String.hpp"
#include "areg/base/Thread.hpp"

#ifdef WINDOWS
    #include <Windows.h>
#endif  // WINDOWS

namespace
{
#ifdef WINDOWS
    //!< Converts Win system time to the areg specific time structure.
    inline void _convWinSysTime2AregSysTime( const SYSTEMTIME & IN winTime, NEUtilities::sSystemTime & OUT aregTime )
    {
        aregTime.stYear = static_cast<int>(winTime.wYear);
        aregTime.stMonth = static_cast<int>(winTime.wMonth);
        aregTime.stDayOfWeek = static_cast<int>(winTime.wDayOfWeek + 1);
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
        winTime.wDayOfWeek = static_cast<WORD>(aregTime.stDayOfWeek - 1);
        winTime.wDay = static_cast<WORD>(aregTime.stDay);
        winTime.wHour = static_cast<WORD>(aregTime.stHour);
        winTime.wMinute = static_cast<WORD>(aregTime.stMinute);
        winTime.wSecond = static_cast<WORD>(aregTime.stSecond);
        winTime.wMilliseconds = static_cast<WORD>(aregTime.stMillisecs);
    }

    //!< Checks the values of SYSTEMTIME type parameter and if does not match
    //!< the expectation, outputs the message to be visible on console.
    //!< The developer passes 'prefix' to identify the value it checks.
    //!< Otherwise, no prefix is displayed.
    void _checkTimeStruct( const SYSTEMTIME & time, const char * prefix = "" )
    {
        EXPECT_GE( time.wYear, 2023 )       << prefix << "Year:   " << time.wYear << std::endl;

        EXPECT_GE( time.wMonth, 1 )         << prefix << "Month:  " << time.wMonth << std::endl;
        EXPECT_LE( time.wMonth, 12 )        << prefix << "Month:  " << time.wMonth << std::endl;

        EXPECT_GE( time.wDayOfWeek, 0 )     << prefix << "DoF:    " << time.wDayOfWeek << std::endl;
        EXPECT_LE( time.wDayOfWeek, 6 )     << prefix << "DoF:    " << time.wDayOfWeek << std::endl;

        EXPECT_GE( time.wDay, 1 )           << prefix << "Day:    " << time.wDay << std::endl;
        EXPECT_LE( time.wDay, 31 )          << prefix << "Day:    " << time.wDay << std::endl;

        EXPECT_GE( time.wHour, 0 )          << prefix << "Hour:   " << time.wHour << std::endl;
        EXPECT_LE( time.wHour, 23 )         << prefix << "Hour:   " << time.wHour << std::endl;

        EXPECT_GE( time.wMinute, 0 )        << prefix << "Minute: " << time.wMinute << std::endl;
        EXPECT_LE( time.wMinute, 59 )       << prefix << "Minute: " << time.wMinute << std::endl;

        EXPECT_GE( time.wSecond, 0 )        << prefix << "Second: " << time.wSecond << std::endl;
        EXPECT_LE( time.wSecond, 59 )       << prefix << "Second: " << time.wSecond << std::endl;

        EXPECT_GE( time.wMilliseconds, 0 )  << prefix << "MSec:   " << time.wMilliseconds << std::endl;
        EXPECT_LE( time.wMilliseconds, 999 )<< prefix << "MSec:   " << time.wMilliseconds << std::endl;
    }

#endif  // WINDOWS

    //!< Checks the values of NEUtilities::sSystemTime type parameter and if does not match
    //!< the expectation, outputs the message to be visible on console.
    //!< The developer passes 'prefix' to identify the value it checks.
    //!< Otherwise, no prefix is displayed.
    void _checkTimeStruct( const NEUtilities::sSystemTime & time, const char * prefix = "" )
    {
        EXPECT_GE( time.stYear, 2023 )      << prefix << "Year:   " << time.stYear << std::endl;

        EXPECT_GE( time.stMonth, 1 )        << prefix << "Month:  " << time.stMonth << std::endl;
        EXPECT_LE( time.stMonth, 12 )       << prefix << "Month:  " << time.stMonth << std::endl;

        EXPECT_GE( time.stDayOfWeek, 1 )    << prefix << "DoF:    " << time.stDayOfWeek << std::endl;
        EXPECT_LE( time.stDayOfWeek, 7 )    << prefix << "DoF:    " << time.stDayOfWeek << std::endl;

        EXPECT_GE( time.stDay, 1 )          << prefix << "Day:    " << time.stDay << std::endl;
        EXPECT_LE( time.stDay, 31 )         << prefix << "Day:    " << time.stDay << std::endl;

        EXPECT_GE( time.stHour, 0 )         << prefix << "Hour:   " << time.stHour << std::endl;
        EXPECT_LE( time.stHour, 23 )        << prefix << "Hour:   " << time.stHour << std::endl;

        EXPECT_GE( time.stMinute, 0 )       << prefix << "Minute: " << time.stMinute << std::endl;
        EXPECT_LE( time.stMinute, 59 )      << prefix << "Minute: " << time.stMinute << std::endl;

        EXPECT_GE( time.stSecond, 0 )       << prefix << "Second: " << time.stSecond << std::endl;
        EXPECT_LE( time.stSecond, 59 )      << prefix << "Second: " << time.stSecond << std::endl;

        EXPECT_GE( time.stMillisecs, 0 )    << prefix << "MSec:   " << time.stMillisecs << std::endl;
        EXPECT_LE( time.stMillisecs, 999 )  << prefix << "MSec:   " << time.stMillisecs << std::endl;

        EXPECT_GE( time.stMicrosecs, 0 )    << prefix << "MuSec:  " << time.stMicrosecs << std::endl;
        EXPECT_LE( time.stMicrosecs, 999 )  << prefix << "MuSec:  " << time.stMicrosecs << std::endl;
    }

    //!< Checks the values of struct tm type parameter and if does not match
    //!< the expectation, outputs the message to be visible on console.
    //!< The developer passes 'prefix' to identify the value it checks.
    //!< Otherwise, no prefix is displayed.
    void _checkTimeStruct( const struct tm & time, const char * prefix = "" )
    {
        constexpr int year{ 2023 - 1900 };

        EXPECT_GE( time.tm_year, year ) << prefix << "Year:   " << time.tm_year << std::endl;

        EXPECT_GE( time.tm_mon, 0 )     << prefix << "Month:  " << time.tm_mon << std::endl;
        EXPECT_LE( time.tm_mon, 11 )    << prefix << "Month:  " << time.tm_mon << std::endl;

        EXPECT_GE( time.tm_wday, 0 )    << prefix << "DoF:    " << time.tm_wday << std::endl;
        EXPECT_LE( time.tm_wday, 6 )    << prefix << "DoF:    " << time.tm_wday << std::endl;

        EXPECT_GE( time.tm_mday, 1 )    << prefix << "Day:    " << time.tm_mday << std::endl;
        EXPECT_LE( time.tm_mday, 31 )   << prefix << "Day:    " << time.tm_mday << std::endl;

        EXPECT_GE( time.tm_hour, 0 )    << prefix << "Hour:   " << time.tm_hour << std::endl;
        EXPECT_LE( time.tm_hour, 23 )   << prefix << "Hour:   " << time.tm_hour << std::endl;

        EXPECT_GE( time.tm_min, 0 )     << prefix << "Minute: " << time.tm_min << std::endl;
        EXPECT_LE( time.tm_min, 59 )    << prefix << "Minute: " << time.tm_min << std::endl;

        EXPECT_GE( time.tm_sec, 0 )     << prefix << "Second: " << time.tm_sec << std::endl;
        EXPECT_LE( time.tm_sec, 59 )    << prefix << "Second: " << time.tm_sec << std::endl;
    }
}

/**
 * \brief   Checks current date-time value.
 **/
TEST( DateTimeTest, TestNow )
{
    NEUtilities::sSystemTime sysTime;
    DateTime date( DateTime::getNow( ) );
    ASSERT_TRUE( date.getTime( ) != 0 );

    date.getDateTime( sysTime );
    _checkTimeStruct( sysTime, "UTC " );
}

TEST( DateTimeTest, TestOperators)
{
    using namespace std::chrono_literals;

    DateTime dateOld( DateTime::getNow( ) );

    Thread::sleep( NECommon::WAIT_100_MILLISECONDS );

    DateTime dateNew( DateTime::getNow( ) );

    ASSERT_TRUE( dateOld < dateNew);
    ASSERT_TRUE( dateOld <= dateNew);
    ASSERT_TRUE( dateOld != dateNew);
    ASSERT_TRUE( dateNew > dateOld);
    ASSERT_TRUE( dateNew >= dateOld);
}

/**
 * \brief   Converts current system time in UTC to local time
 *          by using Win32 API to make sure that the sequence of
 *          function calls is correct.
 **/
TEST( DateTimeTest, TestLocalTimeWin32 )
{
#ifdef WINDOWS

    DateTime date( DateTime::getNow( ) );
    ASSERT_TRUE( date.getTime( ) != 0 );

    NEUtilities::sSystemTime utcTime;
    NEUtilities::convToSystemTime( date.getTime( ), utcTime );
    _checkTimeStruct( utcTime, "UTC AREG " );

    NEUtilities::sSystemTime localTime{ };
    TIME_ZONE_INFORMATION tzi{ };
    SYSTEMTIME utc{ };
    SYSTEMTIME local{ };

    _convAregSysTime2WinSysTime( utcTime, utc );
    _checkTimeStruct( utc, "UTC Win SYS " );

    ASSERT_TRUE( SystemTimeToTzSpecificLocalTime( &tzi, &utc, &local ) );
    _checkTimeStruct( local, "UTC Win Local " );

    _convWinSysTime2AregSysTime( local, localTime );
    localTime.stMicrosecs = utcTime.stMicrosecs;
    _checkTimeStruct( localTime, "Local AREG " );

#endif // WINDOWS
}

/**
 * \brief   Gets current time in UTC and converts to local time.
 *          Checks that values are correct.
 **/
TEST( DateTimeTest, TestLocalTime )
{
    DateTime date( DateTime::getNow( ) );
    ASSERT_TRUE( date.getTime( ) != 0 );

    NEUtilities::sSystemTime utcTime;
    NEUtilities::convToSystemTime( date.getTime( ), utcTime );
    _checkTimeStruct( utcTime, "Areg UTC " );

    time_t secs1 = NEUtilities::convToSeconds(date.getTime());
    time_t secs2 = NEUtilities::convToSeconds(utcTime);
    ASSERT_EQ(secs1, secs2);

    TIME64 micro = NEUtilities::convToTime(utcTime);
    ASSERT_EQ(date.getTime(), micro);

    NEUtilities::sSystemTime localTime{ };
    NEUtilities::convToLocalTime( utcTime, localTime );
    _checkTimeStruct( localTime, "Areg Local " );

    NEUtilities::sSystemTime sysTime;
    NEUtilities::convToLocalTime( date.getTime( ), sysTime );

    ASSERT_EQ( localTime.stYear, sysTime.stYear )   << "localTime.stYear = " << localTime.stYear    << ", sysTime.stYear = "  << sysTime.stYear << std::endl;
    ASSERT_EQ( localTime.stMonth, sysTime.stMonth ) << "localTime.stMonth= " << localTime.stMonth   << ", sysTime.stMonth = " << sysTime.stMonth << std::endl;
    ASSERT_EQ( localTime.stDay, sysTime.stDay )     << "localTime.stDay = "  << localTime.stDay     << ", sysTime.stDay = "   << sysTime.stDay << std::endl;
    ASSERT_EQ( localTime.stDayOfWeek, sysTime.stDayOfWeek ) << "localTime.stDayOfWeek = " << localTime.stDayOfWeek << ", sysTime.stDayOfWeek = " << sysTime.stDayOfWeek << std::endl;
    ASSERT_EQ( localTime.stHour, sysTime.stHour ) << "localTime.stHour = " << localTime.stHour << ", sysTime.stHour = " << sysTime.stHour << std::endl;
    ASSERT_EQ( localTime.stMinute, sysTime.stMinute ) << "localTime.stMinute = " << localTime.stMinute << ", sysTime.stMinute = " << sysTime.stMinute << std::endl;
    ASSERT_EQ( localTime.stSecond, sysTime.stSecond ) << "localTime.stSecond = " << localTime.stSecond << ", sysTime.stSecond = " << sysTime.stSecond << std::endl;
    ASSERT_EQ( localTime.stMillisecs, sysTime.stMillisecs ) << "localTime.stMillisecs = " << localTime.stMillisecs << ", sysTime.stMillisecs = " << sysTime.stMillisecs << std::endl;
    ASSERT_EQ( localTime.stMicrosecs, sysTime.stMicrosecs ) << "localTime.stMicrosecs = " << localTime.stMicrosecs << ", sysTime.stMicrosecs = " << sysTime.stMicrosecs << std::endl;

}

/**
 * brief    Gets current time in UTC, coverts to string of local time using ISO8601 format
 *          and makes sure that the conversion is correct.
 **/
TEST( DateTimeTest, TestFormatISO8601 )
{
    DateTime date( DateTime::getNow( ) );
    ASSERT_TRUE( date.getTime( ) != 0 );

    NEUtilities::sSystemTime utcTime;
    NEUtilities::convToSystemTime( date.getTime( ), utcTime );
    _checkTimeStruct( utcTime, "Areg UTC " );

    NEUtilities::sSystemTime localTime{ };
    NEUtilities::convToLocalTime( utcTime, localTime );
    _checkTimeStruct( localTime, "Areg Local " );

    struct tm conv { };
    NEUtilities::convToTm( localTime, conv );

    _checkTimeStruct( conv, "struct tm Local " );


    NEUtilities::sSystemTime sysTime;
    NEUtilities::convToLocalTime( date.getTime( ), sysTime );

    String timestamp = date.formatTime( NEUtilities::TIME_FORMAT_ISO8601_OUTPUT );
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
