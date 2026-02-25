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
#include "areg/persist/PersistenceDefs.hpp"
#include "areg/base/UtilityDefs.hpp"

#include <utility>

PropertyValue::PropertyValue(const PropertyValue & source)
    : mValue( source.mValue )
{
}

PropertyValue::PropertyValue( PropertyValue && source ) noexcept
    : mValue( std::move( source.mValue ) )
{
}

PropertyValue::PropertyValue( const areg::String & value )
    : mValue( )
{
    parseValue( static_cast<const areg::String &>(value) );
}

PropertyValue::PropertyValue(areg::String&& value) noexcept
    : mValue()
{
    parseValue(std::move(value));
}

PropertyValue::PropertyValue( const char * value )
    : mValue( )
{
    parseValue(value);
}

PropertyValue::PropertyValue(uint32_t intValue)
    : mValue( areg::String::makeString(intValue, areg::Radix::Decimal) )
{
}

PropertyValue::PropertyValue(double dValue)
    : mValue( areg::String::makeString( dValue ) )
{
}

PropertyValue::PropertyValue(bool bValue)
    : mValue(areg::String::makeString(bValue))
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

PropertyValue & PropertyValue::operator = ( const areg::String & value )
{
    parseValue(value);
    return (*this);
}

PropertyValue& PropertyValue::operator = (areg::String && value) noexcept
{
    parseValue(std::move(value));
    return (*this);
}

PropertyValue & PropertyValue::operator = (uint32_t intValue)
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

PropertyValue::operator uint32_t() const
{
    return getInteger(areg::Radix::Decimal);
}

PropertyValue::operator const areg::String& () const
{
    return getString();
}

PropertyValue::operator double() const
{
    return getDouble();
}

PropertyValue::operator bool() const
{
    return getBoolean();
}

const areg::String & PropertyValue::getValue() const
{
    return mValue;
}

void PropertyValue::setValue( const char * value )
{
    parseValue(value);
}

void PropertyValue::setValue( const areg::String & value )
{
    parseValue( static_cast<const areg::String &>(value) );
}

void PropertyValue::setValue( areg::String && value )
{
    parseValue( std::move(value) );
}

const areg::String & PropertyValue::getString() const
{
    return mValue;
}

uint32_t PropertyValue::getInteger( areg::Radix radix /*= areg::Decimal*/ ) const
{
    return mValue.toUInt32( static_cast<areg::Radix>(radix) );
}

double PropertyValue::getDouble() const
{
    return mValue.toDouble( );
}

uint32_t PropertyValue::getIndetifier( const std::vector<Identifier> & idList ) const
{
    uint32_t result = Identifier::BAD_IDENTIFIER_VALUE;
    if ( (idList.empty() == false) && (mValue.isEmpty() == false) )
    {
        std::vector<StringBase<char>> list { mValue.split(areg::SYNTAX_VALUE_LIST_DELIMITER) };
        for (auto& entry : list)
        {
            areg::String value{ entry.trimAll() };
            for (const auto& id : idList)
            {
                const areg::String& idName = id.getName();
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

bool PropertyValue::getBoolean() const
{
    return mValue.toBool();
}

void PropertyValue::setBoolean(bool newValue)
{
    mValue = areg::String::makeString(newValue);
}

void PropertyValue::setInteger(uint32_t intValue, areg::Radix radix /*= areg::Decimal*/ )
{
    mValue = areg::String::makeString(intValue, radix);
}

void PropertyValue::setDouble(double dValue)
{
    mValue = areg::String::makeString( dValue );
}

ArrayList<Identifier> PropertyValue::getIdentifierList(const std::vector<Identifier>& lookupList) const
{
    ArrayList<Identifier> result;
    if ((lookupList.empty() == false) && (mValue.isEmpty() == false))
    {
        std::vector<StringBase<char>> list{ mValue.split(areg::SYNTAX_VALUE_LIST_DELIMITER) };
        for (auto& entry : list)
        {
            areg::String value{ entry.trimAll() };
            for (const auto& id : lookupList)
            {
                const areg::String& idName = id.getName();
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

void PropertyValue::setIdentifierList(uint32_t idBits, const std::vector<Identifier>& lookupList)
{
    mValue.clear();
    for (const auto& entry : lookupList)
    {
        if ((idBits & entry.getValue()) != 0)
        {
            if (mValue.isEmpty() == false)
            {
                mValue.append(areg::SYNTAX_WHITESPACE_DELIMITER)
                      .append(areg::SYNTAX_VALUE_LIST_DELIMITER)
                      .append(areg::SYNTAX_WHITESPACE_DELIMITER);
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
            mValue.append(areg::SYNTAX_WHITESPACE_DELIMITER)
                  .append(areg::SYNTAX_VALUE_LIST_DELIMITER)
                  .append(areg::SYNTAX_WHITESPACE_DELIMITER);
        }

        mValue += entry.getName();
    }
}

ArrayList<areg::String> PropertyValue::getValueList(bool makeUnique /*= false*/) const
{
    ArrayList<areg::String> result;
    if (mValue.isEmpty() == false)
    {
        std::vector<StringBase<char>> list{ mValue.split(areg::SYNTAX_VALUE_LIST_DELIMITER) };
        for (auto& entry : list)
        {
            areg::String value{ entry.trimAll() };
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

void PropertyValue::setValueList(const std::vector<areg::String>& list)
{
    mValue.clear();
    for (const auto& entry : list)
    {
        if (mValue.isEmpty() == false)
        {
            mValue.append(areg::SYNTAX_WHITESPACE_DELIMITER)
                  .append(areg::SYNTAX_VALUE_LIST_DELIMITER)
                  .append(areg::SYNTAX_WHITESPACE_DELIMITER);
        }

        mValue += entry;
    }
}

void PropertyValue::parseValue(const char * value)
{
    mValue  = value;
    _parseValue();
}

void PropertyValue::parseValue( const areg::String & value )
{
    mValue  = value;
    _parseValue();
}

void PropertyValue::parseValue( areg::String && value )
{
    mValue  = std::move(value);
    _parseValue();
}

void PropertyValue::resetValue()
{
    mValue.clear();
}

areg::String PropertyValue::convToString() const
{
    areg::String result (mValue);
    if (mValue.isEmpty() == false)
    {
        result += areg::SYNTAX_END_COMMAND;
    }

    return result;
}

inline void PropertyValue::_parseValue()
{
    mValue.trimAll();
    int32_t len = mValue.getLength();
    while ((len != 0) && (mValue[len - 1] == areg::SYNTAX_END_COMMAND))
    {
        len = mValue.resize(len - 1).getLength();
    }
}
