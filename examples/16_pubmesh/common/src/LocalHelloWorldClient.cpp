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

DEF_LOG_SCOPE(examples_16_pubmesh_common_LocalHelloWorldClient, service_connected);
DEF_LOG_SCOPE(examples_16_pubmesh_common_LocalHelloWorldClient, response_hello_world);
DEF_LOG_SCOPE(examples_16_pubmesh_common_LocalHelloWorldClient, on_service_state_update);
DEF_LOG_SCOPE(examples_16_pubmesh_common_LocalHelloWorldClient, process_timer);

LocalHelloWorldClient::LocalHelloWorldClient( const areg::DependencyEntry & dependency, areg::Component & owner, uint32_t timeout)
    : LocalHelloWorldConsumerBase ( dependency, owner )
    , areg::TimerConsumer         ( )

    , mMsTimeout( timeout )
    , mTimer    ( static_cast<areg::TimerConsumer &>(self()), timer_name( owner ) )
    , mID       ( 0 )
{
}

bool LocalHelloWorldClient::service_connected( areg::ServiceConnectionState status, areg::ProxyBase & proxy)
{
    LOG_SCOPE( examples_16_pubmesh_common_LocalHelloWorldClient, service_connected );

    bool result = LocalHelloWorldConsumerBase::service_connected( status, proxy );

    if ( is_connected( ) )
    {
        LOG_DBG( "Starting timer with timeout [ %d ] ms", mMsTimeout );
        mTimer.start_timer( mMsTimeout, LocalHelloWorldConsumerBase::service_proxy( )->proxy_dispatcher_thread( ) );
    }
    else
    {
        LOG_DBG( "Stopping the timer" );
        mTimer.stop_timer( );
    }

    return result;
}

void LocalHelloWorldClient::response_hello_world(const LocalHelloWorld::sConnectedClient & clientInfo)
{
    LOG_SCOPE( examples_16_pubmesh_common_LocalHelloWorldClient, response_hello_world );
    LOG_DBG("Service [ %s ]: Made output of [ %s ], client ID [ %d ]"
                    , LocalHelloWorldConsumerBase::service_name().as_string()
                    , clientInfo.ccName.as_string()
                    , clientInfo.ccID);

    ASSERT(clientInfo.ccName == mTimer.name());
    mID = clientInfo.ccID;
}

void LocalHelloWorldClient::process_timer(areg::Timer & timer)
{
    LOG_SCOPE( examples_16_pubmesh_common_LocalHelloWorldClient, process_timer );
    ASSERT( &timer == &mTimer );

    LOG_DBG( "Timer [ %s ] expired, sending local service request.", timer.name( ).as_string( ) );
    request_hello_world( timer.name( ) );
}

inline areg::String LocalHelloWorldClient::timer_name( areg::Component & owner ) const
{
    areg::String result;
    result.append("Local_")
          .append(owner.role_name())
          .append(areg::DEFAULT_SPECIAL_CHAR)
          .append(LocalHelloWorldConsumerBase::service_name());

    return result;
}
