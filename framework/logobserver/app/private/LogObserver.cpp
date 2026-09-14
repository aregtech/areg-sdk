/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        logobserver/app/private/LogObserver.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Log Observer to run as a console application.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "logobserver/app/LogObserver.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/base/DateTime.hpp"
#include "areg/base/File.hpp"
#include "areg/base/String.hpp"
#include "areg/base/Thread.hpp"
#include "areg/persist/ConfigManager.hpp"
#include "aregextend/service/SystemServiceDefs.hpp"
#include "areglogger/client/LogObserverApi.h"

#include <stdio.h>

//////////////////////////////////////////////////////////////////////////
// The model used only in console mode.
//////////////////////////////////////////////////////////////////////////

// This model defines a areg::ext::Console Service to run to make data rate outputs.
// The areg::ext::Console Service runs only in verbose mode.

namespace
{
    constexpr std::string_view _msgHelp []
    {
          {"Usage of Areg Log Observer console application :"}
        , areg::ext::MSG_SEPARATOR
        , {"-c, --console   : Run the console after the options. Usage: --console"}
        , {"-e, --query     : Query the list of logging scopes. Usage: --query *, \'*\' can be a cookie ID."}
        , {"-f, --config    : Save current configuration.       Usage: --config"}
        , {"-h, --help      : Display this message on console.  Usage: --help"}
        , {"-l, --load      : Command line option to configure. Usage: \'./logobserver --load=<path-to-init-file>\'"}
        , {"-n, --instances : Display list of log instances.    Usage: --instances"}
        , {"-o, --scope     : Update log scope priorities.      Usage: --scope *::areg_base_NESocket=NOTSET, \'*\' can be a cookie."}
        , {"-p, --pause     : Pause the log observer.           Usage: --pause"}
        , {"-q, --quit      : Stop and quit the log observer.   Usage: --quit"}
        , {"-r, --restart   : Start / continue log observer.    Usage: --restart"}
        , {"-x, --stop      : Stop log observer.                Usage: --stop"}
        , areg::ext::MSG_SEPARATOR
        , {"The options are accepted on the console prompt and in the command line."}
        , {"In the command line the observer connects, waits for the data of the log collector,"}
        , {"runs the options in the given order and exits, unless \'--console\' is specified."}
        , areg::ext::MSG_SEPARATOR
    };

    //!< The maximum time in milliseconds to wait for the data of the log collector.
    constexpr uint32_t  MS_WAIT_DATA    { 15000 };
    //!< The time in milliseconds to wait for the first answer of the log collector after connecting.
    constexpr uint32_t  MS_SETTLE_DATA  { 3000 };
    //!< The time in milliseconds between 2 checks of the received data.
    constexpr uint32_t  MS_WAIT_SLICE   { 100 };
    //!< The time in milliseconds to let the requests of the command line options complete.
    constexpr uint32_t  MS_DRAIN_DATA   { 2000 };

    struct LoggerConnect
    {
        areg::String      lcAddress;
        uint16_t    lcPort{ areg::InvalidPort };
    };

    using ListInstances = areg::ArrayList<LogInstance>;
    using ListScopes    = areg::ArrayList<ScopeInfo>;
    using MapScopes     = areg::HashMap<ITEM_ID, ListScopes>;

    LoggerConnect   _logConnect;
    ListInstances   _listInstances;
    MapScopes       _mapScopes;

    //!< Guards the list of connected instances and the map of their scopes.
    areg::Mutex     _dataLock( false );

    //!< True while the options given in the command line are executed. The console is not used.
    bool            _cmdLineMode{ false };

    //!< True when the log collector reported the list of connected instances at least once.
    bool            _hasInstances{ false };

    //!< Locks or unlocks the console. Has no effect in the command line mode.
    void _lock_console( bool doLock )
    {
        if ( _cmdLineMode == false )
        {
            areg::ext::Console & console = areg::ext::Console::instance( );
            if ( doLock )
            {
                console.lock_console( );
            }
            else
            {
                console.unlock_console( );
            }
        }
    }

    //!< Outputs one line at the given console coordinate and steps to the next line.
    //!< In the command line mode the text goes to the standard output.
    void _output_line( areg::ext::Console::Coord & pos, const char * text )
    {
        if ( _cmdLineMode )
        {
            ::printf( "%s\n", text );
        }
        else
        {
            areg::ext::Console::instance( ).output_txt( pos, text );
        }

        ++ pos.posY;
    }
}

//////////////////////////////////////////////////////////////////////////
// LogObserver class implementation
//////////////////////////////////////////////////////////////////////////

const areg::ext::OptionParser::OptionSetup LogObserver::ValidOptions[ ]
{
      { "-c", "--console"   , static_cast<int32_t>(LoggerOption::CMD_LogConsole)      , areg::ext::OptionParser::NO_DATA         , {}, {}, {} }
    , { "-e", "--query"     , static_cast<int32_t>(LoggerOption::CMD_LogQueryScopes)  , areg::ext::OptionParser::STRING_NO_RANGE , {}, {}, {} }
    , { "-f", "--config"    , static_cast<int32_t>(LoggerOption::CMD_LogSaveConfig)   , areg::ext::OptionParser::STRING_NO_RANGE , {}, {}, {} }
    , { "-h", "--help"      , static_cast<int32_t>(LoggerOption::CMD_LogPrintHelp)    , areg::ext::OptionParser::NO_DATA         , {}, {}, {} }
    , { "-l", "--load"      , static_cast<int32_t>(LoggerOption::CMD_LogLoad)         , areg::ext::OptionParser::STRING_NO_RANGE , {}, {}, {} }
    , { "-n", "--instances" , static_cast<int32_t>(LoggerOption::CMD_LogInstances)    , areg::ext::OptionParser::NO_DATA         , {}, {}, {} }
    , { "-o", "--scope"     , static_cast<int32_t>(LoggerOption::CMD_LogUpdateScope)  , areg::ext::OptionParser::STRING_NO_RANGE , {}, {}, {} }
    , { "-p", "--pause"     , static_cast<int32_t>(LoggerOption::CMD_LogPause)        , areg::ext::OptionParser::NO_DATA         , {}, {}, {} }
    , { "-q", "--quit"      , static_cast<int32_t>(LoggerOption::CMD_LogQuit)         , areg::ext::OptionParser::NO_DATA         , {}, {}, {} }
    , { "-r", "--restart"   , static_cast<int32_t>(LoggerOption::CMD_LogRestart)      , areg::ext::OptionParser::NO_DATA         , {}, {}, {} }
    , { "-x", "--stop"      , static_cast<int32_t>(LoggerOption::CMD_LogStop)         , areg::ext::OptionParser::NO_DATA         , {}, {}, {} }
};

LogObserver & LogObserver::instance()
{
    static LogObserver _instance;
    return _instance;
}

areg::ext::Console::CallBack LogObserver::option_check_callback() const
{
    return areg::ext::Console::CallBack( LogObserver::_check_command );
}

void LogObserver::_run_console_input_extended()
{
    areg::ext::Console & console = areg::ext::Console::instance( );
    LogObserver::_output_title( );

    console.enable_console_input(true);
    console.output_txt(areg::ext::COORD_STATUS_MSG, LogObserver::STATUS_INITIALIZED);
    console.output_txt(areg::ext::COORD_USER_INPUT, areg::ext::FORMAT_WAIT_QUIT);
    // Place the cursor right after the prompt text so that the input is echoed there.
    console.set_cursor_cur_position({ areg::ext::COORD_USER_INPUT.posX + static_cast<int32_t>(areg::ext::FORMAT_WAIT_QUIT.size()),
                                      areg::ext::COORD_USER_INPUT.posY });
    console.refresh_screen();
    console.wait_for_input(option_check_callback());

    console.move_cursor_one_line_up( );
    console.clear_screen( );
    console.uninitialize( );
}

void LogObserver::callback_observer_configured(bool /* is_enabled */, const char* /* address */, uint16_t /* port */)
{
}

void LogObserver::callback_database_configured(bool /* is_enabled */, const char* /* dbName */, const char* /* dbLocation */, const char* /* user */ )
{
}

void LogObserver::callback_service_connected(bool is_connected, const char* address, uint16_t port)
{
    areg::Lock lock(_dataLock);
    if (is_connected)
    {
        _logConnect.lcAddress = address;
        _logConnect.lcPort = port;
    }
    else
    {
        _listInstances.clear();
        _mapScopes.clear();
        _hasInstances = false;
        _logConnect.lcAddress.clear();
        _logConnect.lcPort = areg::InvalidPort;
    }
}

void LogObserver::callback_observer_started(bool /* is_started */)
{
}

void LogObserver::callback_log_db_created(const char* /* dbLocation */)
{
}

void LogObserver::callback_messaging_failed()
{
}

void LogObserver::callback_connected_instances(const LogInstance* instances, uint32_t count)
{
    areg::Lock lock(_dataLock);
    _hasInstances = true;
    if (count == 0)
    {
        _listInstances.clear();
        _mapScopes.clear();
        return;
    }

    for (uint32_t i = 0; i < count; ++i)
    {
        const LogInstance& inst{ instances[i] };
        bool contains{ false };
        for (uint32_t j = 0; j < _listInstances.size(); ++j)
        {
            if (_listInstances[j].liCookie == inst.liCookie)
            {
                contains = true;
                break;
            }
        }

        if (contains == false)
        {
            TIME64 now{ areg::DateTime::now() };
            areg::LogEntry log{ };
            log.logDataType     = areg::LogDataType::Local;
            log.logMsgType      = areg::LogMessageType::MessageText;
            log.logMessagePrio  = areg::LogPriority::PrioAny;
            log.logSource       = inst.liSource;
            log.logTarget       = areg::COOKIE_LOCAL;
            log.logCookie       = inst.liCookie;
            log.logModuleId     = 0u;
            log.logThreadId     = 0u;
            log.logTimestamp    = inst.liTimestamp;
            log.logReceived     = static_cast<TIME64>(now);
            log.logDuration     = 0u;
            log.logScopeId      = 0u;
            log.logSessionId    = 0u;
            log.logMessageLen   = static_cast<uint32_t>(areg::String::format_string(log.logMessage, areg::LOG_MSG_SIZE, "CONNECTED the x%u instance %s with cookie %u", inst.liBitness, inst.liName, inst.liCookie));
            log.logThreadLen    = 0;
            log.logThread[0]    = areg::String::EmptyChar;
            log.logModuleId     = 0;
            log.logModuleLen    = static_cast<uint32_t>(areg::copy_string(log.logModule, areg::LOG_NAME_SIZE, inst.liName));

            _listInstances.add(inst);
            areg::log_any_message(log);

            ASSERT(_mapScopes.contains(inst.liCookie) == false);
            ::log_observer_request_scopes(inst.liCookie);
        }
    }
}

void LogObserver::callback_disconnected_instances(const ITEM_ID * instances, uint32_t count)
{
    areg::Lock lock(_dataLock);
    for (uint32_t i = 0; i < count; ++i)
    {
        const ITEM_ID& cookie = instances[i];
        for (uint32_t j = 0; j < _listInstances.size(); ++j)
        {
            const LogInstance& inst{ _listInstances[j] };
            if (inst.liCookie == cookie)
            {
                TIME64 now{ areg::DateTime::now() };
                areg::LogEntry log{ };
                log.logDataType     = areg::LogDataType::Local;
                log.logMsgType      = areg::LogMessageType::MessageText;
                log.logMessagePrio  = areg::LogPriority::PrioAny;
                log.logSource       = inst.liSource;
                log.logTarget       = areg::COOKIE_LOCAL;
                log.logCookie       = inst.liCookie;
                log.logModuleId     = 0u;
                log.logThreadId     = 0u;
                log.logTimestamp    = static_cast<TIME64>(now);
                log.logReceived     = log.logTimestamp;
                log.logDuration     = 0u;
                log.logScopeId      = 0u;
                log.logSessionId    = 0u;
                log.logMessageLen   = static_cast<uint32_t>(areg::String::format_string(log.logMessage, areg::LOG_MSG_SIZE, "DISCONNECTED the x%u instance %s with cookie %u", inst.liBitness, inst.liName, inst.liCookie));
                log.logThreadLen    = 0;
                log.logThread[0]    = areg::String::EmptyChar;
                log.logModuleId     = 0;
                log.logModuleLen    = static_cast<uint32_t>(areg::copy_string(log.logModule, areg::LOG_NAME_SIZE, inst.liName));

                _listInstances.remove_at(j, 1);
                _mapScopes.remove_at(cookie);

                areg::log_any_message(log);
                break;
            }
        }
    }
}

void LogObserver::callback_log_scopes(ITEM_ID cookie, const ScopeInfo* scopes, uint32_t count)
{
    areg::Lock lock(_dataLock);
    for (uint32_t i = 0; i < _listInstances.size(); ++i)
    {
        const LogInstance& inst{ _listInstances[i] };
        if (cookie == inst.liCookie)
        {
            areg::LogEntry log{ };
            log.logDataType     = areg::LogDataType::Local;
            log.logMsgType      = areg::LogMessageType::MessageText;
            log.logMessagePrio  = areg::LogPriority::PrioAny;
            log.logSource       = inst.liSource;
            log.logTarget       = areg::COOKIE_LOCAL;
            log.logCookie       = inst.liCookie;
            log.logModuleId     = 0u;
            log.logThreadId     = 0u;
            log.logTimestamp    = static_cast<TIME64>(areg::DateTime::now());
            log.logReceived     = log.logTimestamp;
            log.logDuration     = 0u;
            log.logScopeId      = 0u;
            log.logSessionId    = 0u;
            log.logMessageLen   = static_cast<uint32_t>(areg::String::format_string(log.logMessage, areg::LOG_MSG_SIZE, "Registered %u scopes for instance %s with cookie %u", count, inst.liName, inst.liCookie));
            log.logThreadLen    = 0;
            log.logThread[0]    = areg::String::EmptyChar;
            log.logModuleId     = 0;
            log.logModuleLen    = static_cast<uint32_t>(areg::copy_string(log.logModule, areg::LOG_NAME_SIZE, inst.liName));

            _mapScopes.set_value_at(cookie, ListScopes());
            ListScopes& scopeList{ _mapScopes.value_at(cookie) };
            scopeList.resize(count);
            for (uint32_t j = 0; j < count; ++j)
            {
                scopeList[j] = scopes[j];
            }

            areg::log_any_message(log);
            break;
        }
    }
}

void LogObserver::callback_log_update_scopes(ITEM_ID /* cookie */, const ScopeInfo* /* scopes */, uint32_t /* count */)
{
}

void LogObserver::callback_log_message(const LogEntry* /* logMessage */)
{
}

#ifdef  DEBUG
void LogObserver::callback_log_message_ex(const uint8_t* logBuffer, uint32_t size)
#else   // DEBUG
void LogObserver::callback_log_message_ex(const uint8_t* logBuffer, uint32_t /*size*/)
#endif  // DEBUG
{
    if (logBuffer != nullptr)
    {
        ASSERT(size >= areg::log_entry_head());
        const areg::LogEntry & log{ reinterpret_cast<const areg::LogEntry &>(*logBuffer)};
        areg::log_any_message(log);
    }
}

void LogObserver::log_main( int32_t argc, char ** argv )
{
    ObserverEvents evts
    {
          &LogObserver::callback_observer_configured
        , &LogObserver::callback_database_configured
        , &LogObserver::callback_service_connected
        , &LogObserver::callback_observer_started
        , &LogObserver::callback_log_db_created
        , &LogObserver::callback_messaging_failed
        , &LogObserver::callback_connected_instances
        , &LogObserver::callback_disconnected_instances
        , &LogObserver::callback_log_scopes
        , &LogObserver::callback_log_update_scopes
        , nullptr       // set nullptr to receive messages via `callback_log_message_ex` callback
        , &LogObserver::callback_log_message_ex
    };

    areg::Application::set_working_directory(nullptr);
    areg::String fileConfig(areg::DEFAULT_CONFIG_FILE);
    areg::ext::OptionParser::InputOptionList commands;
    bool printHelp{ false };
    bool optConsole{ false };

    areg::ext::OptionParser parser(LogObserver::ValidOptions, std::size(LogObserver::ValidOptions));
    if (parser.parse_command_line(argv, static_cast<uint32_t>(argc)))
    {
        const areg::ext::OptionParser::InputOptionList& opts{ parser.options() };
        for (uint32_t i = 0; i < opts.size(); ++ i)
        {
            const areg::ext::OptionParser::InputOption& opt{ opts[i] };
            switch (static_cast<LoggerOption>(opt.inCommand))
            {
            case LoggerOption::CMD_LogLoad:
                if (opt.inString.empty() == false)
                {
                    areg::String filePath{ opt.inString[0] };
                    if (areg::File::has_file(filePath))
                    {
                        fileConfig = filePath;
                    }
                }
                break;

            case LoggerOption::CMD_LogPrintHelp:
                printHelp = true;
                break;

            case LoggerOption::CMD_LogConsole:
                optConsole = true;
                break;

            default:
                commands.add(opt);
                break;
            }
        }
    }
    else
    {
        printHelp = true;
    }

    if (printHelp)
    {
        _cmdLineMode = true;
        LogObserver::_process_print_help();
        _cmdLineMode = false;
        return;
    }

    ::log_observer_initialize(&evts, fileConfig.as_string());

    bool runConsole{ commands.is_empty() || optConsole };
    if (commands.is_empty() == false)
    {
        runConsole = LogObserver::_run_command_line(commands) ? false : optConsole;
    }

    if (runConsole)
    {
        _run_console_input_extended();
    }

    areg::Application::signal_quit();
    ::log_observer_disconnect_logger();
    ::log_observer_release();
}

bool LogObserver::_wait_collector_data()
{
    uint32_t msPassed{ 0u };
    while ((msPassed < MS_WAIT_DATA) && (::log_observer_is_connected() == false))
    {
        areg::Thread::sleep(MS_WAIT_SLICE);
        msPassed += MS_WAIT_SLICE;
    }

    if (::log_observer_is_connected() == false)
    {
        return false;
    }

    ::log_observer_request_instances();

    // Waits the settle time for the first answer, then waits until each instance reported its scopes.
    uint32_t msSettle{ 0u };
    bool result{ false };
    while (msPassed < MS_WAIT_DATA)
    {
        areg::Thread::sleep(MS_WAIT_SLICE);
        msPassed += MS_WAIT_SLICE;
        msSettle += MS_WAIT_SLICE;

        areg::Lock lock(_dataLock);
        if (_hasInstances == false)
        {
            result = msSettle >= MS_SETTLE_DATA;
            if (result)
            {
                break;
            }

            continue;
        }

        result = true;
        for (uint32_t i = 0; i < _listInstances.size(); ++ i)
        {
            if (_mapScopes.contains(_listInstances[i].liCookie) == false)
            {
                result = false;
                break;
            }
        }

        if (result)
        {
            break;
        }
    }

    return result;
}

bool LogObserver::_run_command_line(const areg::ext::OptionParser::InputOptionList& options)
{
    _cmdLineMode = true;

    if (::log_observer_connect_logger(nullptr, nullptr, areg::InvalidPort) == false)
    {
        ::printf("Failed to trigger the connection with the log collector, check the configuration.\n");
    }
    else if (LogObserver::_wait_collector_data() == false)
    {
        ::printf("The log collector reported no data within %u seconds, the options may have no effect.\n", MS_WAIT_DATA / 1000u);
    }

    bool quit{ false };
    for (uint32_t i = 0; i < options.size(); ++ i)
    {
        LogObserver::CommandResult result;
        if (LogObserver::_execute_option(options[i], result) == false)
        {
            ::printf("ERROR, unexpected option, type \'--help\' to display the list of options.\n");
            continue;
        }

        quit = quit || result.crQuit;
        if (result.crStatus != nullptr)
        {
            const std::string_view& msg{ result.crProcessed ? result.crStatus->osStatus : result.crStatus->osError };
            if (msg.empty() == false)
            {
                ::printf("%.*s\n", static_cast<int>(msg.length()), msg.data());
            }
        }
    }

    // Waits for the answers of the requests that the options sent.
    areg::Thread::sleep(MS_DRAIN_DATA);
    _cmdLineMode = false;

    return quit;
}

bool LogObserver::_execute_option(const areg::ext::OptionParser::InputOption& opt, LogObserver::CommandResult& result)
{
    bool known{ true };
    const LogObserver::LoggerOption option{ static_cast<LogObserver::LoggerOption>(opt.inCommand) };

    switch ( option )
    {
    case LogObserver::LoggerOption::CMD_LogQueryScopes:
        result.crProcessed = LogObserver::_process_query_scopes(opt);
        break;

    case LogObserver::LoggerOption::CMD_LogSaveConfig:
        result.crProcessed = LogObserver::_process_save_config(opt);
        break;

    case LogObserver::LoggerOption::CMD_LogPrintHelp:
        result.crProcessed = LogObserver::_process_print_help();
        break;

    case LogObserver::LoggerOption::CMD_LogInstances:
        result.crProcessed = LogObserver::_process_info_instances();
        break;

    case LogObserver::LoggerOption::CMD_LogUpdateScope:
        result.crProcessed = LogObserver::_process_update_scopes(opt);
        break;

    case LogObserver::LoggerOption::CMD_LogPause:
        result.crProcessed = LogObserver::_process_pause_logging();
        break;

    case LogObserver::LoggerOption::CMD_LogRestart:
        result.crProcessed = LogObserver::_process_start_logging(true);
        break;

    case LogObserver::LoggerOption::CMD_LogStop:
        result.crProcessed = LogObserver::_process_start_logging(false);
        break;

    case LogObserver::LoggerOption::CMD_LogConsole:
        result.crProcessed = true;
        break;

    case LogObserver::LoggerOption::CMD_LogQuit:
        result.crQuit = true;
        result.crProcessed = true;
        return true;

    case LogObserver::LoggerOption::CMD_LogLoad:      // fall through
    case LogObserver::LoggerOption::CMD_LogUndefined: // fall through
    default:
        known = false;
        break;
    }

    if (known)
    {
        result.crStatus = &_observerStatus[static_cast<uint32_t>(opt.inCommand)];
        ASSERT(result.crStatus->osOption == option);
    }

    return known;
}

bool LogObserver::_check_command(const areg::String& cmd)
{
    areg::ext::OptionParser parser( LogObserver::ValidOptions, std::size(LogObserver::ValidOptions) );
    bool quit{ false };
    bool hasError {false};

    LogObserver::_clean_help();
    areg::ext::Console& console = areg::ext::Console::instance();

    if ( parser.parse_option_line( cmd ) )
    {
        const areg::ext::OptionParser::InputOptionList & opts = parser.options( );
        for ( uint32_t i = 0; i < opts.size( ); ++ i )
        {
            LogObserver::CommandResult result;
            if (LogObserver::_execute_option(opts[i], result) == false)
            {
                hasError = true;
                continue;
            }

            quit = quit || result.crQuit;
            if (result.crStatus != nullptr)
            {
                const std::string_view& msg{ result.crProcessed ? result.crStatus->osStatus : result.crStatus->osError };
                console.lock_console();
                console.clear_line(areg::ext::COORD_STATUS_MSG);
                if (msg.empty() == false)
                {
                    console.output_txt(areg::ext::COORD_STATUS_MSG, msg);
                }

                console.unlock_console();
            }
        }
    }
    else
    {
        hasError = true;
    }

    console.lock_console();
    if ( quit == false )
    {
        if ( hasError )
        {
            console.output_msg( areg::ext::COORD_ERROR_MSG, areg::ext::FORMAT_MSG_ERROR.data(), cmd.as_string());
        }
        else
        {
            console.clear_line( areg::ext::COORD_ERROR_MSG );
        }

        console.clear_line( areg::ext::COORD_USER_INPUT );
        console.output_txt( areg::ext::COORD_USER_INPUT, areg::ext::FORMAT_WAIT_QUIT );
        // Place the cursor right after the prompt text so that the input is echoed there.
        console.set_cursor_cur_position({ areg::ext::COORD_USER_INPUT.posX + static_cast<int32_t>(areg::ext::FORMAT_WAIT_QUIT.size()),
                                          areg::ext::COORD_USER_INPUT.posY });
    }
    else
    {
        console.output_txt( areg::ext::COORD_INFO_MSG, areg::ext::FORMAT_QUIT_APP );
    }

    console.refresh_screen( );
    console.unlock_console( );

    return quit;
}

void LogObserver::_output_title()
{
    areg::ext::Console & console = areg::ext::Console::instance( );
    console.lock_console();
    console.output_txt( areg::ext::COORD_TITLE, LogObserver::APP_TITLE );
    console.output_txt( areg::ext::COORD_SUBTITLE, areg::ext::MSG_SEPARATOR );
    console.unlock_console();
}

void LogObserver::_output_info( const areg::String & info )
{
    areg::ext::Console & console = areg::ext::Console::instance( );
    areg::ext::Console::Coord coord{areg::ext::COORD_INFO_MSG};
    console.lock_console( );

    console.output_txt( coord, areg::ext::MSG_SEPARATOR );
    ++ coord.posY;
    console.output_str( coord, info );

    console.unlock_console( );
}

void LogObserver::_clean_help()
{
    if (_cmdLineMode)
    {
        return;
    }

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
}

bool LogObserver::_process_save_config(const areg::ext::OptionParser::InputOption& optSave)
{
    areg::ArrayList<ITEM_ID> listTargets;
    if (optSave.inString.empty() || (optSave.inString[0] == areg::SYNTAX_ALL_MODULES))
    {
        listTargets.add(areg::TARGET_ALL);
    }
    else
    {
        for (const auto& elem : optSave.inString)
        {
            if (elem == areg::SYNTAX_ALL_MODULES)
            {
                listTargets.clear();
                listTargets.add(areg::TARGET_ALL);
                break;
            }
            else if (elem.is_numeric())
            {
                listTargets.add(elem.to_uint32());
            }
        }
    }

    bool result{ true };
    for (const auto& target : listTargets.data())
    {
        result &= ::log_observer_request_save_config(target);
    }

    return result;
}

bool LogObserver::_process_print_help()
{
    areg::ext::Console::Coord line{ areg::ext::COORD_INFO_MSG };
    _lock_console(true);
    for (const auto& text : _msgHelp)
    {
        _output_line(line, text.data());
    }

    _lock_console(false);
    return true;
}

bool LogObserver::_process_info_instances()
{
    static constexpr std::string_view _table{ "   Nr. |  Inst. ID  |  Bits |  Scopes  |  name " };
    static constexpr std::string_view _formt{ "  %3u. |%11u |  x%u  |   %5u  |  %s " };
    static constexpr std::string_view _empty{ "There are no connected instances ..." };

    areg::ext::Console::Coord coord{ areg::ext::COORD_INFO_MSG };
    _lock_console(true);
    areg::Lock lock(_dataLock);

    _output_line(coord, areg::ext::MSG_SEPARATOR.data());
    if (_listInstances.is_empty())
    {
        _output_line(coord, _empty.data());
    }
    else
    {
        _output_line(coord, _table.data());
        _output_line(coord, areg::ext::MSG_SEPARATOR.data());
        for (uint32_t i = 0; i < _listInstances.size(); ++ i)
        {
            const LogInstance& instance{ _listInstances[i] };
            uint32_t id{ static_cast<uint32_t>(instance.liCookie) };
            auto pos = _mapScopes.find(instance.liCookie);
            uint32_t scopes{ pos != _mapScopes.invalid_position() ? _mapScopes.value_at(pos).size() : 0u };
            areg::String line;
            line.format(_formt.data(), (i + 1), id, static_cast<uint32_t>(instance.liBitness), scopes, instance.liName);
            _output_line(coord, line.as_string());
        }
    }

    _output_line(coord, areg::ext::MSG_SEPARATOR.data());
    _lock_console(false);

    return true;
}

bool LogObserver::_process_update_scopes(const areg::ext::OptionParser::InputOption& optScope)
{
    bool result{ false };
    ASSERT(optScope.inCommand == static_cast<int32_t>(LoggerOption::CMD_LogUpdateScope));
    ASSERT(optScope.inString.empty() == false);

    const areg::ext::OptionParser::StrList& optValues{ optScope.inString };
    areg::String scope;
    for (const auto& entry : optValues)
    {
        if (entry == areg::SYNTAX_END_COMMAND)
        {
            LogObserver::_send_scope_update_message(scope);
            scope.clear();
        }
        else
        {
            scope += entry;
        }
    }

    if (scope.is_empty() == false)
    {
        result = LogObserver::_send_scope_update_message(scope);
    }

    return result;
}

bool LogObserver::_process_pause_logging()
{
    return ::log_observer_pause_logging(true);
}

bool LogObserver::_process_start_logging(bool doStart)
{
    bool result{ true };
    if (doStart)
    {
        if (::log_observer_is_initialized())
        {
            if (::log_observer_is_connected() == false)
            {
                result = ::log_observer_connect_logger(nullptr, nullptr, areg::InvalidPort);
            }
            else if (::log_observer_is_started() == false)
            {
                result = ::log_observer_pause_logging(false);
            }
        }
        else
        {
            result = false;
        }
    }
    else
    {
        ::log_observer_disconnect_logger();
    }

    return result;
}

bool LogObserver::_process_query_scopes(const areg::ext::OptionParser::InputOption& optScope)
{
    bool result{ true };
    areg::ArrayList<ITEM_ID> listTargets;
    if (optScope.inString.empty() || (optScope.inString[0] == areg::SYNTAX_ALL_MODULES))
    {
        listTargets.add(areg::TARGET_ALL);
    }
    else
    {
        for (const auto& elem : optScope.inString)
        {
            if (elem == areg::SYNTAX_ALL_MODULES)
            {
                listTargets.clear();
                listTargets.add(areg::TARGET_ALL);
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
        result &= ::log_observer_request_scopes(target);
    }

    return result;
}

areg::String LogObserver::_normalize_scope_property(const areg::String & scope)
{
    const areg::ConfigKey& propKey{ areg::DefaultPropertyKeys[static_cast<uint32_t>(areg::ConfigEntry::LogScope)] };
    areg::String result;
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
        areg::String prop(propKey.property);
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

bool LogObserver::_send_scope_update_message(const areg::String& scope)
{
    bool result{ false };

    if (scope.is_empty() == false)
    {
        areg::Property prop(LogObserver::_normalize_scope_property(scope));
        if (prop.is_valid() && prop.type() == areg::ConfigEntry::LogScope)
        {
            const areg::PropertyKey& key{ prop.key() };
            ITEM_ID target{ key.is_all_modules() ? areg::TARGET_ALL : key.module().to_uint32() };
            if (target >= areg::TARGET_ALL)
            {
                areg::String scopeName{ key.position() };
                uint32_t scopePrio{ prop.value().identifier(areg::LogScopePriorityIndentifiers) };
                ScopeInfo logScope;
                logScope.lsId   = areg::make_scope_id_ex(scopeName.as_string());
                logScope.lsPrio = scopePrio;
                areg::copy_string<char>(logScope.lsName, LENGTH_SCOPE, scopeName.as_string(), scopeName.length());
                result = ::log_observer_request_change_scope_prio(target, &logScope, 1);
            }
        }
    }

    return result;
}

inline void LogObserver::enable_local_logs(areg::ConfigManager& config, bool /* enable */)
{
    constexpr areg::ConfigEntry prioConfKey{ areg::ConfigEntry::LogScope };
    const areg::ConfigKey& keyPrio{ areg::log_scope() };
    uint32_t prios = static_cast<uint32_t>(areg::LogPriority::PrioNotset);
    const areg::String prio{ areg::make_prio_string(prios) };

    config.set_module_property(keyPrio.section, keyPrio.property, areg::String(areg::SYNTAX_ANY_VALUE), prio, prioConfKey, true);
    config.set_log_enabled(areg::LogTarget::Remote, false, true);
}
