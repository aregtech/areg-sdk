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
#include "areg/logging/areg_log.h"

DEF_LOG_SCOPE(examples_16_pubmesh_common_LocalHelloWorldClient_serviceConnected);
DEF_LOG_SCOPE(examples_16_pubmesh_common_LocalHelloWorldClient_responseHelloWorld);
DEF_LOG_SCOPE(examples_16_pubmesh_common_LocalHelloWorldClient_onServiceStateUpdate);
DEF_LOG_SCOPE(examples_16_pubmesh_common_LocalHelloWorldClient_processTimer);

LocalHelloWorldClient::LocalHelloWorldClient( const areg::DependencyEntry & dependency, areg::Component & owner, uint32_t timeout)
    : LocalHelloWorldClientBase ( dependency, owner )
    , areg::TimerConsumer           ( )

    , mMsTimeout                ( timeout )
    , mTimer                    ( static_cast<areg::TimerConsumer &>(self()), timerName( owner ) )
    , mID                       ( 0 )
{
}

bool LocalHelloWorldClient::service_connected( areg::ServiceConnectionState status, areg::ProxyBase & proxy)
{
    LOG_SCOPE(examples_16_pubmesh_common_LocalHelloWorldClient_serviceConnected);

    bool result = LocalHelloWorldClientBase::service_connected( status, proxy );

    if ( is_connected( ) )
    {
        LOG_DBG( "Starting timer with timeout [ %d ] ms", mMsTimeout );
        mTimer.start_timer( mMsTimeout, LocalHelloWorldClientBase::proxy( )->proxy_dispatcher_thread( ) );
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
                    , LocalHelloWorldClientBase::service_role().as_string()
                    , clientInfo.ccName.as_string()
                    , clientInfo.ccID);

    ASSERT(clientInfo.ccName == mTimer.name());
    mID = clientInfo.ccID;
}

void LocalHelloWorldClient::process_timer(areg::Timer & timer)
{
    LOG_SCOPE(examples_16_pubmesh_common_LocalHelloWorldClient_processTimer);
    ASSERT( &timer == &mTimer );

    LOG_DBG( "Timer [ %s ] expired, sending local service request.", timer.name( ).as_string( ) );
    requestHelloWorld( timer.name( ) );
}

inline areg::String LocalHelloWorldClient::timerName( areg::Component & owner ) const
{
    areg::String result;
    result.append("Local_")
          .append(owner.role_name())
          .append(areg::DEFAULT_SPECIAL_CHAR)
          .append(LocalHelloWorldClientBase::service_name());

    return result;
}
