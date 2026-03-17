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

DEF_LOG_SCOPE(examples_22_pubclient_ServiceClient_serviceConnected);
DEF_LOG_SCOPE(examples_22_pubclient_ServiceClient_onServiceStateUpdate);
DEF_LOG_SCOPE(examples_22_pubclient_ServiceClient_responseStartSleep);
DEF_LOG_SCOPE(examples_22_pubclient_ServiceClient_requestStartSleepFailed );
DEF_LOG_SCOPE(examples_22_pubclient_ServiceClient_requestStopServiceFailed);
DEF_LOG_SCOPE(examples_22_pubclient_ServiceClient_requestShutdownServiceFailed);

ServiceClient::ServiceClient(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    : areg::Component              ( entry, owner )
    , HelloWatchdogClientBase( entry.mDependencyServices[0].mRoleName, static_cast<areg::Component &>(self()) )

    , mSleepTimeout          ( 0 )
    , mRestarts              ( 0 )
{
}

bool ServiceClient::service_connected( areg::ServiceConnectionState status, areg::ProxyBase & proxy)
{
    LOG_SCOPE(examples_22_pubclient_ServiceClient_serviceConnected);
    bool result = HelloWatchdogClientBase::service_connected(status, proxy);

    if (is_connected())
    {
        // dynamic subscribe on messages.
        notifyOnServiceStateUpdate( true );
        if (++ mRestarts <= HelloWatchdog::MaximumRestarts)
        {
            mSleepTimeout   = HelloWatchdog::InitialSleepTimeout;
            LOG_DBG( "Initialized thread sleep timeout [ %u ] ms, sending first request", mSleepTimeout );

            requestStartSleep( mSleepTimeout );
        }
        else
        {
            LOG_DBG("Reached maximum number of service restarts, exit application");
            printf("Reached maximum number of service restarts, stopping service to shutdown ...\n");
            requestStopService();
        }
    }
    else
    {
        LOG_DBG( "Completing watchdog test with final sleep timeout [ %u ] ms", mSleepTimeout );
        // clear all subscriptions.
        clearAllNotifications();
    }

    return result;
}

void ServiceClient::onServiceStateUpdate( HelloWatchdog::ComponentState ServiceState, areg::DataState state )
{
    LOG_SCOPE(examples_22_pubclient_ServiceClient_onServiceStateUpdate);
    LOG_DBG("Current service state is [ %s ], data state is [ %s ]", HelloWatchdog::as_string(ServiceState), areg::as_string(state));
    if (state == areg::DataState::DataIsOK)
    {
        if (ServiceState == HelloWatchdog::ComponentState::Stopped)
        {
            printf("Sending request to shutdown and quit application");
            requestShutdownService();
            areg::Application::signal_quit();
        }
    }
}

void ServiceClient::responseStartSleep( uint32_t timeoutSleep )
{
    LOG_SCOPE(examples_22_pubclient_ServiceClient_responseStartSleep);
    LOG_DBG("Completed service sleep, current timeout is [ %u ]", timeoutSleep);

    if (timeoutSleep != 0)
    {
        ASSERT( timeoutSleep == mSleepTimeout );
        mSleepTimeout += HelloWatchdog::TimeoutStep;

        requestStartSleep( mSleepTimeout );
    }
    else
    {
        LOG_INFO("The service didn't sleep");
        printf("The service didn't sleep.\n");
    }
}

#if AREG_LOGGING

void ServiceClient::requestStartSleepFailed( areg::ResultType FailureReason )
{
    LOG_SCOPE( examples_22_pubclient_ServiceClient_requestStartSleepFailed );
    LOG_WARN("Request to sleep service failed with reason [ %s ]", areg::as_string(FailureReason));
}

void ServiceClient::requestStopServiceFailed( areg::ResultType FailureReason )
{
    LOG_SCOPE( examples_22_pubclient_ServiceClient_requestStopServiceFailed );
    LOG_WARN( "Request to stop the service failed with reason [ %s ]", areg::as_string( FailureReason ) );
}

void ServiceClient::requestShutdownServiceFailed( areg::ResultType FailureReason )
{
    LOG_SCOPE( examples_22_pubclient_ServiceClient_requestShutdownServiceFailed );
    LOG_WARN( "Request to shutdown service failed with reason [ %s ]", areg::as_string( FailureReason ) );
}

#endif  // AREG_LOGGING
