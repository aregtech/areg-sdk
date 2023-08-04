/************************************************************************
 * \file        areg/trace/private/TracePropertyKey.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       Trace Property key class. Used when reading or 
 *              saving configuration file. It contains Key value and type
 ************************************************************************/

#include "areg/trace/private/TracePropertyKey.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/NEUtilities.hpp"

#include <utility>

TracePropertyKey::TracePropertyKey( void )
    : mKeyName      ( )
    , mModuleName   ( )
    , mModuleData   ( )
    , mLogConfig    ( NELogConfig::eLogConfig::ConfigUnknown )
    , mConfigKey    ( NELogConfig::eConfigKey::KeyUndefined )
{
}

TracePropertyKey::TracePropertyKey( const char * fullKey )
    : mKeyName      ( )
    , mModuleName   ( )
    , mModuleData   ( )
    , mLogConfig    ( NELogConfig::eLogConfig::ConfigUnknown )
    , mConfigKey    ( NELogConfig::eConfigKey::KeyUndefined )
{
    setKey(fullKey);
}

TracePropertyKey::TracePropertyKey( const String & fullKey )
    : mKeyName      ( )
    , mModuleName   ( )
    , mModuleData   ( )
    , mLogConfig    ( NELogConfig::eLogConfig::ConfigUnknown )
    , mConfigKey    ( NELogConfig::eConfigKey::KeyUndefined )
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
}

TracePropertyKey::TracePropertyKey( TracePropertyKey && source ) noexcept
    : mKeyName      ( std::move(source.mKeyName) )
    , mModuleName   ( std::move(source.mModuleName) )
    , mModuleData   ( std::move(source.mModuleData) )
    , mLogConfig    ( std::move(source.mLogConfig) )
    , mConfigKey    ( std::move(source.mConfigKey) )
{
}

TracePropertyKey & TracePropertyKey::operator = ( const TracePropertyKey & source )
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

TracePropertyKey & TracePropertyKey::operator = ( TracePropertyKey && source ) noexcept
{
    if ( static_cast<TracePropertyKey *>(this) != &source )
    {
        mKeyName    = std::move(source.mKeyName);
        mModuleName = std::move(source.mModuleName);
        mModuleData = std::move(source.mModuleData);
        mLogConfig  = std::move(source.mLogConfig);
        mConfigKey  = std::move(source.mConfigKey);
    }

    return (*this);
}

void TracePropertyKey::setKey(const String & fullKey)
{
    mKeyName    = fullKey;
    mKeyName.trimAll();
    mLogConfig  = NELogConfig::convFromString( mKeyName );
    if ( mLogConfig != NELogConfig::eLogConfig::ConfigUnknown )
    {
        int len = static_cast<int>(NELogConfig::ValidSyntaxList[static_cast<int>(mLogConfig)].length());
        if ( mKeyName.getLength() > len )
        {
            NEString::CharPos pos = mKeyName.findLast(NELogConfig::SYNTAX_OBJECT_SEPARATOR, NEString::END_POS );
            if (mKeyName.isValidPosition(pos) && (pos > len) )
            {
                mKeyName.substring(mModuleName, len + 1, pos - len - 1);
                mKeyName.substring(mModuleData, pos + 1);
            }
            else
            {
                mKeyName.substring(mModuleName, len + 1);
                mModuleData = String::EmptyString;
            }
        }
        else
        {
            mModuleName = String::EmptyString;
            mModuleData = String::EmptyString;
        }

        mConfigKey = (mModuleName.isEmpty( ) || (mModuleName == NELogConfig::SYNTAX_SCOPE_GROUP)) ? NELogConfig::eConfigKey::KeyGlobal : NELogConfig::eConfigKey::KeyLocal;
    }
    else
    {
        mModuleName = String::getEmptyString();
        mConfigKey  = NELogConfig::eConfigKey::KeyUndefined;
    }
}

bool TracePropertyKey::isModuleKeySet(const String & moduleName /*= String::getEmptyString() */) const
{
    if ( mConfigKey != NELogConfig::eConfigKey::KeyUndefined )
    {
        return (mModuleName.isEmpty() || 
               (mModuleName == NELogConfig::SYNTAX_SCOPE_GROUP) || 
               (mModuleName.compare( moduleName, 0, NEString::COUNT_ALL, false ) == NEMath::eCompare::Equal));
    }
    else
    {
        return false;
    }
}

bool TracePropertyKey::isExactModule( const String & module ) const
{
    if ( mConfigKey != NELogConfig::eConfigKey::KeyUndefined )
    {
        return (mModuleName.compare( module, 0, NEString::COUNT_ALL, false ) == NEMath::eCompare::Equal);
    }
    else
    {
        return false;
    }
}

const String TracePropertyKey::getKeySyntax( void ) const
{
    return NELogConfig::ValidSyntaxList[static_cast<int>(mLogConfig)];
}
