/************************************************************************
 * \file        ServiceClient.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This file contains simple implementation of service client to
 *              request message output
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "ServiceClient.hpp"
#include "areg/logging/GELog.h"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentThread.hpp"

DEF_LOG_SCOPE(examples_14_locsvcmesh_ServiceClient_serviceConnected);
DEF_LOG_SCOPE(examples_14_locsvcmesh_ServiceClient_broadcastReachedMaximum);
DEF_LOG_SCOPE(examples_14_locsvcmesh_ServiceClient_responseHelloWorld);
DEF_LOG_SCOPE(examples_14_locsvcmesh_ServiceClient_process_timer);
DEF_LOG_SCOPE(examples_14_locsvcmesh_ServiceClient_ServiceClient);

ServiceClient::ServiceClient(const String & roleName, Component & owner)
    : HelloWorldClientBase  ( roleName, owner )
    , TimerConsumer       ( )

    , mTimer                ( static_cast<TimerConsumer &>(self()), timerName( owner ) )
    , mID                   ( 0 )
{
    LOG_SCOPE(examples_14_locsvcmesh_ServiceClient_ServiceClient);
    LOG_DBG("Client: roleName [ %s ] of service [ %s ] owner [ %s ] in thread [ %s ] has timer [ %s ]"
                    , roleName.as_string()
                    , getServiceName().as_string()
                    , owner.getRoleName().as_string()
                    , owner.getMasterThread().name().as_string()
                    , mTimer.name().as_string());
    LOG_DBG("Proxy: [ %s ]", ProxyAddress::convAddressToPath(getProxy()->getProxyAddress()).as_string());
}

bool ServiceClient::service_connected( NEService::ServiceConnectionState status, ProxyBase & proxy)
{
    LOG_SCOPE(examples_14_locsvcmesh_ServiceClient_serviceConnected);
    bool result = HelloWorldClientBase::service_connected( status, proxy );
    if ( isConnected( ) )
    {
        notifyOnBroadcastReachedMaximum( true );
        mTimer.start_timer( ServiceClient::TIMEOUT_VALUE );
    }
    else
    {
        notifyOnBroadcastReachedMaximum( false );
        mTimer.stop_timer( );
    }

    return result;
}

void ServiceClient::responseHelloWorld( const String & clientName, uint32_t clientId )
{
    LOG_SCOPE(examples_14_locsvcmesh_ServiceClient_responseHelloWorld);
    LOG_DBG("Service [ %s ]: Made output of [ %s ], client ID [ %d ]", getServiceRole().as_string(), clientName.as_string(), clientId);
    ASSERT(clientName == mTimer.name());
    mID = clientId;
}

void ServiceClient::broadcastReachedMaximum( int32_t /* maxNumber */ )
{
    LOG_SCOPE(examples_14_locsvcmesh_ServiceClient_broadcastReachedMaximum);
    LOG_WARN("Service notify reached message output maximum, starting shutdown procedure");
    requestShutdownService(mID, mTimer.name());
}

void ServiceClient::process_timer(Timer & timer)
{
    LOG_SCOPE(examples_14_locsvcmesh_ServiceClient_process_timer);
    ASSERT(&timer == &mTimer);

    LOG_DBG("Timer [ %s ] expired, send request to output message.", timer.name().as_string());
    requestHelloWorld(timer.name());
}

inline String ServiceClient::timerName( Component & /* owner */ ) const
{
    ASSERT( getProxy( ) != nullptr );
    String result = "";
    result.append( getServiceRole( ) )
          .append(NECommon::DEFAULT_SPECIAL_CHAR)
          .append(getServiceName());

    return result;
}
