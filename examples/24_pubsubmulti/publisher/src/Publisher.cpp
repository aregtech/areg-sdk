/************************************************************************
 * \file        publisher/src/Publisher.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file is demonstration of data PubSub (Publish / Subscribe) features.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/

#include "publisher/src/Publisher.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/trace/GETrace.h"
#include "extend/console/Console.hpp"

//////////////////////////////////////////////////////////////////////////
// Log scopes
//////////////////////////////////////////////////////////////////////////

DEF_TRACE_SCOPE(examples_24_pubsubmulti_publisher_Publisher_clientConnected);
DEF_TRACE_SCOPE(examples_24_pubsubmulti_publisher_Publisher_start);
DEF_TRACE_SCOPE(examples_24_pubsubmulti_publisher_Publisher_stop);
DEF_TRACE_SCOPE(examples_24_pubsubmulti_publisher_Publisher_invalidate);
DEF_TRACE_SCOPE(examples_24_pubsubmulti_publisher_Publisher_quit);
DEF_TRACE_SCOPE(examples_24_pubsubmulti_publisher_Publisher_processTimer);


namespace
{
    String generateString(uint32_t seqNr)
    {
        String result;
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

    constexpr Console::Coord    _coordTitle     { 0, 1 };
    constexpr Console::Coord    _coordSubtitle  { 0, 2 };
    constexpr Console::Coord    _coordUserInput { 0, 4 };
    constexpr Console::Coord    _coordSeparate  { 0, 6 };
    constexpr Console::Coord    _coordErrorMsg  { 0, 7 };
    constexpr Console::Coord    _coordInfoMsg   { 0, 8 };

}

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////

const OptionParser::sOptionSetup Publisher::ValidOptions[]
{
      {"i", "invalid", static_cast<int>(eCommands::CMD_Invalidate)  , OptionParser::NO_DATA , {}, {}, {} }
    , {"p", "pause"  , static_cast<int>(eCommands::CMD_Pause)       , OptionParser::NO_DATA , {}, {}, {} }
    , {"s", "start"  , static_cast<int>(eCommands::CMD_Start)       , OptionParser::NO_DATA , {}, {}, {} }
    , {"q", "quit"   , static_cast<int>(eCommands::CMD_Quit)        , OptionParser::NO_DATA , {}, {}, {} }
    , {"h", "help"   , static_cast<int>(eCommands::CMD_Help)        , OptionParser::NO_DATA , {}, {}, {} }
};

Component * Publisher::CreateComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
{
    return DEBUG_NEW Publisher(entry, owner);
}

void Publisher::DeleteComponent(Component & compObject, const NERegistry::ComponentEntry & /*entry*/)
{
    delete (&compObject);
}

//////////////////////////////////////////////////////////////////////////
// Publisher class methods
//////////////////////////////////////////////////////////////////////////

Publisher::Publisher( const NERegistry::ComponentEntry & entry, ComponentThread & owner )
    : Component         ( entry, owner )
    , PubSubStub        ( static_cast<Component &>(self()) )
    , IETimerConsumer   ( )
    , IEThreadConsumer  ( )

    , mTimerOnChange    (static_cast<IETimerConsumer &>(self()), entry.mRoleName + "_OnUpdateTimer")
    , mTimerAlways      (static_cast<IETimerConsumer &>(self()), entry.mRoleName + "_AlwaysTimer")
    , mClientCount      (0)

    , mSeqString        (0)
    , mCountString      (0)

    , mSeqInteger       (0)
    , mCountInteger     (0)

    , mConsoleThread    (static_cast<IEThreadConsumer &>(self()), entry.mRoleName + "_Thread")
    , mLock             (false)
{
}

void Publisher::startupComponent(ComponentThread & comThread)
{
    Component::startupComponent(comThread);
    mConsoleThread.createThread(NECommon::WAIT_INFINITE);
}

void Publisher::shutdownComponent(ComponentThread & comThread)
{
    mConsoleThread.shutdownThread(NECommon::WAIT_INFINITE);
    Component::shutdownComponent(comThread);
}

bool Publisher::clientConnected(const ProxyAddress & client, NEService::eServiceConnection status)
{
    TRACE_SCOPE(examples_24_pubsubmulti_publisher_Publisher_clientConnected);
    bool result = PubSubStub::clientConnected(client, status);

    TRACE_DBG("Connection status [ %s ] of the consumer [ %s ]", NEService::getString(status), ProxyAddress::convAddressToPath(client).getString());
    mClientCount += (NEService::isServiceConnected(status) ? 1 : -1);
    TRACE_DBG("There are [ %d ] connected service consumers", mClientCount);

    if (isServiceProviderStateValid() == false)
    {
        setServiceProviderState(NEPubSub::eServiceState::Uninitialized);
    }

    return result;
}

void Publisher::start(void)
{
    TRACE_SCOPE(examples_24_pubsubmulti_publisher_Publisher_start);

    Lock lock(mLock);
    TRACE_DBG("Requested to re-start the service run. Reset values and re-start timers, there are [ %d ] connected clients",  mClientCount);

    mTimerAlways.stopTimer();
    mTimerOnChange.stopTimer();

    setServiceProviderState(NEPubSub::eServiceState::Running);

    if (isIntegerAlwaysValid() == false)
    {
        mSeqInteger = 0;
        setIntegerAlways(mSeqInteger);
    }

    if (isStringOnChangeValid() == false)
    {
        mSeqString = 0;
        setStringOnChange(generateString(mSeqString));
    }

    mTimerAlways.startTimer(NEPubSub::TimeoutAlways, getComponentThread(), Timer::CONTINUOUSLY);
    mTimerOnChange.startTimer(NEPubSub::TimeoutOnChange, getComponentThread(), Timer::CONTINUOUSLY);
}

void Publisher::stop(void)
{
    TRACE_SCOPE(examples_24_pubsubmulti_publisher_Publisher_stop);

    Lock lock(mLock);
    TRACE_DBG("Stopped servicing, resets data, wait for further instructions. There are [ %d ] connected clients", mClientCount);

    mTimerAlways.stopTimer();
    mTimerOnChange.stopTimer();

    setServiceProviderState(NEPubSub::eServiceState::Stopped);
}

void Publisher::invalidate(void)
{
    TRACE_SCOPE(examples_24_pubsubmulti_publisher_Publisher_invalidate);

    Lock lock(mLock);
    TRACE_DBG("Invalidating all data. There are [ %d ] connected clients", mClientCount);

    mCountString = 0;
    mCountInteger = 0;
    mSeqString = 0;
    mSeqInteger = 0;

    setServiceProviderState(NEPubSub::eServiceState::Uninitialized);

    invalidateIntegerAlways();
    invalidateStringOnChange();
}

void Publisher::quit(void)
{
    TRACE_SCOPE(examples_24_pubsubmulti_publisher_Publisher_quit);

    Lock lock(mLock);
    TRACE_DBG("Requested to quit.There are[% d] connected clients", mClientCount);

    mTimerAlways.stopTimer();
    mTimerOnChange.stopTimer();

    setServiceProviderState(NEPubSub::eServiceState::Shutdown);
    Application::signalAppQuit();
}

void Publisher::processTimer(Timer & timer)
{
    TRACE_SCOPE(examples_24_pubsubmulti_publisher_Publisher_processTimer);

    if (&timer == &mTimerAlways)
    {
        Lock lock(mLock);
        if (++ mCountInteger > NEPubSub::CycleAlways)
        {
            ++ mSeqInteger;
            mCountInteger = 0;
        }

        TRACE_DBG("Timer \'Update Always\' has expired, Integer sequence number is [ %u ], the data should be updated", mSeqInteger);
        setIntegerAlways(mSeqInteger);
    }
    else if (&timer == &mTimerOnChange)
    {
        Lock lock(mLock);
        if (++ mCountString > NEPubSub::CycleAlways)
        {
            ++ mSeqString;
            mCountString = 0;
        }

        String data(generateString(mSeqString));
        TRACE_DBG("Timer \'Update OnChange\' has expired, String is [ %s ], the data should be updated only on update", data.getString());
        setStringOnChange(data);
    }
    else
    {
        ASSERT(false);
    }
}

void Publisher::onThreadRuns(void)
{
    Console & console = Console::getInstance();
    OptionParser parser(ValidOptions, MACRO_ARRAYLEN(ValidOptions));
    console.clearScreen();
    console.enableConsoleInput(true);
    printMessage(String::EmptyString, eCommands::CMD_Undefined);

    eCommands cmd = eCommands::CMD_Undefined;

    do
    {
        String message;
        String usrInput = console.readString();

        if (parser.parseOptionLine(usrInput.getString()))
        {
            const OptionParser::InputOptionList & opts = parser.getOptions();
            cmd = opts.getSize() == 1u ? static_cast<eCommands>(opts[0u].inCommand) : eCommands::CMD_Error;
            switch (cmd)
            {
            case Publisher::eCommands::CMD_Invalidate:
                invalidate();
                message = "Requested to invalidate data...";
                break;

            case Publisher::eCommands::CMD_Pause:
                stop();
                message = "Requested to pause service...";
                break;

            case Publisher::eCommands::CMD_Start:
                start();
                message = "Requested to start service...";
                break;

            case Publisher::eCommands::CMD_Quit:
                quit();
                message = "Request to quit application...";
                break;

            case Publisher::eCommands::CMD_Help:
                break;

            case Publisher::eCommands::CMD_Error:
            case Publisher::eCommands::CMD_Undefined:
            default:
                cmd = Publisher::eCommands::CMD_Error;
                message.format(_fmtError.data(), usrInput.getString());
                break;
            }
        }
        else
        {
            cmd = Publisher::eCommands::CMD_Error;
            message.format(_fmtError.data(), usrInput.getString());
        }

        printMessage(message, cmd);

    } while (cmd != eCommands::CMD_Quit);
}


inline void Publisher::printMessage(const String & message, eCommands cmd)
{
    Console & console = Console::getInstance();
    console.clearScreen();
    console.outputStr(_coordTitle       , _title);
    console.outputStr(_coordSubtitle    , _separator);
    if (cmd == eCommands::CMD_Error)
    {
        console.outputStr(_coordErrorMsg, message);
    }
    else if (cmd == eCommands::CMD_Help)
    {
        console.outputStr(_coordErrorMsg, _help);
    }
    else if (cmd != eCommands::CMD_Undefined)
    {
        console.outputMsg(_coordInfoMsg, message);
    }

   console.outputStr(_coordSeparate , _separator);
   console.outputStr(_coordUserInput, _userInput);
}

inline Publisher & Publisher::self(void)
{
    return (*this);
}
