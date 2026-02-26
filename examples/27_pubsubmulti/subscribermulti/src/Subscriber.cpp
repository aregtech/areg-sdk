/************************************************************************
 * \file        subscribermulti/src/Subscriber.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This file is a demonstration of a data subscriber, which is a service consumer.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "subscribermulti/src/Subscriber.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/logging/GELog.h"
#include "subscribermulti/src/PubSubDefs.hpp"

DEF_LOG_SCOPE(example_27_pubsubmulti_subscribermulti_Subscriber_serviceConnected);
DEF_LOG_SCOPE(example_27_pubsubmulti_subscribermulti_Subscriber_onServiceProviderStateUpdate);

Subscriber::Subscriber( const areg::ComponentEntry & entry, areg::ComponentThread & owner )
    : areg::Component         (entry, owner)
    , SubscriberBase    (entry.mDependencyServices[0], static_cast<areg::Component &>(self()), pubsub::Coord1Integer, pubsub::Coord1String)
    , mStateEventCount  ( 0 )
    , mSecond           ( entry.mDependencyServices[1], static_cast<areg::Component &>(self()) )
{
}

bool Subscriber::serviceConnected( areg::ServiceConnectionState status, ProxyBase & proxy )
{
    LOG_SCOPE(example_27_pubsubmulti_subscribermulti_Subscriber_serviceConnected);
    PubSubClientBase::serviceConnected( status, proxy );

    LOG_DBG("Service connection with status [ %s ]. If connected assign on provider state change", areg::getString(status));

    bool connected = areg::isServiceConnected(status);
    notifyOnServiceProviderStateUpdate(connected);

    Console & console = Console::getInstance();

    if (connected == false)
    {
        notifyOnStringOnChangeUpdate(false);
        notifyOnIntegerAlwaysUpdate(false);

        console.outputMsg(pubsub::CoordStatus, pubsub::FmtDisconnected.data(), areg::getString(status));
    }
    else
    {
        console.clearScreen();
        console.outputTxt(pubsub::CoordTitle, pubsub::AppTitle);
        console.outputTxt(pubsub::CoordSubtitle, pubsub::Separator);
        console.outputTxt(pubsub::CoordStatus, pubsub::TxtConnected);
        console.outputTxt(pubsub::Coord1Subtitle, pubsub::Txt1Subscriber);
        console.outputTxt(pubsub::Coord2Subtitle, pubsub::Txt2Subscriber);
    }

    console.refreshScreen();

    return true;
}

void Subscriber::onServiceProviderStateUpdate(PubSub::RunState ServiceProviderState, areg::DataState state)
{
    LOG_SCOPE(example_27_pubsubmulti_subscribermulti_Subscriber_onServiceProviderStateUpdate);

    ++ mStateEventCount;
    areg::String publisherState = state == areg::DataState::DataIsOK ? PubSub::getString(ServiceProviderState) : pubsub::StrInvalid.data();

    LOG_DBG("Service provider state [ %s ], event count [ %u ]", publisherState.getString(), mStateEventCount);

    Console & console = Console::getInstance();    
    areg::String stateConnect = pubsub::TxtConnected;
    if (PubSubClientBase::isConnected() == false)
    {
        ASSERT(PubSubClientBase::getProxy() != nullptr);
        stateConnect = areg::getString(PubSubClientBase::getProxy()->getConnectionStatus());
    }

    console.outputMsg(  pubsub::CoordStatus
                      , "PubSub service %s, Publisher %s, event count: %u"
                      , stateConnect.getString()
                      , publisherState.getString()
                      , mStateEventCount);
    console.refreshScreen();

    if (state == areg::DataState::DataIsOK)
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

        mSecond.notifyOnServiceProviderStateUpdate(ServiceProviderState != PubSub::RunState::Shutdown);

        if (ServiceProviderState == PubSub::RunState::Shutdown)
        {
            notifyOnStringOnChangeUpdate(false);
            notifyOnIntegerAlwaysUpdate(false);
            Application::signalAppQuit();
        }
    }
}

inline Subscriber & Subscriber::self()
{
    return (*this);
}
