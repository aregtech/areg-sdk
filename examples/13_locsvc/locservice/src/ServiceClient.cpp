/************************************************************************
 * \file        locservice/src/ServiceClient.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This file contains simple implementation of service client to
 *              request message output
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "locservice/src/ServiceClient.hpp"
#include "areg/logging/areg_log.h"

DEF_LOG_SCOPE(examples_13_locservice_ServiceClient, service_connected);
DEF_LOG_SCOPE(examples_13_locservice_ServiceClient, broadcast_reached_maximum);
DEF_LOG_SCOPE(examples_13_locservice_ServiceClient, response_hello_world);
DEF_LOG_SCOPE(examples_13_locservice_ServiceClient, process_timer);

ServiceClient::ServiceClient(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    : areg::Component             ( entry, owner )
    , HelloWorldConsumerBase  ( entry.mDependencyServices[0].mRoleName, static_cast<areg::Component &>(self()) )
    , areg::TimerConsumer       ( )

    , mTimer                (static_cast<areg::TimerConsumer &>(self()), entry.mRoleName)
{
}

bool ServiceClient::service_connected( areg::ServiceConnectionState status, areg::ProxyBase & proxy)
{
    LOG_SCOPE( examples_13_locservice_ServiceClient, service_connected );
    bool result = HelloWorldConsumerBase::service_connected( status, proxy );
    // subscribe when service connected and un-subscribe when disconnected.
    if ( is_connected( ) )
    {
        notify_on_broadcast_reached_maximum(true);
        mTimer.start_timer( ServiceClient::TIMEOUT_VALUE );
    }
    else
    {
        notify_on_broadcast_reached_maximum( false );
        mTimer.stop_timer( );
    }

    return result;
}

void ServiceClient::response_hello_world()
{
    LOG_SCOPE( examples_13_locservice_ServiceClient, response_hello_world );
    LOG_DBG("Received response on request to print greetings from the client");
}

void ServiceClient::broadcast_reached_maximum( [[maybe_unused]] int32_t maxNumber )
{
    LOG_SCOPE( examples_13_locservice_ServiceClient, broadcast_reached_maximum );
    LOG_WARN("Service notify reached maximum number of requests [ %d ], starting shutdown procedure", maxNumber );
    request_shutdown_service( );
}

void ServiceClient::process_timer([[maybe_unused]] areg::Timer & timer)
{
    LOG_SCOPE( examples_13_locservice_ServiceClient, process_timer );
    ASSERT(&timer == &mTimer);

    LOG_DBG("Timer [ %s ] expired, send request to output message.", timer.name().as_string());
    request_hello_world(role_name());
}
