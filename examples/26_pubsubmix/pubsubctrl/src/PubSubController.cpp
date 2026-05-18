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
#include "areg/logging/areg_log.h"
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

const areg::ext::OptionParser::OptionSetup PubSubController::ValidOptions[]
{
      {"i", "invalid", static_cast<int32_t>(OptionFlag::CMD_Invalidate)  , areg::ext::OptionParser::NO_DATA , {}, {}, {} }
    , {"p", "pause"  , static_cast<int32_t>(OptionFlag::CMD_Pause)       , areg::ext::OptionParser::NO_DATA , {}, {}, {} }
    , {"s", "start"  , static_cast<int32_t>(OptionFlag::CMD_Start)       , areg::ext::OptionParser::NO_DATA , {}, {}, {} }
    , {"q", "quit"   , static_cast<int32_t>(OptionFlag::CMD_Quit)        , areg::ext::OptionParser::NO_DATA , {}, {}, {} }
    , {"h", "help"   , static_cast<int32_t>(OptionFlag::CMD_Help)        , areg::ext::OptionParser::NO_DATA , {}, {}, {} }
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

void PubSubController::startup_component(areg::ComponentThread & comThread)
{
    areg::Component::startup_component(comThread);
    mConsoleThread.start(areg::WAIT_INFINITE);
}

void PubSubController::shutdown_component(areg::ComponentThread & comThread)
{
    mConsoleThread.shutdown(areg::WAIT_INFINITE);
    areg::Component::shutdown_component(comThread);
}

void PubSubController::on_run()
{
    areg::ext::Console & console = areg::ext::Console::instance();
    areg::ext::OptionParser parser(ValidOptions, std::size(ValidOptions));
    console.lock_console();
    console.enable_console_input(true);
    print_message(areg::String::EmptyString, OptionFlag::CMD_Undefined);
    console.unlock_console();

    OptionFlag cmd = OptionFlag::CMD_Undefined;

    do
    {
        areg::String message;
        areg::String usrInput = console.read_string();
        console.lock_console();

        if (parser.parse_option_line(usrInput.as_string()))
        {
            const areg::ext::OptionParser::InputOptionList & opts = parser.options();
            cmd = opts.size() == 1u ? static_cast<OptionFlag>(opts[0u].inCommand) : OptionFlag::CMD_Error;
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
                message.format(pubsub::FormatError.data(), usrInput.as_string());
                break;
            }
        }
        else
        {
            cmd = PubSubController::OptionFlag::CMD_Error;
            message.format(pubsub::FormatError.data(), usrInput.as_string());
        }

        print_message(message, cmd);
        console.unlock_console();

    } while (cmd != OptionFlag::CMD_Quit);
}


inline void PubSubController::print_message(const areg::String & message, OptionFlag cmd)
{
    areg::ext::Console & console = areg::ext::Console::instance();
    if (cmd == OptionFlag::CMD_Error)
    {
        console.output_str(pubsub::CoordInfoMsg, message);
    }
    else if (cmd == OptionFlag::CMD_Help)
    {
        console.output_str(pubsub::CoordInfoMsg, _help);
    }
    else if (cmd != OptionFlag::CMD_Undefined)
    {
        console.output_msg(pubsub::CoordInfoMsg, message);
    }

    console.output_str(pubsub::CoordSeparate, pubsub::Separator);
    console.output_str(pubsub::CoordUserInput, pubsub::UserInput);
    console.refresh_screen();
}

inline PubSubController & PubSubController::self()
{
    return (*this);
}
