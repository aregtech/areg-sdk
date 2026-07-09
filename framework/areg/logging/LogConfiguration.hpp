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
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/logging/LogConfiguration.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Interface of log configuration
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"

#include "areg/base/SocketDefs.hpp"
#include "areg/base/Version.hpp"
#include "areg/persist/PersistenceDefs.hpp"
#include "areg/persist/Property.hpp"
#include "areg/logging/LoggingDefs.hpp"

#include <vector>

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {
    class ConfigManager;
    class ScopeController;
} // namespace areg

namespace areg {
//////////////////////////////////////////////////////////////////////////
// LogConfiguration class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Configuration interface for logging system. Provides read-only access to logging settings
 *          including enabled targets (file, remote, database), layout formats, and scope priorities.
 **/
class AREG_API LogConfiguration
{

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
public:
    LogConfiguration();

    ~LogConfiguration() = default;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns true if logging is enabled.
     **/
    [[nodiscard]]
    bool is_logging_enabled() const noexcept;

    /**
     * \brief   Returns true if remote network logging is enabled.
     **/
    [[nodiscard]]
    bool is_remote_logging_enabled() const noexcept;

    /**
     * \brief   Returns true if file logging is enabled.
     **/
    [[nodiscard]]
    bool is_file_logging_enabled() const noexcept;

    /**
     * \brief   Returns true if database logging is enabled. (Not yet implemented.)
     **/
    [[nodiscard]]
    bool is_db_logging_enabled() const noexcept;

    /**
     * \brief   Returns true if debug output (console) logging is enabled.
     **/
    [[nodiscard]]
    bool is_debug_logging_enabled() const noexcept;

    /**
     * \brief   Enables or disables logging for a specific target type.
     *
     * \param   logType     The logging target type to enable or disable.
     * \param   isEnabled   True to enable logging for this target; false to disable.
     **/
    void set_log_enabled(areg::LogTarget logType, bool isEnabled);

    /**
     * \brief   Returns the logging configuration version.
     **/
    [[nodiscard]]
    Version version() const noexcept;

    /**
     * \brief   Returns the layout format for scope entry messages.
     **/
    [[nodiscard]]
    String layout_enter() const;
    /**
     * \brief   Sets the layout format for scope entry messages.
     **/
    void set_layout_enter( const String & prop );

    /**
     * \brief   Returns the layout format for message text output.
     **/
    [[nodiscard]]
    String layout_message() const;
    /**
     * \brief   Sets the layout format for message text output.
     **/
    void set_layout_message( const String & prop );

    /**
     * \brief   Returns the layout format for scope exit messages.
     **/
    [[nodiscard]]
    String layout_exit() const;
    /**
     * \brief   Sets the layout format for scope exit messages.
     **/
    void set_layout_exit( const String & prop );

    /**
     * \brief   Returns the configured stack size.
     **/
    [[nodiscard]]
    uint32_t stack_size() const noexcept;
    /**
     * \brief   Sets the stack size.
     **/
    void set_stack_size( uint32_t prop );

    /**
     * \brief   Returns the logging status (enabled or disabled).
     **/
    [[nodiscard]]
    bool status() const noexcept;
    /**
     * \brief   Enables or disables logging.
     **/
    void set_status( bool prop );

    /**
     * \brief   Returns true if logs should be appended to existing files.
     **/
    [[nodiscard]]
    bool append_data() const noexcept;
    /**
     * \brief   Sets whether logs should be appended to existing files.
     **/
    void set_append_data( bool prop );

    /**
     * \brief   Returns the configured log file path.
     **/
    [[nodiscard]]
    String log_file() const;
    /**
     * \brief   Sets the log file path.
     **/
    void set_log_file( const String & prop );

    /**
     * \brief   Returns true if remote TCP logging is enabled.
     **/
    bool remote_tcp_enable() const;
    /**
     * \brief   Enables or disables remote TCP logging.
     **/
    void set_tcp_enable( bool prop, bool isTemporary = false);

    /**
     * \brief   Returns the remote host address for logging.
     **/
    String remote_tcp_address() const;
    /**
     * \brief   Sets the remote host address for logging.
     **/
    void set_tcp_address( const String & prop, bool isTemporary = false);

    /**
     * \brief   Returns the remote host port for logging.
     **/
    uint16_t remote_tcp_port() const;
    /**
     * \brief   Sets the remote host port for logging.
     **/
    void set_tcp_port( uint16_t prop, bool isTemporary = false);

    /**
     * \brief   Returns true if database logging is enabled.
     **/
    [[nodiscard]]
    bool database_enable() const noexcept;
    /**
     * \brief   Enables or disables database logging.
     **/
    void set_database_enable( bool prop, bool isTemporary = false);

    /**
     * \brief   Returns the number of module-specific log scopes and populates the scope list.
     **/
    [[nodiscard]]
    uint32_t module_scopes(std::vector<Property>& scopeList);
    /**
     * \brief   Sets the module-specific log scopes.
     **/
    void set_module_scopes(const std::vector<Property>& scopeList);

    void enable_scopes(const std::vector<String>& scopeNames, bool enable = true, bool isTemporary = false);

    /**
     * \brief   Returns the database engine name (e.g., 'sqlite').
     **/
    [[nodiscard]]
    String database_engine() const;
    /**
     * \brief   Sets the database engine name.
     **/
    void set_database_engine(const String & dbEngine, bool isTemporary = false);

    /**
     * \brief   Returns the full path to the database file.
     **/
    [[nodiscard]]
    String database_full_path() const;
    /**
     * \brief   Sets the full path to the database file.
     **/
    void set_db_path(const String & dbFullPath, bool isTemporary = false);

    /**
     * \brief   Returns the database name (e.g., 'SQLite').
     **/
    [[nodiscard]]
    String database_name() const;
    /**
     * \brief   Sets the database name.
     **/
    void set_database_name(const String & dbName, bool isTemporary = false);

    /**
     * \brief   Returns the database location (file path or URI).
     **/
    [[nodiscard]]
    String database_location() const;
    /**
     * \brief   Sets the database location.
     **/
    void set_database_location(const String& dbLocation, bool isTemporary = false);

    /**
     * \brief   Returns the database driver name.
     **/
    [[nodiscard]]
    String database_driver() const;
    /**
     * \brief   Sets the database driver name.
     **/
    void set_database_driver(const String& dbDriver, bool isTemporary = false);

    /**
     * \brief   Returns the database service address (IP and port).
     **/
    [[nodiscard]]
    areg::SocketAddress database_address() const;
    /**
     * \brief   Sets the database service address (IP and port).
     **/
    void set_database_address(const areg::SocketAddress& dbAddress, bool isTemporary = false);
    /**
     * \brief   Sets the database service address from host and port.
     **/
    void set_database_address(const String& dbAddress, uint16_t dbPort, bool isTemporary = false);

    /**
     * \brief   Returns the database user credentials (name and password).
     **/
    [[nodiscard]]
    areg::UserData database_user() const;
    /**
     * \brief   Sets the database user credentials.
     **/
    void set_database_user(const areg::UserData& dbUser, bool isTemporary = false);
    /**
     * \brief   Sets the database user name and password.
     **/
    void set_database_user(const String& dbUserName, const String& dbUserPassword, bool isTemporary = false);

    /**
     * \brief   Saves the current configuration to file.
     **/
    void save_configuration();

    /**
     * \brief   Updates configuration with the current state of all active scopes.
     *
     * \param   scopeController     The scope controller providing current scope states.
     **/
    void update_scope_configuration(const ScopeController & scopeController) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    ConfigManager& mConfigMan;  //!< Configuration manager for reading/writing properties.

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
