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
#include "areg/logging/areg_log.h"
#include "subscribermulti/src/PubSubDefs.hpp"

DEF_LOG_SCOPE(examples_27_pubsubmulti_subscribermulti_Subscriber, service_connected);
DEF_LOG_SCOPE(examples_27_pubsubmulti_subscribermulti_Subscriber, on_service_provider_state_update);

Subscriber::Subscriber( const areg::ComponentEntry & entry, areg::ComponentThread & owner )
    : areg::Component         (entry, owner)
    , SubscriberBase    (entry.mDependencyServices[0], static_cast<areg::Component &>(self()), pubsub::Coord1Integer, pubsub::Coord1String)
    , mStateEventCount  ( 0 )
    , mSecond           ( entry.mDependencyServices[1], static_cast<areg::Component &>(self()) )
{
}

bool Subscriber::service_connected( areg::ServiceConnectionState status, areg::ProxyBase & proxy )
{
    LOG_SCOPE( examples_27_pubsubmulti_subscribermulti_Subscriber, service_connected );
    PubSubConsumerBase::service_connected( status, proxy );

    LOG_DBG("Service connection with status [ %s ]. If connected assign on provider state change", areg::as_string(status));

    bool connected = areg::is_service_connected(status);
    notify_on_service_provider_state_update(connected);

    areg::ext::Console & console = areg::ext::Console::instance();

    if (connected == false)
    {
        notify_on_string_on_change_update(false);
        notify_on_integer_always_update(false);

        console.output_msg(pubsub::CoordStatus, pubsub::FmtDisconnected.data(), areg::as_string(status));
    }
    else
    {
        console.clear_screen();
        console.output_txt(pubsub::CoordTitle, pubsub::AppTitle);
        console.output_txt(pubsub::CoordSubtitle, pubsub::Separator);
        console.output_txt(pubsub::CoordStatus, pubsub::TxtConnected);
        console.output_txt(pubsub::Coord1Subtitle, pubsub::Txt1Subscriber);
        console.output_txt(pubsub::Coord2Subtitle, pubsub::Txt2Subscriber);
    }

    console.refresh_screen();

    return true;
}

void Subscriber::on_service_provider_state_update(PubSub::RunState ServiceProviderState, areg::DataState state)
{
    LOG_SCOPE( examples_27_pubsubmulti_subscribermulti_Subscriber, on_service_provider_state_update );

    ++ mStateEventCount;
    areg::String publisherState = state == areg::DataState::DataIsOK ? PubSub::as_string(ServiceProviderState) : pubsub::StrInvalid.data();

    LOG_DBG("Service provider state [ %s ], event count [ %u ]", publisherState.as_string(), mStateEventCount);

    areg::ext::Console & console = areg::ext::Console::instance();    
    areg::String stateConnect = pubsub::TxtConnected;
    if (PubSubConsumerBase::is_connected() == false)
    {
        ASSERT(PubSubConsumerBase::service_proxy() != nullptr);
        stateConnect = areg::as_string(PubSubConsumerBase::service_proxy()->connection_status());
    }

    console.output_msg(  pubsub::CoordStatus
                      , "PubSub service %s, Publisher %s, event count: %u"
                      , stateConnect.as_string()
                      , publisherState.as_string()
                      , mStateEventCount);
    console.refresh_screen();

    if (state == areg::DataState::DataIsOK)
    {

        if (!is_integer_always_valid())
        {
            LOG_DBG("The integer to update ALWAYS is not valid, subscribe on data");
            notify_on_integer_always_update(true);
        }

        if (!is_string_on_change_valid())
        {
            LOG_DBG("The string to update ON CHANGE is not valid, subscribe on data");
            notify_on_string_on_change_update(true);
        }

        mSecond.notify_on_service_provider_state_update(ServiceProviderState != PubSub::RunState::Shutdown);

        if (ServiceProviderState == PubSub::RunState::Shutdown)
        {
            notify_on_string_on_change_update(false);
            notify_on_integer_always_update(false);
            areg::Application::signal_quit();
        }
    }
}

inline Subscriber & Subscriber::self()
{
    return (*this);
}
