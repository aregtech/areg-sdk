/************************************************************************
 * \file        locservice/src/ServiceClient.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This file contains simple implementation of service client to
 *              request message output
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "locservice/src/ServiceClient.hpp"
#include "areg/logging/areg_log.h"
#include "areg/appbase/Application.hpp"

DEF_LOG_SCOPE(examples_21_locwatchdog_ServiceClient, service_connected);
DEF_LOG_SCOPE(examples_21_locwatchdog_ServiceClient, on_service_state_update);
DEF_LOG_SCOPE(examples_21_locwatchdog_ServiceClient, response_start_sleep);

ServiceClient::ServiceClient(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    : areg::Component              ( entry, owner )
    , HelloWatchdogConsumerBase( entry.mDependencyServices[0].mRoleName, static_cast<areg::Component &>(self()) )

    , mSleepTimeout          ( 0 )
    , mRestarts              ( 0 )
{
}

bool ServiceClient::service_connected( areg::ServiceConnectionState status, areg::ProxyBase & proxy)
{
    LOG_SCOPE( examples_21_locwatchdog_ServiceClient, service_connected );
    bool result = HelloWatchdogConsumerBase::service_connected(status, proxy);

    if (is_connected())
    {
        if (++ mRestarts <= HelloWatchdog::MaximumRestarts)
        {
            // dynamic subscribe on messages.
            notify_on_service_state_update( true );
            mSleepTimeout   = HelloWatchdog::InitialSleepTimeout;
            LOG_DBG( "Initialized thread sleep timeout [ %u ] ms, sending first request", mSleepTimeout );

            request_start_sleep( mSleepTimeout );
        }
        else
        {
            LOG_DBG("Reached maximum number of service restarts, exit application");
            printf("Reached maximum number of service restarts, exit application ...\n");
            areg::Application::signal_quit();
        }
    }
    else
    {
        LOG_DBG( "Completing watchdog test with final sleep timeout [ %u ] ms", mSleepTimeout );
        // clear all subscriptions.
        clear_all_notifications();
    }

    return result;
}

#if AREG_LOGGING

void ServiceClient::on_service_state_update( HelloWatchdog::ComponentState ServiceState, areg::DataState state )
{
    LOG_SCOPE( examples_21_locwatchdog_ServiceClient, on_service_state_update );
    LOG_DBG("Current service state is [ %s ], data state is [ %s ]", HelloWatchdog::as_string(ServiceState), areg::as_string(state));
}

#else  // AREG_LOGGING

void ServiceClient::on_service_state_update( HelloWatchdog::ComponentState /*ServiceState*/, areg::DataState /*state*/ )
{
}

#endif  // AREG_LOGGING

void ServiceClient::response_start_sleep( uint32_t timeoutSleep )
{
    LOG_SCOPE( examples_21_locwatchdog_ServiceClient, response_start_sleep );
    LOG_DBG("Completed service sleep, current timeout is [ %u ]", timeoutSleep);

    ASSERT( timeoutSleep == mSleepTimeout);
    mSleepTimeout += HelloWatchdog::TimeoutStep;

    request_start_sleep(mSleepTimeout);
}
