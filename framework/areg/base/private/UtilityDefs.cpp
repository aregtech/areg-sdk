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


namespace areg
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
    extern void _osSystemTimeNow( sSystemTime & sysTime, bool localTime );

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
    extern bool _osConvToLocalTime(const TIME64& utcTime, sSystemTime& localTime);

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
    extern void _osConvToSystemTime(const TIME64& timeValue, areg::sSystemTime& sysTime);

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

    static areg::eCompare _compareLargeIntegers( const areg::uLargeInteger & lhs, const areg::uLargeInteger & rhs )
    {
        areg::eCompare result = areg::eCompare::Equal;
        if ( lhs.u.highPart < rhs.u.highPart )
            result = areg::eCompare::Smaller;
        else if ( lhs.u.highPart > rhs.u.highPart )
            result = areg::eCompare::Bigger;
        else if ( lhs.u.lowPart < rhs.u.lowPart )
            result = areg::eCompare::Smaller;
        else if ( lhs.u.lowPart > rhs.u.lowPart )
            result = areg::eCompare::Bigger;

        return result;
    }
}

/************************************************************************/
// NEUtilities namespace global functions
/************************************************************************/

AREG_API_IMPL time_t areg::convToSeconds(const sSystemTime & sysTime)
{
    const int year{ sysTime.stYear - 1900 };
    return    static_cast<time_t>(sysTime.stSecond)
            + static_cast<time_t>(static_cast<TIME64>(sysTime.stMinute       ) * areg::MIN_TO_SECS)
            + static_cast<time_t>(static_cast<TIME64>(sysTime.stHour         ) * areg::HOUR_TO_SECS)
            + static_cast<time_t>(static_cast<TIME64>(sysTime.stDayOfYear - 1) * areg::DAY_TO_SECS)
            + static_cast<time_t>(static_cast<TIME64>( year -  70            ) * areg::YEAR_TO_SECS)
            + static_cast<time_t>(static_cast<TIME64>((year -  69) /   4     ) * areg::DAY_TO_SECS)
            - static_cast<time_t>(static_cast<TIME64>((year -   1) / 100     ) * areg::DAY_TO_SECS)
            + static_cast<time_t>(static_cast<TIME64>((year + 299) / 400     ) * areg::DAY_TO_SECS);
}

AREG_API_IMPL areg::eCompare areg::compareTimes( const TIME64 & lhs, const TIME64 & rhs )
{
    areg::uLargeInteger lhsLi, rshLi;
    lhsLi.quadPart  = lhs;
    rshLi.quadPart  = rhs;

    return areg::_compareLargeIntegers(lhsLi, rshLi);
}

AREG_API_IMPL void areg::convMicrosecs(const TIME64& time, time_t& secs, unsigned short& milli, unsigned short& micro)
{
    secs = static_cast<time_t>(time / areg::SEC_TO_MICROSECS);
    TIME64 rest = time % areg::SEC_TO_MICROSECS;
    milli = static_cast<unsigned short>(rest / areg::MILLISEC_TO_MICROSECS);
    micro = static_cast<unsigned short>(rest % areg::MILLISEC_TO_MICROSECS);
}

AREG_API_IMPL void areg::convToTm(const sSystemTime & sysTime, struct tm & time)
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
        areg::zeroElement<tm>(time);
    }
}

AREG_API_IMPL void areg::makeTmLocal( struct tm & utcTime )
{
    _osMakeTmLocal(utcTime);
}

AREG_API_IMPL void areg::convToTm(const TIME64& timeMicro, tm& time)
{
    _osConvToTm(timeMicro, time);
}

AREG_API_IMPL void areg::convToSystemTime(const struct tm & time, sSystemTime & sysTime)
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

AREG_API_IMPL areg::eCompare areg::compareTimes( const areg::sSystemTime & lhs, const areg::sSystemTime & rhs )
{
    TIME64 lhsTm{ areg::convToTime(lhs) };
    TIME64 rshTm{ areg::convToTime(rhs) };
    if (lhsTm > rshTm)
    {
        return areg::eCompare::Bigger;
    }
    else if (lhsTm < rshTm)
    {
        return areg::eCompare::Smaller;
    }
    else
    {
        return areg::eCompare::Equal;
    }
}

AREG_API_IMPL String areg::createComponentItemName( const String & componentName, const String & itemName )
{
    String result( componentName );
    if ((componentName.isEmpty() == false) && (itemName.isEmpty() == false))
    {
        result += areg::COMPONENT_ITEM_SEPARATOR;
        result += itemName;

        if (result.getLength() > areg::MAX_GENERATED_NAME_BUFFER_SIZE)
        {
            result.substring(0, areg::MAX_GENERATED_NAME_BUFFER_SIZE);
        }
    }
    else
    {
        result    = String::getEmptyString();
    }

    return result;
}

AREG_API_IMPL String areg::generateName( const char* prefix )
{
    char buffer[areg::MAX_GENERATED_NAME_BUFFER_SIZE];
    areg::generateName(prefix, buffer, areg::MAX_GENERATED_NAME_BUFFER_SIZE);
    return String(buffer);
}

AREG_API_IMPL const char * areg::generateName(const char * prefix, char * out_buffer, int length)
{
    return areg::generateName(prefix, out_buffer, length, areg::DEFAULT_SPECIAL_CHAR.data());
}

AREG_API_IMPL const char * areg::generateName(const char * prefix, char * out_buffer, int length, const char * specChar)
{
    constexpr char const strFormat[]{ "%s%s%08x%s%08x" };

    if (out_buffer != nullptr)
    {
        *out_buffer = '\0';
        const char* spec = specChar != nullptr ? specChar : areg::DEFAULT_SPECIAL_CHAR.data();
        areg::uLargeInteger time{};
        auto now{ std::chrono::high_resolution_clock::now().time_since_epoch() };
        time.quadPart = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::microseconds>(now).count());

        String::formatString( out_buffer, length, strFormat
                            , prefix != nullptr ? prefix : areg::DEFAULT_GENERATED_NAME.data()
                            , spec
                            , time.u.highPart
                            , spec
                            , time.u.lowPart);
    }

    return out_buffer;
}

AREG_API_IMPL unsigned int areg::generateUniqueId()
{
    static std::atomic_uint _id(0u);
    return ++ _id;
}

AREG_API_IMPL uint64_t areg::getTickCount()
{
    return _osGetTickCount();
}

AREG_API_IMPL bool areg::convToLocalTime( const sSystemTime & utcTime, sSystemTime & localTime )
{
    TIME64 quad = areg::convToTime(utcTime);
    return areg::convToLocalTime(quad, localTime);
}

AREG_API_IMPL bool areg::convToLocalTime( const TIME64 & utcTime, sSystemTime & localTime )
{
    return _osConvToLocalTime(utcTime, localTime);
}

AREG_API_IMPL bool areg::convToLocalTm(const TIME64 & utcTime, tm& localTm)
{
    return _osConvToLocalTm(utcTime, localTm);
}

AREG_API_IMPL void areg::systemTimeNow( areg::sSystemTime & sysTime, bool localTime )
{
    _osSystemTimeNow( sysTime, localTime );
}

AREG_API_IMPL TIME64 areg::systemTimeNow()
{
    return _osSystemTimeNow();
}

AREG_API_IMPL TIME64 areg::convToTime( const areg::sSystemTime & sysTime )
{
    const int year{ sysTime.stYear - 1900 };

    return    static_cast<TIME64>(sysTime.stMicrosecs)
            + static_cast<TIME64>(sysTime.stMillisecs    ) * areg::MILLISEC_TO_MICROSECS
            + static_cast<TIME64>(sysTime.stSecond       ) * areg::SEC_TO_MICROSECS
            + static_cast<TIME64>(sysTime.stMinute       ) * areg::MIN_TO_MICROSECS
            + static_cast<TIME64>(sysTime.stHour         ) * areg::HOUR_TO_MICROSECS
            + static_cast<TIME64>(sysTime.stDayOfYear - 1) * areg::DAY_TO_MICROSECS
            + static_cast<TIME64>( year -  70            ) * areg::YEAR_TO_MICROSECS
            + static_cast<TIME64>((year -  69) /   4     ) * areg::DAY_TO_MICROSECS
            - static_cast<TIME64>((year -   1) / 100     ) * areg::DAY_TO_MICROSECS
            + static_cast<TIME64>((year + 299) / 400     ) * areg::DAY_TO_MICROSECS;
}

AREG_API_IMPL TIME64 areg::convToTime(const tm& time)
{
    return    static_cast<TIME64>(time.tm_sec               ) * areg::SEC_TO_MICROSECS
            + static_cast<TIME64>(time.tm_min               ) * areg::MIN_TO_MICROSECS
            + static_cast<TIME64>(time.tm_hour              ) * areg::HOUR_TO_MICROSECS
            + static_cast<TIME64>(time.tm_yday              ) * areg::DAY_TO_MICROSECS
            + static_cast<TIME64>(time.tm_year - 70         ) * areg::YEAR_TO_MICROSECS
            + static_cast<TIME64>((time.tm_year - 69)  / 4  ) * areg::DAY_TO_MICROSECS
            - static_cast<TIME64>((time.tm_year - 1)   / 100) * areg::DAY_TO_MICROSECS
            + static_cast<TIME64>((time.tm_year + 299) / 400) * areg::DAY_TO_MICROSECS;
}

AREG_API_IMPL void areg::convToSystemTime( const TIME64 & timeValue, areg::sSystemTime & sysTime )
{
    _osConvToSystemTime(timeValue, sysTime);
}

AREG_API_IMPL areg::DataLiteral areg::convDataSize( uint64_t dataSize )
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

AREG_API_IMPL areg::DataLiteral areg::convDuration( uint64_t timeDuration )
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
