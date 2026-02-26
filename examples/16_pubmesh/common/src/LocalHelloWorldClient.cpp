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
DEF_LOG_SCOPE(examples_16_pubmesh_common_LocalHelloWorldClient_processTimer);

LocalHelloWorldClient::LocalHelloWorldClient( const areg::DependencyEntry & dependency, areg::Component & owner, uint32_t timeout)
    : LocalHelloWorldClientBase ( dependency, owner )
    , areg::TimerConsumer           ( )

    , mMsTimeout                ( timeout )
    , mTimer                    ( static_cast<areg::TimerConsumer &>(self()), timerName( owner ) )
    , mID                       ( 0 )
{
}

bool LocalHelloWorldClient::serviceConnected( areg::ServiceConnectionState status, ProxyBase & proxy)
{
    LOG_SCOPE(examples_16_pubmesh_common_LocalHelloWorldClient_serviceConnected);

    bool result = LocalHelloWorldClientBase::serviceConnected( status, proxy );

    if ( isConnected( ) )
    {
        LOG_DBG( "Starting timer with timeout [ %d ] ms", mMsTimeout );
        mTimer.startTimer( mMsTimeout, LocalHelloWorldClientBase::getProxy( )->getProxyDispatcherThread( ) );
    }
    else
    {
        LOG_DBG( "Stopping the timer" );
        mTimer.stopTimer( );
    }

    return result;
}

void LocalHelloWorldClient::responseHelloWorld(const LocalHelloWorld::sConnectedClient & clientInfo)
{
    LOG_SCOPE(examples_16_pubmesh_common_LocalHelloWorldClient_responseHelloWorld);
    LOG_DBG("Service [ %s ]: Made output of [ %s ], client ID [ %d ]"
                    , LocalHelloWorldClientBase::getServiceRole().getString()
                    , clientInfo.ccName.getString()
                    , clientInfo.ccID);

    ASSERT(clientInfo.ccName == mTimer.getName());
    mID = clientInfo.ccID;
}

void LocalHelloWorldClient::processTimer(areg::Timer & timer)
{
    LOG_SCOPE(examples_16_pubmesh_common_LocalHelloWorldClient_processTimer);
    ASSERT( &timer == &mTimer );

    LOG_DBG( "Timer [ %s ] expired, sending local service request.", timer.getName( ).getString( ) );
    requestHelloWorld( timer.getName( ) );
}

inline areg::String LocalHelloWorldClient::timerName( areg::Component & owner ) const
{
    areg::String result;
    result.append("Local_")
          .append(owner.getRoleName())
          .append(areg::DEFAULT_SPECIAL_CHAR)
          .append(LocalHelloWorldClientBase::getServiceName());

    return result;
}
