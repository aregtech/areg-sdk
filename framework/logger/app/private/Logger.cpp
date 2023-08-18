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

#include "extensions/console/Console.hpp"

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

Logger::Logger( void )
    : mLoggerState  ( NELoggerSettings::eLoggerState::LoggerStopped )
    , mServiceCmd   ( NELoggerSettings::eServiceCommand::CMD_Undefined )
    , mRunVerbose   ( false )
    // , mServiceServer( )
    , mSvcHandle    ( nullptr )
    , mSeMHandle    ( nullptr )
{
}

Logger::~Logger( void )
{
    _osFreeResources( );
}

bool Logger::parseOptions(int argc, const char** argv)
{
    using ServiceCmd = NELoggerSettings::eServiceCommand;
    bool result{ false };
    
    if (argc > 1)
    {
        bool printHelp{ true };
        OptionParser parser( NELoggerSettings::ServiceCommands, MACRO_ARRAYLEN( NELoggerSettings::ServiceCommands) );
        if ( parser.parseCommandLine( static_cast<const char **>(argv), argc ) )
        {
            printHelp = false;
            const OptionParser::InputOptionList & opts = parser.getOptions( );
            for ( int i = 0; i < static_cast<int>(opts.getSize( )); ++i )
            {
                const OptionParser::sOption & opt = opts[ i ];
                switch ( static_cast<NELoggerSettings::eServiceCommand>(opt.inCommand) )
                {
                case NELoggerSettings::eServiceCommand::CMD_Install:
                case NELoggerSettings::eServiceCommand::CMD_Uninstall:
                case NELoggerSettings::eServiceCommand::CMD_Service:
                case NELoggerSettings::eServiceCommand::CMD_Console:
                    result = true;
                    setCurrentCommand( static_cast<NELoggerSettings::eServiceCommand>(opt.inCommand) );
                    break;

                case NELoggerSettings::eServiceCommand::CMD_Verbose:
                    mRunVerbose = true;
                    setCurrentCommand( NELoggerSettings::eServiceCommand::CMD_Console );
                    result = true;
                    break;

                case NELoggerSettings::eServiceCommand::CMD_Help:
                    printHelp = true;
                    break;

                default:
                    setCurrentCommand( NELoggerSettings::eServiceCommand::CMD_Undefined );
                    printHelp = true;
                    break;
                }
            }
        }

        if ( printHelp )
        {
            std::cout   
                << "Usage of AREG Log Collector (logger) :" << std::endl
                << "---------------------------------------------------------------------------------------------"  << std::endl
                << "-i, --install   : Command to install service. Valid for Windows OS, ignored in other cases."    << std::endl
                << "-u, --uninstall : Command to uninstall service. Valid for Windows OS, ignored in other cases."  << std::endl
                << "-s, --service   : Command to run process as a system service process."                          << std::endl
                << "-c, --console   : Command to run process as a console application."                             << std::endl
                << "-v, --verbose   : Command to display data rate. Can be used in combination with \'--console\'"  << std::endl
                << "-h, --help      : Command to display this message on console."                                  << std::endl
                << "---------------------------------------------------------------------------------------------"  << std::endl
                << "Quit application ..." << std::endl << std::ends;

            result = false;
        }
    }
    else if (argc == 1)
    {
        resetDefaultOptions();
        result = true;
    }

    return result;
}

void Logger::serviceMain( int argc, char ** argv )
{
    // Start only tracing and timer manager.
    Application::initApplication(true, true, false, true, false, NEApplication::DEFAULT_TRACING_CONFIG_FILE.data(), nullptr );

    do 
    {
        TRACE_SCOPE(logger_app_logger_serviceMain);
        TRACE_DBG("Starting logger service. There are [ %d ] arguments in the list...", argc);

#ifdef  DEBUG
        for ( int i = 0; i < argc; ++ i )
            TRACE_DBG("... Command argument [ %d ]: [ %s ]", i, argv[i]);
#endif  // DEBUG

        if ( _osRegisterService() || (mServiceCmd == NELoggerSettings::eServiceCommand::CMD_Console) )
        {
            TRACE_DBG("Starting service");
            serviceStart();
        }

        if ( mServiceCmd == NELoggerSettings::eServiceCommand::CMD_Console )
        {
#if AREG_EXTENDED

            Console& console = Console::getInstance();

            if (mRunVerbose)
            {
                // Disable to block user input until Console Service is up and running.
                console.enableConsoleInput(false);
                Application::loadModel(_modelName);
                // Blocked until user input
                Console::CallBack callback(Logger::_checkCommand);
                console.waitForInput(callback);
                Application::unloadModel(_modelName);
            }
            else
            {
                // No verbose mode.
                // Set local callback, output message and wait for user input.
                Console::CallBack callback(Logger::_checkCommand);
                console.enableConsoleInput(true);
                console.outputTxt(Console::Coord{ 0, 0 }, NELoggerSettings::FORMAT_WAIT_QUIT);
                console.waitForInput(callback);
            }

            console.moveCursorOneLineDown();
            console.printTxt(NELoggerSettings::FORMAT_QUIT_APP);
            console.uninitialize();

#else   // !AREG_EXTENDED

            char cmd[512] { 0 };
            bool quit{ false };
            OptionParser parser( Logger::ValidOptions, MACRO_ARRAYLEN( Logger::ValidOptions ) );

            do
            {
                printf( NELoggerSettings::FORMAT_WAIT_QUIT.data() );
                MACRO_SCANF( "%512s", cmd, 512 );
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
                    printf( NELoggerSettings::FORMAT_MSG_ERROR.data(), cmd );
                    printf( "\n" );
                }

            } while ( quit == false );

#endif  // !AREG_EXTENDED

            Application::signalAppQuit();
        }

        serviceStop();
        TRACE_WARN("Service Stopped and not running anymore");

        Application::releaseApplication();

    } while (false);
}

bool Logger::serviceStart(void)
{
    TRACE_SCOPE(logger_app_logger_serviceStart);
    TRACE_DBG("Starting service [ %s ]", NELoggerSettings::SERVICE_NAME_ASCII);
    bool result = false;

#if 0
    if (  mServiceServer.setupServiceConnectionHost( NEApplication::DEFAULT_ROUTER_CONFIG_FILE.data() ) && mServiceServer.connectServiceHost() )
    {
        result = setState(NELoggerSettings::eLoggerState::LoggerRunning);
    }
    else
    {
        Application::signalAppQuit();
    }
#endif

    return result;
}

void Logger::serviceStop(void)
{
    TRACE_SCOPE(logger_app_logger_serviceStop);
    TRACE_WARN("Stopping service [ %s ]", NELoggerSettings::SERVICE_NAME_ASCII);
    setState(NELoggerSettings::eLoggerState::LoggerStopping);
    // mServiceServer.disconnectServiceHost();
    Application::signalAppQuit();
}

void Logger::servicePause(void)
{
    TRACE_SCOPE(logger_app_logger_servicePause);
    TRACE_DBG("Pausing Router service");

    setState( NELoggerSettings::eLoggerState::LoggerPausing );
    // mServiceServer.disconnectServiceHost();
    setState( NELoggerSettings::eLoggerState::LoggerPaused );
}

bool Logger::serviceContinue(void)
{
    TRACE_SCOPE(logger_app_logger_serviceContinue);
    TRACE_DBG("Continuing Logger service");

    bool result = false;
    setState( NELoggerSettings::eLoggerState::LoggerContinuing );
#if 0
    if ( mServiceServer.isServiceHostSetup() && mServiceServer.connectServiceHost() )
    {
        result = true;
        setState( NELoggerSettings::eLoggerState::LoggerRunning );
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

bool Logger::setState( NELoggerSettings::eLoggerState newState )
{
    TRACE_SCOPE( logger_app_logger_setState );
    TRACE_DBG( "Changing Service Logger state. Old state [ %s ], new state [ %s ]"
                , NELoggerSettings::getString( mLoggerState )
                , NELoggerSettings::getString( newState ) );

    return _osSetState( newState );
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
            console.outputMsg( logger.mRunVerbose ? NELoggerSettings::COORD_ERROR_MSG : Console::Coord{ 0, 1 }
                             , NELoggerSettings::FORMAT_MSG_ERROR.data(), cmd.getString());
        }

        console.outputTxt( logger.mRunVerbose ? NELoggerSettings::COORD_USER_INPUT : Console::Coord{ 0, 0 }
                         , NELoggerSettings::FORMAT_WAIT_QUIT);
        console.refreshScreen();
    }

    return quit;
}
