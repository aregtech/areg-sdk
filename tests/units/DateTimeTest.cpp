/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/DateTimeTest.Cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Areg framework unit test file.
 *              Tests of DateTime object.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/DateTime.hpp"

#include "areg/base/MemoryDefs.hpp"
#include "areg/base/MathDefs.hpp"
#include "areg/base/UtilityDefs.hpp"
#include "areg/base/String.hpp"
#include "areg/base/Thread.hpp"

#ifdef WINDOWS
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif // !NOMINMAX
    #include <Windows.h>
#endif  // WINDOWS

namespace
{
#ifdef WINDOWS
    //!< Converts Win system time to the areg specific time structure.
    inline void _convWinSysTime2AregSysTime( const SYSTEMTIME & winTime, areg::CalendarTime & aregTime )
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
    inline void _convAregSysTime2WinSysTime( const areg::CalendarTime & aregTime, SYSTEMTIME & winTime )
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

    //!< Checks the values of areg::CalendarTime type parameter and if does not match
    //!< the expectation, outputs the message to be visible on console.
    //!< The developer passes 'prefix' to identify the value it checks.
    //!< Otherwise, no prefix is displayed.
    void _checkTimeStruct( const areg::CalendarTime & time, const char * prefix = "" )
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
TEST( DateTimeTest, test_now )
{
    areg::CalendarTime sysTime;
    areg::DateTime date( areg::DateTime::now( ) );
    ASSERT_TRUE( date.time( ) != 0 );

    date.date_time( sysTime );
    _checkTimeStruct( sysTime, "UTC " );
}

TEST( DateTimeTest, test_operators)
{
    using namespace std::chrono_literals;

    areg::DateTime dateOld( areg::DateTime::now( ) );

    areg::Thread::sleep( areg::WAIT_100_MILLISECONDS );

    areg::DateTime dateNew( areg::DateTime::now( ) );

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
TEST( DateTimeTest, test_local_time_win32 )
{
#ifdef WINDOWS

    areg::DateTime date( areg::DateTime::now( ) );
    ASSERT_TRUE( date.time( ) != 0 );

    areg::CalendarTime utcTime;
    areg::to_system_time( date.time( ), utcTime );
    _checkTimeStruct( utcTime, "UTC Areg " );

    areg::CalendarTime localTime{ };
    TIME_ZONE_INFORMATION tzi{ };
    SYSTEMTIME utc{ };
    SYSTEMTIME local{ };

    _convAregSysTime2WinSysTime( utcTime, utc );
    _checkTimeStruct( utc, "UTC Win SYS " );

    ASSERT_TRUE( SystemTimeToTzSpecificLocalTime( &tzi, &utc, &local ) );
    _checkTimeStruct( local, "UTC Win Local " );

    _convWinSysTime2AregSysTime( local, localTime );
    localTime.stMicrosecs = utcTime.stMicrosecs;
    _checkTimeStruct( localTime, "Local Areg " );

#endif // WINDOWS
}

/**
 * \brief   Gets current time in UTC and converts to local time.
 *          Checks that values are correct.
 **/
TEST( DateTimeTest, test_local_time )
{
    areg::DateTime date( areg::DateTime::now( ) );
    ASSERT_TRUE( date.time( ) != 0 );

    areg::CalendarTime utcTime;
    areg::to_system_time( date.time( ), utcTime );
    _checkTimeStruct( utcTime, "Areg UTC " );

    time_t secs1 = areg::to_seconds(date.time());
    time_t secs2 = areg::to_seconds(utcTime);
    ASSERT_EQ(secs1, secs2);

    TIME64 micro = areg::to_time(utcTime);
    ASSERT_EQ(date.time(), micro);

    areg::CalendarTime localTime{ };
    areg::to_local_time( utcTime, localTime );
    _checkTimeStruct( localTime, "Areg Local " );

    areg::CalendarTime sysTime;
    areg::to_local_time( date.time( ), sysTime );

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
TEST( DateTimeTest, test_format_iso8601 )
{
    areg::DateTime date( areg::DateTime::now( ) );
    ASSERT_TRUE( date.time( ) != 0 );

    areg::CalendarTime utcTime;
    areg::to_system_time( date.time( ), utcTime );
    _checkTimeStruct( utcTime, "Areg UTC " );

    areg::CalendarTime localTime{ };
    areg::to_local_time( utcTime, localTime );
    _checkTimeStruct( localTime, "Areg Local " );

    struct tm conv { };
    areg::to_tm( localTime, conv );

    _checkTimeStruct( conv, "struct tm Local " );


    areg::CalendarTime sysTime;
    areg::to_local_time( date.time( ), sysTime );

    areg::String timestamp = date.format_time( areg::TIME_FORMAT_ISO8601_OUTPUT );
    ASSERT_FALSE( timestamp.is_empty( ) );

    char buf[ 128 ];
    areg::String::format_string( buf, 128, "%04u-", sysTime.stYear );
    ASSERT_EQ( timestamp.find_first( buf ), areg::START_POS );

    areg::String::format_string( buf, 128, "-%02u-", sysTime.stMonth );
    ASSERT_TRUE( timestamp.is_valid_position(timestamp.find_first( buf )) );

    areg::String::format_string( buf, 128, "-%02u", sysTime.stDay );
    ASSERT_TRUE( timestamp.is_valid_position(timestamp.find_first( buf )) );

    areg::String::format_string( buf, 128, " %02u:", sysTime.stHour );
    ASSERT_TRUE( timestamp.is_valid_position( timestamp.find_first( buf ) ) );

    areg::String::format_string( buf, 128, ":%02u:", sysTime.stMinute );
    ASSERT_TRUE( timestamp.is_valid_position( timestamp.find_first( buf ) ) );

    areg::String::format_string( buf, 128, ":%02u,", sysTime.stSecond );
    ASSERT_TRUE( timestamp.is_valid_position( timestamp.find_first( buf ) ) );

    areg::String::format_string( buf, 128, ",%03u", sysTime.stMillisecs );
    ASSERT_TRUE( timestamp.is_valid_position( timestamp.find_first( buf ) ) );
}

/**
 * \brief   Tests system and process tick count getters.
 **/
TEST(DateTimeTest, test_time_tick)
{
    {
        uint64_t startTicks = areg::DateTime::system_tick_count();
        uint64_t endTicks = areg::DateTime::system_tick_count();
        EXPECT_GT(startTicks, 0u);
        EXPECT_GE(endTicks, startTicks);
    }
    {
        uint64_t startTicks = areg::DateTime::process_tick_count();
        uint64_t endTicks = areg::DateTime::process_tick_count();
        EXPECT_GT(startTicks, 0u);
        EXPECT_GE(endTicks, startTicks);

    }

}

/**
 * \brief   Tests DateTime validity state, time getters/setters.
 *          And method is_valid()
 **/
TEST(DateTimeTest, test_time_validity)
{
    areg::DateTime date(areg::DateTime::now());
    auto time = date.time();
    auto timecast = static_cast<TIME64>(date);

    EXPECT_EQ(time, timecast);

    date.set_time(10000u);
    auto timeNew = date.time();

    EXPECT_NE(time, timeNew);
    EXPECT_EQ(timeNew, 10000u);

    EXPECT_TRUE(date.is_valid());

    date.set_time(areg::DateTime::INVALID_TIME);
    EXPECT_FALSE(date.is_valid());

}

/**
 * \brief   Test converts Date to year, day , month
 *          _decompose() private method test.
 * */
TEST(DateTimeTest, Date_converts_decompose_Test)
{
    constexpr TIME64 customTime = 1704112245500250u;
    areg::DateTime date(areg::DateTime::now( ));
    date.set_time(customTime);

    auto month = date.month();
    auto day   = date.day();
    auto year  = date.year();

    EXPECT_EQ(year, 2024u);
    EXPECT_EQ(day, 1u);
    EXPECT_EQ(month, 1u);

}
/**
 * \brief   Tests chrono time function and time components
 *          seconds() milliseconds() microseconds()
 **/
TEST(DateTimeTest, test_chrono_conversions_and_time_components)
{
        constexpr TIME64 customTime = 2345678u;

        areg::DateTime date;
        date.set_time(customTime);

        std::chrono::microseconds chronoMicro = date;
        std::chrono::milliseconds chronoMilli = date;
        std::chrono::seconds chronoSec = date;

        EXPECT_EQ(chronoMicro.count(), 2345678);
        EXPECT_EQ(chronoMilli.count(), 2345);
        EXPECT_EQ(chronoSec.count(), 2);

        EXPECT_EQ(date.seconds(), 2u);
        EXPECT_EQ(date.milliseconds(), 345u);
        EXPECT_EQ(date.microseconds(), 678u);

}

/**
 * \brief   Tests clock hours, minutes and seconds decomposition.
 **/
TEST(DateTimeTest, test_time_hours_components)
{
    constexpr TIME64 customTime = 1700000000654321u;
    areg::DateTime date;
    date.set_time(customTime);


    auto hours   = date.hours();
    auto minutes = date.minutes();
    auto seconds =  date.seconds();

    EXPECT_EQ(hours, 22u);
    EXPECT_EQ(minutes, 13u);
    EXPECT_EQ(seconds, 20u);

}

/**
 * \brief   Tests day of year and day of week calendar values.
 **/
TEST(DateTimeTest, test_day_domponents)
{
    constexpr TIME64 customTime = 1704067200000000u;
    areg::DateTime date;
    date.set_time(customTime);

    auto dayOY = date.day_of_year();
    auto dayOW = date.day_of_week();

    EXPECT_EQ(dayOY, 1u);
    EXPECT_EQ(dayOW, 2u);
}

/**
 * \brief   Tests date conversion to CalendarTime and struct tm structures.
 **/
TEST(DateTimeTest, test_date_fnction)
{
    constexpr TIME64 customTime = 1704112245500250u;
    areg::CalendarTime cal;
    areg::DateTime date;
    date.set_time(customTime);

    date.date_time(cal);

    {
        EXPECT_EQ(cal.stSecond, 45);
        EXPECT_EQ(cal.stMinute, 30);
        EXPECT_EQ(cal.stHour, 12);
        EXPECT_EQ(cal.stDay, 1);
        EXPECT_EQ(cal.stMonth, 1);
        EXPECT_EQ(cal.stYear, 2024);
        EXPECT_EQ(cal.stDayOfWeek, 2);
        EXPECT_EQ(cal.stDayOfYear, 1);
    }

    {
        areg::DateTime calToDate;
        calToDate.set_date_time(cal);

        EXPECT_EQ(static_cast<TIME64>(calToDate), customTime);
    }

    {
        struct tm tmTime {};
        date.date_time(tmTime);

        EXPECT_EQ(tmTime.tm_sec, 45);
        EXPECT_EQ(tmTime.tm_min, 30);
        EXPECT_EQ(tmTime.tm_hour, 12);
        EXPECT_EQ(tmTime.tm_mday, 1);
        EXPECT_EQ(tmTime.tm_mon, 0);
        EXPECT_EQ(tmTime.tm_year, 124);
        EXPECT_EQ(tmTime.tm_wday, 1);
        EXPECT_EQ(tmTime.tm_yday, 0);

        areg::DateTime tmDate;
        tmDate.set_date_time(tmTime);

        // I'm setting the time to the nearest whole second.
        constexpr TIME64 customTimeInSecs = (customTime / areg::SEC_TO_MICROSECS) * areg::SEC_TO_MICROSECS;
        EXPECT_EQ(static_cast<TIME64>(tmDate), customTimeInSecs);

    }

}