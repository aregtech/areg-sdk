#ifndef AREG_AREGEXTEND_SERVICE_SYSTEMSERVICEDEFS_HPP
#define AREG_AREGEXTEND_SERVICE_SYSTEMSERVICEDEFS_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        aregextend/service/SystemServiceDefs.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, The system service basic settings.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/base/StringDefs.hpp"
#include "aregextend/console/Console.hpp"
#include "aregextend/console/OptionParser.hpp"

/**
 * \brief   The system service common data.
 **/
namespace NESystemService
{
    /**
     * \brief   NESystemService::ServiceOption
     *          Message routing service options.
     **/
    enum class ServiceOption    : int32_t
    {
          CMD_Undefined     //!< Option is undefined.
        , CMD_Help          //!< Option is to display help on console.
        , CMD_Load          //!< Option is to start the process and load configuration from specified file
        , CMD_Verbose       //!< Option is to display the data rate when execute process as console application.
        , CMD_Uninstall     //!< Option is to uninstall (unregister) service in the system.
        , CMD_Install       //!< Option is to install (register) service in the system.
        , CMD_Service       //!< Option is to execute process as a system service (in background).
        , CMD_Console       //!< Option is to execute process as console application.

        , CMD_Custom        //!< No option, reserved for the custom options.
    };

    /**
     * \brief   Converts and returns string value of NESystemService::ServiceOption type.
     **/
    inline const char * as_string( NESystemService::ServiceOption cmdService );

    /**
     * \brief   The default setup for the system service executable options.
     **/
    const OptionParser::OptionSetup ServiceOptionSetup[ ]
    {
          //!< Default command.
          { ""  , ""            , static_cast<int32_t>(ServiceOption::CMD_Console)     , OptionParser::NO_DATA         , {}, {}, {} }
          //!< Command to run process as a console application.
        , { "-c", "--console"   , static_cast<int32_t>(ServiceOption::CMD_Console)     , OptionParser::NO_DATA         , {}, {}, {} }
          //!< Command to display help on console.
        , { "-h", "--help"      , static_cast<int32_t>(ServiceOption::CMD_Help)        , OptionParser::NO_DATA         , {}, {}, {} }
          //!< Command to display the error message.
        , { "-l", "--load"      , static_cast<int32_t>(ServiceOption::CMD_Load)        , OptionParser::STRING_NO_RANGE , {}, {}, {} }
          //!< Command to install service. Valid for Windows OS, ignored in other cases.
        , { "-i", "--install"   , static_cast<int32_t>(ServiceOption::CMD_Install)     , OptionParser::NO_DATA         , {}, {}, {} }
          //!< Command to run process as a system service process.
        , { "-s", "--service"   , static_cast<int32_t>(ServiceOption::CMD_Service)     , OptionParser::NO_DATA         , {}, {}, {} }
          //!< Command to uninstall service. Valid for Windows OS, ignored in other cases.
        , { "-u", "--uninstall" , static_cast<int32_t>(ServiceOption::CMD_Uninstall)  , OptionParser::NO_DATA          , {}, {}, {} }
          //!< Command to display data rate when run as console application.
        , { "-v", "--verbose"   , static_cast<int32_t>(ServiceOption::CMD_Verbose)     , OptionParser::NO_DATA         , {}, {}, {} }
    };

    /**
     * \brief   NESystemService::ServicePhase
     *          Describes the system service state.
     **/
    enum class ServicePhase : int32_t
    {
          Stopped       //!< Service is stopped.
        , Starting      //!< Service is in starting process.
        , Stopping      //!< Service is in stopping process.
        , Running       //!< Service is up and running.
        , Continuing    //!< Service was paused and now is continuing.
        , Pausing       //!< Service is in pausing state.
        , Paused        //!< Service is paused.
    };

    /**
     * \brief   Returns the human readable string of NESystemService::ServicePhase value
     **/
    inline const char * as_string( NESystemService::ServicePhase serviceState );

    /**
     * \brief   The default option to run Multitarget System as a console application.
     **/
    constexpr ServiceOption   DEFAULT_OPTION   { ServiceOption::CMD_Console };

    /**
     * \brief   The default option to run the Log Collector in verbose mode to output send and receive data rate.
     */
    constexpr bool              DEFAULT_VERBOSE { true };

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
    constexpr std::string_view  FORMAT_WAIT_QUIT{ "Type \'-q\' or \'--quit\' to quit the application ...: " };
    /**
     * \brief   Message quit application.
     **/
    constexpr std::string_view  FORMAT_QUIT_APP { "\nQuit the application ...\n" };

    /**
     * \brief   Underscore to output under the application title.
     **/
    constexpr std::string_view  MSG_SEPARATOR   { "---------------------------------------------------------------------------------------------" };

    /**
     * \brief   Coordinate to display the application title.
     **/
    constexpr Console::Coord    COORD_TITLE         { 0, 1 };
    /**
     * \brief   Coordinate to display the underscore.
     **/
    constexpr Console::Coord    COORD_SUBTITLE      { 0, 2 };

    /**
     * \brief   Coordinate to start to display send data rate message.
     **/
    constexpr Console::Coord    COORD_SEND_RATE     { 0, 3 };
    /**
     * \brief   Coordinate to start to display receive data rate message.
     **/
    constexpr Console::Coord    COORD_RECV_RATE     { 0, 4 };
    /**
     * \brief   Coordinate to display the status message.
     **/
    constexpr Console::Coord    COORD_STATUS_MSG    { 0, 5 };
    /**
     * \brief   Coordinate to start to display user input message.
     **/
    constexpr Console::Coord    COORD_USER_INPUT    { 0, 6 };
    /**
     * \brief   Coordinate to start to display error message.
     **/
    constexpr Console::Coord    COORD_ERROR_MSG     { 0, 7 };
    /**
     * \brief   Coordinate to start to display information message.
     **/
    constexpr Console::Coord    COORD_INFO_MSG      { 0, 8 };

    /**
     * \brief   Converts the argument list from 'char' or 'wchar_t' type to the 'char'.
     *          The memory of returned value should be freed by calling 'deleteArgument'.
     * \tparam  CharType    The type of the string. Should be 'char' or 'wchar_t'.
     * \param   argv        The list of arguments received in 'main' function.
     * \param   argc        The number of arguments in the argument list.
     * \returns Returns argument list of type 'char'
     **/
    template<typename CharType>
    inline char** convertArguments(CharType** argv, int32_t argc);

    /**
     * \brief   Deletes the list of arguments returned by 'convertArguments' function.
     * \param   argv        The list of arguments returned by 'convertArguments' function.
     * \param   argc        The number of arguments in the argument list.
     **/
    inline void deleteArguments(char** argv, int32_t argc);
}

//////////////////////////////////////////////////////////////////////////
// NESystemService namespace inline functions
//////////////////////////////////////////////////////////////////////////

inline const char * NESystemService::as_string( NESystemService::ServiceOption cmdService )
{
    switch ( cmdService )
    {
    case NESystemService::ServiceOption::CMD_Undefined:
        return "NESystemService::CMD_Undefined";
    case NESystemService::ServiceOption::CMD_Console:
        return "NESystemService::CMD_Console";
    case NESystemService::ServiceOption::CMD_Help:
        return "NESystemService::CMD_Help";
    case NESystemService::ServiceOption::CMD_Load:
        return "NESystemService::CMD_Load";
    case NESystemService::ServiceOption::CMD_Install:
        return "NESystemService::CMD_Install";
    case NESystemService::ServiceOption::CMD_Service:
        return "NESystemService::CMD_Service";
    case NESystemService::ServiceOption::CMD_Uninstall:
        return "NESystemService::ServiceOption::CMD_Uninstall";
    case NESystemService::ServiceOption::CMD_Verbose:
        return "NESystemService::ServiceOption::CMD_Verbose";
    case NESystemService::ServiceOption::CMD_Custom:
        return "NESystemService::ServiceOption::CMD_Custom";
    default:
        ASSERT( false );
        return "ERR: Unexpected NESystemService::ServiceOption value!";
    }
}

inline const char * NESystemService::as_string( NESystemService::ServicePhase serviceState )
{
    switch ( serviceState )
    {
    case NESystemService::ServicePhase::Stopped:
        return "NESystemService::ServiceStopped";
    case NESystemService::ServicePhase::Starting:
        return "NESystemService::ServiceStarting";
    case NESystemService::ServicePhase::Stopping:
        return "NESystemService::ServiceStopping";
    case NESystemService::ServicePhase::Running:
        return "NESystemService::ServiceRunning";
    case NESystemService::ServicePhase::Continuing:
        return "NESystemService::ServiceContinuing";
    case NESystemService::ServicePhase::Pausing:
        return "NESystemService::ServicePausing";
    case NESystemService::ServicePhase::Paused:
        return "NESystemService::ServicePaused";
    default:
        ASSERT( false );
        return "ERR: Undefined NESystemService::ServicePhase value!!!";
    }
}

template<typename CharType>
inline char** NESystemService::convertArguments(CharType** argv, int32_t argc)
{
    char** result = argc != 0 ? DEBUG_NEW char* [static_cast<uint32_t>(argc)] : nullptr;
    if (result != nullptr)
    {
        for (uint32_t i = 0; i < static_cast<uint32_t>(argc); ++i)
        {
            CharType* entry = argv[i];
            uint32_t length = static_cast<uint32_t>(NEString::string_length<CharType>(entry));
            uint32_t size = length + 1u;
            char* arg = DEBUG_NEW char[size];
            NEString::copy_string<char, CharType>(arg, static_cast<NEString::CharCount>(size), entry);
            result[i] = arg;
        }
    }

    return result;
}

inline void NESystemService::deleteArguments(char** argv, int32_t argc)
{
    if (argv != nullptr)
    {
        for (int i = 0; i < argc; ++i)
        {
            delete[] argv[i];
        }

        delete[] argv;
    }
}

#endif  // AREG_AREGEXTEND_SERVICE_SYSTEMSERVICEDEFS_HPP
