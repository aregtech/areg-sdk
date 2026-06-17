/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        logcollector/app/private/LogCollector.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, log collector to run as a console application process or service.
 ************************************************************************/

#include "logcollector/app/LogCollector.hpp"
#include "logcollector/app/private/LogCollectorConsoleService.hpp"

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

using areg::ArrayList;
using areg::ConfigManager;
using areg::Property;
using areg::PropertyKey;
using areg::String;
using areg::ext::Console;
using areg::ext::DataRateHelper;
using areg::ext::OptionParser;

//////////////////////////////////////////////////////////////////////////
// The model used only in console mode.
//////////////////////////////////////////////////////////////////////////

// This model defines a Console Service to run to make data rate outputs.
// The Console Service runs only in verbose mode.

static String _modelName("LogCollectorModel");
// Describe mode, set model name
BEGIN_MODEL(_modelName)

    // define console service thread.
    BEGIN_REGISTER_THREAD( "LogCollectorConsoleServiceThread" )
        // Define the console service
        BEGIN_REGISTER_COMPONENT(LogCollectorConsoleService::SERVICE_NAME, LogCollectorConsoleService)
            // register dummy 'empty service'.
            REGISTER_IMPLEMENT_SERVICE( areg::EmptyServiceName, areg::EmptyServiceVersion )
        // end of component description
        END_REGISTER_COMPONENT(LogCollectorConsoleService::SERVICE_NAME )
    // end of thread description
    END_REGISTER_THREAD( "LogCollectorConsoleServiceThread" )

// end of model description
END_MODEL(_modelName)
        
namespace
{
    constexpr std::string_view _msgHelp []
    {
          {"Usage of Areg Log collector (logcollector) :"}
        , areg::ext::MSG_SEPARATOR
        , {"-a, --save      : Command to save logs in the file. Used in console application. Usage: --save"}
        , {"-b, --unsave    : Command to stop saving logs in the file. Used in console application. Usage: --unsave"}
        , {"-c, --console   : Command to run Log Collector as a console application (default option). Usage: \'logcollector --console\'"}
        , {"-e, --query     : Command to query the list of logging scopes. Used in console application. Usage (\'*\' can be a cookie number): --query *"}
        , {"-f, --config    : Command to save current configuration, including log scopes in the config file. Used in console application. Usage: --config"}
        , {"-h, --help      : Command to display this message on console."}
        , {"-i, --install   : Command to install Log Collector as a service. Valid only for Windows OS. Usage: \'logcollector --install\'"}
        , {"-l, --load      : Command to initialize from specified file. Used to start application. Usage: \'logcollector --load=<path-to-init-file>\'"}
        , {"-n, --instances : Command option to display list of connected instances. Used in console application. Usage: --instances"}
        , {"-o, --scope     : Command to update log scope priority. Used in console application. Usage (\'*\' can be a cookie number): --scope *::areg_base_NESocket=NOTSET"}
        , {"-p, --pause     : Command option to pause connection. Used in console application. Usage: --pause"}
        , {"-q, --quit      : Command option to stop Log Collector and quit application. Used in console application. Usage: --quit"}
        , {"-r, --restart   : Command option to restart connection. Used in console application. Usage: --restart"}
        , {"-s, --service   : Command to run logcollector as a system service. Usage: \'logcollector --service\'"}
        , {"-t, --silent    : Command option to stop displaying data rate. Used in console application. Usage: --silent"}
        , {"-u, --uninstall : Command to uninstall logcollector as a service. Valid only for Windows OS. Usage: \'logcollector --uninstall\'"}
        , {"-v, --verbose   : Command option to display data rate. Used in console application. Usage: --verbose"}
        , areg::ext::MSG_SEPARATOR
    };
}

//////////////////////////////////////////////////////////////////////////
// LogCollector class implementation
//////////////////////////////////////////////////////////////////////////

const OptionParser::OptionSetup LogCollector::ValidOptions[ ]
{
      { "-a", "--save"      , static_cast<int32_t>(LoggerOption::CMD_LogSaveLogs)     , OptionParser::STRING_NO_RANGE , {}, {}, {} }
    , { "-b", "--unsave"    , static_cast<int32_t>(LoggerOption::CMD_LogSaveLogsStop) , OptionParser::NO_DATA         , {}, {}, {} }
    , { "-c", "--console"   , static_cast<int32_t>(LoggerOption::CMD_LogConsole)      , OptionParser::NO_DATA         , {}, {}, {} }
    , { "-e", "--query"     , static_cast<int32_t>(LoggerOption::CMD_LogQueryScopes)  , OptionParser::STRING_NO_RANGE , {}, {}, {} }
    , { "-f", "--config"    , static_cast<int32_t>(LoggerOption::CMD_LogSaveConfig)   , OptionParser::STRING_NO_RANGE , {}, {}, {} }
    , { "-h", "--help"      , static_cast<int32_t>(LoggerOption::CMD_LogPrintHelp)    , OptionParser::NO_DATA         , {}, {}, {} }
    , { "-i", "--install"   , static_cast<int32_t>(LoggerOption::CMD_LogInstall)      , OptionParser::NO_DATA         , {}, {}, {} }
    , { "-l", "--load"      , static_cast<int32_t>(LoggerOption::CMD_LogLoad)         , OptionParser::STRING_NO_RANGE , {}, {}, {} }
    , { "-n", "--instances" , static_cast<int32_t>(LoggerOption::CMD_LogInstances)    , OptionParser::NO_DATA         , {}, {}, {} }
    , { "-o", "--scope"     , static_cast<int32_t>(LoggerOption::CMD_LogUpdateScope)  , OptionParser::STRING_NO_RANGE , {}, {}, {} }
    , { "-p", "--pause"     , static_cast<int32_t>(LoggerOption::CMD_LogPause)        , OptionParser::NO_DATA         , {}, {}, {} }
    , { "-q", "--quit"      , static_cast<int32_t>(LoggerOption::CMD_LogQuit)         , OptionParser::NO_DATA         , {}, {}, {} }
    , { "-r", "--restart"   , static_cast<int32_t>(LoggerOption::CMD_LogRestart)      , OptionParser::NO_DATA         , {}, {}, {} }
    , { "-s", "--service"   , static_cast<int32_t>(LoggerOption::CMD_LogService)      , OptionParser::FREESTYLE_DATA  , {}, {}, {} }
    , { "-t", "--silent"    , static_cast<int32_t>(LoggerOption::CMD_LogSilent)       , OptionParser::NO_DATA         , {}, {}, {} }
    , { "-u", "--uninstall" , static_cast<int32_t>(LoggerOption::CMD_LogUninstall)    , OptionParser::NO_DATA         , {}, {}, {} }
    , { "-v", "--verbose"   , static_cast<int32_t>(LoggerOption::CMD_LogVerbose)      , OptionParser::NO_DATA         , {}, {}, {} }
};

LogCollector & LogCollector::instance()
{
    static LogCollector _logger;
    return _logger;
}

#if AREG_EXTENDED
void LogCollector::print_status(const String& status)
{

    if (LogCollector::instance().current_option() == areg::ext::ServiceOption::CMD_Console)
    {
        Console& console{ Console::instance() };
        Console::Coord curPos{ console.cursor_cur_position() };
        LogCollector::_output_info(status);
        console.set_cursor_cur_position(curPos);
    }

}
#else   // AREG_EXTENDED
void LogCollector::print_status(const String& /* status */)
{
}
#endif  // AREG_EXTENDED

LogCollector::LogCollector()
    : ServiceApplicationBase( mServiceServer )
    , mServiceServer        ( )
{
}

Console::CallBack LogCollector::option_check_callback() const
{
    return Console::CallBack( LogCollector::_check_command );
}

void LogCollector::run_console_io()
{
    Console& console = Console::instance();
    LogCollector::_output_title();

    // Show all four stat rows at zero before the rate thread starts.
    console.enable_console_input(true);
    console.output_msg(areg::ext::COORD_SEND_RATE, areg::ext::FORMAT_SEND_DATA.data(), 0.0f, DataRateHelper::MSG_BYTES.data());
    console.output_msg(areg::ext::COORD_RECV_RATE, areg::ext::FORMAT_RECV_DATA.data(), 0.0f, DataRateHelper::MSG_BYTES.data());
    console.output_msg(areg::ext::COORD_SEND_MSGS, areg::ext::FORMAT_SEND_MSGS.data(), 0u);
    console.output_msg(areg::ext::COORD_RECV_MSGS, areg::ext::FORMAT_RECV_MSGS.data(), 0u);
    console.output_txt(areg::ext::COORD_USER_INPUT, areg::ext::FORMAT_WAIT_QUIT);
    // Place the cursor immediately after the prompt text so fgets echoes there.
    console.set_cursor_cur_position({ areg::ext::COORD_USER_INPUT.posX + static_cast<int32_t>(areg::ext::FORMAT_WAIT_QUIT.size()),
                                      areg::ext::COORD_USER_INPUT.posY });
    console.refresh_screen();

    // refresh all four stat rows every second.
    std::mutex rate_mtx;
    std::condition_variable rate_cv;
    bool rate_running{ true };                  // guarded by rate_mtx
    std::thread rate_thread([&]()
    {
        DataRateHelper& helper = data_rate_helper();
        helper.set_verbose(true);
        uint64_t sizeSent{ 0u }, sizeRecv{ 0u };
        uint32_t msgSent{ 0u }, msgRecv{ 0u };

        std::unique_lock<std::mutex> lock(rate_mtx);
        while (rate_running)
        {
            // Wake after 1s for the periodic refresh, or immediately when rate_running is cleared.
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
}

void LogCollector::run_service()
{
    areg::Application::wait_quit(areg::WAIT_INFINITE);
}

std::pair<const OptionParser::OptionSetup*, int32_t> LogCollector::app_options() const
{
    static  std::pair< const OptionParser::OptionSetup*, int32_t> _opts( std::pair< const OptionParser::OptionSetup*
                                                                    , int32_t>(LogCollector::ValidOptions
                                                                    , static_cast<int32_t>(std::size(LogCollector::ValidOptions))));
    return _opts;
}

wchar_t* LogCollector::service_name_w() const
{
    return logcollector::SERVICE_NAME_WIDE;
}

char* LogCollector::service_name_a() const
{
    return logcollector::SERVICE_NAME_ASCII;
}

wchar_t* LogCollector::service_display_name_w() const
{
    return logcollector::SERVICE_DISPLAY_NAME_WIDE;
}

char* LogCollector::service_display_name_a() const
{
    return logcollector::SERVICE_DISPLAY_NAME_ASCII;
}

wchar_t* LogCollector::service_description_w() const
{
    return logcollector::SERVICE_DESCRIBE_WIDE;
}

char* LogCollector::service_description_a() const
{
    return logcollector::SERVICE_DESCRIBE_ASCII;
}

areg::RemoteServiceKind LogCollector::service_type() const
{
    return areg::RemoteServiceKind::Logger;
}

areg::ConnectionType LogCollector::connection_type() const
{
    return areg::ConnectionType::Tcpip;
}

void LogCollector::post_read_configuration(ConfigManager& config)
{
    _enable_local_logs(config, false);
}

void LogCollector::on_setup_configuration(const areg::ListProperties& /* listReadonly */, const areg::ListProperties& /* listWritable */, ConfigManager& config)
{
    _enable_local_logs(config, false);
}

void LogCollector::print_help( bool /* isCmdLine */ )
{
#if     AREG_EXTENDED

    Console::Coord line{ areg::ext::COORD_INFO_MSG };
    Console& console = Console::instance();
    console.lock_console();
    for (const auto& text : _msgHelp)
    {
        console.output_txt(line, text);
        ++line.posY;
    }

    console.unlock_console();

#else   // AREG_EXTENDED

    {
        Console::Coord line{ areg::ext::COORD_INFO_MSG };
        Console& console = Console::instance();
        for (const auto& text : _msgHelp)
        {
            console.output_txt(line, text);
            ++line.posY;
        }

        console.refresh_screen();
    }

#endif  // AREG_EXTENDED
}

void LogCollector::start_console_service()
{
    areg::Application::load_model( _modelName );
}

void LogCollector::stop_console_service()
{
    areg::Application::unload_model( _modelName );
}

bool LogCollector::_check_command(const String& cmd)
{
    OptionParser parser( LogCollector::ValidOptions, std::size( LogCollector::ValidOptions ) );
    bool quit{ false };
    bool hasError {false};

    LogCollector::_clean_help();

    if ( parser.parse_option_line( cmd ) )
    {
        LogCollector & logger = LogCollector::instance( );
        const OptionParser::InputOptionList & opts = parser.options( );
        for ( uint32_t i = 0; i < opts.size( ); ++ i )
        {
            const OptionParser::InputOption & opt = opts[ i ];
            switch ( static_cast<LoggerOption>(opt.inCommand) )
            {
            case LoggerOption::CMD_LogPause:
                LogCollector::_output_info( "Pausing log collector ..." );
                logger.communication_controller().disconnect_service_host( );
                logger.mServiceServer.wait_to_complete( );
                LogCollector::_output_info( "Log collector is paused ..." );
                break;

            case LoggerOption::CMD_LogRestart:
                LogCollector::_output_info( "Restarting log collector ..." );
                logger.communication_controller( ).connect_service_host( );
                LogCollector::_output_info( "Log collector is restarted ..." );
                break;

            case LoggerOption::CMD_LogInstances:
                LogCollector::_output_instances( logger.connected_instances() );
                break;

            case LoggerOption::CMD_LogVerbose:
                LogCollector::_set_verbose_mode( true );
                break;

            case LoggerOption::CMD_LogSilent:
                LogCollector::_set_verbose_mode( false );
                break;

            case LoggerOption::CMD_LogPrintHelp:
                logger.print_help( false );
                break;

            case LoggerOption::CMD_LogQuit:
                quit = true;
                break;

            case LoggerOption::CMD_LogConsole:        // fall through
            case LoggerOption::CMD_LogInstall:        // fall through
            case LoggerOption::CMD_LogUninstall:      // fall through
            case LoggerOption::CMD_LogService:        // fall through
            case LoggerOption::CMD_LogLoad:
                LogCollector::_output_info("This command should be used in command line ...");
                break;

            case LoggerOption::CMD_LogUpdateScope:
                LogCollector::_process_update_scopes(opt);
                break;

            case LoggerOption::CMD_LogQueryScopes:
                LogCollector::_process_query_scopes(opt);
                break;

            case LoggerOption::CMD_LogSaveLogs:       // fall through
            case LoggerOption::CMD_LogSaveConfig:     // fall through
            case LoggerOption::CMD_LogSaveLogsStop:
                LogCollector::_output_info("The feature is not implemented yet!!!");
                break;

            case LoggerOption::CMD_LogUndefined:
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
        Console& console = Console::instance();
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

void LogCollector::_output_title()
{
#if AREG_EXTENDED

    Console & console = Console::instance( );
    console.lock_console();
    console.output_txt( areg::ext::COORD_TITLE, logcollector::APP_TITLE.data( ) );
    console.output_txt( areg::ext::COORD_SUBTITLE, areg::ext::MSG_SEPARATOR.data( ) );
    console.unlock_console();

#else   // !AREG_EXTENDED

    {
        Console& console = Console::instance();
        console.output_txt(areg::ext::COORD_TITLE,    logcollector::APP_TITLE.data());
        console.output_txt(areg::ext::COORD_SUBTITLE, areg::ext::MSG_SEPARATOR.data());
        console.refresh_screen();
    }

#endif  // AREG_EXTENDED
}

void LogCollector::_output_info( const String & info )
{
#if AREG_EXTENDED

    Console & console = Console::instance( );
    Console::Coord coord{areg::ext::COORD_INFO_MSG};
    console.lock_console( );

    console.output_txt( coord, areg::ext::MSG_SEPARATOR.data( ) );
    ++ coord.posY;
    console.output_str( coord, info );

    console.unlock_console( );

#else   // !AREG_EXTENDED

    {
        Console& console = Console::instance();
        Console::Coord coord{ areg::ext::COORD_INFO_MSG };
        console.output_txt(coord, areg::ext::MSG_SEPARATOR.data());
        ++coord.posY;
        console.output_str(coord, info);
        console.refresh_screen();
    }

#endif  // AREG_EXTENDED
}

void LogCollector::_output_instances( const areg::MapInstances & instances )
{
    static constexpr std::string_view _table{ "   Nr. |  Instance ID  |  Bitness  |  name " };
    static constexpr std::string_view _empty{ "There are no connected instances ..." };

#if AREG_EXTENDED

    Console & console = Console::instance( );
    Console::Coord coord{areg::ext::COORD_INFO_MSG};
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
        Console& console = Console::instance();
        Console::Coord coord{ areg::ext::COORD_INFO_MSG };
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
void LogCollector::_set_verbose_mode( bool makeVerbose )
{

    static constexpr std::string_view _verbose{ "Switching to verbose mode to output data rate ..." };
    static constexpr std::string_view _silence{ "Switching to silent mode, no data rate output ..." };
    LogCollector & logger = LogCollector::instance( );
    Console & console = Console::instance( );
    console.lock_console( );
    if ( logger.data_rate_helper().is_verbose() != makeVerbose )
    {
        logger.data_rate_helper().set_verbose(makeVerbose);

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
            console.output_msg( areg::ext::COORD_SEND_RATE, areg::ext::FORMAT_SEND_DATA.data( ), 0.0, DataRateHelper::MSG_BYTES.data( ) );
            console.output_msg( areg::ext::COORD_RECV_RATE, areg::ext::FORMAT_RECV_DATA.data( ), 0.0, DataRateHelper::MSG_BYTES.data( ) );
            console.output_msg( areg::ext::COORD_SEND_MSGS, areg::ext::FORMAT_SEND_MSGS.data( ), 0u );
            console.output_msg( areg::ext::COORD_RECV_MSGS, areg::ext::FORMAT_RECV_MSGS.data( ), 0u );
            console.output_txt( areg::ext::COORD_INFO_MSG, _verbose);
        }

        console.refresh_screen( );
    }

    console.unlock_console( );


    static constexpr std::string_view _unsupported{"This option is available only with extended features"};
    printf( "%s\n", _unsupported.data( ) );
}

#else   // !AREG_EXTENDED

void LogCollector::_set_verbose_mode( bool /* makeVerbose */ )
{
    // In simple (ANSI) mode the rate thread always runs verbose; this command is a no-op.
    static constexpr std::string_view _always_on{ "Data rate display is always active in this mode." };
    Console& console = Console::instance();
    console.output_txt(areg::ext::COORD_INFO_MSG, _always_on);
    console.refresh_screen();
}

#endif  // AREG_EXTENDED

void LogCollector::_clean_help()
{
#if     AREG_EXTENDED

    Console::Coord line{ areg::ext::COORD_INFO_MSG };
    Console& console = Console::instance();
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

void LogCollector::_process_update_scopes(const OptionParser::InputOption& optScope)
{
    LogCollector& logger{ LogCollector::instance() };
    ArrayList<areg::MessageEnvelope> msgList;
    _create_scope_message(optScope, msgList);
    for (uint32_t i = 0; i < msgList.size(); ++ i)
    {
        logger.mServiceServer.dispatch_and_forward_logger_message(msgList[i]);
    }
}

void LogCollector::_process_query_scopes(const OptionParser::InputOption& optScope)
{
    LogCollector& logger{ LogCollector::instance() };
    ArrayList<ITEM_ID> listTargets;
    if (optScope.inString.empty() || (optScope.inString[0] == areg::SYNTAX_ALL_MODULES))
    {
        listTargets.add(areg::COOKIE_ANY);
    }
    else
    {
        for (const auto& elem : optScope.inString)
        {
            if (elem == areg::SYNTAX_ALL_MODULES)
            {
                listTargets.clear();
                listTargets.add(areg::COOKIE_ANY);
                break;
            }
            else if (elem.is_numeric())
            {
                listTargets.add(elem.to_uint32());
            }
        }
    }

    for (const auto& target : listTargets.data())
    {
        logger.mServiceServer.dispatch_and_forward_logger_message(areg::message_query_scopes(areg::COOKIE_LOGGER, target));
    }
}

void LogCollector::_create_scope_message(const OptionParser::InputOption& optScope, ArrayList<areg::MessageEnvelope>& msgList)
{
    ASSERT(optScope.inCommand == static_cast<int32_t>(LoggerOption::CMD_LogUpdateScope));
    ASSERT(optScope.inString.empty() == false);

    const OptionParser::StrList& optValues{ optScope.inString };
    String scope;
    for (const auto& entry : optValues)
    {
        if (entry == areg::SYNTAX_END_COMMAND)
        {
            areg::MessageEnvelope msg{ LogCollector::_create_scope_update_message(scope) };
            scope.clear();
            if (msg.is_valid() == false)
            {
                msgList.clear();
                break;
            }
            else
            {
                msgList.add(msg);
            }
        }
        else
        {
            scope += entry;
        }
    }

    if (scope.is_empty() == false)
    {
        areg::MessageEnvelope msg{ LogCollector::_create_scope_update_message(scope) };
        if (msg.is_valid() == false)
        {
            msgList.clear();
        }
        else
        {
            msgList.add(msg);
        }
    }
}

String LogCollector::_normalize_scope_property(const String & scope)
{
    const areg::ConfigKey& propKey{ areg::DefaultPropertyKeys[static_cast<uint32_t>(areg::ConfigEntry::LogScope)] };
    String result;
    if (scope.starts_with(propKey.property))
    {
        result.append(propKey.section)
              .append(areg::SYNTAX_OBJECT_SEPARATOR)
              .append(areg::SYNTAX_ALL_MODULES)
              .append(areg::SYNTAX_OBJECT_SEPARATOR)
              .append(scope);
    }
    else
    {
        String prop(propKey.property);
        prop += areg::SYNTAX_OBJECT_SEPARATOR;
        areg::CharPos pos = scope.find_first(prop);
        if ( scope.is_valid_position(pos))
        {
            result.append(propKey.section)
                  .append(areg::SYNTAX_OBJECT_SEPARATOR)
                  .append(scope);
        }
        else
        {
            result = scope;
            pos = result.find_last(areg::SYNTAX_OBJECT_SEPARATOR);
            if (result.is_valid_position(pos))
            {
                result.insert_at(prop, pos + static_cast<areg::CharCount>(areg::SYNTAX_OBJECT_SEPARATOR.length()));
            }
            else
            {
                result.insert_at(prop, areg::START_POS);
            }

            result = _normalize_scope_property(result);
        }
    }

    return result;
}

areg::MessageEnvelope LogCollector::_create_scope_update_message(const String& scope)
{
    areg::MessageEnvelope result;

    if (scope.is_empty() == false)
    {
        Property prop(LogCollector::_normalize_scope_property(scope));
        if (prop.is_valid() && prop.type() == areg::ConfigEntry::LogScope)
        {
            const PropertyKey& key{ prop.key() };
            ITEM_ID target{ key.is_all_modules() ? areg::COOKIE_ANY : key.module().to_uint32() };
            if (target >= areg::COOKIE_ANY)
            {
                String scopeName{ key.position() };
                uint32_t scopePrio{ prop.value().identifier(areg::LogScopePriorityIndentifiers) };
                result = areg::message_update_scope(areg::COOKIE_LOGGER, target, scopeName, areg::LOG_SCOPE_ID_NONE, scopePrio);
                result.set_target(static_cast<uint32_t>(target));
            }
        }
    }

    return result;
}

inline void LogCollector::_enable_local_logs(ConfigManager& config, bool enable)
{
    constexpr areg::ConfigEntry prioConfKey{ areg::ConfigEntry::LogScope };
    const areg::ConfigKey& keyPrio{ areg::log_scope() };
    uint32_t prios = enable
                ? static_cast<uint32_t>(areg::LogPriority::PrioNotset) | static_cast<uint32_t>(areg::LogPriority::PrioNotset)
                : static_cast<uint32_t>(areg::LogPriority::PrioNotset);
    const String prio{ areg::make_prio_string(prios) };

    config.set_module_property(keyPrio.section, keyPrio.property, String(areg::SYNTAX_ANY_VALUE), prio, prioConfKey, true);
    config.set_log_enabled(areg::LogTarget::Remote, false, true);
}
