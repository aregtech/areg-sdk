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

namespace areg::os {
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
    extern void _osSystemTimeNow(CalendarTime& sysTime, bool localTime);

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
    extern void _osConvToSystemTime(const TIME64& timeValue, areg::CalendarTime& sysTime);

    /**
     * \brief   Converts the given time in microseconds passed since Unix epoch (1 January 1970) to the time
     *          broken in the tm structure.
     * \param[in]   timeValue   The UTC time in microseconds since Unix epoch (1 January 1970) to break.
     * \param[out]  time        The broken time structure. On output this contains structured calendar structure
     *                          without milliseconds and microseconds.
     **/
    extern void _osConvToTm(const TIME64& timeValue, struct tm& time);

} // namespace areg::os

/************************************************************************/
// areg namespace functions
/************************************************************************/

AREG_API_IMPL void areg::make_tm_local( struct tm & utcTime ) noexcept
{
    areg::os::_osMakeTmLocal(utcTime);
}

AREG_API_IMPL void areg::to_tm(const TIME64& timeMicro, tm& time)
{
    areg::os::_osConvToTm(timeMicro, time);
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
    return areg::os::_osGetTickCount();
}

AREG_API_IMPL bool areg::to_local_time( const areg::CalendarTime & utcTime, areg::CalendarTime & localTime ) noexcept
{
    TIME64 quad = areg::to_time(utcTime);
    return areg::to_local_time(quad, localTime);
}

AREG_API_IMPL bool areg::to_local_time( const TIME64 & utcTime, areg::CalendarTime & localTime ) noexcept
{
    return areg::os::_osConvToLocalTime(utcTime, localTime);
}

AREG_API_IMPL bool areg::to_local_tm(const TIME64 & utcTime, tm& localTm) noexcept
{
    return areg::os::_osConvToLocalTm(utcTime, localTm);
}

AREG_API_IMPL void areg::system_time_now(areg::CalendarTime & sysTime, bool localTime ) noexcept
{
    areg::os::_osSystemTimeNow( sysTime, localTime );
}

AREG_API_IMPL TIME64 areg::system_time_now() noexcept
{
    return areg::os::_osSystemTimeNow();
}

AREG_API_IMPL void areg::to_system_time( const TIME64 & timeValue, areg::CalendarTime & sysTime ) noexcept
{
    areg::os::_osConvToSystemTime(timeValue, sysTime);
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
