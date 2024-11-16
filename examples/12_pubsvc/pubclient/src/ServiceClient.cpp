/************************************************************************
 * \file        pubclient/src/ServiceClient.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of service client to
 *              request message output
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "pubclient/src/ServiceClient.hpp"
#include "areg/logging/GELog.h"
#include "areg/appbase/Application.hpp"

DEF_LOG_SCOPE(examples_12_pubclient_ServiceClient_serviceConnected);
DEF_LOG_SCOPE(examples_12_pubclient_ServiceClient_broadcastReachedMaximum);
DEF_LOG_SCOPE(examples_12_pubclient_ServiceClient_responseHelloWorld);
DEF_LOG_SCOPE(examples_12_pubclient_ServiceClient_processTimer);

Component * ServiceClient::CreateComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
{
    return DEBUG_NEW ServiceClient(entry, owner);
}

void ServiceClient::DeleteComponent(Component & compObject, const NERegistry::ComponentEntry & /* entry */)
{
    delete (&compObject);
}

ServiceClient::ServiceClient(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : Component             ( NEUtilities::generateName(entry.mRoleName), owner )
    , HelloWorldClientBase  ( entry.mDependencyServices[0].mRoleName, static_cast<Component &>(self()) )
    , IETimerConsumer       ( )

    , mTimer                (static_cast<IETimerConsumer &>(self()), entry.mRoleName)
    , mID                   ( 0 )
{
}

bool ServiceClient::serviceConnected( NEService::eServiceConnection status, ProxyBase & proxy )
{
    LOG_SCOPE( examples_12_pubclient_ServiceClient_serviceConnected );
    bool result = HelloWorldClientBase::serviceConnected( status, proxy );

    // subscribe when service connected and un-subscribe when disconnected.
    notifyOnBroadcastReachedMaximum( isConnected( ) );
    if ( isConnected( ) )
    {
        mTimer.startTimer( ServiceClient::TIMEOUT_VALUE );
    }
    else if ( NEService::isServiceConnectionLost( status ) )
    {
        LOG_WARN( "The connection is lost! Waiting for connection recovery!" );
        mTimer.stopTimer( );
    }
    else
    {
        LOG_WARN("Shutting down application!");
        mTimer.stopTimer( );
        Application::signalAppQuit();
    }

    return result;
}

void ServiceClient::responseHelloWorld(const NEHelloWorld::sConnectedClient & clientInfo)
{
    LOG_SCOPE(examples_12_pubclient_ServiceClient_responseHelloWorld);
    LOG_DBG("Greetings from [ %s ] output on console, client ID [ %d ]", clientInfo.ccName.getString(), clientInfo.ccID);
    ASSERT(clientInfo.ccName == getRoleName());
    mID = clientInfo.ccID;
}

#if AREG_LOGS
void ServiceClient::broadcastReachedMaximum( int maxNumber )
{
    LOG_SCOPE(examples_12_pubclient_ServiceClient_broadcastReachedMaximum);
    LOG_WARN("Service notify reached maximum number of requests [ %d ], starting shutdown procedure", maxNumber );
    requestShutdownService(mID, getRoleName());
    mID = 0;
}
#else   // AREG_LOGS
void ServiceClient::broadcastReachedMaximum( int /*maxNumber*/ )
{
    requestShutdownService(mID, getRoleName());
    mID = 0;
}
#endif  // AREG_LOGS

void ServiceClient::processTimer(Timer & timer)
{
    LOG_SCOPE(examples_12_pubclient_ServiceClient_processTimer);
    ASSERT(&timer == &mTimer);

    LOG_DBG("Timer [ %s ] expired, send request to output message.", timer.getName().getString());
    requestHelloWorld(getRoleName());
}
