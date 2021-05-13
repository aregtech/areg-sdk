/************************************************************************
 * \file        areg/persist/private/Property.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Property object for persisting data
 ************************************************************************/
#include "areg/persist/Property.hpp"
#include "areg/persist/private/NEPersistence.hpp"
#include "areg/base/NEUtilities.hpp"

Property::Property(void)
    : mComment  ( )
    , mProperty ( )
{
    ; // do nothing
}

Property::Property(const char * keySet, const char * valueSet, const char * comment /*= NULL */)
    : mComment  ( comment != NULL ? comment : "")
    , mProperty ( PropertyKey(keySet), PropertyValue(valueSet) )
{
    ; // do nothing
}

Property::Property(const Property::Entry & newProperty, const char * comment /*= NULL */)
    : mComment  ( comment != NULL ? comment : "")
    , mProperty ( newProperty )
{
    ; // do nothing
}

Property::Property(const Property & source)
    : mComment  ( source.mComment )
    , mProperty ( source.mProperty )
{
    ; // do nothing
}

Property::~Property(void)
{
    ; // do nothing
}

const Property & Property::operator = ( const Property & source )
{
    if ( static_cast<const Property *>(this) != &source )
    {
        mComment    = source.mComment;
        mProperty   = source.mProperty;
    }
    return (*this);
}

bool Property::operator == ( const Property & other ) const
{
    return (this != &other ? mProperty == other.mProperty : true);
}

bool Property::operator!=(const Property & other) const
{
    return (this != &other ? mProperty != other.mProperty : false);
}

Property::operator unsigned int( void ) const
{
    return static_cast<unsigned int>(mProperty.mKey);
}

void Property::parseKey(const char * keySet)
{
    mProperty.mKey.parseKey(keySet);
}

void Property::setKey(const PropertyKey & Key)
{
    mProperty.mKey = Key;
}

const PropertyKey & Property::getKey(void) const
{
    return mProperty.mKey;
}

String Property::getKeyString(void) const
{
    return mProperty.mKey.convToString();
}

void Property::parseValue(const char * valueSet)
{
    mProperty.mValue.parseValue(valueSet);
}

void Property::setValue(const PropertyValue & Value)
{
    mProperty.mValue = Value;
}

const PropertyValue & Property::getValue(void) const
{
    return mProperty.mValue;
}

String Property::getValueString(void) const
{
    return mProperty.mValue.getString();
}

void Property::setComment(const char * comment)
{
    mComment = comment;
}

void Property::addComment(const char * comment)
{
    if ( mComment.isEmpty() == false )
        mComment += NEPersistence::SYNTAX_LINEEND;
    String temp(comment);
    // if does not begin with "# "
    if ( (temp.isEmpty() == false) && (temp.compare(NEPersistence::SYNTAX_COMMENT) != 0) )
        mComment += NEPersistence::SYNTAX_COMMENT;
    mComment += temp;
}

const char * Property::getComment(void) const
{
    return mComment.getString();
}

void Property::setPropertyPair(const Property::Entry & newPair)
{
    mProperty = newPair;
}

const Property::Entry & Property::getPropertyPair(void) const
{
    return mProperty;
}

bool Property::isValid(void) const
{
    return mProperty.mKey.isValid();
}

bool Property::parseProperty(const char * strProperties)
{
    if ( NEString::isEmpty<char>(strProperties) == false )
    {
        String values         = strProperties != NULL ? strProperties : "";
        NEString::CharPos pos   = values.findFirstOf(NEPersistence::SYNTAX_COMMENT);
        if ( pos != NEString::InvalidPos)
        {
            addComment( strProperties + static_cast<int>(pos));
            values = values.substring(0, pos);
        }            
        if ( values.isEmpty() == false )
        {
            const char * value  = NULL;
            String key        = String::getSubstring(values.getString(), NEPersistence::SYNTAX_EQUAL, &value);

            mProperty.mKey.parseKey(key.getString());
            mProperty.mValue.parseValue(value);
        }
        if ( mProperty.mKey.isValid() == false )
        {
            mProperty.mKey.resetKey();
            mProperty.mValue.resetValue();
        }
    }
    else
    {
        addComment("");
    }
    return isValid();
}

String Property::convToString(void) const
{
    String result = "";
    String key     = mProperty.mKey.convToString();
    String value   = mProperty.mValue.convToString();
    if ( (key.isEmpty() == false) && (value.isEmpty() == false) )
    {
        key += NEPersistence::SYNTAX_WHITESPACE_DELIMITER;
        key += NEPersistence::SYNTAX_EQUAL;
        key += NEPersistence::SYNTAX_WHITESPACE_DELIMITER;
        key += value;
    }

    if ( mComment.isEmpty() == false )
    {
        if ((mComment.findFirstOf(NEPersistence::SYNTAX_LINEEND) != NEString::InvalidPos) || (mComment.getLength() >= 64))
        {
            result += mComment;
            result += NEPersistence::SYNTAX_LINEEND;
            result += key;
        }
    }
    else if ( mComment.isEmpty() == false )
    {
        result += key;
        result += NEPersistence::SYNTAX_WHITESPACE_DELIMITER;
        result += mComment;
    }
    else
    {
        result = key;
    }
    return result;
}

void Property::resetData(void)
{
    mProperty.mKey.resetKey();
    mProperty.mValue.resetValue();
    mComment.clear();
}
