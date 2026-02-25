/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/UtilityDefs.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform Switches
 *
 ************************************************************************/

#include "areg/base/UtilityDefs.hpp"
#include "areg/base/SyncPrimitives.hpp"
#include "areg/base/String.hpp"
#include "areg/base/WideString.hpp"
#include "areg/base/CommonDefs.hpp"

#include <chrono>
#include <string>
#include <time.h>


namespace NEUtilities 
{
/************************************************************************/
// Declaration of OS specific methods
/************************************************************************/

    /**
     * \brief   Returns value as milliseconds that have elapsed since the system was started.
     **/
    extern uint64_t _osGetTickCount();

    /**
     * \brief   Returns value of the current date and time in microseconds passed since Unix epoch (1 January 1970).
     *          The time is in UTC.
     **/
    extern TIME64 _osSystemTimeNow();

    /**
     * \brief   Set the current date and time in the struct pointed to by the `sysTime` argument.
     * \param[out]  sysTime     The structure to break the current date and time.
     * \param[in]   localTime   The flag, indicating whether the time should be local or in UTC.
     **/
    extern void _osSystemTimeNow( CalendarTime & sysTime, bool localTime );

    /**
     * \brief   Converts the UTC time broken in the structure of `tm` to the local time.
     * \param[in,out]   utcTime     The broken time in UTC to convert to the local time.
     **/
    extern void _osMakeTmLocal(struct tm& utcTime);

    /**
     * \brief   Converts the given time in microseconds passed since Unix epoch (1 January 1970) to the local time
     *          broken in the structure of `localTime` parameter.
     * \param[in]   utcTime     The UTC time in microseconds since Unix epoch (1 January 1970) to convert to local time.
     * \param[out]  localTime   The broken time structure. On output this contains structured calendar structure.
     * \return  Returns true if operation succeeded. Otherwise, returns false.
     **/
    extern bool _osConvToLocalTime(const TIME64& utcTime, CalendarTime& localTime);

    /**
     * \brief   Converts the given time in microseconds passed since Unix epoch (1 January 1970) to the local time
     *          broken in the structure of `localTm` parameter.
     * \param[in]   utcTime     The UTC time in microseconds since Unix epoch (1 January 1970) to convert to local time.
     * \param[out]  localTm     The broken time structure. On output this contains structured calendar structure without
     *                          milliseconds and microseconds.
     * \return  Returns true if operation succeeded. Otherwise, returns false.
     **/
    extern bool _osConvToLocalTm(const TIME64& utcTime, struct tm& localTm);

    /**
     * \brief   Converts the given time in microseconds passed since Unix epoch (1 January 1970) to the time
     *          broken in the structure of `sysTime` parameter.
     * \param[in]   timeValue   The UTC time in microseconds since Unix epoch (1 January 1970) to break.
     * \param[out]  sysTime     The broken time structure. On output this contains structured calendar structure,
     *                          including milliseconds and microseconds.
     **/
    extern void _osConvToSystemTime(const TIME64& timeValue, NEUtilities::CalendarTime& sysTime);

    /**
     * \brief   Converts the given time in microseconds passed since Unix epoch (1 January 1970) to the time
     *          broken in the tm structure.
     * \param[in]   timeValue   The UTC time in microseconds since Unix epoch (1 January 1970) to break.
     * \param[out]  time        The broken time structure. On output this contains structured calendar structure
     *                          without milliseconds and microseconds.
     **/
    extern void _osConvToTm(const TIME64& timeValue, struct tm& time);

/************************************************************************/
// Local static methods.
/************************************************************************/

    static NEMath::Ordering _compareLargeIntegers( const NEMath::Int64Parts & lhs, const NEMath::Int64Parts & rhs )
    {
        NEMath::Ordering result = NEMath::Ordering::Equal;
        if ( lhs.u.highPart < rhs.u.highPart )
            result = NEMath::Ordering::Smaller;
        else if ( lhs.u.highPart > rhs.u.highPart )
            result = NEMath::Ordering::Bigger;
        else if ( lhs.u.lowPart < rhs.u.lowPart )
            result = NEMath::Ordering::Smaller;
        else if ( lhs.u.lowPart > rhs.u.lowPart )
            result = NEMath::Ordering::Bigger;

        return result;
    }
}

/************************************************************************/
// NEUtilities namespace global functions
/************************************************************************/

AREG_API_IMPL time_t NEUtilities::to_seconds(const CalendarTime & sysTime)
{
    const int32_t year{ sysTime.stYear - 1900 };
    return    static_cast<time_t>(sysTime.stSecond)
            + static_cast<time_t>(static_cast<TIME64>(sysTime.stMinute       ) * NEUtilities::MIN_TO_SECS)
            + static_cast<time_t>(static_cast<TIME64>(sysTime.stHour         ) * NEUtilities::HOUR_TO_SECS)
            + static_cast<time_t>(static_cast<TIME64>(sysTime.stDayOfYear - 1) * NEUtilities::DAY_TO_SECS)
            + static_cast<time_t>(static_cast<TIME64>( year -  70            ) * NEUtilities::YEAR_TO_SECS)
            + static_cast<time_t>(static_cast<TIME64>((year -  69) /   4     ) * NEUtilities::DAY_TO_SECS)
            - static_cast<time_t>(static_cast<TIME64>((year -   1) / 100     ) * NEUtilities::DAY_TO_SECS)
            + static_cast<time_t>(static_cast<TIME64>((year + 299) / 400     ) * NEUtilities::DAY_TO_SECS);
}

AREG_API_IMPL NEMath::Ordering NEUtilities::compare_times( const TIME64 & lhs, const TIME64 & rhs )
{
    NEMath::Int64Parts lhsLi, rshLi;
    lhsLi.quadPart  = lhs;
    rshLi.quadPart  = rhs;

    return NEUtilities::_compareLargeIntegers(lhsLi, rshLi);
}

AREG_API_IMPL void NEUtilities::conv_microsecs(const TIME64& time, time_t& secs, uint16_t& milli, uint16_t& micro)
{
    secs = static_cast<time_t>(time / NEUtilities::SEC_TO_MICROSECS);
    TIME64 rest = time % NEUtilities::SEC_TO_MICROSECS;
    milli = static_cast<uint16_t>(rest / NEUtilities::MILLISEC_TO_MICROSECS);
    micro = static_cast<uint16_t>(rest % NEUtilities::MILLISEC_TO_MICROSECS);
}

AREG_API_IMPL void NEUtilities::to_tm(const CalendarTime & sysTime, struct tm & time)
{
    if (sysTime.stYear >= 1900)
    {
        time.tm_sec     = static_cast<int32_t>( sysTime.stSecond);
        time.tm_min     = static_cast<int32_t>( sysTime.stMinute);
        time.tm_hour    = static_cast<int32_t>( sysTime.stHour);
        time.tm_mday    = static_cast<int32_t>( sysTime.stDay);
        time.tm_mon     = static_cast<int32_t>( sysTime.stMonth     -    1);    // tm_mon is 0 based
        time.tm_year    = static_cast<int32_t>( sysTime.stYear      - 1900);    // tm_year is 1900 based
        time.tm_wday    = static_cast<int32_t>( sysTime.stDayOfWeek -    1);    // tm_wday is 0 based
        time.tm_yday    = static_cast<int32_t>( sysTime.stDayOfYear -    1);    // day of year 0 based
        time.tm_isdst   = -1;
    }
    else
    {
        ASSERT( false );
        NEMemory::zero_element<tm>(time);
    }
}

AREG_API_IMPL void NEUtilities::make_tm_local( struct tm & utcTime )
{
    _osMakeTmLocal(utcTime);
}

AREG_API_IMPL void NEUtilities::to_tm(const TIME64& timeMicro, tm& time)
{
    _osConvToTm(timeMicro, time);
}

AREG_API_IMPL void NEUtilities::to_system_time(const struct tm & time, CalendarTime & sysTime)
{
    sysTime.stSecond    = static_cast<int32_t>(time.tm_sec);
    sysTime.stMinute    = static_cast<int32_t>(time.tm_min);
    sysTime.stHour      = static_cast<int32_t>(time.tm_hour);
    sysTime.stDay       = static_cast<int32_t>(time.tm_mday);
    sysTime.stMonth     = static_cast<int32_t>(time.tm_mon  +    1);
    sysTime.stYear      = static_cast<int32_t>(time.tm_year + 1900);
    sysTime.stDayOfWeek = static_cast<int32_t>(time.tm_wday +    1);
    sysTime.stDayOfYear = static_cast<int32_t>(time.tm_yday +    1);
}

AREG_API_IMPL NEMath::Ordering NEUtilities::compare_times( const NEUtilities::CalendarTime & lhs, const NEUtilities::CalendarTime & rhs )
{
    TIME64 lhsTm{ NEUtilities::to_time(lhs) };
    TIME64 rshTm{ NEUtilities::to_time(rhs) };
    if (lhsTm > rshTm)
    {
        return NEMath::Ordering::Bigger;
    }
    else if (lhsTm < rshTm)
    {
        return NEMath::Ordering::Smaller;
    }
    else
    {
        return NEMath::Ordering::Equal;
    }
}

AREG_API_IMPL String NEUtilities::create_component_item_name( const String & componentName, const String & itemName )
{
    String result( componentName );
    if ((componentName.is_empty() == false) && (itemName.is_empty() == false))
    {
        result += NECommon::COMPONENT_ITEM_SEPARATOR;
        result += itemName;

        if (result.length() > NEUtilities::MAX_GENERATED_NAME_BUFFER_SIZE)
        {
            result.substring(0, NEUtilities::MAX_GENERATED_NAME_BUFFER_SIZE);
        }
    }
    else
    {
        result    = String::empty_string();
    }

    return result;
}

AREG_API_IMPL String NEUtilities::generate_name( const char* prefix )
{
    char buffer[NEUtilities::MAX_GENERATED_NAME_BUFFER_SIZE];
    NEUtilities::generate_name(prefix, buffer, NEUtilities::MAX_GENERATED_NAME_BUFFER_SIZE);
    return String(buffer);
}

AREG_API_IMPL const char * NEUtilities::generate_name(const char * prefix, char * out_buffer, int32_t length)
{
    return NEUtilities::generate_name(prefix, out_buffer, length, NECommon::DEFAULT_SPECIAL_CHAR.data());
}

AREG_API_IMPL const char * NEUtilities::generate_name(const char * prefix, char * out_buffer, int32_t length, const char * specChar)
{
    constexpr char const strFormat[]{ "%s%s%08x%s%08x" };

    if (out_buffer != nullptr)
    {
        *out_buffer = '\0';
        const char* spec = specChar != nullptr ? specChar : NECommon::DEFAULT_SPECIAL_CHAR.data();
        NEMath::Int64Parts time{};
        auto now{ std::chrono::high_resolution_clock::now().time_since_epoch() };
        time.quadPart = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::microseconds>(now).count());

        String::format_string( out_buffer, length, strFormat
                            , prefix != nullptr ? prefix : NEUtilities::DEFAULT_GENERATED_NAME.data()
                            , spec
                            , time.u.highPart
                            , spec
                            , time.u.lowPart);
    }

    return out_buffer;
}

AREG_API_IMPL uint32_t NEUtilities::generate_unique_id()
{
    static std::atomic_uint _id(0u);
    return ++ _id;
}

AREG_API_IMPL uint64_t NEUtilities::tick_count()
{
    return _osGetTickCount();
}

AREG_API_IMPL bool NEUtilities::to_local_time( const CalendarTime & utcTime, CalendarTime & localTime )
{
    TIME64 quad = NEUtilities::to_time(utcTime);
    return NEUtilities::to_local_time(quad, localTime);
}

AREG_API_IMPL bool NEUtilities::to_local_time( const TIME64 & utcTime, CalendarTime & localTime )
{
    return _osConvToLocalTime(utcTime, localTime);
}

AREG_API_IMPL bool NEUtilities::to_local_tm(const TIME64 & utcTime, tm& localTm)
{
    return _osConvToLocalTm(utcTime, localTm);
}

AREG_API_IMPL void NEUtilities::system_time_now( NEUtilities::CalendarTime & sysTime, bool localTime )
{
    _osSystemTimeNow( sysTime, localTime );
}

AREG_API_IMPL TIME64 NEUtilities::system_time_now()
{
    return _osSystemTimeNow();
}

AREG_API_IMPL TIME64 NEUtilities::to_time( const NEUtilities::CalendarTime & sysTime )
{
    const int32_t year{ sysTime.stYear - 1900 };

    return    static_cast<TIME64>(sysTime.stMicrosecs)
            + static_cast<TIME64>(sysTime.stMillisecs    ) * NEUtilities::MILLISEC_TO_MICROSECS
            + static_cast<TIME64>(sysTime.stSecond       ) * NEUtilities::SEC_TO_MICROSECS
            + static_cast<TIME64>(sysTime.stMinute       ) * NEUtilities::MIN_TO_MICROSECS
            + static_cast<TIME64>(sysTime.stHour         ) * NEUtilities::HOUR_TO_MICROSECS
            + static_cast<TIME64>(sysTime.stDayOfYear - 1) * NEUtilities::DAY_TO_MICROSECS
            + static_cast<TIME64>( year -  70            ) * NEUtilities::YEAR_TO_MICROSECS
            + static_cast<TIME64>((year -  69) /   4     ) * NEUtilities::DAY_TO_MICROSECS
            - static_cast<TIME64>((year -   1) / 100     ) * NEUtilities::DAY_TO_MICROSECS
            + static_cast<TIME64>((year + 299) / 400     ) * NEUtilities::DAY_TO_MICROSECS;
}

AREG_API_IMPL TIME64 NEUtilities::to_time(const tm& time)
{
    return    static_cast<TIME64>(time.tm_sec               ) * NEUtilities::SEC_TO_MICROSECS
            + static_cast<TIME64>(time.tm_min               ) * NEUtilities::MIN_TO_MICROSECS
            + static_cast<TIME64>(time.tm_hour              ) * NEUtilities::HOUR_TO_MICROSECS
            + static_cast<TIME64>(time.tm_yday              ) * NEUtilities::DAY_TO_MICROSECS
            + static_cast<TIME64>(time.tm_year - 70         ) * NEUtilities::YEAR_TO_MICROSECS
            + static_cast<TIME64>((time.tm_year - 69)  / 4  ) * NEUtilities::DAY_TO_MICROSECS
            - static_cast<TIME64>((time.tm_year - 1)   / 100) * NEUtilities::DAY_TO_MICROSECS
            + static_cast<TIME64>((time.tm_year + 299) / 400) * NEUtilities::DAY_TO_MICROSECS;
}

AREG_API_IMPL void NEUtilities::to_system_time( const TIME64 & timeValue, NEUtilities::CalendarTime & sysTime )
{
    _osConvToSystemTime(timeValue, sysTime);
}

AREG_API_IMPL NEUtilities::DataLiteral NEUtilities::conv_data_size( uint64_t dataSize )
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

AREG_API_IMPL NEUtilities::DataLiteral NEUtilities::conv_duration( uint64_t timeDuration )
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
