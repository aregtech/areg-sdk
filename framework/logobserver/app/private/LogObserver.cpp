/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
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
    };

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
}

//////////////////////////////////////////////////////////////////////////
// LogObserver class implementation
//////////////////////////////////////////////////////////////////////////

const areg::ext::OptionParser::OptionSetup LogObserver::ValidOptions[ ]
{
      { "-e", "--query"     , static_cast<int32_t>(LoggerOption::CMD_LogQueryScopes)  , areg::ext::OptionParser::STRING_NO_RANGE , {}, {}, {} }
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
    if (is_connected)
    {
        _logConnect.lcAddress = address;
        _logConnect.lcPort = port;
    }
    else
    {
        _listInstances.clear();
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
        ASSERT(size >= sizeof(areg::LogEntry));
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
    areg::ext::OptionParser parser(LogObserver::ValidOptions, std::size(LogObserver::ValidOptions));
    if (parser.parse_command_line(argv, static_cast<uint32_t>(argc)))
    {
        uint32_t pos = parser.find_option(static_cast<int32_t>(LogObserver::LoggerOption::CMD_LogLoad));
        if (pos != areg::INVALID_POSITION)
        {
            areg::String filePath{ parser.options().value_at(pos).inString[0] };
            if (areg::File::has_file(filePath))
            {
                fileConfig = filePath;
            }
        }
    }

    ::log_observer_initialize(&evts, fileConfig.as_string());

    _run_console_input_extended();

    areg::Application::signal_quit();
    ::log_observer_disconnect_logger();
    ::log_observer_release();
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
            bool processed{ false };
            const LogObserver::ObserverStatus* status{ nullptr };
            const areg::ext::OptionParser::InputOption & opt = opts[ i ];
            switch ( static_cast<LogObserver::LoggerOption>(opt.inCommand) )
            {
            case LogObserver::LoggerOption::CMD_LogQueryScopes:
                processed = LogObserver::_process_query_scopes(opt);
                status = &_observerStatus[static_cast<uint32_t>(LoggerOption::CMD_LogQueryScopes)];
                break;

            case LogObserver::LoggerOption::CMD_LogSaveConfig:
                processed = LogObserver::_process_save_config(opt);
                status = &_observerStatus[static_cast<uint32_t>(LoggerOption::CMD_LogSaveConfig)];
                break;

            case LogObserver::LoggerOption::CMD_LogPrintHelp:
                processed = LogObserver::_process_print_help();
                status = &_observerStatus[static_cast<uint32_t>(LoggerOption::CMD_LogPrintHelp)];
                break;

            case LogObserver::LoggerOption::CMD_LogInstances:
                processed = LogObserver::_process_info_instances();
                status = &_observerStatus[static_cast<uint32_t>(LoggerOption::CMD_LogInstances)];
                break;

            case LogObserver::LoggerOption::CMD_LogUpdateScope:
                processed = LogObserver::_process_update_scopes(opt);
                status = &_observerStatus[static_cast<uint32_t>(LoggerOption::CMD_LogUpdateScope)];
                break;

            case LogObserver::LoggerOption::CMD_LogPause:
                processed = LogObserver::_process_pause_logging();
                status = &_observerStatus[static_cast<uint32_t>(LoggerOption::CMD_LogPause)];
                break;

            case LogObserver::LoggerOption::CMD_LogQuit:
                quit = true;
                break;

            case LogObserver::LoggerOption::CMD_LogRestart:
                processed = LogObserver::_process_start_logging(true);
                status = &_observerStatus[static_cast<uint32_t>(LoggerOption::CMD_LogRestart)];
                break;

            case LogObserver::LoggerOption::CMD_LogStop:
                processed = LogObserver::_process_start_logging(false);
                status = &_observerStatus[static_cast<uint32_t>(LoggerOption::CMD_LogStop)];
                break;

            case LogObserver::LoggerOption::CMD_LogLoad:      // fall through
            case LogObserver::LoggerOption::CMD_LogUndefined: // fall through
            default:
                hasError = true;
                break;
            }

            if (status != nullptr)
            {
                ASSERT(static_cast<LoggerOption>(opt.inCommand) == status->osOption);
                console.lock_console();
                if (processed && (status->osStatus.empty() == false))
                {
                    console.clear_line(areg::ext::COORD_STATUS_MSG);
                    console.output_txt(areg::ext::COORD_STATUS_MSG, status->osStatus);
                }
                else if ((processed == false) && (status->osError.empty() == false))
                {
                    console.clear_line(areg::ext::COORD_STATUS_MSG);
                    console.output_txt(areg::ext::COORD_STATUS_MSG, status->osError);
                }
                else
                {
                    console.clear_line(areg::ext::COORD_STATUS_MSG);
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

        console.clear_line( areg::ext::COORD_USER_INPUT );
        console.output_txt( areg::ext::COORD_USER_INPUT, areg::ext::FORMAT_WAIT_QUIT );
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
    areg::ext::Console& console = areg::ext::Console::instance();
    console.lock_console();
    for (const auto& text : _msgHelp)
    {
        console.output_txt(line, text);
        ++line.posY;
    }

    console.unlock_console();
    return true;
}

bool LogObserver::_process_info_instances()
{
    static constexpr std::string_view _table{ "   Nr. |  Inst. ID  |  Bits |  Scopes  |  name " };
    static constexpr std::string_view _formt{ "  %3u. |%11u |  x%u  |   %5u  |  %s " };
    static constexpr std::string_view _empty{ "There are no connected instances ..." };

    areg::ext::Console& console = areg::ext::Console::instance();
    areg::ext::Console::Coord coord{ areg::ext::COORD_INFO_MSG };
    console.lock_console();

    if (_listInstances.is_empty())
    {
        console.output_txt(coord, areg::ext::MSG_SEPARATOR);
        ++coord.posY;
        console.output_str(coord, _empty);
        ++coord.posY;
    }
    else
    {
        console.output_txt(coord, areg::ext::MSG_SEPARATOR);
        ++coord.posY;
        console.output_txt(coord, _table);
        ++coord.posY;
        console.output_txt(coord, areg::ext::MSG_SEPARATOR);
        ++coord.posY;
        for (uint32_t i = 0; i < _listInstances.size(); ++ i)
        {
            const LogInstance& instance{ _listInstances[i] };
            uint32_t id{ static_cast<uint32_t>(instance.liCookie) };
            auto pos = _mapScopes.find(instance.liCookie);
            uint32_t scopes{ pos != _mapScopes.invalid_position() ? _mapScopes.value_at(pos).size() : 0u };
            console.output_msg(coord, _formt.data(), (i + 1), id, static_cast<uint32_t>(instance.liBitness), scopes, instance.liName);
            ++coord.posY;
        }
    }

    console.output_txt(coord, areg::ext::MSG_SEPARATOR);
    console.unlock_console();

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
