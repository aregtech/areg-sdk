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
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/trace/private/LogConfiguration.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Interface of log configuration
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/trace/private/NELogConfig.hpp"
#include "areg/trace/private/TraceProperty.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/

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
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Protected default constructor.
     **/
    LogConfiguration( void ) = default;

    /**
     * \brief   Protected destructor.
     **/
    ~LogConfiguration( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operations and attributes
//////////////////////////////////////////////////////////////////////////
public:

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
     * \brief   Gets and set property value of file logging settings.
     **/
    inline const TraceProperty & getLogFile( void ) const;
    inline TraceProperty & getLogFile( void );
    inline void setLogFile( const TraceProperty & prop );

    /**
     * \brief   Gets and set property value of remote host name logging settings.
     **/
    inline const TraceProperty & getRemoteHost( void ) const;
    inline TraceProperty & getRemoteHost( void );
    inline void setRemoteHost( const TraceProperty & prop );

    /**
     * \brief   Gets and set property value of remote host port logging settings.
     **/
    inline const TraceProperty & getRemotePort( void ) const;
    inline TraceProperty & getRemotePort( void );
    inline void setRemotePort( const TraceProperty & prop );

    /**
     * \brief   Gets and set property value of database host name logging settings.
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
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    TraceProperty       mProperties[NELogConfig::LOG_PROPERTY_COUNT];

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( LogConfiguration );
};

//////////////////////////////////////////////////////////////////////////
// LogConfiguration inline methods
//////////////////////////////////////////////////////////////////////////

inline void LogConfiguration::clearProperties( void )
{
    for ( TraceProperty & prop : mProperties )
    {
        prop.clearProperty();
    }
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

inline const TraceProperty & LogConfiguration::getRemoteHost( void ) const
{
    return mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogRemoteHost)];
}

inline TraceProperty & LogConfiguration::getRemoteHost( void )
{
    return mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogRemoteHost)];
}

inline void LogConfiguration::setRemoteHost( const TraceProperty & prop )
{
    mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogRemoteHost)] = prop;
}

inline const TraceProperty & LogConfiguration::getRemotePort( void ) const
{
    return mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogRemotePort)];
}

inline TraceProperty & LogConfiguration::getRemotePort( void )
{
    return mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogRemotePort)];
}

inline void LogConfiguration::setRemotePort( const TraceProperty & prop )
{
    mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogRemotePort)] = prop;
}

inline const TraceProperty & LogConfiguration::getDatabaseHost( void ) const
{
    return mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogDatabaseHost)];
}

inline TraceProperty & LogConfiguration::getDatabaseHost( void )
{
    return mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogDatabaseHost)];
}

inline void LogConfiguration::setDatabaseHost( const TraceProperty & prop )
{
    mProperties[static_cast<int>(NELogConfig::eLogConfig::ConfigLogDatabaseHost)] = prop;
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
