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
namespace areg {

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
    : mValue( String::make_string(intValue, areg::Radix::Decimal) )
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

uint32_t PropertyValue::identifier( const std::vector<Identifier> & idList ) const
{
    if (idList.empty() || mValue.is_empty())
        return Identifier::BAD_IDENTIFIER_VALUE;
    
    uint32_t result = Identifier::BAD_IDENTIFIER_VALUE;
    std::vector<StringBase<char>> list { mValue.split(areg::SYNTAX_VALUE_LIST_DELIMITER) };
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

    return result;
}

ArrayList<Identifier> PropertyValue::identifier_list(const std::vector<Identifier>& lookupList) const
{
    ArrayList<Identifier> result;
    if ((lookupList.empty() == false) && (mValue.is_empty() == false))
    {
        std::vector<StringBase<char>> list{ mValue.split(areg::SYNTAX_VALUE_LIST_DELIMITER) };
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
                mValue.append(areg::SYNTAX_WHITESPACE_DELIMITER)
                      .append(areg::SYNTAX_VALUE_LIST_DELIMITER)
                      .append(areg::SYNTAX_WHITESPACE_DELIMITER);
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
            mValue.append(areg::SYNTAX_WHITESPACE_DELIMITER)
                  .append(areg::SYNTAX_VALUE_LIST_DELIMITER)
                  .append(areg::SYNTAX_WHITESPACE_DELIMITER);
        }

        mValue += entry.name();
    }
}

ArrayList<String> PropertyValue::value_list(bool makeUnique /*= false*/) const
{
    ArrayList<String> result;
    if (mValue.is_empty() == false)
    {
        std::vector<StringBase<char>> list{ mValue.split(areg::SYNTAX_VALUE_LIST_DELIMITER) };
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
            mValue.append(areg::SYNTAX_WHITESPACE_DELIMITER)
                  .append(areg::SYNTAX_VALUE_LIST_DELIMITER)
                  .append(areg::SYNTAX_WHITESPACE_DELIMITER);
        }

        mValue += entry;
    }
}

String PropertyValue::to_string() const
{
    String result (mValue);
    if (mValue.is_empty() == false)
    {
        result += areg::SYNTAX_END_COMMAND;
    }

    return result;
}

void PropertyValue::_parse_value()
{
    mValue.trim_all();
    int32_t len = mValue.length();
    while ((len != 0) && (mValue[len - 1] == areg::SYNTAX_END_COMMAND))
    {
        len = mValue.resize(len - 1).length();
    }
}

} // namespace areg
