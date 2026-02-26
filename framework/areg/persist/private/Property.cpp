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
    , mComment      (areg::String::EmptyString)
    , mProperty     ( )
{
}

Property::Property(const areg::String strProperty)
    : mIsTemporary  (false)
    , mComment      (areg::String::EmptyString)
    , mProperty     ()
{
    parseProperty(strProperty);
}

Property::Property(const areg::PropertyKey& key, const areg::PropertyValue& value, const areg::String& comment /*= areg::String::EmptyString*/, bool isTemporary /* = false*/)
    : mIsTemporary  (isTemporary)
    , mComment      (comment)
    , mProperty     (key, value)
{
}

Property::Property(areg::PropertyKey&& key, areg::PropertyValue&& value, areg::String&& comment)
    : mIsTemporary  (false)
    , mComment      ( std::move(comment))
    , mProperty     ( std::move(key), std::move(value))
{
}

Property::Property(areg::PropertyKey&& key, areg::PropertyValue&& value)
    : mIsTemporary  (false)
    , mComment      ( )
    , mProperty     ( std::move(key), std::move(value))
{
}

Property::Property(const areg::String & keySet, const areg::String & valueSet, const areg::String & comment /*= areg::String::EmptyString*/, bool isTemporary /*= false*/)
    : mIsTemporary  ( isTemporary )
    , mComment      ( comment )
    , mProperty     ( areg::PropertyKey(keySet), areg::PropertyValue(valueSet) )
{
}

Property::Property(const char* keySet, const char* valueSet, const char* comment /*= nullptr*/, bool isTemporary /* = false*/)
    : mIsTemporary  ( isTemporary )
    , mComment      ( comment )
    , mProperty     ( areg::PropertyKey(keySet), areg::PropertyValue(valueSet) )
{
}

Property::Property(const std::string_view& keySet, const std::string_view& valueSet, const std::string_view& comment /*= areg::String::EmptyString*/, bool isTemporary /* = false*/)
    : mIsTemporary  ( isTemporary )
    , mComment      ( comment )
    , mProperty     ( areg::PropertyKey(keySet), areg::PropertyValue(valueSet) )
{
}

Property::Property( const std::string_view& section
                  , const std::string_view& module
                  , const std::string_view& property
                  , const std::string_view& position
                  , const std::string_view& value
                  , const std::string_view& comment /*= areg::String::EmptyString*/
                  , bool isTemporary /* = false*/)
    : mIsTemporary  ( isTemporary )
    , mComment      ( comment )
    , mProperty     ( areg::PropertyKey(section, module, property, position), areg::PropertyValue(value) )
{
}

Property::Property(const Property::Entry & newProperty, const areg::String & comment /*= areg::String::getEmptyString*/, bool isTemporary /* = false*/)
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

void Property::parseKey(const areg::String & keySet)
{
    mProperty.mValue.first.parseKey(keySet);
}

void Property::setKey(const areg::PropertyKey & Key)
{
    mProperty.mValue.first = Key;
}

void Property::setKey( areg::PropertyKey && Key )
{
    mProperty.mValue.first = std::move(Key);
}

const areg::PropertyKey & Property::getKey() const
{
    return mProperty.mValue.first;
}

areg::ConfigEntry Property::getPropertyType() const
{
    return mProperty.mValue.first.getKeyType();
}

areg::String Property::getKeyString() const
{
    return mProperty.mValue.first.convToString();
}

void Property::parseValue(const areg::String & valueSet)
{
    mProperty.mValue.second.parseValue(valueSet);
}

void Property::setValue(const areg::PropertyValue & Value)
{
    mProperty.mValue.second = Value;
}

void Property::setValue( areg::PropertyValue && Value )
{
    mProperty.mValue.second = std::move(Value);
}

const areg::PropertyValue & Property::getValue() const
{
    return mProperty.mValue.second;
}

areg::PropertyValue& Property::getValue()
{
    return mProperty.mValue.second;
}

areg::String Property::getValueString() const
{
    return mProperty.mValue.second.getString();
}

void Property::setComment(const areg::String & comment)
{
    mComment = comment;
}

void Property::addComment(const areg::String & comment)
{
    if (comment.startsWith(areg::SYNTAX_COMMENT))
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

const areg::String & Property::getComment() const
{
    return mComment;
}

void Property::setPropertyPair(const Property::Entry & newPair)
{
    mProperty = newPair;
}

void Property::setPropertyPair( Property::Entry && newPair )
{
    mProperty = static_cast<Property::Entry &&>(newPair);
}

const Property::Entry & Property::getPropertyPair() const
{
    return mProperty;
}

bool Property::isValid() const
{
    return mProperty.mValue.first.isValid();
}

bool Property::parseProperty(const areg::String & strProperties)
{
    bool result{ false };
    areg::String data;
    if ( strProperties.getLength() > 1)
    {
        areg::CharPos pos = strProperties.findFirst(areg::SYNTAX_COMMENT.data());

        if (strProperties.isValidPosition(pos))
        {
            if (pos != areg::START_POS)
            {
                addComment(strProperties.getBuffer(pos));
                strProperties.substring(data, 0, pos);
            }
        }
        else if (pos == areg::END_POS)
        {
            data = strProperties;
        }

        if (data.isEmpty() == false )
        {
            const char* value{ nullptr };
            const areg::String key{ areg::String::getSubstring(data.getString(), areg::SYNTAX_EQUAL.data(), &value) };

            mProperty.mValue.first.parseKey(key);
            mProperty.mValue.second.parseValue(value);

            if (isValid())
            {
                mIsTemporary = false;
                result = true;
            }
            else
            {
                mProperty.mValue.first.resetKey();
                mProperty.mValue.second.resetValue();
            }
        }
    }

    return result;
}

areg::String Property::convToString() const
{
    areg::String result;
    areg::String key  (mProperty.mValue.first.convToString());
    areg::String value(mProperty.mValue.second.convToString());

    if ( !key.isEmpty() && !value.isEmpty() )
    {
        key.append(areg::SYNTAX_WHITESPACE_DELIMITER)
           .append(areg::SYNTAX_EQUAL)
           .append(areg::SYNTAX_WHITESPACE_DELIMITER)
           .append(value);
    }

    if ( mComment.isEmpty() == false )
    {
        if (mComment.isValidPosition(mComment.findFirst(areg::SYNTAX_LINEEND.data())) || (mComment.getLength() >= 64))
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

void Property::resetData()
{
    mProperty.mValue.first.resetKey();
    mProperty.mValue.second.resetValue();
    mComment.clear();
    mIsTemporary = false;
}

bool Property::isModuleProperty(const areg::String& module) const
{
    const areg::PropertyKey& key = mProperty.mValue.first;
    return ((module == key.getModule()) || key.isAllModules());
}

void Property::setTemporary(bool isTemporary)
{
    mIsTemporary = isTemporary;
}

bool Property::isTemporary() const
{
    return mIsTemporary;
}
