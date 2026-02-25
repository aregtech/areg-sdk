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
        , aregext::MSG_SEPARATOR
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
        , aregext::MSG_SEPARATOR
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

LogObserver & LogObserver::getInstance()
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
    Console & console = Console::getInstance( );
    LogObserver::_outputTitle( );

    console.enableConsoleInput(true);
    console.outputTxt(aregext::COORD_STATUS_MSG, LogObserver::STATUS_INITIALIZED);
    console.outputTxt(aregext::COORD_USER_INPUT, aregext::FORMAT_WAIT_QUIT);
    console.waitForInput(getOptionCheckCallback());

    console.moveCursorOneLineDown( );
    console.clearScreen( );
    console.uninitialize( );
}

void LogObserver::callbackObserverConfigured(bool /* isEnabled */, const char* /* address */, uint16_t /* port */)
{
}

void LogObserver::callbackDatabaseConfigured(bool /* isEnabled */, const char* /* dbName */, const char* /* dbLocation */, const char* /* user */ )
{
}

void LogObserver::callbackServiceConnected(bool isConnected, const char* address, uint16_t port)
{
    if (isConnected)
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

void LogObserver::callbackObserverStarted(bool /* isStarted */)
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
        for (uint32_t j = 0; j < _listInstances.getSize(); ++j)
        {
            if (_listInstances[j].liCookie == inst.liCookie)
            {
                contains = true;
                break;
            }
        }

        if (contains == false)
        {
            TIME64 now{ areg::DateTime::getNow() };
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
            log.logMessageLen   = static_cast<uint32_t>(areg::String::formatString(log.logMessage, areg::LOG_MESSAGE_IZE, "CONNECTED the x%u instance %s with cookie %llu", inst.liBitness, inst.liName, inst.liCookie));
            log.logThreadLen    = 0;
            log.logThread[0]    = areg::String::EmptyChar;
            log.logModuleId     = 0;
            log.logModuleLen    = static_cast<uint32_t>(areg::copyString(log.logModule, areg::LOG_NAMES_SIZE, inst.liName));

            _listInstances.add(inst);
            areg::logAnyMessage(log);

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
        for (uint32_t j = 0; j < _listInstances.getSize(); ++j)
        {
            const LogInstance& inst{ _listInstances[j] };
            if (inst.liCookie == cookie)
            {
                TIME64 now{ areg::DateTime::getNow() };
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
                log.logMessageLen   = static_cast<uint32_t>(areg::String::formatString(log.logMessage, areg::LOG_MESSAGE_IZE, "DISCONNECTED the x%u instance %s with cookie %llu", inst.liBitness, inst.liName, inst.liCookie));
                log.logThreadLen    = 0;
                log.logThread[0]    = areg::String::EmptyChar;
                log.logModuleId     = 0;
                log.logModuleLen    = static_cast<uint32_t>(areg::copyString(log.logModule, areg::LOG_NAMES_SIZE, inst.liName));

                _listInstances.removeAt(j, 1);
                _mapScopes.removeAt(cookie);

                areg::logAnyMessage(log);
                break;
            }
        }
    }
}

void LogObserver::callbackLogScopes(ITEM_ID cookie, const ScopeInfo* scopes, uint32_t count)
{
    for (uint32_t i = 0; i < _listInstances.getSize(); ++i)
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
            log.logTimestamp    = static_cast<TIME64>(areg::DateTime::getNow());
            log.logReceived     = log.logTimestamp;
            log.logDuration     = 0u;
            log.logScopeId      = 0u;
            log.logSessionId    = 0u;
            log.logMessageLen   = static_cast<uint32_t>(areg::String::formatString(log.logMessage, areg::LOG_MESSAGE_IZE, "Registered %u scopes for instance %s with cookie %llu", count, inst.liName, inst.liCookie));
            log.logThreadLen    = 0;
            log.logThread[0]    = areg::String::EmptyChar;
            log.logModuleId     = 0;
            log.logModuleLen    = static_cast<uint32_t>(areg::copyString(log.logModule, areg::LOG_NAMES_SIZE, inst.liName));

            _mapScopes.setAt(cookie, ListScopes());
            ListScopes& scopeList{ _mapScopes.getAt(cookie) };
            scopeList.resize(count);
            for (uint32_t j = 0; j < count; ++j)
            {
                scopeList[j] = scopes[j];
            }

            areg::logAnyMessage(log);
            break;
        }
    }
}

void LogObserver::callbackLogUpdateScopes(ITEM_ID /* cookie */, const ScopeInfo* /* scopes */, uint32_t /* count */)
{
}

void LogObserver::callbackLogMessage(const LogEntry* /* logMessage */)
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
        ASSERT(size >= sizeof(areg::LogEntry));
        const areg::LogEntry & log{ reinterpret_cast<const areg::LogEntry &>(*logBuffer)};
        areg::logAnyMessage(log);
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

    Application::setWorkingDirectory(nullptr);
    areg::String fileConfig(areg::DEFAULT_CONFIG_FILE);
    OptionParser parser(LogObserver::ValidOptions, std::size(LogObserver::ValidOptions));
    if (parser.parseCommandLine(argv, static_cast<uint32_t>(argc)))
    {
        uint32_t pos = parser.findOption(static_cast<int32_t>(LogObserver::LoggerOption::CMD_LogLoad));
        if (pos != areg::INVALID_POSITION)
        {
            areg::String filePath{ parser.getOptions().getAt(pos).inString[0] };
            if (areg::File::existFile(filePath))
            {
                fileConfig = filePath;
            }
        }
    }

    ::logObserverInitialize(&evts, fileConfig.getString());

    _runConsoleInputExtended();

    Application::signalAppQuit();
    ::logObserverDisconnectLogger();
    ::logObserverRelease();
}

bool LogObserver::_checkCommand(const areg::String& cmd)
{
    OptionParser parser( LogObserver::ValidOptions, std::size(LogObserver::ValidOptions) );
    bool quit{ false };
    bool hasError {false};

    LogObserver::_cleanHelp();
    Console& console = Console::getInstance();

    if ( parser.parseOptionLine( cmd ) )
    {
        const OptionParser::InputOptionList & opts = parser.getOptions( );
        for ( uint32_t i = 0; i < opts.getSize( ); ++ i )
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
                    console.clearLine(aregext::COORD_STATUS_MSG);
                    console.outputTxt(aregext::COORD_STATUS_MSG, status->osStatus);
                }
                else if ((processed == false) && (status->osError.empty() == false))
                {
                    console.clearLine(aregext::COORD_STATUS_MSG);
                    console.outputTxt(aregext::COORD_STATUS_MSG, status->osError);
                }
                else
                {
                    console.clearLine(aregext::COORD_STATUS_MSG);
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
            console.outputMsg( aregext::COORD_ERROR_MSG, aregext::FORMAT_MSG_ERROR.data(), cmd.getString());
        }

        console.clearLine( aregext::COORD_USER_INPUT );
        console.outputTxt( aregext::COORD_USER_INPUT, aregext::FORMAT_WAIT_QUIT );
    }
    else
    {
        console.outputTxt( aregext::COORD_INFO_MSG, aregext::FORMAT_QUIT_APP );
    }

    console.refreshScreen( );
    console.unlockConsole( );

    return quit;
}

void LogObserver::_outputTitle()
{
    Console & console = Console::getInstance( );
    console.lockConsole();
    console.outputTxt( aregext::COORD_TITLE, LogObserver::APP_TITLE );
    console.outputTxt( aregext::COORD_SUBTITLE, aregext::MSG_SEPARATOR );
    console.unlockConsole();
}

void LogObserver::_outputInfo( const areg::String & info )
{
    Console & console = Console::getInstance( );
    Console::Coord coord{aregext::COORD_INFO_MSG};
    console.lockConsole( );

    console.outputTxt( coord, aregext::MSG_SEPARATOR );
    ++ coord.posY;
    console.outputStr( coord, info );

    console.unlockConsole( );
}

void LogObserver::_cleanHelp()
{
    Console::Coord line{ aregext::COORD_INFO_MSG };
    Console& console = Console::getInstance();
    console.lockConsole();

    console.clearLine(aregext::COORD_USER_INPUT);
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
            else if (elem.isNumeric())
            {
                listTargets.add(elem.toUInt64());
            }
        }
    }

    bool result{ true };
    for (const auto& target : listTargets.getData())
    {
        result &= ::logObserverRequestSaveConfig(target);
    }

    return result;
}

bool LogObserver::_processPrintHelp()
{
    Console::Coord line{ aregext::COORD_INFO_MSG };
    Console& console = Console::getInstance();
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
    static constexpr std::string_view _table{ "   Nr. |  Inst. ID  |  Bits |  Scopes  |  Name " };
    static constexpr std::string_view _formt{ "  %3u. |%11u |  x%u  |   %5u  |  %s " };
    static constexpr std::string_view _empty{ "There are no connected instances ..." };

    Console& console = Console::getInstance();
    Console::Coord coord{ aregext::COORD_INFO_MSG };
    console.lockConsole();

    if (_listInstances.isEmpty())
    {
        console.outputTxt(coord, aregext::MSG_SEPARATOR);
        ++coord.posY;
        console.outputStr(coord, _empty);
        ++coord.posY;
    }
    else
    {
        console.outputTxt(coord, aregext::MSG_SEPARATOR);
        ++coord.posY;
        console.outputTxt(coord, _table);
        ++coord.posY;
        console.outputTxt(coord, aregext::MSG_SEPARATOR);
        ++coord.posY;
        for (uint32_t i = 0; i < _listInstances.getSize(); ++ i)
        {
            const LogInstance& instance{ _listInstances[i] };
            uint32_t id{ static_cast<uint32_t>(instance.liCookie) };
            auto pos = _mapScopes.find(instance.liCookie);
            uint32_t scopes{ pos != _mapScopes.invalidPosition() ? _mapScopes.valueAtPosition(pos).getSize() : 0u };
            console.outputMsg(coord, _formt.data(), (i + 1), id, static_cast<uint32_t>(instance.liBitness), scopes, instance.liName);
            ++coord.posY;
        }
    }

    console.outputTxt(coord, aregext::MSG_SEPARATOR);
    console.unlockConsole();

    return true;
}

bool LogObserver::_processUpdateScopes(const OptionParser::InputOption& optScope)
{
    bool result{ false };
    ASSERT(optScope.inCommand == static_cast<int32_t>(LoggerOption::CMD_LogUpdateScope));
    ASSERT(optScope.inString.empty() == false);

    const OptionParser::StrList& optValues{ optScope.inString };
    areg::String scope;
    for (const auto& entry : optValues)
    {
        if (entry == areg::SYNTAX_END_COMMAND)
        {
            LogObserver::_sendScopeUpdateMessage(scope);
            scope.clear();
        }
        else
        {
            scope += entry;
        }
    }

    if (scope.isEmpty() == false)
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
                result = ::logObserverConnectLogger(nullptr, nullptr, areg::InvalidPort);
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
            else if (elem.isNumeric())
            {
                listTargets.add(elem.toUInt64());
            }
        }
    }

    for (const auto& target : listTargets.getData())
    {
        result &= ::logObserverRequestScopes(target);
    }

    return result;
}

areg::String LogObserver::_normalizeScopeProperty(const areg::String & scope)
{
    const areg::ConfigKey& propKey{ areg::DefaultPropertyKeys[static_cast<uint32_t>(areg::ConfigEntry::LogScope)] };
    areg::String result;
    if (scope.startsWith(propKey.property))
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
        areg::CharPos pos = scope.findFirst(prop);
        if ( scope.isValidPosition(pos))
        {
            result.append(propKey.section)
                  .append(areg::SYNTAX_OBJECT_SEPARATOR)
                  .append(scope);
        }
        else
        {
            result = scope;
            pos = result.findLast(areg::SYNTAX_OBJECT_SEPARATOR);
            if (result.isValidPosition(pos))
            {
                result.insertAt(prop, pos + static_cast<areg::CharCount>(areg::SYNTAX_OBJECT_SEPARATOR.length()));
            }
            else
            {
                result.insertAt(prop, areg::START_POS);
            }

            result = _normalizeScopeProperty(result);
        }
    }

    return result;
}

bool LogObserver::_sendScopeUpdateMessage(const areg::String& scope)
{
    bool result{ false };

    if (scope.isEmpty() == false)
    {
        Property prop(LogObserver::_normalizeScopeProperty(scope));
        if (prop.isValid() && prop.getPropertyType() == areg::ConfigEntry::LogScope)
        {
            const PropertyKey& key{ prop.getKey() };
            ITEM_ID target{ key.isAllModules() ? areg::TARGET_ALL : key.getModule().toUInt32() };
            if (target >= areg::TARGET_ALL)
            {
                areg::String scopeName{ key.getPosition() };
                uint32_t scopePrio{ prop.getValue().getIndetifier(areg::LogScopePriorityIndentifiers) };
                ScopeInfo logScope;
                logScope.lsId   = areg::makeScopeIdEx(scopeName.getString());
                logScope.lsPrio = scopePrio;
                areg::copyString<char>(logScope.lsName, LENGTH_SCOPE, scopeName.getString(), scopeName.getLength());
                result = ::logObserverRequestChangeScopePrio(target, &logScope, 1);
            }
        }
    }

    return result;
}

inline void LogObserver::enableLocalLogs(ConfigManager& config, bool /* enable */)
{
    constexpr areg::ConfigEntry prioConfKey{ areg::ConfigEntry::LogScope };
    const areg::ConfigKey& keyPrio{ areg::getLogScope() };
    uint32_t prios = static_cast<uint32_t>(areg::LogPriority::PrioNotset);
    const areg::String prio{ areg::makePrioString(prios) };

    config.setModuleProperty(keyPrio.section, keyPrio.property, areg::String(areg::SYNTAX_ANY_VALUE), prio, prioConfKey, true);
    config.setLogEnabled(areg::LogTarget::Remote, false, true);
}
