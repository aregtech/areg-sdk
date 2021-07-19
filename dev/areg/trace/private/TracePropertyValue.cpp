/************************************************************************
 * \file        areg/trace/private/TracePropertyValue.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Trace Property value class. Used when reading or 
 *              saving configuration file
 ************************************************************************/
#include "areg/trace/private/TracePropertyValue.hpp"

#include "areg/trace/private/NELogConfig.hpp"
#include "areg/base/NEUtilities.hpp"

inline void TracePropertyValue::_setValue( const char * newValue )
{
    mValue = newValue != NULL ? newValue : "";
    NEString::CharPos pos = mValue.findFirstOf(NELogConfig::SYNTAX_END_COMMAND_DELIMITER);
    if ( pos != NEString::InvalidPos )
        mValue  = mValue.substring(0, pos);
    
    mValue.trimAll();

    String prio = mValue;
    mPriority   = static_cast<unsigned int>(NETrace::PrioNotset);
    pos         = mValue.findFirstOf(NELogConfig::SYNTAX_LOGICAL_OR);
    if ( pos != NEString::InvalidPos )
    {
        prio = mValue.substring(0, pos);
        prio.trimAll();
        mPriority |= static_cast<unsigned int>( NETrace::convFromString(prio.getString()));

        prio = mValue.substring(pos + 1);
        prio.trimAll();
        mPriority |= static_cast<unsigned int>( NETrace::convFromString(prio.getString()));
    }
    else
    {
        mPriority |= static_cast<unsigned int>( NETrace::convFromString(prio.getString()));
    }
}


TracePropertyValue::TracePropertyValue(void)
    : mValue    ( String::EmptyString )
    , mPriority ( static_cast<unsigned int>(NETrace::PrioNotset) )
{
    ; // do nothing
}

TracePropertyValue::TracePropertyValue( const char * initValue )
    : mValue    ( String::EmptyString )
    , mPriority ( static_cast<unsigned int>(NETrace::PrioNotset) )
{
    _setValue(initValue);
}

TracePropertyValue::TracePropertyValue( const String & initValue )
    : mValue    ( String::EmptyString )
    , mPriority ( static_cast<unsigned int>(NETrace::PrioNotset) )
{
    _setValue( initValue.getString() );
}

TracePropertyValue::TracePropertyValue( const TracePropertyValue & source )
    : mValue    ( source.mValue )
    , mPriority ( source.mPriority )
{
    ; // do nothing
}

TracePropertyValue::~TracePropertyValue(void)
{
    ; // do nothing
}

const TracePropertyValue & TracePropertyValue::operator = ( const TracePropertyValue & source )
{
    if ( static_cast<const TracePropertyValue *>(this) != &source )
        mValue  = source.mValue;
    return (*this);
}

void TracePropertyValue::operator = ( const String & newValue )
{
    _setValue(newValue.getString());
}

void TracePropertyValue::operator = ( const char * newValue )
{
    _setValue(newValue);
}

void TracePropertyValue::operator = ( unsigned int newValue )
{
    mValue = String::uint32ToString(newValue, NEString::RadixDecimal);
}

void TracePropertyValue::operator = ( NETrace::eLogPriority newValue )
{
    mValue  = NETrace::convToString(newValue);
}

void TracePropertyValue::operator = ( bool newValue )
{
    mValue = newValue ? NELogConfig::SYNTAX_TRUE : NELogConfig::SYNTAX_FALSE;
}

TracePropertyValue::operator const char * ( void ) const
{
    return static_cast<const char *>(mValue);
}

TracePropertyValue::operator const String & ( void ) const
{
    return mValue;
}

TracePropertyValue::operator unsigned int ( void ) const
{
    return static_cast<unsigned int>( mValue.isEmpty() == false ? mValue.convToUInt32(NEString::RadixDecimal) : NELogConfig::DEFAULT_INTEGER_VALUE );
}

TracePropertyValue::operator NETrace::eLogPriority ( void ) const
{
    return static_cast<NETrace::eLogPriority>(mPriority);
}

TracePropertyValue::operator bool ( void ) const
{
    return ( mValue.isEmpty() == false ? mValue.compare(NELogConfig::SYNTAX_TRUE, NEString::StartPos, NEString::EndPos, false) == 0 : NELogConfig::DEFAULT_BOOLEAN_VALUE );
}

TracePropertyValue::operator unsigned char ( void ) const
{
    return (mValue.isEmpty() == false ? static_cast<unsigned char>(mValue.getAt(NEString::StartPos)) : static_cast<unsigned char>(NELogConfig::DEFAULT_INTEGER_VALUE));
}

TracePropertyValue::operator float ( void ) const
{
    return (mValue.isEmpty() == false ? mValue.convToFloat() : static_cast<float>(NELogConfig::DEFAULT_INTEGER_VALUE));
}


void TracePropertyValue::operator += ( NETrace::eLogPriority newValue )
{
    int prioInt     = static_cast<NETrace::eLogPriority>(*this);
    String prioStr= NETrace::convToString(newValue);
    if ( prioInt == NETrace::PrioNotset )
        _setValue( prioStr.getString() );
    else if (( newValue != NETrace::PrioNotset) && (prioInt & static_cast<int>(newValue)) == 0 )
        _setValue( (mValue + NELogConfig::SYNTAX_LOGICAL_OR + prioStr).getString());
}

bool TracePropertyValue::operator == ( const TracePropertyValue & other ) const
{
    return ( this != &other ? mValue == other.mValue : true );
}
