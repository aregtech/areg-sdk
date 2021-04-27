/************************************************************************
 * \file        areg/base/private/CEDateTime.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Date and Time class.
 *
 ************************************************************************/
#include "areg/base/CEDateTime.hpp"
#include "areg/base/IEIOStream.hpp"
#include "areg/base/NEUtilities.hpp"

#include <time.h>


const char * const      CEDateTime::FORMAT_MILLISECOND                  = "%l";

// const char * const      CEDateTime::TIME_FORMAT_ISO8601                 = "ISO8601";
const char * const      CEDateTime::TIME_FORMAT_ISO8601_OUTPUT          = "%Y-%m-%d %H:%M:%S,%l";

// const char * const      CEDateTime::TIME_FORMAT_ABSOLUTE                = "ABSOLUTE";
const char * const      CEDateTime::TIME_FORMAT_ABSOLUTE_OUTPUT         = "%H:%M:%S,%l";

// const char * const      CEDateTime::TIME_FORMAT_DATE                    = "DATE";
const char * const      CEDateTime::TIME_FORMAT_DATE_OUTPUT             = "%d %b %Y %H:%M:%S,%l";


CEDateTime::CEDateTime( void )
    : mDateTime ( 0 )
{
    ; // do nothing
}

CEDateTime::CEDateTime( const NEUtilities::sSystemTime & sysTime )
    : mDateTime( NEUtilities::ConvertSystemTime(sysTime) )
{
    ; // do nothing
}

CEDateTime::CEDateTime( const NEUtilities::sFileTime & fileTime )
    : mDateTime( NEUtilities::ConvertFileTime(fileTime) )
{
    ; // do nothing
}

CEDateTime::CEDateTime( const CEDateTime & src )
    : mDateTime ( src.mDateTime )
{
    ; // do nothing
}

CEDateTime::CEDateTime( const TIME64 & dateTime )
    : mDateTime ( dateTime )
{
    ; // do nothing
}

CEDateTime::CEDateTime( const IEInStream & stream )
    : mDateTime ( stream.Read64Bits() )
{
    ; // do nothing
}

CEDateTime::~CEDateTime(void)
{
    ; // do nothing
}

uint64_t CEDateTime::GetProcessTickClocks(void)
{
#ifdef WINDOWS
    
    return static_cast<uint64_t>( clock() );

#else   // _POSIX

    struct timespec ts;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts);
    return ((ts.tv_sec * NEUtilities::SEC_TO_MILLISECS)+ (ts.tv_nsec / NEUtilities::MILLISEC_TO_NS));

#endif // WINDOWS

}

uint64_t CEDateTime::GetTickCount( void )
{
    return NEUtilities::GetTickCount();
}


void CEDateTime::ToSystemTime( NEUtilities::sSystemTime & out_sysTime ) const
{
    NEUtilities::ConvertToSystemTime(mDateTime, out_sysTime);
}

void CEDateTime::FromSystemTime( const NEUtilities::sSystemTime & sysTime )
{
    mDateTime = NEUtilities::ConvertSystemTime(sysTime);
}

bool CEDateTime::operator == (const CEDateTime & other) const
{
    // return (this != &other ? NEUtilities::CompareTimes( mDateTime, other.mDateTime ) == 0 : true);
    return (mDateTime == other.mDateTime);
}

bool CEDateTime::operator != (const CEDateTime & other) const
{
    // return (this != &other ? NEUtilities::CompareTimes( mDateTime, other.mDateTime ) != 0 : false);
    return (mDateTime != other.mDateTime);
}

bool CEDateTime::operator > (const CEDateTime & other) const
{
    // return (this != &other ? NEUtilities::CompareTimes( mDateTime, other.mDateTime ) == NEMath::CompGreater : false);
    return (mDateTime > other.mDateTime);
}

bool CEDateTime::operator < (const CEDateTime & other) const
{
    // return (this != &other ? NEUtilities::CompareTimes( mDateTime, other.mDateTime ) == NEMath::CompSmaller : false);
    return (mDateTime < other.mDateTime);
}

bool CEDateTime::operator >= (const CEDateTime & other) const
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

bool CEDateTime::operator <= (const CEDateTime & other) const
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

CEDateTime CEDateTime::GetNow( void )
{
    return CEDateTime( NEUtilities::GetSystemTimeNow() );
}

void CEDateTime::GetNow( NEUtilities::sSystemTime & outSysTime )
{
    TIME64 quad = NEUtilities::GetSystemTimeNow();
    NEUtilities::ToLocalTime(quad, outSysTime);
}


CEString CEDateTime::FormatTime( const char * formatName /*= CEDateTime::TIME_FORMAT_ISO8601_OUTPUT */ ) const
{
    char buffer[128] = {0};
    
    if ( mDateTime != 0 )
    {
        time_t secs = static_cast<time_t>(mDateTime / NEUtilities::SEC_TO_MICROSECS);
        time_t rest = mDateTime - (secs * NEUtilities::SEC_TO_MICROSECS);
        unsigned short milli = static_cast<unsigned short>(rest / NEUtilities::MILLISEC_TO_MICROSECS);

        struct tm conv;
#ifdef WINDOWS
        localtime_s(&conv, &secs);
#else
        localtime_r(&secs, &conv);
#endif // WINDOWS

        CEString str = NEString::isEmpty( formatName ) == false ? formatName : CEDateTime::TIME_FORMAT_ISO8601_OUTPUT;
        NEString::CharPos ms = str.FindFirstOf( CEDateTime::FORMAT_MILLISECOND );
        if ( ms != NEString::InvalidPos )
        {
            char buf[12];
            CEString::PrintString( buf, 12, "%03d", milli );
            str.Replace( buf, ms, NEString::getStringLength<char>( CEDateTime::FORMAT_MILLISECOND ) );
        }

        strftime( buffer, 128, str.GetBuffer( ), &conv );
    }
    else
    {
        *buffer = CEString::EmptyChar;
    }

    return CEString(buffer);
}

AREG_API const IEInStream & operator >> ( const IEInStream & stream, CEDateTime & input )
{
    stream >> input.mDateTime;
    return stream;
}

AREG_API IEOutStream & operator << ( IEOutStream & stream, const CEDateTime & output )
{
    stream << output.mDateTime;
    return stream;
}
