/************************************************************************
 * \file        shareipcmix/src/private/RemoteServiceClient.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of service client to
 *              request message output
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "shareipcmix/src/RemoteServiceClient.hpp"
#include "areg/trace/GETrace.h"
#include "areg/component/ComponentThread.hpp"
#include "areg/appbase/Application.hpp"

DEF_TRACE_SCOPE(examples_13_ipcmix_shareipcmix_RemoteServiceClient_serviceConnected);
DEF_TRACE_SCOPE(examples_13_ipcmix_shareipcmix_RemoteServiceClient_onConnectedClientsUpdate);
DEF_TRACE_SCOPE(examples_13_ipcmix_shareipcmix_RemoteServiceClient_onRemainOutputUpdate);
DEF_TRACE_SCOPE(examples_13_ipcmix_shareipcmix_RemoteServiceClient_broadcastHelloClients);
DEF_TRACE_SCOPE(examples_13_ipcmix_shareipcmix_RemoteServiceClient_broadcastServiceUnavailable);
DEF_TRACE_SCOPE(examples_13_ipcmix_shareipcmix_RemoteServiceClient_responseHelloWorld);
DEF_TRACE_SCOPE(examples_13_ipcmix_shareipcmix_RemoteServiceClient_requestHelloWorldFailed);
DEF_TRACE_SCOPE(examples_13_ipcmix_shareipcmix_RemoteServiceClient_requestClientShutdownFailed);
DEF_TRACE_SCOPE(examples_13_ipcmix_shareipcmix_RemoteServiceClient_processTimer);
DEF_TRACE_SCOPE(examples_13_ipcmix_shareipcmix_RemoteServiceClient_RemoteServiceClient);

RemoteServiceClient::RemoteServiceClient(const String & roleName, Component & owner, unsigned int timeout)
    : RemoteHelloWorldClientBase( roleName, owner                                               )
    , IETimerConsumer           (                                                               )

    , mMsTimeout                ( timeout                                                       )
    , mTimer                    ( static_cast<IETimerConsumer &>(self()), timerName( owner )    )
    , mID                       ( 0                                                             )
{
    TRACE_SCOPE(examples_13_ipcmix_shareipcmix_RemoteServiceClient_RemoteServiceClient);
    TRACE_DBG("Client: roleName [ %s ] of service [ %s ] owner [ %s ] in thread [ %s ] has timer [ %s ] with timeout [ %d ] ms"
                    , roleName.getString()
                    , getServiceName().getString()
                    , owner.getRoleName().getString()
                    , owner.getMasterThread().getName().getString()
                    , mTimer.getName().getString()
                    , timeout);
    TRACE_DBG("Proxy: [ %s ]", ProxyAddress::convAddressToPath(getProxy()->getProxyAddress()).getString());
}

RemoteServiceClient::~RemoteServiceClient(void)
{
}

bool RemoteServiceClient::serviceConnected(bool isConnected, ProxyBase & proxy)
{
    TRACE_SCOPE(examples_13_ipcmix_shareipcmix_RemoteServiceClient_serviceConnected);
    bool result = RemoteHelloWorldClientBase::serviceConnected(isConnected, proxy);

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
        TRACE_WARN("Shutting down application!");
        mTimer.stopTimer();
        clearAllNotifications();
        Application::signalAppQuit();
    }

    return result;
}

void RemoteServiceClient::onConnectedClientsUpdate(const NERemoteHelloWorld::ConnectionList & ConnectedClients, NEService::eDataStateType state)
{
    TRACE_SCOPE(examples_13_ipcmix_shareipcmix_RemoteServiceClient_onConnectedClientsUpdate);
    TRACE_DBG("Active client list of [ %s ] service is updated, active clients [ %d ], data is [ %s ]"
                    , getServiceRole().getString()
                    , ConnectedClients.getSize()
                    , NEService::getString(state));
}

void RemoteServiceClient::onRemainOutputUpdate(short RemainOutput, NEService::eDataStateType state)
{
    TRACE_SCOPE(examples_13_ipcmix_shareipcmix_RemoteServiceClient_onRemainOutputUpdate);
    TRACE_DBG("Service [ %s ]: Remain greeting outputs [ %d ], data is [ %s ]", getServiceRole().getString(), RemainOutput, NEService::getString(state));
}

void RemoteServiceClient::responseHelloWorld(const NERemoteHelloWorld::sConnectedClient & clientInfo)
{
    TRACE_SCOPE(examples_13_ipcmix_shareipcmix_RemoteServiceClient_responseHelloWorld);
    TRACE_DBG("Service [ %s ]: Made output of [ %s ], client ID [ %d ]", getServiceRole().getString(), clientInfo.ccName.getString(), clientInfo.ccID);
    ASSERT(clientInfo.ccName == mTimer.getName());

    if ( mID == 0)
    {
        TRACE_DBG("Registring ID [ %d ] for service client [ %s ]", clientInfo.ccID, mTimer.getName().getString());
        mID = clientInfo.ccID;
    }

    if (isNotificationAssigned(NERemoteHelloWorld::MSG_ID_broadcastHelloClients) == false)
    {
        notifyOnBroadcastHelloClients(true);
        notifyOnConnectedClientsUpdate(true);
    }
}

void RemoteServiceClient::broadcastHelloClients(const NERemoteHelloWorld::ConnectionList & clientList)
{
    TRACE_SCOPE(examples_13_ipcmix_shareipcmix_RemoteServiceClient_broadcastHelloClients);

    TRACE_DBG("[ %d ] clients use service [ %s ]", clientList.getSize(), getServiceName().getString());
}

void RemoteServiceClient::broadcastServiceUnavailable(void)
{
    TRACE_SCOPE(examples_13_ipcmix_shareipcmix_RemoteServiceClient_broadcastServiceUnavailable);
    TRACE_WARN("Service reached message output maximum, starting shutdown procedure");
    TRACE_INFO("Sending request to shutdown service for client [ %s ] with ID [ %d ]", mTimer.getName().getString(), mID);

    requestClientShutdown(mID, mTimer.getName());
    mID = 0;
}

void RemoteServiceClient::requestHelloWorldFailed(NEService::eResultType FailureReason)
{
    TRACE_SCOPE(examples_13_ipcmix_shareipcmix_RemoteServiceClient_requestHelloWorldFailed);
    TRACE_ERR("Request to output greetings failed with reason [ %s ]", NEService::getString(FailureReason));
}

void RemoteServiceClient::requestClientShutdownFailed(NEService::eResultType FailureReason)
{
    TRACE_SCOPE(examples_13_ipcmix_shareipcmix_RemoteServiceClient_requestClientShutdownFailed);
    TRACE_ERR("Request to notify client shutdown failed with reason [ %s ]", NEService::getString(FailureReason));
}

void RemoteServiceClient::processTimer(Timer & timer)
{
    TRACE_SCOPE(examples_13_ipcmix_shareipcmix_RemoteServiceClient_processTimer);
    ASSERT(&timer == &mTimer);

    TRACE_DBG("Timer [ %s ] expired, send request to output message.", timer.getName().getString());
    requestHelloWorld(timer.getName(), "remote");
}

inline String RemoteServiceClient::timerName( Component & owner ) const
{
    String result = "";
    result += owner.getRoleName();
    result += NEUtilities::DEFAULT_SPECIAL_CHAR;
    result += getServiceRole();
    result += NEUtilities::DEFAULT_SPECIAL_CHAR;
    result += getServiceName();
    
    return result;
}
