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
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Interface of log configuration
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/base/Version.hpp"
#include "areg/persist/NEPersistence.hpp"
#include "areg/persist/Property.hpp"
#include "areg/trace/NETrace.hpp"

#include <vector>

/************************************************************************
 * Dependencies
 ************************************************************************/
class ScopeController;

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
     * \brief  Constructor / Destructor.
     **/
    LogConfiguration( void ) = default;
    ~LogConfiguration( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns true if logging is enabled.
     **/
    bool isLoggingEnabled( void ) const;

    /**
     * \brief   Returns true if logging via network is enabled.
     **/
    bool isRemoteLoggingEnabled( void ) const;

    /**
     * \brief   Returns true if logging in the file is enabled.
     **/
    bool isFileLoggingEnabled( void ) const;

    /**
     * \brief   Returns true if logging in database is enabled.
     *          NOTE: The database logging is not implemented yet.
     **/
    bool isDatabaseLoggingEnabled( void ) const;

    /**
     * \brief   Returns true if logging on debug output window (or console) is enabled.
     **/
    bool isDebugOutputLoggingEnabled( void ) const;

    /**
     * \brief   Sets logging enable flag for the specified log type.
     * \param   logType     The logging type, which enabled flag is set.
     * \param   isEnabled   The flag, indicating whether the logging of specified type
     *                      is enabled or disabled. If true, the logging for specified type
     *                      is enabled.
     **/
    void setLogEnabled(NETrace::eLogingTypes logType, bool isEnabled);

    /**
     * \brief   Gets and sets the property value of logging version.
     **/
    Version getVersion( void ) const;

    /**
     * \brief   Gets and sets property value of layout format of scope enter.
     **/
    String getLayoutEnter( void ) const;
    void setLayoutEnter( const String & prop );

    /**
     * \brief   Gets and sets property value of layout format form message text output.
     **/
    String getLayoutMessage( void ) const;
    void setLayoutMessage( const String & prop );

    /**
     * \brief   Gets and sets property value of layout format of scope exit.
     **/
    String getLayoutExit( void ) const;
    void setLayoutExit( const String & prop );

    /**
     * \brief   Gets and sets property value of stack size.
     **/
    uint32_t getStackSize( void ) const;
    void setStackSize( uint32_t prop );

    /**
     * \brief   Gets and sets property value of logging status (enabled / disabled).
     **/
    bool getStatus( void ) const;
    void setStatus( bool prop );

    /**
     * \brief   Gets and sets property value of append data status.
     **/
    bool getAppendData( void ) const;
    void setAppendData( bool prop );

    /**
     * \brief   Gets and sets property value of file logging setting.
     **/
    String getLogFile( void ) const;
    void setLogFile( const String & prop );

    /**
     * \brief   Gets and sets property value of remote logging enable setting.
     **/
    bool getRemoteTcpEnable( void ) const;
    void setRemoteTcpEnable( bool prop, bool isTemporary = false);

    /**
     * \brief   Gets and sets property value of remote host name logging setting.
     **/
    String getRemoteTcpAddress( void ) const;
    void setRemoteTcpAddress( const String & prop, bool isTemporary = false);

    /**
     * \brief   Gets and sets property value of remote host port logging setting.
     **/
    uint16_t getRemoteTcpPort( void ) const;
    void setRemoteTcpPort( uint16_t prop, bool isTemporary = false);

    /**
     * \brief   Gets and sets property value of database logging enable / disable setting.
     **/
    bool getDatabaseEnable( void ) const;
    void setDatabaseEnable( bool prop, bool isTemporary = false);

    /**
     * \brief   Gets and sets module (process) specific log scopes.
     **/
    uint32_t getModuleScopes(std::vector<Property>& scopeList);
    void setModuleScopes(const std::vector<Property>& scopeList);

    /**
     * \brief   Saves the configuration in the current config file.
     **/
    void saveConfiguration(void);

    /**
     * \brief   Updates the list of scopes in the application configuration by setting the current state of scopes in configuration.
     * \param   scopeController     The instance of the scope controller to read states of the scopes.
     **/
    void updateScopeConfiguration(const ScopeController & scopeController) const;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( LogConfiguration );
};

//////////////////////////////////////////////////////////////////////////
// LogConfiguration inline methods
//////////////////////////////////////////////////////////////////////////


#endif  // AREG_TRACE_PRIVATE_LOGCONFIGURATION_HPP
