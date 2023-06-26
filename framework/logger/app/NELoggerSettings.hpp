#ifndef AREG_LOGGER_APP_NELOGGERSETTINGS_HPP
#define AREG_LOGGER_APP_NELOGGERSETTINGS_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        logger/app/NELoggerSettings.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Logger settings.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/trace/GETrace.h"
#include "areg/appbase/NEApplication.hpp"

#include "extensions/console/Console.hpp"

#include <string_view>

namespace NELoggerSettings
{
    /**
     * \brief   NELoggerSettings::eLoggerState
     *          Describes the logger state.
     **/
    enum class eLoggerState
    {
          LoggerStopped     //!< Logger is stopped.
        , LoggerStarting    //!< Logger is in starting process.
        , LoggerStopping    //!< Logger is in stopping process.
        , LoggerRunning     //!< Logger is up and running.
        , LoggerContinuing  //!< Logger was paused and in continuing state.
        , LoggerPausing     //!< Logger is in pausing state.
        , LoggerPaused      //!< Logger is paused.
    };

    /**
     * \brief   Returns the human readable string of NELOggerSettings::eLoggerState value
     **/
    inline const char * const getString( NELoggerSettings::eLoggerState LoggerState );

    /**
     * \brief   NELoggerSettings::eServiceCommand
     *          Logger service commands.
     **/
    enum class eServiceCommand
    {
          CMD_Undefined     //!< Command is undefined
        , CMD_Install       //!< Command is to install (register) service in the system
        , CMD_Uninstall     //!< Command is to uninstall (unregister) service in the system
        , CMD_Service       //!< Command is to execute process as a system service (in background)
        , CMD_Console       //!< Command is to execute process as console application.
        , CMD_Verbose       //!< Command is to display the data rate when execute process as console application.
    };

    /**
     * \brief   Converts and returns string value of NELoggerSettings::eServiceCommand type.
     **/
    inline const char * getString( NELoggerSettings::eServiceCommand cmdService );

    /**
     * \brief   The ASCII name of router as a system service.
     **/
    extern char *               SERVICE_NAME_ASCII  /*= NEApplication::LOGGER_SERVICE_NAME_ASCII*/;
    /**
     * \brief   The UNICODE name of router as a system service.
     **/
    extern wchar_t *            SERVICE_NAME_WIDE   /*= NEApplication::LOGGER_SERVICE_NAME_WIDE*/;
    /**
     * \brief   The default option to run multicast router as a console application.
     **/
    constexpr eServiceCommand   DEFAULT_OPTION      { eServiceCommand::CMD_Console };

    /**
     * \brief   List of multicast router string options passed in command line and related eServiceCommand.
     **/
    constexpr NEApplication::ServiceCommand< NELoggerSettings::eServiceCommand> ServiceCommands[]
    {
          {"--install"  , "-i", "Install Logger service"                    , eServiceCommand::CMD_Install}     //!< Command to install service. Valid for Windows OS.
        , {"--uninstall", "-u", "Uninstall Logger service"                  , eServiceCommand::CMD_Uninstall}   //!< Command to uninstall service. Valid for Windows OS.
        , {"--service"  , "-s", "Start Logger as a system service"          , eServiceCommand::CMD_Service}     //!< Command to run process as a system service process.
        , {"--console"  , "-c", "Start Logger as a console application"     , eServiceCommand::CMD_Console}     //!< Command to run process as a console application.
        , {"--verbose"  , "-v", "Display data rate information on console"  , eServiceCommand::CMD_Verbose}     //!< Command to display data rate when run as console application.
        , {""           , ""  , "End of command list"                       ,eServiceCommand::CMD_Undefined}    //!< Any other command.
    };

    /**
     * \brief   The name of main service router thread.
     **/
    constexpr char      LoggerThreadName[]          { "_AREG_SERVICELOGGER_THREAD_" };

    /**
     * \brief   The default option to run multicast router in verbose mode to output send and receive data rate.
     */
    constexpr bool              DEFAULT_VERBOSE{ true };
    /**
     * \brief   Output send data rate message format.
     **/
    constexpr std::string_view  FORMAT_SEND_DATA{ "Send data with the rate: % 7.02f %s" };
    /**
     * \brief   Output receive data rate message format.
     **/
    constexpr std::string_view  FORMAT_RECV_DATA{ "Recv data with the rate: % 7.02f %s" };
    /**
     * \brief   Error command output message format.
     **/
    constexpr std::string_view  FORMAT_MSG_ERROR{ "ERROR, unexpected command [ %s ], please type again ..." };
    /**
     * \brief   Wait for user input command message format.
     **/
    constexpr std::string_view  FORMAT_WAIT_QUIT{ "Type \'quit\' or \'q\' to quit logger ...: " };
    /**
     * \brief   Message quit application.
     **/
    constexpr std::string_view  FORMAT_QUIT_APP{ "\nQuit the logger application ...\n" };
    //!< Char command quit.
    constexpr char              QUIT_CH{ 'q' };
    //!< String command quit.
    constexpr std::string_view  QUIT_STR{ "quit" };

    /**
     * \brief   Coordinate to start to display send data rate message.
     **/
    constexpr Console::Coord    COORD_SEND_RATE{ 0, 1 };
    /**
     * \brief   Coordinate to start to display receive data rate message.
     **/
    constexpr Console::Coord    COORD_RECV_RATE{ 0, 2 };
    /**
     * \brief   Coordinate to start to display user input message.
     **/
    constexpr Console::Coord    COORD_USER_INPUT{ 0, 3 };
    /**
     * \brief   Coordinate to start to display error message.
     **/
    constexpr Console::Coord    COORD_ERROR_MSG{ 0, 4 };

} // namespace NELoggerSettings

//////////////////////////////////////////////////////////////////////////
// NELoggerSettings namespace inline methods.
//////////////////////////////////////////////////////////////////////////

inline const char * const NELoggerSettings::getString(NELoggerSettings::eLoggerState LoggerState )
{
    switch ( LoggerState )
    {
    case NELoggerSettings::eLoggerState::LoggerStopped:
        return "NELoggerSettings::LoggerStopped";
    case NELoggerSettings::eLoggerState::LoggerStarting:
        return "NELoggerSettings::LoggerStarting";
    case NELoggerSettings::eLoggerState::LoggerStopping:
        return "NELoggerSettings::LoggerStopping";
    case NELoggerSettings::eLoggerState::LoggerRunning:
        return "NELoggerSettings::LoggerRunning";
    case NELoggerSettings::eLoggerState::LoggerContinuing:
        return "NELoggerSettings::LoggerContinuing";
    case NELoggerSettings::eLoggerState::LoggerPausing:
        return "NELoggerSettings::LoggerPausing";
    case NELoggerSettings::eLoggerState::LoggerPaused:
        return "NELoggerSettings::LoggerPaused";
    default:
        ASSERT(false);
        return "ERR: Undefined NELoggerSettings::eLoggerState value!!!";
    }
}

inline const char * NELoggerSettings::getString( NELoggerSettings::eServiceCommand cmdService )
{
    switch ( cmdService )
    {
    case NELoggerSettings::eServiceCommand::CMD_Undefined:
        return "NELoggerSettings::CMD_Undefined";
    case NELoggerSettings::eServiceCommand::CMD_Install:
        return "NELoggerSettings::CMD_Install";
    case NELoggerSettings::eServiceCommand::CMD_Uninstall:
        return "NELoggerSettings::CMD_Uninstall";
    case NELoggerSettings::eServiceCommand::CMD_Service:
        return "NELoggerSettings::CMD_Service";
    case NELoggerSettings::eServiceCommand::CMD_Console:
        return "NELoggerSettings::CMD_Console";
    case NELoggerSettings::eServiceCommand::CMD_Verbose:
        return "NELoggerSettings::CMD_Verbose";
    default:
        ASSERT( false );
        return "ERR: Unexpected NELoggerSettings::eServiceCommand value!";
    }
}

#endif  // AREG_MCROUTER_APP_NEMULTICASTROUTERSETTINGS_HPP
