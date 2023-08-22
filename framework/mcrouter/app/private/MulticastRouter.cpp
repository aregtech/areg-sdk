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

#include "extend/console/Console.hpp"

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

namespace
{
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
}

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

const OptionParser::sOptionSetup MulticastRouter::ValidOptions[ ]
{
      { "-p", "--pause"     , static_cast<int>(eRouterOptions::CMD_RouterPause)     , OptionParser::NO_DATA , {}, {}, {} }
    , { "-r", "--restart"   , static_cast<int>(eRouterOptions::CMD_RouterRestart)   , OptionParser::NO_DATA , {}, {}, {} }
    , { "-i", "--instances" , static_cast<int>(eRouterOptions::CMD_RouterInstances) , OptionParser::NO_DATA , {}, {}, {} }
    , { "-h", "--help"      , static_cast<int>(eRouterOptions::CMD_RouterPrintHelp) , OptionParser::NO_DATA , {}, {}, {} }
    , { "-q", "--quit"      , static_cast<int>(eRouterOptions::CMD_RouterQuit)      , OptionParser::NO_DATA , {}, {}, {} }
};

MulticastRouter & MulticastRouter::getInstance(void)
{
    static MulticastRouter _messageRouter;
    return _messageRouter;
}

std::pair<const OptionParser::sOptionSetup *, int> MulticastRouter::getOptionSetup( void )
{
    return std::pair< const OptionParser::sOptionSetup *, int>(MulticastRouter::ValidOptions, static_cast<int>(MACRO_ARRAYLEN( MulticastRouter::ValidOptions )));
}


MulticastRouter::MulticastRouter( void )
    : SystemServiceBase ( mServiceServer )
    , mServiceServer    ( )
{
}

MulticastRouter::~MulticastRouter( void )
{
    _osFreeResources( );
}

Console::CallBack MulticastRouter::getOptionCheckCallback( void ) const
{
    return Console::CallBack( MulticastRouter::_checkCommand );
}

void MulticastRouter::runConsoleInputExtended( void )
{
#if AREG_EXTENDED
    Console & console = Console::getInstance( );

    if ( mRunVerbose )
    {
        // Disable to block user input until Console Service is up and running.
        console.enableConsoleInput( false );
        startConsoleService( );
        // Blocked until user input
        console.waitForInput( getOptionCheckCallback( ) );
        stopConsoleService( );
    }
    else
    {
        // No verbose mode.
        // Set local callback, output message and wait for user input.
        console.enableConsoleInput( true );
        console.outputTxt( Console::Coord{ 0, 0 }, NESystemService::FORMAT_WAIT_QUIT );
        console.waitForInput( getOptionCheckCallback( ) );
    }

    console.moveCursorOneLineDown( );
    console.printTxt( NESystemService::FORMAT_QUIT_APP );
    console.uninitialize( );
#endif   // !AREG_EXTENDED
}

void MulticastRouter::runConsoleInputSimple( void )
{
    char cmd[ 512 ]{ 0 };
    bool quit{ false };
    OptionParser parser( MulticastRouter::ValidOptions, MACRO_ARRAYLEN( MulticastRouter::ValidOptions ) );

    do
    {
        printf( NESystemService::FORMAT_WAIT_QUIT.data( ) );
        MACRO_SCANF( "%512s", cmd, 512 );
        if ( parser.parseOptionLine( cmd ) )
        {
            const OptionParser::InputOptionList & opts = parser.getOptions( );
            for ( int i = 0; i < static_cast<int>(opts.getSize( )); ++ i )
            {
                const OptionParser::sOption & opt = opts[ 0 ];
                switch ( static_cast<MulticastRouter::eRouterOptions>(opt.inCommand) )
                {
                case MulticastRouter::eRouterOptions::CMD_RouterQuit:
                    quit = true;
                    break;

                default:
                    break;
                }
            }
        }
        else
        {
            printf( NESystemService::FORMAT_MSG_ERROR.data( ), cmd );
            printf( "\n" );
        }

    } while ( quit == false );
}

void MulticastRouter::serviceMain( int argc, char ** argv )
{
    // Start only tracing and timer manager.
    Application::initApplication(true, true, false, true, false, NEApplication::DEFAULT_TRACING_CONFIG_FILE.data(), nullptr );
    SystemServiceBase::serviceMain( argc, argv );
    Application::releaseApplication( );
}

bool MulticastRouter::serviceStart(void)
{
    TRACE_SCOPE(mcrouter_app_MulticastRouter_serviceStart);
    TRACE_DBG("Starting service [ %s ]", NEMulticastRouterSettings::SERVICE_NAME_ASCII);
    bool result{ false };
    if ( getService().setupServiceConnectionHost( NEApplication::DEFAULT_ROUTER_CONFIG_FILE.data( ) ) && getService( ).connectServiceHost( ) )
    {
        result = setState(NESystemService::eSystemServiceState::ServiceRunning);
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
    setState(NESystemService::eSystemServiceState::ServiceStopping);
    getService( ).disconnectServiceHost();
    Application::signalAppQuit();
}

void MulticastRouter::servicePause(void)
{
    TRACE_SCOPE(mcrouter_app_MulticastRouter_servicePause);
    TRACE_DBG("Pausing Router service");

    setState( NESystemService::eSystemServiceState::ServicePausing );
    getService( ).disconnectServiceHost();
    setState( NESystemService::eSystemServiceState::ServicePaused );
}

bool MulticastRouter::serviceContinue(void)
{
    TRACE_SCOPE(mcrouter_app_MulticastRouter_serviceContinue);
    TRACE_DBG("Continuing Router service");

    bool result = false;
    setState( NESystemService::eSystemServiceState::ServiceContinuing );
    if ( getService( ).isServiceHostSetup() && getService( ).connectServiceHost() )
    {
        result = true;
        setState( NESystemService::eSystemServiceState::ServiceRunning );
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

bool MulticastRouter::registerService( void )
{
    return _osRegisterService();
}

bool MulticastRouter::serviceOpen( void )
{
    return _osOpenService( );
}

bool MulticastRouter::setState( NESystemService::eSystemServiceState newState )
{
    TRACE_SCOPE( mcrouter_app_MulticastRouter_setState );
    TRACE_DBG( "Changing Service Router state. Old state [ %s ], new state [ %s ]"
                , NESystemService::getString( mSystemServiceState )
                , NESystemService::getString( newState ) );

    return _osSetState( newState );
}

void MulticastRouter::printHelp( void )
{
    std::cout
        << "Usage of AREG Message Router (mcrouter) :" << std::endl
        << "---------------------------------------------------------------------------------------------" << std::endl
        << "-i, --install   : Command to install service. Valid for Windows OS, ignored in other cases." << std::endl
        << "-u, --uninstall : Command to uninstall service. Valid for Windows OS, ignored in other cases." << std::endl
        << "-s, --service   : Command to run process as a system service process." << std::endl
        << "-c, --console   : Command to run process as a console application." << std::endl
        << "-v, --verbose   : Command to display data rate. Can be used in combination with \'--console\'" << std::endl
        << "-h, --help      : Command to display this message on console." << std::endl
        << "---------------------------------------------------------------------------------------------" << std::endl
        << "Quit application ..." << std::endl << std::ends;
}

void MulticastRouter::startConsoleService( void )
{
    Application::loadModel( _modelName );
}

void MulticastRouter::stopConsoleService( void )
{
    Application::unloadModel( _modelName );
}

bool MulticastRouter::_checkCommand(const String& cmd)
{
    OptionParser parser( MulticastRouter::ValidOptions, static_cast<int>(MACRO_ARRAYLEN( MulticastRouter::ValidOptions )) );
    bool quit{ false };
    bool hasError{ false };

    if ( parser.parseOptionLine( cmd ) )
    {
        const OptionParser::InputOptionList & opts = parser.getOptions( );
        for ( int i = 0; i < static_cast<int>(opts.getSize( )); ++ i )
        {
            const OptionParser::sOption & opt = opts[ 0 ];
            switch ( static_cast<eRouterOptions>(opt.inCommand) )
            {
            case eRouterOptions::CMD_RouterQuit:
                quit = true;
                break;

            default:
                break;
            }
        }
    }
    else
    {
        hasError = true;
    }

    if ( quit == false )
    {
        MulticastRouter & router = MulticastRouter::getInstance( );
        Console & console = Console::getInstance( );

        console.clearScreen( );
        if ( hasError )
        {
            console.outputMsg( router.mRunVerbose ? NESystemService::COORD_ERROR_MSG : Console::Coord{ 0, 1 }
                             , NESystemService::FORMAT_MSG_ERROR.data( )
                             , cmd.getString( ) );
        }

        console.outputTxt( router.mRunVerbose ? NESystemService::COORD_USER_INPUT : Console::Coord{ 0, 0 }
                         , NESystemService::FORMAT_WAIT_QUIT );
        console.refreshScreen( );
    }

    return quit;
}
