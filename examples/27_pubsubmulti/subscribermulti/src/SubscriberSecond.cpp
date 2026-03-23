/************************************************************************
 * \file        subscriber/src/SubscriberSecond.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This file is a demonstration of a data subscriber, which is a service consumer.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "subscribermulti/src/SubscriberSecond.hpp"

#include "subscribermulti/src/PubSubDefs.hpp"
#include "areg/logging/areg_log.h"

DEF_LOG_SCOPE(example_27_pubsubmulti_subscribermulti_SubscriberSecond, service_connected);
DEF_LOG_SCOPE(example_27_pubsubmulti_subscribermulti_SubscriberSecond, on_service_provider_state_update);

SubscriberSecond::SubscriberSecond( const areg::DependencyEntry & entry, areg::Component & owner )
    : SubscriberBase    ( entry, owner, pubsub::Coord2Integer, pubsub::Coord2String )
{
}

bool SubscriberSecond::service_connected( areg::ServiceConnectionState status, areg::ProxyBase & proxy )
{
    LOG_SCOPE( example_27_pubsubmulti_subscribermulti_SubscriberSecond, service_connected );
    LOG_DBG("Service connection with status [ %s ]. If connected assign on provider state change", areg::as_string(status));
    if (areg::is_service_disconnected(status))
    {
        notify_on_string_on_change_update(false);
        notify_on_integer_always_update(false);
        notify_on_service_provider_state_update(false);
    }

    return PubSubConsumerBase::service_connected( status, proxy );
}

void SubscriberSecond::on_service_provider_state_update(PubSub::RunState ServiceProviderState, areg::DataState state)
{
    LOG_SCOPE( example_27_pubsubmulti_subscribermulti_SubscriberSecond, on_service_provider_state_update );
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

        if (ServiceProviderState == PubSub::RunState::Shutdown)
        {
            notify_on_string_on_change_update(false);
            notify_on_integer_always_update(false);
        }
    }
}
