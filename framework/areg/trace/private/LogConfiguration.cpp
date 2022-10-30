/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/trace/private/LogConfiguration.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Interface of log configuration
 ************************************************************************/

#include "areg/trace/private/LogConfiguration.hpp"

#include "areg/trace/private/NELogConfig.hpp"

//////////////////////////////////////////////////////////////////////////
// LogConfiguration class implementation
//////////////////////////////////////////////////////////////////////////

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

    getStackSize().clearProperty( );
    getRemoteHost().clearProperty( );
    getRemotePort().clearProperty( );
    getDatabaseHost().clearProperty( );
    getDatabaseName().clearProperty( );
    getDatabaseDriver().clearProperty( );
    getDatabaseUser( ).clearProperty( );
    getDatabasePassword().clearProperty( );
}

bool LogConfiguration::updateProperty( const TraceProperty & prop )
{
    bool result = false;
    const TracePropertyKey &  Key     = prop.getKey( );
    const NELogConfig::eLogConfig whichConfig = Key.getLogConfig();
    
    switch ( whichConfig )
    {
    case NELogConfig::eLogConfig::ConfigLogVersion:
        {
            setVersion( prop );
            result = true;
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

            result = true;
        }
        break;

    case NELogConfig::eLogConfig::ConfigLogRemoteHost:      // fall through
    case NELogConfig::eLogConfig::ConfigLogRemotePort:      // fall through
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
        result = true;
    }
    break;

    case NELogConfig::eLogConfig::ConfigScope:              // fall through
    default:
        break;  // do nothing
    }
    
    return result;
}

