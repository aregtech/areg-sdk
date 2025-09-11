/************************************************************************
 * \file        subscriber/src/Subscriber.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file is a demonstration of a data subscriber, which is a service consumer.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "subscriber/src/Subscriber.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/logging/GELog.h"
#include "aregextend/console/Console.hpp"

#include <string_view>

DEF_LOG_SCOPE(examples_25_subscriber_Subscriber_serviceConnected);
DEF_LOG_SCOPE(examples_25_subscriber_Subscriber_onStringOnChangeUpdate);
DEF_LOG_SCOPE(examples_25_subscriber_Subscriber_onIntegerAlwaysUpdate);
DEF_LOG_SCOPE(examples_25_subscriber_Subscriber_onServiceProviderStateUpdate);

namespace
{
    constexpr std::string_view  _title          { "PubSub features, Subscriber part..."};
    constexpr std::string_view  _separator      { "--------------------------------------------" };

    constexpr std::string_view  _invalid        { "INVALID" };

    constexpr std::string_view  _txtInteger     { "Integer update (Always) .: "};
    constexpr std::string_view  _txtString      { "String update (OnChange) : "};

    constexpr std::string_view  _txtConnected   { "Connected to PubSub service" };
    constexpr std::string_view  _fmtDisconnected{ "Connected to PubSub service with status [ %s ]" };

    constexpr Console::Coord    _coordTitle     { 0, 1 };
    constexpr Console::Coord    _coordSubtitle  { 0, 2 };
    constexpr Console::Coord    _coordStatus    { 0, 3 };

    constexpr Console::Coord    _coordInteger   { 0, 5 };
    constexpr Console::Coord    _coordString    { 0, 6 };
}

Subscriber::Subscriber( const NERegistry::ComponentEntry & entry, ComponentThread & owner )
    : Component         ( entry, owner )
    , PubSubClientBase  ( entry.mDependencyServices[0], static_cast<Component &>(self()) )
    , mOldInteger       ( 0 )
    , mOldState         ( false )
    , mOldString        (_invalid )
{
}

bool Subscriber::serviceConnected( NEService::eServiceConnection status, ProxyBase & proxy )
{
    LOG_SCOPE(examples_25_subscriber_Subscriber_serviceConnected);
    PubSubClientBase::serviceConnected( status, proxy );

    LOG_DBG("Service connection with status [ %s ]. If connected assign on provider state change", NEService::getString(status));

    bool connected = NEService::isServiceConnected(status);
    notifyOnServiceProviderStateUpdate(connected);

    Console & console = Console::getInstance();

    if (connected == false)
    {
        notifyOnStringOnChangeUpdate(false);
        notifyOnIntegerAlwaysUpdate(false);

        console.outputMsg(_coordStatus, _fmtDisconnected.data(), NEService::getString(status));
    }
    else
    {
        console.clearScreen();
        console.outputTxt(_coordTitle, _title);
        console.outputTxt(_coordSubtitle, _separator);
        console.outputTxt(_coordStatus, _txtConnected);
    }

    console.refreshScreen();

    return true;
}

void Subscriber::onStringOnChangeUpdate(const String & StringOnChange, NEService::eDataStateType state)
{
    LOG_SCOPE(examples_25_subscriber_Subscriber_onStringOnChangeUpdate);
    Console & console = Console::getInstance();
    if (state == NEService::eDataStateType::DataIsOK)
    {
        LOG_DBG("The STRING (on change) data is OK, old is [ %s ], new [ %s ]", mOldString.getString(), StringOnChange.getString());
        console.outputMsg(_coordString, "%s%s => %s { changed }", _txtString.data(), mOldString.getString(), StringOnChange.getString());
        mOldString = StringOnChange;
    }
    else
    {
        LOG_INFO("The STRING (on change) have got invalidated, old value [ %s ]", mOldString.getString());

        console.outputMsg(_coordString, "%s%s => INVALID { invalid }", _txtString.data(), mOldString.getString());
        mOldString = _invalid;

        if (isServiceProviderStateValid() == false)
        {
            LOG_WARN("Provider state is invalid, unsubscribe on data { StringOnChange } update");
            notifyOnStringOnChangeUpdate(false);
        }
    }

    console.refreshScreen();
}

void Subscriber::onIntegerAlwaysUpdate(unsigned int IntegerAlways, NEService::eDataStateType state)
{
    LOG_SCOPE(examples_25_subscriber_Subscriber_onIntegerAlwaysUpdate);
    Console & console = Console::getInstance();
    String oldInt = mOldState ? String::makeString(mOldInteger) : _invalid;
    if (state == NEService::eDataStateType::DataIsOK)
    {
        LOG_DBG("The INTEGER (always) data is OK, old is [ %s ], new [ %u ]", oldInt.getString(), IntegerAlways);
        console.outputMsg(_coordInteger, "%s%s => %u { %s }"
                          , _txtInteger.data()
                          , oldInt.getString()
                          , IntegerAlways
                          , (mOldState == false) || (IntegerAlways != mOldInteger) ? "changed" : "UNCHANGED");
        mOldInteger = IntegerAlways;
        mOldState = true;
    }
    else
    {
        LOG_DBG("The INTEGER (ALWAYS) have got invalidated, old value [ %s ]", oldInt.getString());

        console.outputMsg(_coordInteger, "%s%s => INVALID { invalid }", _txtInteger.data(), oldInt.getString());
        mOldInteger = 0;
        mOldState = false;

        if (isServiceProviderStateValid() == false)
        {
            LOG_WARN("Provider state is invalid, unsubscribe on data { IntegerAlways } update");
            notifyOnIntegerAlwaysUpdate(false);
        }
    }

    console.refreshScreen();
}

void Subscriber::onServiceProviderStateUpdate(NEPubSub::eServiceState ServiceProviderState, NEService::eDataStateType state)
{
    LOG_SCOPE(examples_25_subscriber_Subscriber_onServiceProviderStateUpdate);
    if (state == NEService::eDataStateType::DataIsOK)
    {
        if (isIntegerAlwaysValid() == false)
        {
            LOG_DBG("The integer to update ALWAYS is not valid, subscribe on data");
            notifyOnIntegerAlwaysUpdate(true);
        }

        if (isStringOnChangeValid() == false)
        {
            LOG_DBG("The string to update ON CHANGE is not valid, subscribe on data");
            notifyOnStringOnChangeUpdate(true);
        }

        if (ServiceProviderState == NEPubSub::eServiceState::Shutdown)
        {
            notifyOnStringOnChangeUpdate(false);
            notifyOnIntegerAlwaysUpdate(false);
            Application::signalAppQuit();
        }
    }
}

inline Subscriber & Subscriber::self(void)
{
    return (*this);
}
