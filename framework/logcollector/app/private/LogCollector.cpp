/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        logcollector/app/private/LogCollector.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, log collector to run as a console application process or service.
 ************************************************************************/

#include "logcollector/app/LogCollector.hpp"
#include "logcollector/app/private/LogCollectorConsoleService.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/appbase/NEApplication.hpp"
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

namespace
{
    static String _modelName("LogCollectorModel");

    // Describe mode, set model name
    BEGIN_MODEL(_modelName)

        // define console service thread.
        BEGIN_REGISTER_THREAD( "LogCollectorConsoleServiceThread", NECommon::WATCHDOG_IGNORE)
            // Define the console service
            BEGIN_REGISTER_COMPONENT(LogCollectorConsoleService::SERVICE_NAME, LogCollectorConsoleService)
                // register dummy 'empty service'.
                REGISTER_IMPLEMENT_SERVICE( NEService::EmptyServiceName, NEService::EmptyServiceVersion )
            // end of component description
            END_REGISTER_COMPONENT(LogCollectorConsoleService::SERVICE_NAME )
        // end of thread description
        END_REGISTER_THREAD( "LogCollectorConsoleServiceThread" )

    // end of model description
    END_MODEL(_modelName)

    constexpr std::string_view _msgHelp []
    {
          {"Usage of AREG Log collector (logcollector) :"}
        , NESystemService::MSG_SEPARATOR
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
        , NESystemService::MSG_SEPARATOR
    };
}

//////////////////////////////////////////////////////////////////////////
// Scopes.
//////////////////////////////////////////////////////////////////////////

DEF_LOG_SCOPE(logcollector_app_logcollector_serviceMain);
DEF_LOG_SCOPE(logcollector_app_logcollector_serviceStart);
DEF_LOG_SCOPE(logcollector_app_logcollector_serviceStop);
DEF_LOG_SCOPE(logcollector_app_logcollector_servicePause);
DEF_LOG_SCOPE(logcollector_app_logcollector_serviceContinue);
DEF_LOG_SCOPE(logcollector_app_logcollector_serviceInstall);
DEF_LOG_SCOPE(logcollector_app_logcollector_serviceUninstall);
DEF_LOG_SCOPE(logcollector_app_logcollector_setState);

//////////////////////////////////////////////////////////////////////////
// LogCollector class implementation
//////////////////////////////////////////////////////////////////////////

const OptionParser::sOptionSetup LogCollector::ValidOptions[ ]
{
      { "-a", "--save"      , static_cast<int>(eLoggerOptions::CMD_LogSaveLogs)     , OptionParser::STRING_NO_RANGE , {}, {}, {} }
    , { "-b", "--unsave"    , static_cast<int>(eLoggerOptions::CMD_LogSaveLogsStop) , OptionParser::NO_DATA         , {}, {}, {} }
    , { "-c", "--console"   , static_cast<int>(eLoggerOptions::CMD_LogConsole)      , OptionParser::NO_DATA         , {}, {}, {} }
    , { "-e", "--query"     , static_cast<int>(eLoggerOptions::CMD_LogQueryScopes)  , OptionParser::STRING_NO_RANGE , {}, {}, {} }
    , { "-f", "--config"    , static_cast<int>(eLoggerOptions::CMD_LogSaveConfig)   , OptionParser::STRING_NO_RANGE , {}, {}, {} }
    , { "-h", "--help"      , static_cast<int>(eLoggerOptions::CMD_LogPrintHelp)    , OptionParser::NO_DATA         , {}, {}, {} }
    , { "-i", "--install"   , static_cast<int>(eLoggerOptions::CMD_LogInstall)      , OptionParser::NO_DATA         , {}, {}, {} }
    , { "-l", "--load"      , static_cast<int>(eLoggerOptions::CMD_LogLoad)         , OptionParser::STRING_NO_RANGE , {}, {}, {} }
    , { "-n", "--instances" , static_cast<int>(eLoggerOptions::CMD_LogInstances)    , OptionParser::NO_DATA         , {}, {}, {} }
    , { "-o", "--scope"     , static_cast<int>(eLoggerOptions::CMD_LogUpdateScope)  , OptionParser::STRING_NO_RANGE , {}, {}, {} }
    , { "-p", "--pause"     , static_cast<int>(eLoggerOptions::CMD_LogPause)        , OptionParser::NO_DATA         , {}, {}, {} }
    , { "-q", "--quit"      , static_cast<int>(eLoggerOptions::CMD_LogQuit)         , OptionParser::NO_DATA         , {}, {}, {} }
    , { "-r", "--restart"   , static_cast<int>(eLoggerOptions::CMD_LogRestart)      , OptionParser::NO_DATA         , {}, {}, {} }
    , { "-s", "--service"   , static_cast<int>(eLoggerOptions::CMD_LogService)      , OptionParser::NO_DATA         , {}, {}, {} }
    , { "-t", "--silent"    , static_cast<int>(eLoggerOptions::CMD_LogSilent)       , OptionParser::NO_DATA         , {}, {}, {} }
    , { "-u", "--uninstall" , static_cast<int>(eLoggerOptions::CMD_LogUninstall)    , OptionParser::NO_DATA         , {}, {}, {} }
    , { "-v", "--verbose"   , static_cast<int>(eLoggerOptions::CMD_LogVerbose)      , OptionParser::NO_DATA         , {}, {}, {} }
};

LogCollector & LogCollector::getInstance(void)
{
    static LogCollector _logger;
    return _logger;
}

#if AREG_EXTENDED
void LogCollector::printStatus(const String& status)
{

    if (LogCollector::getInstance().getCurrentOption() == NESystemService::eServiceOption::CMD_Console)
    {
        Console& console{ Console::getInstance() };
        Console::Coord curPos{ console.getCursorCurPosition() };
        LogCollector::_outputInfo(status);
        console.setCursorCurPosition(curPos);
    }

}
#else   // AREG_EXTENDED
void LogCollector::printStatus(const String& /* status */)
{
}
#endif  // AREG_EXTENDED

LogCollector::LogCollector( void )
    : ServiceApplicationBase( mServiceServer )
    , mServiceServer        ( )
{
}

Console::CallBack LogCollector::getOptionCheckCallback( void ) const
{
    return Console::CallBack( LogCollector::_checkCommand );
}

void LogCollector::runConsoleInputExtended( void )
{
#if AREG_EXTENDED

    Console & console = Console::getInstance( );
    LogCollector::_outputTitle( );

    if (getDataRateHelper().isVerbose())
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

void LogCollector::runConsoleInputSimple( void )
{
    constexpr uint32_t bufSize{ 512 };
    char cmd[bufSize]{ 0 };
    bool quit{ false };

    LogCollector::_outputTitle( );

    do
    {
        printf( "%s", NESystemService::FORMAT_WAIT_QUIT.data( ) );
        if (inputConsoleData(cmd, bufSize) == false)
            continue;

        quit = LogCollector::_checkCommand( cmd );

    } while ( quit == false );
}

void LogCollector::runService(void)
{
    Application::waitAppQuit(NECommon::WAIT_INFINITE);
}

std::pair<const OptionParser::sOptionSetup*, int> LogCollector::getAppOptions(void) const
{
    static  std::pair< const OptionParser::sOptionSetup*, int> _opts( std::pair< const OptionParser::sOptionSetup*
                                                                    , int>(LogCollector::ValidOptions
                                                                    , static_cast<int>(MACRO_ARRAYLEN(LogCollector::ValidOptions))));
    return _opts;
}

wchar_t* LogCollector::getServiceNameW(void) const
{
    return NELogCollectorSettings::SERVICE_NAME_WIDE;
}

char* LogCollector::getServiceNameA(void) const
{
    return NELogCollectorSettings::SERVICE_NAME_ASCII;
}

wchar_t* LogCollector::getServiceDisplayNameW(void) const
{
    return NELogCollectorSettings::SERVICE_DISPLAY_NAME_WIDE;
}

char* LogCollector::getServiceDisplayNameA(void) const
{
    return NELogCollectorSettings::SERVICE_DISPLAY_NAME_ASCII;
}

wchar_t* LogCollector::getServiceDescriptionW(void) const
{
    return NELogCollectorSettings::SERVICE_DESCRIBE_WIDE;
}

char* LogCollector::getServiceDescriptionA(void) const
{
    return NELogCollectorSettings::SERVICE_DESCRIBE_ASCII;
}

NERemoteService::eRemoteServices LogCollector::getServiceType(void) const
{
    return NERemoteService::eRemoteServices::ServiceLogger;
}

NERemoteService::eConnectionTypes LogCollector::getConnectionType(void) const
{
    return NERemoteService::eConnectionTypes::ConnectTcpip;
}

void LogCollector::postReadConfiguration(ConfigManager& config)
{
    _enableLocalLogs(config, false);
}

void LogCollector::onSetupConfiguration(const NEPersistence::ListProperties& /* listReadonly */, const NEPersistence::ListProperties& /* listWritable */, ConfigManager& config)
{
    _enableLocalLogs(config, false);
}

void LogCollector::printHelp( bool /* isCmdLine */ )
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

void LogCollector::startConsoleService( void )
{
    Application::loadModel( _modelName );
}

void LogCollector::stopConsoleService( void )
{
    Application::unloadModel( _modelName );
}

bool LogCollector::_checkCommand(const String& cmd)
{
    OptionParser parser( LogCollector::ValidOptions, MACRO_ARRAYLEN( LogCollector::ValidOptions ) );
    bool quit{ false };
    bool hasError {false};

    LogCollector::_cleanHelp();

    if ( parser.parseOptionLine( cmd ) )
    {
        LogCollector & logger = LogCollector::getInstance( );
        const OptionParser::InputOptionList & opts = parser.getOptions( );
        for ( uint32_t i = 0; i < opts.getSize( ); ++ i )
        {
            const OptionParser::sOption & opt = opts[ i ];
            switch ( static_cast<eLoggerOptions>(opt.inCommand) )
            {
            case eLoggerOptions::CMD_LogPause:
                LogCollector::_outputInfo( "Pausing log collector ..." );
                logger.getCommunicationController().disconnectServiceHost( );
                logger.mServiceServer.waitToComplete( );
                LogCollector::_outputInfo( "Log collector is paused ..." );
                break;

            case eLoggerOptions::CMD_LogRestart:
                LogCollector::_outputInfo( "Restarting log collector ..." );
                logger.getCommunicationController( ).connectServiceHost( );
                LogCollector::_outputInfo( "Log collector is restarted ..." );
                break;

            case eLoggerOptions::CMD_LogInstances:
                LogCollector::_outputInstances( logger.getConnetedInstances() );
                break;

            case eLoggerOptions::CMD_LogVerbose:
                LogCollector::_setVerboseMode( true );
                break;

            case eLoggerOptions::CMD_LogSilent:
                LogCollector::_setVerboseMode( false );
                break;

            case eLoggerOptions::CMD_LogPrintHelp:
                logger.printHelp( false );
                break;

            case eLoggerOptions::CMD_LogQuit:
                quit = true;
                break;

            case eLoggerOptions::CMD_LogConsole:        // fall through
            case eLoggerOptions::CMD_LogInstall:        // fall through
            case eLoggerOptions::CMD_LogUninstall:      // fall through
            case eLoggerOptions::CMD_LogService:        // fall through
            case eLoggerOptions::CMD_LogLoad:
                LogCollector::_outputInfo("This command should be used in command line ...");
                break;

            case eLoggerOptions::CMD_LogUpdateScope:
                LogCollector::_processUpdateScopes(opt);
                break;

            case eLoggerOptions::CMD_LogQueryScopes:
                LogCollector::_processQueryScopes(opt);
                break;

            case eLoggerOptions::CMD_LogSaveLogs:       // fall through
            case eLoggerOptions::CMD_LogSaveConfig:     // fall through
            case eLoggerOptions::CMD_LogSaveLogsStop:
                LogCollector::_outputInfo("The feature is not implemented yet!!!");
                break;

            case eLoggerOptions::CMD_LogUndefined:
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

void LogCollector::_outputTitle( void )
{
#if AREG_EXTENDED

    Console & console = Console::getInstance( );
    console.lockConsole();
    console.outputTxt( NESystemService::COORD_TITLE, NELogCollectorSettings::APP_TITLE.data( ) );
    console.outputTxt( NESystemService::COORD_SUBTITLE, NESystemService::MSG_SEPARATOR.data( ) );
    console.unlockConsole();

#else   // !AREG_EXTENDED

    printf( "%s\n", NELogCollectorSettings::APP_TITLE.data( ) );
    printf( "%s\n", NESystemService::MSG_SEPARATOR.data( ) );

#endif  // AREG_EXTENDED
}

void LogCollector::_outputInfo( const String & info )
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

void LogCollector::_outputInstances( const NEService::MapInstances & instances )
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

#if AREG_EXTENDED
void LogCollector::_setVerboseMode( bool makeVerbose )
{

    static constexpr std::string_view _verbose{ "Switching to verbose mode to output data rate ..." };
    static constexpr std::string_view _silence{ "Switching to silent mode, no data rate output ..." };
    LogCollector & logger = LogCollector::getInstance( );
    Console & console = Console::getInstance( );
    console.lockConsole( );
    if ( logger.getDataRateHelper().isVerbose() != makeVerbose )
    {
        logger.getDataRateHelper().setVerbose(makeVerbose);

        if ( makeVerbose == false )
        {
            console.clearLine( NESystemService::COORD_SEND_RATE );
            console.clearLine( NESystemService::COORD_RECV_RATE );
            console.outputTxt( NESystemService::COORD_INFO_MSG, _silence );
        }
        else
        {
            console.outputMsg( NESystemService::COORD_SEND_RATE, NESystemService::FORMAT_SEND_DATA.data( ), 0.0, DataRateHelper::MSG_BYTES.data( ) );
            console.outputMsg( NESystemService::COORD_RECV_RATE, NESystemService::FORMAT_RECV_DATA.data( ), 0.0, DataRateHelper::MSG_BYTES.data( ) );
            console.outputTxt( NESystemService::COORD_INFO_MSG, _verbose);
        }

        console.refreshScreen( );
    }

    console.unlockConsole( );


    static constexpr std::string_view _unsupported{"This option is available only with extended features"};
    printf( "%s\n", _unsupported.data( ) );
}

#else   // !AREG_EXTENDED

void LogCollector::_setVerboseMode( bool /* makeVerbose */ )
{
    static constexpr std::string_view _unsupported{"This option is available only with extended features"};
    printf( "%s\n", _unsupported.data( ) );
}

#endif  // AREG_EXTENDED

void LogCollector::_cleanHelp(void)
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

void LogCollector::_processUpdateScopes(const OptionParser::sOption& optScope)
{
    LogCollector& logger{ LogCollector::getInstance() };
    TEArrayList<RemoteMessage> msgList;
    _createScopeMessage(optScope, msgList);
    for (uint32_t i = 0; i < msgList.getSize(); ++ i)
    {
        logger.mServiceServer.dispatchAndForwardLoggerMessage(msgList[i]);
    }
}

void LogCollector::_processQueryScopes(const OptionParser::sOption& optScope)
{
    LogCollector& logger{ LogCollector::getInstance() };
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
        logger.mServiceServer.dispatchAndForwardLoggerMessage(NELogging::messageQueryScopes(NEService::COOKIE_LOGGER, target));
    }
}

void LogCollector::_createScopeMessage(const OptionParser::sOption& optScope, TEArrayList<RemoteMessage>& OUT msgList)
{
    ASSERT(optScope.inCommand == static_cast<int>(eLoggerOptions::CMD_LogUpdateScope));
    ASSERT(optScope.inString.empty() == false);

    const OptionParser::StrList& optValues{ optScope.inString };
    String scope;
    for (const auto& entry : optValues)
    {
        if (entry == NEPersistence::SYNTAX_END_COMMAND)
        {
            RemoteMessage msg{ LogCollector::_createScopeUpdateMessage(scope) };
            scope.clear();
            if (msg.isValid() == false)
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

    if (scope.isEmpty() == false)
    {
        RemoteMessage msg{ LogCollector::_createScopeUpdateMessage(scope) };
        if (msg.isValid() == false)
        {
            msgList.clear();
        }
        else
        {
            msgList.add(msg);
        }
    }
}

String LogCollector::_normalizeScopeProperty(const String & scope)
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

RemoteMessage LogCollector::_createScopeUpdateMessage(const String& scope)
{
    RemoteMessage result;

    if (scope.isEmpty() == false)
    {
        Property prop(LogCollector::_normalizeScopeProperty(scope));
        if (prop.isValid() && prop.getPropertyType() == NEPersistence::eConfigKeys::EntryLogScope)
        {
            const PropertyKey& key{ prop.getKey() };
            ITEM_ID target{ key.isAllModules() ? NEService::COOKIE_ANY : key.getModule().toUInt32() };
            if (target >= NEService::COOKIE_ANY)
            {
                String scopeName{ key.getPosition() };
                uint32_t scopePrio{ prop.getValue().getIndetifier(NEApplication::LogScopePriorityIndentifiers) };
                result = NELogging::messageUpdateScope(NEService::COOKIE_LOGGER, target, scopeName, NELogging::LOG_SCOPE_ID_NONE, scopePrio);
                result.setTarget(target);
            }
        }
    }

    return result;
}

inline void LogCollector::_enableLocalLogs(ConfigManager& config, bool enable)
{
    constexpr NEPersistence::eConfigKeys prioConfKey{ NEPersistence::eConfigKeys::EntryLogScope };
    const NEPersistence::sPropertyKey& keyPrio{ NEPersistence::getLogScope() };
    unsigned int prios = enable
                ? static_cast<unsigned int>(NELogging::eLogPriority::PrioNotset) | static_cast<unsigned int>(NELogging::eLogPriority::PrioNotset)
                : static_cast<unsigned int>(NELogging::eLogPriority::PrioNotset);
    const String prio{ NELogging::makePrioString(prios) };

    config.setModuleProperty(keyPrio.section, keyPrio.property, String(NEPersistence::SYNTAX_ANY_VALUE), prio, prioConfKey, true);
    config.setLogEnabled(NELogging::eLogingTypes::LogTypeRemote, false, true);
}
