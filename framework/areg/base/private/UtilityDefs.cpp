/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
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
#include <string>
#include <time.h>

namespace {

/**
 * \brief   Platform-safe wrapper for gmtime. Converts seconds since Unix epoch to broken-down UTC time.
 * \param[in]   secs    Seconds since Unix epoch to convert.
 * \param[out]  result  Broken-down UTC time structure.
 * \return  Returns true if conversion succeeded; otherwise false.
 **/
inline bool _safe_gmtime(time_t secs, struct tm & result) noexcept
{
#ifdef _WIN32
    return ::gmtime_s(&result, &secs) == 0;
#else
    return ::gmtime_r(&secs, &result) != nullptr;
#endif
}

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

} // namespace

namespace areg::os {

    /**
     * \brief   Returns current date and time in UTC as microseconds since Unix epoch (1 January 1970).
     **/
    extern TIME64 _os_system_time_now();

    /**
     * \brief   Fills the calendar time structure with the current system date and time.
     * \param[out]  sysTime     The structure to receive broken-down current date and time.
     * \param[in]   localTime   If true, fills sysTime with local time; if false, fills with UTC time.
     **/
    extern void _os_system_time_now(CalendarTime & sysTime, bool localTime);

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

    /**
     * \brief   Converts a broken-down UTC time structure to local time in-place.
     * \param[in,out]   utcTime     On input contains broken-down UTC time; on output contains local time.
     **/
    void _os_make_tm_local(struct tm & utcTime)
    {
        mem_set(&utcTime, sizeof(struct tm), 0);
        time_t timer = mktime(&utcTime);
        _safe_localtime(timer, utcTime);
    }

    /**
     * \brief   Converts UTC time in microseconds since Unix epoch to broken-down local calendar time.
     * \param[in]   utcTime     UTC time in microseconds since Unix epoch (1 January 1970).
     * \param[out]  localTime   Receives the broken-down local calendar time including milliseconds and microseconds.
     * \return  Returns true if conversion succeeded; otherwise false.
     **/
    bool _os_to_local_time(const TIME64 & utcTime, CalendarTime & localTime)
    {
        time_t secs;
        uint16_t milli, micro;
        conv_microsecs(utcTime, secs, milli, micro);

        struct tm tmLocal {};
        if (_safe_localtime(secs, tmLocal))
        {
            to_system_time(tmLocal, localTime);
            localTime.stMillisecs = milli;
            localTime.stMicrosecs = micro;
            return true;
        }

        return false;
    }

    /**
     * \brief   Converts UTC time in microseconds since Unix epoch to broken-down local tm structure.
     * \param[in]   utcTime     UTC time in microseconds since Unix epoch (1 January 1970).
     * \param[out]  localTm     Receives the broken-down local time without milliseconds and microseconds.
     * \return  Returns true if conversion succeeded; otherwise false.
     **/
    bool _os_to_local_tm(const TIME64 & utcTime, struct tm & localTm)
    {
        const time_t secs = static_cast<time_t>(utcTime / SEC_TO_MICROSECS);
        return _safe_localtime(secs, localTm);
    }

    /**
     * \brief   Converts UTC time in microseconds since Unix epoch to broken-down calendar time structure.
     * \param[in]   timeValue   UTC time in microseconds since Unix epoch (1 January 1970).
     * \param[out]  sysTime     Receives the broken-down calendar time including milliseconds and microseconds.
     **/
    void _os_to_system_time(const TIME64 & timeValue, CalendarTime & sysTime)
    {
        time_t secs;
        uint16_t milli{ 0u };
        uint16_t micro{ 0u };
        conv_microsecs(timeValue, secs, milli, micro);

        struct tm gmt {};
        if (_safe_gmtime(secs, gmt))
        {
            to_system_time(gmt, sysTime);
            sysTime.stMillisecs = milli;
            sysTime.stMicrosecs = micro;
        }
    }

    /**
     * \brief   Converts UTC time in microseconds since Unix epoch to broken-down tm structure.
     * \param[in]   timeValue   UTC time in microseconds since Unix epoch (1 January 1970).
     * \param[out]  time        Receives the broken-down UTC time without milliseconds and microseconds.
     **/
    void _os_to_tm(const TIME64 & timeValue, struct tm & time)
    {
        const time_t secs = static_cast<time_t>(timeValue / SEC_TO_MICROSECS);
        _safe_gmtime(secs, time);
    }

} // namespace areg::os

/************************************************************************/
// areg namespace functions
/************************************************************************/

AREG_API_IMPL void areg::make_tm_local( struct tm & utcTime ) noexcept
{
    areg::os::_os_make_tm_local(utcTime);
}

AREG_API_IMPL void areg::to_tm(const TIME64& timeMicro, tm& time)
{
    areg::os::_os_to_tm(timeMicro, time);
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
        areg::Int64Parts time{};
        auto now{ std::chrono::high_resolution_clock::now().time_since_epoch() };
        time.quadPart = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::microseconds>(now).count());

        String::format_string( out_buffer, length, strFormat
                            , prefix != nullptr ? prefix : areg::DEFAULT_GENERATED_NAME.data()
                            , spec
                            , time.u.highPart
                            , spec
                            , time.u.lowPart);
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
    return areg::os::_os_to_local_time(utcTime, localTime);
}

AREG_API_IMPL bool areg::to_local_tm(const TIME64 & utcTime, tm& localTm) noexcept
{
    return areg::os::_os_to_local_tm(utcTime, localTm);
}

AREG_API_IMPL void areg::system_time_now(areg::CalendarTime & sysTime, bool localTime ) noexcept
{
    areg::os::_os_system_time_now( sysTime, localTime );
}

AREG_API_IMPL TIME64 areg::system_time_now() noexcept
{
    return areg::os::_os_system_time_now();
}

AREG_API_IMPL void areg::to_system_time( const TIME64 & timeValue, areg::CalendarTime & sysTime ) noexcept
{
    areg::os::_os_to_system_time(timeValue, sysTime);
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
