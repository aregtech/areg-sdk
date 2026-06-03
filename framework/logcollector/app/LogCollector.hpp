#ifndef AREG_LOGCOLLECTOR_APP_LOGCOLLECTOR_HPP
#define AREG_LOGCOLLECTOR_APP_LOGCOLLECTOR_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        logcollector/app/LogCollector.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Log Collector to run as a console application process or service.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "aregextend/service/ServiceApplicationBase.hpp"

#include "areg/base/SyncPrimitives.hpp"
#include "logcollector/app/LogCollectorNames.hpp"
#include "logcollector/service/LogCollectorServerService.hpp"
#include "aregextend/console/OptionParser.hpp"

#include <utility>

namespace areg::ext {
    class Console;
}

//////////////////////////////////////////////////////////////////////////
// LogCollector class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Separate process that collects and displays log messages from running applications, with
 *          options to save to file or forward to a log viewer.
 **/
class LogCollector final : public areg::ext::ServiceApplicationBase
{
//////////////////////////////////////////////////////////////////////////
// Internal types
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   logcollector::LoggerOption
     *          The commands to handle the log collector.
     **/
    enum class LoggerOption : int32_t
    {
          CMD_LogUndefined      = static_cast<int32_t>(areg::ext::ServiceOption::CMD_Undefined)  //!< Undefined command.
        , CMD_LogPrintHelp      = static_cast<int32_t>(areg::ext::ServiceOption::CMD_Help)       //!< Output help message.
        , CMD_LogLoad           = static_cast<int32_t>(areg::ext::ServiceOption::CMD_Load)       //!< Start the service by loading initialization instructions from configuration file.
        , CMD_LogVerbose        = static_cast<int32_t>(areg::ext::ServiceOption::CMD_Verbose)    //!< Display data rate information if possible. Functions only with extended features.
        , CMD_LogUninstall      = static_cast<int32_t>(areg::ext::ServiceOption::CMD_Uninstall)  //!< Uninstall as a service. Valid only as a command line option in Windows OS.
        , CMD_LogInstall        = static_cast<int32_t>(areg::ext::ServiceOption::CMD_Install)    //!< Install as service. Valid only as a command line option in Windows OS.
        , CMD_LogService        = static_cast<int32_t>(areg::ext::ServiceOption::CMD_Service)    //!< Start logcollector as a service. Valid only as a command line option in Windows OS.
        , CMD_LogConsole        = static_cast<int32_t>(areg::ext::ServiceOption::CMD_Console)    //!< Run as console application. Valid only as a command line option.
        , CMD_LogPause          = static_cast<int32_t>(areg::ext::ServiceOption::CMD_Custom)     //!< Pause logcollector.
        , CMD_LogRestart                                                                                //!< Restart logcollector.
        , CMD_LogInstances                                                                              //!< Display the names of connected log provider and log observer instances.
        , CMD_LogSilent                                                                                 //!< Silent mode, no data rate is displayed.
        , CMD_LogQuit                                                                                   //!< Quit logcollector.
        , CMD_LogQueryScopes                                                                            //!< Query the list of scopes
        , CMD_LogUpdateScope                                                                            //!< Set the scope priorities.
        , CMD_LogSaveLogs                                                                               //!< Log Collector save logs in the file.
        , CMD_LogSaveLogsStop                                                                           //!< Stop saving logs in the file.
        , CMD_LogSaveConfig                                                                             //!< Save the log configuration in the config file.
    };

    /**
     * \brief   The setup to validate input options of the log collector.
     **/
    static const areg::ext::OptionParser::OptionSetup ValidOptions[ ];

//////////////////////////////////////////////////////////////////////////
// statics
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the singleton instance of LogCollector.
     **/
    static LogCollector& instance();

    /**
     * \brief   Outputs the specified message on the console for console applications compiled with
     *          Areg Extended features; ignored otherwise.
     *
     * \param   status      The status message to print on the console.
     **/
    static void print_status(const areg::String& status);

//////////////////////////////////////////////////////////////////////////
// Hidden constructor / destructor
//////////////////////////////////////////////////////////////////////////
private:
    LogCollector();

    virtual ~LogCollector() = default;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// SystemServiceBase protected overrides
/************************************************************************/

    /**
     * \brief   Prints help message on the console, showing either command-line usage or interactive
     *          command options.
     *
     * \param   isCmdLine       If true, print help for command-line usage; otherwise print help for
     *                          interactive input commands.
     **/
    void print_help( bool isCmdLine ) final;

    /**
     * \brief   Starts the console service.
     **/
    void start_console_service() final;

    /**
     * \brief   Stops the console service.
     **/
    void stop_console_service() final;

    /**
     * \brief   Returns a callback function to validate and check input option values.
     **/
    areg::ext::Console::CallBack option_check_callback() const final;

    /**
     * \brief   Runs console with extended features that support output at any screen position.
     **/
    void run_console_io() final;

    /**
     * \brief   Runs the application as a background process without console input or output.
     **/
    void run_service() final;

/************************************************************************/
// ServiceApplicationBase protected overrides
/************************************************************************/
    /**
     * \brief   Returns a pair containing the pointer to the option list and the count of elements.
     **/
    std::pair<const areg::ext::OptionParser::OptionSetup*, int32_t> app_options() const final;

    /**
     * \brief   Returns the Unicode name of the service application.
     **/
    wchar_t* service_name_w() const final;

    /**
     * \brief   Returns the ASCII name of the service application.
     **/
    char* service_name_a() const final;

    /**
     * \brief   Returns the Unicode display name of the service application; may be shown in system
     *          service lists on some platforms (e.g., Windows).
     **/
    wchar_t* service_display_name_w() const final;

    /**
     * \brief   Returns the ASCII display name of the service application; may be shown in system
     *          service lists on some platforms (e.g., Windows).
     **/
    char* service_display_name_a() const final;

    /**
     * \brief   Returns the Unicode description of the service application; may appear in system
     *          service lists on some platforms (e.g., Windows).
     **/
    wchar_t* service_description_w() const final;

    /**
     * \brief   Returns the ASCII description of the service application; may appear in system
     *          service lists on some platforms (e.g., Windows).
     **/
    char* service_description_a() const final;

    /**
     * \brief   Returns the type of the remote service (Areg SDK services only).
     **/
    areg::RemoteServiceKind service_type() const final;

    /**
     * \brief   Returns the connection type of remote services (Areg SDK services only).
     **/
    areg::ConnectionType connection_type() const final;

/************************************************************************/
// ConfigListener protected overrides
/************************************************************************/

    /**
     * \brief   Called by the configuration manager after loading configuration from file.
     *
     * \param   config      The configuration manager instance.
     **/
    void post_read_configuration(areg::ConfigManager& config) final;

    /**
     * \brief   Called by the configuration manager after setting read-only and writable properties
     *          (e.g., when default configuration is established).
     *
     * \param   listReadonly    The list of read-only properties to set in the configuration.
     * \param   listWritable    The list of module/process-specific properties to set in the
     *                          configuration.
     * \param   config          The configuration manager instance.
     **/
    void on_setup_configuration(const areg::ListProperties& listReadonly, const areg::ListProperties& listWritable, areg::ConfigManager& config) final;

//////////////////////////////////////////////////////////////////////////
// Hidden methods.
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Returns the map of connected instances.
     **/
    inline const areg::MapInstances & connected_instances() const;

    /**
     * \brief   Returns a reference to this LogCollector instance.
     **/
    inline LogCollector& self();

    /**
     * \brief   Enables or disables local log messages by setting priority to NOTSET when enabled.
     *
     * \param   config      The configuration manager instance to control log messages.
     * \param   enable      If true, enable logs; otherwise disable them.
     **/
    inline void _enable_local_logs(areg::ConfigManager& config, bool enable);

    /**
     * \brief   Returns true if the command is recognized as a valid console command.
     *
     * \param   cmd     The command typed on the console.
     * \return  Returns true if the command is recognized; false otherwise.
     **/
    static bool _check_command(const areg::String& cmd);

    /**
     * \brief   Outputs the title on the console.
     **/
    static void _output_title();

    /**
     * \brief   Prints information on the console.
     *
     * \param   info    The information message to print.
     **/
    static void _output_info( const areg::String & info );

    /**
     * \brief   Outputs information about connected instances on the console.
     *
     * \param   instances       The map of connected instances to display.
     **/
    static void _output_instances( const areg::MapInstances & instances );

    /**
     * \brief   Sets verbose or silent mode for data rate output (available only with extended
     *          features enabled).
     *
     * \param   makeVerbose     If true, set verbose mode; otherwise set silent mode.
     **/
    static void _set_verbose_mode( bool makeVerbose );

    /**
     * \brief   Clears all message outputs including help and prompt displays.
     **/
    static void _clean_help();

    /**
     * \brief   Processes scope priority update commands; scopes should be separated by semicolon.
     *
     * \param   optScope    The option entry containing scope priority update instruction; multiple
     *                      scopes should be separated by ';'.
     **/
    static void _process_update_scopes(const areg::ext::OptionParser::InputOption& optScope);

    /**
     * \brief   Processes scope list queries by sending requests to specified client application IDs
     *          (separated by space or semicolon).
     *
     * \param   optScope    The option entry containing query command and list of client application
     *                      IDs; IDs can be separated by space ' ' or semicolon ';'.
     **/
    static void _process_query_scopes(const areg::ext::OptionParser::InputOption& optScope);

    /**
     * \brief   Creates a list of remote messages to update log scope priorities; each message
     *          contains an instruction for a single scope or scope group.
     *
     * \param   optScope    The option entry containing scope priority update instruction; multiple
     *                      scopes should be separated by ';'.
     * \param[out] msgList     On output, contains the list of messages to send to targets
     *                         (areg::TARGET_ALL if addressed to all clients).
     **/
    static void _create_scope_message(const areg::ext::OptionParser::InputOption& optScope, areg::ArrayList<areg::EventEnvelope> & msgList);

    /**
     * \brief   Returns a normalized scope string suitable for generating a property object with key
     *          and value.
     *
     * \param   scope       The scope string to normalize.
     * \return  Returns the normalized scope priority string to parse and generate a property
     *          object.
     **/
    static areg::String _normalize_scope_property(const areg::String & scope);

    /**
     * \brief   Returns a scope update message to send to a remote client.
     *
     * \param   scope       The scope priority string to parse and create the message from.
     * \return  Returns the message to send to the remote target client.
     **/
    static areg::EventEnvelope _create_scope_update_message(const areg::String& scope);

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The service connection object to communicate with processes.
     **/
    LogCollectorServerService mServiceServer;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( LogCollector );
};

//////////////////////////////////////////////////////////////////////////
// LogCollector class inline methods.
//////////////////////////////////////////////////////////////////////////

inline const areg::MapInstances & LogCollector::connected_instances() const
{
    return mServiceServer.instances( );
}

inline LogCollector & LogCollector::self()
{
    return (*this);
}

#endif  // AREG_LOGCOLLECTOR_APP_LOGCOLLECTOR_HPP
