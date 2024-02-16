/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/private/DateTime.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Date and Time class.
 *
 ************************************************************************/
#include "areg/base/DateTime.hpp"
#include "areg/base/NEUtilities.hpp"

#include <time.h>

namespace 
{
    /**
     * \brief   Output format of milliseconds
     **/
    constexpr std::string_view   FORMAT_MILLISECOND{ "%l" };
}


DateTime::DateTime( void )
    : mDateTime ( INVALID_TIME )
{
}

DateTime::DateTime( const NEUtilities::sSystemTime & sysTime )
    : mDateTime( NEUtilities::convToTime(sysTime) )
{
}

DateTime::DateTime( const DateTime & dateTime )
    : mDateTime ( dateTime.mDateTime )
{
}

DateTime::DateTime( DateTime && dateTime ) noexcept
    : mDateTime ( dateTime.mDateTime )
{
}

DateTime::DateTime( const TIME64 & dateTime )
    : mDateTime ( dateTime )
{
}

DateTime::DateTime( const IEInStream & stream )
    : mDateTime ( stream.read64Bits() )
{
}

uint64_t DateTime::getProcessTickCount(void)
{
#ifdef WINDOWS
    
    return static_cast<uint64_t>( clock() );

#else   // _POSIX

    struct timespec ts;
    ::clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts);
    return ((ts.tv_sec * NEUtilities::SEC_TO_MILLISECS)+ (ts.tv_nsec / NEUtilities::MILLISEC_TO_NS));

#endif // WINDOWS

}

void DateTime::formatTime(const DateTime& dateTime, String& OUT result, const std::string_view& formatName)
{
    char buffer[128] = { 0 };

    if (dateTime.mDateTime != 0)
    {
        time_t secs;
        unsigned short milli, micro;
        struct tm conv { 0 };
        NEUtilities::convMicrosecs(dateTime.mDateTime, secs, milli, micro);
        NEUtilities::convToLocalTm(dateTime.mDateTime, conv);

        String str(formatName.empty() == false ? formatName : DateTime::TIME_FORMAT_ISO8601_OUTPUT);
        NEString::CharPos ms = str.findFirst(FORMAT_MILLISECOND.data());
        if (str.isValidPosition(ms))
        {
            char buf[128];
            String::formatString(buf, 128, "%03u", static_cast<uint32_t>(milli));
            str.replace(ms, static_cast<NEString::CharCount>(FORMAT_MILLISECOND.length()), buf);
        }

        std::size_t count{ std::strftime(buffer, 128, str.getString(), &conv) };

        result.assign(buffer, static_cast<NEString::CharCount>(count));
    }
    else
    {
        result = String::EmptyChar;
    }
}

uint64_t DateTime::getSystemTickCount( void )
{
    return NEUtilities::getTickCount();
}

bool DateTime::operator == (const DateTime & other) const
{
    return (mDateTime == other.mDateTime);
}

bool DateTime::operator != (const DateTime & other) const
{
    return (mDateTime != other.mDateTime);
}

bool DateTime::operator > (const DateTime & other) const
{
    return (mDateTime > other.mDateTime);
}

bool DateTime::operator < (const DateTime & other) const
{
    return (mDateTime < other.mDateTime);
}

bool DateTime::operator >= (const DateTime & other) const
{
    return (mDateTime >= other.mDateTime);
}

bool DateTime::operator <= (const DateTime & other) const
{
    return (mDateTime <= other.mDateTime);
}

DateTime DateTime::getNow( void )
{
    return DateTime( NEUtilities::systemTimeNow() );
}

void DateTime::getNow( NEUtilities::sSystemTime & OUT timeData, bool localTime )
{
    NEUtilities::systemTimeNow(timeData, localTime);
}

String DateTime::formatTime( const std::string_view & formatName /*= DateTime::TIME_FORMAT_ISO8601_OUTPUT */ ) const
{
    String result;
    DateTime::formatTime(*this, result, formatName);
    return result;
}

unsigned int DateTime::getYear(void) const
{
    constexpr double   _secsInYear{ 60.0 * 60.0 * 24.0 * 365.25 };
    constexpr uint32_t _unixEpoch{ 1970 };

    uint64_t secs{ mDateTime / NEUtilities::SEC_TO_MICROSECS };
    return static_cast<unsigned int>(static_cast<double>(secs) / _secsInYear) + _unixEpoch;
}

unsigned int DateTime::getMonth(void) const
{
    // Define the number of days in each month
    constexpr uint32_t _DAYS_IN_MONTH[] { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    // the result (month)
    uint32_t month{ 1u };

    // Calculate the number of days
    uint32_t days{ static_cast<uint32_t>(mDateTime / NEUtilities::DAY_TO_MICROSECS) };

    // Calculate the number of years and remaining days
    uint32_t years = days / 365;
    uint32_t remainDays = days % 365;

    // Adjust for leap years
    uint32_t leapYears = (years + 1) / 4;
    remainDays -= leapYears;

    // Find the current month
    while (remainDays >= _DAYS_IN_MONTH[month - 1])
    {
        remainDays -= _DAYS_IN_MONTH[month - 1];
        ++month;
    }

    return month;
}

unsigned int DateTime::getDay(void) const
{
    // Define the number of days in each month
    constexpr uint32_t _DAYS_IN_MONTH[]{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    // Calculate the number of days
    uint32_t days{ static_cast<uint32_t>(mDateTime / NEUtilities::DAY_TO_MICROSECS) };

    // Calculate the number of years and remaining days
    uint32_t years = days / 365;
    uint32_t remainDays = days % 365;

    // Adjust for leap years
    uint32_t leapYears = (years + 1) / 4;
    remainDays -= leapYears;

    // Current month
    uint32_t curMonth{ 1u };
    // Find the current month
    while (remainDays >= _DAYS_IN_MONTH[curMonth - 1])
    {
        remainDays -= _DAYS_IN_MONTH[curMonth - 1];
        ++curMonth;
    }

    // Find the current day within the month
    return (remainDays + 1); // Days are 1-based
}

unsigned int DateTime::getHours(void) const
{
    uint64_t secs{ mDateTime / NEUtilities::SEC_TO_MICROSECS };
    uint32_t remainSecs{ static_cast<uint32_t>(secs % NEUtilities::DAY_TO_SECS) };
    return (remainSecs / NEUtilities::HOUR_TO_SECS);
}

unsigned int DateTime::getMinutex(void) const
{
    uint64_t secs{ mDateTime / NEUtilities::SEC_TO_MICROSECS };
    uint32_t remainSecs{ static_cast<uint32_t>(secs % NEUtilities::DAY_TO_SECS) };
    uint32_t mins{ static_cast<uint32_t>((remainSecs % NEUtilities::HOUR_TO_SECS) / NEUtilities::MIN_TO_SECS) };
    return mins;
}

unsigned int DateTime::getSecons(void) const
{
    uint64_t secs{ mDateTime / NEUtilities::SEC_TO_MICROSECS };
    uint32_t remainSecs{ static_cast<uint32_t>(secs % NEUtilities::DAY_TO_SECS) };
    uint32_t result{ static_cast<uint32_t>((remainSecs % NEUtilities::HOUR_TO_SECS) % NEUtilities::MIN_TO_SECS) };
    return result;
}

unsigned int DateTime::getMilliscones(void) const
{
    return static_cast<uint32_t>( (mDateTime / NEUtilities::MILLISEC_TO_MICROSECS) % NEUtilities::MILLISEC_TO_MICROSECS );
}

unsigned int DateTime::getMicroseconds(void) const
{
    return static_cast<uint32_t>(mDateTime % NEUtilities::SEC_TO_MICROSECS);
}

unsigned int DateTime::getDayOfTheYear(void) const
{
    // Define the number of days in each month
    constexpr uint32_t _DAYS_IN_MONTH[]{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    // Calculate the number of days
    uint32_t days{ static_cast<uint32_t>(mDateTime / NEUtilities::DAY_TO_MICROSECS) };

    // Calculate the number of years and remaining days
    uint32_t years = days / 365;
    uint32_t remainDays = days % 365;

    // Adjust for leap years
    uint32_t leapYears = (years + 1) / 4;
    remainDays -= leapYears;

    // the result (month)
    uint32_t curMonth{ 1u };
    // Find the current month
    while (remainDays >= _DAYS_IN_MONTH[curMonth - 1])
    {
        remainDays -= _DAYS_IN_MONTH[curMonth - 1];
        ++curMonth;
    }

    // Find the current day within the month
    uint32_t curDay = (remainDays + 1); // Days are 1-based
    // Calculate the day of the year
    uint32_t dayOfYear = 0;
    for (uint32_t i = 0; i < curMonth - 1; ++i)
    {
        dayOfYear += _DAYS_IN_MONTH[i];
    }

    dayOfYear += curDay;

    return dayOfYear;
}

unsigned int DateTime::getDayOfTheWeek(void) const
{
    // Define the number of days in each month
    constexpr uint32_t _DAYS_IN_MONTH[]{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    // Calculate the number of days
    uint32_t days{ static_cast<uint32_t>(mDateTime / NEUtilities::DAY_TO_MICROSECS) };

    // Calculate the number of years and remaining days
    uint32_t years = days / 365;
    uint32_t remainDays = days % 365;

    // Adjust for leap years
    uint32_t leapYears = (years + 1) / 4;
    remainDays -= leapYears;

    // the result (month)
    uint32_t curMonth{ 1u };
    // Find the current month
    while (remainDays >= _DAYS_IN_MONTH[curMonth - 1])
    {
        remainDays -= _DAYS_IN_MONTH[curMonth - 1];
        ++curMonth;
    }

    // Find the current day within the month
    uint32_t curDay = (remainDays + 1); // Days are 1-based
    // Calculate the day of the week (Sunday is 0, Monday is 1, and so on)
    // Zeller's Congruence formula
    uint32_t dayOfWeek = (curDay + ((13 * (curMonth + 1)) / 5) + years + (years / 4) - (years / 100) + (years / 400)) % 7;

    // Adjust for zero-based indexing of days
    return ((dayOfWeek + 6) % 7);
}

void DateTime::getDateTime(NEUtilities::sSystemTime& OUT sysTime)
{
    NEUtilities::convToSystemTime(mDateTime, sysTime);
}

void DateTime::setDateTime(const NEUtilities::sSystemTime& IN sysTime)
{
    mDateTime = NEUtilities::convToTime(sysTime);
}

void DateTime::getDateTime(tm& OUT time)
{
    NEUtilities::convToTm(mDateTime, time);
}

void DateTime::setDateTime(const tm& IN time)
{
    mDateTime = NEUtilities::convToTime(time);
}
