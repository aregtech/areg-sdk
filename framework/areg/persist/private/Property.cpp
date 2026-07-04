/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/persist/private/Property.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Property object for persisting data
 ************************************************************************/
#include "areg/persist/Property.hpp"
#include "areg/persist/PersistenceDefs.hpp"
#include "areg/base/UtilityDefs.hpp"

#include <utility>
namespace areg {

Property::Property()
    : mIsTemporary  (true)
    , mComment      (String::EmptyString)
    , mProperty     ( )
{
}

Property::Property(const String strProperty)
    : mIsTemporary  (false)
    , mComment      (String::EmptyString)
    , mProperty     ()
{
    parse(strProperty);
}

Property::Property(const PropertyKey& key, const PropertyValue& value, const String& comment /*= String::EmptyString*/, bool isTemporary /* = false*/)
    : mIsTemporary  (isTemporary)
    , mComment      (comment)
    , mProperty     (key, value)
{
}

Property::Property(PropertyKey&& key, PropertyValue&& value, String&& comment)
    : mIsTemporary  (false)
    , mComment      ( std::move(comment))
    , mProperty     ( std::move(key), std::move(value))
{
}

Property::Property(PropertyKey&& key, PropertyValue&& value)
    : mIsTemporary  (false)
    , mComment      ( )
    , mProperty     ( std::move(key), std::move(value))
{
}

Property::Property(const String & keySet, const String & valueSet, const String & comment /*= String::EmptyString*/, bool isTemporary /*= false*/)
    : mIsTemporary  ( isTemporary )
    , mComment      ( comment )
    , mProperty     ( PropertyKey(keySet), PropertyValue(valueSet) )
{
}

Property::Property(const char* keySet, const char* valueSet, const char* comment /*= nullptr*/, bool isTemporary /* = false*/)
    : mIsTemporary  ( isTemporary )
    , mComment      ( comment )
    , mProperty     ( PropertyKey(keySet), PropertyValue(valueSet) )
{
}

Property::Property(std::string_view keySet, std::string_view valueSet, std::string_view comment /*= String::EmptyString*/, bool isTemporary /* = false*/)
    : mIsTemporary  ( isTemporary )
    , mComment      ( comment )
    , mProperty     ( PropertyKey(keySet), PropertyValue(valueSet) )
{
}

Property::Property( std::string_view section
                  , std::string_view module
                  , std::string_view property
                  , std::string_view position
                  , std::string_view value
                  , std::string_view comment /*= String::EmptyString*/
                  , bool isTemporary /* = false*/)
    : mIsTemporary  ( isTemporary )
    , mComment      ( comment )
    , mProperty     ( PropertyKey(section, module, property, position), PropertyValue(value) )
{
}

Property::Property(const Property::Entry & newProperty, const String & comment /*= String::empty_string*/, bool isTemporary /* = false*/)
    : mIsTemporary  ( isTemporary )
    , mComment      ( comment )
    , mProperty     ( newProperty )
{
}

Property::Property(const Property & source)
    : mIsTemporary  (source.mIsTemporary)
    , mComment      (source.mComment)
    , mProperty     (source.mProperty)
{
}

Property::Property( Property && source ) noexcept
    : mIsTemporary  (source.mIsTemporary)
    , mComment      (std::move(source.mComment))
    , mProperty     (std::move(source.mProperty))
{
}

Property & Property::operator = ( const Property & source )
{
    mIsTemporary = source.mIsTemporary;
    mComment    = source.mComment;
    mProperty   = source.mProperty;

    return (*this);
}

Property & Property::operator = ( Property && source ) noexcept
{
    mIsTemporary = source.mIsTemporary;
    mComment    = std::move(source.mComment);
    mProperty   = std::move(source.mProperty);

    return (*this);
}

void Property::add_comment(const String & comment)
{
    if (comment.starts_with(areg::SYNTAX_COMMENT))
    {
        mComment += comment;
    }
    else
    {
        // if does not begin with "# "
        mComment += areg::SYNTAX_COMMENT;
        mComment += comment;
    }
}

bool Property::parse(const String & strProperties)
{
    bool result{ false };
    String data;
    if ( strProperties.length() > 1)
    {
        areg::CharPos pos = strProperties.find_first(areg::SYNTAX_COMMENT.data());

        if (strProperties.is_valid_position(pos))
        {
            if (pos != areg::START_POS)
            {
                add_comment(strProperties.buffer(pos));
                strProperties.substring(data, 0, pos);
            }
        }
        else if (pos == areg::END_POS)
        {
            data = strProperties;
        }

        if (data.is_empty() == false )
        {
            const char* value{ nullptr };
            const String key{ String::substr(data.as_string(), areg::SYNTAX_EQUAL.data(), &value) };

            mProperty.mValue.first.parse_key(key);
            mProperty.mValue.second.parse_value(value);

            if (is_valid())
            {
                mIsTemporary = false;
                result = true;
            }
            else
            {
                mProperty.mValue.first.reset();
                mProperty.mValue.second.reset();
            }
        }
    }

    return result;
}

String Property::to_string() const
{
    String result;
    String key  (mProperty.mValue.first.to_string());
    String value(mProperty.mValue.second.to_string());

    if ( !key.is_empty() && !value.is_empty() )
    {
        key.append(areg::SYNTAX_WHITESPACE_DELIMITER)
           .append(areg::SYNTAX_EQUAL)
           .append(areg::SYNTAX_WHITESPACE_DELIMITER)
           .append(value);
    }

    if ( mComment.is_empty() == false )
    {
        if (mComment.is_valid_position(mComment.find_first(areg::SYNTAX_LINEEND.data())) || (mComment.length() >= 64))
        {
            result.append(mComment).append(areg::SYNTAX_LINEEND).append(key);
        }
        else
        {
            result.append(key).append(areg::SYNTAX_WHITESPACE_DELIMITER).append(mComment);
        }
    }
    else
    {
        result = key;
    }

    return result;
}

} // namespace areg
