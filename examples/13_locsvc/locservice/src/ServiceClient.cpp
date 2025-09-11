/************************************************************************
 * \file        locservice/src/ServiceClient.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of service client to
 *              request message output
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "locservice/src/ServiceClient.hpp"
#include "areg/logging/GELog.h"

DEF_LOG_SCOPE(examples_13_locservice_ServiceClient_serviceConnected);
DEF_LOG_SCOPE(examples_13_locservice_ServiceClient_broadcastReachedMaximum);
DEF_LOG_SCOPE(examples_13_locservice_ServiceClient_responseHelloWorld);
DEF_LOG_SCOPE(examples_13_locservice_ServiceClient_processTimer);

ServiceClient::ServiceClient(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : Component             ( entry, owner )
    , HelloWorldClientBase  ( entry.mDependencyServices[0].mRoleName, static_cast<Component &>(self()) )
    , IETimerConsumer       ( )

    , mTimer                (static_cast<IETimerConsumer &>(self()), entry.mRoleName)
{
}

bool ServiceClient::serviceConnected( NEService::eServiceConnection status, ProxyBase & proxy)
{
    LOG_SCOPE(examples_13_locservice_ServiceClient_serviceConnected);
    bool result = HelloWorldClientBase::serviceConnected( status, proxy );
    // subscribe when service connected and un-subscribe when disconnected.
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

void ServiceClient::responseHelloWorld( void )
{
    LOG_SCOPE(examples_13_locservice_ServiceClient_responseHelloWorld);
    LOG_DBG("Received response on request to print greetings from the client");
}

#if AREG_LOGS
void ServiceClient::broadcastReachedMaximum( int maxNumber )
{
    LOG_SCOPE(examples_13_locservice_ServiceClient_broadcastReachedMaximum );
    LOG_WARN("Service notify reached maximum number of requests [ %d ], starting shutdown procedure", maxNumber );
    requestShutdownService( );
}
#else   // AREG_LOGS
void ServiceClient::broadcastReachedMaximum( int /*maxNumber*/ )
{
    requestShutdownService( );
}
#endif  // AREG_LOGS

void ServiceClient::processTimer(Timer & timer)
{
    LOG_SCOPE(examples_13_locservice_ServiceClient_processTimer);
    ASSERT(&timer == &mTimer);

    LOG_DBG("Timer [ %s ] expired, send request to output message.", timer.getName().getString());
    requestHelloWorld(getRoleName());
}
