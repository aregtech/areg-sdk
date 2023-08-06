/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/trace/private/LogConfiguration.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Interface of log configuration
 ************************************************************************/

#include "areg/trace/private/LogConfiguration.hpp"

#include "areg/appbase/NEApplication.hpp"
#include "areg/base/Containers.hpp"
#include "areg/base/File.hpp"
#include "areg/base/Process.hpp"
#include "areg/trace/TraceScope.hpp"
#include "areg/trace/private/NELogConfig.hpp"
#include "areg/trace/private/ScopeController.hpp"
#include "areg/trace/private/ScopeNodes.hpp"

namespace
{
    String _getDefaultConfigFile( void )
    {
        return (Process::getInstance( ).getPath( ) + File::PATH_SEPARATOR + NEApplication::DEFAULT_TRACING_CONFIG_FILE);
    }
}

//////////////////////////////////////////////////////////////////////////
// LogConfiguration class implementation
//////////////////////////////////////////////////////////////////////////

LogConfiguration::LogConfiguration( ScopeController & scopeController )
    : mScopeController  ( scopeController )
    , mFilePath         ( )
    , mIsConfigured     ( false )
    , mProperties       ( NELogConfig::LOG_PROPERTY_COUNT )
{
}

bool LogConfiguration::loadConfig( const String & filePath )
{
    if ( mIsConfigured == false )
    {
        ASSERT( mFilePath.isEmpty( ) );
        String path = filePath.isEmpty( ) ? _getDefaultConfigFile( ) : filePath;
        path = File::getFileFullPath( File::normalizePath( path ) );
        File fileConfig( path, FileBase::FO_MODE_EXIST | FileBase::FO_MODE_READ | FileBase::FO_MODE_TEXT | FileBase::FO_MODE_SHARE_READ );
        fileConfig.open( );

        if ( loadConfig( fileConfig ) )
        {
            mFilePath = fileConfig.getName( );
        }
    }

    return mIsConfigured;
}

bool LogConfiguration::loadConfig( const FileBase & file )
{
    clearProperties( );
    if ( file.isOpened( ) )
    {
        file.moveToBegin( );

        const String & moduleName = Process::getInstance( ).getAppName( );
        String line;
        TraceProperty newProperty;
        while ( file.readLine( line ) > 0 )
        {
            if ( newProperty.parseProperty( line ) )
            {
                // add new entry if unique. otherwise, update existing.
                const TracePropertyKey & Key = newProperty.getKey( );
                if ( Key.isModuleKeySet( moduleName ) )
                {
                    updateProperty( newProperty );
                }

                newProperty.clearProperty( );
            }
        }

        mIsConfigured = true;
    }

    return mIsConfigured;
}

void LogConfiguration::unloadConfig( void )
{
    clearProperties( );
    mScopeController.clearConfigScopes( );
    mIsConfigured = false;
    mFilePath.clear( );
}

bool LogConfiguration::saveConfig( void ) const
{
    return saveConfig( mFilePath );
}

bool LogConfiguration::saveConfig( const String & filePath ) const
{
    constexpr unsigned int mode {   File::FO_MODE_WRITE     |
                                    File::FO_MODE_READ      |
                                    File::FO_MODE_TEXT      |
                                    File::FO_MODE_CREATE    |
                                    File::FO_MODE_SHARE_READ};

    String path = filePath.isEmpty( ) ? mFilePath : filePath;
    path = File::getFileFullPath( File::normalizePath( path ) );
    File fileConfig( path, mode );
    
    fileConfig.open( );

    return saveConfig( fileConfig );
}

bool LogConfiguration::saveConfig( FileBase & file ) const
{
    constexpr unsigned int mode{ FileBase::FO_MODE_READ | FileBase::FO_MODE_TEXT | FileBase::FO_MODE_EXIST | FileBase::FO_MODE_SHARE_READ };
    File fileConfig( mFilePath, mode);

    if ( fileConfig.open( ) == false )
    {
        return false;
    }

    if ( file.isOpened( ) == false )
    {
        return false;
    }

    if ( file.canWrite( ) == false )
    {
        return false;
    }

    PropertyList scopes;
    PropertyList properties;

    const String & moduleName = Process::getInstance( ).getAppName( );
    String line;
    TraceProperty newProperty;
    while ( fileConfig.readLine( line ) > 0 )
    {
        if ( newProperty.parseProperty( line ) )
        {
            // add new entry if unique. otherwise, update existing.
            const TracePropertyKey & Key = newProperty.getKey( );
            if ( Key.getLogConfig( ) != NELogConfig::eLogConfig::ConfigScope )
            {
                if ( Key.getLogConfig( ) == NELogConfig::eLogConfig::ConfigLogVersion )
                {
                    newProperty.setData( NELogConfig::SYNTAX_CMD_LOG_VERSION.data( ), NETrace::LOG_VERSION.data( ) );
                }
                else
                {
                    const TraceProperty & prop{ getProperty( Key.getLogConfig( ) ) };
                    if ( prop.isValid( ) )
                    {
                        newProperty.setProperty( prop.getProperty( ) );
                    }
                }

                properties.addIfUnique( newProperty, false );
            }
            else if ( Key.isExactModule( moduleName ) == false )
            {
                scopes.addIfUnique( newProperty, true );
            }

            newProperty.clearProperty( true );
        }
    }

    if ( newProperty.isEmpty( ) == false )
    {
        properties.add( newProperty );
    }

    fileConfig.close( );
    file.moveToBegin( );

    for ( unsigned int i = 0; i < properties.getSize( ); ++ i )
    {
        if ( properties[ i ].isValid( ) )
        {
            file.write( properties[ i ].makeConfigString( ) );
        }
    }

    for ( unsigned int i = 0; i < scopes.getSize( ); ++ i )
    {
        file.write( scopes[ i ].makeConfigString( ) );
    }

    const auto & scopeList = mScopeController.getScopeList( );
    unsigned int key{ 0 };
    TraceScope * scope = scopeList.resourceFirstKey( key );
    ScopeRoot root;
    while ( scope != nullptr )
    {
        root.addChildRecursive( *scope );
        scope = scopeList.resourceNextKey( key );
    }

    root.groupRecursive( );
    return (root.saveNodeConfig( file, String::EmptyString ) != 0);
}

void LogConfiguration::setDefaultValues( void )
{
    getVersion().parseProperty( NELogConfig::DEFAULT_LOG_VERSION.data() );
    getLayoutEnter().parseProperty( NELogConfig::DEFAULT_LOG_LAYOUT_ENTER.data( ) );
    getLayoutMessage().parseProperty( NELogConfig::DEFAULT_LOG_LAYOUT_MESSAGE.data( ) );
    getLayoutExit().parseProperty( NELogConfig::DEFAULT_LOG_LAYOUT_EXIT.data( ) );
    getDebugOutput().parseProperty( NELogConfig::DEFAULT_LOG_LAYOUT_DEBUG.data( ) );
    getStatus().parseProperty( NELogConfig::DEFAULT_LOG_ENABLE.data( ) );
    getAppendData().parseProperty( NELogConfig::DEFAULT_LOG_APPEND.data( ) );
    getLogFile().parseProperty( NELogConfig::DEFAULT_LOG_FILE.data( ) );
    getRemoteTcpEnable( ).parseProperty( NELogConfig::DEFAULT_LOG_REMOTE_TCP_ENABLE.data( ) );

    getStackSize().clearProperty( );
    getRemoteTcpHost().clearProperty( );
    getRemoteTcpPort().clearProperty( );
    getDatabaseHost().clearProperty( );
    getDatabaseName().clearProperty( );
    getDatabaseDriver().clearProperty( );
    getDatabaseUser( ).clearProperty( );
    getDatabasePassword().clearProperty( );
}

bool LogConfiguration::updateProperty( const TraceProperty & prop )
{
    bool result{ true };
    const TracePropertyKey &  Key     = prop.getKey( );
    const NELogConfig::eLogConfig whichConfig = Key.getLogConfig();
    
    switch ( whichConfig )
    {
    case NELogConfig::eLogConfig::ConfigLogVersion:
        {
            setVersion( prop );
        }
        break;

    case NELogConfig::eLogConfig::ConfigLogFile:
        {
            const TracePropertyKey & propKey = getLogFile().getKey( );
            if ( (propKey.isLocalKey( ) == false) || (propKey.getModule( ) == Key.getModule( )) )
            {
                setLogFile( prop );
                TracePropertyValue & value = getLogFile().getValue( );
                if ( value.isValid( ) == false )
                {
                    value = NELogConfig::DEFAULT_LOG_FILE_NAME.data( );
                }
            }
        }
        break;

    case NELogConfig::eLogConfig::ConfigLogRemoteTcpEnable: // fall through
    case NELogConfig::eLogConfig::ConfigLogRemoteTcpHost:   // fall through
    case NELogConfig::eLogConfig::ConfigLogRemoteTcpPort:   // fall through
    case NELogConfig::eLogConfig::ConfigLogDatabaseEnable:  // fall through
    case NELogConfig::eLogConfig::ConfigLogDatabaseDriver:  // fall through
    case NELogConfig::eLogConfig::ConfigLogDatabaseHost:    // fall through
    case NELogConfig::eLogConfig::ConfigLogDatabaseUser:    // fall through
    case NELogConfig::eLogConfig::ConfigLogDatabasePwd:     // fall through
    case NELogConfig::eLogConfig::ConfigLogDebug:           // fall through
    case NELogConfig::eLogConfig::ConfigLogAppend:          // fall through
    case NELogConfig::eLogConfig::ConfigLogStack:           // fall through
    case NELogConfig::eLogConfig::ConfigLogEnable:          // fall through
    case NELogConfig::eLogConfig::ConfigLogLayoutEnter:     // fall through
    case NELogConfig::eLogConfig::ConfigLogLayoutMessage:   // fall through
    case NELogConfig::eLogConfig::ConfigLogLayoutExit:
    {
        const TracePropertyKey & propKey = getProperty(whichConfig).getKey( );
        if ( (propKey.isLocalKey( ) == false) || (propKey.getModule( ) == Key.getModule( )) )
        {
            setProperty(whichConfig, prop);
        }
    }
    break;

    case NELogConfig::eLogConfig::ConfigScope:
        {
            mScopeController.configureScopes( prop );
        }
        break;

    default:
        result = false;
        ASSERT( false );
        break;  // do nothing
    }
    
    return result;
}
