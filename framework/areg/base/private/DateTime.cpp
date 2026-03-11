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


DateTime::DateTime( const InStream & stream )
    : mDateTime ( stream.read64_bits() )
{
}

uint64_t DateTime::process_tick_count()
{
    return areg::tick_count();
}

void DateTime::format_time(const DateTime& dateTime, String& result, std::string_view formatName /*= areg::DEFAULT_TIME_FORMAT_OUTPUT*/)
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

DateTime DateTime::now()
{
    return DateTime( areg::system_time_now() );
}

TIME64 DateTime::timestamp()
{
    return areg::system_time_now();
}

void DateTime::now( areg::CalendarTime & timeData, bool localTime )
{
    areg::system_time_now(timeData, localTime);
}

String DateTime::format_time( std::string_view formatName /*= areg::DEFAULT_TIME_FORMAT_OUTPUT */ ) const
{
    String result;
    DateTime::format_time(*this, result, formatName);
    return result;
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
