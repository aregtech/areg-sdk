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
#include "areg/trace/private/NELogConfig.hpp"
#include "areg/trace/private/ScopeController.hpp"


namespace
{
    String _getDefaultConfigFile( void )
    {
        return (Process::getInstance( ).getPath( ) + File::getPathSeparator( ) + NEApplication::DEFAULT_TRACING_CONFIG_FILE);
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
            mFilePath = fileConfig.getFileFullPath( );
        }
    }

    return mIsConfigured;
}

bool LogConfiguration::loadConfig( FileBase & file )
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

bool LogConfiguration::saveConfig( void ) const
{
    return saveConfig( mFilePath );
}

bool LogConfiguration::saveConfig( const String & filePath ) const
{
    String path = filePath.isEmpty( ) ? _getDefaultConfigFile( ) : filePath;
    path = File::getFileFullPath( File::normalizePath( path ) );
    File fileConfig( path, FileBase::FO_MODE_EXIST | FileBase::FO_MODE_READ | FileBase::FO_MODE_TEXT | FileBase::FO_MODE_SHARE_READ );
    fileConfig.open( );

    return saveConfig( fileConfig );
}

bool LogConfiguration::saveConfig( FileBase & file ) const
{
    bool result{ false };

    if ( file.isOpened( ) && file.canWrite( ) )
    {
        file.moveToBegin( );
        PropertyList properties( mProperties );
        PropertyList scopes;

        const String & moduleName = Process::getInstance( ).getAppName( );
        String line;
        TraceProperty newProperty;
        while ( file.readLine( line ) > 0 )
        {
            if ( newProperty.parseProperty( line ) )
            {
                // add new entry if unique. otherwise, update existing.
                const TracePropertyKey & Key = newProperty.getKey( );
                if ( Key.getLogConfig( ) != NELogConfig::eLogConfig::ConfigScope )
                {
                    properties.addIfUnique( newProperty, false );
                }
                else if ( Key.isModuleKeySet( moduleName ) == false)
                {
                    scopes.addIfUnique( newProperty, true );
                }

                newProperty.clearProperty( );
            }
        }

        file.moveToBegin( );
        for ( unsigned int i = 0; i < properties.getSize( ); ++ i )
        {
            file.writeString( properties[ i ].makeString( ) );
        }

        for ( unsigned int i = 0; i < properties.getSize( ); ++ i )
        {
            file.writeString( scopes[ i ].makeString( ) );
        }

        // The scopes controller should save scopes here.
    }

    return result;
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

