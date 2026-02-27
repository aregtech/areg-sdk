/************************************************************************
 * \file        common/src/LocalHelloWorldClient.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This file contains simple implementation of service client to
 *              request message output
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "common/src/LocalHelloWorldClient.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/logging/GELog.h"

DEF_LOG_SCOPE(examples_16_pubmesh_common_LocalHelloWorldClient_serviceConnected);
DEF_LOG_SCOPE(examples_16_pubmesh_common_LocalHelloWorldClient_responseHelloWorld);
DEF_LOG_SCOPE(examples_16_pubmesh_common_LocalHelloWorldClient_onServiceStateUpdate);
DEF_LOG_SCOPE(examples_16_pubmesh_common_LocalHelloWorldClient_process_timer);

LocalHelloWorldClient::LocalHelloWorldClient( const NERegistry::DependencyEntry & dependency, Component & owner, uint32_t timeout)
    : LocalHelloWorldClientBase ( dependency, owner )
    , TimerConsumer           ( )

    , mMsTimeout                ( timeout )
    , mTimer                    ( static_cast<TimerConsumer &>(self()), timerName( owner ) )
    , mID                       ( 0 )
{
}

bool LocalHelloWorldClient::service_connected( NEService::ServiceConnectionState status, ProxyBase & proxy)
{
    LOG_SCOPE(examples_16_pubmesh_common_LocalHelloWorldClient_serviceConnected);

    bool result = LocalHelloWorldClientBase::service_connected( status, proxy );

    if ( isConnected( ) )
    {
        LOG_DBG( "Starting timer with timeout [ %d ] ms", mMsTimeout );
        mTimer.start_timer( mMsTimeout, LocalHelloWorldClientBase::getProxy( )->getProxyDispatcherThread( ) );
    }
    else
    {
        LOG_DBG( "Stopping the timer" );
        mTimer.stop_timer( );
    }

    return result;
}

void LocalHelloWorldClient::responseHelloWorld(const LocalHelloWorld::sConnectedClient & clientInfo)
{
    LOG_SCOPE(examples_16_pubmesh_common_LocalHelloWorldClient_responseHelloWorld);
    LOG_DBG("Service [ %s ]: Made output of [ %s ], client ID [ %d ]"
                    , LocalHelloWorldClientBase::getServiceRole().as_string()
                    , clientInfo.ccName.as_string()
                    , clientInfo.ccID);

    ASSERT(clientInfo.ccName == mTimer.name());
    mID = clientInfo.ccID;
}

void LocalHelloWorldClient::process_timer(Timer & timer)
{
    LOG_SCOPE(examples_16_pubmesh_common_LocalHelloWorldClient_process_timer);
    ASSERT( &timer == &mTimer );

    LOG_DBG( "Timer [ %s ] expired, sending local service request.", timer.name( ).as_string( ) );
    requestHelloWorld( timer.name( ) );
}

inline String LocalHelloWorldClient::timerName( Component & owner ) const
{
    String result;
    result.append("Local_")
          .append(owner.getRoleName())
          .append(NECommon::DEFAULT_SPECIAL_CHAR)
          .append(LocalHelloWorldClientBase::getServiceName());

    return result;
}
