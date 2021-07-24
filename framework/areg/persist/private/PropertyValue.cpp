/************************************************************************
 * \file        areg/persist/private/PropertyValue.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Property Key object to persist application data.
 ************************************************************************/
#include "areg/persist/PropertyValue.hpp"
#include "areg/persist/private/NEPersistence.hpp"
#include "areg/base/NEUtilities.hpp"

PropertyValue::PropertyValue(void)
    : mValue    ( )
{
    ; // do nothing
}

PropertyValue::PropertyValue(const PropertyValue & source)
    : mValue    ( source.mValue )
{
    ; // do nothing
}

PropertyValue::PropertyValue( const char * strValue )
    : mValue    ( )
{
    parseValue(strValue);
}

PropertyValue::PropertyValue(unsigned int intValue)
    : mValue    ( String::uint32ToString(intValue, NEString::RadixDecimal) )
{
    ; // do nothing
}

PropertyValue::PropertyValue(double dValue)
    : mValue( String::doubleToString( dValue ) )
{
    ; // do nothing
}

PropertyValue::PropertyValue(const TEArrayList<Identifier, const Identifier &> idList)
    : mValue    ( )
{
    setIndentifier(idList);
}

PropertyValue::~PropertyValue(void)
{
    ; // do nothing
}

const PropertyValue & PropertyValue::operator = ( const PropertyValue & source )
{
    mValue  = source.mValue;
    return (*this);
}

const PropertyValue & PropertyValue::operator = ( const String & strValue )
{
    parseValue(strValue);
    return (*this);
}

const PropertyValue & PropertyValue::operator = (unsigned int intValue)
{
    mValue.convFromUInt32(intValue);
    return (*this);
}

const PropertyValue & PropertyValue::operator = (double dValue)
{
    mValue.convFromDouble(dValue);
    return (*this);
}

const PropertyValue & PropertyValue::operator = (const TEArrayList<Identifier, const Identifier &> idList)
{
    setIndentifier(idList);
    return (*this);
}

bool PropertyValue::operator == ( const PropertyValue & other ) const
{
    return (this != &other ? mValue == other.mValue : true );
}

bool PropertyValue::operator != ( const PropertyValue & other ) const
{
    return (this != &other ? mValue != other.mValue : false );
}

const String & PropertyValue::getValue(void) const
{
    return mValue;
}

void PropertyValue::setValue( const char * strValue )
{
    parseValue(strValue);
}

const char * PropertyValue::getString(void) const
{
    return mValue.getString();
}

unsigned int PropertyValue::getInteger( NEString::eRadix radix /*= NEString::RadixDecimal*/ ) const
{
    return mValue.convToUInt32( static_cast<NEString::eRadix>(radix) );
}

double PropertyValue::getDouble(void) const
{
    return mValue.convToDouble( );
}

unsigned int PropertyValue::getIndetifier( const TEArrayList<Identifier, const Identifier &> idList ) const
{
    static const char _or[2] = {NEPersistence::SYNTAX_LOGICAL_OR, '\0'};
    unsigned int result = Identifier::BAD_IDENTIFIER.getValue();
    if ( idList.isEmpty() == false )
    {
        result = 0;
        String temp = mValue;
        for ( int i = 0; (i < idList.getSize()) && (temp.isEmpty() == false); ++ i)
        {
            const char * idName = idList[i].getName();
            
            if ( temp.compare(idName, NEString::StartPos, NEString::CountAll, false) == 0 )
            {
                // found identifier
                result |= idList[i].getValue();
                temp = temp.substring(NEString::StartPos, NEString::getStringLength<char>(idName));
                if ( temp.isEmpty() == false )
                {
                    const char * next = NULL;
                    String::getSubstring(temp.getString(), _or, &next);
                    temp = next != NULL ? next : "";
                    temp.trimAll();
                    i = -1; // reset, to search next identifier value again or stop loop if temp is empty.
                }
            }
        }
    }
    return result;
}

void PropertyValue::setString(const char * strValue)
{
    parseValue( strValue );
}

void PropertyValue::setInteger(unsigned int intValue, NEString::eRadix radix /*= NEString::RadixDecimal*/ )
{
    mValue = String::uint32ToString(intValue, radix);
}

void PropertyValue::setDouble(double dValue)
{
    mValue = String::doubleToString( dValue );
}

void PropertyValue::setIndentifier(const TEArrayList<Identifier, const Identifier &> idList)
{
    mValue.clear();
    for ( int i = 0; i < idList.getSize(); ++ i )
    {
        if ( mValue.isEmpty() == false )
        {
            mValue += NEPersistence::SYNTAX_WHITESPACE_DELIMITER;
            mValue += NEPersistence::SYNTAX_LOGICAL_OR;
            mValue += NEPersistence::SYNTAX_WHITESPACE_DELIMITER;
        }
        mValue += idList[i].getName();
    }
}

void PropertyValue::parseValue(const char * strValue)
{
    mValue  = strValue;
    mValue.trimAll();
    int len = mValue.getLength();
    for ( ; mValue[len - 1] == NEPersistence::SYNTAX_END_COMMAND && len > 0; -- len)
        mValue = mValue.substring(0, len - 1);
}

void PropertyValue::resetValue(void)
{
    mValue.clear();
}

String PropertyValue::convToString(void) const
{
    String result (mValue);
    result += mValue.isEmpty() == false ? NEPersistence::SYNTAX_END_COMMAND : '\0';
    return result;
}
