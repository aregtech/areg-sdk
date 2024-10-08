#ifndef AREG_LOGOBSERVER_APP_LOGOBSERVER_HPP
#define AREG_LOGOBSERVER_APP_LOGOBSERVER_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        logobserver/app/LogObserver.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, logger to run as a console application process or service.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "logobserver/app/NELogObserverSettings.hpp"
#include "areg/persist/IEConfigurationListener.hpp"

#include "areg/base/SynchObjects.hpp"
#include "aregextend/console/Console.hpp"
#include "aregextend/console/OptionParser.hpp"

#include <utility>

/************************************************************************
 * Dependencies.
 ************************************************************************/
struct sLogInstance;
struct sLogMessage;
struct sLogScope;

//////////////////////////////////////////////////////////////////////////
// LogObserver class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The LogObserver service is a separate process, which receives
 *          and collects the log messages from the running applications.
 *          It may save logs in the file or forward to log viewer application..
 **/
class LogObserver
{
//////////////////////////////////////////////////////////////////////////
// Internal types
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The commands to handle the logger.
     **/
    enum class eLoggerOptions : int32_t
    {
          CMD_LogUndefined  = 0 //!< Undefined command.
        , CMD_LogQueryScopes    //!< Query the list of scopes
        , CMD_LogSaveConfig     //!< Save the configuration file.
        , CMD_LogPrintHelp      //!< Output help message.
        , CMD_LogInformation    //!< Display the list of logging source instances.
        , CMD_LogUpdateScope    //!< Set and update the log scope priorities.
        , CMD_LogPause          //!< Pause log observer.
        , CMD_LogQuit           //!< Quit log observer.
        , CMD_LogRestart        //!< Restart / continue log observer .
        , CMD_LogStop           //!< Stop log observer.
    };

    /**
     * \brief   The structure to fill text of observer status and error message on certain action
     **/
    struct sObserverStatus
    {
        eLoggerOptions      osOption;   //!< The action
        std::string_view    osStatus;   //!< The status message to display when action succeeds
        std::string_view    osError;    //!< The error message when action fails.
    };

    /**
     * \brief   The setup to validate input options of the logger.
     **/
    static const OptionParser::sOptionSetup ValidOptions[];

    /**
     * \brief   The list of actions, associated status and error messages.
     **/
    static constexpr sObserverStatus    ObserverStatus[]
    {
          //!< No status, no error
          { eLoggerOptions::CMD_LogUndefined    , "", "" }
          //!< The status and error message when query scopes.
        , { eLoggerOptions::CMD_LogQueryScopes  , "Log observer queries scopes."                    , "Log observer failed to query scopes." }
          //!< The status and error message when request to save configuration
        , { eLoggerOptions::CMD_LogSaveConfig   , "Log observer requested to save configuration."   , "Log observer failed to request save config." }
          //!< No status or error when output print help
        , { eLoggerOptions::CMD_LogPrintHelp    , "", "" }
          //!< The status or error message when request to output list of  connected instances.
        , { eLoggerOptions::CMD_LogInformation  , "List of connected instances ..."                 , "" }
          //!< The status or error message when request to update scopes.
        , { eLoggerOptions::CMD_LogUpdateScope  , "Log observer requested to update scopes."        , "Log observer failed to request update scopes." }
          //!< The status or error message when request to pause logging.
        , { eLoggerOptions::CMD_LogPause        , "Log observer is paused, type \'-r\' to resume."  , "" }
          //!< No status or error when request to quit log observer
        , { eLoggerOptions::CMD_LogQuit         , "", "" }
          //!< The status or error  message when request to start or resume log observer.
        , { eLoggerOptions::CMD_LogRestart      , "Log observer triggered connection."              , "Log observer failed to trigger connection. Check initialization." }
          //!< The status or error message when request to stop logging.
        , { eLoggerOptions::CMD_LogStop         , "Log observer stops, type \'-r\' to resume."      , "Log observer failed to stop. Restart application." }
    };

    //!< The initialized status.
    static constexpr std::string_view  STATUS_INITIALIZED       { "The log observer is initialized, type \'-r\' to connect." };

    //!< The title to display on console when run application.
    static constexpr std::string_view  APP_TITLE                { "AREG Log Observer console application ..." };

//////////////////////////////////////////////////////////////////////////
// statics
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns singleton instance of the LogObserver.
     **/
    static LogObserver& getInstance( void );

//////////////////////////////////////////////////////////////////////////
// Hidden constructor / destructor
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Default constructor and destructor.
     **/
    LogObserver( void ) = default;
    ~LogObserver(void) = default;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Called from main to start execution of  message router service.
     * \param   argc    The 'argc' parameter passed from 'main', indicates the number of parameters passed to executable.
     * \param   argv    The 'argv' parameter passed from 'main', indicated parameters passed to executable.
     **/
    void logMain( int argc, char ** argv );

    /**
     * \brief   Triggered to receive a function to validate and check the input option values.
     **/
    Console::CallBack getOptionCheckCallback( void ) const;

private:

    /**
     * \brief   The callback of the event triggered when initializing and configuring the observer.
     *          The callback indicates the IP address and port number of the logger service set
     *          in the configuration file.
     * \param   isEnabled       The flag, indicating whether the logging service is enabled or not.
     * \param   address         The null-terminated string of the IP address of the logger service set in the configuration file.
     * \param   port            The IP port number of the logger service set in the configuration file.
     **/
    static void callbackObserverConfigured(bool isEnabled, const char * address, uint16_t port);

    /**
     * \brief   The callback of the event triggered when initializing and configuring the observer.
     *          The callback indicates the supported database, the database location or URI and
     *          the database user name.
     * \param   isEnabled       The flag, indicating whether the logging in the database is enabler or not.
     * \param   dbName          The name of the  supported database.
     * \param   dbLocation      The relative or absolute path the database. The path may contain a mask.
     * \param   user            The database user to use when log in. If null or empty, the database may not require the user name.
     **/
    static void callbackDatabaseConfigured(bool isEnabled, const char* dbName, const char* dbLocation, const char* user);

    /**
     * \brief   The callback of the event triggered when the observer connects or disconnects from the logger service.
     * \param   isConnected     Flag, indicating whether observer is connected or disconnected.
     * \param   address         The IP address of the logger service to connect or disconnect.
     * \param   port            The IP port number of the logger service to connect or disconnect.
     **/
    static void callbackServiceConnected(bool isConnected, const char* address, uint16_t port);

    /**
     * \brief   The callback of the event trigger when starting or pausing the log observer.
     *          If the log observer is paused, on start it continues to write logs in the same file.
     *          If the log observer is stopped (disconnected is called), on start it creates new file.
     * \param   isStarted       The flag indicating whether the lob observer is started or paused.
     **/
    static void callbackObserverStarted(bool isStarted);

    /**
     * \brief   The callback of the event triggered when fails to send or receive message.
     **/
    static void callbackMessagingFailed(void);

    /**
     * \brief   The callback of the event triggered when receive the list of connected instances that make logs.
     * \param   instances   The pointer to the list of the connected instances.
     * \param   count       The number of entries in the list.
     **/
    static void callbackConnectedInstances(const sLogInstance* instances, uint32_t count);

    /**
     * \brief   The callback of the event triggered when receive the list of disconnected instances that make logs.
     * \param   instances   The pointer to the list of IDs of the disconnected instances.
     * \param   count       The number of entries in the list.
     **/
    static void callbackDisconnecteInstances(const ITEM_ID * instances, uint32_t count);

    /**
     * \brief   The callback of the event triggered when receive the list of the scopes registered in an application.
     * \param   cookie  The cookie ID of the connected instance / application. Same as sLogInstance::liCookie
     * \param   scopes  The list of the scopes registered in the application. Each entry contains the ID of the scope, message priority and the full name.
     * \param   count   The number of scope entries in the list.
     **/
    static void callbackLogScopes(ITEM_ID cookie, const sLogScope* scopes, uint32_t count);

    /**
     * \brief   The callback of the event triggered when receive remote message to log.
     *          The buffer indicates to the NETrace::sLogMessage structure.
     * \param   logBuffer   The pointer to the NETrace::sLogMessage structure to log messages.
     * \param   size        The size of the buffer with log message.
     **/
    static void callbackLogMessageEx(const unsigned char * logBuffer, uint32_t size);

//////////////////////////////////////////////////////////////////////////
// Hidden methods.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Triggered if need to run console with extended features.
     *          In extended feature, the console can output message at any position on the screen.
     **/
    void _runConsoleInputExtended(void);

    /**
     * \brief   Enables or disables local log messages of the current process.
     *          The method does not enable or disable logging, it only enables logging messages,
     *          i.e. sets the priority NOTSET.
     * \param   config  Instance of the configuration manager to enable or disable log messages.
     * \param   enable  Flag, indicating whether the logs should be enabled or not.
     *                  If true, the logs are enabled. Otherwise, the logs are disabled.
     **/
    inline void enableLocalLogs(ConfigManager& config, bool enable);

    /**
     * \brief   Checks the command typed on console. Relevant only if it runs as a console application.
     * \param   cmd     The command typed on the console.
     * \return  Returns true if command is recognized. Otherwise, returns false.
     **/
    static bool _checkCommand(const String& cmd);

    /**
     * \brief   Output on console the title.
     **/
    static void _outputTitle( void );

    /**
     * \brief   Prints info on console.
     **/
    static void _outputInfo( const String & info );

    /**
     * \brief   Call to clean all message outputs like help, prompt, etc.
     *          Normally, help is the largest message.
     **/
    static void _cleanHelp(void);

    /**
     * \brief   Triggered when requested to save the configuration of the client(s).
     * \param   optSave     The option entry that contains the list of target clients
     *                      that should save configuration.
     * \return  Returns true if processed with success. Otherwise, returns false.
     **/
    static bool _processSaveConfig(const OptionParser::sOption& optSave);

    /**
     * \brief   Triggered to print the help message on console.
     * \return  Returns true if processed with success. Otherwise, returns false.
     **/
    static bool _processPrintHelp(void);

    /**
     * \brief   Triggered to print the information of instances, such as ID, number of registered scope, name, etc.
     * \return  Returns true if processed with success. Otherwise, returns false.
     **/
    static bool _processInfoInstances(void);

    /**
     * \brief   Triggered to process update scope priority command.
     * \param   optScope    The option entry that contains scope priority update instruction.
     *                      If the command contains a list of scopes to update, the should be split by ';'.
     * \return  Returns true if processed with success. Otherwise, returns false.
     **/
    static bool _processUpdateScopes(const OptionParser::sOption& optScope);

    /**
     * \brief   Triggered to pause logging. The lob observer is paused if it does not write logs in the file.
     *          But the log observer may remain connected.
     * \return  Returns true if processed with success. Otherwise, returns false.
     **/
    static bool _processPauseLogging(void);

    /**
     * \brief   Triggered to stop, resume or start logging. When starts it may create new log file.
     *          The resumed logging continues writing logs in the existing log file.
     * \param   doStart     If true it resumes or starts the logging.
     *                      If false, it stops and disconnects the log observer from logging service.
     * \return  Returns true if processed with success. Otherwise, returns false.
     **/
    static bool _processStartLogging(bool doStart);

    /**
     * \brief   Triggered to trigger querying the list of registered scopes.
     * \param   optScope    The option entry that contains query command and list of client application IDs to request scope list.
     *                      If the command contains a list of IDs, it can be separated either by space ' ' or semicolon ';'.
     * \return  Returns true if processed with success. Otherwise, returns false.
     **/
    static bool _processQueryScopes(const OptionParser::sOption& optScope);

    /**
     * \brief   Normalizes the scope to make it suitable to generate property object with the key and value.
     * \param   scope   The scope to normalize.
     * \return  Returns normalized scope priority string to parse and generate property object with key and value.
     **/
    static String _normalizeScopeProperty(const String & scope);

    /**
     * \brief   Creates a scope update message to send to the target client.
     * \param   scope   The scope priority string to parse and create message.
     * \return  Returns message to send to the remote target client.
     **/
    static bool _sendScopeUpdateMessage(const String& scope);

//////////////////////////////////////////////////////////////////////////
// OS specific hidden methods.
//////////////////////////////////////////////////////////////////////////
private:
    
    /**
     * \brief   OS specific implementation of waiting for user input on console.
     * \param   buffer  The allocated buffer to stream input from console.
     * \param   bufSize The size of allocated bugger.
     * \return  Returns true if succeeded to get user input.
     **/
    bool _osWaitUserInput(char* buffer, unsigned int bufSize);

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( LogObserver );
};

#endif  // AREG_LOGOBSERVER_APP_LOGOBSERVER_HPP
