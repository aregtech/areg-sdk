/************************************************************************
 * \file        common/src/LocalHelloWorldClient.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
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
#include "areg/trace/GETrace.h"

#include "common/src/NECommon.hpp"

DEF_TRACE_SCOPE(examples_13_pubmesh_common_LocalHelloWorldClient_serviceConnected);
DEF_TRACE_SCOPE(examples_13_pubmesh_common_LocalHelloWorldClient_responseHelloWorld);
DEF_TRACE_SCOPE(examples_13_pubmesh_common_LocalHelloWorldClient_onServiceStateUpdate);
DEF_TRACE_SCOPE(examples_13_pubmesh_common_LocalHelloWorldClient_processTimer);

LocalHelloWorldClient::LocalHelloWorldClient( const NERegistry::DependencyEntry & dependency, Component & owner, unsigned int timeout)
    : LocalHelloWorldClientBase ( dependency, owner )
    , IETimerConsumer           ( )

    , mMsTimeout                ( timeout )
    , mTimer                    ( static_cast<IETimerConsumer &>(self()), timerName( owner ) )
    , mID                       ( 0 )
{
}

bool LocalHelloWorldClient::serviceConnected( NEService::eServiceConnection status, ProxyBase & proxy)
{
    TRACE_SCOPE(examples_13_pubmesh_common_LocalHelloWorldClient_serviceConnected);

    bool result = LocalHelloWorldClientBase::serviceConnected( status, proxy );

    if ( isConnected( ) )
    {
        TRACE_DBG( "Starting timer with timeout [ %d ] ms", mMsTimeout );
        mTimer.startTimer( mMsTimeout, LocalHelloWorldClientBase::getProxy( )->getProxyDispatcherThread( ) );
    }
    else
    {
        TRACE_DBG( "Stopping the timer" );
        mTimer.stopTimer( );
    }

    return result;
}

void LocalHelloWorldClient::responseHelloWorld(const NELocalHelloWorld::sConnectedClient & clientInfo)
{
    TRACE_SCOPE(examples_13_pubmesh_common_LocalHelloWorldClient_responseHelloWorld);
    TRACE_DBG("Service [ %s ]: Made output of [ %s ], client ID [ %d ]"
                    , LocalHelloWorldClientBase::getServiceRole().getString()
                    , clientInfo.ccName.getString()
                    , clientInfo.ccID);

    ASSERT(clientInfo.ccName == mTimer.getName());
    mID = clientInfo.ccID;
}

void LocalHelloWorldClient::processTimer(Timer & timer)
{
    TRACE_SCOPE(examples_13_pubmesh_common_LocalHelloWorldClient_processTimer);
    ASSERT( &timer == &mTimer );

    TRACE_DBG( "Timer [ %s ] expired, sending local service request.", timer.getName( ).getString( ) );
    requestHelloWorld( timer.getName( ) );
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
