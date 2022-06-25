/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/persist/private/PropertyValue.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       Property Key object to persist application data.
 ************************************************************************/
#include "areg/persist/PropertyValue.hpp"
#include "areg/persist/private/NEPersistence.hpp"
#include "areg/base/NEUtilities.hpp"

#include <utility>

PropertyValue::PropertyValue(const PropertyValue & source)
    : mValue    ( source.mValue )
{
}

PropertyValue::PropertyValue( PropertyValue && source ) noexcept
    : mValue    ( std::move( source.mValue ) )
{
}

PropertyValue::PropertyValue( const String & value )
    : mValue    ( )
{
    parseValue( static_cast<const String &>(value) );
}

PropertyValue::PropertyValue( const char * value )
    : mValue    ( )
{
    parseValue(value);
}

PropertyValue::PropertyValue(unsigned int intValue)
    : mValue    ( String::toString(intValue, NEString::eRadix::RadixDecimal) )
{
}

PropertyValue::PropertyValue(double dValue)
    : mValue( String::toString( dValue ) )
{
}

PropertyValue::PropertyValue(const TEArrayList<Identifier> idList)
    : mValue    ( )
{
    setIndentifier(idList);
}

PropertyValue & PropertyValue::operator = ( const PropertyValue & source )
{
    mValue  = source.mValue;
    return (*this);
}

PropertyValue & PropertyValue::operator = ( PropertyValue && source ) noexcept
{
    mValue  = std::move(source.mValue);
    return (*this);
}

PropertyValue & PropertyValue::operator = ( const String & value )
{
    parseValue(value);
    return (*this);
}

PropertyValue & PropertyValue::operator = (unsigned int intValue)
{
    mValue.fromUInt32(intValue);
    return (*this);
}

PropertyValue & PropertyValue::operator = (double dValue)
{
    mValue.fromDouble(dValue);
    return (*this);
}

PropertyValue & PropertyValue::operator = (const TEArrayList<Identifier> & idList)
{
    setIndentifier(idList);
    return (*this);
}

bool PropertyValue::operator == ( const PropertyValue & other ) const
{
    return (mValue == other.mValue);
}

bool PropertyValue::operator != ( const PropertyValue & other ) const
{
    return (mValue != other.mValue);
}

const String & PropertyValue::getValue(void) const
{
    return mValue;
}

void PropertyValue::setValue( const char * value )
{
    parseValue(value);
}

void PropertyValue::setValue( const String & value )
{
    parseValue( static_cast<const String &>(value) );
}

void PropertyValue::setValue( String && value )
{
    parseValue( static_cast<String &&>(value) );
}

const char * PropertyValue::getString(void) const
{
    return mValue.getString();
}

unsigned int PropertyValue::getInteger( NEString::eRadix radix /*= NEString::RadixDecimal*/ ) const
{
    return mValue.toUInt32( static_cast<NEString::eRadix>(radix) );
}

double PropertyValue::getDouble(void) const
{
    return mValue.toDouble( );
}

unsigned int PropertyValue::getIndetifier( const TEArrayList<Identifier> idList ) const
{
    static const char _or[2] = {NEPersistence::SYNTAX_LOGICAL_OR, '\0'};
    unsigned int result = Identifier::BAD_IDENTIFIER.getValue();
    if ( idList.isEmpty() == false )
    {
        result = 0;
        String temp = mValue;
        for ( uint32_t i = 0; (i < idList.getSize()) && (temp.isEmpty() == false); ++ i)
        {
            const char * idName = idList[i].getName();
            
            if ( temp.compare(idName, NEString::START_POS, NEString::COUNT_ALL, false) == NEMath::eCompare::Equal )
            {
                // found identifier
                result |= idList[i].getValue();
                temp.substring(NEString::START_POS, NEString::getStringLength<char>(idName));
                if ( temp.isEmpty() == false )
                {
                    const char * next = nullptr;
                    String::getSubstring(temp.getString(), _or, &next);
                    temp = next != nullptr ? next : "";
                    temp.trimAll();
                    i = 0; // reset, to search next identifier value again or stop loop if temp is empty.
                }
            }
        }
    }

    return result;
}

void PropertyValue::setString(const char * value)
{
    parseValue( value );
}

void PropertyValue::setInteger(unsigned int intValue, NEString::eRadix radix /*= NEString::RadixDecimal*/ )
{
    mValue = String::toString(intValue, radix);
}

void PropertyValue::setDouble(double dValue)
{
    mValue = String::toString( dValue );
}

void PropertyValue::setIndentifier(const TEArrayList<Identifier> idList)
{
    mValue.clear();
    for ( uint32_t i = 0; i < idList.getSize(); ++ i )
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

void PropertyValue::parseValue(const char * value)
{
    mValue  = value;
    mValue.trimAll();
    int len = mValue.getLength();
    for ( ; mValue[len - 1] == NEPersistence::SYNTAX_END_COMMAND && len > 0; -- len)
        mValue.substring(0, len - 1);
}

void PropertyValue::parseValue( const String & value )
{
    mValue  = value;
    mValue.trimAll( );
    int len = mValue.getLength( );
    for ( ; mValue[len - 1] == NEPersistence::SYNTAX_END_COMMAND && len > 0; -- len )
        mValue.substring( 0, len - 1 );
}

void PropertyValue::parseValue( String && value )
{
    mValue  = static_cast<String &&>(value);
    mValue.trimAll( );
    int len = mValue.getLength( );
    for ( ; mValue[len - 1] == NEPersistence::SYNTAX_END_COMMAND && len > 0; -- len )
        mValue.substring( 0, len - 1 );
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
