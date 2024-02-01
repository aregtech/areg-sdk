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

#include <time.h>
#include <string>


/************************************************************************/
// static local functions
/************************************************************************/
namespace NEUtilities 
{
    extern const char * _osGenerateName(const char * prefix, char * out_buffer, int length, const char * specChar);

    extern uint64_t _osGetTickCount( void );

    extern bool _osConvToLocalTime( const sSystemTime & IN utcTime, sSystemTime & OUT localTime );

    extern bool _osConvToLocalTime( const TIME64 & IN utcTime, sSystemTime & OUT localTime );

    extern void _osSystemTimeNow( NEUtilities::sSystemTime & OUT sysTime, bool localTime );

    extern TIME64 _osConvToTime( const NEUtilities::sSystemTime & IN sysTime );

    extern void _osConvToSystemTime( const TIME64 & IN timeValue, NEUtilities::sSystemTime & OUT sysTime );
    
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
AREG_API_IMPL NEMath::eCompare NEUtilities::compareTimes( const TIME64 & lhs, const TIME64 & rhs )
{
    NEMath::uLargeInteger lhsLi, rshLi;
    lhsLi.quadPart  = lhs;
    rshLi.quadPart  = rhs;

    return NEUtilities::compareLargeIntegers(lhsLi, rshLi);
}

AREG_API_IMPL void NEUtilities::convToTm(const sSystemTime & IN sysTime, struct tm & OUT time)
{
    if (sysTime.stYear >= 1900)
    {
        time.tm_sec     = static_cast<int>( sysTime.stSecond);
        time.tm_min     = static_cast<int>( sysTime.stMinute);
        time.tm_hour    = static_cast<int>( sysTime.stHour);
        time.tm_mday    = static_cast<int>( sysTime.stDay);
        time.tm_mon     = static_cast<int>( sysTime.stMonth - 1);       // tm_mon is 0 based
        time.tm_year    = static_cast<int>( sysTime.stYear - 1900);     // tm_year is 1900 based
        time.tm_wday    = static_cast<int>( sysTime.stDayOfWeek);       // tm_wday is 0 based
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
    NEMemory::memSet( &utcTime, sizeof( struct tm ), 0 );
    time_t _timer = mktime( &utcTime );

#ifdef  _WIN32
    localtime_s( &utcTime, &_timer );
#else   // _WIN32
    localtime_r( &_timer, &utcTime );
#endif  // _WIN32
}

AREG_API_IMPL void NEUtilities::convToSystemTime(const struct tm & IN time, sSystemTime & OUT sysTime)
{
    sysTime.stSecond    = static_cast<int>(time.tm_sec);
    sysTime.stMinute    = static_cast<int>(time.tm_min);
    sysTime.stHour      = static_cast<int>(time.tm_hour);
    sysTime.stDay       = static_cast<int>(time.tm_mday);
    sysTime.stMonth     = static_cast<int>(time.tm_mon + 1);
    sysTime.stYear      = static_cast<int>(time.tm_year + 1970);
    sysTime.stDayOfWeek = static_cast<int>(time.tm_wday);
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
    return NEUtilities::_osGenerateName(prefix, out_buffer, length, specChar);
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
    return _osConvToLocalTime( utcTime, localTime );
}

AREG_API_IMPL bool NEUtilities::convToLocalTime( const TIME64 & IN utcTime, sSystemTime & OUT localTime )
{
    return _osConvToLocalTime( utcTime, localTime );
}

AREG_API_IMPL void NEUtilities::systemTimeNow( NEUtilities::sSystemTime & OUT sysTime, bool localTime )
{
    _osSystemTimeNow( sysTime, localTime );
}

AREG_API_IMPL TIME64 NEUtilities::systemTimeNow( void )
{
    struct timespec ts { 0 };
    std::timespec_get(&ts, TIME_UTC);
    std::chrono::seconds secs{ ts.tv_sec };
    std::chrono::nanoseconds ns{ ts.tv_nsec };
    std::chrono::microseconds now{ std::chrono::duration_cast<std::chrono::microseconds>(secs) + std::chrono::duration_cast<std::chrono::microseconds>(ns) };
    return static_cast<TIME64>(now.count());
}

AREG_API_IMPL TIME64 NEUtilities::convToTime( const NEUtilities::sSystemTime & IN sysTime )
{
    return _osConvToTime( sysTime );
}

AREG_API_IMPL void NEUtilities::convToSystemTime( const TIME64 & IN timeValue, NEUtilities::sSystemTime & OUT sysTime )
{
    _osConvToSystemTime( timeValue, sysTime );
}

/**
AREG_API_IMPL void NEUtilities::convToFileTime( const TIME64 & IN timeValue, NEUtilities::sFileTime & OUT fileTime )
{
    uint64_t quad = timeValue + WIN_TO_POSIX_EPOCH_BIAS_MICROSECS;

    fileTime.ftLowDateTime = MACRO_64_LO_BYTE32( quad );
    fileTime.ftHighDateTime = MACRO_64_HI_BYTE32( quad );
}
**/

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
