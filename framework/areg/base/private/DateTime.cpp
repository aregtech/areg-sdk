/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/DateTime.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Date and Time class.
 *
 ************************************************************************/
#include "areg/base/DateTime.hpp"
#include "areg/base/UtilityDefs.hpp"

#include <time.h>
namespace areg {

namespace 
{
    /**
     * \brief   Output format of milliseconds
     **/
    constexpr std::string_view   FORMAT_MILLISECOND{ "%l" };
}


DateTime::DateTime()
    : mDateTime ( INVALID_TIME )
{
}

DateTime::DateTime( const areg::CalendarTime & sysTime )
    : mDateTime( areg::to_time(sysTime) )
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

DateTime::DateTime( const InStream & stream )
    : mDateTime ( stream.read64_bits() )
{
}

uint64_t DateTime::process_tick_count()
{
    return areg::tick_count();
}

void DateTime::format_time(const DateTime& dateTime, String& result, const std::string_view& formatName /*= areg::DEFAULT_TIME_FORMAT_OUTPUT*/)
{
    char buffer[128] = { 0 };

    if (dateTime.mDateTime != 0)
    {
        time_t secs;
        uint16_t milli, micro;
        struct tm conv { };
        areg::conv_microsecs(dateTime.mDateTime, secs, milli, micro);
        areg::to_local_tm(dateTime.mDateTime, conv);

        String str(formatName.empty() == false ? formatName : areg::DEFAULT_TIME_FORMAT_OUTPUT);
        areg::CharPos ms = str.find_first(FORMAT_MILLISECOND.data());
        if (str.is_valid_position(ms))
        {
            char buf[128];
            String::format_string(buf, 128, "%03u", static_cast<uint32_t>(milli));
            str.replace(ms, static_cast<areg::CharCount>(FORMAT_MILLISECOND.length()), buf);
        }

        std::size_t count{ std::strftime(buffer, 128, str.as_string(), &conv) };

        result.assign(buffer, static_cast<areg::CharCount>(count));
    }
    else
    {
        result = String::EmptyChar;
    }
}

uint64_t DateTime::system_tick_count()
{
    return areg::tick_count();
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

DateTime DateTime::now()
{
    return DateTime( areg::system_time_now() );
}

void DateTime::now( areg::CalendarTime & timeData, bool localTime )
{
    areg::system_time_now(timeData, localTime);
}

String DateTime::format_time( const std::string_view & formatName /*= areg::DEFAULT_TIME_FORMAT_OUTPUT */ ) const
{
    String result;
    DateTime::format_time(*this, result, formatName);
    return result;
}

uint32_t DateTime::year() const
{
    constexpr double   _secsInYear{ 60.0 * 60.0 * 24.0 * 365.25 };
    constexpr uint32_t _unixEpoch{ 1970 };

    uint64_t secs{ mDateTime / areg::SEC_TO_MICROSECS };
    return static_cast<uint32_t>(static_cast<double>(secs) / _secsInYear) + _unixEpoch;
}

uint32_t DateTime::month() const
{
    // Define the number of days in each month
    constexpr uint32_t _DAYS_IN_MONTH[] { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    // the result (month)
    uint32_t month{ 1u };

    // Calculate the number of days
    uint32_t days{ static_cast<uint32_t>(mDateTime / areg::DAY_TO_MICROSECS) };

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

uint32_t DateTime::day() const
{
    // Define the number of days in each month
    constexpr uint32_t _DAYS_IN_MONTH[]{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    // Calculate the number of days
    uint32_t days{ static_cast<uint32_t>(mDateTime / areg::DAY_TO_MICROSECS) };

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

uint32_t DateTime::hours() const
{
    uint64_t secs{ mDateTime / areg::SEC_TO_MICROSECS };
    uint32_t remainSecs{ static_cast<uint32_t>(secs % areg::DAY_TO_SECS) };
    return (remainSecs / areg::HOUR_TO_SECS);
}

uint32_t DateTime::minutex() const
{
    uint64_t secs{ mDateTime / areg::SEC_TO_MICROSECS };
    uint32_t remainSecs{ static_cast<uint32_t>(secs % areg::DAY_TO_SECS) };
    uint32_t mins{ static_cast<uint32_t>((remainSecs % areg::HOUR_TO_SECS) / areg::MIN_TO_SECS) };
    return mins;
}

uint32_t DateTime::secons() const
{
    uint64_t secs{ mDateTime / areg::SEC_TO_MICROSECS };
    uint32_t remainSecs{ static_cast<uint32_t>(secs % areg::DAY_TO_SECS) };
    uint32_t result{ static_cast<uint32_t>((remainSecs % areg::HOUR_TO_SECS) % areg::MIN_TO_SECS) };
    return result;
}

uint32_t DateTime::milliscones() const
{
    return static_cast<uint32_t>( (mDateTime / areg::MILLISEC_TO_MICROSECS) % areg::MILLISEC_TO_MICROSECS );
}

uint32_t DateTime::microseconds() const
{
    return static_cast<uint32_t>(mDateTime % areg::SEC_TO_MICROSECS);
}

uint32_t DateTime::day_of_year() const
{
    // Define the number of days in each month
    constexpr uint32_t _DAYS_IN_MONTH[]{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    // Calculate the number of days
    uint32_t days{ static_cast<uint32_t>(mDateTime / areg::DAY_TO_MICROSECS) };

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

uint32_t DateTime::day_of_week() const
{
    // Define the number of days in each month
    constexpr uint32_t _DAYS_IN_MONTH[]{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    // Calculate the number of days
    uint32_t days{ static_cast<uint32_t>(mDateTime / areg::DAY_TO_MICROSECS) };

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

void DateTime::date_time(areg::CalendarTime& sysTime)
{
    areg::to_system_time(mDateTime, sysTime);
}

void DateTime::set_date_time(const areg::CalendarTime& sysTime)
{
    mDateTime = areg::to_time(sysTime);
}

void DateTime::date_time(tm& time)
{
    areg::to_tm(mDateTime, time);
}

void DateTime::set_date_time(const tm& time)
{
    mDateTime = areg::to_time(time);
}

} // namespace areg
