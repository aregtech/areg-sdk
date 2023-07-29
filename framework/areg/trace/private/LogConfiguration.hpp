#ifndef AREG_TRACE_PRIVATE_LOGCONFIGURATION_HPP
#define AREG_TRACE_PRIVATE_LOGCONFIGURATION_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/trace/private/LogConfiguration.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Interface of log configuration
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/base/TEArrayList.hpp"
#include "areg/trace/private/NELogConfig.hpp"
#include "areg/trace/private/TraceProperty.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class ScopeController;
class FileBase;

//////////////////////////////////////////////////////////////////////////
// LogConfiguration class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Trace configuration is an interface of configuration container.
 *          The inter has methods to get values of configured properties.
 *          All properties are read only and cannot be modified
 **/
class LogConfiguration
{
    using PropertyList = TEArrayList< TraceProperty >;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Protected default constructor.
     **/
    LogConfiguration( ScopeController & scopeController );

    /**
     * \brief   Protected destructor.
     **/
    ~LogConfiguration( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns the path of the configuration file.
     **/
    inline const String & getConfigFile( void ) const;

    /**
     * \brief   Returns true if the logs are configured.
     **/
    inline bool isConfigured( void ) const;

    /**
     * \brief   Returns true if logging is enabled.
     **/
    inline bool isLoggingEnabled( void ) const;

    /**
     * \brief   Returns true if logging via network is enabled.
     **/
    inline bool isNetLoggingEnabled( void ) const;

    /**
     * \brief   Returns true if logging in the file is enabled.
     **/
    inline bool isFileLoggingEnabled( void ) const;

    /**
     * \brief   Returns true if logging in database is enabled.
     *          NOTE: The database logging is not implemented yet.
     **/
    inline bool isDatabasLoggingEnabled( void ) const;

    /**
     * \brief   Returns true if logging on debug output window (or console) is enabled.
     **/
    inline bool isDebugOutputEnabled( void ) const;

    /**
     * \brief   Gets and sets the specified property object.
     * \param   whichProperty   The configuration property to return.
     **/
    inline const TraceProperty & getProperty( NELogConfig::eLogConfig whichProperty ) const;
    inline TraceProperty & getProperty( NELogConfig::eLogConfig whichProperty );
    inline void setProperty( NELogConfig::eLogConfig whichProperty, const TraceProperty & newProp );

    /**
     * \brief   Gets and set the property value of logging version.
     **/
    inline const TraceProperty & getVersion( void ) const;
    inline TraceProperty & getVersion( void );
    inline void setVersion( const TraceProperty & prop );

    /**
     * \brief   Gets and set property value of layout format of scope enter.
     **/
    inline const TraceProperty & getLayoutEnter( void ) const;
    inline TraceProperty & getLayoutEnter( void );
    inline void setLayoutEnter( const TraceProperty & prop);

    /**
     * \brief   Gets and set property value of layout format form message text output.
     **/
    inline const TraceProperty & getLayoutMessage( void ) const;
    inline TraceProperty & getLayoutMessage( void );
    inline void setLayoutMessage( const TraceProperty & prop );

    /**
     * \brief   Gets and set property value of layout format of scope exit.
     **/
    inline const TraceProperty & getLayoutExit( void ) const;
    inline TraceProperty & getLayoutExit( void );
    inline void setLayoutExit( const TraceProperty & prop );

    /**
     * \brief   Gets and set property value of stack size.
     **/
    inline const TraceProperty & getStackSize( void ) const;
    inline TraceProperty & getStackSize( void );
    inline void setStackSize( const TraceProperty & prop );

    /**
     * \brief   Gets and set property value of debug output settings.
     **/
    inline const TraceProperty & getDebugOutput( void ) const;
    inline TraceProperty & getDebugOutput( void );
    inline void setDebugOutput( const TraceProperty & prop );

    /**
     * \brief   Gets and set property value of logging status (enabled / disabled).
     **/
    inline const TraceProperty & getStatus( void ) const;
    inline TraceProperty & getStatus( void );
    inline void setStatus( const TraceProperty & prop );

    /**
     * \brief   Gets and set property value of append data status.
     **/
    inline const TraceProperty & getAppendData( void ) const;
    inline TraceProperty & getAppendData( void );
    inline void setAppendData( const TraceProperty & prop );

    /**
     * \brief   Gets and set property value of file logging setting.
     **/
    inline const TraceProperty & getLogFile( void ) const;
    inline TraceProperty & getLogFile( void );
    inline void setLogFile( const TraceProperty & prop );

    /**
     * \brief   Gets and set property value of remote logging enable setting.
     **/
    inline const TraceProperty & getRemoteTcpEnable( void ) const;
    inline TraceProperty & getRemoteTcpEnable( void );
    inline void setRemoteTcpEnable( const TraceProperty & prop );

    /**
     * \brief   Gets and set property value of remote host name logging setting.
     **/
    inline const TraceProperty & getRemoteTcpHost( void ) const;
    inline TraceProperty & getRemoteTcpHost( void );
    inline void setRemoteTcpHost( const TraceProperty & prop );

    /**
     * \brief   Gets and set property value of remote host port logging setting.
     **/
    inline const TraceProperty & getRemoteTcpPort( void ) const;
    inline TraceProperty & getRemoteTcpPort( void );
    inline void setRemoteTcpPort( const TraceProperty & prop );

    /**
     * \brief   Gets and set property value of database logging enable / disable setting.
     **/
    inline const TraceProperty & getDatabaseEnable( void ) const;
    inline TraceProperty & getDatabaseEnable( void );
    inline void setDatabaseEnable( const TraceProperty & prop );

    /**
     * \brief   Gets and set property value of database host name logging setting.
     **/
    inline const TraceProperty & getDatabaseHost( void ) const;
    inline TraceProperty & getDatabaseHost( void );
    inline void setDatabaseHost( const TraceProperty & prop );

    /**
     * \brief   Gets and set property value of database name logging settings.
     **/
    inline const TraceProperty & getDatabaseName( void ) const;
    inline TraceProperty & getDatabaseName( void );
    inline void setDatabaseName( const TraceProperty & prop );

    /**
     * \brief   Gets and set property value of database driver name logging settings.
     **/
    inline const TraceProperty & getDatabaseDriver( void ) const;
    inline TraceProperty & getDatabaseDriver( void );
    inline void setDatabaseDriver( const TraceProperty & prop );

    /**
     * \brief   Gets and set property value of database user name logging settings.
     **/
    inline const TraceProperty & getDatabaseUser( void ) const;
    inline TraceProperty & getDatabaseUser( void );
    inline void setDatabaseUser( const TraceProperty & prop );

    /**
     * \brief   Gets and set property value of database user password logging settings.
     **/
    inline const TraceProperty & getDatabasePassword( void ) const;
    inline TraceProperty & getDatabasePassword( void );
    inline void setDatabasePassword( const TraceProperty & prop );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Reads logging configuration from the specified file path
     *          and activates the logging scopes.
     * \param   filePath    The relative or absolute path to the log configuration file.
     * \return  Returns true if logging is configured.
     **/
    bool loadConfig( const String & filePath );

    /**
     * \brief   Reads logging configuration from the specified file
     *          and activates the logging scopes.
     * \param   file    The opened file object of the log configuration file.
     * \return  Returns true if logging is configured.
     **/
    bool loadConfig( FileBase & file );

    /**
     * \brief   Saves the configuration in the file.
     *          It modifies only the part relevant with current executable.
     *          All existing data remains unchanged.
     *          The file path should already exist, i.e. the configuration
     *          should be already loaded.
     * \return  Returns true if operation succeeded.
     **/
    bool saveConfig( void ) const;

    /**
     * \brief   Saves the configuration in the file with specified path.
     *          If the path name is empty, it uses the default log configuration path.
     *          It modifies only the part relevant with current executable.
     *          All existing data remains unchanged.
     * \return  Returns true if operation succeeded.
     **/
    bool saveConfig( const String & filePath ) const;
    
    /**
     * \brief   Saves the configuration in the file opened for writing in the text mode.
     *          It modifies only the part relevant with current executable.
     *          All existing data remains unchanged.
     * \return  Returns true if operation succeeded.
     **/
    bool saveConfig( FileBase & file ) const;

    /**
     * \brief   Sets the logging configuration default values.
     **/
    void setDefaultValues( void );

    /**
     * \brief   Updates the property values.
     * \param   prop    The new property value to set. The property 
     *                  as well contains information of target log configuration
     *                  entry to update.
     * \return  Returns true, if succeed to update.
     **/
    bool updateProperty( const TraceProperty & prop );

    /**
     * \brief   Clear all logging configuration properties
     **/
    inline void clearProperties( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    //!< The instance of scope controller to setup scope priorities.
    ScopeController &   mScopeController;

    //!< The path of the log configuration file.
    String              mFilePath;
    
    //!< Flag, indicating whether the logs are configured or not.
    bool                mIsConfigured;

    //!< The list of log properties that can be configured.
    PropertyList       mProperties;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    LogConfiguration( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( LogConfiguration );
};

//////////////////////////////////////////////////////////////////////////
// LogConfiguration inline methods
//////////////////////////////////////////////////////////////////////////

inline void LogConfiguration::clearProperties( void )
{
    for ( unsigned int i = 0; i < mProperties.getSize(); ++ i)
    {
        mProperties[ i ].clearProperty( );
    }
}

inline const String & LogConfiguration::getConfigFile( void ) const
{
    return mFilePath;
}

inline bool LogConfiguration::isConfigured( void ) const
{
    return mIsConfigured;
}

inline bool LogConfiguration::isLoggingEnabled( void ) const
{
    const TraceProperty & prop = getStatus( );
    return (prop.isValid() ? static_cast<bool>(prop.getValue()) : NELogConfig::DEFAULT_LOG_ENABLED);
}

inline bool LogConfiguration::isNetLoggingEnabled( void ) const
{
    const TraceProperty & prop = getRemoteTcpEnable();
    return (prop.isValid( ) ? static_cast<bool>(prop.getValue( )) : NELogConfig::DEFAULT_REMOTE_ENABLED);
}

inline bool LogConfiguration::isFileLoggingEnabled( void ) const
{
    return getLogFile( ).isValid( );
}

inline bool LogConfiguration::isDatabasLoggingEnabled( void ) const
{
    const TraceProperty & prop = getDatabaseEnable( );
    return (prop.isValid( ) ? static_cast<bool>(prop.getValue( )) : NELogConfig::DEFAULT_DATABASE_ENABLED);
}

inline bool LogConfiguration::isDebugOutputEnabled( void ) const
{
    const TraceProperty & prop = getDebugOutput( );
    return (prop.isValid( ) && static_cast<bool>(prop.getValue( )));
}

inline const TraceProperty & LogConfiguration::getProperty( NELogConfig::eLogConfig whichProperty ) const
{
    return mProperties[ static_cast<int>(whichProperty)];
}

inline TraceProperty & LogConfiguration::getProperty( NELogConfig::eLogConfig whichProperty )
{
    return mProperties[static_cast<int>(whichProperty)];
}

inline void LogConfiguration::setProperty( NELogConfig::eLogConfig whichProperty, const TraceProperty & newProp )
{
    mProperties[static_cast<int>(whichProperty)] = newProp;
}

inline const TraceProperty & LogConfiguration::getVersion( void ) const
{
    return mProperties[ static_cast<int>(NELogConfig::eLogConfig::ConfigLogVersion)];
}

inline TraceProperty & LogConfiguration::getVersion( void )
{
    return mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogVersion)];
}

inline void LogConfiguration::setVersion( const TraceProperty & prop )
{
    mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogVersion)] = prop;
}

inline const TraceProperty & LogConfiguration::getLayoutEnter( void ) const
{
    return mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogLayoutEnter)];
}

inline TraceProperty & LogConfiguration::getLayoutEnter( void )
{
    return mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogLayoutEnter)];
}

inline void LogConfiguration::setLayoutEnter( const TraceProperty & prop )
{
    mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogLayoutEnter)] = prop;
}

inline const TraceProperty & LogConfiguration::getLayoutMessage( void ) const
{
    return mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogLayoutMessage)];
}

inline TraceProperty & LogConfiguration::getLayoutMessage( void )
{
    return mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogLayoutMessage)];
}

inline void LogConfiguration::setLayoutMessage( const TraceProperty & prop )
{
    mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogLayoutMessage)] = prop;
}

inline const TraceProperty & LogConfiguration::getLayoutExit( void ) const
{
    return mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogLayoutExit)];
}

inline TraceProperty & LogConfiguration::getLayoutExit( void )
{
    return mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogLayoutExit)];
}

inline void LogConfiguration::setLayoutExit( const TraceProperty & prop )
{
    mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogLayoutExit)] = prop;
}

inline const TraceProperty & LogConfiguration::getStackSize( void ) const
{
    return mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogStack)];
}

inline TraceProperty & LogConfiguration::getStackSize( void )
{
    return mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogStack)];
}

inline void LogConfiguration::setStackSize( const TraceProperty & prop )
{
    mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogStack)] = prop;
}

inline const TraceProperty & LogConfiguration::getDebugOutput( void ) const
{
    return mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogDebug)];
}

inline TraceProperty & LogConfiguration::getDebugOutput( void )
{
    return mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogDebug)];
}

inline void LogConfiguration::setDebugOutput( const TraceProperty & prop )
{
    mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogDebug)] = prop;
}

inline const TraceProperty & LogConfiguration::getStatus( void ) const
{
    return mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogEnable)];
}

inline TraceProperty & LogConfiguration::getStatus( void )
{
    return mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogEnable)];
}

inline void LogConfiguration::setStatus( const TraceProperty & prop )
{
    mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogEnable)] = prop;
}

inline const TraceProperty & LogConfiguration::getAppendData( void ) const
{
    return mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogAppend)];
}

inline TraceProperty & LogConfiguration::getAppendData( void )
{
    return mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogAppend)];
}

inline void LogConfiguration::setAppendData( const TraceProperty & prop )
{
    mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogAppend)] = prop;
}

inline const TraceProperty & LogConfiguration::getLogFile( void ) const
{
    return mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogFile)];
}

inline TraceProperty & LogConfiguration::getLogFile( void )
{
    return mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogFile)];
}

inline void LogConfiguration::setLogFile( const TraceProperty & prop )
{
    mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogFile)] = prop;
}

inline const TraceProperty & LogConfiguration::getRemoteTcpEnable( void ) const
{
    return mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogRemoteTcpEnable)];
}

inline TraceProperty & LogConfiguration::getRemoteTcpEnable( void )
{
    return mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogRemoteTcpEnable)];
}

inline void LogConfiguration::setRemoteTcpEnable( const TraceProperty & prop )
{
    mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogRemoteTcpEnable)] = prop;
}

inline const TraceProperty & LogConfiguration::getRemoteTcpHost( void ) const
{
    return mProperties[ static_cast<int>(NELogConfig::eLogConfig::ConfigLogRemoteTcpHost) ];
}

inline TraceProperty & LogConfiguration::getRemoteTcpHost( void )
{
    return mProperties[ static_cast<int>(NELogConfig::eLogConfig::ConfigLogRemoteTcpHost) ];
}

inline void LogConfiguration::setRemoteTcpHost( const TraceProperty & prop )
{
    mProperties[ static_cast<int>(NELogConfig::eLogConfig::ConfigLogRemoteTcpHost) ] = prop;
}

inline const TraceProperty & LogConfiguration::getRemoteTcpPort( void ) const
{
    return mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogRemoteTcpPort)];
}

inline TraceProperty & LogConfiguration::getRemoteTcpPort( void )
{
    return mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogRemoteTcpPort)];
}

inline void LogConfiguration::setRemoteTcpPort( const TraceProperty & prop )
{
    mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogRemoteTcpPort)] = prop;
}

inline const TraceProperty & LogConfiguration::getDatabaseEnable( void ) const
{
    return mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogDatabaseEnable)];
}

inline TraceProperty & LogConfiguration::getDatabaseEnable( void )
{
    return mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogDatabaseEnable)];
}

inline void LogConfiguration::setDatabaseEnable( const TraceProperty & prop )
{
    mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogDatabaseEnable)] = prop;
}

inline const TraceProperty & LogConfiguration::getDatabaseHost( void ) const
{
    return mProperties[ static_cast<int>(NELogConfig::eLogConfig::ConfigLogDatabaseHost) ];
}

inline TraceProperty & LogConfiguration::getDatabaseHost( void )
{
    return mProperties[ static_cast<int>(NELogConfig::eLogConfig::ConfigLogDatabaseHost) ];
}

inline void LogConfiguration::setDatabaseHost( const TraceProperty & prop )
{
    mProperties[ static_cast<int>(NELogConfig::eLogConfig::ConfigLogDatabaseHost) ] = prop;
}

inline const TraceProperty & LogConfiguration::getDatabaseName( void ) const
{
    return mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogDatabaseName)];
}

inline TraceProperty & LogConfiguration::getDatabaseName( void )
{
    return mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogDatabaseName)];
}

inline void LogConfiguration::setDatabaseName( const TraceProperty & prop )
{
    mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogDatabaseName)] = prop;
}

inline const TraceProperty & LogConfiguration::getDatabaseDriver( void ) const
{
    return mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogDatabaseDriver)];
}

inline TraceProperty & LogConfiguration::getDatabaseDriver( void )
{
    return mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogDatabaseDriver)];
}

inline void LogConfiguration::setDatabaseDriver( const TraceProperty & prop )
{
    mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogDatabaseDriver)] = prop;
}

inline const TraceProperty & LogConfiguration::getDatabaseUser( void ) const
{
    return mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogDatabaseUser)];
}

inline TraceProperty & LogConfiguration::getDatabaseUser( void )
{
    return mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogDatabaseUser)];
}

inline void LogConfiguration::setDatabaseUser( const TraceProperty & prop )
{
    mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogDatabaseUser)] = prop;
}

inline const TraceProperty & LogConfiguration::getDatabasePassword( void ) const
{
    return mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogDatabasePwd)];
}

inline TraceProperty & LogConfiguration::getDatabasePassword( void )
{
    return mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogDatabasePwd)];
}

inline void LogConfiguration::setDatabasePassword( const TraceProperty & prop )
{
    mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogDatabasePwd)] = prop;
}

#endif  // AREG_TRACE_PRIVATE_LOGCONFIGURATION_HPP
