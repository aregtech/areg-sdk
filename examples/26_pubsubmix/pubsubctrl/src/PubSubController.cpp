/************************************************************************
 * \file        pubsubctrl/src/PubSubController.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
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

#include "common/src/NECommon.hpp"

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

const OptionParser::sOptionSetup PubSubController::ValidOptions[]
{
      {"i", "invalid", static_cast<int>(eCommands::CMD_Invalidate)  , OptionParser::NO_DATA , {}, {}, {} }
    , {"p", "pause"  , static_cast<int>(eCommands::CMD_Pause)       , OptionParser::NO_DATA , {}, {}, {} }
    , {"s", "start"  , static_cast<int>(eCommands::CMD_Start)       , OptionParser::NO_DATA , {}, {}, {} }
    , {"q", "quit"   , static_cast<int>(eCommands::CMD_Quit)        , OptionParser::NO_DATA , {}, {}, {} }
    , {"h", "help"   , static_cast<int>(eCommands::CMD_Help)        , OptionParser::NO_DATA , {}, {}, {} }
};

//////////////////////////////////////////////////////////////////////////
// PubSubController class methods
//////////////////////////////////////////////////////////////////////////

PubSubController::PubSubController( const NERegistry::ComponentEntry & entry, ComponentThread & owner )
    : Component         ( entry, owner )
    , Publisher         ( static_cast<Component &>(self()) )
    , IEThreadConsumer  ( )

    , mSubscriber       ( entry.mDependencyServices[0], static_cast<Component &>(self()), 0 )

    , mConsoleThread    (static_cast<IEThreadConsumer &>(self()), entry.mRoleName + "_Thread")
{
}

void PubSubController::startupComponent(ComponentThread & comThread)
{
    Component::startupComponent(comThread);
    mConsoleThread.createThread(NECommon::WAIT_INFINITE);
}

void PubSubController::shutdownComponent(ComponentThread & comThread)
{
    mConsoleThread.shutdownThread(NECommon::WAIT_INFINITE);
    Component::shutdownComponent(comThread);
}

void PubSubController::onThreadRuns(void)
{
    Console & console = Console::getInstance();
    OptionParser parser(ValidOptions, MACRO_ARRAYLEN(ValidOptions));
    console.lockConsole();
    console.enableConsoleInput(true);
    printMessage(String::EmptyString, eCommands::CMD_Undefined);
    console.unlockConsole();

    eCommands cmd = eCommands::CMD_Undefined;

    do
    {
        String message;
        String usrInput = console.readString();
        console.lockConsole();

        if (parser.parseOptionLine(usrInput.getString()))
        {
            const OptionParser::InputOptionList & opts = parser.getOptions();
            cmd = opts.getSize() == 1u ? static_cast<eCommands>(opts[0u].inCommand) : eCommands::CMD_Error;
            switch (cmd)
            {
            case PubSubController::eCommands::CMD_Invalidate:
                invalidate();
                message = "Requested to invalidate data...";
                break;

            case PubSubController::eCommands::CMD_Pause:
                stop();
                message = "Requested to pause service...";
                break;

            case PubSubController::eCommands::CMD_Start:
                start();
                message = "Requested to start service...";
                break;

            case PubSubController::eCommands::CMD_Quit:
                quit();
                message = "Request to quit application...";
                break;

            case PubSubController::eCommands::CMD_Help:
                break;

            case PubSubController::eCommands::CMD_Undefined:
            case PubSubController::eCommands::CMD_Error:
            default:
                cmd = PubSubController::eCommands::CMD_Error;
                message.format(NECommon::FormatError.data(), usrInput.getString());
                break;
            }
        }
        else
        {
            cmd = PubSubController::eCommands::CMD_Error;
            message.format(NECommon::FormatError.data(), usrInput.getString());
        }

        printMessage(message, cmd);
        console.unlockConsole();

    } while (cmd != eCommands::CMD_Quit);
}


inline void PubSubController::printMessage(const String & message, eCommands cmd)
{
    Console & console = Console::getInstance();
    if (cmd == eCommands::CMD_Error)
    {
        console.outputStr(NECommon::CoordInfoMsg, message);
    }
    else if (cmd == eCommands::CMD_Help)
    {
        console.outputStr(NECommon::CoordInfoMsg, _help);
    }
    else if (cmd != eCommands::CMD_Undefined)
    {
        console.outputMsg(NECommon::CoordInfoMsg, message);
    }

    console.outputStr(NECommon::CoordSeparate, NECommon::Separator);
    console.outputStr(NECommon::CoordUserInput, NECommon::UserInput);
    console.refreshScreen();
}

inline PubSubController & PubSubController::self(void)
{
    return (*this);
}
