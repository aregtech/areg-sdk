/************************************************************************
 * \file        mcrouter/app/private/MulticastRouter.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       Router, Multicast Router Service process common part
 ************************************************************************/

#include "mcrouter/app/MulticastRouter.hpp"
#include "mcrouter/app/private/RouterConsoleService.hpp"

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

static String _modelName("MCRouterModel");

// Describe mode, set model name
BEGIN_MODEL(_modelName)

    // define console service thread.
    BEGIN_REGISTER_THREAD( "RouterConsoleServiceThread", NECommon::WATCHDOG_IGNORE)
        // Define the console service
        BEGIN_REGISTER_COMPONENT(RouterConsoleService::SERVICE_NAME, RouterConsoleService)
            // register dummy 'empty service'.
            REGISTER_IMPLEMENT_SERVICE( NEService::EmptyServiceName, NEService::EmptyServiceVersion )
        // end of component description
        END_REGISTER_COMPONENT(RouterConsoleService::SERVICE_NAME )
    // end of thread description
    END_REGISTER_THREAD( "RouterConsoleServiceThread" )

// end of model description
END_MODEL(_modelName)

//////////////////////////////////////////////////////////////////////////
// Traces.
//////////////////////////////////////////////////////////////////////////

DEF_TRACE_SCOPE(mcrouter_app_MulticastRouter_serviceMain);
DEF_TRACE_SCOPE(mcrouter_app_MulticastRouter_serviceStart);
DEF_TRACE_SCOPE(mcrouter_app_MulticastRouter_serviceStop);
DEF_TRACE_SCOPE(mcrouter_app_MulticastRouter_servicePause);
DEF_TRACE_SCOPE(mcrouter_app_MulticastRouter_serviceContinue);
DEF_TRACE_SCOPE(mcrouter_app_MulticastRouter_serviceInstall);
DEF_TRACE_SCOPE(mcrouter_app_MulticastRouter_serviceUninstall);
DEF_TRACE_SCOPE(mcrouter_app_MulticastRouter_setState);

//////////////////////////////////////////////////////////////////////////
// MulticastRouter class implementation
//////////////////////////////////////////////////////////////////////////
MulticastRouter & MulticastRouter::getInstance(void)
{
    static MulticastRouter _messageRouter;
    return _messageRouter;
}

MulticastRouter::MulticastRouter( void )
    : mRouterState  ( NEMulticastRouterSettings::eRouterState::RouterStopped )
    , mServiceCmd   ( NEMulticastRouterSettings::eServiceCommand::CMD_Undefined )
    , mRunVerbose   ( false )
    , mServiceServer( )
    , mSvcHandle    ( nullptr )
    , mSeMHandle    ( nullptr )
{
}

MulticastRouter::~MulticastRouter( void )
{
    _osFreeResources( );
}

bool MulticastRouter::parseOptions(int argc, char** argv)
{
    using ServiceCmd = NEMulticastRouterSettings::eServiceCommand;
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
                ServiceCmd cmd = NEApplication::parseOption< ServiceCmd, ServiceCmd::CMD_Undefined>(opt, NEMulticastRouterSettings::ServiceCommands, MACRO_ARRAYLEN(NEMulticastRouterSettings::ServiceCommands));

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

void MulticastRouter::serviceMain( int argc, char ** argv )
{
    // Start only tracing and timer manager.
    Application::initApplication(true, true, false, true, false, NEApplication::DEFAULT_TRACING_CONFIG_FILE.data(), nullptr );

    do 
    {
        TRACE_SCOPE(mcrouter_app_MulticastRouter_serviceMain);
        TRACE_DBG("Starting service. There are [ %d ] arguments in the list...", argc);

#ifdef  DEBUG
        for ( int i = 0; i < argc; ++ i )
            TRACE_DBG("... Command argument [ %d ]: [ %s ]", i, argv[i]);
#endif  // DEBUG

        if ( _osRegisterService() || (mServiceCmd == NEMulticastRouterSettings::eServiceCommand::CMD_Console) )
        {
            TRACE_DBG("Starting service");
            serviceStart();
        }

        if ( mServiceCmd == NEMulticastRouterSettings::eServiceCommand::CMD_Console )
        {
#if AREG_EXTENDED

            Console& console = Console::getInstance();

            if (mRunVerbose)
            {
                // Disable to block user input until Console Service is up and running.
                console.enableConsoleInput(false);
                Application::loadModel(_modelName);
                // Blocked until user input
                Console::CallBack callback(RouterConsoleService::checkCommand);
                console.waitForInput(callback);
                Application::unloadModel(_modelName);
            }
            else
            {
                // No verbose mode.
                // Set local callback, output message and wait for user input.
                Console::CallBack callback(MulticastRouter::_checkCommand);
                console.enableConsoleInput(true);
                console.outputTxt(Console::Coord{ 0, 0 }, NEMulticastRouterSettings::FORMAT_WAIT_QUIT);
                console.waitForInput(callback);
            }

            console.moveCursorOneLineDown();
            console.printTxt(NEMulticastRouterSettings::FORMAT_QUIT_APP);
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

bool MulticastRouter::serviceStart(void)
{
    TRACE_SCOPE(mcrouter_app_MulticastRouter_serviceStart);
    TRACE_DBG("Starting service [ %s ]", NEMulticastRouterSettings::SERVICE_NAME_ASCII);
    bool result{ false };
    if ( getService().setupServiceConnectionHost( NEApplication::DEFAULT_ROUTER_CONFIG_FILE.data( ) ) && getService( ).connectServiceHost( ) )
    {
        result = setState(NEMulticastRouterSettings::eRouterState::RouterRunning);
    }
    else
    {
        Application::signalAppQuit();
    }

    return result;
}

void MulticastRouter::serviceStop(void)
{
    TRACE_SCOPE(mcrouter_app_MulticastRouter_serviceStop);
    TRACE_WARN("Stopping service [ %s ]", NEMulticastRouterSettings::SERVICE_NAME_ASCII);
    setState(NEMulticastRouterSettings::eRouterState::RouterStopping);
    getService( ).disconnectServiceHost();
    Application::signalAppQuit();
}

void MulticastRouter::servicePause(void)
{
    TRACE_SCOPE(mcrouter_app_MulticastRouter_servicePause);
    TRACE_DBG("Pausing Router service");

    setState( NEMulticastRouterSettings::eRouterState::RouterPausing );
    getService( ).disconnectServiceHost();
    setState( NEMulticastRouterSettings::eRouterState::RouterPaused );
}

bool MulticastRouter::serviceContinue(void)
{
    TRACE_SCOPE(mcrouter_app_MulticastRouter_serviceContinue);
    TRACE_DBG("Continuing Router service");

    bool result = false;
    setState( NEMulticastRouterSettings::eRouterState::RouterContinuing );
    if ( getService( ).isServiceHostSetup() && getService( ).connectServiceHost() )
    {
        result = true;
        setState( NEMulticastRouterSettings::eRouterState::RouterRunning );
    }
    else
    {
        TRACE_ERR("Failed to restart remote servicing");
        Application::signalAppQuit();
    }

    return result;
}

bool MulticastRouter::serviceInstall(void)
{
    TRACE_SCOPE(mcrouter_app_MulticastRouter_serviceInstall);
    
    if ( _osOpenService() == false )
    {
        _osCcreateService();
    }

    return _osIsValid();
}

void MulticastRouter::serviceUninstall(void)
{
    TRACE_SCOPE(mcrouter_app_MulticastRouter_serviceUninstall);

    if ( _osOpenService() )
    {
        _osDeleteService();
    }

    _osFreeResources();
}

bool MulticastRouter::setState( NEMulticastRouterSettings::eRouterState newState )
{
    TRACE_SCOPE( mcrouter_app_MulticastRouter_setState );
    TRACE_DBG( "Changing Service Router state. Old state [ %s ], new state [ %s ]"
                , NEMulticastRouterSettings::getString( mRouterState )
                , NEMulticastRouterSettings::getString( newState ) );

    return _osSetState( newState );
}

bool MulticastRouter::_checkCommand(const String& cmd)
{
    String command(cmd);
    command.makeLower();
    if ((command == NEMulticastRouterSettings::QUIT_CH) || (command == NEMulticastRouterSettings::QUIT_STR))
    {
        return true; // interrupt, requested quit
    }
    else
    {
        Console& console = Console::getInstance();

        ASSERT(MulticastRouter::getInstance().mRunVerbose == false);
        console.outputMsg(Console::Coord{ 0, 1 }, NEMulticastRouterSettings::FORMAT_MSG_ERROR.data(), cmd.getString());
        console.outputTxt(Console::Coord{ 0, 0 }, NEMulticastRouterSettings::FORMAT_WAIT_QUIT);
        console.refreshScreen();

        return false;
    }
}
