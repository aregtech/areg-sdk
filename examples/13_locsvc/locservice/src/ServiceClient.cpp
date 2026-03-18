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

DEF_LOG_SCOPE(examples_13_locservice_ServiceClient_serviceConnected);
DEF_LOG_SCOPE(examples_13_locservice_ServiceClient_broadcastReachedMaximum);
DEF_LOG_SCOPE(examples_13_locservice_ServiceClient_responseHelloWorld);
DEF_LOG_SCOPE(examples_13_locservice_ServiceClient_processTimer);

ServiceClient::ServiceClient(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    : areg::Component             ( entry, owner )
    , HelloWorldConsumerBase  ( entry.mDependencyServices[0].mRoleName, static_cast<areg::Component &>(self()) )
    , areg::TimerConsumer       ( )

    , mTimer                (static_cast<areg::TimerConsumer &>(self()), entry.mRoleName)
{
}

bool ServiceClient::service_connected( areg::ServiceConnectionState status, areg::ProxyBase & proxy)
{
    LOG_SCOPE(examples_13_locservice_ServiceClient_serviceConnected);
    bool result = HelloWorldConsumerBase::service_connected( status, proxy );
    // subscribe when service connected and un-subscribe when disconnected.
    if ( is_connected( ) )
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

void ServiceClient::responseHelloWorld()
{
    LOG_SCOPE(examples_13_locservice_ServiceClient_responseHelloWorld);
    LOG_DBG("Received response on request to print greetings from the client");
}

#if AREG_LOGGING
void ServiceClient::broadcastReachedMaximum( int32_t maxNumber )
{
    LOG_SCOPE(examples_13_locservice_ServiceClient_broadcastReachedMaximum );
    LOG_WARN("Service notify reached maximum number of requests [ %d ], starting shutdown procedure", maxNumber );
    requestShutdownService( );
}
#else   // AREG_LOGGING
void ServiceClient::broadcastReachedMaximum( int32_t /*maxNumber*/ )
{
    requestShutdownService( );
}
#endif  // AREG_LOGGING

void ServiceClient::process_timer(areg::Timer & timer)
{
    LOG_SCOPE(examples_13_locservice_ServiceClient_processTimer);
    ASSERT(&timer == &mTimer);

    LOG_DBG("Timer [ %s ] expired, send request to output message.", timer.name().as_string());
    requestHelloWorld(role_name());
}
