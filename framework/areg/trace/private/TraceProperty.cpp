/************************************************************************
 * \file        areg/trace/private/TraceProperty.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Trace Property value class. Used when reading or 
 *              saving configuration file
 ************************************************************************/
#include "areg/trace/private/TraceProperty.hpp"

#include "areg/trace/private/NELogConfig.hpp"
#include "areg/base/File.hpp"
#include "areg/base/NEUtilities.hpp"

TraceProperty::TraceProperty( const char * Key, const char * Value )
    : mProperty ( )
    , mComment  ( )
{
    mProperty.mValue.first  = Key;
    mProperty.mValue.second = Value;
}

TraceProperty::TraceProperty( const char * Key, unsigned int Value )
    : mProperty ( )
    , mComment  ( )
{
    mProperty.mValue.first  = Key;
    mProperty.mValue.second = Value;
}

TraceProperty::TraceProperty( const char * Key, NETrace::eLogPriority Value )
    : mProperty ( )
    , mComment  ( )
{
    mProperty.mValue.first  = Key;
    mProperty.mValue.second = Value;
}

TraceProperty::TraceProperty( const char * Key, bool Value )
    : mProperty ( )
    , mComment  ( )
{
    mProperty.mValue.first  = Key;
    mProperty.mValue.second = Value;
}

TraceProperty::TraceProperty( String & source )
    : mProperty()
    , mComment()
{
    _parseProperty(source);
}

TraceProperty::TraceProperty( const TraceProperty & source )
    : mProperty ( source.mProperty )
    , mComment  ( source.mComment )
{
}

TraceProperty::TraceProperty( TraceProperty && source ) noexcept
    : mProperty ( std::move(source.mProperty) )
    , mComment  ( std::move(source.mComment) )
{
}

TraceProperty & TraceProperty::operator = ( const TraceProperty & source )
{
    mProperty   = source.mProperty;
    mComment    = source.mComment;

    return (*this);
}

TraceProperty & TraceProperty::operator = ( TraceProperty && source ) noexcept
{
    mProperty   = std::move(source.mProperty);
    mComment    = std::move( source.mComment);

    return (*this);
}

void TraceProperty::setData( const char * Key, const char * Value )
{
    mProperty.mValue.first  = Key;
    mProperty.mValue.second = Value;
}

void TraceProperty::setData(const char * Key, const char * Value, const char * Comment )
{
    mProperty.mValue.first  = Key;
    mProperty.mValue.second = Value;
    mComment                = Comment;
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
    mProperty.mValue.first  = String::EmptyString;
    mProperty.mValue.second = String::EmptyString;
    if ( clearComment )
    {
        mComment = String::EmptyString;
    }
}

String TraceProperty::makeConfigString( void ) const
{
    String result (mComment);
    if ( mProperty.mValue.first.isValidKey( ) )
    {
        String config;
        config.format( "%s = %s\n", mProperty.mValue.first.getKey( ).getString( ), static_cast<const char *>(mProperty.mValue.second) );
        result += config;
    }

    return result;
}

bool TraceProperty::parseProperty( const char * logSetting, bool discardComment /*= true*/ )
{
    bool result = false;
    if ( NEString::isEmpty<char>(logSetting) == false )
    {
        String setting(logSetting);
        result = parseProperty(setting, discardComment);
    }

    return result;
}

bool TraceProperty::parseProperty( String & line, bool discardComment /*= true*/ )
{
    _parseProperty(line, discardComment);
    return isValid();
}

bool TraceProperty::operator == ( const TraceProperty & other ) const
{
    return mProperty.mValue.first == other.mProperty.mValue.first;
}

void TraceProperty::_parseProperty(String& source, bool discardComment /*= true*/ )
{
    NEString::CharPos posComment = source.findFirst(NELogConfig::SYNTAX_COMMENT);
    NEString::CharPos posEqual = source.findFirst( NELogConfig::SYNTAX_EQUAL );

    bool isValidPos{ false };
    if (source.isValidPosition(posComment))
    {
        if ( discardComment == false )
        {
            String comment;
            isValidPos = true;
            source.substring( comment, posComment );
            ASSERT( comment.isEmpty( ) == false );
            mComment += comment;
            mComment += NELogConfig::SYNTAX_LINEEND;
        }

        source.substring(0, posComment);
    }

    if (source.isValidPosition(posEqual) && ((isValidPos == false) || (posEqual < posComment)))
    {
        String temp;

        source.substring(temp, NEString::START_POS, posEqual);
        mProperty.mValue.first = temp.getString();

        source.substring(temp, posEqual + 1, isValidPos ? posComment - posEqual : NEString::END_POS);
        mProperty.mValue.second = temp.getString();
    }
    else if ( (source.isValidPosition( posEqual ) == false) && (isValidPos == false) && (discardComment == false))
    {
        mComment += NELogConfig::SYNTAX_LINEEND;
    }
}