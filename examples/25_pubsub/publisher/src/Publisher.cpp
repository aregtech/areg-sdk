/************************************************************************
 * \file        publisher/src/Publisher.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This file is demonstration of data PubSub (Publish / Subscribe) features.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/

#include "publisher/src/Publisher.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/logging/areg_log.h"
#include "aregextend/console/Console.hpp"

//////////////////////////////////////////////////////////////////////////
// Log scopes
//////////////////////////////////////////////////////////////////////////

DEF_LOG_SCOPE(examples_25_publisher_Publisher, consumer_connected);
DEF_LOG_SCOPE(examples_25_publisher_Publisher, start);
DEF_LOG_SCOPE(examples_25_publisher_Publisher, stop);
DEF_LOG_SCOPE(examples_25_publisher_Publisher, invalidate);
DEF_LOG_SCOPE(examples_25_publisher_Publisher, quit);
DEF_LOG_SCOPE(examples_25_publisher_Publisher, process_timer);


namespace
{
    areg::String generateString(uint32_t seqNr)
    {
        areg::String result;
        return result.format("string_%u", seqNr);
    }

    constexpr std::string_view  _title      { "PubSub features, Publisher part ..."};
    constexpr std::string_view  _separator  { "--------------------------------------------" };
    constexpr std::string_view  _userInput  { "Enter command {i, p, s, q} or \'h\' for help: "};
    constexpr std::string_view  _fmtError   { "Wrong command [ %s ] entered, type \'h\' for help"};
    constexpr std::string_view  _help       
    { 
        "Type one this commands to handle PubSub feature service:\n"
        "\'i\' or \'invalid\'   - to invalidate all data;\n"
        "\'p\' or \'pause\'     - to pause the data updates;\n"
        "\'s\' or \'start\'     - to start / restart the data updates;\n"
        "\'q\' or \'quit\'      - to quit the application;\n"
        "\'h\' or \'help\'      - to print this help.\n"
    };

    constexpr areg::ext::Console::Coord    _coordTitle     { 0, 1 };
    constexpr areg::ext::Console::Coord    _coordSubtitle  { 0, 2 };
    constexpr areg::ext::Console::Coord    _coordUserInput { 0, 4 };
    constexpr areg::ext::Console::Coord    _coordSeparate  { 0, 6 };
    constexpr areg::ext::Console::Coord    _coordErrorMsg  { 0, 7 };
    constexpr areg::ext::Console::Coord    _coordInfoMsg   { 0, 8 };

}

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////

const areg::ext::OptionParser::OptionSetup Publisher::ValidOptions[]
{
      {"i", "invalid", static_cast<int32_t>(OptionFlag::CMD_Invalidate)  , areg::ext::OptionParser::NO_DATA , {}, {}, {} }
    , {"p", "pause"  , static_cast<int32_t>(OptionFlag::CMD_Pause)       , areg::ext::OptionParser::NO_DATA , {}, {}, {} }
    , {"s", "start"  , static_cast<int32_t>(OptionFlag::CMD_Start)       , areg::ext::OptionParser::NO_DATA , {}, {}, {} }
    , {"q", "quit"   , static_cast<int32_t>(OptionFlag::CMD_Quit)        , areg::ext::OptionParser::NO_DATA , {}, {}, {} }
    , {"h", "help"   , static_cast<int32_t>(OptionFlag::CMD_Help)        , areg::ext::OptionParser::NO_DATA , {}, {}, {} }
};

//////////////////////////////////////////////////////////////////////////
// Publisher class methods
//////////////////////////////////////////////////////////////////////////

Publisher::Publisher( const areg::ComponentEntry & entry, areg::ComponentThread & owner )
    : areg::Component         ( entry, owner )
    , PubSubProviderBase        ( static_cast<areg::Component &>(self()) )
    , areg::TimerConsumer   ( )
    , areg::ThreadConsumer  ( )

    , mTimerOnChange    (static_cast<areg::TimerConsumer &>(self()), entry.mRoleName + "_OnUpdateTimer")
    , mTimerAlways      (static_cast<areg::TimerConsumer &>(self()), entry.mRoleName + "_AlwaysTimer")
    , mClientCount      (0)

    , mSeqString        (0)
    , mCountString      (0)

    , mSeqInteger       (0)
    , mCountInteger     (0)

    , mConsoleThread    (static_cast<areg::ThreadConsumer &>(self()), entry.mRoleName + "_Thread")
    , mLock             (false)
{
}

void Publisher::startup_component(areg::ComponentThread & comThread)
{
    areg::Component::startup_component(comThread);
    mConsoleThread.start(areg::WAIT_INFINITE);
}

void Publisher::shutdown_component(areg::ComponentThread & comThread)
{
    mConsoleThread.shutdown(areg::WAIT_INFINITE);
    areg::Component::shutdown_component(comThread);
}

bool Publisher::consumer_connected(const areg::ProxyAddress & client, areg::ServiceConnectionState status)
{
    LOG_SCOPE( examples_25_publisher_Publisher, consumer_connected);
    bool result = PubSubProviderBase::consumer_connected(client, status);

    LOG_DBG("Connection status [ %s ] of the consumer [ %s ]", areg::as_string(status), areg::ProxyAddress::to_path(client).as_string());
    mClientCount += (areg::is_service_connected(status) ? 1 : -1);
    LOG_DBG("There are [ %d ] connected service consumers", mClientCount);

    if (!is_service_provider_state_valid())
    {
        set_service_provider_state(PubSub::RunState::Uninitialized);
    }

    return result;
}

void Publisher::start()
{
    LOG_SCOPE( examples_25_publisher_Publisher, start );

    areg::Lock lock(mLock);
    LOG_DBG("Requested to re-start the service run. Reset values and re-start timers, there are [ %d ] connected clients",  mClientCount);

    mTimerAlways.stop_timer();
    mTimerOnChange.stop_timer();

    set_service_provider_state(PubSub::RunState::Running);

    if (is_integer_always_valid() == false)
    {
        mSeqInteger = 0;
        set_integer_always(mSeqInteger);
    }

    if (is_string_on_change_valid() == false)
    {
        mSeqString = 0;
        set_string_on_change(generateString(mSeqString));
    }

    mTimerAlways.start_timer(PubSub::TimeoutAlways, component_thread(), areg::Timer::CONTINUOUSLY);
    mTimerOnChange.start_timer(PubSub::TimeoutOnChange, component_thread(), areg::Timer::CONTINUOUSLY);
}

void Publisher::stop()
{
    LOG_SCOPE( examples_25_publisher_Publisher, stop );

    areg::Lock lock(mLock);
    LOG_DBG("Stopped servicing, resets data, wait for further instructions. There are [ %d ] connected clients", mClientCount);

    mTimerAlways.stop_timer();
    mTimerOnChange.stop_timer();

    set_service_provider_state(PubSub::RunState::Stopped);
}

void Publisher::invalidate()
{
    LOG_SCOPE( examples_25_publisher_Publisher, invalidate );

    areg::Lock lock(mLock);
    LOG_DBG("Invalidating all data. There are [ %d ] connected clients", mClientCount);

    mCountString = 0;
    mCountInteger = 0;
    mSeqString = 0;
    mSeqInteger = 0;

    set_service_provider_state(PubSub::RunState::Uninitialized);

    invalidate_integer_always();
    invalidate_string_on_change();
}

void Publisher::quit()
{
    LOG_SCOPE( examples_25_publisher_Publisher, quit );

    areg::Lock lock(mLock);
    LOG_DBG("Requested to quit.There are[% d] connected clients", mClientCount);

    mTimerAlways.stop_timer();
    mTimerOnChange.stop_timer();

    set_service_provider_state(PubSub::RunState::Shutdown);
    areg::Application::signal_quit();
}

void Publisher::process_timer(areg::Timer & timer)
{
    LOG_SCOPE( examples_25_publisher_Publisher, process_timer );

    if (&timer == &mTimerAlways)
    {
        areg::Lock lock(mLock);
        if (++ mCountInteger > PubSub::CycleAlways)
        {
            ++ mSeqInteger;
            mCountInteger = 0;
        }

        LOG_DBG("Timer \'Update Always\' has expired, Integer sequence number is [ %u ], the data should be updated", mSeqInteger);
        set_integer_always(mSeqInteger);
    }
    else if (&timer == &mTimerOnChange)
    {
        areg::Lock lock(mLock);
        if (++ mCountString > PubSub::CycleAlways)
        {
            ++ mSeqString;
            mCountString = 0;
        }

        areg::String data(generateString(mSeqString));
        LOG_DBG("Timer \'Update OnChange\' has expired, String is [ %s ], the data should be updated only on update", data.as_string());
        set_string_on_change(data);
    }
    else
    {
        ASSERT(false);
    }
}

void Publisher::on_run()
{
    areg::ext::Console & console = areg::ext::Console::instance();
    areg::ext::OptionParser parser(ValidOptions, std::size(ValidOptions));
    console.clear_screen();
    console.enable_console_input(true);
    print_message(areg::String::EmptyString, OptionFlag::CMD_Undefined);

    OptionFlag cmd = OptionFlag::CMD_Undefined;

    do
    {
        areg::String message;
        areg::String usrInput = console.read_string();

        if (parser.parse_option_line(usrInput.as_string()))
        {
            const areg::ext::OptionParser::InputOptionList & opts = parser.options();
            cmd = opts.size() == 1u ? static_cast<OptionFlag>(opts[0u].inCommand) : OptionFlag::CMD_Error;
            switch (cmd)
            {
            case Publisher::OptionFlag::CMD_Invalidate:
                invalidate();
                message = "Requested to invalidate data...";
                break;

            case Publisher::OptionFlag::CMD_Pause:
                stop();
                message = "Requested to pause service...";
                break;

            case Publisher::OptionFlag::CMD_Start:
                start();
                message = "Requested to start service...";
                break;

            case Publisher::OptionFlag::CMD_Quit:
                quit();
                message = "Request to quit application...";
                break;

            case Publisher::OptionFlag::CMD_Help:
                break;

            case Publisher::OptionFlag::CMD_Undefined:
            case Publisher::OptionFlag::CMD_Error:
            default:
                cmd = Publisher::OptionFlag::CMD_Error;
                message.format(_fmtError.data(), usrInput.as_string());
                break;
            }
        }
        else
        {
            cmd = Publisher::OptionFlag::CMD_Error;
            message.format(_fmtError.data(), usrInput.as_string());
        }

        print_message(message, cmd);

    } while (cmd != Publisher::OptionFlag::CMD_Quit);
}


inline void Publisher::print_message(const areg::String & message, OptionFlag cmd)
{
    areg::ext::Console & console = areg::ext::Console::instance();
    console.clear_screen();
    console.output_str(_coordTitle       , _title);
    console.output_str(_coordSubtitle    , _separator);
    if (cmd == OptionFlag::CMD_Error)
    {
        console.output_str(_coordErrorMsg, message);
    }
    else if (cmd == OptionFlag::CMD_Help)
    {
        console.output_str(_coordErrorMsg, _help);
    }
    else if (cmd != OptionFlag::CMD_Undefined)
    {
        console.output_msg(_coordInfoMsg, message);
    }

   console.output_str(_coordSeparate , _separator);
   console.output_str(_coordUserInput, _userInput);
}

inline Publisher & Publisher::self()
{
    return (*this);
}
