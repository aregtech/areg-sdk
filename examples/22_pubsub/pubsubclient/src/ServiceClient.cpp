/************************************************************************
 * \file        pubsubclient/src/ServiceClient.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file is demonstration of manual unlbocking of the request.
 *              The service client declaration
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "pubsubclient/src/ServiceClient.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/trace/GETrace.h"
#include "extend/console/Console.hpp"

#include <string_view>

DEF_TRACE_SCOPE(examples_22_pubsubservice_ServiceClient_serviceConnected);
DEF_TRACE_SCOPE(examples_22_pubsubservice_ServiceClient_onStringOnChangeUpdate);
DEF_TRACE_SCOPE(examples_22_pubsubservice_ServiceClient_onIntegerAlwaysUpdate);
DEF_TRACE_SCOPE(examples_22_pubsubservice_ServiceClient_onServiceProviderStateUpdate);

namespace
{
    constexpr std::string_view  _title      { "PubSub features, service consumer..."};
    constexpr std::string_view  _separator  { "--------------------------------------------" };

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

Component * ServiceClient::CreateComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner )
{
    return DEBUG_NEW ServiceClient( entry, owner );
}

void ServiceClient::DeleteComponent( Component & compObject, const NERegistry::ComponentEntry & /*entry*/ )
{
    delete (&compObject);
}

ServiceClient::ServiceClient( const NERegistry::ComponentEntry & entry, ComponentThread & owner )
    : Component         ( entry, owner )
    , PubSubClientBase  ( entry.mDependencyServices[0], static_cast<Component &>(self()) )
    , mOldInteger       ( 0 )
    , mOldState         ( false )
    , mOldString        (_invalid )
{
}

bool ServiceClient::serviceConnected( NEService::eServiceConnection status, ProxyBase & proxy )
{
    TRACE_SCOPE(examples_22_pubsubservice_ServiceClient_serviceConnected);
    PubSubClientBase::serviceConnected( status, proxy );

    TRACE_DBG("Service connection with status [ %s ]. If connected assign on provider state change", NEService::getString(status));

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

void ServiceClient::onStringOnChangeUpdate(const String & StringOnChange, NEService::eDataStateType state)
{
    TRACE_SCOPE(examples_22_pubsubservice_ServiceClient_onStringOnChangeUpdate);
    Console & console = Console::getInstance();
    if (state == NEService::eDataStateType::DataIsOK)
    {
        TRACE_DBG("The STRING (on change) data is OK, old is [ %s ], new [ %s ]", mOldString.getString(), StringOnChange.getString());
        console.outputMsg(_coordString, "%s%s => %s { changed }", _txtString.data(), mOldString.getString(), StringOnChange.getString());
        mOldString = StringOnChange;
    }
    else
    {
        TRACE_INFO("The STRING (on change) have got invalidated, old value [ %s ]", mOldString.getString());

        console.outputMsg(_coordString, "%s%s => INVALID { invalid }", _txtString.data(), mOldString.getString());
        mOldString = _invalid;

        if (isServiceProviderStateValid() == false)
        {
            TRACE_WARN("Provider state is invalid, unsubscribe on data { StringOnChange } update");
            notifyOnStringOnChangeUpdate(false);
        }
    }

    console.refreshScreen();
}

void ServiceClient::onIntegerAlwaysUpdate(unsigned int IntegerAlways, NEService::eDataStateType state)
{
    TRACE_SCOPE(examples_22_pubsubservice_ServiceClient_onIntegerAlwaysUpdate);
    Console & console = Console::getInstance();
    String oldInt = mOldState ? String::toString(mOldInteger) : _invalid;
    if (state == NEService::eDataStateType::DataIsOK)
    {
        TRACE_DBG("The INTEGER (always) data is OK, old is [ %s ], new [ %u ]", oldInt.getString(), IntegerAlways);
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
        TRACE_DBG("The INTEGER (ALWAYS) have got invalidated, old value [ %s ]", oldInt.getString());

        console.outputMsg(_coordInteger, "%s%s => INVALID { invalid }", _txtInteger.data(), mOldString.getString());
        mOldInteger = 0;
        mOldState = false;

        if (isServiceProviderStateValid() == false)
        {
            TRACE_WARN("Provider state is invalid, unsubscribe on data { IntegerAlways } update");
            notifyOnIntegerAlwaysUpdate(false);
        }
    }

    console.refreshScreen();
}

void ServiceClient::onServiceProviderStateUpdate(NEPubSub::eServiceState ServiceProviderState, NEService::eDataStateType state)
{
    TRACE_SCOPE(examples_22_pubsubservice_ServiceClient_onServiceProviderStateUpdate);
    if (state == NEService::eDataStateType::DataIsOK)
    {
        if (isIntegerAlwaysValid() == false)
        {
            TRACE_DBG("The integer to update ALWAYS is not valid, subscribe on data");
            notifyOnIntegerAlwaysUpdate(true);
        }

        if (isStringOnChangeValid() == false)
        {
            TRACE_DBG("The string to update ON CHANGE is not valid, subscribe on data");
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

inline ServiceClient & ServiceClient::self(void)
{
    return (*this);
}
