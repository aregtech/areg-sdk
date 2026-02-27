/************************************************************************
 * \file        pubsubctrl/src/PubSubController.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This file is demonstration of data PubSub (Publish / Subscribe) features.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/

#include "pubsubctrl/src/PubSubController.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/logging/GELog.h"
#include "aregextend/console/Console.hpp"

#include "common/src/PubSubDefs.hpp"

//////////////////////////////////////////////////////////////////////////
// Log scopes
//////////////////////////////////////////////////////////////////////////

namespace
{
    constexpr std::string_view  _help       
    { 
        "Type one this commands to handle PubSub feature service:\n"
        "\'i\' or \'invalid\'   - to invalidate all data;\n"
        "\'p\' or \'pause\'     - to pause the data updates;\n"
        "\'s\' or \'start\'     - to start / restart the data updates;\n"
        "\'q\' or \'quit\'      - to quit the application;\n"
        "\'h\' or \'help\'      - to print this help.\n"
    };

}

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////

const aregext::OptionParser::OptionSetup PubSubController::ValidOptions[]
{
      {"i", "invalid", static_cast<int32_t>(OptionFlag::CMD_Invalidate)  , aregext::OptionParser::NO_DATA , {}, {}, {} }
    , {"p", "pause"  , static_cast<int32_t>(OptionFlag::CMD_Pause)       , aregext::OptionParser::NO_DATA , {}, {}, {} }
    , {"s", "start"  , static_cast<int32_t>(OptionFlag::CMD_Start)       , aregext::OptionParser::NO_DATA , {}, {}, {} }
    , {"q", "quit"   , static_cast<int32_t>(OptionFlag::CMD_Quit)        , aregext::OptionParser::NO_DATA , {}, {}, {} }
    , {"h", "help"   , static_cast<int32_t>(OptionFlag::CMD_Help)        , aregext::OptionParser::NO_DATA , {}, {}, {} }
};

//////////////////////////////////////////////////////////////////////////
// PubSubController class methods
//////////////////////////////////////////////////////////////////////////

PubSubController::PubSubController( const areg::ComponentEntry & entry, areg::ComponentThread & owner )
    : areg::Component         ( entry, owner )
    , Publisher         ( static_cast<areg::Component &>(self()) )
    , areg::ThreadConsumer  ( )

    , mSubscriber       ( entry.mDependencyServices[0], static_cast<areg::Component &>(self()), 0 )

    , mConsoleThread    (static_cast<areg::ThreadConsumer &>(self()), entry.mRoleName + "_Thread")
{
}

void PubSubController::startupComponent(areg::ComponentThread & comThread)
{
    areg::Component::startupComponent(comThread);
    mConsoleThread.createThread(areg::WAIT_INFINITE);
}

void PubSubController::shutdownComponent(areg::ComponentThread & comThread)
{
    mConsoleThread.shutdownThread(areg::WAIT_INFINITE);
    areg::Component::shutdownComponent(comThread);
}

void PubSubController::onThreadRuns()
{
    Console & console = Console::getInstance();
    aregext::OptionParser parser(ValidOptions, std::size(ValidOptions));
    console.lockConsole();
    console.enableConsoleInput(true);
    printMessage(areg::String::EmptyString, OptionFlag::CMD_Undefined);
    console.unlockConsole();

    OptionFlag cmd = OptionFlag::CMD_Undefined;

    do
    {
        areg::String message;
        areg::String usrInput = console.readString();
        console.lockConsole();

        if (parser.parseOptionLine(usrInput.getString()))
        {
            const aregext::OptionParser::InputOptionList & opts = parser.getOptions();
            cmd = opts.getSize() == 1u ? static_cast<OptionFlag>(opts[0u].inCommand) : OptionFlag::CMD_Error;
            switch (cmd)
            {
            case PubSubController::OptionFlag::CMD_Invalidate:
                invalidate();
                message = "Requested to invalidate data...";
                break;

            case PubSubController::OptionFlag::CMD_Pause:
                stop();
                message = "Requested to pause service...";
                break;

            case PubSubController::OptionFlag::CMD_Start:
                start();
                message = "Requested to start service...";
                break;

            case PubSubController::OptionFlag::CMD_Quit:
                quit();
                message = "Request to quit application...";
                break;

            case PubSubController::OptionFlag::CMD_Help:
                break;

            case PubSubController::OptionFlag::CMD_Undefined:
            case PubSubController::OptionFlag::CMD_Error:
            default:
                cmd = PubSubController::OptionFlag::CMD_Error;
                message.format(pubsub::FormatError.data(), usrInput.getString());
                break;
            }
        }
        else
        {
            cmd = PubSubController::OptionFlag::CMD_Error;
            message.format(pubsub::FormatError.data(), usrInput.getString());
        }

        printMessage(message, cmd);
        console.unlockConsole();

    } while (cmd != OptionFlag::CMD_Quit);
}


inline void PubSubController::printMessage(const areg::String & message, OptionFlag cmd)
{
    Console & console = Console::getInstance();
    if (cmd == OptionFlag::CMD_Error)
    {
        console.outputStr(pubsub::CoordInfoMsg, message);
    }
    else if (cmd == OptionFlag::CMD_Help)
    {
        console.outputStr(pubsub::CoordInfoMsg, _help);
    }
    else if (cmd != OptionFlag::CMD_Undefined)
    {
        console.outputMsg(pubsub::CoordInfoMsg, message);
    }

    console.outputStr(pubsub::CoordSeparate, pubsub::Separator);
    console.outputStr(pubsub::CoordUserInput, pubsub::UserInput);
    console.refreshScreen();
}

inline PubSubController & PubSubController::self()
{
    return (*this);
}
