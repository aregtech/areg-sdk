/************************************************************************
 * \file        areg/src/trace/private/CETracePropertyKey.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Trace Property key class. Used when reading or 
 *              saving configuration file. It contains Key value and type
 ************************************************************************/

#include "areg/src/trace/private/CETracePropertyKey.hpp"
#include "areg/src/base/CEProcess.hpp"
#include "areg/src/base/NEUtilities.hpp"

CETracePropertyKey::CETracePropertyKey( void )
    : mKeyName      ( )
    , mModuleName   ( )
    , mModuleData   ( )
    , mLogConfig    ( NELogConfig::ConfigUnknown )
    , mConfigKey    ( NELogConfig::KeyUndefined )
{
    ; // do nothing
}

CETracePropertyKey::CETracePropertyKey( const char * fullKey )
    : mKeyName      ( )
    , mModuleName   ( )
    , mModuleData   ( )
    , mLogConfig    ( NELogConfig::ConfigUnknown )
    , mConfigKey    ( NELogConfig::KeyUndefined )
{
    SetKey(fullKey);
}

CETracePropertyKey::CETracePropertyKey( const CEString & fullKey )
    : mKeyName( )
    , mModuleName( )
    , mModuleData( )
    , mLogConfig( NELogConfig::ConfigUnknown )
    , mConfigKey( NELogConfig::KeyUndefined )
{
    SetKey( fullKey );
}

CETracePropertyKey::CETracePropertyKey( const CETracePropertyKey & source )
    : mKeyName      ( source.mKeyName )
    , mModuleName   ( source.mModuleName )
    , mModuleData   ( source.mModuleData )
    , mLogConfig    ( source.mLogConfig )
    , mConfigKey    ( source.mConfigKey )
{
    ; // do nothing
}

CETracePropertyKey::~CETracePropertyKey( void )
{
    ; // do nothing
}

const CETracePropertyKey & CETracePropertyKey::operator = ( const CETracePropertyKey & source )
{
    if ( static_cast<const CETracePropertyKey *>(this) != &source )
    {
        mKeyName    = source.mKeyName;
        mModuleName = source.mModuleName;
        mModuleData = source.mModuleData;
        mLogConfig  = source.mLogConfig;
        mConfigKey  = source.mConfigKey;
    }
    return (*this);
}

void CETracePropertyKey::SetKey(const char * fullKey)
{
    mKeyName    = fullKey != NULL ? fullKey : "";
    mKeyName.TrimAll();
    mLogConfig  = NELogConfig::FromString( mKeyName );
    if ( mLogConfig != NELogConfig::ConfigUnknown )
    {
        unsigned int len = static_cast<unsigned int>(NELogConfig::ValidSytaxList[static_cast<int>(mLogConfig)].syntaxLength);
        if ( mKeyName.GetLength() > len )
        {
            NEString::CharPos pos = mKeyName.FindFirstOf(NELogConfig::SYNTAX_OBJECT_SEPARATOR, len + 1);
            if ( (pos != NEString::InvalidPos) && (pos > len) )
            {
                mModuleName = mKeyName.Substr(len + 1, pos - len - 1);
                mModuleData = mKeyName.Substr(pos + 1);
            }
            else
            {
                mModuleName = mKeyName.Substr( len + 1);
                mModuleData = "";
            }

            if (mModuleName == NELogConfig::SYNTAX_SCOPE_GROUP)
                mModuleName.Clear();

            if ( false && mModuleName.IsEmpty() == false )
            {
                pos = mModuleName.FindFirstOf(NELogConfig::SYNTAX_OBJECT_SEPARATOR, len + 1);
                if ( pos != NEString::InvalidPos )
                    mModuleName = mModuleName.Substr(pos);
            }
        }
        else
        {
            mModuleName = "";
            mModuleData = "";
        }
        mConfigKey  = mModuleName.IsEmpty() ? NELogConfig::KeyGlobal : NELogConfig::KeyLocal;
    }
    else
    {
        mModuleName = "";
        mConfigKey  = NELogConfig::KeyUndefined;
    }
}

bool CETracePropertyKey::IsModuleKeySet(const char * moduleName /*= NULL */) const
{
    return ( mConfigKey != NELogConfig::KeyUndefined ? (mModuleName.IsEmpty() || (mModuleName.Compare(moduleName, 0, NEString::CountAll, false) == 0)) : false );
}

const char * CETracePropertyKey::GetKeySytax( void ) const
{
    return NELogConfig::ValidSytaxList[static_cast<int>(mLogConfig)].sytaxName;
}
