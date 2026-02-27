#ifndef AREG_LOGGING_LOGCONFIGURATION_HPP
#define AREG_LOGGING_LOGCONFIGURATION_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/logging/LogConfiguration.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Interface of log configuration
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/base/SocketDefs.hpp"
#include "areg/base/Version.hpp"
#include "areg/persist/PersistenceDefs.hpp"
#include "areg/persist/Property.hpp"
#include "areg/logging/LoggingDefs.hpp"

#include <vector>

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg
{
    class ScopeController;
}

namespace areg
{
    //////////////////////////////////////////////////////////////////////////
    // LogConfiguration class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Logging configuration is an interface of configuration container.
     *          The inter has methods to get values of configured properties.
     *          All properties are read only and cannot be modified
     **/
    class AREG_API LogConfiguration
    {

    //////////////////////////////////////////////////////////////////////////
    // Constructor / Destructor. Protected
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief  Constructor / Destructor.
         **/
        LogConfiguration() = default;
        ~LogConfiguration() = default;

    //////////////////////////////////////////////////////////////////////////
    // Attributes
    //////////////////////////////////////////////////////////////////////////
    public:

        /**
         * \brief   Returns true if logging is enabled.
         **/
        bool isLoggingEnabled() const;

        /**
         * \brief   Returns true if logging via network is enabled.
         **/
        bool isRemoteLoggingEnabled() const;

        /**
         * \brief   Returns true if logging in the file is enabled.
         **/
        bool isFileLoggingEnabled() const;

        /**
         * \brief   Returns true if logging in database is enabled.
         *          NOTE: The database logging is not implemented yet.
         **/
        bool isDatabaseLoggingEnabled() const;

        /**
         * \brief   Returns true if logging on debug output window (or console) is enabled.
         **/
        bool isDebugOutputLoggingEnabled() const;

        /**
         * \brief   Sets logging enable flag for the specified log type.
         * \param   logType     The logging type, which enabled flag is set.
         * \param   isEnabled   The flag, indicating whether the logging of specified type
         *                      is enabled or disabled. If true, the logging for specified type
         *                      is enabled.
         **/
        void setLogEnabled(LogTarget logType, bool isEnabled);

        /**
         * \brief   Gets and sets the property value of logging version.
         **/
        Version getVersion() const;

        /**
         * \brief   Gets and sets property value of layout format of scope enter.
         **/
        String getLayoutEnter() const;
        void setLayoutEnter( const String & prop );

        /**
         * \brief   Gets and sets property value of layout format form message text output.
         **/
        String getLayoutMessage() const;
        void setLayoutMessage( const String & prop );

        /**
         * \brief   Gets and sets property value of layout format of scope exit.
         **/
        String getLayoutExit() const;
        void setLayoutExit( const String & prop );

        /**
         * \brief   Gets and sets property value of stack size.
         **/
        uint32_t getStackSize() const;
        void setStackSize( uint32_t prop );

        /**
         * \brief   Gets and sets property value of logging status (enabled / disabled).
         **/
        bool getStatus() const;
        void setStatus( bool prop );

        /**
         * \brief   Gets and sets property value of append data status.
         **/
        bool getAppendData() const;
        void setAppendData( bool prop );

        /**
         * \brief   Gets and sets property value of file logging setting.
         **/
        String getLogFile() const;
        void setLogFile( const String & prop );

        /**
         * \brief   Gets and sets property value of remote logging enable setting.
         **/
        bool getRemoteTcpEnable() const;
        void setRemoteTcpEnable( bool prop, bool isTemporary = false);

        /**
         * \brief   Gets and sets property value of remote host name logging setting.
         **/
        String getRemoteTcpAddress() const;
        void setRemoteTcpAddress( const String & prop, bool isTemporary = false);

        /**
         * \brief   Gets and sets property value of remote host port logging setting.
         **/
        uint16_t getRemoteTcpPort() const;
        void setRemoteTcpPort( uint16_t prop, bool isTemporary = false);

        /**
         * \brief   Gets and sets property value of database logging enable / disable setting.
         **/
        bool getDatabaseEnable() const;
        void setDatabaseEnable( bool prop, bool isTemporary = false);

        /**
         * \brief   Gets and sets module (process) specific log scopes.
         **/
        uint32_t getModuleScopes(std::vector<Property>& scopeList);
        void setModuleScopes(const std::vector<Property>& scopeList);

        /**
         * \brief   Gets and sets the name of database engine such as `sqlite`.
         **/
        String getDatabaseEngine() const;
        void setDatabaseEngine(const String & dbEngine, bool isTemporary = false);

        /**
         * \brief   Gets and sets the full path of the database file.
         *          When gets, it adds database file name to the location.
         *          Wehn sets, it splits file path to 2 parts: database name and database location.
         **/
        String getDatabaseFullPath() const;
        void setDatabaseFullPath(const String & dbFullPath, bool isTemporary = false);

        /**
         * \brief   Gets and sets the database name like "SQLite" or "MySQL", etc.
         **/
        String getDatabaseName() const;
        void setDatabaseName(const String & dbName, bool isTemporary = false);

        /**
         * \brief   Gets and sets the database location. The location can be full or relative file path, or an URI.
         **/
        String getDatabaseLocation() const;
        void setDatabaseLocation(const String& dbLocation, bool isTemporary = false);

        /**
         * \brief   Gets and sets the database driver, if needed.
         **/
        String getDatabaseDriver() const;
        void setDatabaseDriver(const String& dbDriver, bool isTemporary = false);

        /**
         * \brief   Gets and sets the database service address that contains IP-address and port number.
         **/
        SocketAddress getDatabaseAddress() const;
        void setDatabaseAddress(const SocketAddress& dbAddress, bool isTemporary = false);
        void setDatabaseAddress(const String& dbAddress, uint16_t dbPort, bool isTemporary = false);

        /**
         * \brief   Gets and sets the database user name and user password.
         **/
        UserData getDatabaseUser() const;
        void setDatabaseUser(const UserData& dbUser, bool isTemporary = false);
        void setDatabaseUser(const String& dbUserName, const String& dbUserPassword, bool isTemporary = false);

        /**
         * \brief   Saves the configuration in the current config file.
         **/
        void saveConfiguration();

        /**
         * \brief   Updates the list of scopes in the application configuration by setting the current state of scopes in configuration.
         * \param   scopeController     The instance of the scope controller to read states of the scopes.
         **/
        void updateScopeConfiguration(const ScopeController & scopeController) const;

    //////////////////////////////////////////////////////////////////////////
    // Forbidden calls
    //////////////////////////////////////////////////////////////////////////
    private:
        AREG_NOCOPY_NOMOVE( LogConfiguration );
    };

    //////////////////////////////////////////////////////////////////////////
    // LogConfiguration inline methods
    //////////////////////////////////////////////////////////////////////////


} // namespace areg
#endif  // AREG_LOGGING_LOGCONFIGURATION_HPP
