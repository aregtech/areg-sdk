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
#include "extensions/console/OptionParser.hpp"

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
     * \brief   Returns the human readable string of NELoggerSettings::eLoggerState value
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
        , CMD_Help          //!< Command is to display help on console
    };

    /**
     * \brief   Converts and returns string value of NELoggerSettings::eServiceCommand type.
     **/
    inline const char * getString( NELoggerSettings::eServiceCommand cmdService );

    /**
     * \brief   The ASCII name of the logger as a system service.
     **/
    extern char *               SERVICE_NAME_ASCII  /*= NEApplication::LOGGER_SERVICE_NAME_ASCII*/;
    /**
     * \brief   The UNICODE name of the logger as a system service.
     **/
    extern wchar_t *            SERVICE_NAME_WIDE   /*= NEApplication::LOGGER_SERVICE_NAME_WIDE*/;
    /**
     * \brief   The default option to run the logger. Run as a console application.
     **/
    constexpr eServiceCommand   DEFAULT_OPTION      { eServiceCommand::CMD_Console };

    /**
     * \brief   List of the logger options passed in command line and related eServiceCommand.
     **/
    const OptionParser::sOptionSetup ServiceCommands[ ]
    {
        //!< Default command.
        { ""  , ""         , static_cast<int>(eServiceCommand::CMD_Console)    , OptionParser::NO_DATA, {}, {}, {} }
        //!< Command to install service. Valid for Windows OS, ignored in other cases.
      , { "-i", "--install", static_cast<int>(eServiceCommand::CMD_Install)    , OptionParser::NO_DATA, {}, {}, {} }
        //!< Command to uninstall service. Valid for Windows OS, ignored in other cases.
      , { "-u", "--uninstall", static_cast<int>(eServiceCommand::CMD_Uninstall), OptionParser::NO_DATA, {}, {}, {} }
        //!< Command to run process as a system service process.
      , { "-s", "--service", static_cast<int>(eServiceCommand::CMD_Service)    , OptionParser::NO_DATA, {}, {}, {} }
        //!< Command to run process as a console application.
      , { "-c", "--console", static_cast<int>(eServiceCommand::CMD_Console)    , OptionParser::NO_DATA, {}, {}, {} }
        //!< Command to display data rate when run as console application.
      , { "-v", "--verbose", static_cast<int>(eServiceCommand::CMD_Verbose)    , OptionParser::NO_DATA, {}, {}, {} }
        //!< Command to display help on console.
      , { "-h", "--help"   , static_cast<int>(eServiceCommand::CMD_Help)       , OptionParser::NO_DATA, {}, {}, {} }
    };

    /**
     * \brief   The name of main thread in the logger.
     **/
    constexpr char      LoggerThreadName[]          { "_AREG_SERVICELOGGER_THREAD_" };

    /**
     * \brief   The default option to run the logger in verbose mode to output send and receive data rate.
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
    constexpr std::string_view  FORMAT_WAIT_QUIT{ "Type \'-q\' or \'--quit\' to quit the logger ...: " };
    /**
     * \brief   Message quit application.
     **/
    constexpr std::string_view  FORMAT_QUIT_APP{ "\nQuit the logger application ...\n" };

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

#endif  // AREG_LOGGER_APP_NELOGGERSETTINGS_HPP
