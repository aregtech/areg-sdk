/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/private/NEUtilities.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform Switches
 *
 ************************************************************************/

#include "areg/base/NEUtilities.hpp"
#include "areg/base/SynchObjects.hpp"
#include "areg/base/String.hpp"
#include "areg/base/WideString.hpp"
#include "areg/base/NECommon.hpp"

#include <chrono>
#include <string>
#include <time.h>


/************************************************************************/
// static local functions
/************************************************************************/
namespace NEUtilities 
{
    extern uint64_t _osGetTickCount( void );

    extern void _osSystemTimeNow( sSystemTime & OUT sysTime, bool localTime );

    extern void _osMakeTmLocal(struct tm& IN OUT utcTime);

    extern bool _osConvToLocalTime(const TIME64& IN utcTime, sSystemTime& OUT localTime);

    extern bool _osConvToLocalTm(const TIME64& IN utcTime, struct tm& OUT localTm);

    extern void _osConvToSystemTime(const TIME64& IN timeValue, NEUtilities::sSystemTime& OUT sysTime);

    NEMath::eCompare compareLargeIntegers( const NEMath::uLargeInteger & lhs, const NEMath::uLargeInteger & rhs )
    {
        NEMath::eCompare result = NEMath::eCompare::Equal;
        if ( lhs.u.highPart < rhs.u.highPart )
            result = NEMath::eCompare::Smaller;
        else if ( lhs.u.highPart > rhs.u.highPart )
            result = NEMath::eCompare::Bigger;
        else if ( lhs.u.lowPart < rhs.u.lowPart )
            result = NEMath::eCompare::Smaller;
        else if ( lhs.u.lowPart > rhs.u.lowPart )
            result = NEMath::eCompare::Bigger;

        return result;
    }
}

/************************************************************************/
// NEUtilities namespace global functions
/************************************************************************/

AREG_API_IMPL time_t NEUtilities::convToSeconds(const sSystemTime & sysTime)
{
    const int year{ sysTime.stYear - 1900 };
    return    static_cast<time_t>(sysTime.stSecond)
            + static_cast<time_t>(sysTime.stMinute      * NEUtilities::MIN_TO_SECS)
            + static_cast<time_t>(sysTime.stHour        * NEUtilities::HOUR_TO_SECS)
            + static_cast<time_t>((sysTime.stDayOfYear - 1) * NEUtilities::DAY_TO_SECS)
            + static_cast<time_t>(((year -  70)     )   * NEUtilities::YEAR_TO_SECS)
            + static_cast<time_t>(((year -  69) /   4)  * NEUtilities::DAY_TO_SECS)
            - static_cast<time_t>(((year -   1) / 100)  * NEUtilities::DAY_TO_SECS)
            + static_cast<time_t>(((year + 299) / 400)  * NEUtilities::DAY_TO_SECS);
}

AREG_API_IMPL NEMath::eCompare NEUtilities::compareTimes( const TIME64 & lhs, const TIME64 & rhs )
{
    NEMath::uLargeInteger lhsLi, rshLi;
    lhsLi.quadPart  = lhs;
    rshLi.quadPart  = rhs;

    return NEUtilities::compareLargeIntegers(lhsLi, rshLi);
}

AREG_API_IMPL void NEUtilities::convMicrosecs(const TIME64& IN time, time_t& OUT secs, unsigned short& OUT milli, unsigned short& OUT micro)
{
    secs = static_cast<time_t>(time / NEUtilities::SEC_TO_MICROSECS);
    time_t rest = static_cast<time_t>(time % NEUtilities::SEC_TO_MICROSECS);
    milli = static_cast<unsigned short>(rest / NEUtilities::MILLISEC_TO_MICROSECS);
    micro = static_cast<unsigned short>(rest % NEUtilities::MILLISEC_TO_MICROSECS);
}

AREG_API_IMPL void NEUtilities::convToTm(const sSystemTime & IN sysTime, struct tm & OUT time)
{
    if (sysTime.stYear >= 1900)
    {
        time.tm_sec     = static_cast<int>( sysTime.stSecond);
        time.tm_min     = static_cast<int>( sysTime.stMinute);
        time.tm_hour    = static_cast<int>( sysTime.stHour);
        time.tm_mday    = static_cast<int>( sysTime.stDay);
        time.tm_mon     = static_cast<int>( sysTime.stMonth     -    1);    // tm_mon is 0 based
        time.tm_year    = static_cast<int>( sysTime.stYear      - 1900);    // tm_year is 1900 based
        time.tm_wday    = static_cast<int>( sysTime.stDayOfWeek -    1);    // tm_wday is 0 based
        time.tm_yday    = static_cast<int>( sysTime.stDayOfYear -    1);    // day of year 0 based
        time.tm_isdst   = -1;
    }
    else
    {
        ASSERT( false );
        NEMemory::zeroElement<tm>(time);
    }
}

AREG_API_IMPL void NEUtilities::makeTmLocal( struct tm & IN OUT utcTime )
{
    _osMakeTmLocal(utcTime);
}

AREG_API_IMPL void NEUtilities::convToSystemTime(const struct tm & IN time, sSystemTime & OUT sysTime)
{
    sysTime.stSecond    = static_cast<int>(time.tm_sec);
    sysTime.stMinute    = static_cast<int>(time.tm_min);
    sysTime.stHour      = static_cast<int>(time.tm_hour);
    sysTime.stDay       = static_cast<int>(time.tm_mday);
    sysTime.stMonth     = static_cast<int>(time.tm_mon  +    1);
    sysTime.stYear      = static_cast<int>(time.tm_year + 1900);
    sysTime.stDayOfWeek = static_cast<int>(time.tm_wday +    1);
    sysTime.stDayOfYear = static_cast<int>(time.tm_yday +    1);
}

AREG_API_IMPL NEMath::eCompare NEUtilities::compareTimes( const NEUtilities::sSystemTime & lhs, const NEUtilities::sSystemTime & rhs )
{
    TIME64 lhsTm{ NEUtilities::convToTime(lhs) };
    TIME64 rshTm{ NEUtilities::convToTime(lhs) };
    if (lhsTm > rshTm)
    {
        return NEMath::eCompare::Bigger;
    }
    else if (lhsTm < rshTm)
    {
        return NEMath::eCompare::Smaller;
    }
    else
    {
        return NEMath::eCompare::Equal;
    }
}

AREG_API_IMPL String NEUtilities::createComponentItemName( const String & componentName, const String & itemName )
{
    String result( componentName );
    if ((componentName.isEmpty() == false) && (itemName.isEmpty() == false))
    {
        result += NECommon::COMPONENT_ITEM_SEPARATOR;
        result += itemName;

        if (result.getLength() > NEUtilities::MAX_GENERATED_NAME_BUFFER_SIZE)
        {
            result.substring(0, NEUtilities::MAX_GENERATED_NAME_BUFFER_SIZE);
        }
    }
    else
    {
        result    = String::getEmptyString();
    }

    return result;
}

AREG_API_IMPL String NEUtilities::generateName( const char* prefix )
{
    char buffer[NEUtilities::MAX_GENERATED_NAME_BUFFER_SIZE];
    NEUtilities::generateName(prefix, buffer, NEUtilities::MAX_GENERATED_NAME_BUFFER_SIZE);
    return String(buffer);
}

AREG_API_IMPL const char * NEUtilities::generateName(const char * prefix, char * OUT out_buffer, int length)
{
    return NEUtilities::generateName(prefix, out_buffer, length, NECommon::DEFAULT_SPECIAL_CHAR.data());
}

AREG_API_IMPL const char * NEUtilities::generateName(const char * prefix, char * OUT out_buffer, int length, const char * specChar)
{
    constexpr char const strFormat[]{ "%s%s%08x%s%08x" };

    if (out_buffer != nullptr)
    {
        *out_buffer = '\0';
        const char* spec = specChar != nullptr ? specChar : NECommon::DEFAULT_SPECIAL_CHAR.data();
        NEMath::uLargeInteger time{};
        auto now{ std::chrono::high_resolution_clock::now().time_since_epoch() };
        time.quadPart = std::chrono::duration_cast<std::chrono::microseconds>(now).count();

        String::formatString( out_buffer, length, strFormat
                            , prefix != nullptr ? prefix : NEUtilities::DEFAULT_GENERATED_NAME.data()
                            , spec
                            , time.u.highPart
                            , spec
                            , time.u.lowPart);
    }

    return out_buffer;
}

AREG_API_IMPL unsigned int NEUtilities::generateUniqueId( void )
{
    static std::atomic_uint _id(0u);
    return ++ _id;
}

AREG_API_IMPL uint64_t NEUtilities::getTickCount( void )
{
    return _osGetTickCount();
}

AREG_API_IMPL bool NEUtilities::convToLocalTime( const sSystemTime & IN utcTime, sSystemTime & OUT localTime )
{
    TIME64 quad = NEUtilities::convToTime(utcTime);
    return NEUtilities::convToLocalTime(quad, localTime);
}

AREG_API_IMPL bool NEUtilities::convToLocalTime( const TIME64 & IN utcTime, sSystemTime & OUT localTime )
{
    return _osConvToLocalTime(utcTime, localTime);
}

AREG_API bool NEUtilities::convToLocalTm(const TIME64 & IN utcTime, tm& OUT localTm)
{
    return _osConvToLocalTm(utcTime, localTm);
}

AREG_API_IMPL void NEUtilities::systemTimeNow( NEUtilities::sSystemTime & OUT sysTime, bool localTime )
{
    _osSystemTimeNow( sysTime, localTime );
}

AREG_API_IMPL TIME64 NEUtilities::systemTimeNow( void )
{
    TIME64 result{ 0 };
    struct timespec ts { 0 };
    if (std::timespec_get(&ts, TIME_UTC) != 0)
    {
        std::chrono::seconds secs{ ts.tv_sec };
        std::chrono::nanoseconds ns{ ts.tv_nsec };
        std::chrono::microseconds now{ std::chrono::duration_cast<std::chrono::microseconds>(secs) + std::chrono::duration_cast<std::chrono::microseconds>(ns) };
        result = static_cast<TIME64>(now.count());
    }

    return result;
}

AREG_API_IMPL TIME64 NEUtilities::convToTime( const NEUtilities::sSystemTime & IN sysTime )
{
    const int year{ sysTime.stYear - 1900 };

    return    static_cast<TIME64>(sysTime.stMicrosecs)
            + static_cast<TIME64>(sysTime.stMillisecs       * NEUtilities::MILLISEC_TO_MICROSECS)
            + static_cast<TIME64>(sysTime.stSecond          * NEUtilities::SEC_TO_MICROSECS)
            + static_cast<TIME64>(sysTime.stMinute          * NEUtilities::MIN_TO_MICROSECS)
            + static_cast<TIME64>(sysTime.stHour            * NEUtilities::HOUR_TO_MICROSECS)
            + static_cast<TIME64>((sysTime.stDayOfYear - 1) * NEUtilities::DAY_TO_MICROSECS)
            + static_cast<TIME64>(((year -  70)      )      * NEUtilities::YEAR_TO_MICROSECS)
            + static_cast<TIME64>(((year -  69) /   4)      * NEUtilities::DAY_TO_MICROSECS)
            - static_cast<TIME64>(((year -   1) / 100)      * NEUtilities::DAY_TO_MICROSECS)
            + static_cast<TIME64>(((year + 299) / 400)      * NEUtilities::DAY_TO_MICROSECS);
}

AREG_API_IMPL void NEUtilities::convToSystemTime( const TIME64 & IN timeValue, NEUtilities::sSystemTime & OUT sysTime )
{
    _osConvToSystemTime(timeValue, sysTime);
}

AREG_API_IMPL NEUtilities::DataLiteral NEUtilities::convDataSize( uint64_t dataSize )
{
    NEUtilities::DataLiteral result{ static_cast<double>(dataSize), NECommon::STR_ONE_BYTE};
    if ( dataSize >= NECommon::ONE_MEGABYTE )
    {
        result.first = static_cast<double>(dataSize) / NECommon::ONE_MEGABYTE;
        result.second = NECommon::STR_ONE_MEGABYTE;
    }
    else if ( dataSize >= NECommon::ONE_KILOBYTE )
    {
        result.first = static_cast<double>(dataSize) / NECommon::ONE_KILOBYTE;
        result.second = NECommon::STR_ONE_KILOBYTE;
    }

    return result;
}

AREG_API_IMPL NEUtilities::DataLiteral NEUtilities::convDuration( uint64_t timeDuration )
{
    NEUtilities::DataLiteral result{ static_cast<double>(timeDuration), NECommon::STR_1_NS_SHORT };
    if ( timeDuration >= NECommon::DURATION_1_SEC )
    {
        result.first = static_cast<double>(timeDuration) / NECommon::DURATION_1_SEC;
        result.second = NECommon::STR_1_SEC_SHORT;
    }
    else if ( timeDuration >= NECommon::DURATION_1_MILLI )
    {
        result.first = static_cast<double>(timeDuration) / NECommon::DURATION_1_MILLI;
        result.second = NECommon::STR_1_MILLISEC_SHORT;
    }
    else if ( timeDuration >= NECommon::DURATION_1_MICRO )
    {
        result.first = static_cast<double>(timeDuration) / NECommon::DURATION_1_MICRO;
        result.second = NECommon::STR_1_MICROSEC_SHORT;
    }

    return result;
}
