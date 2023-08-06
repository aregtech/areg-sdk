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

TEST( DateTimeTest, TestFormatISO8601 )
{
    char buf[ 128 ];

    NEUtilities::sSystemTime sysTime;
    DateTime date( DateTime::getNow( ) );
    ASSERT_TRUE( date.getTime( ) != 0 );

    String timestamp = date.formatTime( DateTime::TIME_FORMAT_ISO8601_OUTPUT );
    ASSERT_FALSE( timestamp.isEmpty( ) );

    String tmp;
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
