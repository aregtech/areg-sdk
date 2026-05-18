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
#include "pubclient/src/ServiceClient.hpp"
#include "areg/logging/areg_log.h"
#include "areg/appbase/Application.hpp"

DEF_LOG_SCOPE(examples_22_pubclient_ServiceClient, service_connected);
DEF_LOG_SCOPE(examples_22_pubclient_ServiceClient, on_service_state_update);
DEF_LOG_SCOPE(examples_22_pubclient_ServiceClient, response_start_sleep);
DEF_LOG_SCOPE(examples_22_pubclient_ServiceClient, request_start_sleep_failed);
DEF_LOG_SCOPE(examples_22_pubclient_ServiceClient, request_stop_service_failed);
DEF_LOG_SCOPE(examples_22_pubclient_ServiceClient, request_shutdown_service_failed);

ServiceClient::ServiceClient(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    : areg::Component              ( entry, owner )
    , HelloWatchdogConsumerBase( entry.mDependencyServices[0].mRoleName, static_cast<areg::Component &>(self()) )

    , mSleepTimeout          ( 0 )
    , mRestarts              ( 0 )
{
}

bool ServiceClient::service_connected( areg::ServiceConnectionState status, areg::ProxyBase & proxy)
{
    LOG_SCOPE( examples_22_pubclient_ServiceClient, service_connected );
    bool result = HelloWatchdogConsumerBase::service_connected(status, proxy);

    if (is_connected())
    {
        // dynamic subscribe on messages.
        notify_on_service_state_update( true );
        if (++ mRestarts <= HelloWatchdog::MaximumRestarts)
        {
            mSleepTimeout   = HelloWatchdog::InitialSleepTimeout;
            LOG_DBG( "Initialized thread sleep timeout [ %u ] ms, sending first request", mSleepTimeout );

            request_start_sleep( mSleepTimeout );
        }
        else
        {
            LOG_DBG("Reached maximum number of service restarts, exit application");
            printf("Reached maximum number of service restarts, stopping service to shutdown ...\n");
            request_stop_service();
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

void ServiceClient::on_service_state_update( HelloWatchdog::ComponentState ServiceState, areg::DataState state )
{
    LOG_SCOPE( examples_22_pubclient_ServiceClient, on_service_state_update );
    LOG_DBG("Current service state is [ %s ], data state is [ %s ]", HelloWatchdog::as_string(ServiceState), areg::as_string(state));
    if (state == areg::DataState::DataIsOK)
    {
        if (ServiceState == HelloWatchdog::ComponentState::Stopped)
        {
            printf("Sending request to shutdown and quit application");
            request_shutdown_service();
            areg::Application::signal_quit();
        }
    }
}

void ServiceClient::response_start_sleep( uint32_t timeoutSleep )
{
    LOG_SCOPE( examples_22_pubclient_ServiceClient, response_start_sleep );
    LOG_DBG("Completed service sleep, current timeout is [ %u ]", timeoutSleep);

    if (timeoutSleep != 0)
    {
        ASSERT( timeoutSleep == mSleepTimeout );
        mSleepTimeout += HelloWatchdog::TimeoutStep;

        request_start_sleep( mSleepTimeout );
    }
    else
    {
        LOG_INFO("The service didn't sleep");
        printf("The service didn't sleep.\n");
    }
}

#if AREG_LOGGING

void ServiceClient::request_start_sleep_failed( areg::ResultType FailureReason )
{
    LOG_SCOPE( examples_22_pubclient_ServiceClient, request_start_sleep_failed );
    LOG_WARN("Request to sleep service failed with reason [ %s ]", areg::as_string(FailureReason));
}

void ServiceClient::request_stop_service_failed( areg::ResultType FailureReason )
{
    LOG_SCOPE( examples_22_pubclient_ServiceClient, request_stop_service_failed );
    LOG_WARN( "Request to stop the service failed with reason [ %s ]", areg::as_string( FailureReason ) );
}

void ServiceClient::request_shutdown_service_failed( areg::ResultType FailureReason )
{
    LOG_SCOPE( examples_22_pubclient_ServiceClient, request_shutdown_service_failed );
    LOG_WARN( "Request to shutdown service failed with reason [ %s ]", areg::as_string( FailureReason ) );
}

#endif  // AREG_LOGGING
