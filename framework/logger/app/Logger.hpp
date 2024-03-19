#ifndef AREG_LOGGER_APP_LOGGER_HPP
#define AREG_LOGGER_APP_LOGGER_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        logger/app/Logger.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, logger to run as a console application process or service.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "extend/service/ServiceApplicationBase.hpp"

#include "areg/base/SynchObjects.hpp"
#include "logger/app/NELoggerSettings.hpp"
#include "logger/service/LoggerServerService.hpp"
#include "extend/console/OptionParser.hpp"

#include <utility>

class Console;

//////////////////////////////////////////////////////////////////////////
// Logger class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The Logger service is a separate process, which receives
 *          and collects the log messages from the running applications.
 *          It may save logs in the file or forward to log viewer application..
 **/
class Logger : public ServiceApplicationBase
{
//////////////////////////////////////////////////////////////////////////
// Internal types
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   NELoggerSettings::eRouterOptions
     *          The commands to handle the logger.
     **/
    enum class eLoggerOptions : int32_t
    {
          CMD_LogUndefined      //!< Undefined command.
        , CMD_LogPause          //!< Pause logger.
        , CMD_LogRestart        //!< Restart logger.
        , CMD_LogInstances      //!< Display the names of connected instances.
        , CMD_LogVerbose        //!< Display data rate information if possible. Functions only with extended features.
        , CMD_LogSilent         //!< Silent mode, no data rate is displayed.
        , CMD_LogPrintHelp      //!< Output help message.
        , CMD_LogQuit           //!< Quit logger.
        , CMD_LogConsole        //!< Run as console application. Valid only as a command line option.
        , CMD_LogInstall        //!< Install as service. Valid only as a command line option in Windows OS.
        , CMD_LogUninstall      //!< Uninstall as a service. Valid only as a command line option in Windows OS.
        , CMD_LogQueryScopes    //!< Query the list of scopes
        , CMD_LogService        //!< Start logger as a service. Valid only as a command line option in Windows OS.
        , CMD_LogUpdateScope    //!< Set the scope priorities.
        , CMD_LogSaveLogs       //!< Logger save logs in the file.
        , CMD_LogSaveLogsStop   //!< Cancel saving logs in the file.
        , CMD_LogSaveConfig     //!< Save the log configuration in the config file.
    };

    /**
     * \brief   The setup to validate input options of the logger.
     **/
    static const OptionParser::sOptionSetup ValidOptions[ ];

//////////////////////////////////////////////////////////////////////////
// statics
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns singleton instance of the Logger.
     **/
    static Logger& getInstance( void );

    /**
     * \brief   Outputs the specified message on the console.
     *          The method is valid only for console application compiled
     *          with AREG Extended features.
     *          Otherwise, the method ignores request to output message.
     * \param   status  The status message to print on console.
     **/
    static void printStatus(const String& status);

//////////////////////////////////////////////////////////////////////////
// Hidden constructor / destructor
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Default constructor and destructor.
     **/
    Logger( void );

    virtual ~Logger( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// SystemServiceBase protected overrides
/************************************************************************/

    /**
     * \brief   Triggered to print the help message on console.
     * \param   isCmdLine   Flag indicating whether it should print the help
     *                      of using service in command line or help of user input commands.
     *                      If 'true', the printing message is about using the service in
     *                      command line. Otherwise, if application expects user inputs, prints
     *                      the help of command options.
     **/
    virtual void printHelp( bool isCmdLine ) override;

    /**
     * \brief   Triggered to start the console service.
     **/
    virtual void startConsoleService( void ) override;

    /**
     * \brief   Stops the consoler service.
     **/
    virtual void stopConsoleService( void ) override;

    /**
     * \brief   Triggered to receive a function to validate and check the input option values.
     **/
    virtual Console::CallBack getOptionCheckCallback( void ) const override;

    /**
     * \brief   Triggered if need to run console with extended features.
     *          In extended feature, the console can output message at any position on the screen.
     **/
    virtual void runConsoleInputExtended( void ) override;

    /**
     * \brief   Triggered if need to run console with simple (not extended) features.
     **/
    virtual void runConsoleInputSimple( void ) override;

    /**
     * \brief   Run application as a background process without input or output on console. 
     **/
    virtual void runService(void) override;

/************************************************************************/
// ServiceApplicationBase protected overrides
/************************************************************************/
    /**
     * \brief   Returns list of the options to validate contained in the pair object,
     *          where the first entry is the pointer to the list and second entry is
     *          the number of elements in the list
     **/
    virtual std::pair<const OptionParser::sOptionSetup*, int> getAppOptions(void) const override;

    /**
     * \brief   Returns the UNICODE name of the service application.
     **/
    virtual wchar_t* getServiceNameW(void) const override;

    /**
     * \brief   Returns the ASCII name of the service application.
     **/
    virtual char* getServiceNameA(void) const override;

    /**
     * \brief   Returns the UNICODE display name of the service application.
     *          This optional display name could be valid only for specific OS.
     *          For example, in Windows this name is displayed in the list of services.
     **/
    virtual wchar_t* getServiceDisplayNameW(void) const override;

    /**
     * \brief   Returns the ASCII display name of the service application.
     *          This optional display name could be valid only for specific OS.
     *          For example, in Windows this name is displayed in the list of services.
     **/
    virtual char* getServiceDisplayNameA(void) const override;

    /**
     * \brief   Returns the UNICODE description of the service application.
     *          This optional service description could be valid only for specific OS.
     *          For example, in Windows this description is shown in the list of services.
     **/
    virtual wchar_t* getServiceDescriptionW(void) const override;

    /**
     * \brief   Returns the ASCII description of the service application.
     *          This optional service description could be valid only for specific OS.
     *          For example, in Windows this description is shown in the list of services.
     **/
    virtual char* getServiceDescriptionA(void) const override;

    /**
     * \brief   Returns the type of the remote service.
     *          Valid only for AREG SDK services.
     **/
    virtual NERemoteService::eRemoteServices getServiceType(void) const override;

    /**
     * \brief   Returns the type of the connection of the remote services.
     *          Valid only for AREG SDK services.
     **/
    virtual NERemoteService::eConnectionTypes getConnectionType(void) const override;

/************************************************************************/
// IEConfigurationListener protected overrides
/************************************************************************/

    /**
     * \brief   Called by configuration manager when configuration is completed to load data from the file.
     * \param   config  The instance of configuration manager.
     **/
    virtual void postReadConfiguration(ConfigManager& config) override;

    /**
     * \brief   Called by configuration manager after setting read-only and writable properties.
     *          For example, when the default configuration is set.
     * \param   listReadonly    The list of read-only properties to set in the configuration.
     * \param   listWritable    The list of module / process specific properties to set in the configuration;
     * \param   config          The instance of configuration manager.
     **/
    virtual void onSetupConfiguration(const NEPersistence::ListProperties& listReadonly, const NEPersistence::ListProperties& listWritable, ConfigManager& config) override;

//////////////////////////////////////////////////////////////////////////
// Hidden methods.
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Returns the list of connected instances.
     **/
    inline const NEService::MapInstances & getConnetedInstances( void ) const;

    /**
     * \brief   Returns instance of the logger service.
     **/
    inline Logger& self(void);

    /**
     * \brief   Enables or disables local log messages of the current process.
     *          The method does not enable or disable logging, it only enables logging messages,
     *          i.e. sets the priority NOTSET.
     * \param   config  Instance of the configuration manager to enable or disable log messages.
     * \param   enable  Flag, indicating whether the logs should be enabled or not.
     *                  If true, the logs are enabled. Otherwise, the logs are disabled.
     **/
    inline void _enableLocalLogs(ConfigManager& config, bool enable);

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
     * \brief   Outputs on console the information about connected instances.
     **/
    static void _outputInstances( const NEService::MapInstances & instances );

    /**
     * \brief   Sets verbose or silent mode to output data rate.
     *          The feature is available only if compile with enabled extended features.
     *          Otherwise, it outputs error message and nothing happens.
     **/
    static void _setVerboseMode( bool makeVerbose );

    /**
     * \brief   Call to clean all message outputs like help, prompt, etc.
     *          Normally, help is the largest message.
     **/
    static void _cleanHelp(void);

    /**
     * \brief   Triggered to process update scope priority command.
     * \param   optScope    The option entry that contains scope priority update instruction.
     *                      If the command contains a list of scopes to update, the should be split by ';'.
     **/
    static void _processUpdateScopes(const OptionParser::sOption& optScope);

    /**
     * \brief   Triggered to trigger querying the list of registered scopes.
     * \param   optScope    The option entry that contains query command and list of client application IDs to request scope list.
     *                      If the command contains a list of IDs, it can be separated either by space ' ' or semicolon ';'.
     **/
    static void _processQueryScopes(const OptionParser::sOption& optScope);

    /**
     * \brief   Creates a list of remote messages to send to update log scope priorities.
     *          On output the 'msgList' contains the list of message. Each message contains
     *          instruction to update single scope or single scope group.
     * \param   optScope    The option entry that contains scope priority update instruction.
     *                      If the command contains a list of scopes to update, the should be split by ';'.
     * \param   msgList     On output it contains a list a messages to send to the targets.
     *                      If a message is referred to all connected clients, the target is NEService::COOKIE_ANY.
     **/
    static void _createScopeMessage(const OptionParser::sOption& optScope, TEArrayList<RemoteMessage> & OUT msgList);

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
    static RemoteMessage _createScopeUpdateMessage(const String& scope);

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The service connection object to communicate with processes.
     **/
    LoggerServerService mServiceServer;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( Logger );
};

//////////////////////////////////////////////////////////////////////////
// Logger class inline methods.
//////////////////////////////////////////////////////////////////////////

inline const NEService::MapInstances & Logger::getConnetedInstances( void ) const
{
    return mServiceServer.getInstances( );
}

inline Logger & Logger::self( void )
{
    return (*this);
}

#endif  // AREG_LOGGER_APP_LOGGER_HPP
