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

DEF_TRACE_SCOPE(examples_10_locservice_ServiceClient_serviceConnected);
DEF_TRACE_SCOPE(examples_10_locservice_ServiceClient_broadcastReachedMaximum);
DEF_TRACE_SCOPE(examples_10_locservice_ServiceClient_responseHelloWorld);
DEF_TRACE_SCOPE(examples_10_locservice_ServiceClient_processTimer);

Component * ServiceClient::CreateComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
{
    return DEBUG_NEW ServiceClient(entry, owner);
}

void ServiceClient::DeleteComponent(Component & compObject, const NERegistry::ComponentEntry & entry)
{
    delete (&compObject);
}

ServiceClient::ServiceClient(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : Component             ( entry, owner )
    , HelloWorldClientBase  ( entry.mDependencyServices[0].mRoleName, static_cast<Component &>(self()) )
    , IETimerConsumer       ( )

    , mTimer                (static_cast<IETimerConsumer &>(self()), entry.mRoleName)
{
}

bool ServiceClient::serviceConnected(bool isConnected, ProxyBase & proxy)
{
    TRACE_SCOPE(examples_10_locservice_ServiceClient_serviceConnected);
    bool result = HelloWorldClientBase::serviceConnected(isConnected, proxy);

    TRACE_DBG("Client [ %s ] of [ %s ] service is [ %s ]"
                , proxy.getProxyAddress().getRoleName().getString()
                , proxy.getProxyAddress().getServiceName().getString()
                , isConnected ? "connected" : "disconnected");

    // subscribe when service connected and un-subscribe when disconnected.
    notifyOnBroadcastReachedMaximum(isConnected);
    if (isConnected)
    {
        mTimer.startTimer(ServiceClient::TIMEOUT_VALUE);
    }
    else
    {
        mTimer.stopTimer();
    }

    return result;
}

void ServiceClient::responseHelloWorld( void )
{
    TRACE_SCOPE(examples_10_locservice_ServiceClient_responseHelloWorld);
    TRACE_DBG("Received response on request to print greetings from the client");
}

void ServiceClient::broadcastReachedMaximum( int maxNumber )
{
    TRACE_SCOPE(examples_10_locservice_ServiceClient_broadcastReachedMaximum );
    TRACE_WARN("Service notify reached maximum number of requests [ %d ], starting shutdown procedure", maxNumber );
    requestShutdownService( );
}

void ServiceClient::processTimer(Timer & timer)
{
    TRACE_SCOPE(examples_10_locservice_ServiceClient_processTimer);
    ASSERT(&timer == &mTimer);

    TRACE_DBG("Timer [ %s ] expired, send request to output message.", timer.getName().getString());
    requestHelloWorld(getRoleName());
}
