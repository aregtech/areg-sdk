/************************************************************************
 * \file        areg/persist/private/CEPropertyValue.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Property Key object to persist application data.
 ************************************************************************/
#include "areg/persist/CEPropertyValue.hpp"
#include "areg/persist/private/NEPersistence.hpp"
#include "areg/base/NEUtilities.hpp"

CEPropertyValue::CEPropertyValue(void)
    : mValue    ( )
{
    ; // do nothing
}

CEPropertyValue::CEPropertyValue(const CEPropertyValue & source)
    : mValue    ( source.mValue )
{
    ; // do nothing
}

CEPropertyValue::CEPropertyValue( const char * strValue )
    : mValue    ( )
{
    ParseValue(strValue);
}

CEPropertyValue::CEPropertyValue(unsigned int intValue)
    : mValue    ( CEString::UInt32ToString(intValue, NEString::RadixDecimal) )
{
    ; // do nothing
}

CEPropertyValue::CEPropertyValue(double dValue)
    : mValue( CEString::DoubleToString( dValue ) )
{
    ; // do nothing
}

CEPropertyValue::CEPropertyValue(const TEArrayList<CEIdentifier, const CEIdentifier &> idList)
    : mValue    ( )
{
    SetIndentifier(idList);
}

CEPropertyValue::~CEPropertyValue(void)
{
    ; // do nothing
}

const CEPropertyValue & CEPropertyValue::operator = ( const CEPropertyValue & source )
{
    mValue  = source.mValue;
    return (*this);
}

const CEPropertyValue & CEPropertyValue::operator = ( const CEString & strValue )
{
    ParseValue(strValue);
    return (*this);
}

const CEPropertyValue & CEPropertyValue::operator = (unsigned int intValue)
{
    mValue.FromUInt32(intValue);
    return (*this);
}

const CEPropertyValue & CEPropertyValue::operator = (double dValue)
{
    mValue.FromDouble(dValue);
    return (*this);
}

const CEPropertyValue & CEPropertyValue::operator = (const TEArrayList<CEIdentifier, const CEIdentifier &> idList)
{
    SetIndentifier(idList);
    return (*this);
}

bool CEPropertyValue::operator == ( const CEPropertyValue & other ) const
{
    return (this != &other ? mValue == other.mValue : true );
}

bool CEPropertyValue::operator != ( const CEPropertyValue & other ) const
{
    return (this != &other ? mValue != other.mValue : false );
}

const CEString & CEPropertyValue::GetValue(void) const
{
    return mValue;
}

void CEPropertyValue::SetValue( const char * strValue )
{
    ParseValue(strValue);
}

const char * CEPropertyValue::GetString(void) const
{
    return mValue.GetBuffer();
}

unsigned int CEPropertyValue::GetInteger( NEString::eRadix radix /*= NEString::RadixDecimal*/ ) const
{
    return mValue.ToUInt32( static_cast<NEString::eRadix>(radix) );
}

double CEPropertyValue::GetDouble(void) const
{
    return mValue.ToDouble( );
}

unsigned int CEPropertyValue::GetIndetifier( const TEArrayList<CEIdentifier, const CEIdentifier &> idList ) const
{
    static const char _or[2] = {NEPersistence::SYNTAX_LOGICAL_OR, '\0'};
    unsigned int result = CEIdentifier::BAD_IDENTIFIER.GetValue();
    if ( idList.IsEmpty() == false )
    {
        result = 0;
        CEString temp = mValue;
        for ( int i = 0; (i < idList.GetSize()) && (temp.IsEmpty() == false); ++ i)
        {
            const char * idName = idList[i].GetName();
            
            if ( temp.Compare(idName, NEString::StartPos, NEString::CountAll, false) == 0 )
            {
                // found identifier
                result |= idList[i].GetValue();
                temp = temp.Substr(NEString::StartPos, NEString::getStringLength<char>(idName));
                if ( temp.IsEmpty() == false )
                {
                    const char * next = NULL;
                    CEString::GetSubstring(temp.GetBuffer(), _or, &next);
                    temp = next != NULL ? next : "";
                    temp.TrimAll();
                    i = -1; // reset, to search next identifier value again or stop loop if temp is empty.
                }
            }
        }
    }
    return result;
}

void CEPropertyValue::SetString(const char * strValue)
{
    ParseValue( strValue );
}

void CEPropertyValue::SetInteger(unsigned int intValue, NEString::eRadix radix /*= NEString::RadixDecimal*/ )
{
    mValue = CEString::UInt32ToString(intValue, radix);
}

void CEPropertyValue::SetDouble(double dValue)
{
    mValue = CEString::DoubleToString( dValue );
}

void CEPropertyValue::SetIndentifier(const TEArrayList<CEIdentifier, const CEIdentifier &> idList)
{
    mValue.Clear();
    for ( int i = 0; i < idList.GetSize(); ++ i )
    {
        if ( mValue.IsEmpty() == false )
        {
            mValue += NEPersistence::SYNTAX_WHITESPACE_DELIMITER;
            mValue += NEPersistence::SYNTAX_LOGICAL_OR;
            mValue += NEPersistence::SYNTAX_WHITESPACE_DELIMITER;
        }
        mValue += idList[i].GetName();
    }
}

void CEPropertyValue::ParseValue(const char * strValue)
{
    mValue  = strValue;
    mValue.TrimAll();
    int len = mValue.GetLength();
    for ( ; mValue[len - 1] == NEPersistence::SYNTAX_END_COMMAND && len > 0; -- len)
        mValue = mValue.Substr(0, len - 1);
}

void CEPropertyValue::ResetValue(void)
{
    mValue.Clear();
}

CEString CEPropertyValue::ToString(void) const
{
    CEString result (mValue);
    result += mValue.IsEmpty() == false ? NEPersistence::SYNTAX_END_COMMAND : '\0';
    return result;
}
