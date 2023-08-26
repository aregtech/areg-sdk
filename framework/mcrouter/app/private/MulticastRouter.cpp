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
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
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
    , { "-v", "--verbose"   , static_cast<int>(eRouterOptions::CMD_RouterVerbose)   , OptionParser::NO_DATA , {}, {}, {} }
    , { "-n", "--silent"    , static_cast<int>(eRouterOptions::CMD_RouterSilent)    , OptionParser::NO_DATA , {}, {}, {} }
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
        int count = MACRO_SCANF( "%510s", cmd, 512 );
        if ((count <= 0) || (count >= 512))
            continue;

        quit = MulticastRouter::_checkCommand( cmd );

    } while ( quit == false );
}

void MulticastRouter::serviceMain( int argc, char ** argv )
{
    // Start only tracing and timer manager.
    Application::initApplication(true, true, false, true, false, NEApplication::DEFAULT_TRACING_CONFIG_FILE.data(), nullptr );
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
    getService().disconnectServiceHost();
    Application::signalAppQuit();
}

void MulticastRouter::servicePause(void)
{
    TRACE_SCOPE(mcrouter_app_MulticastRouter_servicePause);
    TRACE_DBG("Pausing Message Router service");

    setState( NESystemService::eSystemServiceState::ServicePausing );
    getService( ).disconnectServiceHost();
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
        MulticastRouter & router = MulticastRouter::getInstance( );
        const OptionParser::InputOptionList & opts = parser.getOptions( );
        for ( int i = 0; i < static_cast<int>(opts.getSize( )); ++ i )
        {
            const OptionParser::sOption & opt = opts[ 0 ];
            switch ( static_cast<eRouterOptions>(opt.inCommand) )
            {
            case eRouterOptions::CMD_RouterPause:
                MulticastRouter::_outputInfo( "Pausing message router ..." );
                router.getService().disconnectServiceHost( );
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
            console.clearLine( NESystemService::COORD_ERROR_MSG );
            console.clearLine( NESystemService::COORD_INFO_MSG );
            console.outputMsg( NESystemService::COORD_ERROR_MSG, NESystemService::FORMAT_MSG_ERROR.data( ), cmd.getString( ) );
        }
        else
        {
            console.clearLine( NESystemService::COORD_ERROR_MSG );
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
    console.outputTxt( NESystemService::COORD_TITLE, NEMulticastRouterSettings::APP_TITLE.data( ) );
    console.outputTxt( NESystemService::COORD_SUBTITLE, NESystemService::MSG_SEPARATOR.data( ) );
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

void MulticastRouter::_outputInstances( const ServiceCommunicatonBase::InstanceMap & instances )
{
    static constexpr std::string_view _table{ "   Nr. |  Instance ID  |  Name " };
    static constexpr std::string_view _empty{ "There are no connected instances ..." };

#if AREG_EXTENDED

    Console & console = Console::getInstance( );
    Console::Coord coord{NESystemService::COORD_INFO_MSG};
    console.lockConsole( );

#endif // AREG_EXTENDED

    if ( instances.isEmpty( ) )
    {
#if AREG_EXTENDED

        console.outputTxt( coord, NESystemService::MSG_SEPARATOR.data( ) );
        ++ coord.posY;
        console.outputStr( coord, _empty );

#else   // !AREG_EXTENDED

        printf( "%s\n", _empty.data() );

#endif  // AREG_EXTENDED
    }
    else
    {
#if AREG_EXTENDED

        console.outputTxt( coord, NESystemService::MSG_SEPARATOR.data( ) );
        ++ coord.posY;
        console.outputTxt( coord, _table );
        ++ coord.posY;
        console.outputTxt( coord, NESystemService::MSG_SEPARATOR.data( ) );
        ++ coord.posY;

#else   // !AREG_EXTENDED

        printf( "%s\n", NESystemService::MSG_SEPARATOR.data( ) );
        printf( "%s\n", _table.data() );
        printf( "%s\n", NESystemService::MSG_SEPARATOR.data( ) );

#endif  // AREG_EXTENDED

        int i{ 1 };
        for ( auto pos = instances.firstPosition( ); instances.isValidPosition( pos ); pos = instances.nextPosition( pos ) )
        {
            ITEM_ID cookie{ 0 };
            String name;
            instances.getAtPosition( pos, cookie, name );
            unsigned int id{ static_cast<unsigned int>(cookie) };

#if AREG_EXTENDED

            console.outputMsg( coord, " %4d. |  %11u  |  %s ", i ++, id, name.getString( ) );
            ++ coord.posY;

#else   // !AREG_EXTENDED

            printf( " %4d. |  %11u  |  %s \n", i ++, id, name.getString( ) );

#endif  // AREG_EXTENDED
        }

        console.outputTxt( coord, NESystemService::MSG_SEPARATOR.data( ) );
    }

#if AREG_EXTENDED

    console.unlockConsole( );

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
