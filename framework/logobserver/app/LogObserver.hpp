#ifndef AREG_LOGOBSERVER_APP_LOGOBSERVER_HPP
#define AREG_LOGOBSERVER_APP_LOGOBSERVER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        logobserver/app/LogObserver.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Log Observer to run as a console application.
 ************************************************************************/

#if !defined(IMP_AREG_DLL) && !defined(IMP_AREG_LIB) && !defined(IMPORT_SHARED_SYMBOLS) && !defined(IMPORT_STATIC_SYMBOLS)
    #define IMP_AREG_DLL
#elif defined(IMP_AREG_DLL) || defined(IMPORT_SHARED_SYMBOLS)
    #undef  IMP_AREG_DLL
    #define IMP_AREG_DLL
#elif defined(IMP_AREG_LIB) || defined(IMPORT_STATIC_SYMBOLS)
    #undef  IMP_AREG_LIB
    #define IMP_AREG_LIB
#endif  // !defined(IMP_AREG_DLL) && !defined(IMP_AREG_LIB) && !defined(IMPORT_SHARED_SYMBOLS) && !defined(IMPORT_STATIC_SYMBOLS)

#if !defined(IMP_LOGGER_DLL) && !defined(IMP_LOGGER_LIB)
    #define     IMP_LOGGER_DLL
#endif  // !defined(IMP_LOGGER_DLL) || !defined(IMP_LOGGER_LIB)

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areglogger/client/LogObserverSwitches.h"
#include "areg/persist/ConfigListener.hpp"

#include "areg/base/SyncPrimitives.hpp"
#include "aregextend/console/Console.hpp"
#include "aregextend/console/OptionParser.hpp"

#include <utility>

/************************************************************************
 * Dependencies.
 ************************************************************************/
struct LogInstance;
struct LogEntry;
struct ScopeInfo;

//////////////////////////////////////////////////////////////////////////
// LogObserver class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Log observer service that receives and collects log messages from running applications,
 *          saving them to files or forwarding them to a log viewer.
 **/
class LogObserver
{
//////////////////////////////////////////////////////////////////////////
// Internal types
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The commands to handle the Log Observer.
     **/
    enum class LoggerOption : int32_t
    {
          CMD_LogUndefined   = 0//!< Undefined command.
        , CMD_LogPrintHelp      //!< Output help message.
        , CMD_LogLoad           //!< Start the application by loading initialization instructions from configuration file.
        , CMD_LogPause          //!< Pause log observer
        , CMD_LogRestart        //!< Start / Restart log observer
        , CMD_LogInstances      //!< Display the list of log providers
        , CMD_LogQuit           //!< Quit Log Observer.
        , CMD_LogQueryScopes    //!< Query the list of scopes
        , CMD_LogUpdateScope    //!< Set and update the log scope priorities.
        , CMD_LogSaveConfig     //!< Save the configuration file.
        , CMD_LogStop           //!< Stop log observer.
    };

    /**
     * \brief   The structure to fill text of observer status and error message on certain action
     **/
    struct ObserverStatus
    {
        LoggerOption      osOption;   //!< The action
        std::string_view    osStatus;   //!< The status message to display when action succeeds
        std::string_view    osError;    //!< The error message when action fails.
    };

    /**
     * \brief   The setup to validate input options of the Log Observer.
     **/
    static const OptionParser::OptionSetup ValidOptions[];

    /**
     * \brief   The list of actions, associated status and error messages.
     **/
    static constexpr ObserverStatus    _observerStatus[]
    {
          // ----------------------------------------------------------------------------------------------------
          // | Option                           |  status                                           |  error    |
          // ----------------------------------------------------------------------------------------------------
          //!< No status, no error
          { LoggerOption::CMD_LogUndefined    , ""                                                , "" }
          //!< No status or error when output print help
        , { LoggerOption::CMD_LogPrintHelp    , ""                                                , "" }
          //!< No status or error when Start the application by loading initialization instructions from configuration file.
        , { LoggerOption::CMD_LogLoad         , ""                                                , "The configuration file does not exist or wrong, make default initialization." }
          //!< The status or error message when request to pause logging.
        , { LoggerOption::CMD_LogPause        , "Log observer is paused, type \'-r\' to resume."  , "" }
          //!< The status or error  message when request to start or resume log observer.
        , { LoggerOption::CMD_LogRestart      , "Log observer triggered connection."              , "Log observer failed to trigger connection. Check initialization." }
          //!< The status or error message when request to output list of  connected instances.
        , { LoggerOption::CMD_LogInstances    , "List of connected instances ..."                 , "" }
          //!< No status or error when request to quit log observer
        , { LoggerOption::CMD_LogQuit         , "", "" }
          //!< The status and error message when query scopes.
        , { LoggerOption::CMD_LogQueryScopes  , "Log observer queries scopes."                    , "Log observer failed to query scopes." }
          //!< The status or error message when request to update scopes.
        , { LoggerOption::CMD_LogUpdateScope  , "Log observer requested to update scopes."        , "Log observer failed to request update scopes." }
          //!< The status and error message when request to save configuration
        , { LoggerOption::CMD_LogSaveConfig   , "Log observer requested to save configuration."   , "Log observer failed to request save config." }
          //!< The status or error message when request to stop logging.
        , { LoggerOption::CMD_LogStop         , "Log observer stops, type \'-r\' to resume."      , "Log observer failed to stop. Restart application." }
    };

    //!< The initialized status.
    static constexpr std::string_view  STATUS_INITIALIZED       { "The log observer is initialized, type \'-r\' to connect." };

    //!< The title to display on console when run application.
    static constexpr std::string_view  APP_TITLE                { "Areg Log Observer console application ..." };

//////////////////////////////////////////////////////////////////////////
// statics
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the singleton instance of LogObserver.
     **/
    static LogObserver& instance();

//////////////////////////////////////////////////////////////////////////
// Hidden constructor / destructor
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief
     **/
    LogObserver() = default;
    ~LogObserver() = default;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Starts execution of the log observer service from main.
     *
     * \param   argc    The number of command-line arguments passed to the executable.
     * \param   argv    The command-line arguments passed to the executable.
     **/
    void log_main( int32_t argc, char ** argv );

    /**
     * \brief   Returns a callback function to validate and check input option values.
     **/
    Console::CallBack option_check_callback() const;

private:

    /**
     * \brief   Called when the observer is initialized and configured with remote log collector
     *          service settings.
     *
     * \param   is_enabled      True if the logging service is enabled; false otherwise.
     * \param   address         IP address of the remote log collector service from the
     *                          configuration file.
     * \param   port            Port number of the remote log collector service from the
     *                          configuration file.
     **/
    static void callback_observer_configured(bool is_enabled, const char * address, uint16_t port);

    /**
     * \brief   Called when the observer is initialized and configured with database settings.
     *
     * \param   is_enabled      True if database logging is enabled; false otherwise.
     * \param   dbName          The name of the supported database.
     * \param   dbLocation      The relative or absolute path to the database, which may contain
     *                          wildcards.
     * \param   user            The database user name; null or empty if not required by the
     *                          database.
     **/
    static void callback_database_configured(bool is_enabled, const char* dbName, const char* dbLocation, const char* user);

    /**
     * \brief   Called when the observer connects to or disconnects from the remote log collector
     *          service.
     *
     * \param   is_connected    True if connected; false if disconnected.
     * \param   address         IP address of the remote log collector service.
     * \param   port            Port number of the remote log collector service.
     **/
    static void callback_service_connected(bool is_connected, const char* address, uint16_t port);

    /**
     * \brief   Called when the log observer is started or paused.
     *
     * \param   is_started      True if the observer is started; false if paused. When resumed,
     *                          continues writing to the same file; when started after disconnect,
     *                          creates a new file.
     **/
    static void callback_observer_started(bool is_started);

    /**
     * \brief   Called when the logging database is created.
     *
     * \param   dbLocation      The relative or absolute path to the logging database.
     **/
    static void callback_log_db_created(const char* dbLocation);

    /**
     * \brief   Called when message sending or receiving fails.
     **/
    static void callback_messaging_failed();

    /**
     * \brief   Called when the list of connected instances (applications making logs) is received.
     *
     * \param   instances       Pointer to the array of connected instances.
     * \param   count           The number of entries in the instances array.
     **/
    static void callback_connected_instances(const LogInstance* instances, uint32_t count);

    /**
     * \brief   Called when the list of disconnected instances (applications that made logs) is
     *          received.
     *
     * \param   instances       Pointer to the array of IDs of disconnected instances.
     * \param   count           The number of entries in the instances array.
     **/
    static void callback_disconnected_instances(const ITEM_ID * instances, uint32_t count);

    /**
     * \brief   Called when the list of registered scopes in an application is received.
     *
     * \param   cookie      The cookie ID of the connected instance (application), same as
     *                      LogInstance::liCookie.
     * \param   scopes      Pointer to the array of scope entries, each containing the scope ID,
     *                      message priority, and full name.
     * \param   count       The number of scope entries in the array.
     **/
    static void callback_log_scopes(ITEM_ID cookie, const ScopeInfo* scopes, uint32_t count);

    /**
     * \brief   Called when previously registered scopes are updated with new priorities.
     *
     * \param   cookie      The cookie ID of the connected instance (application), same as
     *                      LogInstance::liCookie.
     * \param   scopes      Pointer to the array of updated scope entries, each containing the scope
     *                      ID, new priority, and full name.
     * \param   count       The number of scope entries in the array.
     **/
    static void callback_log_update_scopes(ITEM_ID cookie, const ScopeInfo* scopes, uint32_t count);

    /**
     * \brief   Called when a log message is received.
     *
     * \param   log_message     Pointer to the log message to log.
     **/
    static void callback_log_message(const LogEntry* log_message);

    /**
     * \brief   Called when a remote message is received for logging.
     *
     * \param   logBuffer       Pointer to a buffer containing a NELogging::LogEntry structure with
     *                          the log message.
     * \param   size            The size of the buffer.
     **/
    static void callback_log_message_ex(const uint8_t * logBuffer, uint32_t size);

//////////////////////////////////////////////////////////////////////////
// Hidden methods.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Runs the console with extended features for output anywhere on the screen.
     **/
    void _run_console_input_extended();

    /**
     * \brief   Enables or disables local log messages by setting the priority to NOTSET.
     *
     * \param   config      The configuration manager instance used to enable or disable log
     *                      messages.
     * \param   enable      True to enable logs; false to disable them.
     **/
    inline void enable_local_logs(ConfigManager& config, bool enable);

    /**
     * \brief   Returns true if the command is recognized; false otherwise.
     *
     * \param   cmd     The command typed on the console.
     **/
    static bool _check_command(const String& cmd);

    /**
     * \brief   Outputs the title to the console.
     **/
    static void _output_title();

    /**
     * \brief   Prints information to the console.
     *
     * \param   info    The information string to print.
     **/
    static void _output_info( const String & info );

    /**
     * \brief   Clears all console message outputs such as help text and prompts.
     **/
    static void _clean_help();

    /**
     * \brief   Saves the configuration of the specified client(s).
     *
     * \param   optSave     The option entry containing the list of target clients that should save
     *                      their configuration.
     **/
    static bool _process_save_config(const OptionParser::InputOption& optSave);

    /**
     * \brief   Prints the help message to the console.
     **/
    static bool _process_print_help();

    /**
     * \brief   Prints information about instances, such as ID, scope count, and name.
     **/
    static bool _process_info_instances();

    /**
     * \brief   Processes the scope priority update command.
     *
     * \param   optScope    The option entry containing scope priority update instructions; scope
     *                      entries are separated by ';'.
     **/
    static bool _process_update_scopes(const OptionParser::InputOption& optScope);

    /**
     * \brief   Pauses logging so the observer stops writing to the file while remaining connected.
     **/
    static bool _process_pause_logging();

    /**
     * \brief   Starts, resumes, or stops logging.
     *
     * \param   doStart     True to resume or start logging (creates a new file if starting after
     *                      disconnect); false to stop and disconnect from the logging service.
     **/
    static bool _process_start_logging(bool doStart);

    /**
     * \brief   Queries the list of registered scopes from the specified client applications.
     *
     * \param   optScope    The option entry containing the query command and list of client
     *                      application IDs; IDs are separated by space or semicolon.
     **/
    static bool _process_query_scopes(const OptionParser::InputOption& optScope);

    /**
     * \brief   Normalizes a scope string for property object generation.
     *
     * \param   scope       The scope string to normalize.
     **/
    static String _normalize_scope_property(const String & scope);

    /**
     * \brief   Creates and sends a scope update message to the target client.
     *
     * \param   scope       The scope priority string to parse and create the message.
     **/
    static bool _send_scope_update_message(const String& scope);

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( LogObserver );
};

#endif  // AREG_LOGOBSERVER_APP_LOGOBSERVER_HPP
