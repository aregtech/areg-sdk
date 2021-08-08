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
#include "shareipcmix/src/IPCMixCommon.hpp"

DEF_TRACE_SCOPE(examples_13_ipcmix_shareipcmix_LocalServiceClient_serviceConnected);
DEF_TRACE_SCOPE(examples_13_ipcmix_shareipcmix_LocalServiceClient_responseHelloWorld);
DEF_TRACE_SCOPE(examples_13_ipcmix_shareipcmix_LocalServiceClient_processTimer);
DEF_TRACE_SCOPE(examples_13_ipcmix_shareipcmix_LocalServiceClient_LocalServiceClient);

LocalServiceClient::LocalServiceClient(const String & roleName, Component & owner, unsigned int timeout)
    : LocalHelloWorldClientBase ( roleName, owner                                               )
    , SystemShutdownClientBase  ( IPCMixCommon::MainService, owner                              )
    , IETimerConsumer           (                                                               )

    , mMsTimeout                ( timeout                                                       )
    , mTimer                    ( static_cast<IETimerConsumer &>(self()), timerName( owner )    )
    , mID                       ( 0                                                             )
{
    TRACE_SCOPE(examples_13_ipcmix_shareipcmix_LocalServiceClient_LocalServiceClient);
    TRACE_DBG("Client: roleName [ %s ] of service [ %s ] owner [ %s ] in thread [ %s ] has timer [ %s ] with timeout [ %d ] ms"
                    , roleName.getString()
                    , LocalHelloWorldClientBase::getServiceName().getString()
                    , owner.getRoleName().getString()
                    , owner.getMasterThread().getName().getString()
                    , mTimer.getName().getString()
                    , timeout);
    TRACE_DBG("Proxy: [ %s ]", ProxyAddress::convAddressToPath(LocalHelloWorldClientBase::getProxy()->getProxyAddress()).getString());
}

LocalServiceClient::~LocalServiceClient(void)
{
}

bool LocalServiceClient::serviceConnected(bool isConnected, ProxyBase & proxy)
{
    TRACE_SCOPE(examples_13_ipcmix_shareipcmix_LocalServiceClient_serviceConnected);
    TRACE_DBG("Client [ %p ]: Proxy [ %s ] is [ %s ]"
                , this
                , ProxyAddress::convAddressToPath(proxy.getProxyAddress()).getString()
                , isConnected ? "CONNECTED" : "DISCONNECTED");

    if (LocalHelloWorldClientBase::serviceConnected(isConnected, proxy) )
    {
        if (isConnected)
        {
            TRACE_DBG("Starting timer with timeout [ %d ] ms", mMsTimeout);
            mTimer.startTimer(mMsTimeout, LocalHelloWorldClientBase::getProxy()->getProxyDispatcherThread());
        }
        else
        {
            mTimer.stopTimer();
            LocalHelloWorldClientBase::clearAllNotifications();
        }

        return true;
    }
    else if (SystemShutdownClientBase::serviceConnected(isConnected, proxy))
    {
        TRACE_DBG("Proxy [ %s ] is [ %s ]", ProxyAddress::convAddressToPath(proxy.getProxyAddress()).getString(), isConnected ? "CONNECTED" : "DISCONNECTED");
        notifyOnBroadcastServiceUnavailable(isConnected);
        return true;
    }
    else
    {
        return false;
    }
}

void LocalServiceClient::responseHelloWorld(const NELocalHelloWorld::sConnectedClient & clientInfo)
{
    TRACE_SCOPE(examples_13_ipcmix_shareipcmix_LocalServiceClient_responseHelloWorld);
    TRACE_DBG("Service [ %s ]: Made output of [ %s ], client ID [ %d ]", LocalHelloWorldClientBase::getServiceRole().getString(), clientInfo.ccName.getString(), clientInfo.ccID);
    
    ASSERT(clientInfo.ccName == mTimer.getName());
    mID = clientInfo.ccID;
}

void LocalServiceClient::processTimer(Timer & timer)
{
    TRACE_SCOPE(examples_13_ipcmix_shareipcmix_LocalServiceClient_processTimer);
    ASSERT(&timer == &mTimer);

    TRACE_DBG("Timer [ %s ] expired, send request to output message.", timer.getName().getString());
    if (timer.isActive())
    {
        requestHelloWorld(timer.getName(), "");
    }
}

inline String LocalServiceClient::timerName( Component & owner ) const
{
    String result = "";
    result += owner.getRoleName();
    result += NEUtilities::DEFAULT_SPECIAL_CHAR;
    result += LocalHelloWorldClientBase::getServiceRole();
    result += NEUtilities::DEFAULT_SPECIAL_CHAR;
    result += LocalHelloWorldClientBase::getServiceName();
    
    return result;
}
