/************************************************************************
 * \file        generated/src/private/RemoteServiceClient.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of service client to
 *              request message output
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "generated/src/RemoteServiceClient.hpp"

#include "generated/src/NECommon.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/ProxyBase.hpp"
#include "areg/component/Component.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/base/Process.hpp"
#include "areg/trace/GETrace.h"

DEF_TRACE_SCOPE(examples_13_pubmesh_generated_RemoteServiceClient_RemoteServiceClient);
DEF_TRACE_SCOPE(examples_13_pubmesh_generated_RemoteServiceClient_serviceConnected);
DEF_TRACE_SCOPE(examples_13_pubmesh_generated_RemoteServiceClient_responseRegister);
DEF_TRACE_SCOPE(examples_13_pubmesh_generated_RemoteServiceClient_responseHelloWorld);
DEF_TRACE_SCOPE(examples_13_pubmesh_generated_RemoteServiceClient_broadcastServiceUnavailable);
DEF_TRACE_SCOPE(examples_13_pubmesh_generated_RemoteServiceClient_onServiceStateUpdate);
DEF_TRACE_SCOPE(examples_13_pubmesh_generated_RemoteServiceClient_processTimer);

RemoteServiceClient::RemoteServiceClient(const String & roleName, Component & owner, unsigned int timeout)
    : RemoteRegistryClientBase  ( roleName, owner                                               )
    , SystemShutdownClientBase  ( NECommon::MainService, owner                              )
    , IETimerConsumer           (                                                               )

    , mMsTimeout                ( timeout                                                       )
    , mTimer                    ( static_cast<IETimerConsumer &>(self()), timerName( owner )    )
    , mClient                   (                                                               )
{
    TRACE_SCOPE(examples_13_pubmesh_generated_RemoteServiceClient_RemoteServiceClient);
    TRACE_DBG("Client: roleName [ %s ] of service [ %s ] owner [ %s ] in thread [ %s ] has timer [ %s ] with timeout [ %d ] ms"
                    , roleName.getString()
                    , RemoteRegistryClientBase::getServiceName().getString()
                    , owner.getRoleName().getString()
                    , owner.getMasterThread().getName().getString()
                    , mTimer.getName().getString()
                    , timeout);
    TRACE_DBG("Proxy: [ %s ]", ProxyAddress::convAddressToPath(RemoteRegistryClientBase::getProxy()->getProxyAddress()).getString());
}

bool RemoteServiceClient::serviceConnected(bool isConnected, ProxyBase & proxy)
{
    TRACE_SCOPE(examples_13_pubmesh_generated_RemoteServiceClient_serviceConnected);
    TRACE_DBG("Client [ %p ] - [ %s ] is [ %s ]", this, ProxyAddress::convAddressToPath(proxy.getProxyAddress()).getString(), isConnected ? "CONNECTED" : "DISCONNECTED");

    if ( RemoteRegistryClientBase::serviceConnected(isConnected, proxy) )
    {
        // On connect it has no effect, because the timer is stopped.
        mTimer.stopTimer();
        // Reset the ID here. Otherwise, it keeps old value when service connection lost.
        mClient.crID = 0;
        if (isConnected )
        {
            TRACE_DBG("Client [ %p ]-[ %s ] requests to be registered", this, mTimer.getName().getString());
            requestRegister(mTimer.getName(), proxy.getProxyAddress(), proxy.getProxyDispatcherThread().getName(), Process::getInstance().getAppName());
        }

        return true;
    }
    else if (SystemShutdownClientBase::serviceConnected(isConnected, proxy))
    {
        TRACE_DBG("Client [ %p ]-[ %s ]subscribes on service unavailable and service state update messages", this, mTimer.getName().getString());
        notifyOnBroadcastServiceUnavailable(isConnected);
        notifyOnServiceStateUpdate(isConnected);

        return true;
    }
    else
    {
        return false;
    }
}

void RemoteServiceClient::responseRegister( const NERemoteRegistry::sClientRegister & client )
{
    TRACE_SCOPE(examples_13_pubmesh_generated_RemoteServiceClient_responseRegister);

    if (client.crID != 0)
    {
        ASSERT(client.crName == mTimer.getName());

        TRACE_INFO("The client [ %s ] is registered and got ID [ %d ], can use services", client.crName.getString(), client.crID);
        mClient = client;
        mTimer.startTimer(mMsTimeout, RemoteRegistryClientBase::getProxy()->getProxyDispatcherThread());
    }
    else
    {
        TRACE_ERR("Failed to register, cannot use service!");
    }

}

void RemoteServiceClient::responseHelloWorld(unsigned int clientID)
{
    TRACE_SCOPE(examples_13_pubmesh_generated_RemoteServiceClient_responseHelloWorld);
    if ( (clientID != 0) && (mClient.crID == clientID) )
    {
        TRACE_DBG("Client [ %s ] SUCCEEDED to make output on remote service [ %s ]", mClient.crName.getString(), RemoteRegistryClientBase::getServiceRole().getString());
    }
    else
    {
        TRACE_DBG("Client [ %s ] FAILED to make output on remote service [ %s ]", mClient.crName.getString(), RemoteRegistryClientBase::getServiceRole().getString());
        mTimer.stopTimer();
        requestUnregister(mClient);
        mClient.crID = 0;
    }
}

void RemoteServiceClient::broadcastServiceUnavailable(void)
{
    TRACE_SCOPE(examples_13_pubmesh_generated_RemoteServiceClient_broadcastServiceUnavailable);
    TRACE_WARN("Service reached message output maximum, starting shutdown procedure");

    mTimer.stopTimer();
    if (mClient.crID != 0)
    {
        requestUnregister(mClient);
        mClient.crID = 0;
    }
}

void RemoteServiceClient::onServiceStateUpdate( NESystemShutdown::eServiceState ServiceState, NEService::eDataStateType state )
{
    TRACE_SCOPE(examples_13_pubmesh_generated_RemoteServiceClient_onServiceStateUpdate);
    TRACE_DBG("Service state updated [ %s ], data state [ %s ]", NESystemShutdown::getString(ServiceState), NEService::getString(state));

    if (state == NEService::eDataStateType::DataIsOK)
    {
        if (ServiceState != NESystemShutdown::eServiceState::ServiceReady)
        {
            mTimer.stopTimer();
            if (mClient.crID != 0)
            {
                requestUnregister(mClient);
                mClient.crID = 0;
            }
        }
        else if ( (mClient.crID != 0) && (mTimer.isActive() == false) )
        {
            mTimer.startTimer(mMsTimeout, RemoteRegistryClientBase::getProxy()->getProxyDispatcherThread());
        }
    }
}


void RemoteServiceClient::processTimer(Timer & timer)
{
    TRACE_SCOPE(examples_13_pubmesh_generated_RemoteServiceClient_processTimer);
    ASSERT(&timer == &mTimer);

    TRACE_DBG("Timer [ %s ] of client ID [ %d ] has expired, send request to output message.", timer.getName().getString(), mClient.crID);

    NEService::eDataStateType dataState = NEService::eDataStateType::DataIsInvalid;
    if ( (getServiceState(dataState) == NESystemShutdown::eServiceState::ServiceReady) && (dataState == NEService::eDataStateType::DataIsOK))
    {
        requestHelloWorld(mClient.crID);
    }
    else
    {
        TRACE_WARN("Ignored sending request, the service state is not active anymore");
    }
}

inline String RemoteServiceClient::timerName( Component & owner ) const
{
    String result = "";
    result.append(owner.getRoleName())
          .append(NECommon::DEFAULT_SPECIAL_CHAR)
          .append(RemoteRegistryClientBase::getServiceRole())
          .append(NECommon::DEFAULT_SPECIAL_CHAR)
          .append(RemoteRegistryClientBase::getServiceName());

    return result;
}
