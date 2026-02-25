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

PropertyValue::PropertyValue( const String & value )
    : mValue( )
{
    parse_value( static_cast<const String &>(value) );
}

PropertyValue::PropertyValue(String&& value) noexcept
    : mValue()
{
    parse_value(std::move(value));
}

PropertyValue::PropertyValue( const char * value )
    : mValue( )
{
    parse_value(value);
}

PropertyValue::PropertyValue(uint32_t intValue)
    : mValue( String::make_string(intValue, NEString::Radix::Decimal) )
{
}

PropertyValue::PropertyValue(double dValue)
    : mValue( String::make_string( dValue ) )
{
}

PropertyValue::PropertyValue(bool bValue)
    : mValue(String::make_string(bValue))
{
}

PropertyValue::PropertyValue(const std::vector<Identifier> & idList)
    : mValue( )
{
    set_identifier(idList);
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
    parse_value(value);
    return (*this);
}

PropertyValue& PropertyValue::operator = (String && value) noexcept
{
    parse_value(std::move(value));
    return (*this);
}

PropertyValue & PropertyValue::operator = (uint32_t intValue)
{
    mValue.from_uint32(intValue);
    return (*this);
}

PropertyValue & PropertyValue::operator = (double dValue)
{
    mValue.from_double(dValue);
    return (*this);
}

PropertyValue& PropertyValue::operator = (bool bValue)
{
    mValue.from_bool(bValue);
    return (*this);
}

PropertyValue & PropertyValue::operator = (const std::vector<Identifier> & idList)
{
    set_identifier(idList);
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
    return as_integer(NEString::Radix::Decimal);
}

PropertyValue::operator const String& () const
{
    return as_string();
}

PropertyValue::operator double() const
{
    return as_double();
}

PropertyValue::operator bool() const
{
    return as_boolean();
}

const String & PropertyValue::value() const
{
    return mValue;
}

void PropertyValue::set_value( const char * value )
{
    parse_value(value);
}

void PropertyValue::set_value( const String & value )
{
    parse_value( static_cast<const String &>(value) );
}

void PropertyValue::set_value( String && value )
{
    parse_value( std::move(value) );
}

const String & PropertyValue::as_string() const
{
    return mValue;
}

uint32_t PropertyValue::as_integer( NEString::Radix radix /*= NEString::Decimal*/ ) const
{
    return mValue.to_uint32( static_cast<NEString::Radix>(radix) );
}

double PropertyValue::as_double() const
{
    return mValue.to_double( );
}

uint32_t PropertyValue::identifier( const std::vector<Identifier> & idList ) const
{
    uint32_t result = Identifier::BAD_IDENTIFIER_VALUE;
    if ( (idList.empty() == false) && (mValue.is_empty() == false) )
    {
        std::vector<StringBase<char>> list { mValue.split(NEPersistence::SYNTAX_VALUE_LIST_DELIMITER) };
        for (auto& entry : list)
        {
            String value{ entry.trim_all() };
            for (const auto& id : idList)
            {
                const String& idName = id.name();
                if (value == idName)
                {
                    if (result == Identifier::BAD_IDENTIFIER_VALUE)
                    {
                        result = id.value();
                    }
                    else
                    {
                        result |= id.value();
                    }
                }
            }
        }
    }

    return result;
}

void PropertyValue::set_string(const char * value)
{
    parse_value( value );
}

bool PropertyValue::as_boolean() const
{
    return mValue.to_bool();
}

void PropertyValue::set_boolean(bool newValue)
{
    mValue = String::make_string(newValue);
}

void PropertyValue::set_integer(uint32_t intValue, NEString::Radix radix /*= NEString::Decimal*/ )
{
    mValue = String::make_string(intValue, radix);
}

void PropertyValue::set_double(double dValue)
{
    mValue = String::make_string( dValue );
}

ArrayList<Identifier> PropertyValue::identifier_list(const std::vector<Identifier>& lookupList) const
{
    ArrayList<Identifier> result;
    if ((lookupList.empty() == false) && (mValue.is_empty() == false))
    {
        std::vector<StringBase<char>> list{ mValue.split(NEPersistence::SYNTAX_VALUE_LIST_DELIMITER) };
        for (auto& entry : list)
        {
            String value{ entry.trim_all() };
            for (const auto& id : lookupList)
            {
                const String& idName = id.name();
                if (value == idName)
                {
                    result.add_if_unique(id);
                    break; // found and added, break the loop
                }
            }
        }
    }

    return result;
}

void PropertyValue::set_identifier_list(uint32_t idBits, const std::vector<Identifier>& lookupList)
{
    mValue.clear();
    for (const auto& entry : lookupList)
    {
        if ((idBits & entry.value()) != 0)
        {
            if (mValue.is_empty() == false)
            {
                mValue.append(NEPersistence::SYNTAX_WHITESPACE_DELIMITER)
                      .append(NEPersistence::SYNTAX_VALUE_LIST_DELIMITER)
                      .append(NEPersistence::SYNTAX_WHITESPACE_DELIMITER);
            }

            mValue += entry.name();
        }
    }
}

void PropertyValue::set_identifier(const std::vector<Identifier> & idList)
{
    mValue.clear();
    for ( const auto& entry : idList )
    {
        if ( mValue.is_empty() == false )
        {
            mValue.append(NEPersistence::SYNTAX_WHITESPACE_DELIMITER)
                  .append(NEPersistence::SYNTAX_VALUE_LIST_DELIMITER)
                  .append(NEPersistence::SYNTAX_WHITESPACE_DELIMITER);
        }

        mValue += entry.name();
    }
}

ArrayList<String> PropertyValue::value_list(bool makeUnique /*= false*/) const
{
    ArrayList<String> result;
    if (mValue.is_empty() == false)
    {
        std::vector<StringBase<char>> list{ mValue.split(NEPersistence::SYNTAX_VALUE_LIST_DELIMITER) };
        for (auto& entry : list)
        {
            String value{ entry.trim_all() };
            if (makeUnique)
            {
                result.add_if_unique(value);
            }
            else
            {
                result.add(value);
            }
        }
    }

    return result;
}

void PropertyValue::set_value_list(const std::vector<String>& list)
{
    mValue.clear();
    for (const auto& entry : list)
    {
        if (mValue.is_empty() == false)
        {
            mValue.append(NEPersistence::SYNTAX_WHITESPACE_DELIMITER)
                  .append(NEPersistence::SYNTAX_VALUE_LIST_DELIMITER)
                  .append(NEPersistence::SYNTAX_WHITESPACE_DELIMITER);
        }

        mValue += entry;
    }
}

void PropertyValue::parse_value(const char * value)
{
    mValue  = value;
    _parse_value();
}

void PropertyValue::parse_value( const String & value )
{
    mValue  = value;
    _parse_value();
}

void PropertyValue::parse_value( String && value )
{
    mValue  = std::move(value);
    _parse_value();
}

void PropertyValue::reset()
{
    mValue.clear();
}

String PropertyValue::to_string() const
{
    String result (mValue);
    if (mValue.is_empty() == false)
    {
        result += NEPersistence::SYNTAX_END_COMMAND;
    }

    return result;
}

inline void PropertyValue::_parse_value()
{
    mValue.trim_all();
    int32_t len = mValue.length();
    while ((len != 0) && (mValue[len - 1] == NEPersistence::SYNTAX_END_COMMAND))
    {
        len = mValue.resize(len - 1).length();
    }
}
