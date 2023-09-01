#ifndef PUBSUBMIX_COMMON_SRC_NECOMMON_HPP
#define PUBSUBMIX_COMMON_SRC_NECOMMON_HPP

/************************************************************************
 * \file        common/src/NECommon.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file is a common part to share between projects.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "areg/base/GEGlobal.h"

#include "extend/console/Console.hpp"
#include <string_view>

/**
 * \brief   List of service names of the same remote and one local service interface that is present in every process.
 *          The agenda is to demonstrate uniqueness of services:
 *              - Remote services should be unique in the entire network.
 *              - Local services should be unique in the process.
 *            
 **/
namespace NECommon
{
    //!< The main service that is responsible to shutdown the complete system.
    constexpr char  ContollerPublisher[]        { "ContollerPublisher" };

    //!< The prefix of any other second PubSub service.
    constexpr char PublisherSecond[]            { "PublisherSecondary" };

    //!< The prefix of any other third PubSub service.
    constexpr char PublisherThird[]             { "PublisherThird" };

    //!< The separation string
    constexpr std::string_view  Separator       { "--------------------------------------------" };
    //!< Prompt to input command
    constexpr std::string_view  UserInput       { "Enter command {i, p, s, q} or \'h\' for help: "};
    //!< Format to output error message
    constexpr std::string_view  FormatError     { "Wrong command [ %s ] entered, type \'h\' for help"};
    //!< String to output for invalid parameters
    constexpr std::string_view  Invalid         { "INVALID" };

    //!< Format to output when integer changed
    constexpr std::string_view  FormatInteger   { "%s named integer update (Always) .: %s => %s { %s }"};
    //!< Format to output when string changed
    constexpr std::string_view  FormatString    { "%s named string update (OnChange) : %s => %s { %s }"};

    //!< Text message when connected.
    constexpr std::string_view  TxtConnected    { "Connected to Publisher service" };
    //!< Formant of message to display when disconnected.
    constexpr std::string_view  FormatDisconnect{ "Connected to Publisher service with status [ %s ]" };

    //!< The coordinate in console to output application title
    constexpr Console::Coord    CoordTitle      { 0, 1 };
    //!< The coordinate in console to output application subtitle (separator)
    constexpr Console::Coord    CoordSubtitle   { 0, 2 };
    //!< The coordinate in console to output user input prompt
    constexpr Console::Coord    CoordUserInput  { 0, 3 };
    //!< The coordinate in console to output separator
    constexpr Console::Coord    CoordSeparate   { 0, 4 };

    //!< The coordinate in console to display connection status
    constexpr Console::Coord    CoordStatus    { 0, 5 };
    //!< The coordinate in console to display integer update value
    constexpr Console::Coord    CoordInteger   { 0, 6 };
    //!< The coordinate in console to display string update value
    constexpr Console::Coord    CoordString    { 0, 7 };
    //!< The coordinate in console to display a separator
    constexpr Console::Coord    CoordSeparator { 0, 8 };

    //!< The coordinate in console to display an information.
    extern Console::Coord       CoordInfoMsg;
}

#endif // PUBSUBMIX_COMMON_SRC_NECOMMON_HPP
