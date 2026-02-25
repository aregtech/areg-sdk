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

// This model defines a Console Service to run to make data rate outputs.
// The Console Service runs only in verbose mode.

namespace
{
    constexpr std::string_view _msgHelp []
    {
          {"Usage of Areg Log Observer console application :"}
        , NESystemService::MSG_SEPARATOR
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
        , NESystemService::MSG_SEPARATOR
    };

    struct LoggerConnect
    {
        String      lcAddress;
        uint16_t    lcPort{ NESocket::InvalidPort };
    };

    using ListInstances = ArrayList<LogInstance>;
    using ListScopes    = ArrayList<ScopeInfo>;
    using MapScopes     = HashMap<ITEM_ID, ListScopes>;

    LoggerConnect   _logConnect;
    ListInstances   _listInstances;
    MapScopes       _mapScopes;
}

//////////////////////////////////////////////////////////////////////////
// LogObserver class implementation
//////////////////////////////////////////////////////////////////////////

const OptionParser::OptionSetup LogObserver::ValidOptions[ ]
{
      { "-e", "--query"     , static_cast<int32_t>(LoggerOption::CMD_LogQueryScopes)  , OptionParser::STRING_NO_RANGE , {}, {}, {} }
    , { "-f", "--config"    , static_cast<int32_t>(LoggerOption::CMD_LogSaveConfig)   , OptionParser::STRING_NO_RANGE , {}, {}, {} }
    , { "-h", "--help"      , static_cast<int32_t>(LoggerOption::CMD_LogPrintHelp)    , OptionParser::NO_DATA         , {}, {}, {} }
    , { "-l", "--load"      , static_cast<int32_t>(LoggerOption::CMD_LogLoad)         , OptionParser::STRING_NO_RANGE , {}, {}, {} }
    , { "-n", "--instances" , static_cast<int32_t>(LoggerOption::CMD_LogInstances)    , OptionParser::NO_DATA         , {}, {}, {} }
    , { "-o", "--scope"     , static_cast<int32_t>(LoggerOption::CMD_LogUpdateScope)  , OptionParser::STRING_NO_RANGE , {}, {}, {} }
    , { "-p", "--pause"     , static_cast<int32_t>(LoggerOption::CMD_LogPause)        , OptionParser::NO_DATA         , {}, {}, {} }
    , { "-q", "--quit"      , static_cast<int32_t>(LoggerOption::CMD_LogQuit)         , OptionParser::NO_DATA         , {}, {}, {} }
    , { "-r", "--restart"   , static_cast<int32_t>(LoggerOption::CMD_LogRestart)      , OptionParser::NO_DATA         , {}, {}, {} }
    , { "-x", "--stop"      , static_cast<int32_t>(LoggerOption::CMD_LogStop)         , OptionParser::NO_DATA         , {}, {}, {} }
};

LogObserver & LogObserver::instance()
{
    static LogObserver _instance;
    return _instance;
}

Console::CallBack LogObserver::getOptionCheckCallback() const
{
    return Console::CallBack( LogObserver::_checkCommand );
}

void LogObserver::_runConsoleInputExtended()
{
    Console & console = Console::instance( );
    LogObserver::_outputTitle( );

    console.enableConsoleInput(true);
    console.outputTxt(NESystemService::COORD_STATUS_MSG, LogObserver::STATUS_INITIALIZED);
    console.outputTxt(NESystemService::COORD_USER_INPUT, NESystemService::FORMAT_WAIT_QUIT);
    console.waitForInput(getOptionCheckCallback());

    console.moveCursorOneLineDown( );
    console.clearScreen( );
    console.uninitialize( );
}

void LogObserver::callbackObserverConfigured(bool /* is_enabled */, const char* /* address */, uint16_t /* port */)
{
}

void LogObserver::callbackDatabaseConfigured(bool /* is_enabled */, const char* /* dbName */, const char* /* dbLocation */, const char* /* user */ )
{
}

void LogObserver::callbackServiceConnected(bool is_connected, const char* address, uint16_t port)
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
        _logConnect.lcPort = NESocket::InvalidPort;
    }
}

void LogObserver::callbackObserverStarted(bool /* is_started */)
{
}

void LogObserver::callbackLogDbCreated(const char* /* dbLocation */)
{
}

void LogObserver::callbackMessagingFailed()
{
}

void LogObserver::callbackConnectedInstances(const LogInstance* instances, uint32_t count)
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
            TIME64 now{ DateTime::now() };
            NELogging::LogEntry log{ };
            log.logDataType     = NELogging::LogDataType::Local;
            log.logMsgType      = NELogging::LogMessageType::MessageText;
            log.logMessagePrio  = NELogging::LogPriority::PrioAny;
            log.logSource       = inst.liSource;
            log.logTarget       = NEService::COOKIE_LOCAL;
            log.logCookie       = inst.liCookie;
            log.logModuleId     = 0u;
            log.logThreadId     = 0u;
            log.logTimestamp    = inst.liTimestamp;
            log.logReceived     = static_cast<TIME64>(now);
            log.logDuration     = 0u;
            log.logScopeId      = 0u;
            log.logSessionId    = 0u;
            log.logMessageLen   = static_cast<uint32_t>(String::format_string(log.log_message, NELogging::LOG_MESSAGE_IZE, "CONNECTED the x%u instance %s with cookie %llu", inst.liBitness, inst.liName, inst.liCookie));
            log.logThreadLen    = 0;
            log.logThread[0]    = String::EmptyChar;
            log.logModuleId     = 0;
            log.logModuleLen    = static_cast<uint32_t>(NEString::copy_string(log.logModule, NELogging::LOG_NAMES_SIZE, inst.liName));

            _listInstances.add(inst);
            NELogging::log_any_message(log);

            ASSERT(_mapScopes.contains(inst.liCookie) == false);
            ::logObserverRequestScopes(inst.liCookie);
        }
    }
}

void LogObserver::callbackDisconnecteInstances(const ITEM_ID * instances, uint32_t count)
{
    for (uint32_t i = 0; i < count; ++i)
    {
        const ITEM_ID& cookie = instances[i];
        for (uint32_t j = 0; j < _listInstances.size(); ++j)
        {
            const LogInstance& inst{ _listInstances[j] };
            if (inst.liCookie == cookie)
            {
                TIME64 now{ DateTime::now() };
                NELogging::LogEntry log{ };
                log.logDataType     = NELogging::LogDataType::Local;
                log.logMsgType      = NELogging::LogMessageType::MessageText;
                log.logMessagePrio  = NELogging::LogPriority::PrioAny;
                log.logSource       = inst.liSource;
                log.logTarget       = NEService::COOKIE_LOCAL;
                log.logCookie       = inst.liCookie;
                log.logModuleId     = 0u;
                log.logThreadId     = 0u;
                log.logTimestamp    = static_cast<TIME64>(now);
                log.logReceived     = log.logTimestamp;
                log.logDuration     = 0u;
                log.logScopeId      = 0u;
                log.logSessionId    = 0u;
                log.logMessageLen   = static_cast<uint32_t>(String::format_string(log.log_message, NELogging::LOG_MESSAGE_IZE, "DISCONNECTED the x%u instance %s with cookie %llu", inst.liBitness, inst.liName, inst.liCookie));
                log.logThreadLen    = 0;
                log.logThread[0]    = String::EmptyChar;
                log.logModuleId     = 0;
                log.logModuleLen    = static_cast<uint32_t>(NEString::copy_string(log.logModule, NELogging::LOG_NAMES_SIZE, inst.liName));

                _listInstances.remove_at(j, 1);
                _mapScopes.remove_at(cookie);

                NELogging::log_any_message(log);
                break;
            }
        }
    }
}

void LogObserver::callbackLogScopes(ITEM_ID cookie, const ScopeInfo* scopes, uint32_t count)
{
    for (uint32_t i = 0; i < _listInstances.size(); ++i)
    {
        const LogInstance& inst{ _listInstances[i] };
        if (cookie == inst.liCookie)
        {
            NELogging::LogEntry log{ };
            log.logDataType     = NELogging::LogDataType::Local;
            log.logMsgType      = NELogging::LogMessageType::MessageText;
            log.logMessagePrio  = NELogging::LogPriority::PrioAny;
            log.logSource       = inst.liSource;
            log.logTarget       = NEService::COOKIE_LOCAL;
            log.logCookie       = inst.liCookie;
            log.logModuleId     = 0u;
            log.logThreadId     = 0u;
            log.logTimestamp    = static_cast<TIME64>(DateTime::now());
            log.logReceived     = log.logTimestamp;
            log.logDuration     = 0u;
            log.logScopeId      = 0u;
            log.logSessionId    = 0u;
            log.logMessageLen   = static_cast<uint32_t>(String::format_string(log.log_message, NELogging::LOG_MESSAGE_IZE, "Registered %u scopes for instance %s with cookie %llu", count, inst.liName, inst.liCookie));
            log.logThreadLen    = 0;
            log.logThread[0]    = String::EmptyChar;
            log.logModuleId     = 0;
            log.logModuleLen    = static_cast<uint32_t>(NEString::copy_string(log.logModule, NELogging::LOG_NAMES_SIZE, inst.liName));

            _mapScopes.set_at(cookie, ListScopes());
            ListScopes& scopeList{ _mapScopes.at(cookie) };
            scopeList.resize(count);
            for (uint32_t j = 0; j < count; ++j)
            {
                scopeList[j] = scopes[j];
            }

            NELogging::log_any_message(log);
            break;
        }
    }
}

void LogObserver::callbackLogUpdateScopes(ITEM_ID /* cookie */, const ScopeInfo* /* scopes */, uint32_t /* count */)
{
}

void LogObserver::callbackLogMessage(const LogEntry* /* log_message */)
{
}

#ifdef  DEBUG
void LogObserver::callbackLogMessageEx(const uint8_t* logBuffer, uint32_t size)
#else   // DEBUG
void LogObserver::callbackLogMessageEx(const uint8_t* logBuffer, uint32_t /*size*/)
#endif  // DEBUG
{
    if (logBuffer != nullptr)
    {
        ASSERT(size >= sizeof(NELogging::LogEntry));
        const NELogging::LogEntry & log{ reinterpret_cast<const NELogging::LogEntry &>(*logBuffer)};
        NELogging::log_any_message(log);
    }
}

void LogObserver::logMain( int32_t argc, char ** argv )
{
    ObserverEvents evts
    {
          &LogObserver::callbackObserverConfigured
        , &LogObserver::callbackDatabaseConfigured
        , &LogObserver::callbackServiceConnected
        , &LogObserver::callbackObserverStarted
        , &LogObserver::callbackLogDbCreated
        , &LogObserver::callbackMessagingFailed
        , &LogObserver::callbackConnectedInstances
        , &LogObserver::callbackDisconnecteInstances
        , &LogObserver::callbackLogScopes
        , &LogObserver::callbackLogUpdateScopes
        , nullptr       // set nullptr to receive messages via `callbackLogMessageEx` callback
        , &LogObserver::callbackLogMessageEx
    };

    Application::set_working_directory(nullptr);
    String fileConfig(NEApplication::DEFAULT_CONFIG_FILE);
    OptionParser parser(LogObserver::ValidOptions, std::size(LogObserver::ValidOptions));
    if (parser.parseCommandLine(argv, static_cast<uint32_t>(argc)))
    {
        uint32_t pos = parser.findOption(static_cast<int32_t>(LogObserver::LoggerOption::CMD_LogLoad));
        if (pos != NECommon::INVALID_POSITION)
        {
            String filePath{ parser.getOptions().at(pos).inString[0] };
            if (File::has_file(filePath))
            {
                fileConfig = filePath;
            }
        }
    }

    ::logObserverInitialize(&evts, fileConfig.as_string());

    _runConsoleInputExtended();

    Application::signal_app_quit();
    ::logObserverDisconnectLogger();
    ::logObserverRelease();
}

bool LogObserver::_checkCommand(const String& cmd)
{
    OptionParser parser( LogObserver::ValidOptions, std::size(LogObserver::ValidOptions) );
    bool quit{ false };
    bool hasError {false};

    LogObserver::_cleanHelp();
    Console& console = Console::instance();

    if ( parser.parseOptionLine( cmd ) )
    {
        const OptionParser::InputOptionList & opts = parser.getOptions( );
        for ( uint32_t i = 0; i < opts.size( ); ++ i )
        {
            bool processed{ false };
            const LogObserver::ObserverStatus* status{ nullptr };
            const OptionParser::InputOption & opt = opts[ i ];
            switch ( static_cast<LogObserver::LoggerOption>(opt.inCommand) )
            {
            case LogObserver::LoggerOption::CMD_LogQueryScopes:
                processed = LogObserver::_processQueryScopes(opt);
                status = &_observerStatus[static_cast<uint32_t>(LoggerOption::CMD_LogQueryScopes)];
                break;

            case LogObserver::LoggerOption::CMD_LogSaveConfig:
                processed = LogObserver::_processSaveConfig(opt);
                status = &_observerStatus[static_cast<uint32_t>(LoggerOption::CMD_LogSaveConfig)];
                break;

            case LogObserver::LoggerOption::CMD_LogPrintHelp:
                processed = LogObserver::_processPrintHelp();
                status = &_observerStatus[static_cast<uint32_t>(LoggerOption::CMD_LogPrintHelp)];
                break;

            case LogObserver::LoggerOption::CMD_LogInstances:
                processed = LogObserver::_processInfoInstances();
                status = &_observerStatus[static_cast<uint32_t>(LoggerOption::CMD_LogInstances)];
                break;

            case LogObserver::LoggerOption::CMD_LogUpdateScope:
                processed = LogObserver::_processUpdateScopes(opt);
                status = &_observerStatus[static_cast<uint32_t>(LoggerOption::CMD_LogUpdateScope)];
                break;

            case LogObserver::LoggerOption::CMD_LogPause:
                processed = LogObserver::_processPauseLogging();
                status = &_observerStatus[static_cast<uint32_t>(LoggerOption::CMD_LogPause)];
                break;

            case LogObserver::LoggerOption::CMD_LogQuit:
                quit = true;
                break;

            case LogObserver::LoggerOption::CMD_LogRestart:
                processed = LogObserver::_processStartLogging(true);
                status = &_observerStatus[static_cast<uint32_t>(LoggerOption::CMD_LogRestart)];
                break;

            case LogObserver::LoggerOption::CMD_LogStop:
                processed = LogObserver::_processStartLogging(false);
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
                console.lockConsole();
                if (processed && (status->osStatus.empty() == false))
                {
                    console.clearLine(NESystemService::COORD_STATUS_MSG);
                    console.outputTxt(NESystemService::COORD_STATUS_MSG, status->osStatus);
                }
                else if ((processed == false) && (status->osError.empty() == false))
                {
                    console.clearLine(NESystemService::COORD_STATUS_MSG);
                    console.outputTxt(NESystemService::COORD_STATUS_MSG, status->osError);
                }
                else
                {
                    console.clearLine(NESystemService::COORD_STATUS_MSG);
                }

                console.unlockConsole();
            }
        }
    }
    else
    {
        hasError = true;
    }
    
    console.lockConsole();
    if ( quit == false )
    {
        if ( hasError )
        {
            console.outputMsg( NESystemService::COORD_ERROR_MSG, NESystemService::FORMAT_MSG_ERROR.data(), cmd.as_string());
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

    return quit;
}

void LogObserver::_outputTitle()
{
    Console & console = Console::instance( );
    console.lockConsole();
    console.outputTxt( NESystemService::COORD_TITLE, LogObserver::APP_TITLE );
    console.outputTxt( NESystemService::COORD_SUBTITLE, NESystemService::MSG_SEPARATOR );
    console.unlockConsole();
}

void LogObserver::_outputInfo( const String & info )
{
    Console & console = Console::instance( );
    Console::Coord coord{NESystemService::COORD_INFO_MSG};
    console.lockConsole( );

    console.outputTxt( coord, NESystemService::MSG_SEPARATOR );
    ++ coord.posY;
    console.outputStr( coord, info );

    console.unlockConsole( );
}

void LogObserver::_cleanHelp()
{
    Console::Coord line{ NESystemService::COORD_INFO_MSG };
    Console& console = Console::instance();
    console.lockConsole();

    console.clearLine(NESystemService::COORD_USER_INPUT);
    uint32_t count = std::size(_msgHelp);
    for (uint32_t i = 0; i < count; ++ i)
    {
        console.clearLine(line);
        ++line.posY;
    }

    console.unlockConsole();
}

bool LogObserver::_processSaveConfig(const OptionParser::InputOption& optSave)
{
    ArrayList<ITEM_ID> listTargets;
    if (optSave.inString.empty() || (optSave.inString[0] == NEPersistence::SYNTAX_ALL_MODULES))
    {
        listTargets.add(NEService::TARGET_ALL);
    }
    else
    {
        for (const auto& elem : optSave.inString)
        {
            if (elem == NEPersistence::SYNTAX_ALL_MODULES)
            {
                listTargets.clear();
                listTargets.add(NEService::TARGET_ALL);
                break;
            }
            else if (elem.is_numeric())
            {
                listTargets.add(elem.to_uint64());
            }
        }
    }

    bool result{ true };
    for (const auto& target : listTargets.data())
    {
        result &= ::logObserverRequestSaveConfig(target);
    }

    return result;
}

bool LogObserver::_processPrintHelp()
{
    Console::Coord line{ NESystemService::COORD_INFO_MSG };
    Console& console = Console::instance();
    console.lockConsole();
    for (const auto& text : _msgHelp)
    {
        console.outputTxt(line, text);
        ++line.posY;
    }

    console.unlockConsole();
    return true;
}

bool LogObserver::_processInfoInstances()
{
    static constexpr std::string_view _table{ "   Nr. |  Inst. ID  |  Bits |  Scopes  |  name " };
    static constexpr std::string_view _formt{ "  %3u. |%11u |  x%u  |   %5u  |  %s " };
    static constexpr std::string_view _empty{ "There are no connected instances ..." };

    Console& console = Console::instance();
    Console::Coord coord{ NESystemService::COORD_INFO_MSG };
    console.lockConsole();

    if (_listInstances.is_empty())
    {
        console.outputTxt(coord, NESystemService::MSG_SEPARATOR);
        ++coord.posY;
        console.outputStr(coord, _empty);
        ++coord.posY;
    }
    else
    {
        console.outputTxt(coord, NESystemService::MSG_SEPARATOR);
        ++coord.posY;
        console.outputTxt(coord, _table);
        ++coord.posY;
        console.outputTxt(coord, NESystemService::MSG_SEPARATOR);
        ++coord.posY;
        for (uint32_t i = 0; i < _listInstances.size(); ++ i)
        {
            const LogInstance& instance{ _listInstances[i] };
            uint32_t id{ static_cast<uint32_t>(instance.liCookie) };
            auto pos = _mapScopes.find(instance.liCookie);
            uint32_t scopes{ pos != _mapScopes.invalid_position() ? _mapScopes.value_at_position(pos).size() : 0u };
            console.outputMsg(coord, _formt.data(), (i + 1), id, static_cast<uint32_t>(instance.liBitness), scopes, instance.liName);
            ++coord.posY;
        }
    }

    console.outputTxt(coord, NESystemService::MSG_SEPARATOR);
    console.unlockConsole();

    return true;
}

bool LogObserver::_processUpdateScopes(const OptionParser::InputOption& optScope)
{
    bool result{ false };
    ASSERT(optScope.inCommand == static_cast<int32_t>(LoggerOption::CMD_LogUpdateScope));
    ASSERT(optScope.inString.empty() == false);

    const OptionParser::StrList& optValues{ optScope.inString };
    String scope;
    for (const auto& entry : optValues)
    {
        if (entry == NEPersistence::SYNTAX_END_COMMAND)
        {
            LogObserver::_sendScopeUpdateMessage(scope);
            scope.clear();
        }
        else
        {
            scope += entry;
        }
    }

    if (scope.is_empty() == false)
    {
        result = LogObserver::_sendScopeUpdateMessage(scope);
    }

    return result;
}

bool LogObserver::_processPauseLogging()
{
    return ::logObserverPauseLogging(true);
}

bool LogObserver::_processStartLogging(bool doStart)
{
    bool result{ true };
    if (doStart)
    {
        if (::logObserverIsInitialized())
        {
            if (::logObserverIsConnected() == false)
            {
                result = ::logObserverConnectLogger(nullptr, nullptr, NESocket::InvalidPort);
            }
            else if (::logObserverIsStarted() == false)
            {
                result = ::logObserverPauseLogging(false);
            }
        }
        else
        {
            result = false;
        }
    }
    else
    {
        ::logObserverDisconnectLogger();
    }

    return result;
}

bool LogObserver::_processQueryScopes(const OptionParser::InputOption& optScope)
{
    bool result{ true };
    ArrayList<ITEM_ID> listTargets;
    if (optScope.inString.empty() || (optScope.inString[0] == NEPersistence::SYNTAX_ALL_MODULES))
    {
        listTargets.add(NEService::TARGET_ALL);
    }
    else
    {
        for (const auto& elem : optScope.inString)
        {
            if (elem == NEPersistence::SYNTAX_ALL_MODULES)
            {
                listTargets.clear();
                listTargets.add(NEService::TARGET_ALL);
                break;
            }
            else if (elem.is_numeric())
            {
                listTargets.add(elem.to_uint64());
            }
        }
    }

    for (const auto& target : listTargets.data())
    {
        result &= ::logObserverRequestScopes(target);
    }

    return result;
}

String LogObserver::_normalizeScopeProperty(const String & scope)
{
    const NEPersistence::ConfigKey& propKey{ NEPersistence::DefaultPropertyKeys[static_cast<uint32_t>(NEPersistence::ConfigEntry::LogScope)] };
    String result;
    if (scope.starts_with(propKey.property))
    {
        result.append(propKey.section)
              .append(NEPersistence::SYNTAX_OBJECT_SEPARATOR)
              .append(NEPersistence::SYNTAX_ALL_MODULES)
              .append(NEPersistence::SYNTAX_OBJECT_SEPARATOR)
              .append(scope);
    }
    else
    {
        String prop(propKey.property);
        prop += NEPersistence::SYNTAX_OBJECT_SEPARATOR;
        NEString::CharPos pos = scope.find_first(prop);
        if ( scope.is_valid_position(pos))
        {
            result.append(propKey.section)
                  .append(NEPersistence::SYNTAX_OBJECT_SEPARATOR)
                  .append(scope);
        }
        else
        {
            result = scope;
            pos = result.find_last(NEPersistence::SYNTAX_OBJECT_SEPARATOR);
            if (result.is_valid_position(pos))
            {
                result.insert_at(prop, pos + static_cast<NEString::CharCount>(NEPersistence::SYNTAX_OBJECT_SEPARATOR.length()));
            }
            else
            {
                result.insert_at(prop, NEString::START_POS);
            }

            result = _normalizeScopeProperty(result);
        }
    }

    return result;
}

bool LogObserver::_sendScopeUpdateMessage(const String& scope)
{
    bool result{ false };

    if (scope.is_empty() == false)
    {
        Property prop(LogObserver::_normalizeScopeProperty(scope));
        if (prop.is_valid() && prop.type() == NEPersistence::ConfigEntry::LogScope)
        {
            const PropertyKey& key{ prop.key() };
            ITEM_ID target{ key.is_all_modules() ? NEService::TARGET_ALL : key.module().to_uint32() };
            if (target >= NEService::TARGET_ALL)
            {
                String scopeName{ key.position() };
                uint32_t scopePrio{ prop.value().identifier(NEApplication::LogScopePriorityIndentifiers) };
                ScopeInfo logScope;
                logScope.lsId   = NELogging::make_scope_id_ex(scopeName.as_string());
                logScope.lsPrio = scopePrio;
                NEString::copy_string<char>(logScope.lsName, LENGTH_SCOPE, scopeName.as_string(), scopeName.length());
                result = ::logObserverRequestChangeScopePrio(target, &logScope, 1);
            }
        }
    }

    return result;
}

inline void LogObserver::enableLocalLogs(ConfigManager& config, bool /* enable */)
{
    constexpr NEPersistence::ConfigEntry prioConfKey{ NEPersistence::ConfigEntry::LogScope };
    const NEPersistence::ConfigKey& keyPrio{ NEPersistence::log_scope() };
    uint32_t prios = static_cast<uint32_t>(NELogging::LogPriority::PrioNotset);
    const String prio{ NELogging::make_prio_string(prios) };

    config.set_module_property(keyPrio.section, keyPrio.property, String(NEPersistence::SYNTAX_ANY_VALUE), prio, prioConfKey, true);
    config.set_log_enabled(NELogging::LogTarget::Remote, false, true);
}
