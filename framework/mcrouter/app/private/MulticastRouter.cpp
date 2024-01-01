/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        mcrouter/app/private/MulticastRouter.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Multi-cast routing to run as process or service.
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

#include <stdio.h>

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
        BEGIN_REGISTER_THREAD( "RouterConsoleServiceThread", NECommon::WATCHDOG_IGNORE )
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


    constexpr std::string_view _msgHelp []
    {
          {"Usage of AREG Message Router (mcrouter) :"}
        , NESystemService::MSG_SEPARATOR
        , {"-c, --console   : Command to run mcrouter as a console application (default option). Usage: \'mcrouter --console\'"}
        , {"-h, --help      : Command to display this message on console."}
        , {"-i, --install   : Command to install mcrouter as a service. Valid only for Windows OS. Usage: \'mcrouter --install\'"}
        , {"-l, --silent    : Command option to stop displaying data rate. Used in console application. Usage: --silent"}
        , {"-n, --instances : Command option to display list of connected instances. Used in console application. Usage: --instances"}
        , {"-p, --pause     : Command option to pause connection. Used in console application. Usage: --pause"}
        , {"-q, --quit      : Command option to stop router and quit application. Used in console application. Usage: --quit"}
        , {"-r, --restart   : Command option to restart connection. Used in console application. Usage: --restart"}
        , {"-s, --service   : Command to run mcrouter as a system service. Usage: \'mcrouter --service\'"}
        , {"-u, --uninstall : Command to uninstall mcrouter as a service. Valid only for Windows OS. Usage: \'mcrouter --uninstall\'"}
        , {"-v, --verbose   : Command option to display data rate. Used in console application. Usage: --verbose"}
        , NESystemService::MSG_SEPARATOR
    };
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
      { "-c", "--console"   , static_cast<int>(eRouterOptions::CMD_RouterConsole)   , OptionParser::NO_DATA , {}, {}, {} }
    , { "-h", "--help"      , static_cast<int>(eRouterOptions::CMD_RouterPrintHelp) , OptionParser::NO_DATA , {}, {}, {} }
    , { "-i", "--install"   , static_cast<int>(eRouterOptions::CMD_RouterInstall)   , OptionParser::NO_DATA , {}, {}, {} }
    , { "-l", "--silent"    , static_cast<int>(eRouterOptions::CMD_RouterSilent)    , OptionParser::NO_DATA , {}, {}, {} }
    , { "-n", "--instances" , static_cast<int>(eRouterOptions::CMD_RouterInstances) , OptionParser::NO_DATA , {}, {}, {} }
    , { "-p", "--pause"     , static_cast<int>(eRouterOptions::CMD_RouterPause)     , OptionParser::NO_DATA , {}, {}, {} }
    , { "-q", "--quit"      , static_cast<int>(eRouterOptions::CMD_RouterQuit)      , OptionParser::NO_DATA , {}, {}, {} }
    , { "-r", "--restart"   , static_cast<int>(eRouterOptions::CMD_RouterRestart)   , OptionParser::NO_DATA , {}, {}, {} }
    , { "-s", "--service"   , static_cast<int>(eRouterOptions::CMD_RouterService)   , OptionParser::NO_DATA , {}, {}, {} }
    , { "-u", "--uninstall" , static_cast<int>(eRouterOptions::CMD_RouterUninstall) , OptionParser::NO_DATA , {}, {}, {} }
    , { "-v", "--verbose"   , static_cast<int>(eRouterOptions::CMD_RouterVerbose)   , OptionParser::NO_DATA , {}, {}, {} }
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
    MulticastRouter::_outputTitle( );

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
        console.outputTxt( NESystemService::COORD_USER_INPUT, NESystemService::FORMAT_WAIT_QUIT );
        console.waitForInput( getOptionCheckCallback( ) );
    }

    console.moveCursorOneLineDown( );
    console.clearScreen( );
    console.uninitialize( );

#endif   // !AREG_EXTENDED
}

void MulticastRouter::runConsoleInputSimple( void )
{
    char cmd[ 512 ]{ 0 };
    bool quit{ false };
    OptionParser parser( MulticastRouter::ValidOptions, MACRO_ARRAYLEN( MulticastRouter::ValidOptions ) );

    MulticastRouter::_outputTitle( );

    do
    {
        printf( "%s", NESystemService::FORMAT_WAIT_QUIT.data( ) );
        if (_osWaitUserInput(cmd, 512) == false)
            continue;

        quit = MulticastRouter::_checkCommand( cmd );

    } while ( quit == false );
}

void MulticastRouter::serviceMain( int argc, char ** argv )
{
    // Start only tracing and timer manager.
    Application::initApplication(true, true, false, true, false, NEApplication::DEFAULT_CONFIG_FILE.data() );
    SystemServiceBase::serviceMain( argc, argv );
    setState(NESystemService::eSystemServiceState::ServiceStopped);
    mServiceServer.waitToComplete( );
    Application::releaseApplication( );
}

bool MulticastRouter::serviceStart(void)
{
    TRACE_SCOPE(mcrouter_app_MulticastRouter_serviceStart);
    TRACE_DBG("Starting service [ %s ]", NEMulticastRouterSettings::SERVICE_NAME_ASCII);
    bool result{ false };
    IEServiceConnectionProvider& service{ getService() };
    if (service.setupServiceConnectionData(NERemoteService::eRemoteServices::ServiceRouter, static_cast<uint32_t>(NERemoteService::eConnectionTypes::ConnectTcpip)) &&
        service.connectServiceHost())
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
    getService().disconnectServiceHost();
    mServiceServer.waitToComplete( );
    Application::signalAppQuit();
}

void MulticastRouter::servicePause(void)
{
    TRACE_SCOPE(mcrouter_app_MulticastRouter_servicePause);
    TRACE_DBG("Pausing Message Router service");

    setState( NESystemService::eSystemServiceState::ServicePausing );
    getService( ).disconnectServiceHost();
    mServiceServer.waitToComplete( );
    setState( NESystemService::eSystemServiceState::ServicePaused );
}

bool MulticastRouter::serviceContinue(void)
{
    TRACE_SCOPE(mcrouter_app_MulticastRouter_serviceContinue);
    TRACE_DBG("Continuing Message Router service");

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
    TRACE_DBG( "Changing Message Router service state. Old state [ %s ], new state [ %s ]"
                , NESystemService::getString( mSystemServiceState )
                , NESystemService::getString( newState ) );

    return _osSetState( newState );
}

void MulticastRouter::printHelp( bool isCmdLine )
{
#if     AREG_EXTENDED

    Console::Coord line{ NESystemService::COORD_INFO_MSG };
    Console& console = Console::getInstance();
    console.lockConsole();
    for (const auto& text : _msgHelp)
    {
        console.outputTxt(line, text);
        ++line.posY;
    }

    console.unlockConsole();

#else   // AREG_EXTENDED

    for (const auto& line : _msgHelp)
    {
        std::cout << line << std::endl;
    }

    std::cout << std::ends;

#endif  // AREG_EXTENDED
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

    MulticastRouter::_cleanHelp();

    if ( parser.parseOptionLine( cmd ) )
    {
        MulticastRouter & router = MulticastRouter::getInstance( );
        const OptionParser::InputOptionList & opts = parser.getOptions( );
        for (uint32_t i = 0; i < opts.getSize( ); ++ i )
        {
            const OptionParser::sOption & opt = opts[ i ];
            switch ( static_cast<eRouterOptions>(opt.inCommand) )
            {
            case eRouterOptions::CMD_RouterPause:
                MulticastRouter::_outputInfo( "Pausing message router ..." );
                router.getService().disconnectServiceHost( );
                router.mServiceServer.waitToComplete( );
                MulticastRouter::_outputInfo( "Message router is paused ..." );
                break;

            case eRouterOptions::CMD_RouterRestart:
                MulticastRouter::_outputInfo( "Restarting message router ..." );
                router.getService( ).connectServiceHost( );
                MulticastRouter::_outputInfo( "Message router is restarted ..." );
                break;

            case eRouterOptions::CMD_RouterInstances:
                MulticastRouter::_outputInstances( router.getConnetedInstances() );
                break;

            case eRouterOptions::CMD_RouterVerbose:
                MulticastRouter::_setVerboseMode( true );
                break;

            case eRouterOptions::CMD_RouterSilent:
                MulticastRouter::_setVerboseMode( false );
                break;

            case eRouterOptions::CMD_RouterPrintHelp:
                router.printHelp( false );
                break;

            case eRouterOptions::CMD_RouterQuit:
                quit = true;
                break;

            case eRouterOptions::CMD_RouterConsole:     // pass through
            case eRouterOptions::CMD_RouterInstall:     // pass through
            case eRouterOptions::CMD_RouterUninstall:   // pass through
            case eRouterOptions::CMD_RouterService:
                MulticastRouter::_outputInfo("This command should be used in command line ...");
                break;

            default:
                hasError = true;
                break;
            }
        }
    }
    else
    {
        hasError = true;
    }

#if AREG_EXTENDED
    
    Console & console = Console::getInstance( );
    console.lockConsole();
    if ( quit == false )
    {
        if ( hasError )
        {
            console.outputMsg( NESystemService::COORD_ERROR_MSG, NESystemService::FORMAT_MSG_ERROR.data( ), cmd.getString( ) );
        }

        console.clearLine( NESystemService::COORD_USER_INPUT );
        console.outputTxt( NESystemService::COORD_USER_INPUT, NESystemService::FORMAT_WAIT_QUIT );
    }
    else
    {
        console.outputTxt( NESystemService::COORD_INFO_MSG, NESystemService::FORMAT_QUIT_APP );
    }

    console.refreshScreen( );
    console.unlockConsole( );

#else   // !AREG_EXTENDED

    if ( quit == false )
    {
        if ( hasError )
        {
            printf( NESystemService::FORMAT_MSG_ERROR.data( ), cmd.getString() );
            printf( "\n" );
        }
    }
    else
    {
        printf( "%s\n", NESystemService::FORMAT_QUIT_APP.data( ) );
    }

#endif  // AREG_EXTENDED

    return quit;
}

void MulticastRouter::_outputTitle( void )
{
#if AREG_EXTENDED

    Console & console = Console::getInstance( );
    console.lockConsole();
    console.outputTxt( NESystemService::COORD_TITLE, NEMulticastRouterSettings::APP_TITLE.data( ) );
    console.outputTxt( NESystemService::COORD_SUBTITLE, NESystemService::MSG_SEPARATOR.data( ) );
    console.unlockConsole();

#else   // !AREG_EXTENDED

    printf( "%s\n", NEMulticastRouterSettings::APP_TITLE.data( ) );
    printf( "%s\n", NESystemService::MSG_SEPARATOR.data( ) );

#endif  // AREG_EXTENDED
}

void MulticastRouter::_outputInfo( const String & info )
{
#if AREG_EXTENDED

    Console & console = Console::getInstance( );
    Console::Coord coord{NESystemService::COORD_INFO_MSG};
    console.lockConsole( );

    console.outputTxt( coord, NESystemService::MSG_SEPARATOR.data( ) );
    ++ coord.posY;
    console.outputStr( coord, info );

    console.unlockConsole( );

#else   // !AREG_EXTENDED

    printf( "%s\n", info.getString() );

#endif  // AREG_EXTENDED
}

void MulticastRouter::_outputInstances( const ServiceCommunicatonBase::MapInstances & instances )
{
    static constexpr std::string_view _table{ "   Nr. |  Instance ID  |  Bitness  |  Name " };
    static constexpr std::string_view _empty{ "There are no connected instances ..." };

#if AREG_EXTENDED

    Console & console = Console::getInstance( );
    Console::Coord coord{NESystemService::COORD_INFO_MSG};
    console.lockConsole( );

    if ( instances.isEmpty( ) )
    {
        console.outputTxt( coord, NESystemService::MSG_SEPARATOR.data( ) );
        ++ coord.posY;
        console.outputStr( coord, _empty );
        ++ coord.posY;
    }
    else
    {
        console.outputTxt( coord, NESystemService::MSG_SEPARATOR.data( ) );
        ++ coord.posY;
        console.outputTxt( coord, _table );
        ++ coord.posY;
        console.outputTxt( coord, NESystemService::MSG_SEPARATOR.data( ) );
        ++ coord.posY;
        int i{ 1 };
        for ( auto pos = instances.firstPosition( ); instances.isValidPosition( pos ); pos = instances.nextPosition( pos ) )
        {
            ITEM_ID cookie{ 0 };
            NEService::sServiceConnectedInstance instance;
            instances.getAtPosition( pos, cookie, instance);
            unsigned int id{ static_cast<unsigned int>(cookie) };

            console.outputMsg(coord, " %4d. |  %11u  |    %u     |  %s ", i++, id, static_cast<uint32_t>(instance.ciBitness), instance.ciInstance.getString());
            ++ coord.posY;
        }
    }

    console.outputTxt( coord, NESystemService::MSG_SEPARATOR.data( ) );
    console.unlockConsole( );

#else   // !AREG_EXTENDED

    if ( instances.isEmpty( ) )
    {
        printf( "%s\n", _empty.data() );
    }
    else
    {
        printf( "%s\n", NESystemService::MSG_SEPARATOR.data( ) );
        printf( "%s\n", _table.data() );
        printf( "%s\n", NESystemService::MSG_SEPARATOR.data( ) );

        int i{ 1 };
        for ( auto pos = instances.firstPosition( ); instances.isValidPosition( pos ); pos = instances.nextPosition( pos ) )
        {
            ITEM_ID cookie{ 0 };
            NEService::sServiceConnectedInstance instance;
            instances.getAtPosition( pos, cookie, instance);
            unsigned int id{ static_cast<unsigned int>(cookie) };

            printf(" %4d. |  %11u  |    %u     |  %s \n", i++, id, static_cast<uint32_t>(instance.ciBitness), instance.ciInstance.getString());
        }
    }

    printf( "%s\n", NESystemService::MSG_SEPARATOR.data( ) );

#endif  // AREG_EXTENDED
}

void MulticastRouter::_setVerboseMode( bool makeVerbose )
{
#if AREG_EXTENDED

    static constexpr std::string_view _verbose{ "Switching to verbose mode to output data rate ..." };
    static constexpr std::string_view _silence{ "Switching to silent mode, no data rate output ..." };
    MulticastRouter & router = MulticastRouter::getInstance( );
    Console & console = Console::getInstance( );
    console.lockConsole( );
    if ( router.mRunVerbose != makeVerbose )
    {
        router.mRunVerbose = makeVerbose;

        if ( makeVerbose == false )
        {
            console.clearLine( NESystemService::COORD_SEND_RATE );
            console.clearLine( NESystemService::COORD_RECV_RATE );
            console.outputTxt( NESystemService::COORD_INFO_MSG, _silence );
        }
        else
        {
            console.outputMsg( NESystemService::COORD_SEND_RATE, NESystemService::FORMAT_SEND_DATA.data( ), 0.0f, SystemServiceConsole::MSG_BYTES.data( ) );
            console.outputMsg( NESystemService::COORD_RECV_RATE, NESystemService::FORMAT_RECV_DATA.data( ), 0.0f, SystemServiceConsole::MSG_BYTES.data( ) );
            console.outputTxt( NESystemService::COORD_INFO_MSG, _verbose);
        }

        console.refreshScreen( );
    }

    console.unlockConsole( );

#else   // !AREG_EXTENDED

    static constexpr std::string_view _unsupported{"This option is available only with extended features"};
    printf( "%s\n", _unsupported.data( ) );

#endif  // AREG_EXTENDED
}

void MulticastRouter::_cleanHelp(void)
{
#if     AREG_EXTENDED

    Console::Coord line{ NESystemService::COORD_INFO_MSG };
    Console& console = Console::getInstance();
    console.lockConsole();

    console.clearLine(NESystemService::COORD_USER_INPUT);
    uint32_t count = MACRO_ARRAYLEN(_msgHelp);
    for (uint32_t i = 0; i < count; ++ i)
    {
        console.clearLine(line);
        ++line.posY;
    }

    console.unlockConsole();

#endif  // AREG_EXTENDED
}
