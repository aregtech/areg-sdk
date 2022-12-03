/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/base/private/NEUtilities.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
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

    extern void _osSystemTimeNow( NEUtilities::sFileTime & OUT fileTime, bool localTime );

    extern TIME64 _osSystemTimeNow( void );

    extern TIME64 _osConvToTime( const NEUtilities::sSystemTime & IN sysTime );

    extern void _osConvToSystemTime( const TIME64 & IN timeValue, NEUtilities::sSystemTime & OUT sysTime );
    
    extern void _osConvToSystemTime( const NEUtilities::sFileTime & IN fileTime, NEUtilities::sSystemTime & OUT sysTime );

    extern void _osConvToFileTime( const NEUtilities::sSystemTime & IN sysTime, NEUtilities::sFileTime & OUT fileTime );
    
    NEMath::eCompare compareLargeIntegers( const NEMath::uLargeInteger & lsh, const NEMath::uLargeInteger & rhs )
    {
        NEMath::eCompare result = NEMath::eCompare::Equal;
        if ( lsh.u.highPart < rhs.u.highPart )
            result = NEMath::eCompare::Smaller;
        else if ( lsh.u.highPart > rhs.u.highPart )
            result = NEMath::eCompare::Bigger;
        else if ( lsh.u.lowPart < rhs.u.lowPart )
            result = NEMath::eCompare::Smaller;
        else if ( lsh.u.lowPart > rhs.u.lowPart )
            result = NEMath::eCompare::Bigger;

        return result;
    }
}
/************************************************************************/
// NEUtilities namespace global functions
/************************************************************************/
AREG_API_IMPL NEMath::eCompare NEUtilities::compareTimes( const TIME64 & lsh, const TIME64 & rhs )
{
    NEMath::uLargeInteger lshLi, rshLi;
    lshLi.quadPart  = lsh;
    rshLi.quadPart  = rhs;

    return NEUtilities::compareLargeIntegers(lshLi, rshLi);
}

AREG_API_IMPL void NEUtilities::convToTm(const sSystemTime & sysTime, tm & OUT time)
{
    if (sysTime.stYear >= 1900)
    {
        time.tm_sec     = static_cast<int>( sysTime.stSecond);
        time.tm_min     = static_cast<int>( sysTime.stMinute);
        time.tm_hour    = static_cast<int>( sysTime.stHour);
        time.tm_mday    = static_cast<int>( sysTime.stDay);
        time.tm_mon     = static_cast<int>( sysTime.stMonth - 1);   // tm_mon is 0 based
        time.tm_year    = static_cast<int>( sysTime.stYear - 1900); // tm_year is 1900 based
        time.tm_wday    = static_cast<int>( sysTime.stDayOfWeek);
        time.tm_isdst   = -1;
    }
    else
    {
        NEMemory::zeroElement<tm>(time);
    }
}

AREG_API_IMPL void NEUtilities::makeTmLocal( struct tm & IN OUT utcTime )
{
    time_t _timer = mktime( &utcTime );
#ifdef  _WIN32
    localtime_s( &utcTime, &_timer );
#else   // _WIN32
    struct tm * temp = localtime( &_timer );
    if ( temp != nullptr )
        NEMemory::memCopy( &utcTime, static_cast<unsigned int>(sizeof(tm)), temp, static_cast<unsigned int>(sizeof(tm)) );
#endif  // _WIN32
}

AREG_API_IMPL void NEUtilities::convToSystemTime(const tm & time, sSystemTime & OUT sysTime)
{
    sysTime.stSecond    = static_cast<unsigned short>(time.tm_sec);
    sysTime.stMinute    = static_cast<unsigned short>(time.tm_min);
    sysTime.stHour      = static_cast<unsigned short>(time.tm_hour);
    sysTime.stDay       = static_cast<unsigned short>(time.tm_mday);
    sysTime.stMonth     = static_cast<unsigned short>(time.tm_mon + 1);
    sysTime.stYear      = static_cast<unsigned short>(time.tm_year + 1900);
    sysTime.stDayOfWeek = static_cast<unsigned short>(time.tm_wday);
}

AREG_API_IMPL NEMath::eCompare NEUtilities::compareTimes( const NEUtilities::sSystemTime & lsh, const NEUtilities::sSystemTime & rhs )
{
    sFileTime lshFile, rshFile;
    NEUtilities::convToFileTime( lsh, lshFile );
    NEUtilities::convToFileTime( rhs, rshFile );

    return NEUtilities::compareTimes(lshFile, rshFile);
}

AREG_API_IMPL NEMath::eCompare NEUtilities::compareTimes( const NEUtilities::sFileTime & lsh, const NEUtilities::sFileTime & rhs )
{
    NEMath::uLargeInteger lshLi, rshLi;
    lshLi.u.lowPart = lsh.ftLowDateTime;
    lshLi.u.highPart= lsh.ftHighDateTime;

    rshLi.u.lowPart = rhs.ftLowDateTime;
    rshLi.u.highPart= rhs.ftHighDateTime;

    return NEUtilities::compareLargeIntegers( lshLi, rshLi );
}

AREG_API_IMPL TIME64 NEUtilities::convToTime( const NEUtilities::sFileTime & IN fileTime )
{
    NEMath::uLargeInteger li;
    li.u.lowPart    = fileTime.ftLowDateTime;
    li.u.highPart   = fileTime.ftHighDateTime;

    return static_cast<uint64_t>(li.quadPart);
}

AREG_API_IMPL String NEUtilities::createComponentItemName( const char * componentName, const char* itemName )
{
    String result( componentName != nullptr ? componentName : String::EmptyString );
    if ((result.isEmpty() == false) && (NEString::isEmpty<char>(itemName) == false))
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
        result    = String::EmptyString;
    }

    return result;
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
        result    = String::EmptyString;
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
    return _osConvToLocalTime( static_cast<const sSystemTime & >(utcTime), static_cast<sSystemTime &>(localTime) );
}

AREG_API_IMPL bool NEUtilities::convToLocalTime( const TIME64 & IN utcTime, sSystemTime & OUT localTime )
{
    return _osConvToLocalTime( static_cast<const TIME64 &>(utcTime), static_cast<sSystemTime &>(localTime) );
}

AREG_API_IMPL void NEUtilities::systemTimeNow( NEUtilities::sSystemTime & OUT sysTime, bool localTime )
{
    _osSystemTimeNow( static_cast<NEUtilities::sSystemTime &>(sysTime), localTime );
}

AREG_API_IMPL void NEUtilities::systemTimeNow( NEUtilities::sFileTime & OUT fileTime, bool localTime )
{
    _osSystemTimeNow( static_cast<NEUtilities::sFileTime &>(fileTime), localTime );
}

AREG_API_IMPL TIME64 NEUtilities::systemTimeNow( void )
{
    return _osSystemTimeNow( );
}

AREG_API_IMPL TIME64 NEUtilities::convToTime( const NEUtilities::sSystemTime & IN sysTime )
{
    return _osConvToTime( sysTime );
}

AREG_API_IMPL void NEUtilities::convToSystemTime( const TIME64 & IN timeValue, NEUtilities::sSystemTime & OUT sysTime )
{
    _osConvToSystemTime( static_cast<const TIME64 &>(timeValue), static_cast<NEUtilities::sSystemTime &>(sysTime) );
}

AREG_API_IMPL void NEUtilities::convToSystemTime( const NEUtilities::sFileTime & IN fileTime, NEUtilities::sSystemTime & OUT sysTime )
{
    _osConvToSystemTime( static_cast<const NEUtilities::sFileTime &>(fileTime), static_cast<NEUtilities::sSystemTime &>(sysTime) );
}

AREG_API_IMPL void NEUtilities::convToFileTime( const NEUtilities::sSystemTime & IN sysTime, NEUtilities::sFileTime & OUT fileTime )
{
    _osConvToFileTime( sysTime, fileTime );
}

AREG_API_IMPL void NEUtilities::convToFileTime( const TIME64 & IN timeValue, NEUtilities::sFileTime & OUT fileTime )
{
    uint64_t quad = timeValue + WIN_TO_POSIX_EPOCH_BIAS_MICROSECS;

    fileTime.ftLowDateTime = MACRO_64_LO_BYTE32( quad );
    fileTime.ftHighDateTime = MACRO_64_HI_BYTE32( quad );
}
