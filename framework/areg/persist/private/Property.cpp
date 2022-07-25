/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/persist/private/Property.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       Property object for persisting data
 ************************************************************************/
#include "areg/persist/Property.hpp"
#include "areg/persist/private/NEPersistence.hpp"
#include "areg/base/NEUtilities.hpp"

#include <utility>

Property::Property(const String & keySet, const String & valueSet, const String & comment /*= String::EmptyString */)
    : mComment  ( comment )
    , mProperty ( PropertyKey(keySet), PropertyValue(valueSet) )
{
}

Property::Property(const Property::Entry & newProperty, const String & comment /*= String::EmptyString */)
    : mComment  ( comment )
    , mProperty ( newProperty )
{
}

Property::Property(const Property & source)
    : mComment  ( source.mComment )
    , mProperty ( source.mProperty )
{
}

Property::Property( Property && source ) noexcept
    : mComment  ( std::move(source.mComment) )
    , mProperty ( std::move(source.mProperty) )
{
}

Property & Property::operator = ( const Property & source )
{
    mComment    = source.mComment;
    mProperty   = source.mProperty;

    return (*this);
}

Property & Property::operator = ( Property && source ) noexcept
{
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

Property::operator unsigned int ( void ) const
{
    return static_cast<unsigned int>(mProperty.mValue.first);
}

void Property::parseKey(const String & keySet)
{
    mProperty.mValue.first.parseKey(keySet);
}

void Property::setKey(const PropertyKey & Key)
{
    mProperty.mValue.first = Key;
}

void Property::setKey( PropertyKey && Key )
{
    mProperty.mValue.first = std::move(Key);
}

const PropertyKey & Property::getKey(void) const
{
    return mProperty.mValue.first;
}

String Property::getKeyString(void) const
{
    return mProperty.mValue.first.convToString();
}

void Property::parseValue(const String & valueSet)
{
    mProperty.mValue.second.parseValue(valueSet);
}

void Property::setValue(const PropertyValue & Value)
{
    mProperty.mValue.second = Value;
}

void Property::setValue( PropertyValue && Value )
{
    mProperty.mValue.second = std::move(Value);
}

const PropertyValue & Property::getValue(void) const
{
    return mProperty.mValue.second;
}

String Property::getValueString(void) const
{
    return mProperty.mValue.second.getString();
}

void Property::setComment(const String & comment)
{
    mComment = comment;
}

void Property::addComment(const String & comment)
{
    if ( mComment.isEmpty() == false )
    {
        mComment += NEPersistence::SYNTAX_LINEEND;
    }

    // if does not begin with "# "
    if (comment.startsWith(NEPersistence::SYNTAX_COMMENT))
    {
        mComment += comment;
    }
    else
    {
        mComment += NEPersistence::SYNTAX_COMMENT;
        mComment += comment;
    }
}

const String & Property::getComment(void) const
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

const Property::Entry & Property::getPropertyPair(void) const
{
    return mProperty;
}

bool Property::isValid(void) const
{
    return mProperty.mValue.first.isValid();
}

bool Property::parseProperty(String strProperties)
{
    if ( strProperties.isEmpty() == false)
    {
        NEString::CharPos pos = strProperties.findFirst(NEPersistence::SYNTAX_COMMENT.data());

        if (strProperties.isValidPosition(pos))
        {
            addComment(strProperties.getBuffer(pos));
            strProperties.substring(0, pos);
        }

        if (strProperties.isEmpty() == false )
        {
            const char * value  = nullptr;
            String key          = String::getSubstring(strProperties.getString(), NEPersistence::SYNTAX_EQUAL.data(), &value);

            mProperty.mValue.first.parseKey(key);
            mProperty.mValue.second.parseValue(value);
        }

        if ( mProperty.mValue.first.isValid() == false )
        {
            mProperty.mValue.first.resetKey();
            mProperty.mValue.second.resetValue();
        }
    }
    else
    {
        addComment(String::EmptyString);
    }

    return isValid();
}

String Property::convToString(void) const
{
    String result;
    String key  (mProperty.mValue.first.convToString());
    String value(mProperty.mValue.second.convToString());

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

void Property::resetData(void)
{
    mProperty.mValue.first.resetKey();
    mProperty.mValue.second.resetValue();
    mComment.clear();
}
