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
#include "areg/base/areg_global.h"

#include "areg/base/StringDefs.hpp"
#include "aregextend/console/Console.hpp"
#include "aregextend/console/OptionParser.hpp"

/**
 * \brief   The system service common data.
 **/
namespace areg::ext {

    /**
     * \brief   areg::ext::ServiceOption
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
     * \brief   Converts and returns string value of areg::ext::ServiceOption type.
     **/
    [[nodiscard]]
    inline constexpr const char * as_string( areg::ext::ServiceOption cmdService ) noexcept;

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
     * \brief   areg::ext::ServicePhase
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
     * \brief   Returns the human readable string of areg::ext::ServicePhase value.
     **/
    [[nodiscard]]
    inline constexpr const char * as_string( areg::ext::ServicePhase serviceState ) noexcept;

    /**
     * \brief   The default option to run Multitarget System as a console application.
     **/
    constexpr ServiceOption     DEFAULT_OPTION  { ServiceOption::CMD_Console };

    /**
     * \brief   Default number of pool thread pairs created by ServiceCommunicationBase
     *          when pool mode is enabled.  Each pair has one dedicated send thread and
     *          one dedicated receive thread.  Clients are routed to pairs via
     *          \c cookie % DEFAULT_COMMUNICATION_PAIR_COUNT.
     *          Pass \c 0 to \c ServiceCommunicationBase to keep the legacy shared-thread
     *          path (used by logcollector which handles low volumes of data).
     **/
    inline constexpr uint32_t   DEFAULT_COMMUNICATION_PAIR_COUNT { 0u };

    /**
     * \brief   The default option to run the Log Collector in verbose mode to output send and receive data rate.
     */
    constexpr bool              DEFAULT_VERBOSE { true };

    /**
     * \brief   Output send data rate message format.
     **/
    constexpr std::string_view  FORMAT_SEND_DATA{ " Send data rate ...: %9.2f %s" };
    /**
     * \brief   Output receive data rate message format.
     **/
    constexpr std::string_view  FORMAT_RECV_DATA{ " Recv data rate ...: %9.2f %s" };
    /**
     * \brief   Output send message rate format.
     **/
    constexpr std::string_view  FORMAT_SEND_MSGS{ " Send messages ....: %9u msgs/sec" };
    /**
     * \brief   Output receive message rate format.
     **/
    constexpr std::string_view  FORMAT_RECV_MSGS{ " Recv messages ....: %9u msgs/sec" };
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
    constexpr Console::Coord    COORD_TITLE         { 1, 1 };
    /**
     * \brief   Coordinate to display the underscore.
     **/
    constexpr Console::Coord    COORD_SUBTITLE      { 1, 2 };

    /**
     * \brief   Coordinate to start to display send data rate message.
     **/
    constexpr Console::Coord    COORD_SEND_RATE     { 1, 3 };
    /**
     * \brief   Coordinate to start to display receive data rate message.
     **/
    constexpr Console::Coord    COORD_RECV_RATE     { 1, 4 };
    /**
     * \brief   Coordinate to start to display send message rate.
     **/
    constexpr Console::Coord    COORD_SEND_MSGS     { 1, 5 };
    /**
     * \brief   Coordinate to start to display receive message rate.
     **/
    constexpr Console::Coord    COORD_RECV_MSGS     { 1, 6 };
    /**
     * \brief   Coordinate to display the status message.
     **/
    constexpr Console::Coord    COORD_STATUS_MSG    { 1, 7 };
    /**
     * \brief   Coordinate to start to display user input message.
     **/
    constexpr Console::Coord    COORD_USER_INPUT    { 1, 8 };
    /**
     * \brief   Coordinate to start to display error message.
     **/
    constexpr Console::Coord    COORD_ERROR_MSG     { 1, 9 };
    /**
     * \brief   Coordinate to start to display information message.
     **/
    constexpr Console::Coord    COORD_INFO_MSG      { 1, 10 };

    /**
     * \brief   Converts the argument list from 'char' or 'wchar_t' type to the 'char'. The memory
     *          of returned value should be freed by calling 'deleteArgument'.
     *
     * \param   argv    The list of arguments received in 'main' function.
     * \param   argc    The number of arguments in the argument list.
     * \return  Returns argument list of type 'char'
     **/
    template<typename CharType>
    inline char** convert_arguments(CharType** argv, int32_t argc);

    /**
     * \brief   Deletes the list of arguments returned by 'convert_arguments' function.
     *
     * \param   argv    The list of arguments returned by 'convert_arguments' function.
     * \param   argc    The number of arguments in the argument list.
     **/
    inline void delete_arguments(char** argv, int32_t argc);

//////////////////////////////////////////////////////////////////////////
// areg::ext namespace inline functions
//////////////////////////////////////////////////////////////////////////

inline constexpr const char* as_string( areg::ext::ServiceOption cmdService ) noexcept
{
    switch ( cmdService )
    {
    case areg::ext::ServiceOption::CMD_Undefined:
        return "areg::ext::CMD_Undefined";
    case areg::ext::ServiceOption::CMD_Console:
        return "areg::ext::CMD_Console";
    case areg::ext::ServiceOption::CMD_Help:
        return "areg::ext::CMD_Help";
    case areg::ext::ServiceOption::CMD_Load:
        return "areg::ext::CMD_Load";
    case areg::ext::ServiceOption::CMD_Install:
        return "areg::ext::CMD_Install";
    case areg::ext::ServiceOption::CMD_Service:
        return "areg::ext::CMD_Service";
    case areg::ext::ServiceOption::CMD_Uninstall:
        return "areg::ext::ServiceOption::CMD_Uninstall";
    case areg::ext::ServiceOption::CMD_Verbose:
        return "areg::ext::ServiceOption::CMD_Verbose";
    case areg::ext::ServiceOption::CMD_Custom:
        return "areg::ext::ServiceOption::CMD_Custom";
    default:
        ASSERT( false );
        return "ERR: Unexpected areg::ext::ServiceOption value!";
    }
}

inline constexpr const char * as_string( areg::ext::ServicePhase serviceState ) noexcept
{
    switch ( serviceState )
    {
    case areg::ext::ServicePhase::Stopped:
        return "areg::ext::ServiceStopped";
    case areg::ext::ServicePhase::Starting:
        return "areg::ext::ServiceStarting";
    case areg::ext::ServicePhase::Stopping:
        return "areg::ext::ServiceStopping";
    case areg::ext::ServicePhase::Running:
        return "areg::ext::ServiceRunning";
    case areg::ext::ServicePhase::Continuing:
        return "areg::ext::ServiceContinuing";
    case areg::ext::ServicePhase::Pausing:
        return "areg::ext::ServicePausing";
    case areg::ext::ServicePhase::Paused:
        return "areg::ext::ServicePaused";
    default:
        ASSERT( false );
        return "ERR: Undefined areg::ext::ServicePhase value!!!";
    }
}

template<typename CharType>
inline char** convert_arguments(CharType** argv, int32_t argc)
{
    char** result = argc != 0 ? DEBUG_NEW char* [static_cast<uint32_t>(argc)] : nullptr;
    if (result != nullptr)
    {
        for (uint32_t i = 0; i < static_cast<uint32_t>(argc); ++i)
        {
            CharType* entry = argv[i];
            uint32_t length = static_cast<uint32_t>(areg::string_length<CharType>(entry));
            uint32_t size = length + 1u;
            char* arg = DEBUG_NEW char[size];
            areg::copy_string<char, CharType>(arg, static_cast<areg::CharCount>(size), entry);
            result[i] = arg;
        }
    }

    return result;
}

inline void delete_arguments(char** argv, int32_t argc)
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

} // namespace areg::ext

#endif  // AREG_AREGEXTEND_SERVICE_SYSTEMSERVICEDEFS_HPP
