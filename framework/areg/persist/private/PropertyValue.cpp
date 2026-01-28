/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/persist/private/PropertyValue.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Property Key object to persist application data.
 ************************************************************************/
#include "areg/persist/PropertyValue.hpp"
#include "areg/persist/NEPersistence.hpp"
#include "areg/base/NEUtilities.hpp"

#include <utility>

PropertyValue::PropertyValue(const PropertyValue & source)
    : mValue( source.mValue )
{
}

PropertyValue::PropertyValue( PropertyValue && source ) noexcept
    : mValue( std::move( source.mValue ) )
{
}

PropertyValue::PropertyValue( const String & value )
    : mValue( )
{
    parseValue( static_cast<const String &>(value) );
}

PropertyValue::PropertyValue(String&& value) noexcept
    : mValue()
{
    parseValue(std::move(value));
}

PropertyValue::PropertyValue( const char * value )
    : mValue( )
{
    parseValue(value);
}

PropertyValue::PropertyValue(unsigned int intValue)
    : mValue( String::makeString(intValue, NEString::eRadix::RadixDecimal) )
{
}

PropertyValue::PropertyValue(double dValue)
    : mValue( String::makeString( dValue ) )
{
}

PropertyValue::PropertyValue(bool bValue)
    : mValue(String::makeString(bValue))
{
}

PropertyValue::PropertyValue(const std::vector<Identifier> & idList)
    : mValue( )
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

PropertyValue& PropertyValue::operator = (String && value) noexcept
{
    parseValue(std::move(value));
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

PropertyValue& PropertyValue::operator = (bool bValue)
{
    mValue.fromBool(bValue);
    return (*this);
}

PropertyValue & PropertyValue::operator = (const std::vector<Identifier> & idList)
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

PropertyValue::operator unsigned int(void) const
{
    return getInteger(NEString::eRadix::RadixDecimal);
}

PropertyValue::operator const String& (void) const
{
    return getString();
}

PropertyValue::operator double(void) const
{
    return getDouble();
}

PropertyValue::operator bool(void) const
{
    return getBoolean();
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
    parseValue( std::move(value) );
}

const String & PropertyValue::getString(void) const
{
    return mValue;
}

unsigned int PropertyValue::getInteger( NEString::eRadix radix /*= NEString::RadixDecimal*/ ) const
{
    return mValue.toUInt32( static_cast<NEString::eRadix>(radix) );
}

double PropertyValue::getDouble(void) const
{
    return mValue.toDouble( );
}

unsigned int PropertyValue::getIndetifier( const std::vector<Identifier> & idList ) const
{
    unsigned int result = Identifier::BAD_IDENTIFIER_VALUE;
    if ( (idList.empty() == false) && (mValue.isEmpty() == false) )
    {
        std::vector<TEString<char>> list { mValue.split(NEPersistence::SYNTAX_VALUE_LIST_DELIMITER) };
        for (auto& entry : list)
        {
            String value{ entry.trimAll() };
            for (const auto& id : idList)
            {
                const String& idName = id.getName();
                if (value == idName)
                {
                    if (result == Identifier::BAD_IDENTIFIER_VALUE)
                    {
                        result = id.getValue();
                    }
                    else
                    {
                        result |= id.getValue();
                    }
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

bool PropertyValue::getBoolean(void) const
{
    return mValue.toBool();
}

void PropertyValue::setBoolean(bool newValue)
{
    mValue = String::makeString(newValue);
}

void PropertyValue::setInteger(unsigned int intValue, NEString::eRadix radix /*= NEString::RadixDecimal*/ )
{
    mValue = String::makeString(intValue, radix);
}

void PropertyValue::setDouble(double dValue)
{
    mValue = String::makeString( dValue );
}

TEArrayList<Identifier> PropertyValue::getIdentifierList(const std::vector<Identifier>& lookupList) const
{
    TEArrayList<Identifier> result;
    if ((lookupList.empty() == false) && (mValue.isEmpty() == false))
    {
        std::vector<TEString<char>> list{ mValue.split(NEPersistence::SYNTAX_VALUE_LIST_DELIMITER) };
        for (auto& entry : list)
        {
            String value{ entry.trimAll() };
            for (const auto& id : lookupList)
            {
                const String& idName = id.getName();
                if (value == idName)
                {
                    result.addIfUnique(id);
                    break; // found and added, break the loop
                }
            }
        }
    }

    return result;
}

void PropertyValue::setIdentifierList(unsigned int idBits, const std::vector<Identifier>& lookupList)
{
    mValue.clear();
    for (const auto& entry : lookupList)
    {
        if ((idBits & entry.getValue()) != 0)
        {
            if (mValue.isEmpty() == false)
            {
                mValue.append(NEPersistence::SYNTAX_WHITESPACE_DELIMITER)
                      .append(NEPersistence::SYNTAX_VALUE_LIST_DELIMITER)
                      .append(NEPersistence::SYNTAX_WHITESPACE_DELIMITER);
            }

            mValue += entry.getName();
        }
    }
}

void PropertyValue::setIndentifier(const std::vector<Identifier> & idList)
{
    mValue.clear();
    for ( const auto& entry : idList )
    {
        if ( mValue.isEmpty() == false )
        {
            mValue.append(NEPersistence::SYNTAX_WHITESPACE_DELIMITER)
                  .append(NEPersistence::SYNTAX_VALUE_LIST_DELIMITER)
                  .append(NEPersistence::SYNTAX_WHITESPACE_DELIMITER);
        }

        mValue += entry.getName();
    }
}

TEArrayList<String> PropertyValue::getValueList(bool makeUnique /*= false*/) const
{
    TEArrayList<String> result;
    if (mValue.isEmpty() == false)
    {
        std::vector<TEString<char>> list{ mValue.split(NEPersistence::SYNTAX_VALUE_LIST_DELIMITER) };
        for (auto& entry : list)
        {
            String value{ entry.trimAll() };
            if (makeUnique)
            {
                result.addIfUnique(value);
            }
            else
            {
                result.add(value);
            }
        }
    }

    return result;
}

void PropertyValue::setValueList(const std::vector<String>& list)
{
    mValue.clear();
    for (const auto& entry : list)
    {
        if (mValue.isEmpty() == false)
        {
            mValue.append(NEPersistence::SYNTAX_WHITESPACE_DELIMITER)
                  .append(NEPersistence::SYNTAX_VALUE_LIST_DELIMITER)
                  .append(NEPersistence::SYNTAX_WHITESPACE_DELIMITER);
        }

        mValue += entry;
    }
}

void PropertyValue::parseValue(const char * value)
{
    mValue  = value;
    _parseValue();
}

void PropertyValue::parseValue( const String & value )
{
    mValue  = value;
    _parseValue();
}

void PropertyValue::parseValue( String && value )
{
    mValue  = std::move(value);
    _parseValue();
}

void PropertyValue::resetValue(void)
{
    mValue.clear();
}

String PropertyValue::convToString(void) const
{
    String result (mValue);
    if (mValue.isEmpty() == false)
    {
        result += NEPersistence::SYNTAX_END_COMMAND;
    }

    return result;
}

inline void PropertyValue::_parseValue(void)
{
    mValue.trimAll();
    int32_t len = mValue.getLength();
    while ((len != 0) && (mValue[len - 1] == NEPersistence::SYNTAX_END_COMMAND))
    {
        len = mValue.resize(len - 1).getLength();
    }
}
