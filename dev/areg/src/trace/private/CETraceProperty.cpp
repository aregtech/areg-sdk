/************************************************************************
 * \file        areg/src/trace/private/CETraceProperty.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Trace Property value class. Used when reading or 
 *              saving configuration file
 ************************************************************************/
#include "areg/src/trace/private/CETraceProperty.hpp"

#include "areg/src/trace/private/NELogConfig.hpp"
#include "areg/src/base/CEFile.hpp"
#include "areg/src/base/NEUtilities.hpp"

CETraceProperty::CETraceProperty( void )
    : mProperty ( )
    , mComment  ( )
{
    ; // do nothing
}

CETraceProperty::CETraceProperty( const char * Key, const char * Value )
    : mProperty ( )
    , mComment  ( )
{
    mProperty.mKey  = Key;
    mProperty.mValue= Value;
}

CETraceProperty::CETraceProperty( const char * Key, unsigned int Value )
    : mProperty ( )
    , mComment  ( )
{
    mProperty.mKey  = Key;
    mProperty.mValue= Value;
}

CETraceProperty::CETraceProperty( const char * Key, NETrace::eLogPriority Value )
    : mProperty ( )
    , mComment  ( )
{
    mProperty.mKey  = Key;
    mProperty.mValue= Value;
}

CETraceProperty::CETraceProperty( const char * Key, bool Value )
    : mProperty ( )
    , mComment  ( )
{
    mProperty.mKey  = Key;
    mProperty.mValue= Value;
}

CETraceProperty::CETraceProperty( const CETraceProperty & source )
    : mProperty ( source.mProperty )
    , mComment  ( source.mComment )
{
    ; // do nothing
}

CETraceProperty::~CETraceProperty( void )
{
    ; // do nothing
}

const CETraceProperty & CETraceProperty::operator = ( const CETraceProperty & source )
{
    if ( static_cast<const CETraceProperty *>(this) != &source )
    {
        mProperty   = source.mProperty;
        mComment    = source.mComment;
    }
    return (*this);
}

void CETraceProperty::SetData( const char * Key, const char * Value )
{
    mProperty.mKey  = Key;
    mProperty.mValue= Value;
}

void CETraceProperty::SetData(const char * Key, const char * Value, const char * Comment )
{
    mProperty.mKey  = Key;
    mProperty.mValue= Value;
    mComment        = Comment;
}

bool CETraceProperty::ReadProperty( const CEFile & fileConfig )
{
    ClearProperty();
    if ( fileConfig.IsOpened() )
    {
        CEString line;
        while ( fileConfig.ReadLine(line) > 0 && ParseProperty(line) == false )
            ;
    }
    return IsValid();
}

void CETraceProperty::ClearProperty( bool clearComment /* = true */ )
{
    mProperty.mKey  = "";
    mProperty.mValue= "";
    if ( clearComment )
        mComment        = "";
}

bool CETraceProperty::ParseProperty( const char * logSetting )
{
    bool result = false;
    if ( NEString::isEmpty<char>(logSetting) == false )
    {
        CEString setting(logSetting);
        result = ParseProperty(setting);
    }

    return result;
}

bool CETraceProperty::ParseProperty( CEString & line)
{
    NEString::CharPos posComment  = line.FindFirstOf(NELogConfig::SYNTAX_COMMENT);
    if (posComment != NEString::InvalidPos)
    {
        mComment   += line.Substr(posComment);
        mComment   += NELogConfig::SYNTAX_LINEEND;
        line        = line.Substr(0, posComment);
    }

    NEString::CharPos posEqual    = line.FindFirstOf(NELogConfig::SYNTAX_EQUAL);
    if ( (posEqual != NEString::InvalidPos) && ((posComment == NEString::InvalidPos) || (posEqual < posComment))  )
    {
        mProperty.mKey  = line.Substr(NEString::StartPos, posEqual).GetBuffer();
        mProperty.mValue= line.Substr( posEqual + 1, posComment != NEString::InvalidPos ? posComment - posEqual : NEString::EndPos).GetBuffer();
    }
    return IsValid();
}

bool CETraceProperty::operator == ( const CETraceProperty & other ) const
{
    return mProperty.mKey == other.mProperty.mKey;
}
