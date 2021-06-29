/************************************************************************
 * \file        areg/base/private/DateTime.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Date and Time class.
 *
 ************************************************************************/
#include "areg/base/DateTime.hpp"
#include "areg/base/IEIOStream.hpp"
#include "areg/base/NEUtilities.hpp"

#include <time.h>


const char * const      DateTime::FORMAT_MILLISECOND                  = "%l";

// const char * const      DateTime::TIME_FORMAT_ISO8601                 = "ISO8601";
const char * const      DateTime::TIME_FORMAT_ISO8601_OUTPUT          = "%Y-%m-%d %H:%M:%S,%l";

// const char * const      DateTime::TIME_FORMAT_ABSOLUTE                = "ABSOLUTE";
const char * const      DateTime::TIME_FORMAT_ABSOLUTE_OUTPUT         = "%H:%M:%S,%l";

// const char * const      DateTime::TIME_FORMAT_DATE                    = "DATE";
const char * const      DateTime::TIME_FORMAT_DATE_OUTPUT             = "%d %b %Y %H:%M:%S,%l";


DateTime::DateTime( void )
    : mDateTime ( 0 )
{
    ; // do nothing
}

DateTime::DateTime( const NEUtilities::sSystemTime & sysTime )
    : mDateTime( NEUtilities::convToTime(sysTime) )
{
    ; // do nothing
}

DateTime::DateTime( const NEUtilities::sFileTime & fileTime )
    : mDateTime( NEUtilities::convToTime(fileTime) )
{
    ; // do nothing
}

DateTime::DateTime( const DateTime & src )
    : mDateTime ( src.mDateTime )
{
    ; // do nothing
}

DateTime::DateTime( const TIME64 & dateTime )
    : mDateTime ( dateTime )
{
    ; // do nothing
}

DateTime::DateTime( const IEInStream & stream )
    : mDateTime ( stream.read64Bits() )
{
    ; // do nothing
}

DateTime::~DateTime(void)
{
    ; // do nothing
}

uint64_t DateTime::getProcessTickCount(void)
{
#ifdef WINDOWS
    
    return static_cast<uint64_t>( clock() );

#else   // _POSIX

    struct timespec ts;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts);
    return ((ts.tv_sec * NEUtilities::SEC_TO_MILLISECS)+ (ts.tv_nsec / NEUtilities::MILLISEC_TO_NS));

#endif // WINDOWS

}

uint64_t DateTime::getSystemTickCount( void )
{
    return NEUtilities::getTickCount();
}


void DateTime::convToSystemTime( NEUtilities::sSystemTime & out_sysTime ) const
{
    NEUtilities::convToSystemTime(mDateTime, out_sysTime);
}

void DateTime::convFromSystemTime( const NEUtilities::sSystemTime & sysTime )
{
    mDateTime = NEUtilities::convToTime(sysTime);
}

bool DateTime::operator == (const DateTime & other) const
{
    // return (this != &other ? NEUtilities::CompareTimes( mDateTime, other.mDateTime ) == 0 : true);
    return (mDateTime == other.mDateTime);
}

bool DateTime::operator != (const DateTime & other) const
{
    // return (this != &other ? NEUtilities::CompareTimes( mDateTime, other.mDateTime ) != 0 : false);
    return (mDateTime != other.mDateTime);
}

bool DateTime::operator > (const DateTime & other) const
{
    // return (this != &other ? NEUtilities::CompareTimes( mDateTime, other.mDateTime ) == NEMath::CompGreater : false);
    return (mDateTime > other.mDateTime);
}

bool DateTime::operator < (const DateTime & other) const
{
    // return (this != &other ? NEUtilities::CompareTimes( mDateTime, other.mDateTime ) == NEMath::CompSmaller : false);
    return (mDateTime < other.mDateTime);
}

bool DateTime::operator >= (const DateTime & other) const
{
    return (mDateTime >= other.mDateTime);

    /**********************
    bool result = true;
    if ( this != &other )
    {
        NEMath::eCompare comp = NEUtilities::CompareTimes( mDateTime, other.mDateTime );
        result = (comp == NEMath::CompEqual) || (comp == NEMath::CompGreater);
    }
    return result;
    **********************/
}

bool DateTime::operator <= (const DateTime & other) const
{
    return (mDateTime <= other.mDateTime);
    /**********************
    bool result = true;
    if ( this != &other )
    {
        NEMath::eCompare comp = NEUtilities::CompareTimes( mDateTime, other.mDateTime );
        result = (comp == NEMath::CompEqual) || (comp == NEMath::CompSmaller);
    }
    return result;
    **********************/
}

DateTime DateTime::getNow( void )
{
    return DateTime( NEUtilities::systemTimeNow() );
}

void DateTime::getNow( NEUtilities::sSystemTime & OUT timeData, bool localTime )
{
    NEUtilities::systemTimeNow(timeData, localTime);
//     TIME64 quad = NEUtilities::systemTimeNow();
//     NEUtilities::convToLocalTime(quad, outSysTime);
}


String DateTime::formatTime( const char * formatName /*= DateTime::TIME_FORMAT_ISO8601_OUTPUT */ ) const
{
    char buffer[128] = {0};
    
    if ( mDateTime != 0 )
    {
        NEUtilities::sSystemTime sysTime;
        struct tm conv;

        NEUtilities::convToLocalTime( mDateTime, sysTime);
        NEUtilities::convToTm(sysTime, conv);

        unsigned short milli = sysTime.stMillisecs;

        String str = NEString::isEmpty( formatName ) == false ? formatName : DateTime::TIME_FORMAT_ISO8601_OUTPUT;
        NEString::CharPos ms = str.findFirstOf( DateTime::FORMAT_MILLISECOND );
        if ( ms != NEString::InvalidPos )
        {
            char buf[12];
            String::formatString( buf, 12, "%03d", milli );
            str.replace( buf, ms, NEString::getStringLength<char>( DateTime::FORMAT_MILLISECOND ) );
        }

        strftime( buffer, 128, str.getString(), &conv );
    }
    else
    {
        *buffer = String::EmptyChar;
    }

    return String(buffer);
}

AREG_API const IEInStream & operator >> ( const IEInStream & stream, DateTime & input )
{
    stream >> input.mDateTime;
    return stream;
}

AREG_API IEOutStream & operator << ( IEOutStream & stream, const DateTime & output )
{
    stream << output.mDateTime;
    return stream;
}
