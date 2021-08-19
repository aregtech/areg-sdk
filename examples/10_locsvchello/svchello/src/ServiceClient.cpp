/************************************************************************
 * \file        svchello/src/ServiceClient.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of service client to
 *              request message output
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "svchello/src/ServiceClient.hpp"
#include "areg/trace/GETrace.h"

DEF_TRACE_SCOPE(examples_10_svchello_ServiceClient_serviceConnected);
DEF_TRACE_SCOPE(examples_10_svchello_ServiceClient_onConnectedClientsUpdate);
DEF_TRACE_SCOPE(examples_10_svchello_ServiceClient_onRemainOutputUpdate);
DEF_TRACE_SCOPE(examples_10_svchello_ServiceClient_broadcastHelloClients);
DEF_TRACE_SCOPE(examples_10_svchello_ServiceClient_broadcastServiceUnavailable);
DEF_TRACE_SCOPE(examples_10_svchello_ServiceClient_responseHelloWorld);
DEF_TRACE_SCOPE(examples_10_svchello_ServiceClient_processTimer);

const unsigned int  ServiceClient::TIMEOUT_VALUE    = 237;


Component * ServiceClient::CreateComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
{
    return DEBUG_NEW ServiceClient(entry, owner);
}

void ServiceClient::DeleteComponent(Component & compObject, const NERegistry::ComponentEntry & entry)
{
    delete (&compObject);
}

ServiceClient::ServiceClient(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : Component             ( owner, entry.mRoleName)
    , HelloWorldClientBase  ( entry.mDependencyServices[0].mRoleName, static_cast<Component &>(self()) )
    , IETimerConsumer       ( )

    , mTimer                (static_cast<IETimerConsumer &>(self()), entry.mRoleName)
    , mID                   ( 0 )
{

}


ServiceClient::~ServiceClient(void)
{
}

bool ServiceClient::serviceConnected(bool isConnected, ProxyBase & proxy)
{
    TRACE_SCOPE(examples_10_svchello_ServiceClient_serviceConnected);
    bool result = HelloWorldClientBase::serviceConnected(isConnected, proxy);

    TRACE_DBG("Client [ %s ] of [ %s ] service is [ %s ]"
                , proxy.getProxyAddress().getRoleName().getString()
                , proxy.getProxyAddress().getServiceName().getString()
                , isConnected ? "connected to" : "disconnected from");

    if (isConnected)
    {
        // dynamic subscribe on messages.
        notifyOnRemainOutputUpdate(true);
        notifyOnBroadcastServiceUnavailable(true);
        mTimer.startTimer(ServiceClient::TIMEOUT_VALUE);
    }
    else
    {
        mTimer.stopTimer();
        // clear all subscriptions.
        clearAllNotifications();
    }

    return result;
}

void ServiceClient::onConnectedClientsUpdate(const NEHelloWorld::ConnectionList & ConnectedClients, NEService::eDataStateType state)
{
    TRACE_SCOPE(examples_10_svchello_ServiceClient_onConnectedClientsUpdate);
    TRACE_DBG("Active client list is updated. There are [ %d ] clients, data is [ %s ]", ConnectedClients.getSize(), NEService::getString(state));
}

void ServiceClient::onRemainOutputUpdate(short RemainOutput, NEService::eDataStateType state)
{
    TRACE_SCOPE(examples_10_svchello_ServiceClient_onRemainOutputUpdate);
    TRACE_DBG("Remain greeting outputs [ %d ], data is [ %s ]", RemainOutput, NEService::getString(state));
}

void ServiceClient::responseHelloWorld(const NEHelloWorld::sConnectedClient & clientInfo)
{
    TRACE_SCOPE(examples_10_svchello_ServiceClient_responseHelloWorld);
    TRACE_DBG("Greetings from [ %s ] output on console, client ID [ %d ]", clientInfo.ccName.getString(), clientInfo.ccID);
    ASSERT(clientInfo.ccName == getRoleName());
    mID = clientInfo.ccID;

    if (isNotificationAssigned(NEHelloWorld::MSG_ID_broadcastHelloClients) == false)
    {
        // If it is not subscribed on message, make subscription, which is cleaned when disconnect service.
        notifyOnBroadcastHelloClients(true);
        notifyOnRemainOutputUpdate(true);
    }
}

void ServiceClient::broadcastHelloClients(const NEHelloWorld::ConnectionList & clientList)
{
    TRACE_SCOPE(examples_10_svchello_ServiceClient_broadcastHelloClients);
    TRACE_DBG("[ %d ] clients use service [ %s ]", clientList.getSize(), getServiceName().getString());
}

void ServiceClient::broadcastServiceUnavailable(void)
{
    TRACE_SCOPE(examples_10_svchello_ServiceClient_broadcastServiceUnavailable);
    TRACE_WARN("Service notify reached message output maximum, starting shutdown procedure");
    requestClientShutdown(mID, getRoleName());
}

void ServiceClient::processTimer(Timer & timer)
{
    TRACE_SCOPE(examples_10_svchello_ServiceClient_processTimer);
    ASSERT(&timer == &mTimer);

    TRACE_DBG("Timer [ %s ] expired, send request to output message.", timer.getName().getString());
    requestHelloWorld(getRoleName(), "");
}
