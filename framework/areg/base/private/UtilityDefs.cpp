/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
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
#include "areg/base/MemoryDefs.hpp"

#include <chrono>
#include <limits>
#include <string>
#include <time.h>

namespace {

/**
 * \brief   Platform-safe wrapper for localtime. Converts seconds since Unix epoch to broken-down local time.
 * \param[in]   secs    Seconds since Unix epoch to convert.
 * \param[out]  result  Broken-down local time structure.
 * \return  Returns true if conversion succeeded; otherwise false.
 **/
inline bool _safe_localtime(time_t secs, struct tm & result) noexcept
{
#ifdef _WIN32
    return ::localtime_s(&result, &secs) == 0;
#else
    return ::localtime_r(&secs, &result) != nullptr;
#endif
}

/**
 * \brief   Returns the seconds since Unix epoch of a broken-down time read as UTC.
 *          The date is read from tm_year, tm_mon and tm_mday.
 **/
constexpr int64_t _tm_seconds(const struct tm & time) noexcept
{
    return    areg::days_since_epoch(time.tm_year + 1900, time.tm_mon + 1, time.tm_mday) * static_cast<int64_t>(areg::DAY_TO_SECS)
            + static_cast<int64_t>(time.tm_hour) * static_cast<int64_t>(areg::HOUR_TO_SECS)
            + static_cast<int64_t>(time.tm_min ) * static_cast<int64_t>(areg::MIN_TO_SECS)
            + static_cast<int64_t>(time.tm_sec );
}

/**
 * \brief   Writes the calendar fields of the seconds since Unix epoch into a tm, the other fields are kept.
 **/
constexpr void _set_tm_fields(int64_t secs, struct tm & time) noexcept
{
    const int64_t daySecs{ static_cast<int64_t>(areg::DAY_TO_SECS) };
    const int64_t days{ (secs >= 0 ? secs : secs - daySecs + 1) / daySecs };
    const int64_t rest{ secs - days * daySecs };
    int32_t year{ 0 };
    int32_t month{ 0 };
    int32_t day{ 0 };
    areg::date_from_days(days, year, month, day);

    time.tm_year = year - 1900;
    time.tm_mon  = month - 1;
    time.tm_mday = day;
    time.tm_hour = static_cast<int32_t>(rest / static_cast<int64_t>(areg::HOUR_TO_SECS));
    time.tm_min  = static_cast<int32_t>((rest % static_cast<int64_t>(areg::HOUR_TO_SECS)) / static_cast<int64_t>(areg::MIN_TO_SECS));
    time.tm_sec  = static_cast<int32_t>(rest % static_cast<int64_t>(areg::MIN_TO_SECS));
    time.tm_wday = static_cast<int32_t>(((days + 4) % 7 + 7) % 7);
    time.tm_yday = static_cast<int32_t>(days - areg::days_since_epoch(year, 1, 1));
}

/**
 * \brief   Returns the seconds of the same date and time in a year that time_t holds, as the given seconds
 *          when time_t holds them. The year found has the same length and starts on the same weekday.
 **/
int64_t _time_t_seconds(int64_t utcSecs) noexcept
{
    if constexpr (sizeof(time_t) < sizeof(int64_t))
    {
        constexpr int32_t LAST_YEAR{ 2037 };
        constexpr int32_t YEAR_CYCLE{ 28 };
        if ((utcSecs >= static_cast<int64_t>(std::numeric_limits<time_t>::min())) && (utcSecs <= static_cast<int64_t>(std::numeric_limits<time_t>::max())))
        {
            return utcSecs;
        }

        const int64_t daySecs{ static_cast<int64_t>(areg::DAY_TO_SECS) };
        int32_t year{ 0 };
        int32_t month{ 0 };
        int32_t day{ 0 };
        areg::date_from_days((utcSecs >= 0 ? utcSecs : utcSecs - daySecs + 1) / daySecs, year, month, day);
        const int64_t start{ areg::days_since_epoch(year, 1, 1) };
        const int64_t length{ areg::days_since_epoch(year + 1, 1, 1) - start };
        for (int32_t candidate = LAST_YEAR - YEAR_CYCLE + 1; candidate <= LAST_YEAR; ++candidate)
        {
            const int64_t other{ areg::days_since_epoch(candidate, 1, 1) };
            if (((areg::days_since_epoch(candidate + 1, 1, 1) - other) == length) && ((((other - start) % 7) + 7) % 7 == 0))
            {
                return utcSecs + (other - start) * daySecs;
            }
        }
    }

    return utcSecs;
}

/**
 * \brief   Converts seconds since Unix epoch to broken-down local time.
 *          The time zone rules of an instant that time_t cannot hold are those of the year _time_t_seconds() finds.
 * \param[in]   utcSecs     UTC time in seconds since Unix epoch.
 * \param[out]  localTm     Receives the broken-down local time.
 * \return  Returns true if conversion succeeded; otherwise false.
 **/
bool _local_tm(int64_t utcSecs, struct tm & localTm) noexcept
{
    const int64_t probe{ _time_t_seconds(utcSecs) };
    struct tm result{};
    if (_safe_localtime(static_cast<time_t>(probe), result) == false)
    {
        return false;
    }

    if (probe != utcSecs)
    {
        _set_tm_fields(_tm_seconds(result) + (utcSecs - probe), result);
    }

    localTm = result;
    return true;
}

} // namespace

namespace areg::os {

    /**
     * \brief   Returns current date and time in UTC as microseconds since Unix epoch (1 January 1970).
     **/
    extern TIME64 _os_system_time_now();

    /**
     * \brief   Returns milliseconds elapsed since an unspecified fixed point (monotonic clock).
     **/
    uint64_t _os_tick_count()
    {
        return static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now().time_since_epoch()
            ).count()
        );
    }

} // namespace areg::os

/************************************************************************/
// areg namespace functions
/************************************************************************/

AREG_API_IMPL void areg::make_tm_local( struct tm & utcTime ) noexcept
{
    struct tm local{};
    if (_local_tm(_tm_seconds(utcTime), local))
    {
        utcTime = local;
    }
}

AREG_API_IMPL void areg::to_tm(const TIME64& timeMicro, tm& time)
{
    time = tm{};
    _set_tm_fields(areg::to_seconds(timeMicro), time);
}

AREG_API_IMPL areg::String areg::create_component_item_name( const areg::String & componentName, const areg::String & itemName )
{
    String result( componentName );
    if ((componentName.is_empty() == false) && (itemName.is_empty() == false))
    {
        result += areg::COMPONENT_ITEM_SEPARATOR;
        result += itemName;

        if (result.length() > areg::MAX_GENERATED_NAME_BUFFER_SIZE)
        {
            result.substring(0, areg::MAX_GENERATED_NAME_BUFFER_SIZE);
        }
    }
    else
    {
        result    = String::empty_string();
    }

    return result;
}

AREG_API_IMPL areg::String areg::generate_name( const char* prefix )
{
    char buffer[areg::MAX_GENERATED_NAME_BUFFER_SIZE];
    areg::generate_name(prefix, buffer, areg::MAX_GENERATED_NAME_BUFFER_SIZE);
    return String(buffer);
}

AREG_API_IMPL const char * areg::generate_name(const char * prefix, char * out_buffer, int32_t length) noexcept
{
    return areg::generate_name(prefix, out_buffer, length, areg::DEFAULT_SPECIAL_CHAR.data());
}

AREG_API_IMPL const char * areg::generate_name(const char * prefix, char * out_buffer, int32_t length, const char * specChar) noexcept
{
    constexpr char const strFormat[]{ "%s%s%08x%s%08x" };

    if (out_buffer != nullptr)
    {
        *out_buffer = '\0';
        const char* spec = specChar != nullptr ? specChar : areg::DEFAULT_SPECIAL_CHAR.data();
        auto now{ std::chrono::high_resolution_clock::now().time_since_epoch() };
        const uint64_t time{ static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::microseconds>(now).count()) };

        String::format_string( out_buffer, length, strFormat
                            , prefix != nullptr ? prefix : areg::DEFAULT_GENERATED_NAME.data()
                            , spec
                            , static_cast<uint32_t>(time >> 32u)
                            , spec
                            , static_cast<uint32_t>(time));
    }

    return out_buffer;
}

AREG_API_IMPL uint32_t areg::generate_unique_id() noexcept
{
    static std::atomic_uint _id(0u);
    return ++ _id;
}

AREG_API_IMPL uint64_t areg::tick_count() noexcept
{
    return areg::os::_os_tick_count();
}

AREG_API_IMPL bool areg::to_local_time( const areg::CalendarTime & utcTime, areg::CalendarTime & localTime ) noexcept
{
    TIME64 quad = areg::to_time(utcTime);
    return areg::to_local_time(quad, localTime);
}

AREG_API_IMPL bool areg::to_local_time( const TIME64 & utcTime, areg::CalendarTime & localTime ) noexcept
{
    struct tm local{};
    if (_local_tm(areg::to_seconds(utcTime), local) == false)
    {
        return false;
    }

    const TIME64 rest{ utcTime % areg::SEC_TO_MICROSECS };
    areg::to_system_time(local, localTime);
    localTime.stMillisecs = static_cast<int32_t>(rest / areg::MILLISEC_TO_MICROSECS);
    localTime.stMicrosecs = static_cast<int32_t>(rest % areg::MILLISEC_TO_MICROSECS);
    return true;
}

AREG_API_IMPL bool areg::to_local_tm(const TIME64 & utcTime, tm& localTm) noexcept
{
    return _local_tm(areg::to_seconds(utcTime), localTm);
}

AREG_API_IMPL void areg::system_time_now(areg::CalendarTime & sysTime, bool localTime ) noexcept
{
    const TIME64 now{ areg::os::_os_system_time_now() };
    if (localTime)
    {
        areg::to_local_time(now, sysTime);
    }
    else
    {
        areg::to_system_time(now, sysTime);
    }
}

AREG_API_IMPL TIME64 areg::system_time_now() noexcept
{
    return areg::os::_os_system_time_now();
}

AREG_API_IMPL areg::DataLiteral areg::conv_data_size( uint64_t dataSize )
{
    areg::DataLiteral result{ static_cast<double>(dataSize), areg::STR_ONE_BYTE};
    if ( dataSize >= areg::ONE_MEGABYTE )
    {
        result.first = static_cast<double>(dataSize) / areg::ONE_MEGABYTE;
        result.second = areg::STR_ONE_MEGABYTE;
    }
    else if ( dataSize >= areg::ONE_KILOBYTE )
    {
        result.first = static_cast<double>(dataSize) / areg::ONE_KILOBYTE;
        result.second = areg::STR_ONE_KILOBYTE;
    }

    return result;
}

AREG_API_IMPL areg::DataLiteral areg::conv_duration( uint64_t timeDuration )
{
    areg::DataLiteral result{ static_cast<double>(timeDuration), areg::STR_1_NS_SHORT };
    if ( timeDuration >= areg::DURATION_1_SEC )
    {
        result.first = static_cast<double>(timeDuration) / areg::DURATION_1_SEC;
        result.second = areg::STR_1_SEC_SHORT;
    }
    else if ( timeDuration >= areg::DURATION_1_MILLI )
    {
        result.first = static_cast<double>(timeDuration) / areg::DURATION_1_MILLI;
        result.second = areg::STR_1_MILLISEC_SHORT;
    }
    else if ( timeDuration >= areg::DURATION_1_MICRO )
    {
        result.first = static_cast<double>(timeDuration) / areg::DURATION_1_MICRO;
        result.second = areg::STR_1_MICROSEC_SHORT;
    }

    return result;
}
