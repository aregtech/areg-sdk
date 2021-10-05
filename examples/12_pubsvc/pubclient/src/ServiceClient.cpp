/************************************************************************
 * \file        pubclient/src/ServiceClient.cpp
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
#include "areg/component/ComponentThread.hpp"
#include "areg/appbase/Application.hpp"

DEF_TRACE_SCOPE(examples_12_pubclient_ServiceClient_serviceConnected);
DEF_TRACE_SCOPE(examples_12_pubclient_ServiceClient_broadcastServiceUnavailable);
DEF_TRACE_SCOPE(examples_12_pubclient_ServiceClient_responseHelloWorld);
DEF_TRACE_SCOPE(examples_12_pubclient_ServiceClient_requestHelloWorldFailed);
DEF_TRACE_SCOPE(examples_12_pubclient_ServiceClient_requestClientShutdownFailed);
DEF_TRACE_SCOPE(examples_12_pubclient_ServiceClient_processTimer);
DEF_TRACE_SCOPE(examples_12_pubclient_ServiceClient_ServiceClient);

const unsigned int  ServiceClient::TIMEOUT_VALUE    = 491;

Component * ServiceClient::CreateComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
{
    return DEBUG_NEW ServiceClient(entry, owner);
}

void ServiceClient::DeleteComponent(Component & compObject, const NERegistry::ComponentEntry & entry)
{
    delete (&compObject);
}


ServiceClient::ServiceClient(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : Component             ( owner, entry.mRoleName )
    , HelloWorldClientBase  ( entry.mDependencyServices[0].mRoleName.getString(), owner )
    , IETimerConsumer       ( )

    , mTimer                ( static_cast<IETimerConsumer &>(self()), timerName( static_cast<Component &>(self()) ).getString() )
    , mID                   ( 0 )
{
    TRACE_SCOPE(examples_12_pubclient_ServiceClient_ServiceClient);
    TRACE_DBG("Client: roleName [ %s ] of service [ %s ] owner [ %s ] in thread [ %s ] has timer [ %s ]"
                    , entry.mDependencyServices[0].mRoleName.getString()
                    , getServiceName().getString()
                    , getRoleName().getString()
                    , owner.getName().getString()
                    , mTimer.getName().getString());
    TRACE_DBG("Proxy: [ %s ]", ProxyAddress::convAddressToPath(getProxy()->getProxyAddress()).getString());
}

bool ServiceClient::serviceConnected(bool isConnected, ProxyBase & proxy)
{
    TRACE_SCOPE(examples_12_pubclient_ServiceClient_serviceConnected);
    bool result = HelloWorldClientBase::serviceConnected(isConnected, proxy);

    TRACE_DBG("Proxy [ %s ] is [ %s ]"
                , ProxyAddress::convAddressToPath(proxy.getProxyAddress()).getString()
                , isConnected ? "connected" : "disconnected");

    // subscribe when service connected and un-subscribe when disconnected.
    notifyOnBroadcastServiceUnavailable(isConnected);
    if (isConnected)
    {
        mTimer.startTimer(ServiceClient::TIMEOUT_VALUE);
    }
    else
    {
        TRACE_WARN("Shutting down application!");
        mTimer.stopTimer();
        clearAllNotifications();
        Application::signalAppQuit();
    }

    return result;
}

void ServiceClient::responseHelloWorld(const NEHelloWorld::sConnectedClient & clientInfo)
{
    TRACE_SCOPE(examples_12_pubclient_ServiceClient_responseHelloWorld);
    TRACE_DBG("Service [ %s ]: Made output of [ %s ], client ID [ %d ]", getServiceRole().getString(), clientInfo.ccName.getString(), clientInfo.ccID);
    ASSERT(clientInfo.ccName == mTimer.getName());

    if ( mID == 0)
    {
        TRACE_DBG("Registring ID [ %d ] for service client [ %s ]", clientInfo.ccID, mTimer.getName().getString());
        mID = clientInfo.ccID;
    }
}

void ServiceClient::broadcastServiceUnavailable(void)
{
    TRACE_SCOPE(examples_12_pubclient_ServiceClient_broadcastServiceUnavailable);
    TRACE_WARN("Service reached message output maximum, starting shutdown procedure");
    TRACE_INFO("Sending request to shutdown service for client [ %s ] with ID [ %d ]", mTimer.getName().getString(), mID);

    requestClientShutdown(mID, mTimer.getName());
    mID = 0;
}

void ServiceClient::processTimer(Timer & timer)
{
    TRACE_SCOPE(examples_12_pubclient_ServiceClient_processTimer);
    ASSERT(&timer == &mTimer);

    TRACE_DBG("Timer [ %s ] expired, send request to output message.", timer.getName().getString());
    requestHelloWorld(timer.getName(), "remote");
}

void ServiceClient::requestHelloWorldFailed(NEService::eResultType FailureReason)
{
    // make error handling here.
    TRACE_SCOPE(examples_12_pubclient_ServiceClient_requestHelloWorldFailed);
    TRACE_ERR("Request to output greetings failed with reason [ %s ]", NEService::getString(FailureReason));
}

void ServiceClient::requestClientShutdownFailed(NEService::eResultType FailureReason)
{
    // make error handling here.
    TRACE_SCOPE(examples_12_pubclient_ServiceClient_requestClientShutdownFailed);
    TRACE_ERR("Request to notify client shutdown failed with reason [ %s ]", NEService::getString(FailureReason));
}

inline String ServiceClient::timerName( Component & owner ) const
{
    String result = "";
    result += owner.getRoleName();
    result += NECommon::DEFAULT_SPECIAL_CHAR.data();
    result += getServiceRole();
    result += NECommon::DEFAULT_SPECIAL_CHAR.data();
    result += getServiceName();
    
    return result;
}
