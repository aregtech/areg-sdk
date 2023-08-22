/************************************************************************
 * \file        logger/app/private/Logger.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       Logger common part
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
      {"-p", "--pause"      , static_cast<int>(Logger::eLogCommands::CMD_LogPause)     , OptionParser::NO_DATA         , {}, {}, {} }
    , {"-r", "--restart"    , static_cast<int>(Logger::eLogCommands::CMD_LogRestart)   , OptionParser::NO_DATA         , {}, {}, {} }
    , {"-s", "--scope"      , static_cast<int>(Logger::eLogCommands::CMD_LogSetScope)  , OptionParser::FREESTYLE_DATA  , {}, {}, {} }
    , {"-i", "--instances"  , static_cast<int>(Logger::eLogCommands::CMD_LogInstances) , OptionParser::NO_DATA         , {}, {}, {} }
    , {"-l", "--save-log"   , static_cast<int>(Logger::eLogCommands::CMD_LogSaveLogs)  , OptionParser::FREESTYLE_DATA  , {}, {}, {} }
    , {"-c", "--config"     , static_cast<int>(Logger::eLogCommands::CMD_LogSaveConfig), OptionParser::FREESTYLE_DATA  , {}, {}, {} }
    , {"-h", "--help"       , static_cast<int>(Logger::eLogCommands::CMD_LogPrintHelp) , OptionParser::NO_DATA         , {}, {}, {} }
    , {"-q", "--quit"       , static_cast<int>(Logger::eLogCommands::CMD_LogQuit)      , OptionParser::NO_DATA         , {}, {}, {} }
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
        console.outputTxt( NESystemService::COORD_TITLE, mTitle.getString( ) );
        console.outputTxt( NESystemService::COORD_SUBTITLE, NESystemService::APP_SUBTITE.data( ) );
        console.outputTxt( NESystemService::COORD_USER_INPUT, NESystemService::FORMAT_WAIT_QUIT );
        console.waitForInput( getOptionCheckCallback( ) );
    }

    console.moveCursorOneLineDown( );
    console.printTxt( NESystemService::FORMAT_QUIT_APP );
    console.uninitialize( );
#endif   // !AREG_EXTENDED
}

void Logger::runConsoleInputSimple( void )
{
    char cmd[ 512 ]{ 0 };
    bool quit{ false };
    OptionParser parser( Logger::ValidOptions, MACRO_ARRAYLEN( Logger::ValidOptions ) );

    printf( "%s\n", APP_TITLE.data() );
    printf( "%s\n", NESystemService::APP_SUBTITE.data( ) );

    do
    {
        printf( "%s", NESystemService::FORMAT_WAIT_QUIT.data( ) );
        int count = MACRO_SCANF( "%512s", cmd, 512 );
        if ((count <= 0) || (count >= 512))
            continue;

        if ( parser.parseOptionLine( cmd ) )
        {
            const OptionParser::InputOptionList & opts = parser.getOptions( );
            for ( int i = 0; i < static_cast<int>(opts.getSize( )); ++ i )
            {
                const OptionParser::sOption & opt = opts[ 0 ];
                switch ( static_cast<Logger::eLogCommands>(opt.inCommand) )
                {
                case Logger::eLogCommands::CMD_LogQuit:
                    quit = true;
                    break;

                default:
                    break;
                }
            }
        }
        else
        {
            printf( NESystemService::FORMAT_MSG_ERROR.data( ), cmd );
            printf( "\n" );
        }

    } while ( quit == false );
}

void Logger::serviceMain( int argc, char ** argv )
{
    // Start only tracing and timer manager.
    Application::initApplication(true, true, false, true, false, NEApplication::DEFAULT_TRACING_CONFIG_FILE.data(), nullptr );
    SystemServiceBase::serviceMain( argc, argv );
    Application::releaseApplication( );
}

bool Logger::serviceStart( void )
{
    TRACE_SCOPE( logger_app_logger_serviceStart );
    TRACE_DBG( "Starting service [ %s ]", NELoggerSettings::SERVICE_NAME_ASCII );
    bool result = false;

#if 0
    if ( mServiceServer.setupServiceConnectionHost( NEApplication::DEFAULT_ROUTER_CONFIG_FILE.data( ) ) && mServiceServer.connectServiceHost( ) )
    {
        result = setState( NESystemService::eSystemServiceState::ServiceRunning );
    }
    else
    {
        Application::signalAppQuit( );
    }
#endif

    return result;
}

void Logger::serviceStop(void)
{
    TRACE_SCOPE(logger_app_logger_serviceStop);
    TRACE_WARN("Stopping service [ %s ]", NELoggerSettings::SERVICE_NAME_ASCII);
    setState(NESystemService::eSystemServiceState::ServiceStopping);
    // mServiceServer.disconnectServiceHost();
    Application::signalAppQuit();
}

void Logger::servicePause(void)
{
    TRACE_SCOPE(logger_app_logger_servicePause);
    TRACE_DBG("Pausing Router service");

    setState( NESystemService::eSystemServiceState::ServicePausing );
    // mServiceServer.disconnectServiceHost();
    setState( NESystemService::eSystemServiceState::ServicePaused );
}

bool Logger::serviceContinue(void)
{
    TRACE_SCOPE(logger_app_logger_serviceContinue);
    TRACE_DBG("Continuing Logger service");

    bool result = false;
    setState( NESystemService::eSystemServiceState::ServiceContinuing );
#if 0
    if ( mServiceServer.isServiceHostSetup() && mServiceServer.connectServiceHost() )
    {
        result = true;
        setState( NESystemService::eSystemServiceState::ServiceRunning );
    }
    else
    {
        TRACE_ERR("Failed to restart remote servicing");
        Application::signalAppQuit();
    }
#endif

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
    return _osRegisterService( );
}

bool Logger::serviceOpen( void )
{
    return _osOpenService( );
}

bool Logger::setState( NESystemService::eSystemServiceState newState )
{
    TRACE_SCOPE( logger_app_logger_setState );
    TRACE_DBG( "Changing Service Router state. Old state [ %s ], new state [ %s ]"
               , NESystemService::getString( mSystemServiceState )
               , NESystemService::getString( newState ) );

    return _osSetState( newState );
}

void Logger::printHelp( bool isCmdLine )
{
    std::cout
        << "Usage of AREG Message Router (mcrouter) :" << std::endl
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
    OptionParser parser( Logger::ValidOptions, MACRO_ARRAYLEN( Logger::ValidOptions ) );
    bool quit{ false };
    bool hasError {false};

    if ( parser.parseOptionLine( cmd ) )
    {
        const OptionParser::InputOptionList & opts = parser.getOptions( );
        for ( int i = 0; i < static_cast<int>(opts.getSize( )); ++ i )
        {
            const OptionParser::sOption & opt = opts[ 0 ];
            switch ( static_cast<eLogCommands>(opt.inCommand) )
            {
            case eLogCommands::CMD_LogPause:
            case eLogCommands::CMD_LogRestart:
            case eLogCommands::CMD_LogSetScope:
            case eLogCommands::CMD_LogInstances:
            case eLogCommands::CMD_LogSaveLogs:
            case eLogCommands::CMD_LogSaveConfig:
            case eLogCommands::CMD_LogPrintHelp:
                break;

            case eLogCommands::CMD_LogQuit:
                quit = true;
                break;

            default:
                break;
            }
        }
    }
    else
    {
        hasError = true;
    }

    if (quit == false)
    {
        Logger & logger = Logger::getInstance();
        Console& console = Console::getInstance();

        console.clearScreen();
        if (hasError)
        {
            console.outputMsg( logger.mRunVerbose ? NESystemService::COORD_ERROR_MSG : Console::Coord{ 0, 1 }
                               , NESystemService::FORMAT_MSG_ERROR.data( )
                               , cmd.getString( ) );
        }

        console.outputTxt( logger.mRunVerbose ? NESystemService::COORD_USER_INPUT : Console::Coord{ 0, 0 }
                         , NESystemService::FORMAT_WAIT_QUIT );
        console.refreshScreen();
    }

    return quit;
}
