/************************************************************************
 * \file        common/src/PublicHelloWorldClient.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of service client to
 *              request message output
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "common/src/PublicHelloWorldClient.hpp"

#include "common/src/NECommon.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/ProxyBase.hpp"
#include "areg/component/Component.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/base/Process.hpp"
#include "areg/logging/GELog.h"

DEF_LOG_SCOPE(examples_16_pubmesh_common_PublicHelloWorldClient_serviceConnected);
DEF_LOG_SCOPE(examples_16_pubmesh_common_PublicHelloWorldClient_responseRegister);
DEF_LOG_SCOPE(examples_16_pubmesh_common_PublicHelloWorldClient_responseHelloWorld);
DEF_LOG_SCOPE(examples_16_pubmesh_common_PublicHelloWorldClient_broadcastServiceUnavailable);
DEF_LOG_SCOPE(examples_16_pubmesh_common_PublicHelloWorldClient_onServiceStateUpdate);
DEF_LOG_SCOPE(examples_16_pubmesh_common_PublicHelloWorldClient_processTimer);

PublicHelloWorldClient::PublicHelloWorldClient( const NERegistry::DependencyEntry & dependency, Component & owner, unsigned int timeout)
    : PublicHelloWorldClientBase( dependency, owner )
    , SystemShutdownClientBase  ( NECommon::PublicControllerService, owner )
    , IETimerConsumer           ( )

    , mMsTimeout                ( timeout )
    , mTimer                    ( static_cast<IETimerConsumer &>(self()), timerName(owner) )
    , mClient                   ( )
{
}

bool PublicHelloWorldClient::serviceConnected( NEService::eServiceConnection status, ProxyBase & proxy)
{
    LOG_SCOPE(examples_16_pubmesh_common_PublicHelloWorldClient_serviceConnected);
    bool result{ true };

    // Since this class is using multiple proxies and client base classes, check for each of that class.
    if ( PublicHelloWorldClientBase::serviceConnected(status, proxy) )
    {
        // Reset the ID here. Otherwise, it keeps old value when service connection lost.
        mClient.crID = 0;
        if ( PublicHelloWorldClientBase::isConnected() )
        {
            LOG_DBG("Client [ %p ]-[ %s ] sends request to register", this, mTimer.getName().getString());
            requestRegister(mTimer.getName(), proxy.getProxyAddress(), proxy.getProxyDispatcherThread().getName(), Process::getInstance().getAppName());
        }
        else
        {
            LOG_DBG( "Disconnected [ %s : %s ]"
                    , proxy.getStubAddress( ).isSourcePublic( ) ? "LOCAL PUBLIC" : "REMOTE PUBLIC"
                    , StubAddress::convAddressToPath( proxy.getStubAddress()).getString());
            printf("----- Disconnected %s service consumer -----\n", proxy.getStubAddress().isSourcePublic() ? "LOCAL PUBLIC" : "REMOTE PUBLIC");
            mTimer.stopTimer( );
        }
    }
    else if (SystemShutdownClientBase::serviceConnected(status, proxy))
    {
        bool connected = SystemShutdownClientBase::isConnected();
        LOG_DBG("Consumer [ %p : %s ] is [ %s ]"
                  , this
                  , ProxyAddress::convAddressToPath(proxy.getProxyAddress()).getString()
                  , connected ? "CONNECTED" : "DISCONNECTED");
        notifyOnServiceStateUpdate( connected );
    }
    else
    {
        result = false;
    }

    return result;
}

void PublicHelloWorldClient::responseRegister( const NEPublicHelloWorld::sClientRegister & client )
{
    LOG_SCOPE(examples_16_pubmesh_common_PublicHelloWorldClient_responseRegister);

    if (client.crID != 0)
    {
        ASSERT(client.crName == mTimer.getName());

        LOG_INFO("The client [ %s ] is registered and got ID [ %d ], can use services", client.crName.getString(), client.crID);
        mClient = client;
        if ( (mClient.crID != 0) && (mTimer.isActive( ) == false) )
        {
            mTimer.startTimer( mMsTimeout );
        }
    }
    else
    {
        LOG_ERR("Failed to register, cannot use service!");
    }
}

void PublicHelloWorldClient::responseHelloWorld(unsigned int clientID)
{
    LOG_SCOPE(examples_16_pubmesh_common_PublicHelloWorldClient_responseHelloWorld);
    if ( (clientID != 0) && (mClient.crID == clientID) )
    {
        LOG_DBG("Client [ %s ] SUCCEEDED to make output on remote service [ %s ]", mClient.crName.getString(), PublicHelloWorldClientBase::getServiceRole().getString());
        printf( "..... public greetings succeeded .....\n" );
    }
    else
    {
        LOG_ERR("Client [ %s ] FAILED to make output on remote service [ %s ]", mClient.crName.getString(), PublicHelloWorldClientBase::getServiceRole().getString());
        printf( ">>> ERROR: Unexpected client ID!....\n" );
        mTimer.stopTimer();
        requestUnregister(mClient);
        mClient.crID = 0;
        ASSERT( false );
    }
}

void PublicHelloWorldClient::onServiceStateUpdate( NESystemShutdown::eServiceState ServiceState, NEService::eDataStateType state )
{
    LOG_SCOPE(examples_16_pubmesh_common_PublicHelloWorldClient_onServiceStateUpdate);
    LOG_DBG("Service state updated [ %s ], data state [ %s ], client [ %d : %s ]"
               , NESystemShutdown::getString(ServiceState)
               , NEService::getString(state)
               , mClient.crID
               , mClient.crName.getString());

    if (state == NEService::eDataStateType::DataIsOK)
    {
        if (ServiceState == NESystemShutdown::eServiceState::ServiceShutdown)
        {
            if ( SystemShutdownClientBase::getProxy()->getStubAddress( ).isSourcePublic( ) )
            {
                mTimer.stopTimer( );
                LOG_DBG( "External source of message, going to shutdown the application" );
                // disable assign on notification if the service is in the same process.
                printf( ">>>>>>>>>> Shutting down the application <<<<<<<<<<\n" );
                mTimer.stopTimer( );
                if ( mClient.crID != 0 )
                {
                    requestUnregister( mClient );
                    mClient.crID = 0;
                }

                requestSystemShutdown( );
                Application::signalAppQuit( );
                Thread::switchThread();
            }
            else
            {
                LOG_DBG( "Internal source of message, ignoring to shutdown the application" );
            }
        }
        else if ( (mClient.crID != 0) && (mTimer.isActive() == false) )
        {
            LOG_DBG( "Starting timer to send requests" );
            mTimer.startTimer(mMsTimeout);
        }
        else
        {
            LOG_WARN( "Ignoring the message, either the client is [ %d ] or timer is [ %s ]", mClient.crID, mTimer.isActive( ) ? "ACTIVE" : "INACTIVE" );
        }
    }
    else
    {
        LOG_DBG( "Ignoring the system state change, the data is unavailable" );
    }
}

void PublicHelloWorldClient::processTimer(Timer & timer)
{
    LOG_SCOPE(examples_16_pubmesh_common_PublicHelloWorldClient_processTimer);
    ASSERT(&timer == &mTimer);

    LOG_DBG("Timer [ %s ] of client ID [ %d ] has expired, send request to output message.", timer.getName().getString(), mClient.crID);

    NEService::eDataStateType dataState { NEService::eDataStateType::DataIsUndefined };
    NESystemShutdown::eServiceState serviceState = getServiceState( dataState );
    if ( dataState == NEService::eDataStateType::DataIsOK )
    {
        if ( serviceState == NESystemShutdown::eServiceState::ServiceReady )
        {
            LOG_DBG( "Client [ %s ] sends hello world request.", PublicHelloWorldClientBase::getServiceRole( ).getString( ) );
            requestHelloWorld( mClient.crID );
        }
        else
        {
            LOG_WARN( "Ignored sending request, the service state is [ %s ]", NESystemShutdown::getString( serviceState ) );
        }
    }
    else
    {
        LOG_WARN( "Ignored sending request, the data state is [ %s ]", NEService::getString( dataState ) );
    }
}

inline String PublicHelloWorldClient::timerName( Component & owner ) const
{
    String result;
    result.append("Public_")
          .append( owner.getRoleName( ) )
          .append(NECommon::DEFAULT_SPECIAL_CHAR)
          .append(PublicHelloWorldClientBase::getServiceName());

    return result;
}
