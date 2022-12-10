/************************************************************************
 * \file        ServiceClient.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of service client to
 *              request message output
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "ServiceClient.hpp"
#include "areg/trace/GETrace.h"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentThread.hpp"

DEF_TRACE_SCOPE(examples_11_locsvcmesh_ServiceClient_serviceConnected);
DEF_TRACE_SCOPE(examples_11_locsvcmesh_ServiceClient_broadcastReachedMaximum);
DEF_TRACE_SCOPE(examples_11_locsvcmesh_ServiceClient_responseHelloWorld);
DEF_TRACE_SCOPE(examples_11_locsvcmesh_ServiceClient_requestHelloWorldFailed);
DEF_TRACE_SCOPE(examples_11_locsvcmesh_ServiceClient_requestClientShutdownFailed);
DEF_TRACE_SCOPE(examples_11_locsvcmesh_ServiceClient_processTimer);
DEF_TRACE_SCOPE(examples_11_locsvcmesh_ServiceClient_ServiceClient);

ServiceClient::ServiceClient(const String & roleName, Component & owner)
    : HelloWorldClientBase  ( roleName, owner )
    , IETimerConsumer       ( )

    , mTimer                ( static_cast<IETimerConsumer &>(self()), timerName( owner ) )
    , mID                   ( 0 )
{
    TRACE_SCOPE(examples_11_locsvcmesh_ServiceClient_ServiceClient);
    TRACE_DBG("Client: roleName [ %s ] of service [ %s ] owner [ %s ] in thread [ %s ] has timer [ %s ]"
                    , roleName.getString()
                    , getServiceName().getString()
                    , owner.getRoleName().getString()
                    , owner.getMasterThread().getName().getString()
                    , mTimer.getName().getString());
    TRACE_DBG("Proxy: [ %s ]", ProxyAddress::convAddressToPath(getProxy()->getProxyAddress()).getString());
}

bool ServiceClient::serviceConnected(bool isConnected, ProxyBase & proxy)
{
    TRACE_SCOPE(examples_11_locsvcmesh_ServiceClient_serviceConnected);
    if ( HelloWorldClientBase::serviceConnected( isConnected, proxy ) )
    {
        TRACE_DBG( "Proxy [ %s ] is [ %s ]"
            , ProxyAddress::convAddressToPath( proxy.getProxyAddress( ) ).getString( )
            , isConnected ? "connected" : "disconnected" );

        notifyOnBroadcastReachedMaximum( isConnected );

        if ( isConnected )
        {
            mTimer.startTimer( ServiceClient::TIMEOUT_VALUE );    // dynamic subscribe.
        }
        else
        {
            mTimer.stopTimer( );
        }

        return true;
    }
    else
    {
        return false;
    }
}

void ServiceClient::responseHelloWorld(const NEHelloWorld::sConnectedClient & clientInfo)
{
    TRACE_SCOPE(examples_11_locsvcmesh_ServiceClient_responseHelloWorld);
    TRACE_DBG("Service [ %s ]: Made output of [ %s ], client ID [ %d ]", getServiceRole().getString(), clientInfo.ccName.getString(), clientInfo.ccID);
    ASSERT(clientInfo.ccName == mTimer.getName());
    mID = clientInfo.ccID;
}

void ServiceClient::broadcastReachedMaximum( int maxNumber )
{
    TRACE_SCOPE(examples_11_locsvcmesh_ServiceClient_broadcastReachedMaximum);
    TRACE_WARN("Service notify reached message output maximum, starting shutdown procedure");
    requestClientShutdown(mID, mTimer.getName());
}

void ServiceClient::requestHelloWorldFailed(NEService::eResultType FailureReason)
{
    TRACE_SCOPE(examples_11_locsvcmesh_ServiceClient_requestHelloWorldFailed);
    TRACE_ERR("Request to output greetings failed with reason [ %s ]", NEService::getString(FailureReason));
}

void ServiceClient::requestClientShutdownFailed(NEService::eResultType FailureReason)
{
    TRACE_SCOPE(examples_11_locsvcmesh_ServiceClient_requestClientShutdownFailed);
    TRACE_ERR("Request to notify client shutdown failed with reason [ %s ]", NEService::getString(FailureReason));
}

void ServiceClient::processTimer(Timer & timer)
{
    TRACE_SCOPE(examples_11_locsvcmesh_ServiceClient_processTimer);
    ASSERT(&timer == &mTimer);

    TRACE_DBG("Timer [ %s ] expired, send request to output message.", timer.getName().getString());
    requestHelloWorld(timer.getName());
}

inline String ServiceClient::timerName( Component & owner ) const
{
    ASSERT( getProxy( ) != nullptr );
    String result = "";
    result.append(getProxy()->getProxyAddress().getRoleName( ) )
          .append(NECommon::DEFAULT_SPECIAL_CHAR)
          .append(getServiceName());

    return result;
}
