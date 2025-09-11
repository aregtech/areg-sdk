/************************************************************************
 * \file        locservice/src/ServiceClient.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of service client to
 *              request message output
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "locservice/src/ServiceClient.hpp"
#include "areg/logging/GELog.h"
#include "areg/appbase/Application.hpp"

DEF_LOG_SCOPE(examples_21_locwatchdog_ServiceClient_serviceConnected);
DEF_LOG_SCOPE(examples_21_locwatchdog_ServiceClient_onServiceStateUpdate);
DEF_LOG_SCOPE(examples_21_locwatchdog_ServiceClient_responseStartSleep);

ServiceClient::ServiceClient(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : Component              ( entry, owner )
    , HelloWatchdogClientBase( entry.mDependencyServices[0].mRoleName, static_cast<Component &>(self()) )

    , mSleepTimeout          ( 0 )
    , mRestarts              ( 0 )
{
}

bool ServiceClient::serviceConnected( NEService::eServiceConnection status, ProxyBase & proxy)
{
    LOG_SCOPE(examples_21_locwatchdog_ServiceClient_serviceConnected);
    bool result = HelloWatchdogClientBase::serviceConnected(status, proxy);

    if (isConnected())
    {
        if (++ mRestarts <= NEHelloWatchdog::MaximumRestarts)
        {
            // dynamic subscribe on messages.
            notifyOnServiceStateUpdate( true );
            mSleepTimeout   = NEHelloWatchdog::InitialSleepTimeout;
            LOG_DBG( "Initialized thread sleep timeout [ %u ] ms, sending first request", mSleepTimeout );

            requestStartSleep( mSleepTimeout );
        }
        else
        {
            LOG_DBG("Reached maximum number of service restarts, exit application");
            printf("Reached maximum number of service restarts, exit application ...\n");
            Application::signalAppQuit();
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

#if AREG_LOGS

void ServiceClient::onServiceStateUpdate( NEHelloWatchdog::eState ServiceState, NEService::eDataStateType state )
{
    LOG_SCOPE(examples_21_locwatchdog_ServiceClient_onServiceStateUpdate);
    LOG_DBG("Current service state is [ %s ], data state is [ %s ]", NEHelloWatchdog::getString(ServiceState), NEService::getString(state));
}

#else  // AREG_LOGS

void ServiceClient::onServiceStateUpdate( NEHelloWatchdog::eState /*ServiceState*/, NEService::eDataStateType /*state*/ )
{
}

#endif  // AREG_LOGS

void ServiceClient::responseStartSleep( unsigned int timeoutSleep )
{
    LOG_SCOPE(examples_21_locwatchdog_ServiceClient_responseStartSleep);
    LOG_DBG("Completed service sleep, current timeout is [ %u ]", timeoutSleep);

    ASSERT( timeoutSleep == mSleepTimeout);
    mSleepTimeout += NEHelloWatchdog::TimeoutStep;

    requestStartSleep(mSleepTimeout);
}
