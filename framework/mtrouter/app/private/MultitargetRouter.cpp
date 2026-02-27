/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        mtrouter/app/private/MultitargetRouter.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Multi-cast routing to run as process or service.
 ************************************************************************/

#include "mtrouter/app/MultitargetRouter.hpp"
#include "mtrouter/app/private/RouterConsoleService.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/appbase/AppDefs.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/String.hpp"
#include "areg/logging/GELog.h"

#include "aregextend/console/Console.hpp"

#include <stdio.h>

//////////////////////////////////////////////////////////////////////////
// The model used only in console mode.
//////////////////////////////////////////////////////////////////////////

// This model defines a Console Service to run to make data rate outputs.
// The Console Service runs only in verbose mode.

static String _modelName("mtrouterModel");

// Describe mode, set model name
BEGIN_MODEL(_modelName)

    // define console service thread.
    BEGIN_REGISTER_THREAD( "RouterConsoleServiceThread" )
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
        
namespace
{
    constexpr std::string_view _msgHelp []
    {
          {"Usage of Areg Multi-target Message Router (mtrouter) :"}
        , NESystemService::MSG_SEPARATOR
        , {"-c, --console   : Command to run mtrouter as a console application (default option). Usage: \'mtrouter --console\'"}
        , {"-h, --help      : Command to display this message on console."}
        , {"-i, --install   : Command to install mtrouter as a service. Valid only for Windows OS. Usage: \'mtrouter --install\'"}
        , {"-l, --load      : Command to initialize from specified file. Used to start application. Usage: \'mtrouter --load=<path-to-init-file>\'"}
        , {"-n, --instances : Command option to display list of connected instances. Used in console application. Usage: --instances"}
        , {"-p, --pause     : Command option to pause connection. Used in console application. Usage: --pause"}
        , {"-q, --quit      : Command option to stop router and quit application. Used in console application. Usage: --quit"}
        , {"-r, --restart   : Command option to restart connection. Used in console application. Usage: --restart"}
        , {"-s, --service   : Command to run mtrouter as a system service. Usage: \'mtrouter --service\'"}
        , {"-t, --silent    : Command option to stop displaying data rate. Used in console application. Usage: --silent"}
        , {"-u, --uninstall : Command to uninstall mtrouter as a service. Valid only for Windows OS. Usage: \'mtrouter --uninstall\'"}
        , {"-v, --verbose   : Command option to display data rate. Used in console application. Usage: --verbose"}
        , NESystemService::MSG_SEPARATOR
    };
}

//////////////////////////////////////////////////////////////////////////
// Scopes
//////////////////////////////////////////////////////////////////////////

DEF_LOG_SCOPE(mtrouter_app_MultitargetRouter_serviceMain);
DEF_LOG_SCOPE(mtrouter_app_MultitargetRouter_serviceStart);
DEF_LOG_SCOPE(mtrouter_app_MultitargetRouter_serviceStop);
DEF_LOG_SCOPE(mtrouter_app_MultitargetRouter_servicePause);
DEF_LOG_SCOPE(mtrouter_app_MultitargetRouter_serviceContinue);
DEF_LOG_SCOPE(mtrouter_app_MultitargetRouter_serviceInstall);
DEF_LOG_SCOPE(mtrouter_app_MultitargetRouter_serviceUninstall);
DEF_LOG_SCOPE(mtrouter_app_MultitargetRouter_setState);

//////////////////////////////////////////////////////////////////////////
// MultitargetRouter class implementation
//////////////////////////////////////////////////////////////////////////

const OptionParser::OptionSetup MultitargetRouter::ValidOptions[ ]
{
      { "-c", "--console"   , static_cast<int32_t>(RouterOption::CMD_RouterConsole)   , OptionParser::NO_DATA         , {}, {}, {} }
    , { "-h", "--help"      , static_cast<int32_t>(RouterOption::CMD_RouterPrintHelp) , OptionParser::NO_DATA         , {}, {}, {} }
    , { "-i", "--install"   , static_cast<int32_t>(RouterOption::CMD_RouterInstall)   , OptionParser::NO_DATA         , {}, {}, {} }
    , { "-l", "--load"      , static_cast<int32_t>(RouterOption::CMD_RouterLoad)      , OptionParser::STRING_NO_RANGE , {}, {}, {} }
    , { "-n", "--instances" , static_cast<int32_t>(RouterOption::CMD_RouterInstances) , OptionParser::NO_DATA         , {}, {}, {} }
    , { "-p", "--pause"     , static_cast<int32_t>(RouterOption::CMD_RouterPause)     , OptionParser::NO_DATA         , {}, {}, {} }
    , { "-q", "--quit"      , static_cast<int32_t>(RouterOption::CMD_RouterQuit)      , OptionParser::NO_DATA         , {}, {}, {} }
    , { "-r", "--restart"   , static_cast<int32_t>(RouterOption::CMD_RouterRestart)   , OptionParser::NO_DATA         , {}, {}, {} }
    , { "-s", "--service"   , static_cast<int32_t>(RouterOption::CMD_RouterService)   , OptionParser::FREESTYLE_DATA  , {}, {}, {} }
    , { "-t", "--silent"    , static_cast<int32_t>(RouterOption::CMD_RouterSilent)    , OptionParser::NO_DATA         , {}, {}, {} }
    , { "-u", "--uninstall" , static_cast<int32_t>(RouterOption::CMD_RouterUninstall) , OptionParser::NO_DATA         , {}, {}, {} }
    , { "-v", "--verbose"   , static_cast<int32_t>(RouterOption::CMD_RouterVerbose)   , OptionParser::NO_DATA         , {}, {}, {} }
};

MultitargetRouter & MultitargetRouter::instance()
{
    static MultitargetRouter _messageRouter;
    return _messageRouter;
}

#if AREG_EXTENDED
void MultitargetRouter::printStatus(const String& status)
{

    if (MultitargetRouter::instance().current_option() == NESystemService::ServiceOption::CMD_Console)
    {
        Console& console{ Console::instance() };
        Console::Coord curPos{ console.cursor_cur_position() };
        MultitargetRouter::_output_info(status);
        console.set_cursor_cur_position(curPos);
    }
}
#else   // AREG_EXTENDED
void MultitargetRouter::printStatus(const String& /* status */)
{
}
#endif  // AREG_EXTENDED

MultitargetRouter::MultitargetRouter()
    : ServiceApplicationBase( mServiceServer )
    , mServiceServer        ( )
{
}

Console::CallBack MultitargetRouter::option_check_callback() const
{
    return Console::CallBack( MultitargetRouter::_check_command );
}

void MultitargetRouter::run_console_input_extended()
{
#if AREG_EXTENDED

    Console & console = Console::instance( );
    MultitargetRouter::_output_title( );

    if (data_rate_helper().is_verbose())
    {
        // Disable to block user input until Console Service is up and running.
        console.enable_console_input( false );
        start_console_service( );
        // Blocked until user input
        console.wait_for_input( option_check_callback( ) );
        stop_console_service( );
    }
    else
    {
        // No verbose mode.
        // Set local callback, output message and wait for user input.
        console.enable_console_input( true );
        console.output_txt( NESystemService::COORD_USER_INPUT, NESystemService::FORMAT_WAIT_QUIT );
        console.wait_for_input( option_check_callback( ) );
    }

    console.move_cursor_one_line_down( );
    console.clear_screen( );
    console.uninitialize( );

#endif   // !AREG_EXTENDED
}

void MultitargetRouter::run_console_input_simple()
{
    constexpr uint32_t bufSize{ 512 };
    char cmd[bufSize]{ 0 };
    bool quit{ false };

    MultitargetRouter::_output_title( );

    do
    {
        printf( "%s", NESystemService::FORMAT_WAIT_QUIT.data( ) );
        if (input_console_data(cmd, bufSize) == false)
            continue;

        quit = MultitargetRouter::_check_command( cmd );

    } while ( quit == false );
}

std::pair<const OptionParser::OptionSetup*, int32_t> MultitargetRouter::app_options() const
{
    static  std::pair< const OptionParser::OptionSetup*, int32_t> _opts(std::pair< const OptionParser::OptionSetup*, int32_t>(MultitargetRouter::ValidOptions, static_cast<int32_t>(std::size(MultitargetRouter::ValidOptions))));
    return _opts;
}

wchar_t* MultitargetRouter::service_name_w() const
{
    return NEMultitargetRouterSettings::SERVICE_NAME_WIDE;
}

char* MultitargetRouter::service_name_a() const
{
    return NEMultitargetRouterSettings::SERVICE_NAME_ASCII;
}

wchar_t* MultitargetRouter::service_display_name_w() const
{
    return NEMultitargetRouterSettings::SERVICE_DISPLAY_NAME_WIDE;
}

char* MultitargetRouter::service_display_name_a() const
{
    return NEMultitargetRouterSettings::SERVICE_DISPLAY_NAME_ASCII;
}

wchar_t* MultitargetRouter::service_description_w() const
{
    return NEMultitargetRouterSettings::SERVICE_DESCRIBE_WIDE;
}

char* MultitargetRouter::service_description_a() const
{
    return NEMultitargetRouterSettings::SERVICE_DESCRIBE_ASCII;
}

NERemoteService::RemoteServiceKind MultitargetRouter::service_type() const
{
    return NERemoteService::RemoteServiceKind::Router;
}

NERemoteService::ConnectionType MultitargetRouter::connection_type() const
{
    return NERemoteService::ConnectionType::Tcpip;
}

void MultitargetRouter::print_help( bool /* isCmdLine */ )
{
#if     AREG_EXTENDED

    Console::Coord line{ NESystemService::COORD_INFO_MSG };
    Console& console = Console::instance();
    console.lock_console();
    for (const auto& text : _msgHelp)
    {
        console.output_txt(line, text);
        ++line.posY;
    }

    console.unlock_console();

#else   // AREG_EXTENDED

    for (const auto& line : _msgHelp)
    {
        std::cout << line << std::endl;
    }

    std::cout << std::ends;

#endif  // AREG_EXTENDED
}

void MultitargetRouter::start_console_service()
{
    Application::load_model( _modelName );
}

void MultitargetRouter::stop_console_service()
{
    Application::unload_model( _modelName );
}

bool MultitargetRouter::_check_command(const String& cmd)
{
    OptionParser parser( MultitargetRouter::ValidOptions, std::size( MultitargetRouter::ValidOptions) );
    bool quit{ false };
    bool hasError{ false };

    MultitargetRouter::_clean_help();

    if ( parser.parse_option_line( cmd ) )
    {
        MultitargetRouter & router = MultitargetRouter::instance( );
        const OptionParser::InputOptionList & opts = parser.options( );
        for (uint32_t i = 0; i < opts.size( ); ++ i )
        {
            const OptionParser::InputOption & opt = opts[ i ];
            switch ( static_cast<MultitargetRouter::RouterOption>(opt.inCommand) )
            {
            case MultitargetRouter::RouterOption::CMD_RouterPause:
                MultitargetRouter::_output_info( "Pausing message router ..." );
                router.communication_controller().disconnect_service_host( );
                router.mServiceServer.wait_to_complete( );
                MultitargetRouter::_output_info( "Message router is paused ..." );
                break;

            case MultitargetRouter::RouterOption::CMD_RouterRestart:
                MultitargetRouter::_output_info( "Restarting message router ..." );
                router.communication_controller( ).connect_service_host( );
                MultitargetRouter::_output_info( "Message router is restarted ..." );
                break;

            case MultitargetRouter::RouterOption::CMD_RouterInstances:
                MultitargetRouter::_output_instances( router.conneted_instances() );
                break;

            case MultitargetRouter::RouterOption::CMD_RouterVerbose:
                MultitargetRouter::_set_verbose_mode( true );
                break;

            case MultitargetRouter::RouterOption::CMD_RouterSilent:
                MultitargetRouter::_set_verbose_mode( false );
                break;

            case MultitargetRouter::RouterOption::CMD_RouterPrintHelp:
                router.print_help( false );
                break;

            case MultitargetRouter::RouterOption::CMD_RouterQuit:
                quit = true;
                break;

            case MultitargetRouter::RouterOption::CMD_RouterConsole:    // pass through
            case MultitargetRouter::RouterOption::CMD_RouterInstall:    // pass through
            case MultitargetRouter::RouterOption::CMD_RouterUninstall:  // pass through
            case MultitargetRouter::RouterOption::CMD_RouterService:    // pass through
            case MultitargetRouter::RouterOption::CMD_RouterLoad:
                MultitargetRouter::_output_info("This command should be used in command line ...");
                break;

            case MultitargetRouter::RouterOption::CMD_RouterUndefined:  // pass through
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
    
    Console & console = Console::instance( );
    console.lock_console();
    if ( quit == false )
    {
        if ( hasError )
        {
            console.output_msg( NESystemService::COORD_ERROR_MSG, NESystemService::FORMAT_MSG_ERROR.data( ), cmd.as_string( ) );
        }

        console.clear_line( NESystemService::COORD_USER_INPUT );
        console.output_txt( NESystemService::COORD_USER_INPUT, NESystemService::FORMAT_WAIT_QUIT );
    }
    else
    {
        console.output_txt( NESystemService::COORD_INFO_MSG, NESystemService::FORMAT_QUIT_APP );
    }

    console.refresh_screen( );
    console.unlock_console( );

#else   // !AREG_EXTENDED

    if ( quit == false )
    {
        if ( hasError )
        {
            printf( NESystemService::FORMAT_MSG_ERROR.data( ), cmd.as_string() );
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

void MultitargetRouter::_output_title()
{
#if AREG_EXTENDED

    Console & console = Console::instance( );
    console.lock_console();
    console.output_txt( NESystemService::COORD_TITLE, NEMultitargetRouterSettings::APP_TITLE.data( ) );
    console.output_txt( NESystemService::COORD_SUBTITLE, NESystemService::MSG_SEPARATOR.data( ) );
    console.unlock_console();

#else   // !AREG_EXTENDED

    printf( "%s\n", NEMultitargetRouterSettings::APP_TITLE.data( ) );
    printf( "%s\n", NESystemService::MSG_SEPARATOR.data( ) );

#endif  // AREG_EXTENDED
}

void MultitargetRouter::_output_info( const String & info )
{
#if AREG_EXTENDED

    Console & console = Console::instance( );
    Console::Coord coord{NESystemService::COORD_INFO_MSG};
    console.lock_console( );

    console.output_txt( coord, NESystemService::MSG_SEPARATOR.data( ) );
    ++ coord.posY;
    console.output_str( coord, info );

    console.unlock_console( );

#else   // !AREG_EXTENDED

    printf( "%s\n", info.as_string() );

#endif  // AREG_EXTENDED
}

void MultitargetRouter::_output_instances( const NEService::MapInstances & instances )
{
    static constexpr std::string_view _table{ "   Nr. |  Instance ID  |  Bitness  |  name " };
    static constexpr std::string_view _empty{ "There are no connected instances ..." };

#if AREG_EXTENDED

    Console & console = Console::instance( );
    Console::Coord coord{NESystemService::COORD_INFO_MSG};
    console.lock_console( );

    if ( instances.is_empty( ) )
    {
        console.output_txt( coord, NESystemService::MSG_SEPARATOR.data( ) );
        ++ coord.posY;
        console.output_str( coord, _empty );
        ++ coord.posY;
    }
    else
    {
        console.output_txt( coord, NESystemService::MSG_SEPARATOR.data( ) );
        ++ coord.posY;
        console.output_txt( coord, _table );
        ++ coord.posY;
        console.output_txt( coord, NESystemService::MSG_SEPARATOR.data( ) );
        ++ coord.posY;
        int32_t i{ 1 };
        for ( auto pos = instances.first_position( ); instances.is_valid_position( pos ); pos = instances.next_position( pos ) )
        {
            ITEM_ID cookie{ 0 };
            NEService::ConnectedInstance instance;
            instances.at_position( pos, cookie, instance);
            uint32_t id{ static_cast<uint32_t>(cookie) };

            console.output_msg(coord, " %4d. |  %11u  |    %u     |  %s ", i++, id, static_cast<uint32_t>(instance.ciBitness), instance.ciInstance.c_str());
            ++ coord.posY;
        }
    }

    console.output_txt( coord, NESystemService::MSG_SEPARATOR.data( ) );
    console.unlock_console( );

#else   // !AREG_EXTENDED

    if ( instances.is_empty( ) )
    {
        printf( "%s\n", _empty.data() );
    }
    else
    {
        printf( "%s\n", NESystemService::MSG_SEPARATOR.data( ) );
        printf( "%s\n", _table.data() );
        printf( "%s\n", NESystemService::MSG_SEPARATOR.data( ) );

        int32_t i{ 1 };
        for ( auto pos = instances.first_position( ); instances.is_valid_position( pos ); pos = instances.next_position( pos ) )
        {
            ITEM_ID cookie{ 0 };
            NEService::ConnectedInstance instance;
            instances.at_position( pos, cookie, instance);
            uint32_t id{ static_cast<uint32_t>(cookie) };

            printf(" %4d. |  %11u  |    %u     |  %s \n", i++, id, static_cast<uint32_t>(instance.ciBitness), instance.ciInstance.c_str());
        }
    }

    printf( "%s\n", NESystemService::MSG_SEPARATOR.data( ) );

#endif  // AREG_EXTENDED
}

#if AREG_EXTENDED
void MultitargetRouter::_set_verbose_mode( bool makeVerbose )
{
    static constexpr std::string_view _verbose{ "Switching to verbose mode to output data rate ..." };
    static constexpr std::string_view _silence{ "Switching to silent mode, no data rate output ..." };
    MultitargetRouter & router = MultitargetRouter::instance( );
    Console & console = Console::instance( );
    console.lock_console( );
    if ( router.data_rate_helper().is_verbose() != makeVerbose )
    {
        router.data_rate_helper().set_verbose(makeVerbose);

        if ( makeVerbose == false )
        {
            console.clear_line( NESystemService::COORD_SEND_RATE );
            console.clear_line( NESystemService::COORD_RECV_RATE );
            console.output_txt( NESystemService::COORD_INFO_MSG, _silence );
        }
        else
        {
            console.output_msg( NESystemService::COORD_SEND_RATE, NESystemService::FORMAT_SEND_DATA.data( ), 0.0, DataRateHelper::MSG_BYTES.data( ) );
            console.output_msg( NESystemService::COORD_RECV_RATE, NESystemService::FORMAT_RECV_DATA.data( ), 0.0, DataRateHelper::MSG_BYTES.data( ) );
            console.output_txt( NESystemService::COORD_INFO_MSG, _verbose);
        }

        console.refresh_screen( );
    }

    console.unlock_console( );

    static constexpr std::string_view _unsupported{"This option is available only with extended features"};
    printf( "%s\n", _unsupported.data( ) );
}

#else   // !AREG_EXTENDED

void MultitargetRouter::_set_verbose_mode( bool /* makeVerbose */ )
{
    static constexpr std::string_view _unsupported{"This option is available only with extended features"};
    printf( "%s\n", _unsupported.data( ) );
}

#endif  // AREG_EXTENDED

void MultitargetRouter::_clean_help()
{
#if     AREG_EXTENDED

    Console::Coord line{ NESystemService::COORD_INFO_MSG };
    Console& console = Console::instance();
    console.lock_console();

    console.clear_line(NESystemService::COORD_USER_INPUT);
    uint32_t count = std::size(_msgHelp);
    for (uint32_t i = 0; i < count; ++ i)
    {
        console.clear_line(line);
        ++line.posY;
    }

    console.unlock_console();

#endif  // AREG_EXTENDED
}
