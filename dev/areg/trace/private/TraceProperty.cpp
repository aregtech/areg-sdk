/************************************************************************
 * \file        areg/trace/private/TraceProperty.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Trace Property value class. Used when reading or 
 *              saving configuration file
 ************************************************************************/
#include "areg/trace/private/TraceProperty.hpp"

#include "areg/trace/private/NELogConfig.hpp"
#include "areg/base/File.hpp"
#include "areg/base/NEUtilities.hpp"

TraceProperty::TraceProperty( void )
    : mProperty ( )
    , mComment  ( )
{
    ; // do nothing
}

TraceProperty::TraceProperty( const char * Key, const char * Value )
    : mProperty ( )
    , mComment  ( )
{
    mProperty.mKey  = Key;
    mProperty.mValue= Value;
}

TraceProperty::TraceProperty( const char * Key, unsigned int Value )
    : mProperty ( )
    , mComment  ( )
{
    mProperty.mKey  = Key;
    mProperty.mValue= Value;
}

TraceProperty::TraceProperty( const char * Key, NETrace::eLogPriority Value )
    : mProperty ( )
    , mComment  ( )
{
    mProperty.mKey  = Key;
    mProperty.mValue= Value;
}

TraceProperty::TraceProperty( const char * Key, bool Value )
    : mProperty ( )
    , mComment  ( )
{
    mProperty.mKey  = Key;
    mProperty.mValue= Value;
}

TraceProperty::TraceProperty( const TraceProperty & source )
    : mProperty ( source.mProperty )
    , mComment  ( source.mComment )
{
    ; // do nothing
}

TraceProperty::~TraceProperty( void )
{
    ; // do nothing
}

const TraceProperty & TraceProperty::operator = ( const TraceProperty & source )
{
    if ( static_cast<const TraceProperty *>(this) != &source )
    {
        mProperty   = source.mProperty;
        mComment    = source.mComment;
    }
    return (*this);
}

void TraceProperty::setData( const char * Key, const char * Value )
{
    mProperty.mKey  = Key;
    mProperty.mValue= Value;
}

void TraceProperty::setData(const char * Key, const char * Value, const char * Comment )
{
    mProperty.mKey  = Key;
    mProperty.mValue= Value;
    mComment        = Comment;
}

bool TraceProperty::readProperty( const File & fileConfig )
{
    clearProperty();
    if ( fileConfig.isOpened() )
    {
        String line;
        while ( fileConfig.readLine(line) > 0 && parseProperty(line) == false )
            ;
    }
    return isValid();
}

void TraceProperty::clearProperty( bool clearComment /* = true */ )
{
    mProperty.mKey  = String::EmptyString;
    mProperty.mValue= String::EmptyString;
    if ( clearComment )
        mComment = String::EmptyString;
}

bool TraceProperty::parseProperty( const char * logSetting )
{
    bool result = false;
    if ( NEString::isEmpty<char>(logSetting) == false )
    {
        String setting(logSetting);
        result = parseProperty(setting);
    }

    return result;
}

bool TraceProperty::parseProperty( String & line)
{
    NEString::CharPos posComment  = line.findFirstOf(NELogConfig::SYNTAX_COMMENT);
    if (posComment != NEString::InvalidPos)
    {
        mComment   += line.substring(posComment);
        mComment   += NELogConfig::SYNTAX_LINEEND;
        line        = line.substring(0, posComment);
    }

    NEString::CharPos posEqual    = line.findFirstOf(NELogConfig::SYNTAX_EQUAL);
    if ( (posEqual != NEString::InvalidPos) && ((posComment == NEString::InvalidPos) || (posEqual < posComment))  )
    {
        mProperty.mKey  = line.substring(NEString::StartPos, posEqual).getString();
        mProperty.mValue= line.substring( posEqual + 1, posComment != NEString::InvalidPos ? posComment - posEqual : NEString::EndPos).getString();
    }
    return isValid();
}

bool TraceProperty::operator == ( const TraceProperty & other ) const
{
    return mProperty.mKey == other.mProperty.mKey;
}
