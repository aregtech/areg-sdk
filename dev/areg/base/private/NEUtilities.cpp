/************************************************************************
 * \file        areg/base/private/NEUtilities.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Switches
 *
 ************************************************************************/

#include "areg/base/NEUtilities.hpp"
#include "areg/base/ESynchObjects.hpp"
#include "areg/base/CEString.hpp"
#include "areg/base/CEWideString.hpp"

#include <time.h>

/************************************************************************
 * \brief   Local predefined constants
 ************************************************************************/
/**
 * \brief   ASCII type of end of string value
 **/
#define EOS_A           static_cast<char>('\0')
/**
 * \brief   Unicode type of end of string value
 **/
#define EOS_W           static_cast<wchar_t>('\0')
/**
 * \brief   ASCII type of new line / end line value
 **/
#define EOL_A           static_cast<char>('\n')
/**
 * \brief   Unicode type of new line / end of line value
 **/
#define EOL_W           static_cast<wchar_t>('\n')
/**
 * \brief   ASCII type of carriage return / end line value
 **/
#define CR_A            static_cast<char>('\r')
/**
 * \brief   Unicode type of carriage return / end of line value
 **/
#define CR_W            static_cast<wchar_t>('\r')

/**
 * \brief   Checks whether ASCII character can be considered as end of line value
 **/
#define IS_EOL_A(ch)    ((ch) == EOS_A || (ch) == EOL_A || (ch) == CR_A)
/**
 * \brief   Checks whether Unicode character can be considered as end of line value
 **/
#define IS_EOL_W(ch)    ((ch) == EOS_W || (ch) == EOL_W || (ch) == CR_W)


/************************************************************************/
// static local functions
/************************************************************************/
namespace NEUtilities 
{
    extern void _generateName(const char * prefix, char * out_buffer, int length, const char * specChar);


    NEMath::eCompare compareLargeIntegers( const NEMath::uLargeInteger & lsh, const NEMath::uLargeInteger & rhs )
    {
        NEMath::eCompare result = NEMath::CompEqual;
        if ( lsh.u.highPart < rhs.u.highPart )
            result = NEMath::CompSmaller;
        else if ( lsh.u.highPart > rhs.u.highPart )
            result = NEMath::CompGreater;
        else if ( lsh.u.lowPart < rhs.u.lowPart )
            result = NEMath::CompSmaller;
        else if ( lsh.u.lowPart > rhs.u.lowPart )
            result = NEMath::CompGreater;

        return result;
    }
}
/************************************************************************/
// NEUtilities namespace global functions
/************************************************************************/

AREG_API NEMath::eCompare NEUtilities::CompareTimes( const uint64_t & lsh, const uint64_t & rhs )
{
    NEMath::uLargeInteger lshLi, rshLi;
    lshLi.quadPart  = lsh;
    rshLi.quadPart  = rhs;

    return NEUtilities::compareLargeIntegers(lshLi, rshLi);
}

AREG_API void NEUtilities::SysTimeToTime(const sSystemTime & sysTime, tm & out_time)
{
    if (sysTime.stYear >= 1900)
    {
        out_time.tm_sec     = static_cast<int>( sysTime.stSecond);
        out_time.tm_min     = static_cast<int>( sysTime.stMinute);
        out_time.tm_hour    = static_cast<int>( sysTime.stHour);
        out_time.tm_mday    = static_cast<int>( sysTime.stDay);
        out_time.tm_mon     = static_cast<int>( sysTime.stMonth - 1);   // tm_mon is 0 based
        out_time.tm_year    = static_cast<int>( sysTime.stYear - 1900); // tm_year is 1900 based
        out_time.tm_wday    = static_cast<int>( sysTime.stDayOfWeek);
        out_time.tm_isdst   = -1;

        time_t _timer = mktime( &out_time );
#ifdef  _WIN32
        localtime_s( &out_time, &_timer );
#else   // _WIN32
        struct tm * temp = localtime( &_timer );
        if ( temp != NULL )
            NEMemory::MemCopy( &out_time, static_cast<unsigned int>(sizeof(tm)), temp, static_cast<unsigned int>(sizeof(tm)) );
#endif  // _WIN32
    }
    else
    {
        NEMemory::ZeroData<tm>(out_time);
    }
}

AREG_API void NEUtilities::TimeToSysTime(const tm & time, sSystemTime & out_sysTime)
{
    out_sysTime.stSecond    = static_cast<unsigned short>(time.tm_sec);
    out_sysTime.stMinute    = static_cast<unsigned short>(time.tm_min);
    out_sysTime.stHour      = static_cast<unsigned short>(time.tm_hour);
    out_sysTime.stDay       = static_cast<unsigned short>(time.tm_mday);
    out_sysTime.stMonth     = static_cast<unsigned short>(time.tm_mon + 1);
    out_sysTime.stYear      = static_cast<unsigned short>(time.tm_year + 1900);
    out_sysTime.stDayOfWeek = static_cast<unsigned short>(time.tm_wday);
}

AREG_API void NEUtilities::ConvertToFileTime( const uint64_t &  timeValue, NEUtilities::sFileTime & out_fileTime )
{
    uint64_t quad = timeValue + WIN_TO_POSIX_EPOCH_BIAS_MICROSECS;

    out_fileTime.ftLowDateTime  = MACRO_64_LO_BYTE32(quad);
    out_fileTime.ftHighDateTime = MACRO_64_HI_BYTE32(quad);
}

AREG_API NEMath::eCompare NEUtilities::CompareSystemTimes( const NEUtilities::sSystemTime & lsh, const NEUtilities::sSystemTime & rhs )
{
    sFileTime lshFile, rshFile;
    NEUtilities::ConvertSystemTimeToFileTime( lsh, lshFile );
    NEUtilities::ConvertSystemTimeToFileTime( rhs, rshFile );

    return NEUtilities::CompareFileTimes(lshFile, rshFile);
}

AREG_API NEMath::eCompare NEUtilities::CompareFileTimes( const NEUtilities::sFileTime & lsh, const NEUtilities::sFileTime & rhs )
{
    NEMath::uLargeInteger lshLi, rshLi;
    lshLi.u.lowPart = lsh.ftLowDateTime;
    lshLi.u.highPart= lsh.ftHighDateTime;

    rshLi.u.lowPart = rhs.ftLowDateTime;
    rshLi.u.highPart= rhs.ftHighDateTime;

    return NEUtilities::compareLargeIntegers( lshLi, rshLi );
}

AREG_API uint64_t NEUtilities::ConvertFileTime( const NEUtilities::sFileTime & fileTime )
{
    NEMath::uLargeInteger li;
    li.u.lowPart    = fileTime.ftLowDateTime;
    li.u.highPart   = fileTime.ftHighDateTime;

    return static_cast<uint64_t>(li.quadPart);
}

AREG_API CEString NEUtilities::CreateComponentItemName( const char * componentName, const char* itemName )
{
    CEString result = componentName != NULL ? componentName : "";
    if ((result.IsEmpty() == false) && (NEString::isEmpty<char>(itemName) == false))
    {
        result += NEUtilities::COMPONENT_ITEM_SEPARATOR;
        result += itemName;

        if (result.GetLength() > NEUtilities::MAX_GENERATED_NAME_BUFFER_SIZE)
            result = result.Substr(0, NEUtilities::MAX_GENERATED_NAME_BUFFER_SIZE);
    }
    else
    {
        result    = "";
    }
    return result;
}

AREG_API CEString NEUtilities::GenerateName( const char* prefix )
{
    char buffer[NEUtilities::MAX_GENERATED_NAME_BUFFER_SIZE];
    NEUtilities::GenerateName(prefix, buffer, NEUtilities::MAX_GENERATED_NAME_BUFFER_SIZE);
    return CEString(buffer);
}

AREG_API void NEUtilities::GenerateName(const char * prefix, char * out_buffer, int length)
{
    return NEUtilities::GenerateName(prefix, out_buffer, length, NEUtilities::DEFAULT_SPECIAL_CHAR);
}

AREG_API void NEUtilities::GenerateName(const char * prefix, char * out_buffer, int length, const char * specChar)
{
    NEUtilities::_generateName(prefix, out_buffer, length, specChar);
}

AREG_API unsigned int NEUtilities::GenerateUniqueId( void )
{
    static CEInterlockedValue _id(static_cast<unsigned int>(0));
    return _id.Increment();
}
