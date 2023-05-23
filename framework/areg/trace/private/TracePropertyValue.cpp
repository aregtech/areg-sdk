/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
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

inline void TracePropertyValue::_setValue( const String & newValue )
{
    mValue = newValue;
    NEString::CharPos pos = newValue.findFirst(NELogConfig::SYNTAX_END_COMMAND_DELIMITER);
    if (newValue.isValidPosition(pos))
    {
        newValue.substring(mValue, 0, pos);
    }
    else
    {
        mValue = newValue;
    }
    
    mValue.trimAll();

    String prio = mValue;
    mPriority   = static_cast<unsigned int>(NETrace::PrioNotset);
    pos         = mValue.findFirst(NELogConfig::SYNTAX_LOGICAL_OR.data());
    if ( mValue.isValidPosition(pos) )
    {
        mValue.substring(prio, 0, pos);
        prio.trimAll();
        mPriority |= static_cast<unsigned int>( NETrace::convFromString(prio.getString()));

        mValue.substring(prio, pos + 1);
        prio.trimAll();
        mPriority |= static_cast<unsigned int>( NETrace::convFromString(prio.getString()));
    }
    else
    {
        mPriority |= static_cast<unsigned int>( NETrace::convFromString(prio.getString()));
    }
}


TracePropertyValue::TracePropertyValue( void )
    : mValue    ( String::getEmptyString(), 0 )
    , mPriority ( static_cast<unsigned int>(NETrace::PrioNotset) )
{
}

TracePropertyValue::TracePropertyValue( const char * initValue )
    : mValue    ( String::getEmptyString(), 0 )
    , mPriority ( static_cast<unsigned int>(NETrace::PrioNotset) )
{
    _setValue(initValue);
}

TracePropertyValue::TracePropertyValue( const String & initValue )
    : mValue    ( String::getEmptyString(), 0 )
    , mPriority ( static_cast<unsigned int>(NETrace::PrioNotset) )
{
    _setValue( initValue.getString() );
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
    mValue = newValue ? NELogConfig::SYNTAX_TRUE.data( ) : NELogConfig::SYNTAX_FALSE.data( );
    return (*this);
}

TracePropertyValue::operator unsigned int ( void ) const
{
    return static_cast<unsigned int>(mValue.isEmpty( ) == false ? mValue.toUInt32( NEString::eRadix::RadixDecimal ) : NELogConfig::DEFAULT_INTEGER_VALUE);
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
    int prioInt     = static_cast<NETrace::eLogPriority>(*this);
    String prioStr= NETrace::convToString(newValue);
    if ( prioInt == NETrace::PrioNotset )
    {
        _setValue( prioStr.getString( ) );
    }
    else if (( newValue != NETrace::PrioNotset) && (prioInt & static_cast<int>(newValue)) == 0 )
    {
        _setValue( (mValue + NELogConfig::SYNTAX_LOGICAL_OR.data( ) + prioStr).getString( ) );
    }

    return (*this);
}
