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
#include "pubclient/src/ServiceClient.hpp"
#include "areg/trace/GETrace.h"
#include "areg/appbase/Application.hpp"

DEF_TRACE_SCOPE(examples_19_pubclient_ServiceClient_serviceConnected);
DEF_TRACE_SCOPE(examples_19_pubclient_ServiceClient_onServiceStateUpdate);
DEF_TRACE_SCOPE(examples_19_pubclient_ServiceClient_responseStartSleep);
DEF_TRACE_SCOPE(examples_19_pubclient_ServiceClient_requestStartSleepFailed );
DEF_TRACE_SCOPE(examples_19_pubclient_ServiceClient_requestStopServiceFailed);
DEF_TRACE_SCOPE(examples_19_pubclient_ServiceClient_requestShutdownServiceFailed);

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
    TRACE_SCOPE(examples_19_pubclient_ServiceClient_serviceConnected);
    bool result = HelloWatchdogClientBase::serviceConnected(isConnected, proxy);

    TRACE_DBG("Client [ %s ] of [ %s ] service is [ %s ]"
                , proxy.getProxyAddress().getRoleName().getString()
                , proxy.getProxyAddress().getServiceName().getString()
                , isConnected ? "connected" : "disconnected");

    if (isConnected)
    {
        // dynamic subscribe on messages.
        notifyOnServiceStateUpdate( true );
        if (++ mRestarts <= NEHelloWatchdog::MaximumRestarts)
        {
            mSleepTimeout   = NEHelloWatchdog::InitialSleepTimeout;
            TRACE_DBG( "Initialized thread sleep timeout [ %u ] ms, sending first request", mSleepTimeout );

            requestStartSleep( mSleepTimeout );
        }
        else
        {
            TRACE_DBG("Reached maximum number of service restarts, exit application");
            printf("Reached maximum number of service restarts, stopping service to shutdown ...\n");
            requestStopService();
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
    TRACE_SCOPE(examples_19_pubclient_ServiceClient_onServiceStateUpdate);
    TRACE_DBG("Current service state is [ %s ], data state is [ %s ]", NEHelloWatchdog::getString(ServiceState), NEService::getString(state));
    if (state == NEService::eDataStateType::DataIsOK)
    {
        if (ServiceState == NEHelloWatchdog::eState::Stopped)
        {
            printf("Sending request to shutdown and quit application");
            requestShutdownService();
            Application::signalAppQuit();
        }
    }
}

void ServiceClient::responseStartSleep( unsigned int timeoutSleep )
{
    TRACE_SCOPE(examples_19_pubclient_ServiceClient_responseStartSleep);
    TRACE_DBG("Completed service sleep, current timeout is [ %u ]", timeoutSleep);

    if (timeoutSleep != 0)
    {
        ASSERT( timeoutSleep == mSleepTimeout );
        mSleepTimeout += NEHelloWatchdog::TimeoutStep;

        requestStartSleep( mSleepTimeout );
    }
    else
    {
        TRACE_INFO("The service didn't sleep");
        printf("The service didn't sleep.\n");
    }
}

void ServiceClient::requestStartSleepFailed( NEService::eResultType FailureReason )
{
    TRACE_SCOPE( examples_19_pubclient_ServiceClient_requestStartSleepFailed );
    TRACE_WARN("Request to sleep service failed with reason [ %s ]", NEService::getString(FailureReason));
}

void ServiceClient::requestStopServiceFailed( NEService::eResultType FailureReason )
{
    TRACE_SCOPE( examples_19_pubclient_ServiceClient_requestStopServiceFailed );
    TRACE_WARN( "Request to stop the service failed with reason [ %s ]", NEService::getString( FailureReason ) );
}

void ServiceClient::requestShutdownServiceFailed( NEService::eResultType FailureReason )
{
    TRACE_SCOPE( examples_19_pubclient_ServiceClient_requestShutdownServiceFailed );
    TRACE_WARN( "Request to shutdown service failed with reason [ %s ]", NEService::getString( FailureReason ) );
}
