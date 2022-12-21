/************************************************************************
 * \file        locservice/src/ServiceClient.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of service client to
 *              request message output
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "locservice/src/ServiceClient.hpp"
#include "areg/trace/GETrace.h"
#include "areg/appbase/Application.hpp"

DEF_TRACE_SCOPE(examples_18_locwatchdog_ServiceClient_serviceConnected);
DEF_TRACE_SCOPE(examples_18_locwatchdog_ServiceClient_onServiceStateUpdate);
DEF_TRACE_SCOPE(examples_18_locwatchdog_ServiceClient_responseStartSleep);

Component * ServiceClient::CreateComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
{
    return DEBUG_NEW ServiceClient(entry, owner);
}

void ServiceClient::DeleteComponent(Component & compObject, const NERegistry::ComponentEntry & entry)
{
    delete (&compObject);
}

ServiceClient::ServiceClient(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : Component              ( entry, owner )
    , HelloWatchdogClientBase( entry.mDependencyServices[0].mRoleName, static_cast<Component &>(self()) )

    , mSleepTimeout          ( 0 )
    , mRestarts              ( 0 )
{
}

bool ServiceClient::serviceConnected(bool isConnected, ProxyBase & proxy)
{
    TRACE_SCOPE(examples_18_locwatchdog_ServiceClient_serviceConnected);
    bool result = HelloWatchdogClientBase::serviceConnected(isConnected, proxy);

    TRACE_DBG("Client [ %s ] of [ %s ] service is [ %s ]"
                , proxy.getProxyAddress().getRoleName().getString()
                , proxy.getProxyAddress().getServiceName().getString()
                , isConnected ? "connected" : "disconnected");

    if (isConnected)
    {
        if (++ mRestarts <= NEHelloWatchdog::MaximumRestarts)
        {
            // dynamic subscribe on messages.
            notifyOnServiceStateUpdate( true );
            mSleepTimeout   = NEHelloWatchdog::InitialSleepTimeout;
            TRACE_DBG( "Initialized thread sleep timeout [ %u ] ms, sending first request", mSleepTimeout );

            requestStartSleep( mSleepTimeout );
        }
        else
        {
            TRACE_DBG("Reached maximum number of service restarts, exit application");
            printf("Reached maximum number of service restarts, exit application ...\n");
            Application::signalAppQuit();
        }
    }
    else
    {
        TRACE_DBG( "Completing watchdog test with final sleep timeout [ %u ] ms", mSleepTimeout );
        // clear all subscriptions.
        clearAllNotifications();
    }

    return result;
}

void ServiceClient::onServiceStateUpdate( NEHelloWatchdog::eState ServiceState, NEService::eDataStateType state )
{
    TRACE_SCOPE(examples_18_locwatchdog_ServiceClient_onServiceStateUpdate);
    TRACE_DBG("Current service state is [ %s ], data state is [ %s ]", NEHelloWatchdog::getString(ServiceState), NEService::getString(state));
}

void ServiceClient::responseStartSleep( unsigned int timeoutSleep )
{
    TRACE_SCOPE(examples_18_locwatchdog_ServiceClient_responseStartSleep);
    TRACE_DBG("Completed service sleep, current timeout is [ %u ]", timeoutSleep);

    ASSERT( timeoutSleep == mSleepTimeout);
    mSleepTimeout += NEHelloWatchdog::TimeoutStep;

    requestStartSleep(mSleepTimeout);
}
