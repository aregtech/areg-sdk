/************************************************************************
 * \file        areg/src/persist/private/CEProperty.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Property object for persisting data
 ************************************************************************/
#include "areg/src/persist/CEProperty.hpp"
#include "areg/src/persist/private/NEPersistence.hpp"
#include "areg/src/base/NEUtilities.hpp"

CEProperty::CEProperty(void)
    : mComment  ( )
    , mProperty ( )
{
    ; // do nothing
}

CEProperty::CEProperty(const char * keySet, const char * valueSet, const char * comment /*= NULL */)
    : mComment  ( comment != NULL ? comment : "")
    , mProperty ( CEPropertyKey(keySet), CEPropertyValue(valueSet) )
{
    ; // do nothing
}

CEProperty::CEProperty(const Property & newProperty, const char * comment /*= NULL */)
    : mComment  ( comment != NULL ? comment : "")
    , mProperty ( newProperty )
{
    ; // do nothing
}

CEProperty::CEProperty(const CEProperty & source)
    : mComment  ( source.mComment )
    , mProperty ( source.mProperty )
{
    ; // do nothing
}

CEProperty::~CEProperty(void)
{
    ; // do nothing
}

const CEProperty & CEProperty::operator = ( const CEProperty & source )
{
    if ( static_cast<const CEProperty *>(this) != &source )
    {
        mComment    = source.mComment;
        mProperty   = source.mProperty;
    }
    return (*this);
}

bool CEProperty::operator == ( const CEProperty & other ) const
{
    return (this != &other ? mProperty == other.mProperty : true);
}

bool CEProperty::operator!=(const CEProperty & other) const
{
    return (this != &other ? mProperty != other.mProperty : false);
}

CEProperty::operator unsigned int( void ) const
{
    return static_cast<unsigned int>(mProperty.mKey);
}

void CEProperty::ParseKey(const char * keySet)
{
    mProperty.mKey.ParseKey(keySet);
}

void CEProperty::SetKey(const CEPropertyKey & Key)
{
    mProperty.mKey = Key;
}

const CEPropertyKey & CEProperty::GetKey(void) const
{
    return mProperty.mKey;
}

CEString CEProperty::GetKeyString(void) const
{
    return mProperty.mKey.ToString();
}

void CEProperty::ParseValue(const char * valueSet)
{
    mProperty.mValue.ParseValue(valueSet);
}

void CEProperty::SetValue(const CEPropertyValue & Value)
{
    mProperty.mValue = Value;
}

const CEPropertyValue & CEProperty::GetValue(void) const
{
    return mProperty.mValue;
}

CEString CEProperty::GetValueString(void) const
{
    return mProperty.mValue.GetString();
}

void CEProperty::SetCommend(const char * comment)
{
    mComment = comment;
}

void CEProperty::AddComment(const char * comment)
{
    if ( mComment.IsEmpty() == false )
        mComment += NEPersistence::SYNTAX_LINEEND;
    CEString temp(comment);
    // if does not begin with "# "
    if ( (temp.IsEmpty() == false) && (temp.Compare(NEPersistence::SYNTAX_COMMENT) != 0) )
        mComment += NEPersistence::SYNTAX_COMMENT;
    mComment += temp;
}

const char * CEProperty::GetComment(void) const
{
    return mComment.GetBuffer();
}

void CEProperty::SetPropertyPair(const Property & newPair)
{
    mProperty = newPair;
}

const CEProperty::Property & CEProperty::GetPropertyPair(void) const
{
    return mProperty;
}

bool CEProperty::IsValid(void) const
{
    return mProperty.mKey.IsValid();
}

bool CEProperty::ParseProperty(const char * strProperties)
{
    if ( NEString::isEmpty<char>(strProperties) == false )
    {
        CEString values         = strProperties != NULL ? strProperties : "";
        NEString::CharPos pos   = values.FindFirstOf(NEPersistence::SYNTAX_COMMENT);
        if ( pos != NEString::InvalidPos)
        {
            AddComment( strProperties + static_cast<int>(pos));
            values = values.Substr(0, pos);
        }            
        if ( values.IsEmpty() == false )
        {
            const char * value  = NULL;
            CEString key        = CEString::GetSubstring(values.GetBuffer(), NEPersistence::SYNTAX_EQUAL, &value);

            mProperty.mKey.ParseKey(key.GetBuffer());
            mProperty.mValue.ParseValue(value);
        }
        if ( mProperty.mKey.IsValid() == false )
        {
            mProperty.mKey.ResetKey();
            mProperty.mValue.ResetValue();
        }
    }
    else
    {
        AddComment("");
    }
    return IsValid();
}

CEString CEProperty::ToString(void) const
{
    CEString result = "";
    CEString key     = mProperty.mKey.ToString();
    CEString value   = mProperty.mValue.ToString();
    if ( (key.IsEmpty() == false) && (value.IsEmpty() == false) )
    {
        key += NEPersistence::SYNTAX_WHITESPACE_DELIMITER;
        key += NEPersistence::SYNTAX_EQUAL;
        key += NEPersistence::SYNTAX_WHITESPACE_DELIMITER;
        key += value;
    }

    if ( mComment.IsEmpty() == false )
    {
        if ((mComment.FindFirstOf(NEPersistence::SYNTAX_LINEEND) != NEString::InvalidPos) || (mComment.GetLength() >= 64))
        {
            result += mComment;
            result += NEPersistence::SYNTAX_LINEEND;
            result += key;
        }
    }
    else if ( mComment.IsEmpty() == false )
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

void CEProperty::ResetData(void)
{
    mProperty.mKey.ResetKey();
    mProperty.mValue.ResetValue();
    mComment.Clear();
}
