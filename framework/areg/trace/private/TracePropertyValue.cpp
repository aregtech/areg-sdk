/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/trace/private/TracePropertyValue.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       Trace Property value class. Used when reading or 
 *              saving configuration file
 ************************************************************************/
#include "areg/trace/private/TracePropertyValue.hpp"

#include "areg/trace/private/NELogConfig.hpp"
#include "areg/base/NEUtilities.hpp"

#include <utility>

TracePropertyValue::TracePropertyValue( void )
    : mValue    ( String::getEmptyString(), 0 )
    , mPriority ( static_cast<unsigned int>(NETrace::PrioInvalid) )
{
}

TracePropertyValue::TracePropertyValue( const char * initValue )
    : mValue    ( String::getEmptyString(), 0 )
    , mPriority ( static_cast<unsigned int>(NETrace::PrioInvalid) )
{
    setData(initValue);
}

TracePropertyValue::TracePropertyValue( const String & initValue )
    : mValue    ( String::getEmptyString(), 0 )
    , mPriority ( static_cast<unsigned int>(NETrace::PrioNotset) )
{
    setData( initValue.getString() );
}

TracePropertyValue::TracePropertyValue( const TracePropertyValue & source )
    : mValue    ( source.mValue )
    , mPriority ( source.mPriority )
{
}

TracePropertyValue::TracePropertyValue( TracePropertyValue && source ) noexcept
    : mValue    ( std::move(source.mValue) )
    , mPriority ( std::move(source.mPriority) )
{
}

TracePropertyValue & TracePropertyValue::operator = ( bool newValue )
{
    mValue = newValue ? NELogConfig::SYNTAX_TRUE : NELogConfig::SYNTAX_FALSE;
    return (*this);
}

TracePropertyValue::operator unsigned int ( void ) const
{
    return static_cast<unsigned int>(mValue.isEmpty( ) == false ? mValue.toUInt32( NEString::eRadix::RadixDecimal ) : NELogConfig::DEFAULT_INTEGER_VALUE);
}

TracePropertyValue::operator unsigned short(void) const
{
    return static_cast<unsigned short>(mValue.isEmpty() == false ? mValue.toUInt32(NEString::eRadix::RadixDecimal) : NELogConfig::DEFAULT_INTEGER_VALUE);
}

TracePropertyValue::operator bool ( void ) const
{
    if ( mValue.isEmpty( ) == false )
    {
        return mValue.compare( NELogConfig::SYNTAX_TRUE.data( ), NEString::START_POS, NEString::END_POS, false ) == NEMath::eCompare::Equal;
    }
    else
    {
        return NELogConfig::DEFAULT_BOOLEAN_VALUE;
    }
}

TracePropertyValue::operator unsigned char ( void ) const
{
    return (mValue.isEmpty( ) == false ? static_cast<unsigned char>(mValue.getAt( NEString::START_POS )) : static_cast<unsigned char>(NELogConfig::DEFAULT_INTEGER_VALUE));
}

TracePropertyValue::operator float ( void ) const
{
    return (mValue.isEmpty( ) == false ? mValue.toFloat( ) : static_cast<float>(NELogConfig::DEFAULT_INTEGER_VALUE));
}

TracePropertyValue & TracePropertyValue::operator += ( NETrace::eLogPriority newValue )
{
    setPriority( newValue );
    return (*this);
}

unsigned int TracePropertyValue::setPriority( NETrace::eLogPriority newPriority )
{
    if ( NETrace::isValidLogPriority(newPriority))
    {
        if ( newPriority == NETrace::eLogPriority::PrioScope )
        {
            mPriority &= static_cast<unsigned int>(NETrace::eLogPriority::PrioLogs);
            mPriority |= static_cast<unsigned int>(NETrace::eLogPriority::PrioScope);
        }
        else
        {
            mPriority &= static_cast<unsigned int>(NETrace::eLogPriority::PrioScope);
            mPriority |= static_cast<unsigned int>(newPriority);
        }

        mValue = NETrace::makePrioString( mPriority );
    }
    else
    {
        mPriority = static_cast<unsigned int>(NETrace::eLogPriority::PrioNotset);
        mValue = NETrace::convToString( NETrace::eLogPriority::PrioNotset );
    }

    return mPriority;
}

void TracePropertyValue::setData( String newValue )
{
    NEString::CharPos pos = newValue.findFirst( NELogConfig::SYNTAX_END_COMMAND_DELIMITER );
    if ( newValue.isValidPosition( pos ) )
    {
        newValue.substring(0, pos );
    }

    newValue.trimAll( );
    mValue = newValue;
    mPriority = static_cast<unsigned int>(NETrace::eLogPriority::PrioInvalid);

    pos = newValue.findFirst( NELogConfig::SYNTAX_LOGICAL_OR.data( ) );
    if ( newValue.isValidPosition( pos ) )
    {
        String strPrio1, strPrio2;
        newValue.substring( strPrio1, 0, pos );
        newValue.substring( strPrio2, pos + 1);

        strPrio1.trimAll( );
        strPrio2.trimAll( );

        NETrace::eLogPriority prio1 = NETrace::convFromString( strPrio1 );
        NETrace::eLogPriority prio2 = NETrace::convFromString( strPrio2 );

        if ( NETrace::isValidLogPriority( prio1 ) && NETrace::isValidLogPriority( prio2 ) )
        {
            mPriority = static_cast<unsigned int>(prio1) | static_cast<unsigned int>(prio2);
            mValue = NETrace::makePrioString( mPriority );
        }
    }
    else
    {

        NETrace::eLogPriority prio = NETrace::convFromString( newValue );
        if ( NETrace::isValidLogPriority( prio ) )
        {
            mPriority = static_cast<unsigned int>(prio);
            mValue = NETrace::convToString( prio );
        }
    }
}
