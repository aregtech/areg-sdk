/************************************************************************
 * \file        ServiceClient.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit examples
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

bool ServiceClient::serviceConnected( NEService::eServiceConnection status, ProxyBase & proxy)
{
    TRACE_SCOPE(examples_11_locsvcmesh_ServiceClient_serviceConnected);
    bool result = HelloWorldClientBase::serviceConnected( status, proxy );
    if ( isConnected( ) )
    {
        notifyOnBroadcastReachedMaximum( true );
        mTimer.startTimer( ServiceClient::TIMEOUT_VALUE );
    }
    else
    {
        notifyOnBroadcastReachedMaximum( false );
        mTimer.stopTimer( );
    }

    return result;
}

void ServiceClient::responseHelloWorld( const String & clientName, unsigned int clientId )
{
    TRACE_SCOPE(examples_11_locsvcmesh_ServiceClient_responseHelloWorld);
    TRACE_DBG("Service [ %s ]: Made output of [ %s ], client ID [ %d ]", getServiceRole().getString(), clientName.getString(), clientId);
    ASSERT(clientName == mTimer.getName());
    mID = clientId;
}

void ServiceClient::broadcastReachedMaximum( int maxNumber )
{
    TRACE_SCOPE(examples_11_locsvcmesh_ServiceClient_broadcastReachedMaximum);
    TRACE_WARN("Service notify reached message output maximum, starting shutdown procedure");
    requestShutdownService(mID, mTimer.getName());
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
    result.append( getServiceRole( ) )
          .append(NECommon::DEFAULT_SPECIAL_CHAR)
          .append(getServiceName());

    return result;
}
