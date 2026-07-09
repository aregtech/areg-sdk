/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
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
#include "areg/logging/areg_log.h"

#include "aregextend/console/Console.hpp"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <stdio.h>

//////////////////////////////////////////////////////////////////////////
// The model used only in console mode.
//////////////////////////////////////////////////////////////////////////

// This model defines a areg::ext::Console Service to run to make data rate outputs.
// The areg::ext::Console Service runs only in verbose mode.

static areg::String _modelName("mtrouterModel");

// Describe mode, set model name
BEGIN_MODEL(_modelName)

    // define console service thread.
    BEGIN_REGISTER_THREAD( "RouterConsoleServiceThread" )
        // Define the console service
        BEGIN_REGISTER_COMPONENT(RouterConsoleService::SERVICE_NAME, RouterConsoleService)
            // register dummy 'empty service'.
            REGISTER_IMPLEMENT_SERVICE( areg::EmptyServiceName, areg::EmptyServiceVersion )
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
        , areg::ext::MSG_SEPARATOR
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
        , {"-w, --threads   : Command option to display active per-client thread pair count. Usage: --threads"}
        , areg::ext::MSG_SEPARATOR
    };
}

DEF_LOG_SCOPE(mtrouter_app_MultitargetRouter, _check_command);
DEF_LOG_SCOPE(mtrouter_app_MultitargetRouter, stop_console_service);
DEF_LOG_SCOPE(mtrouter_app_MultitargetRouter, run_console_io);

//////////////////////////////////////////////////////////////////////////
// MultitargetRouter class implementation
//////////////////////////////////////////////////////////////////////////

const areg::ext::OptionParser::OptionSetup MultitargetRouter::ValidOptions[ ]
{
      { "-c", "--console"   , static_cast<int32_t>(RouterOption::CMD_RouterConsole)   , areg::ext::OptionParser::NO_DATA         , {}, {}, {} }
    , { "-h", "--help"      , static_cast<int32_t>(RouterOption::CMD_RouterPrintHelp) , areg::ext::OptionParser::NO_DATA         , {}, {}, {} }
    , { "-i", "--install"   , static_cast<int32_t>(RouterOption::CMD_RouterInstall)   , areg::ext::OptionParser::NO_DATA         , {}, {}, {} }
    , { "-l", "--load"      , static_cast<int32_t>(RouterOption::CMD_RouterLoad)      , areg::ext::OptionParser::STRING_NO_RANGE , {}, {}, {} }
    , { "-n", "--instances" , static_cast<int32_t>(RouterOption::CMD_RouterInstances) , areg::ext::OptionParser::NO_DATA         , {}, {}, {} }
    , { "-p", "--pause"     , static_cast<int32_t>(RouterOption::CMD_RouterPause)     , areg::ext::OptionParser::NO_DATA         , {}, {}, {} }
    , { "-q", "--quit"      , static_cast<int32_t>(RouterOption::CMD_RouterQuit)      , areg::ext::OptionParser::NO_DATA         , {}, {}, {} }
    , { "-r", "--restart"   , static_cast<int32_t>(RouterOption::CMD_RouterRestart)   , areg::ext::OptionParser::NO_DATA         , {}, {}, {} }
    , { "-s", "--service"   , static_cast<int32_t>(RouterOption::CMD_RouterService)   , areg::ext::OptionParser::FREESTYLE_DATA  , {}, {}, {} }
    , { "-t", "--silent"    , static_cast<int32_t>(RouterOption::CMD_RouterSilent)    , areg::ext::OptionParser::NO_DATA         , {}, {}, {} }
    , { "-u", "--uninstall" , static_cast<int32_t>(RouterOption::CMD_RouterUninstall) , areg::ext::OptionParser::NO_DATA         , {}, {}, {} }
    , { "-v", "--verbose"   , static_cast<int32_t>(RouterOption::CMD_RouterVerbose)   , areg::ext::OptionParser::NO_DATA         , {}, {}, {} }
    , { "-w", "--threads"   , static_cast<int32_t>(RouterOption::CMD_RouterThreads)   , areg::ext::OptionParser::NO_DATA         , {}, {}, {} }
};

MultitargetRouter & MultitargetRouter::instance()
{
    static MultitargetRouter _messageRouter;
    return _messageRouter;
}

#if AREG_EXTENDED
void MultitargetRouter::print_status(const areg::String& status)
{

    if (MultitargetRouter::instance().current_option() == areg::ext::ServiceOption::CMD_Console)
    {
        areg::ext::Console& console{ areg::ext::Console::instance() };
        areg::ext::Console::Coord curPos{ console.cursor_cur_position() };
        MultitargetRouter::_output_info(status);
        console.set_cursor_cur_position(curPos);
    }
}
#else   // AREG_EXTENDED
void MultitargetRouter::print_status(const areg::String& /* status */)
{
}
#endif  // AREG_EXTENDED

MultitargetRouter::MultitargetRouter()
    : ServiceApplicationBase( mServiceServer )
    , mServiceServer        ( )
{
}

areg::ext::Console::CallBack MultitargetRouter::option_check_callback() const
{
    return areg::ext::Console::CallBack( MultitargetRouter::_check_command );
}

void MultitargetRouter::run_console_io()
{
    LOG_SCOPE(mtrouter_app_MultitargetRouter, run_console_io);
    LOG_DBG("Running console ...");

    areg::ext::Console& console = areg::ext::Console::instance();
    MultitargetRouter::_output_title();

    // Show all four stat rows at zero before the rate thread starts.
    console.enable_console_input(true);
    console.output_msg(areg::ext::COORD_SEND_RATE, areg::ext::FORMAT_SEND_DATA.data(), 0.0f, areg::ext::DataRateHelper::MSG_BYTES.data());
    console.output_msg(areg::ext::COORD_RECV_RATE, areg::ext::FORMAT_RECV_DATA.data(), 0.0f, areg::ext::DataRateHelper::MSG_BYTES.data());
    console.output_msg(areg::ext::COORD_SEND_MSGS, areg::ext::FORMAT_SEND_MSGS.data(), 0u);
    console.output_msg(areg::ext::COORD_RECV_MSGS, areg::ext::FORMAT_RECV_MSGS.data(), 0u);
    console.output_txt(areg::ext::COORD_USER_INPUT, areg::ext::FORMAT_WAIT_QUIT);
    // Place the cursor immediately after the prompt text so fgets echoes there.
    console.set_cursor_cur_position({ areg::ext::COORD_USER_INPUT.posX + static_cast<int32_t>(areg::ext::FORMAT_WAIT_QUIT.size()),
                                      areg::ext::COORD_USER_INPUT.posY });
    console.refresh_screen();

    // update all four stat rows every second
    std::mutex rate_mtx;
    std::condition_variable rate_cv;
    bool rate_running{ true };                  // guarded by rate_mtx
    std::thread rate_thread([&]()
        {
            areg::ext::DataRateHelper& helper = data_rate_helper();
            helper.set_verbose(true);
            uint64_t sizeSent{ 0u }, sizeRecv{ 0u };
            uint32_t msgSent{ 0u }, msgRecv{ 0u };

            std::unique_lock<std::mutex> lock(rate_mtx);
            while (rate_running)
            {
                if (rate_cv.wait_for(lock, std::chrono::seconds(1), [&]() { return !rate_running; }))
                    break;

                lock.unlock();
                helper.query_data_sent(sizeSent, msgSent);
                helper.query_data_received(sizeRecv, msgRecv);

                areg::ext::DataRateHelper::DataRate sent = areg::ext::DataRateHelper::convert_data_rate_literals(sizeSent);
                areg::ext::DataRateHelper::DataRate recv = areg::ext::DataRateHelper::convert_data_rate_literals(sizeRecv);

                console.output_msg(areg::ext::COORD_SEND_RATE, areg::ext::FORMAT_SEND_DATA.data(), static_cast<double>(sent.first), sent.second.c_str());
                console.output_msg(areg::ext::COORD_RECV_RATE, areg::ext::FORMAT_RECV_DATA.data(), static_cast<double>(recv.first), recv.second.c_str());
                console.output_msg(areg::ext::COORD_SEND_MSGS, areg::ext::FORMAT_SEND_MSGS.data(), msgSent);
                console.output_msg(areg::ext::COORD_RECV_MSGS, areg::ext::FORMAT_RECV_MSGS.data(), msgRecv);
                console.refresh_screen();
                lock.lock();
            }
        });

    // Block until the user types '-q' / '--quit'.
    console.wait_for_input(option_check_callback());

    {
        std::lock_guard<std::mutex> lock(rate_mtx);
        rate_running = false;
    }
    rate_cv.notify_one();
    rate_thread.join();

    console.uninitialize();
    LOG_DBG("Exit console with simple (ANSI) features...");
}

std::pair<const areg::ext::OptionParser::OptionSetup*, int32_t> MultitargetRouter::app_options() const
{
    static  std::pair< const areg::ext::OptionParser::OptionSetup*, int32_t> _opts(std::pair< const areg::ext::OptionParser::OptionSetup*, int32_t>(MultitargetRouter::ValidOptions, static_cast<int32_t>(std::size(MultitargetRouter::ValidOptions))));
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

areg::RemoteServiceKind MultitargetRouter::service_type() const
{
    return areg::RemoteServiceKind::Router;
}

areg::ConnectionType MultitargetRouter::connection_type() const
{
    return areg::ConnectionType::Tcpip;
}

void MultitargetRouter::print_help( bool /* isCmdLine */ )
{
#if     AREG_EXTENDED

    areg::ext::Console::Coord line{ areg::ext::COORD_INFO_MSG };
    areg::ext::Console& console = areg::ext::Console::instance();
    console.lock_console();
    for (const auto& text : _msgHelp)
    {
        console.output_txt(line, text);
        ++line.posY;
    }

    console.unlock_console();

#else   // AREG_EXTENDED

    {
        areg::ext::Console::Coord line{ areg::ext::COORD_INFO_MSG };
        areg::ext::Console& console = areg::ext::Console::instance();
        for (const auto& text : _msgHelp)
        {
            console.output_txt(line, text);
            ++line.posY;
        }

        console.refresh_screen();
    }

#endif  // AREG_EXTENDED
}

void MultitargetRouter::start_console_service()
{
    // areg::Application::load_model( _modelName );
}

void MultitargetRouter::stop_console_service()
{
    LOG_SCOPE(mtrouter_app_MultitargetRouter, stop_console_service);
    LOG_DBG("Stopping console service, unloading model [ %s ]", _modelName.as_string());
    // areg::Application::unload_model( _modelName );
    LOG_DBG("Console service is unloaded...");
}

bool MultitargetRouter::_check_command(const areg::String& cmd)
{
    LOG_SCOPE(mtrouter_app_MultitargetRouter, _check_command);
    LOG_DBG("Checking command [ %s ]", cmd.as_string());

    areg::ext::OptionParser parser( MultitargetRouter::ValidOptions, std::size( MultitargetRouter::ValidOptions) );
    bool quit{ false };
    bool hasError{ false };

    MultitargetRouter::_clean_help();

    if ( parser.parse_option_line( cmd ) )
    {
        MultitargetRouter & router = MultitargetRouter::instance( );
        const areg::ext::OptionParser::InputOptionList & opts = parser.options( );
        for (uint32_t i = 0; i < opts.size( ); ++ i )
        {
            const areg::ext::OptionParser::InputOption & opt = opts[ i ];
            MultitargetRouter::RouterOption optRoute = static_cast<MultitargetRouter::RouterOption>(opt.inCommand);
            LOG_DBG("Found option [ %s ], going to process...", MultitargetRouter::as_string(optRoute));
            switch ( optRoute )
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

            case MultitargetRouter::RouterOption::CMD_RouterThreads:
                {
                    areg::String msg;
                    msg.format("Active per-client thread pairs: %u", router.communication_controller().active_client_pair_count());
                    MultitargetRouter::_output_info(msg);
                }
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
    
    areg::ext::Console & console = areg::ext::Console::instance( );
    console.lock_console();
    if ( quit == false )
    {
        if ( hasError )
        {
            console.output_msg( areg::ext::COORD_ERROR_MSG, areg::ext::FORMAT_MSG_ERROR.data( ), cmd.as_string( ) );
        }

        console.clear_line( areg::ext::COORD_USER_INPUT );
        console.output_txt( areg::ext::COORD_USER_INPUT, areg::ext::FORMAT_WAIT_QUIT );
        console.set_cursor_cur_position({ areg::ext::COORD_USER_INPUT.posX + static_cast<int32_t>(areg::ext::FORMAT_WAIT_QUIT.size()),
                                          areg::ext::COORD_USER_INPUT.posY });
    }
    else
    {
        console.output_txt( areg::ext::COORD_INFO_MSG, areg::ext::FORMAT_QUIT_APP );
    }

    console.refresh_screen( );
    console.unlock_console( );

#else   // !AREG_EXTENDED

    {
        areg::ext::Console& console = areg::ext::Console::instance();
        if (quit == false)
        {
            if (hasError)
            {
                console.output_msg(areg::ext::COORD_ERROR_MSG, areg::ext::FORMAT_MSG_ERROR.data(), cmd.as_string());
            }
            else
            {
                console.clear_line(areg::ext::COORD_ERROR_MSG);
            }

            console.clear_line(areg::ext::COORD_USER_INPUT);
            console.output_txt(areg::ext::COORD_USER_INPUT, areg::ext::FORMAT_WAIT_QUIT);
            console.set_cursor_cur_position({ areg::ext::COORD_USER_INPUT.posX + static_cast<int32_t>(areg::ext::FORMAT_WAIT_QUIT.size()),
                                              areg::ext::COORD_USER_INPUT.posY });
        }
        else
        {
            console.output_txt(areg::ext::COORD_INFO_MSG, areg::ext::FORMAT_QUIT_APP);
        }

        console.refresh_screen();
    }

#endif  // AREG_EXTENDED

    return quit;
}

void MultitargetRouter::_output_title()
{
#if AREG_EXTENDED

    areg::ext::Console & console = areg::ext::Console::instance( );
    console.lock_console();
    console.output_txt( areg::ext::COORD_TITLE, NEMultitargetRouterSettings::APP_TITLE.data( ) );
    console.output_txt( areg::ext::COORD_SUBTITLE, areg::ext::MSG_SEPARATOR.data( ) );
    console.unlock_console();

#else   // !AREG_EXTENDED

    areg::ext::Console& console = areg::ext::Console::instance();
    console.output_txt(areg::ext::COORD_TITLE,    NEMultitargetRouterSettings::APP_TITLE.data());
    console.output_txt(areg::ext::COORD_SUBTITLE, areg::ext::MSG_SEPARATOR.data());
    console.refresh_screen();

#endif  // AREG_EXTENDED
}

void MultitargetRouter::_output_info( const areg::String & info )
{
#if AREG_EXTENDED

    areg::ext::Console & console = areg::ext::Console::instance( );
    areg::ext::Console::Coord coord{areg::ext::COORD_INFO_MSG};
    console.lock_console( );

    console.output_txt( coord, areg::ext::MSG_SEPARATOR.data( ) );
    ++ coord.posY;
    console.output_str( coord, info );

    console.unlock_console( );

#else   // !AREG_EXTENDED

    {
        areg::ext::Console& console = areg::ext::Console::instance();
        areg::ext::Console::Coord coord{ areg::ext::COORD_INFO_MSG };
        console.output_txt(coord, areg::ext::MSG_SEPARATOR.data());
        ++coord.posY;
        console.output_str(coord, info);
        console.refresh_screen();
    }

#endif  // AREG_EXTENDED
}

void MultitargetRouter::_output_instances( const areg::MapInstances & instances )
{
    static constexpr std::string_view _table{ "   Nr. |  Instance ID  |  Bitness  |  name " };
    static constexpr std::string_view _empty{ "There are no connected instances ..." };

#if AREG_EXTENDED

    areg::ext::Console & console = areg::ext::Console::instance( );
    areg::ext::Console::Coord coord{areg::ext::COORD_INFO_MSG};
    console.lock_console( );

    if ( instances.is_empty( ) )
    {
        console.output_txt( coord, areg::ext::MSG_SEPARATOR.data( ) );
        ++ coord.posY;
        console.output_str( coord, _empty );
        ++ coord.posY;
    }
    else
    {
        console.output_txt( coord, areg::ext::MSG_SEPARATOR.data( ) );
        ++ coord.posY;
        console.output_txt( coord, _table );
        ++ coord.posY;
        console.output_txt( coord, areg::ext::MSG_SEPARATOR.data( ) );
        ++ coord.posY;
        int32_t i{ 1 };
        for ( auto pos = instances.first_position( ); instances.is_valid_position( pos ); pos = instances.next_position( pos ) )
        {
            ITEM_ID cookie{ 0 };
            areg::ConnectedInstance instance;
            instances.at_position( pos, cookie, instance);
            uint32_t id{ static_cast<uint32_t>(cookie) };

            console.output_msg(coord, " %4d. |  %11u  |    %u     |  %s ", i++, id, static_cast<uint32_t>(instance.ciBitness), instance.ciInstance.c_str());
            ++ coord.posY;
        }
    }

    console.output_txt( coord, areg::ext::MSG_SEPARATOR.data( ) );
    console.unlock_console( );

#else   // !AREG_EXTENDED

    {
        areg::ext::Console& console = areg::ext::Console::instance();
        areg::ext::Console::Coord coord{ areg::ext::COORD_INFO_MSG };
        if (instances.is_empty())
        {
            console.output_txt(coord, areg::ext::MSG_SEPARATOR.data());
            ++coord.posY;
            console.output_txt(coord, _empty);
        }
        else
        {
            console.output_txt(coord, areg::ext::MSG_SEPARATOR.data());
            ++coord.posY;
            console.output_txt(coord, _table);
            ++coord.posY;
            console.output_txt(coord, areg::ext::MSG_SEPARATOR.data());
            ++coord.posY;
            int32_t i{ 1 };
            for (auto pos = instances.first_position(); instances.is_valid_position(pos); pos = instances.next_position(pos))
            {
                ITEM_ID cookie{ 0 };
                areg::ConnectedInstance instance;
                instances.at_position(pos, cookie, instance);
                uint32_t id{ static_cast<uint32_t>(cookie) };
                console.output_msg(coord, " %4d. |  %11u  |    %u     |  %s ",
                                   i++, id, static_cast<uint32_t>(instance.ciBitness), instance.ciInstance.c_str());
                ++coord.posY;
            }
        }

        console.output_txt(coord, areg::ext::MSG_SEPARATOR.data());
        console.refresh_screen();
    }

#endif  // AREG_EXTENDED
}

#if AREG_EXTENDED
void MultitargetRouter::_set_verbose_mode( bool makeVerbose )
{
    static constexpr std::string_view _verbose{ "Switching to verbose mode to output data rate ..." };
    static constexpr std::string_view _silence{ "Switching to silent mode, no data rate output ..." };
    MultitargetRouter & router = MultitargetRouter::instance( );
    areg::ext::Console & console = areg::ext::Console::instance( );
    console.lock_console( );
    if ( router.data_rate_helper().is_verbose() != makeVerbose )
    {
        router.data_rate_helper().set_verbose(makeVerbose);

        if ( makeVerbose == false )
        {
            console.clear_line( areg::ext::COORD_SEND_RATE );
            console.clear_line( areg::ext::COORD_RECV_RATE );
            console.clear_line( areg::ext::COORD_SEND_MSGS );
            console.clear_line( areg::ext::COORD_RECV_MSGS );
            console.output_txt( areg::ext::COORD_INFO_MSG, _silence );
        }
        else
        {
            console.output_msg( areg::ext::COORD_SEND_RATE, areg::ext::FORMAT_SEND_DATA.data( ), 0.0, areg::ext::DataRateHelper::MSG_BYTES.data( ) );
            console.output_msg( areg::ext::COORD_RECV_RATE, areg::ext::FORMAT_RECV_DATA.data( ), 0.0, areg::ext::DataRateHelper::MSG_BYTES.data( ) );
            console.output_msg( areg::ext::COORD_SEND_MSGS, areg::ext::FORMAT_SEND_MSGS.data( ), 0u );
            console.output_msg( areg::ext::COORD_RECV_MSGS, areg::ext::FORMAT_RECV_MSGS.data( ), 0u );
            console.output_txt( areg::ext::COORD_INFO_MSG, _verbose);
        }

        console.refresh_screen( );
    }

    console.unlock_console( );
}

#else   // !AREG_EXTENDED

void MultitargetRouter::_set_verbose_mode( bool /* makeVerbose */ )
{
    // In simple (ANSI) mode the rate thread always runs verbose; this command is a no-op.
    static constexpr std::string_view _always_on{ "Data rate display is always active in this mode." };
    areg::ext::Console& console = areg::ext::Console::instance();
    console.output_txt(areg::ext::COORD_INFO_MSG, _always_on);
    console.refresh_screen();
}

#endif  // AREG_EXTENDED

void MultitargetRouter::_clean_help()
{
#if     AREG_EXTENDED

    areg::ext::Console::Coord line{ areg::ext::COORD_INFO_MSG };
    areg::ext::Console& console = areg::ext::Console::instance();
    console.lock_console();

    console.clear_line(areg::ext::COORD_USER_INPUT);
    uint32_t count = std::size(_msgHelp);
    for (uint32_t i = 0; i < count; ++ i)
    {
        console.clear_line(line);
        ++line.posY;
    }

    console.unlock_console();

#endif  // AREG_EXTENDED
}
