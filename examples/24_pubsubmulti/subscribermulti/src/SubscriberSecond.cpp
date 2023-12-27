/************************************************************************
 * \file        subscriber/src/SubscriberSecond.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file is a demonstration of a data subscriber, which is a service consumer.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "subscribermulti/src/SubscriberSecond.hpp"

#include "subscribermulti/src/NECommon.hpp"
#include "areg/trace/GETrace.h"

DEF_TRACE_SCOPE(example_24_pubsubmulti_subscribermulti_SubscriberSecond_serviceConnected);
DEF_TRACE_SCOPE(example_24_pubsubmulti_subscribermulti_SubscriberSecond_onServiceProviderStateUpdate);

SubscriberSecond::SubscriberSecond( const NERegistry::DependencyEntry & entry, Component & owner )
    : SubscriberBase    ( entry, owner, NECommon::Coord2Integer, NECommon::Coord2String )
{
}

bool SubscriberSecond::serviceConnected( NEService::eServiceConnection status, ProxyBase & proxy )
{
    TRACE_SCOPE(example_24_pubsubmulti_subscribermulti_SubscriberSecond_serviceConnected);
    TRACE_DBG("Service connection with status [ %s ]. If connected assign on provider state change", NEService::getString(status));
    if (NEService::isServiceDisconnected(status))
    {
        notifyOnStringOnChangeUpdate(false);
        notifyOnIntegerAlwaysUpdate(false);
        notifyOnServiceProviderStateUpdate(false);
    }

    return PubSubClientBase::serviceConnected( status, proxy );
}

void SubscriberSecond::onServiceProviderStateUpdate(NEPubSub::eServiceState ServiceProviderState, NEService::eDataStateType state)
{
    TRACE_SCOPE(example_24_pubsubmulti_subscribermulti_SubscriberSecond_onServiceProviderStateUpdate);
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
        }
    }
}
