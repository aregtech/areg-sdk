/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        logger/app/private/Logger.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, logger to run as a console application process or service.
 ************************************************************************/

#include "logger/app/Logger.hpp"
#include "logger/app/private/LoggerConsoleService.hpp"

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
    static String _modelName("LoggerModel");

    // Describe mode, set model name
    BEGIN_MODEL(_modelName)

        // define console service thread.
        BEGIN_REGISTER_THREAD( "LoggerConsoleServiceThread", NECommon::WATCHDOG_IGNORE)
            // Define the console service
            BEGIN_REGISTER_COMPONENT( LoggerConsoleService::SERVICE_NAME, LoggerConsoleService)
                // register dummy 'empty service'.
                REGISTER_IMPLEMENT_SERVICE( NEService::EmptyServiceName, NEService::EmptyServiceVersion )
            // end of component description
            END_REGISTER_COMPONENT(LoggerConsoleService::SERVICE_NAME )
        // end of thread description
        END_REGISTER_THREAD( "LoggerConsoleServiceThread" )

    // end of model description
    END_MODEL(_modelName)
}

//////////////////////////////////////////////////////////////////////////
// Traces.
//////////////////////////////////////////////////////////////////////////

DEF_TRACE_SCOPE(logger_app_logger_serviceMain);
DEF_TRACE_SCOPE(logger_app_logger_serviceStart);
DEF_TRACE_SCOPE(logger_app_logger_serviceStop);
DEF_TRACE_SCOPE(logger_app_logger_servicePause);
DEF_TRACE_SCOPE(logger_app_logger_serviceContinue);
DEF_TRACE_SCOPE(logger_app_logger_serviceInstall);
DEF_TRACE_SCOPE(logger_app_logger_serviceUninstall);
DEF_TRACE_SCOPE(logger_app_logger_setState);

//////////////////////////////////////////////////////////////////////////
// Logger class implementation
//////////////////////////////////////////////////////////////////////////

const OptionParser::sOptionSetup Logger::ValidOptions[ ]
{
      {"-p", "--pause"      , static_cast<int>(eLogCommands::CMD_LogPause)      , OptionParser::NO_DATA         , {}, {}, {} }
    , {"-r", "--restart"    , static_cast<int>(eLogCommands::CMD_LogRestart)    , OptionParser::NO_DATA         , {}, {}, {} }
    , {"-s", "--scope"      , static_cast<int>(eLogCommands::CMD_LogSetScope)   , OptionParser::FREESTYLE_DATA  , {}, {}, {} }
    , {"-i", "--instances"  , static_cast<int>(eLogCommands::CMD_LogInstances)  , OptionParser::NO_DATA         , {}, {}, {} }
    , {"-l", "--save-log"   , static_cast<int>(eLogCommands::CMD_LogSaveLogs)   , OptionParser::FREESTYLE_DATA  , {}, {}, {} }
    , {"-c", "--config"     , static_cast<int>(eLogCommands::CMD_LogSaveConfig) , OptionParser::FREESTYLE_DATA  , {}, {}, {} }
    , {"-v", "--verbose"    , static_cast<int>(eLogCommands::CMD_LogVerbose)    , OptionParser::NO_DATA         , {}, {}, {} }
    , {"-n", "--silent"     , static_cast<int>(eLogCommands::CMD_LogSilent)     , OptionParser::NO_DATA         , {}, {}, {} }
    , {"-h", "--help"       , static_cast<int>(eLogCommands::CMD_LogPrintHelp)  , OptionParser::NO_DATA         , {}, {}, {} }
    , {"-q", "--quit"       , static_cast<int>(eLogCommands::CMD_LogQuit)       , OptionParser::NO_DATA         , {}, {}, {} }
};

Logger & Logger::getInstance(void)
{
    static Logger _logger;
    return _logger;
}

std::pair<const OptionParser::sOptionSetup *, int> Logger::getOptionSetup( void )
{
    return std::pair< const OptionParser::sOptionSetup *, int>( Logger::ValidOptions, static_cast<int>(MACRO_ARRAYLEN( Logger::ValidOptions )) );
}

Logger::Logger( void )
    : SystemServiceBase ( mServiceServer )
    , mServiceServer    ( )
{
}

Logger::~Logger( void )
{
    _osFreeResources( );
}

Console::CallBack Logger::getOptionCheckCallback( void ) const
{
    return Console::CallBack( Logger::_checkCommand );
}

void Logger::runConsoleInputExtended( void )
{
#if AREG_EXTENDED
    Console & console = Console::getInstance( );
    Logger::_outputTitle( );

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

void Logger::runConsoleInputSimple( void )
{
    char cmd[ 512 ]{ 0 };
    bool quit{ false };
    OptionParser parser( Logger::ValidOptions, MACRO_ARRAYLEN( Logger::ValidOptions ) );

    Logger::_outputTitle( );

    do
    {
        printf( "%s", NESystemService::FORMAT_WAIT_QUIT.data( ) );
        int count = MACRO_SCANF( "%510s", cmd, 512 );
        if ((count <= 0) || (count >= 512))
            continue;

        quit = Logger::_checkCommand( cmd );

    } while ( quit == false );
}

void Logger::serviceMain( int argc, char ** argv )
{
    // Start only tracing and timer manager.
    Application::initApplication(true, true, false, true, false, NEApplication::DEFAULT_TRACING_CONFIG_FILE.data(), nullptr );
    SystemServiceBase::serviceMain( argc, argv );
#if  AREG_EXTENDED
    Application::waitAppQuit( );
#endif // AREG_EXTENDED
    Application::releaseApplication( );
}

bool Logger::serviceStart(void)
{
    TRACE_SCOPE( logger_app_logger_serviceStart );
    TRACE_DBG( "Starting service [ %s ]", NELoggerSettings::SERVICE_NAME_ASCII );
    bool result{ false };
    if ( getService().setupServiceConnectionHost( NEApplication::DEFAULT_LOGGER_CONFIG_FILE.data( ) ) && getService().connectServiceHost( ) )
    {
        result = setState(NESystemService::eSystemServiceState::ServiceRunning);
    }
    else
    {
        Application::signalAppQuit();
    }

    return result;
}

void Logger::serviceStop(void)
{
    TRACE_SCOPE(logger_app_logger_serviceStop);
    TRACE_WARN("Stopping service [ %s ]", NELoggerSettings::SERVICE_NAME_ASCII);
    setState(NESystemService::eSystemServiceState::ServiceStopping);
    getService().disconnectServiceHost();
    Application::signalAppQuit();
}

void Logger::servicePause(void)
{
    TRACE_SCOPE(logger_app_logger_servicePause);
    TRACE_DBG("Pausing Log Collector service");

    setState( NESystemService::eSystemServiceState::ServicePausing );
    getService( ).disconnectServiceHost();
    setState( NESystemService::eSystemServiceState::ServicePaused );
}

bool Logger::serviceContinue(void)
{
    TRACE_SCOPE(logger_app_logger_serviceContinue);
    TRACE_DBG("Continuing Log Collector service");

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

bool Logger::serviceInstall(void)
{
    TRACE_SCOPE(logger_app_logger_serviceInstall);
    
    if ( _osOpenService() == false )
    {
        _osCcreateService();
    }

    return _osIsValid();
}

void Logger::serviceUninstall(void)
{
    TRACE_SCOPE(logger_app_logger_serviceUninstall);

    if ( _osOpenService() )
    {
        _osDeleteService();
    }

    _osFreeResources();
}

bool Logger::registerService( void )
{
    return _osRegisterService();
}

bool Logger::serviceOpen( void )
{
    return _osOpenService( );
}

bool Logger::setState( NESystemService::eSystemServiceState newState )
{
    TRACE_SCOPE( logger_app_logger_setState );
    TRACE_DBG( "Changing Log Collector service state. Old state [ %s ], new state [ %s ]"
               , NESystemService::getString( mSystemServiceState )
               , NESystemService::getString( newState ) );

    return _osSetState( newState );
}

void Logger::printHelp( bool isCmdLine )
{
    std::cout
        << "Usage of AREG Log Collector (logger) :" << std::endl
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

void Logger::startConsoleService( void )
{
    Application::loadModel( _modelName );
}

void Logger::stopConsoleService( void )
{
    Application::unloadModel( _modelName );
}

bool Logger::_checkCommand(const String& cmd)
{
    OptionParser parser( Logger::ValidOptions, static_cast<int>(MACRO_ARRAYLEN( Logger::ValidOptions )) );
    bool quit{ false };
    bool hasError {false};

    if ( parser.parseOptionLine( cmd ) )
    {
        Logger & logger = Logger::getInstance( );
        const OptionParser::InputOptionList & opts = parser.getOptions( );
        for ( int i = 0; i < static_cast<int>(opts.getSize( )); ++ i )
        {
            const OptionParser::sOption & opt = opts[ 0 ];
            switch ( static_cast<eLogCommands>(opt.inCommand) )
            {
            case eLogCommands::CMD_LogPause:
                Logger::_outputInfo( "Pausing log collector ..." );
                logger.getService().disconnectServiceHost( );
                Logger::_outputInfo( "Log collector is paused ..." );
                break;

            case eLogCommands::CMD_LogRestart:
                Logger::_outputInfo( "Restarting log collector ..." );
                logger.getService( ).connectServiceHost( );
                Logger::_outputInfo( "Log collector is restarted ..." );
                break;

            case eLogCommands::CMD_LogInstances:
                Logger::_outputInstances( logger.getConnetedInstances() );
                break;

            case eLogCommands::CMD_LogVerbose:
                Logger::_setVerboseMode( true );
                break;

            case eLogCommands::CMD_LogSilent:
                Logger::_setVerboseMode( false );
                break;

            case eLogCommands::CMD_LogPrintHelp:
                logger.printHelp( false );
                break;

            case eLogCommands::CMD_LogQuit:
                quit = true;
                break;

            case eLogCommands::CMD_LogSetScope:
            case eLogCommands::CMD_LogSaveLogs:
            case eLogCommands::CMD_LogSaveConfig:

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
        printf( "%s\n", NESystemService::FORMAT_WAIT_QUIT.data( ) );
    }

#endif  // AREG_EXTENDED

    return quit;
}

void Logger::_outputTitle( void )
{
#if AREG_EXTENDED
    Console & console = Console::getInstance( );
    console.outputTxt( NESystemService::COORD_TITLE, NELoggerSettings::APP_TITLE.data( ) );
    console.outputTxt( NESystemService::COORD_SUBTITLE, NESystemService::MSG_SEPARATOR.data( ) );
#else   // !AREG_EXTENDED
    printf( "%s\n", NELoggerSettings::APP_TITLE.data( ) );
    printf( "%s\n", NESystemService::MSG_SEPARATOR.data( ) );
#endif  // AREG_EXTENDED
}

void Logger::_outputInfo( const String & info )
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

void Logger::_outputInstances( const ServiceCommunicatonBase::InstanceMap & instances )
{
    static constexpr std::string_view _table{ "Nr.   |  Instance ID  |  Name " };
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
#if AREG_EXTENDED
            console.outputMsg( coord, " % 3d. |  % 6u       |  %s ", i ++, static_cast<unsigned int>(cookie), name.getString( ) );
            ++ coord.posY;
#else   // !AREG_EXTENDED
            printf( " % 3d. |  % 6u       |  %s \n", i ++, static_cast<unsigned int>(cookie), name.getString( ) );
#endif  // AREG_EXTENDED
        }
    }

#if AREG_EXTENDED
    console.unlockConsole( );
#else   // !AREG_EXTENDED
    printf( "%s\n", NESystemService::FORMAT_WAIT_QUIT.data( ) );
#endif  // AREG_EXTENDED
}

void Logger::_setVerboseMode( bool makeVerbose )
{
#if AREG_EXTENDED

    static constexpr std::string_view _verbose{ "Switching to verbose mode to output data rate ..." };
    static constexpr std::string_view _silence{ "Switching to silent mode, no data rate output ..." };
    Logger & logger = Logger::getInstance( );
    Console & console = Console::getInstance( );
    console.lockConsole( );
    if ( logger.mRunVerbose != makeVerbose )
    {
        logger.mRunVerbose = makeVerbose;

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
