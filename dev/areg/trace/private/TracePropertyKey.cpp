/************************************************************************
 * \file        areg/trace/private/TracePropertyKey.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Trace Property key class. Used when reading or 
 *              saving configuration file. It contains Key value and type
 ************************************************************************/

#include "areg/trace/private/TracePropertyKey.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/NEUtilities.hpp"

TracePropertyKey::TracePropertyKey( void )
    : mKeyName      ( )
    , mModuleName   ( )
    , mModuleData   ( )
    , mLogConfig    ( NELogConfig::ConfigUnknown )
    , mConfigKey    ( NELogConfig::KeyUndefined )
{
    ; // do nothing
}

TracePropertyKey::TracePropertyKey( const char * fullKey )
    : mKeyName      ( )
    , mModuleName   ( )
    , mModuleData   ( )
    , mLogConfig    ( NELogConfig::ConfigUnknown )
    , mConfigKey    ( NELogConfig::KeyUndefined )
{
    setKey(fullKey);
}

TracePropertyKey::TracePropertyKey( const String & fullKey )
    : mKeyName( )
    , mModuleName( )
    , mModuleData( )
    , mLogConfig( NELogConfig::ConfigUnknown )
    , mConfigKey( NELogConfig::KeyUndefined )
{
    setKey( fullKey );
}

TracePropertyKey::TracePropertyKey( const TracePropertyKey & source )
    : mKeyName      ( source.mKeyName )
    , mModuleName   ( source.mModuleName )
    , mModuleData   ( source.mModuleData )
    , mLogConfig    ( source.mLogConfig )
    , mConfigKey    ( source.mConfigKey )
{
    ; // do nothing
}

TracePropertyKey::~TracePropertyKey( void )
{
    ; // do nothing
}

const TracePropertyKey & TracePropertyKey::operator = ( const TracePropertyKey & source )
{
    if ( static_cast<const TracePropertyKey *>(this) != &source )
    {
        mKeyName    = source.mKeyName;
        mModuleName = source.mModuleName;
        mModuleData = source.mModuleData;
        mLogConfig  = source.mLogConfig;
        mConfigKey  = source.mConfigKey;
    }
    return (*this);
}

void TracePropertyKey::setKey(const char * fullKey)
{
    mKeyName    = fullKey != NULL ? fullKey : "";
    mKeyName.trimAll();
    mLogConfig  = NELogConfig::convFromString( mKeyName );
    if ( mLogConfig != NELogConfig::ConfigUnknown )
    {
        int len = NELogConfig::ValidSytaxList[static_cast<int>(mLogConfig)].syntaxLength;
        if ( mKeyName.getLength() > len )
        {
            NEString::CharPos pos = mKeyName.findFirstOf(NELogConfig::SYNTAX_OBJECT_SEPARATOR, len + 1);
            if ( (pos != NEString::InvalidPos) && (pos > len) )
            {
                mModuleName = mKeyName.substring(len + 1, pos - len - 1);
                mModuleData = mKeyName.substring(pos + 1);
            }
            else
            {
                mModuleName = mKeyName.substring( len + 1);
                mModuleData = "";
            }

            if (mModuleName == NELogConfig::SYNTAX_SCOPE_GROUP)
                mModuleName.clear();

            if ( false && mModuleName.isEmpty() == false )
            {
                pos = mModuleName.findFirstOf(NELogConfig::SYNTAX_OBJECT_SEPARATOR, len + 1);
                if ( pos != NEString::InvalidPos )
                    mModuleName = mModuleName.substring(pos);
            }
        }
        else
        {
            mModuleName = "";
            mModuleData = "";
        }
        mConfigKey  = mModuleName.isEmpty() ? NELogConfig::KeyGlobal : NELogConfig::KeyLocal;
    }
    else
    {
        mModuleName = "";
        mConfigKey  = NELogConfig::KeyUndefined;
    }
}

bool TracePropertyKey::isModuleKeySet(const char * moduleName /*= NULL */) const
{
    return ( mConfigKey != NELogConfig::KeyUndefined ? (mModuleName.isEmpty() || (mModuleName.compare(moduleName, 0, NEString::CountAll, false) == 0)) : false );
}

const char * TracePropertyKey::getKeySytax( void ) const
{
    return NELogConfig::ValidSytaxList[static_cast<int>(mLogConfig)].sytaxName;
}
