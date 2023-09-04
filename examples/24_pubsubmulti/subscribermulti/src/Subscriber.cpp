/************************************************************************
 * \file        subscribermulti/src/Subscriber.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file is a demonstration of a data subscriber, which is a service consumer.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "subscribermulti/src/Subscriber.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/trace/GETrace.h"
#include "subscribermulti/src/NECommon.hpp"

DEF_TRACE_SCOPE(example_24_pubsubmulti_subscribermulti_Subscriber_serviceConnected);
DEF_TRACE_SCOPE(example_24_pubsubmulti_subscribermulti_Subscriber_onServiceProviderStateUpdate);

Component * Subscriber::CreateComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner )
{
    return DEBUG_NEW Subscriber( entry, owner );
}

void Subscriber::DeleteComponent( Component & compObject, const NERegistry::ComponentEntry & /*entry*/ )
{
    delete (&compObject);
}

Subscriber::Subscriber( const NERegistry::ComponentEntry & entry, ComponentThread & owner )
    : SubscriberBase    (entry.mDependencyServices[0], static_cast<Component &>(self()), NECommon::Coord1Integer, NECommon::Coord1String)
    , Component         ( entry, owner )
    , mStateEventCount  ( 0 )
    , mSecond           ( entry.mDependencyServices[1], static_cast<Component &>(self()) )
{
}

bool Subscriber::serviceConnected( NEService::eServiceConnection status, ProxyBase & proxy )
{
    TRACE_SCOPE(example_24_pubsubmulti_subscribermulti_Subscriber_serviceConnected);
    PubSubClientBase::serviceConnected( status, proxy );

    TRACE_DBG("Service connection with status [ %s ]. If connected assign on provider state change", NEService::getString(status));

    bool connected = NEService::isServiceConnected(status);
    notifyOnServiceProviderStateUpdate(connected);

    Console & console = Console::getInstance();

    if (connected == false)
    {
        notifyOnStringOnChangeUpdate(false);
        notifyOnIntegerAlwaysUpdate(false);

        console.outputMsg(NECommon::CoordStatus, NECommon::FmtDisconnected.data(), NEService::getString(status));
    }
    else
    {
        console.clearScreen();
        console.outputTxt(NECommon::CoordTitle, NECommon::AppTitle);
        console.outputTxt(NECommon::CoordSubtitle, NECommon::Separator);
        console.outputTxt(NECommon::CoordStatus, NECommon::TxtConnected);
        console.outputTxt(NECommon::Coord1Subtitle, NECommon::Txt1Subscriber);
        console.outputTxt(NECommon::Coord2Subtitle, NECommon::Txt2Subscriber);
    }

    console.refreshScreen();

    return true;
}

void Subscriber::onServiceProviderStateUpdate(NEPubSub::eServiceState ServiceProviderState, NEService::eDataStateType state)
{
    TRACE_SCOPE(example_24_pubsubmulti_subscribermulti_Subscriber_onServiceProviderStateUpdate);

    ++ mStateEventCount;
    String publisherState = state == NEService::eDataStateType::DataIsOK ? NEPubSub::getString(ServiceProviderState) : NECommon::StrInvalid.data();

    TRACE_DBG("Service provider state [ %s ], event count [ %u ]", publisherState.getString(), mStateEventCount);

    Console & console = Console::getInstance();    
    String stateConnect = NECommon::TxtConnected;
    if (PubSubClientBase::isConnected() == false)
    {
        ASSERT(PubSubClientBase::getProxy() != nullptr);
        stateConnect = NEService::getString(PubSubClientBase::getProxy()->getConnectionStatus());
    }

    console.outputMsg(  NECommon::CoordStatus
                      , "PubSub service %s, Publisher %s, event count: %u"
                      , stateConnect.getString()
                      , publisherState.getString()
                      , mStateEventCount);
    console.refreshScreen();

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

        mSecond.notifyOnServiceProviderStateUpdate(ServiceProviderState != NEPubSub::eServiceState::Shutdown);

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
