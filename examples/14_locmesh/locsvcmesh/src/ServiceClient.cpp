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
#include "areg/logging/areg_log.h"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentThread.hpp"

DEF_LOG_SCOPE(examples_14_locsvcmesh_ServiceClient, ServiceClient);
DEF_LOG_SCOPE(examples_14_locsvcmesh_ServiceClient, service_connected);
DEF_LOG_SCOPE(examples_14_locsvcmesh_ServiceClient, broadcast_reached_maximum);
DEF_LOG_SCOPE(examples_14_locsvcmesh_ServiceClient, response_hello_world);
DEF_LOG_SCOPE(examples_14_locsvcmesh_ServiceClient, process_timer);

ServiceClient::ServiceClient(const areg::String & roleName, areg::Component & owner)
    : HelloWorldConsumerBase  ( roleName, owner )
    , areg::TimerConsumer       ( )

    , mTimer                ( static_cast<areg::TimerConsumer &>(self()), timer_name( owner ) )
    , mID                   ( 0 )
{
    LOG_SCOPE( examples_14_locsvcmesh_ServiceClient, ServiceClient );
    LOG_DBG("Client: roleName [ %s ] of service [ %s ] owner [ %s ] in thread [ %s ] has timer [ %s ]"
                    , roleName.as_string()
                    , service_name().as_string()
                    , owner.role_name().as_string()
                    , owner.master_thread().name().as_string()
                    , mTimer.name().as_string());
    LOG_DBG("Proxy: [ %s ]", areg::ProxyAddress::to_path(service_proxy()->proxy_address()).as_string());
}

bool ServiceClient::service_connected( areg::ServiceConnectionState status, areg::ProxyBase & proxy)
{
    LOG_SCOPE( examples_14_locsvcmesh_ServiceClient, service_connected );
    bool result = HelloWorldConsumerBase::service_connected( status, proxy );
    if ( is_connected( ) )
    {
        notify_on_broadcast_reached_maximum( true );
        mTimer.start_timer( ServiceClient::TIMEOUT_VALUE );
    }
    else
    {
        notify_on_broadcast_reached_maximum( false );
        mTimer.stop_timer( );
    }

    return result;
}

void ServiceClient::response_hello_world( const areg::String & clientName, uint32_t clientId )
{
    LOG_SCOPE( examples_14_locsvcmesh_ServiceClient, response_hello_world );
    LOG_DBG("Service [ %s ]: Made output of [ %s ], client ID [ %d ]", service_name().as_string(), clientName.as_string(), clientId);
    ASSERT(clientName == mTimer.name());
    mID = clientId;
}

void ServiceClient::broadcast_reached_maximum( int32_t /* maxNumber */ )
{
    LOG_SCOPE( examples_14_locsvcmesh_ServiceClient, broadcast_reached_maximum );
    LOG_WARN("Service notify reached message output maximum, starting shutdown procedure");
    request_shutdown_service(mID, mTimer.name());
}

void ServiceClient::process_timer(areg::Timer & timer)
{
    LOG_SCOPE( examples_14_locsvcmesh_ServiceClient, process_timer );
    ASSERT(&timer == &mTimer);

    LOG_DBG("Timer [ %s ] expired, send request to output message.", timer.name().as_string());
    request_hello_world(timer.name());
}

inline areg::String ServiceClient::timer_name( areg::Component & /* owner */ ) const
{
    ASSERT(service_proxy( ) != nullptr );
    areg::String result = "";
    result.append(service_name( ) )
          .append(areg::DEFAULT_SPECIAL_CHAR)
          .append(interface_name());

    return result;
}
