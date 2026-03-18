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

DEF_LOG_SCOPE(examples_15_pubclient_ServiceClient_serviceConnected);
DEF_LOG_SCOPE(examples_15_pubclient_ServiceClient_broadcastReachedMaximum);
DEF_LOG_SCOPE(examples_15_pubclient_ServiceClient_responseHelloWorld);
DEF_LOG_SCOPE(examples_15_pubclient_ServiceClient_processTimer);

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
    LOG_SCOPE( examples_15_pubclient_ServiceClient_serviceConnected );
    bool result = HelloWorldConsumerBase::service_connected( status, proxy );

    // subscribe when service connected and un-subscribe when disconnected.
    notifyOnBroadcastReachedMaximum( is_connected( ) );
    if ( is_connected( ) )
    {
        mTimer.start_timer( ServiceClient::TIMEOUT_VALUE );
    }
    else if ( areg::is_service_connection_lost( status ) )
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

void ServiceClient::responseHelloWorld(const HelloWorld::sConnectedClient & clientInfo)
{
    LOG_SCOPE(examples_15_pubclient_ServiceClient_responseHelloWorld);
    LOG_DBG("Greetings from [ %s ] output on console, client ID [ %d ]", clientInfo.ccName.as_string(), clientInfo.ccID);
    ASSERT(clientInfo.ccName == role_name());
    mID = clientInfo.ccID;
}

#if AREG_LOGGING
void ServiceClient::broadcastReachedMaximum( int32_t maxNumber )
{
    LOG_SCOPE(examples_15_pubclient_ServiceClient_broadcastReachedMaximum);
    LOG_WARN("Service notify reached maximum number of requests [ %d ], starting shutdown procedure", maxNumber );
    requestShutdownService(mID, role_name());
    mID = 0;
}
#else   // AREG_LOGGING
void ServiceClient::broadcastReachedMaximum( int32_t /*maxNumber*/ )
{
    requestShutdownService(mID, role_name());
    mID = 0;
}
#endif  // AREG_LOGGING

void ServiceClient::process_timer(areg::Timer & timer)
{
    LOG_SCOPE(examples_15_pubclient_ServiceClient_processTimer);
    ASSERT(&timer == &mTimer);

    LOG_DBG("Timer [ %s ] expired, send request to output message.", timer.name().as_string());
    requestHelloWorld(role_name());
}
