/************************************************************************
 * \file        pubclient/src/ServiceClient.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This file contains simple implementation of service client to
 *              request message output
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "pubclient/src/ServiceClient.hpp"
#include "areg/logging/areg_log.h"
#include "areg/appbase/Application.hpp"

DEF_LOG_SCOPE(examples_15_pubclient_ServiceClient, service_connected);
DEF_LOG_SCOPE(examples_15_pubclient_ServiceClient, broadcast_reached_maximum);
DEF_LOG_SCOPE(examples_15_pubclient_ServiceClient, response_hello_world);
DEF_LOG_SCOPE(examples_15_pubclient_ServiceClient, process_timer);

ServiceClient::ServiceClient(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    : areg::Component             ( areg::generate_name(entry.mRoleName), owner )
    , HelloWorldConsumerBase  ( entry.mDependencyServices[0].mRoleName, static_cast<areg::Component &>(self()) )
    , areg::TimerConsumer       ( )

    , mTimer                (static_cast<areg::TimerConsumer &>(self()), entry.mRoleName)
    , mID                   ( 0 )
{
}

bool ServiceClient::service_connected( areg::ServiceConnectionState status, areg::ProxyBase & proxy )
{
    LOG_SCOPE( examples_15_pubclient_ServiceClient, service_connected );
    bool result = HelloWorldConsumerBase::service_connected( status, proxy );

    // subscribe when service connected and un-subscribe when disconnected.
    notify_on_broadcast_reached_maximum( is_connected( ) );
    if ( is_connected( ) )
    {
        mTimer.start_timer( ServiceClient::TIMEOUT_VALUE );
    }
    else if (is_connection_lost( status ) )
    {
        LOG_WARN( "The connection is lost! Waiting for connection recovery!" );
        mTimer.stop_timer( );
    }
    else
    {
        LOG_WARN("Shutting down application!");
        mTimer.stop_timer( );
        areg::Application::signal_quit();
    }

    return result;
}

void ServiceClient::response_hello_world(const HelloWorld::sConnectedClient & clientInfo)
{
    LOG_SCOPE( examples_15_pubclient_ServiceClient, response_hello_world );
    LOG_DBG("Greetings from [ %s ] output on console, client ID [ %d ]", clientInfo.ccName.as_string(), clientInfo.ccID);
    ASSERT(clientInfo.ccName == role_name());
    mID = clientInfo.ccID;
}

void ServiceClient::broadcast_reached_maximum( [[maybe_unused]] int32_t maxNumber )
{
    LOG_SCOPE( examples_15_pubclient_ServiceClient, broadcast_reached_maximum );
    LOG_WARN("Service notify reached maximum number of requests [ %d ], starting shutdown procedure", maxNumber );
    request_shutdown_service(mID, role_name());
    mID = 0;
}

void ServiceClient::process_timer([[maybe_unused]] areg::Timer & timer)
{
    LOG_SCOPE( examples_15_pubclient_ServiceClient, process_timer );
    ASSERT(&timer == &mTimer);

    LOG_DBG("Timer [ %s ] expired, send request to output message.", timer.name().as_string());
    request_hello_world(role_name());
}
