/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/persist/private/Property.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Property object for persisting data
 ************************************************************************/
#include "areg/persist/Property.hpp"
#include "areg/persist/PersistenceDefs.hpp"
#include "areg/base/UtilityDefs.hpp"

#include <utility>

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

Property::Property(const PropertyKey& key, const PropertyValue& value, const String& comment /*= String::EmptyString*/, bool is_temporary /* = false*/)
    : mIsTemporary  (is_temporary)
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

Property::Property(const String & keySet, const String & valueSet, const String & comment /*= String::EmptyString*/, bool is_temporary /*= false*/)
    : mIsTemporary  ( is_temporary )
    , mComment      ( comment )
    , mProperty     ( PropertyKey(keySet), PropertyValue(valueSet) )
{
}

Property::Property(const char* keySet, const char* valueSet, const char* comment /*= nullptr*/, bool is_temporary /* = false*/)
    : mIsTemporary  ( is_temporary )
    , mComment      ( comment )
    , mProperty     ( PropertyKey(keySet), PropertyValue(valueSet) )
{
}

Property::Property(const std::string_view& keySet, const std::string_view& valueSet, const std::string_view& comment /*= String::EmptyString*/, bool is_temporary /* = false*/)
    : mIsTemporary  ( is_temporary )
    , mComment      ( comment )
    , mProperty     ( PropertyKey(keySet), PropertyValue(valueSet) )
{
}

Property::Property( const std::string_view& section
                  , const std::string_view& module
                  , const std::string_view& property
                  , const std::string_view& position
                  , const std::string_view& value
                  , const std::string_view& comment /*= String::EmptyString*/
                  , bool is_temporary /* = false*/)
    : mIsTemporary  ( is_temporary )
    , mComment      ( comment )
    , mProperty     ( PropertyKey(section, module, property, position), PropertyValue(value) )
{
}

Property::Property(const Property::Entry & newProperty, const String & comment /*= String::getEmptyString*/, bool is_temporary /* = false*/)
    : mIsTemporary  ( is_temporary )
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

bool Property::operator == ( const Property & other ) const
{
    return (this != &other ? mProperty == other.mProperty : true);
}

bool Property::operator != (const Property & other) const
{
    return (this != &other ? mProperty != other.mProperty : false);
}

Property::operator uint32_t () const
{
    return static_cast<uint32_t>(mProperty.mValue.first);
}

void Property::parse_key(const String & keySet)
{
    mProperty.mValue.first.parse_key(keySet);
}

void Property::set_key(const PropertyKey & Key)
{
    mProperty.mValue.first = Key;
}

void Property::set_key( PropertyKey && Key )
{
    mProperty.mValue.first = std::move(Key);
}

const PropertyKey & Property::key() const
{
    return mProperty.mValue.first;
}

NEPersistence::ConfigEntry Property::type() const
{
    return mProperty.mValue.first.key_type();
}

String Property::key_string() const
{
    return mProperty.mValue.first.to_string();
}

void Property::parse_value(const String & valueSet)
{
    mProperty.mValue.second.parse_value(valueSet);
}

void Property::set_value(const PropertyValue & Value)
{
    mProperty.mValue.second = Value;
}

void Property::set_value( PropertyValue && Value )
{
    mProperty.mValue.second = std::move(Value);
}

const PropertyValue & Property::value() const
{
    return mProperty.mValue.second;
}

PropertyValue& Property::value()
{
    return mProperty.mValue.second;
}

String Property::value_string() const
{
    return mProperty.mValue.second.as_string();
}

void Property::set_comment(const String & comment)
{
    mComment = comment;
}

void Property::add_comment(const String & comment)
{
    if (comment.startsWith(NEPersistence::SYNTAX_COMMENT))
    {
        mComment += comment;
    }
    else
    {
        // if does not begin with "# "
        mComment += NEPersistence::SYNTAX_COMMENT;
        mComment += comment;
    }
}

const String & Property::comment() const
{
    return mComment;
}

void Property::set_pair(const Property::Entry & newPair)
{
    mProperty = newPair;
}

void Property::set_pair( Property::Entry && newPair )
{
    mProperty = static_cast<Property::Entry &&>(newPair);
}

const Property::Entry & Property::pair() const
{
    return mProperty;
}

bool Property::is_valid() const
{
    return mProperty.mValue.first.is_valid();
}

bool Property::parse(const String & strProperties)
{
    bool result{ false };
    String data;
    if ( strProperties.getLength() > 1)
    {
        NEString::CharPos pos = strProperties.findFirst(NEPersistence::SYNTAX_COMMENT.data());

        if (strProperties.isValidPosition(pos))
        {
            if (pos != NEString::START_POS)
            {
                add_comment(strProperties.getBuffer(pos));
                strProperties.substring(data, 0, pos);
            }
        }
        else if (pos == NEString::END_POS)
        {
            data = strProperties;
        }

        if (data.isEmpty() == false )
        {
            const char* value{ nullptr };
            const String key{ String::getSubstring(data.as_string(), NEPersistence::SYNTAX_EQUAL.data(), &value) };

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

    if ( !key.isEmpty() && !value.isEmpty() )
    {
        key.append(NEPersistence::SYNTAX_WHITESPACE_DELIMITER)
           .append(NEPersistence::SYNTAX_EQUAL)
           .append(NEPersistence::SYNTAX_WHITESPACE_DELIMITER)
           .append(value);
    }

    if ( mComment.isEmpty() == false )
    {
        if (mComment.isValidPosition(mComment.findFirst(NEPersistence::SYNTAX_LINEEND.data())) || (mComment.getLength() >= 64))
        {
            result.append(mComment).append(NEPersistence::SYNTAX_LINEEND).append(key);
        }
        else
        {
            result.append(key).append(NEPersistence::SYNTAX_WHITESPACE_DELIMITER).append(mComment);
        }
    }
    else
    {
        result = key;
    }

    return result;
}

void Property::reset()
{
    mProperty.mValue.first.reset();
    mProperty.mValue.second.reset();
    mComment.clear();
    mIsTemporary = false;
}

bool Property::is_module_property(const String& module) const
{
    const PropertyKey& key = mProperty.mValue.first;
    return ((module == key.module()) || key.is_all_modules());
}

void Property::set_temporary(bool is_temporary)
{
    mIsTemporary = is_temporary;
}

bool Property::is_temporary() const
{
    return mIsTemporary;
}
