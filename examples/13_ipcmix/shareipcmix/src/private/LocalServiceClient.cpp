/************************************************************************
 * \file        shareipcmix/src/private/LocalServiceClient.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of service client to
 *              request message output
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "shareipcmix/src/LocalServiceClient.hpp"
#include "areg/trace/GETrace.h"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentThread.hpp"

DEF_TRACE_SCOPE(examples_13_ipcmix_shareipcmix_LocalServiceClient_serviceConnected);
DEF_TRACE_SCOPE(examples_13_ipcmix_shareipcmix_LocalServiceClient_onConnectedClientsUpdate);
DEF_TRACE_SCOPE(examples_13_ipcmix_shareipcmix_LocalServiceClient_onRemainOutputUpdate);
DEF_TRACE_SCOPE(examples_13_ipcmix_shareipcmix_LocalServiceClient_broadcastHelloClients);
DEF_TRACE_SCOPE(examples_13_ipcmix_shareipcmix_LocalServiceClient_broadcastServiceUnavailable);
DEF_TRACE_SCOPE(examples_13_ipcmix_shareipcmix_LocalServiceClient_responseHelloWorld);
DEF_TRACE_SCOPE(examples_13_ipcmix_shareipcmix_LocalServiceClient_requestHelloWorldFailed);
DEF_TRACE_SCOPE(examples_13_ipcmix_shareipcmix_LocalServiceClient_requestClientShutdownFailed);
DEF_TRACE_SCOPE(examples_13_ipcmix_shareipcmix_LocalServiceClient_processTimer);
DEF_TRACE_SCOPE(examples_13_ipcmix_shareipcmix_LocalServiceClient_LocalServiceClient);

LocalServiceClient::LocalServiceClient(const String & roleName, Component & owner, unsigned int timeout)
    : LocalHelloWorldClientBase ( roleName, owner                                               )
    , IETimerConsumer           (                                                               )

    , mMsTimeout                ( timeout                                                       )
    , mTimer                    ( static_cast<IETimerConsumer &>(self()), timerName( owner )    )
    , mID                       ( 0                                                             )
{
    TRACE_SCOPE(examples_13_ipcmix_shareipcmix_LocalServiceClient_LocalServiceClient);
    TRACE_DBG("Client: roleName [ %s ] of service [ %s ] owner [ %s ] in thread [ %s ] has timer [ %s ]"
                    , roleName.getString()
                    , getServiceName().getString()
                    , owner.getRoleName().getString()
                    , owner.getMasterThread().getName().getString()
                    , mTimer.getName().getString());
    TRACE_DBG("Proxy: [ %s ]", ProxyAddress::convAddressToPath(getProxy()->getProxyAddress()).getString());
}

LocalServiceClient::~LocalServiceClient(void)
{
}

bool LocalServiceClient::serviceConnected(bool isConnected, ProxyBase & proxy)
{
    TRACE_SCOPE(examples_13_ipcmix_shareipcmix_LocalServiceClient_serviceConnected);
    bool result = LocalHelloWorldClientBase::serviceConnected(isConnected, proxy);

    TRACE_DBG("Proxy [ %s ] is [ %s ]"
                , ProxyAddress::convAddressToPath(proxy.getProxyAddress()).getString()
                , isConnected ? "connected" : "disconnected");

    if (isConnected)
    {
        notifyOnRemainOutputUpdate(true);
        notifyOnBroadcastServiceUnavailable(true);
        mTimer.startTimer(mMsTimeout);
    }
    else
    {
        mTimer.stopTimer();
        clearAllNotifications();
    }

    return result;
}

void LocalServiceClient::onConnectedClientsUpdate(const NELocalHelloWorld::ConnectionList & ConnectedClients, NEService::eDataStateType state)
{
    TRACE_SCOPE(examples_13_ipcmix_shareipcmix_LocalServiceClient_onConnectedClientsUpdate);
    TRACE_DBG("Active client list of [ %s ] service is updated, active clients [ %d ], data is [ %s ]"
                    , getServiceRole().getString()
                    , ConnectedClients.getSize()
                    , NEService::getString(state));
}

void LocalServiceClient::onRemainOutputUpdate(short RemainOutput, NEService::eDataStateType state)
{
    TRACE_SCOPE(examples_13_ipcmix_shareipcmix_LocalServiceClient_onRemainOutputUpdate);
    TRACE_DBG("Service [ %s ]: Remain greeting outputs [ %d ], data is [ %s ]", getServiceRole().getString(), static_cast<int>(RemainOutput), NEService::getString(state));
}

void LocalServiceClient::responseHelloWorld(const NELocalHelloWorld::sConnectedClient & clientInfo)
{
    TRACE_SCOPE(examples_13_ipcmix_shareipcmix_LocalServiceClient_responseHelloWorld);
    TRACE_DBG("Service [ %s ]: Made output of [ %s ], client ID [ %d ]", getServiceRole().getString(), clientInfo.ccName.getString(), clientInfo.ccID);
    ASSERT(clientInfo.ccName == mTimer.getName());
    mID = clientInfo.ccID;

    if (isNotificationAssigned(NELocalHelloWorld::MSG_ID_broadcastHelloClients) == false)
    {
        notifyOnBroadcastHelloClients(true);
        notifyOnConnectedClientsUpdate(true);
    }
}

void LocalServiceClient::broadcastHelloClients(const NELocalHelloWorld::ConnectionList & clientList)
{
    TRACE_SCOPE(examples_13_ipcmix_shareipcmix_LocalServiceClient_broadcastHelloClients);

    TRACE_DBG("[ %d ] clients use service [ %s ]", clientList.getSize(), getServiceName().getString());
}

void LocalServiceClient::broadcastServiceUnavailable(void)
{
    TRACE_SCOPE(examples_13_ipcmix_shareipcmix_LocalServiceClient_broadcastServiceUnavailable);
    TRACE_WARN("Service notify reached message output maximum, starting shutdown procedure");
    requestClientShutdown(mID, mTimer.getName());
}

void LocalServiceClient::requestHelloWorldFailed(NEService::eResultType FailureReason)
{
    TRACE_SCOPE(examples_13_ipcmix_shareipcmix_LocalServiceClient_requestHelloWorldFailed);
    TRACE_ERR("Request to output greetings failed with reason [ %s ]", NEService::getString(FailureReason));
}

void LocalServiceClient::requestClientShutdownFailed(NEService::eResultType FailureReason)
{
    TRACE_SCOPE(examples_13_ipcmix_shareipcmix_LocalServiceClient_requestClientShutdownFailed);
    TRACE_ERR("Request to notify client shutdown failed with reason [ %s ]", NEService::getString(FailureReason));
}

void LocalServiceClient::processTimer(Timer & timer)
{
    TRACE_SCOPE(examples_13_ipcmix_shareipcmix_LocalServiceClient_processTimer);
    ASSERT(&timer == &mTimer);

    TRACE_DBG("Timer [ %s ] expired, send request to output message.", timer.getName().getString());
    requestHelloWorld(timer.getName(), "");
}

inline String LocalServiceClient::timerName( Component & owner ) const
{
    String result = "";
    result += owner.getRoleName();
    result += NEUtilities::DEFAULT_SPECIAL_CHAR;
    result += getServiceRole();
    result += NEUtilities::DEFAULT_SPECIAL_CHAR;
    result += getServiceName();
    
    return result;
}
