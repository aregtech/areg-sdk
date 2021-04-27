/************************************************************************
 * \file        areg/trace/private/CETracePropertyValue.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Trace Property value class. Used when reading or 
 *              saving configuration file
 ************************************************************************/
#include "areg/trace/private/CETracePropertyValue.hpp"

#include "areg/trace/private/NELogConfig.hpp"
#include "areg/base/NEUtilities.hpp"

inline void CETracePropertyValue::_setValue( const char * newValue )
{
    mValue = newValue != NULL ? newValue : "";
    NEString::CharPos pos = mValue.FindFirstOf(NELogConfig::SYNTAX_END_COMMAND_DELIMITER);
    if ( pos != NEString::InvalidPos )
        mValue  = mValue.Substr(0, pos);
    mValue.TrimAll();

    CEString prio   = mValue;
    mPriority       = static_cast<unsigned int>(NETrace::PrioNotset);
    pos             = mValue.FindFirstOf(NELogConfig::SYNTAX_LOGICAL_OR);
    if ( pos != NEString::InvalidPos )
    {
        prio = mValue.Substr(0, pos);
        prio.TrimAll();
        mPriority |= static_cast<unsigned int>( NETrace::FromString(prio.GetBuffer()));

        prio = mValue.Substr(pos + 1);
        prio.TrimAll();
        mPriority |= static_cast<unsigned int>( NETrace::FromString(prio.GetBuffer()));
    }
    else
    {
        mPriority |= static_cast<unsigned int>( NETrace::FromString(prio.GetBuffer()));
    }
}


CETracePropertyValue::CETracePropertyValue(void)
    : mValue    ( "" )
    , mPriority ( static_cast<unsigned int>(NETrace::PrioNotset) )
{
    ; // do nothing
}

CETracePropertyValue::CETracePropertyValue( const char * initValue )
    : mValue    ( )
    , mPriority ( static_cast<unsigned int>(NETrace::PrioNotset) )
{
    _setValue(initValue);
}

CETracePropertyValue::CETracePropertyValue( const CEString & initValue )
    : mValue( )
    , mPriority( static_cast<unsigned int>(NETrace::PrioNotset) )
{
    _setValue( initValue.GetBuffer() );
}

CETracePropertyValue::CETracePropertyValue( const CETracePropertyValue & source )
    : mValue    ( source.mValue )
    , mPriority ( source.mPriority )
{
    ; // do nothing
}

CETracePropertyValue::~CETracePropertyValue(void)
{
    ; // do nothing
}

const CETracePropertyValue & CETracePropertyValue::operator = ( const CETracePropertyValue & source )
{
    if ( static_cast<const CETracePropertyValue *>(this) != &source )
        mValue  = source.mValue;
    return (*this);
}

void CETracePropertyValue::operator = ( const CEString & newValue )
{
    _setValue(newValue.GetBuffer());
}

void CETracePropertyValue::operator = ( const char * newValue )
{
    _setValue(newValue);
}

void CETracePropertyValue::operator = ( unsigned int newValue )
{
    mValue = CEString::UInt32ToString(newValue, NEString::RadixDecimal);
}

void CETracePropertyValue::operator = ( NETrace::eLogPriority newValue )
{
    mValue  = NETrace::ToString(newValue);
}

void CETracePropertyValue::operator = ( bool newValue )
{
    mValue = newValue ? NELogConfig::SYNTAX_TRUE : NELogConfig::SYNTAX_FALSE;
}

CETracePropertyValue::operator const char * ( void ) const
{
    return static_cast<const char *>(mValue);
}

CETracePropertyValue::operator const CEString & ( void ) const
{
    return mValue;
}

CETracePropertyValue::operator unsigned int ( void ) const
{
    return static_cast<unsigned int>( mValue.IsEmpty() == false ? mValue.ToUInt32(NEString::RadixDecimal) : NELogConfig::DEFAULT_INTEGER_VALUE );
}

CETracePropertyValue::operator NETrace::eLogPriority ( void ) const
{
    return static_cast<NETrace::eLogPriority>(mPriority);
}

CETracePropertyValue::operator bool ( void ) const
{
    return ( mValue.IsEmpty() == false ? mValue.Compare(NELogConfig::SYNTAX_TRUE, NEString::StartPos, NEString::EndPos, false) == 0 : NELogConfig::DEFAULT_BOOLEAN_VALUE );
}

CETracePropertyValue::operator unsigned char ( void ) const
{
    return (mValue.IsEmpty() == false ? static_cast<unsigned char>(mValue.GetCharAt(NEString::StartPos)) : static_cast<unsigned char>(NELogConfig::DEFAULT_INTEGER_VALUE));
}

CETracePropertyValue::operator float ( void ) const
{
    return (mValue.IsEmpty() == false ? mValue.ToFloat() : static_cast<float>(NELogConfig::DEFAULT_INTEGER_VALUE));
}


void CETracePropertyValue::operator += ( NETrace::eLogPriority newValue )
{
    int prioInt     = static_cast<NETrace::eLogPriority>(*this);
    CEString prioStr= NETrace::ToString(newValue);
    if ( prioInt == NETrace::PrioNotset )
        _setValue( prioStr.GetBuffer() );
    else if (( newValue != NETrace::PrioNotset) && (prioInt & static_cast<int>(newValue)) == 0 )
        _setValue( (mValue + NELogConfig::SYNTAX_LOGICAL_OR + prioStr).GetBuffer());
}

bool CETracePropertyValue::operator == ( const CETracePropertyValue & other ) const
{
    return ( this != &other ? mValue == other.mValue : true );
}
