/************************************************************************
 * \file        logger/app/private/Logger.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       Logger common part
 ************************************************************************/

#include "logger/app/Logger.hpp"
#include "logger/app/private/LoggerConsoleService.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/appbase/NEApplication.hpp"
#include "areg/base/File.hpp"
#include "areg/base/NEUtilities.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/String.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/trace/GETrace.h"

#include "extensions/console/Console.hpp"

#ifdef _WINDOWS
    #define MACRO_SCANF(fmt, data, len)     scanf_s(fmt, data, len)
#else   // _POSIX
    #define MACRO_SCANF(fmt, data, len)     scanf(fmt, data)
#endif  // _WINDOWS


//////////////////////////////////////////////////////////////////////////
// The model used only in console mode.
//////////////////////////////////////////////////////////////////////////

// This model defines a Console Service to run to make data rate outputs.
// The Console Service runs only in verbose mode.

static String _modelName("LoggerModel");

// Describe mode, set model name
BEGIN_MODEL(_modelName)

    // define console service thread.
    BEGIN_REGISTER_THREAD( "LoggerConsoleServiceThread", NECommon::WATCHDOG_IGNORE)
        // Define the console service
        BEGIN_REGISTER_COMPONENT( LoggerConsoleService::SERVICE_NAME, LoggerConsoleService)
            // register dummy 'empty service'.
            REGISTER_IMPLEMENT_SERVICE( NEService::EmptyServiceName, NEService::EmptyServiceVersion )
        // end of component description
        END_REGISTER_COMPONENT(LoggerConsoleService::SERVICE_NAME )
    // end of thread description
    END_REGISTER_THREAD( "LoggerConsoleServiceThread" )

// end of model description
END_MODEL(_modelName)

//////////////////////////////////////////////////////////////////////////
// Traces.
//////////////////////////////////////////////////////////////////////////

DEF_TRACE_SCOPE(mcrouter_app_Logger_serviceMain);
DEF_TRACE_SCOPE(mcrouter_app_Logger_serviceStart);
DEF_TRACE_SCOPE(mcrouter_app_Logger_serviceStop);
DEF_TRACE_SCOPE(mcrouter_app_Logger_servicePause);
DEF_TRACE_SCOPE(mcrouter_app_Logger_serviceContinue);
DEF_TRACE_SCOPE(mcrouter_app_Logger_serviceInstall);
DEF_TRACE_SCOPE(mcrouter_app_Logger_serviceUninstall);
DEF_TRACE_SCOPE(mcrouter_app_Logger_setState);

//////////////////////////////////////////////////////////////////////////
// Logger class implementation
//////////////////////////////////////////////////////////////////////////
Logger & Logger::getInstance(void)
{
    static Logger _logger;
    return _logger;
}

Logger::Logger( void )
    : mLoggerState  ( NELoggerSettings::eLoggerState::LoggerStopped )
    , mServiceCmd   ( NELoggerSettings::eServiceCommand::CMD_Undefined )
    , mRunVerbose   ( false )
    // , mServiceServer( )
    , mSvcHandle    ( nullptr )
    , mSeMHandle    ( nullptr )
{
}

Logger::~Logger( void )
{
    _osFreeResources( );
}

bool Logger::parseOptions(int argc, char** argv)
{
    using ServiceCmd = NELoggerSettings::eServiceCommand;
    bool result{ false };
    
    if (argc > 1)
    {
        bool isValid{ true };

        ServiceCmd foundCmd{ ServiceCmd::CMD_Undefined };
        bool isVerbose{ false };

        for (int i = 1; i < argc; ++i)
        {
            const char* opt = argv[i];
            if (opt != nullptr)
            {
                ServiceCmd cmd = NEApplication::parseOption< ServiceCmd, ServiceCmd::CMD_Undefined>(opt, NELoggerSettings::ServiceCommands, MACRO_ARRAYLEN(NELoggerSettings::ServiceCommands));

                if (cmd == ServiceCmd::CMD_Verbose)
                {
                    isVerbose = true;   // Output verbose.
                }
                else if (cmd == ServiceCmd::CMD_Undefined)
                {
                    isValid = false;    // Invalid option, break and return false.
                    break;
                }
                else if (foundCmd == ServiceCmd::CMD_Undefined)
                {
                    foundCmd = cmd;     // No option was set, set current.
                }
                else
                {
                    isValid = false;    // Already has an option, break and return false.
                    break;
                }
            }
        }

        if (isValid)
        {
            setCurrentCommand(foundCmd);
            mRunVerbose = isVerbose;
            result = true;
        }
    }
    else if (argc == 1)
    {
        resetDefaultOptions();
        result = true;
    }

    return result;
}

void Logger::serviceMain( int argc, char ** argv )
{
    // Start only tracing and timer manager.
    Application::initApplication(true, true, false, true, false, NEApplication::DEFAULT_TRACING_CONFIG_FILE.data(), nullptr );

    do 
    {
        TRACE_SCOPE(mcrouter_app_Logger_serviceMain);
        TRACE_DBG("Starting service. There are [ %d ] arguments in the list...", argc);

#ifdef  DEBUG
        for ( int i = 0; i < argc; ++ i )
            TRACE_DBG("... Command argument [ %d ]: [ %s ]", i, argv[i]);
#endif  // DEBUG

        if ( _osRegisterService() || (mServiceCmd == NELoggerSettings::eServiceCommand::CMD_Console) )
        {
            TRACE_DBG("Starting service");
            serviceStart();
        }

        if ( mServiceCmd == NELoggerSettings::eServiceCommand::CMD_Console )
        {
#if AREG_EXTENDED

            Console& console = Console::getInstance();

            if (mRunVerbose)
            {
                // Disable to block user input until Console Service is up and running.
                console.enableConsoleInput(false);
                Application::loadModel(_modelName);
                // Blocked until user input
                Console::CallBack callback(LoggerConsoleService::checkCommand);
                console.waitForInput(callback);
                Application::unloadModel(_modelName);
            }
            else
            {
                // No verbose mode.
                // Set local callback, output message and wait for user input.
                Console::CallBack callback(Logger::_checkCommand);
                console.enableConsoleInput(true);
                console.outputTxt(Console::Coord{ 0, 0 }, NELoggerSettings::FORMAT_WAIT_QUIT);
                console.waitForInput(callback);
            }

            console.moveCursorOneLineDown();
            console.printTxt(NELoggerSettings::FORMAT_QUIT_APP);
            console.uninitialize();

#else   // !AREG_EXTENDED

            printf("Type \'quit\' or \'q\' to quit message router ...: ");
            const char quit = static_cast<int>('q');
            char cmd[8] = { 0 };
            int charRead = 0;

            do
            {
                if (MACRO_SCANF("%4s", cmd, 8) != 1)
                {
                    printf("\nERROR: Unexpected choice of command, quit application ...\n");
                }

            } while ((NEString::makeAsciiLower<char>(*cmd) != quit) && (charRead > 0));

#endif  // !AREG_EXTENDED

            Application::signalAppQuit();
        }

        serviceStop();
        TRACE_WARN("Service Stopped and not running anymore");

        Application::releaseApplication();

    } while (false);
}

bool Logger::serviceStart(void)
{
    TRACE_SCOPE(mcrouter_app_Logger_serviceStart);
    TRACE_DBG("Starting service [ %s ]", NELoggerSettings::SERVICE_NAME_ASCII);
    bool result = false;

#if 0
    if (  mServiceServer.configureRemoteServicing( NEApplication::DEFAULT_ROUTER_CONFIG_FILE.data() ) && mServiceServer.startRemoteServicing() )
    {
        result = setState(NELoggerSettings::eLoggerState::LoggerRunning);
    }
    else
    {
        Application::signalAppQuit();
    }
#endif

    return result;
}

void Logger::serviceStop(void)
{
    TRACE_SCOPE(mcrouter_app_Logger_serviceStop);
    TRACE_WARN("Stopping service [ %s ]", NELoggerSettings::SERVICE_NAME_ASCII);
    setState(NELoggerSettings::eLoggerState::LoggerStopping);
    // mServiceServer.stopRemoteServicing();
    Application::signalAppQuit();
}

void Logger::servicePause(void)
{
    TRACE_SCOPE(mcrouter_app_Logger_servicePause);
    TRACE_DBG("Pausing Router service");

    setState( NELoggerSettings::eLoggerState::LoggerPausing );
    // mServiceServer.stopRemoteServicing();
    setState( NELoggerSettings::eLoggerState::LoggerPaused );
}

bool Logger::serviceContinue(void)
{
    TRACE_SCOPE(mcrouter_app_Logger_serviceContinue);
    TRACE_DBG("Continuing Logger service");

    bool result = false;
    setState( NELoggerSettings::eLoggerState::LoggerContinuing );
#if 0
    if ( mServiceServer.isRemoteServicingConfigured() && mServiceServer.startRemoteServicing() )
    {
        result = true;
        setState( NELoggerSettings::eLoggerState::LoggerRunning );
    }
    else
    {
        TRACE_ERR("Failed to restart remote servicing");
        Application::signalAppQuit();
    }
#endif

    return result;
}

bool Logger::serviceInstall(void)
{
    TRACE_SCOPE(mcrouter_app_Logger_serviceInstall);
    
    if ( _osOpenService() == false )
    {
        _osCcreateService();
    }

    return _osIsValid();
}

void Logger::serviceUninstall(void)
{
    TRACE_SCOPE(mcrouter_app_Logger_serviceUninstall);

    if ( _osOpenService() )
    {
        _osDeleteService();
    }

    _osFreeResources();
}

bool Logger::setState( NELoggerSettings::eLoggerState newState )
{
    TRACE_SCOPE( mcrouter_app_Logger_setState );
    TRACE_DBG( "Changing Service Logger state. Old state [ %s ], new state [ %s ]"
                , NELoggerSettings::getString( mLoggerState )
                , NELoggerSettings::getString( newState ) );

    return _osSetState( newState );
}

bool Logger::_checkCommand(const String& cmd)
{
    String command(cmd);
    command.makeLower();
    if ((command == NELoggerSettings::QUIT_CH) || (command == NELoggerSettings::QUIT_STR))
    {
        return true; // interrupt, requested quit
    }
    else
    {
        Console& console = Console::getInstance();

        ASSERT(Logger::getInstance().mRunVerbose == false);
        console.outputMsg(Console::Coord{ 0, 1 }, NELoggerSettings::FORMAT_MSG_ERROR.data(), cmd.getString());
        console.outputTxt(Console::Coord{ 0, 0 }, NELoggerSettings::FORMAT_WAIT_QUIT);
        console.refreshScreen();

        return false;
    }
}
