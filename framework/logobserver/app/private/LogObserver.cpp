/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        logobserver/app/private/LogObserver.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, logger to run as a console application process or service.
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
#include "aregextend/service/NESystemService.hpp"
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
          {"Usage of AREG Log Observer console application :"}
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

    struct sLoggerConnect
    {
        String      lcAddress;
        uint16_t    lcPort{ NESocket::InvalidPort };
    };

    using ListInstances = TEArrayList<sLogInstance>;
    using ListScopes    = TEArrayList<sLogScope>;
    using MapScopes     = TEHashMap<ITEM_ID, ListScopes>;

    sLoggerConnect  _logConnect;
    ListInstances   _listInstances;
    MapScopes       _mapScopes;

}

//////////////////////////////////////////////////////////////////////////
// LogObserver class implementation
//////////////////////////////////////////////////////////////////////////

const OptionParser::sOptionSetup LogObserver::ValidOptions[ ]
{
      { "-e", "--query"     , static_cast<int>(eLoggerOptions::CMD_LogQueryScopes)  , OptionParser::STRING_NO_RANGE , {}, {}, {} }
    , { "-f", "--config"    , static_cast<int>(eLoggerOptions::CMD_LogSaveConfig)   , OptionParser::STRING_NO_RANGE , {}, {}, {} }
    , { "-h", "--help"      , static_cast<int>(eLoggerOptions::CMD_LogPrintHelp)    , OptionParser::NO_DATA         , {}, {}, {} }
    , { "-l", "--load"      , static_cast<int>(eLoggerOptions::CMD_LogLoad)         , OptionParser::STRING_NO_RANGE , {}, {}, {} }
    , { "-n", "--instances" , static_cast<int>(eLoggerOptions::CMD_LogInstances)    , OptionParser::NO_DATA         , {}, {}, {} }
    , { "-o", "--scope"     , static_cast<int>(eLoggerOptions::CMD_LogUpdateScope)  , OptionParser::STRING_NO_RANGE , {}, {}, {} }
    , { "-p", "--pause"     , static_cast<int>(eLoggerOptions::CMD_LogPause)        , OptionParser::NO_DATA         , {}, {}, {} }
    , { "-q", "--quit"      , static_cast<int>(eLoggerOptions::CMD_LogQuit)         , OptionParser::NO_DATA         , {}, {}, {} }
    , { "-r", "--restart"   , static_cast<int>(eLoggerOptions::CMD_LogRestart)      , OptionParser::NO_DATA         , {}, {}, {} }
    , { "-x", "--stop"      , static_cast<int>(eLoggerOptions::CMD_LogStop)         , OptionParser::NO_DATA         , {}, {}, {} }
};

LogObserver & LogObserver::getInstance(void)
{
    static LogObserver _instance;
    return _instance;
}

Console::CallBack LogObserver::getOptionCheckCallback( void ) const
{
    return Console::CallBack( LogObserver::_checkCommand );
}

void LogObserver::_runConsoleInputExtended( void )
{
    Console & console = Console::getInstance( );
    LogObserver::_outputTitle( );

    console.enableConsoleInput(true);
    console.outputTxt(NESystemService::COORD_STATUS_MSG, LogObserver::STATUS_INITIALIZED);
    console.outputTxt(NESystemService::COORD_USER_INPUT, NESystemService::FORMAT_WAIT_QUIT);
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
        _logConnect.lcPort = NESocket::InvalidPort;
    }
}

void LogObserver::callbackObserverStarted(bool /* isStarted */)
{
}

void LogObserver::callbackMessagingFailed(void)
{
}

void LogObserver::callbackConnectedInstances(const sLogInstance* instances, uint32_t count)
{
    if (count == 0)
    {
        _listInstances.clear();
        _mapScopes.clear();
        return;
    }

    for (uint32_t i = 0; i < count; ++i)
    {
        const sLogInstance& inst{ instances[i] };
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
            NETrace::sLogMessage log{ };
            log.logDataType = NETrace::eLogDataType::LogDataLocal;
            log.logMsgType = NETrace::eLogMessageType::LogMessageText;
            log.logMessagePrio = NETrace::eLogPriority::PrioAny;
            log.logSource = inst.liSource;
            log.logTarget = NEService::COOKIE_LOCAL;
            log.logCookie = inst.liCookie;
            log.logModuleId = 0u;
            log.logThreadId = 0u;
            log.logTimestamp = inst.liTimestamp;
            log.logScopeId = 0u;
            log.logMessageLen = static_cast<uint32_t>(String::formatString(log.logMessage, NETrace::LOG_MESSAGE_IZE, "CONNECTED the x%u instance %s with cookie %llu", inst.liBitness, inst.liName, inst.liCookie));
            log.logThreadLen = 0;
            log.logThread[0] = String::EmptyChar;
            log.logModuleId = 0;
            log.logModuleLen = static_cast<uint32_t>(NEString::copyString(log.logModule, NETrace::LOG_NAMES_SIZE, inst.liName));

            _listInstances.add(inst);
            NETrace::logAnyMessage(log);

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
            const sLogInstance& inst{ _listInstances[j] };
            if (inst.liCookie == cookie)
            {
                NETrace::sLogMessage log{ };
                log.logDataType     = NETrace::eLogDataType::LogDataLocal;
                log.logMsgType      = NETrace::eLogMessageType::LogMessageText;
                log.logMessagePrio  = NETrace::eLogPriority::PrioAny;
                log.logSource       = inst.liSource;
                log.logTarget       = NEService::COOKIE_LOCAL;
                log.logCookie       = inst.liCookie;
                log.logModuleId     = 0u;
                log.logThreadId     = 0u;
                log.logTimestamp    = static_cast<TIME64>(DateTime::getNow());
                log.logScopeId      = 0u;
                log.logMessageLen   = static_cast<uint32_t>(String::formatString(log.logMessage, NETrace::LOG_MESSAGE_IZE, "DISCONNECTED the x%u instance %s with cookie %llu", inst.liBitness, inst.liName, inst.liCookie));
                log.logThreadLen    = 0;
                log.logThread[0]    = String::EmptyChar;
                log.logModuleId     = 0;
                log.logModuleLen    = static_cast<uint32_t>(NEString::copyString(log.logModule, NETrace::LOG_NAMES_SIZE, inst.liName));

                _listInstances.removeAt(j, 1);
                _mapScopes.removeAt(cookie);

                NETrace::logAnyMessage(log);
                break;
            }
        }
    }
}

void LogObserver::callbackLogScopes(ITEM_ID cookie, const sLogScope* scopes, uint32_t count)
{
    for (uint32_t i = 0; i < _listInstances.getSize(); ++i)
    {
        const sLogInstance& inst{ _listInstances[i] };
        if (cookie == inst.liCookie)
        {
            NETrace::sLogMessage log{ };
            log.logDataType     = NETrace::eLogDataType::LogDataLocal;
            log.logMsgType      = NETrace::eLogMessageType::LogMessageText;
            log.logMessagePrio  = NETrace::eLogPriority::PrioAny;
            log.logSource       = inst.liSource;
            log.logTarget       = NEService::COOKIE_LOCAL;
            log.logCookie       = inst.liCookie;
            log.logModuleId     = 0u;
            log.logThreadId     = 0u;
            log.logTimestamp    = static_cast<TIME64>(DateTime::getNow());
            log.logScopeId      = 0u;
            log.logMessageLen   = static_cast<uint32_t>(String::formatString(log.logMessage, NETrace::LOG_MESSAGE_IZE, "Registered %u scopes for instance %s with cookie %llu", count, inst.liName, inst.liCookie));
            log.logThreadLen    = 0;
            log.logThread[0]    = String::EmptyChar;
            log.logModuleId     = 0;
            log.logModuleLen    = static_cast<uint32_t>(NEString::copyString(log.logModule, NETrace::LOG_NAMES_SIZE, inst.liName));

            _mapScopes.setAt(cookie, ListScopes());
            ListScopes& scopeList{ _mapScopes.getAt(cookie) };
            scopeList.resize(count);
            for (uint32_t j = 0; j < count; ++j)
            {
                scopeList[j] = scopes[j];
            }

            NETrace::logAnyMessage(log);
            break;
        }
    }
}

void LogObserver::callbackLogMessageEx(const unsigned char* logBuffer, uint32_t size)
{
    if (logBuffer != nullptr)
    {
        ASSERT(size >= sizeof(NETrace::sLogMessage));
        const NETrace::sLogMessage & log{ reinterpret_cast<const NETrace::sLogMessage &>(*logBuffer)};
        NETrace::logAnyMessage(log);
    }
}

void LogObserver::logMain( int argc, char ** argv )
{
    sObserverEvents evts
    {
          &LogObserver::callbackObserverConfigured
        , &LogObserver::callbackDatabaseConfigured
        , &LogObserver::callbackServiceConnected
        , &LogObserver::callbackObserverStarted
        , &LogObserver::callbackMessagingFailed
        , &LogObserver::callbackConnectedInstances
        , &LogObserver::callbackDisconnecteInstances
        , &LogObserver::callbackLogScopes
        , nullptr
        , nullptr
        , &LogObserver::callbackLogMessageEx
    };

    Application::setWorkingDirectory(nullptr);
    String fileConfig(NEApplication::DEFAULT_CONFIG_FILE);
    OptionParser parser(LogObserver::ValidOptions, MACRO_ARRAYLEN(LogObserver::ValidOptions));
    if (parser.parseCommandLine(argv, static_cast<uint32_t>(argc)))
    {
        uint32_t pos = parser.findOption(static_cast<int32_t>(LogObserver::eLoggerOptions::CMD_LogLoad));
        if (pos != NECommon::INVALID_POSITION)
        {
            String filePath{ parser.getOptions().getAt(pos).inString[0] };
            if (File::existFile(filePath))
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

bool LogObserver::_checkCommand(const String& cmd)
{
    OptionParser parser( LogObserver::ValidOptions, MACRO_ARRAYLEN(LogObserver::ValidOptions) );
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
            const LogObserver::sObserverStatus* status{ nullptr };
            const OptionParser::sOption & opt = opts[ i ];
            switch ( static_cast<LogObserver::eLoggerOptions>(opt.inCommand) )
            {
            case LogObserver::eLoggerOptions::CMD_LogQueryScopes:
                processed = LogObserver::_processQueryScopes(opt);
                status = &ObserverStatus[static_cast<uint32_t>(eLoggerOptions::CMD_LogQueryScopes)];
                break;

            case LogObserver::eLoggerOptions::CMD_LogSaveConfig:
                processed = LogObserver::_processSaveConfig(opt);
                status = &ObserverStatus[static_cast<uint32_t>(eLoggerOptions::CMD_LogSaveConfig)];
                break;

            case LogObserver::eLoggerOptions::CMD_LogPrintHelp:
                processed = LogObserver::_processPrintHelp();
                status = &ObserverStatus[static_cast<uint32_t>(eLoggerOptions::CMD_LogPrintHelp)];
                break;

            case LogObserver::eLoggerOptions::CMD_LogInstances:
                processed = LogObserver::_processInfoInstances();
                status = &ObserverStatus[static_cast<uint32_t>(eLoggerOptions::CMD_LogInstances)];
                break;

            case LogObserver::eLoggerOptions::CMD_LogUpdateScope:
                processed = LogObserver::_processUpdateScopes(opt);
                status = &ObserverStatus[static_cast<uint32_t>(eLoggerOptions::CMD_LogUpdateScope)];
                break;

            case LogObserver::eLoggerOptions::CMD_LogPause:
                processed = LogObserver::_processPauseLogging();
                status = &ObserverStatus[static_cast<uint32_t>(eLoggerOptions::CMD_LogPause)];
                break;

            case LogObserver::eLoggerOptions::CMD_LogQuit:
                quit = true;
                break;

            case LogObserver::eLoggerOptions::CMD_LogRestart:
                processed = LogObserver::_processStartLogging(true);
                status = &ObserverStatus[static_cast<uint32_t>(eLoggerOptions::CMD_LogRestart)];
                break;

            case LogObserver::eLoggerOptions::CMD_LogStop:
                processed = LogObserver::_processStartLogging(false);
                status = &ObserverStatus[static_cast<uint32_t>(eLoggerOptions::CMD_LogStop)];
                break;

            case LogObserver::eLoggerOptions::CMD_LogLoad:      // fall through
            case LogObserver::eLoggerOptions::CMD_LogUndefined: // fall through
            default:
                hasError = true;
                break;
            }

            if (status != nullptr)
            {
                ASSERT(static_cast<eLoggerOptions>(opt.inCommand) == status->osOption);
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
            console.outputMsg( NESystemService::COORD_ERROR_MSG, NESystemService::FORMAT_MSG_ERROR.data(), cmd.getString());
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

void LogObserver::_outputTitle( void )
{
    Console & console = Console::getInstance( );
    console.lockConsole();
    console.outputTxt( NESystemService::COORD_TITLE, LogObserver::APP_TITLE );
    console.outputTxt( NESystemService::COORD_SUBTITLE, NESystemService::MSG_SEPARATOR );
    console.unlockConsole();
}

void LogObserver::_outputInfo( const String & info )
{
    Console & console = Console::getInstance( );
    Console::Coord coord{NESystemService::COORD_INFO_MSG};
    console.lockConsole( );

    console.outputTxt( coord, NESystemService::MSG_SEPARATOR );
    ++ coord.posY;
    console.outputStr( coord, info );

    console.unlockConsole( );
}

void LogObserver::_cleanHelp(void)
{
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
}

bool LogObserver::_processSaveConfig(const OptionParser::sOption& optSave)
{
    TEArrayList<ITEM_ID> listTargets;
    if (optSave.inString.empty() || (optSave.inString[0] == NEPersistence::SYNTAX_ALL_MODULES))
    {
        listTargets.add(NEService::COOKIE_ANY);
    }
    else
    {
        for (const auto& elem : optSave.inString)
        {
            if (elem == NEPersistence::SYNTAX_ALL_MODULES)
            {
                listTargets.clear();
                listTargets.add(NEService::COOKIE_ANY);
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

bool LogObserver::_processPrintHelp(void)
{
    Console::Coord line{ NESystemService::COORD_INFO_MSG };
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

bool LogObserver::_processInfoInstances(void)
{
    static constexpr std::string_view _table{ "   Nr. |  Inst. ID  |  Bits |  Scopes  |  Name " };
    static constexpr std::string_view _formt{ "  %3u. |%11u |  x%u  |   %5u  |  %s " };
    static constexpr std::string_view _empty{ "There are no connected instances ..." };

    Console& console = Console::getInstance();
    Console::Coord coord{ NESystemService::COORD_INFO_MSG };
    console.lockConsole();

    if (_listInstances.isEmpty())
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
        for (uint32_t i = 0; i < _listInstances.getSize(); ++ i)
        {
            const sLogInstance& instance{ _listInstances[i] };
            uint32_t id{ static_cast<uint32_t>(instance.liCookie) };
            auto pos = _mapScopes.find(instance.liCookie);
            uint32_t scopes{ pos != _mapScopes.invalidPosition() ? _mapScopes.valueAtPosition(pos).getSize() : 0u };
            console.outputMsg(coord, _formt.data(), (i + 1), id, static_cast<uint32_t>(instance.liBitness), scopes, instance.liName);
            ++coord.posY;
        }
    }

    console.outputTxt(coord, NESystemService::MSG_SEPARATOR);
    console.unlockConsole();

    return true;
}

bool LogObserver::_processUpdateScopes(const OptionParser::sOption& optScope)
{
    bool result{ false };
    ASSERT(optScope.inCommand == static_cast<int>(eLoggerOptions::CMD_LogUpdateScope));
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

    if (scope.isEmpty() == false)
    {
        result = LogObserver::_sendScopeUpdateMessage(scope);
    }

    return result;
}

bool LogObserver::_processPauseLogging(void)
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

bool LogObserver::_processQueryScopes(const OptionParser::sOption& optScope)
{
    bool result{ true };
    TEArrayList<ITEM_ID> listTargets;
    if (optScope.inString.empty() || (optScope.inString[0] == NEPersistence::SYNTAX_ALL_MODULES))
    {
        listTargets.add(NEService::COOKIE_ANY);
    }
    else
    {
        for (const auto& elem : optScope.inString)
        {
            if (elem == NEPersistence::SYNTAX_ALL_MODULES)
            {
                listTargets.clear();
                listTargets.add(NEService::COOKIE_ANY);
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

String LogObserver::_normalizeScopeProperty(const String & scope)
{
    const NEPersistence::sPropertyKey& propKey{ NEPersistence::DefaultPropertyKeys[static_cast<uint32_t>(NEPersistence::eConfigKeys::EntryLogScope)] };
    String result;
    if (scope.startsWith(propKey.property))
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
        NEString::CharPos pos = scope.findFirst(prop);
        if ( scope.isValidPosition(pos))
        {
            result.append(propKey.section)
                  .append(NEPersistence::SYNTAX_OBJECT_SEPARATOR)
                  .append(scope);
        }
        else
        {
            result = scope;
            pos = result.findLast(NEPersistence::SYNTAX_OBJECT_SEPARATOR);
            if (result.isValidPosition(pos))
            {
                result.insertAt(prop, pos + static_cast<NEString::CharCount>(NEPersistence::SYNTAX_OBJECT_SEPARATOR.length()));
            }
            else
            {
                result.insertAt(prop, NEString::START_POS);
            }

            result = _normalizeScopeProperty(result);
        }
    }

    return result;
}

bool LogObserver::_sendScopeUpdateMessage(const String& scope)
{
    bool result{ false };

    if (scope.isEmpty() == false)
    {
        Property prop(LogObserver::_normalizeScopeProperty(scope));
        if (prop.isValid() && prop.getPropertyType() == NEPersistence::eConfigKeys::EntryLogScope)
        {
            const PropertyKey& key{ prop.getKey() };
            ITEM_ID target{ key.isAllModules() ? NEService::COOKIE_ANY : key.getModule().toUInt32() };
            if (target >= NEService::COOKIE_ANY)
            {
                String scopeName{ key.getPosition() };
                uint32_t scopePrio{ prop.getValue().getIndetifier(NEApplication::LogScopePriorityIndentifiers) };
                sLogScope logScope;
                logScope.lsId   = NETrace::makeScopeIdEx(scopeName.getString());
                logScope.lsPrio = scopePrio;
                NEString::copyString<char>(logScope.lsName, LENGTH_SCOPE, scopeName.getString(), scopeName.getLength());
                result = ::logObserverRequestChangeScopePrio(target, &logScope, 1);
            }
        }
    }

    return result;
}

inline void LogObserver::enableLocalLogs(ConfigManager& config, bool /* enable */)
{
    constexpr NEPersistence::eConfigKeys prioConfKey{ NEPersistence::eConfigKeys::EntryLogScope };
    const NEPersistence::sPropertyKey& keyPrio{ NEPersistence::getLogScope() };
    unsigned int prios = static_cast<unsigned int>(NETrace::eLogPriority::PrioNotset);
    const String prio{ NETrace::makePrioString(prios) };

    config.setModuleProperty(keyPrio.section, keyPrio.property, String(NEPersistence::SYNTAX_ANY_VALUE), prio, prioConfKey, true);
    config.setLogEnabled(NETrace::eLogingTypes::LogTypeRemote, false, true);
}
