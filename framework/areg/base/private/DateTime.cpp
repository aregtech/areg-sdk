/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/private/DateTime.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Date and Time class.
 *
 ************************************************************************/
#include "areg/base/DateTime.hpp"
#include "areg/base/NEUtilities.hpp"

#include <time.h>

namespace 
{
    /**
     * \brief   Output format of milliseconds
     **/
    constexpr std::string_view   FORMAT_MILLISECOND{ "%l" };
}


DateTime::DateTime( void )
    : mDateTime ( INVALID_TIME )
{
}

DateTime::DateTime( const NEUtilities::sSystemTime & sysTime )
    : mDateTime( NEUtilities::convToTime(sysTime) )
{
}

DateTime::DateTime( const NEUtilities::sFileTime & fileTime )
    : mDateTime( NEUtilities::convToTime(fileTime) )
{
}

DateTime::DateTime( const DateTime & dateTime )
    : mDateTime ( dateTime.mDateTime )
{
}

DateTime::DateTime( DateTime && dateTime ) noexcept
    : mDateTime ( dateTime.mDateTime )
{
}

DateTime::DateTime( const TIME64 & dateTime )
    : mDateTime ( dateTime )
{
}

DateTime::DateTime( const IEInStream & stream )
    : mDateTime ( stream.read64Bits() )
{
}

uint64_t DateTime::getProcessTickCount(void)
{
#ifdef WINDOWS
    
    return static_cast<uint64_t>( clock() );

#else   // _POSIX

    struct timespec ts;
    ::clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts);
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
    return (mDateTime == other.mDateTime);
}

bool DateTime::operator != (const DateTime & other) const
{
    return (mDateTime != other.mDateTime);
}

bool DateTime::operator > (const DateTime & other) const
{
    return (mDateTime > other.mDateTime);
}

bool DateTime::operator < (const DateTime & other) const
{
    return (mDateTime < other.mDateTime);
}

bool DateTime::operator >= (const DateTime & other) const
{
    return (mDateTime >= other.mDateTime);
}

bool DateTime::operator <= (const DateTime & other) const
{
    return (mDateTime <= other.mDateTime);
}

DateTime DateTime::getNow( void )
{
    return DateTime( NEUtilities::systemTimeNow() );
}

void DateTime::getNow( NEUtilities::sSystemTime & OUT timeData, bool localTime )
{
    NEUtilities::systemTimeNow(timeData, localTime);
}

String DateTime::formatTime( const std::string_view & formatName /*= DateTime::TIME_FORMAT_ISO8601_OUTPUT */ ) const
{
    char buffer[128] = {0};
    
    if ( mDateTime != 0 )
    {
        NEUtilities::sSystemTime sysTime{ 0 };
        struct tm conv { 0 };

        NEUtilities::convToLocalTime( mDateTime, sysTime);
        NEUtilities::convToTm(sysTime, conv);

        unsigned short milli = sysTime.stMillisecs;

        String str(formatName.empty() == false ? formatName : DateTime::TIME_FORMAT_ISO8601_OUTPUT);
        NEString::CharPos ms = str.findFirst( FORMAT_MILLISECOND.data() );
        if ( str.isValidPosition(ms) )
        {
            char buf[128];
            String::formatString( buf, 128, "%03u", milli );
            str.replace( ms, static_cast<NEString::CharCount>(FORMAT_MILLISECOND.length()), buf);
        }

        strftime( buffer, 128, str.getString(), &conv );
    }
    else
    {
        *buffer = String::EmptyChar;
    }

    return String(buffer);
}
