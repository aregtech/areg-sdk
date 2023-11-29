/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/ipc/private/ServiceClientConnectionBase.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Service Connection client implementation
 ************************************************************************/
#include "areg/ipc/ServiceClientConnectionBase.hpp"

#include "areg/component/DispatcherThread.hpp"

#include "areg/ipc/IEServiceConnectionConsumer.hpp"
#include "areg/ipc/ConnectionConfiguration.hpp"
#include "areg/ipc/NERemoteService.hpp"
#include "areg/ipc/private/NEConnection.hpp"

#include "areg/component/DispatcherThread.hpp"
#include "areg/component/StreamableEvent.hpp"
#include "areg/component/ResponseEvents.hpp"
#include "areg/component/RequestEvents.hpp"
#include "areg/component/NEService.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/File.hpp"
#include "areg/trace/GETrace.h"

DEF_TRACE_SCOPE(areg_ipc_private_ServiceClientConnectionBase_onServiceReconnectTimerExpired);
DEF_TRACE_SCOPE(areg_ipc_private_ServiceClientConnectionBase_onServiceConnectionStart);
DEF_TRACE_SCOPE(areg_ipc_private_ServiceClientConnectionBase_onServiceConnectionStop);
DEF_TRACE_SCOPE(areg_ipc_private_ServiceClientConnectionBase_onServiceConnectionStarted);
DEF_TRACE_SCOPE(areg_ipc_private_ServiceClientConnectionBase_onServiceConnectionStopped);
DEF_TRACE_SCOPE(areg_ipc_private_ServiceClientConnectionBase_onServiceConnectionLost);

DEF_TRACE_SCOPE(areg_ipc_private_ServiceClientConnectionBase_startConnection);
DEF_TRACE_SCOPE(areg_ipc_private_ServiceClientConnectionBase_cancelConnection);

//////////////////////////////////////////////////////////////////////////
// ServiceClientConnectionBase class implementation
//////////////////////////////////////////////////////////////////////////

ServiceClientConnectionBase::ServiceClientConnectionBase( const ITEM_ID & target
                                                        , NERemoteService::eRemoteServices service
                                                        , unsigned int connectTypes
                                                        , NEService::eMessageSource msgSource
                                                        , IEServiceConnectionConsumer& connectionConsumer
                                                        , IERemoteMessageHandler & messageHandler
                                                        , DispatcherThread & messageDispatcher
                                                        , const String & prefixName)
    : IEServiceConnectionProvider   ( )
    , IEServiceEventConsumerBase    ( )

    , mTarget               (target)
    , mService              (service)
    , mConnectTypes         (connectTypes)
    , mMessageSource        (msgSource)
    , mClientConnection     ( )
    , mConnectionConsumer   (connectionConsumer)
    , mMessageDispatcher    (messageDispatcher)
    , mChannel              ( )
    , mConnectionState      ( eConnectionState::ConnectionStopped )
    , mEventConsumer        ( static_cast<IEServiceEventConsumerBase &>(self()) )
    , mLock                 ( )

    , mTimerConnect         ( static_cast<IETimerConsumer &>(mTimerConsumer), prefixName + NEConnection::CLIENT_CONNECT_TIMER_NAME )
    , mThreadReceive        (messageHandler, mClientConnection, prefixName)
    , mThreadSend           (messageHandler, mClientConnection, prefixName)
    , mTimerConsumer        ( static_cast<IEServiceEventConsumerBase &>(self()) )
{
    ASSERT((target > NEService::TARGET_LOCAL) && (target < NEService::COOKIE_REMOTE_SERVICE));
}

bool ServiceClientConnectionBase::setupServiceConnectionData(NERemoteService::eRemoteServices service, uint32_t connectTypes)
{
    Lock lock( mLock );

    bool result{ false };
    if ((mService == service) && ((mConnectTypes & connectTypes) != 0))
    {
        if ((mConnectTypes & static_cast<uint32_t>(NERemoteService::eConnectionTypes::ConnectTcpip)) != 0)
        {
            ConnectionConfiguration config(service, NERemoteService::eConnectionTypes::ConnectTcpip);
            if (config.isConfigured() && config.getConnectionEnableFlag())
            {
                String address{ config.getConnectionAddress() };
                unsigned short port{ config.getConnectionPort() };
                result = mClientConnection.setAddress(address, port);
            }
        }
    }

    return result;
}

void ServiceClientConnectionBase::applyServiceConnectionData( const String & hostName, unsigned short portNr )
{
    Lock lock( mLock );
    mClientConnection.setAddress( hostName, portNr );
}

bool ServiceClientConnectionBase::connectServiceHost(void)
{
    Lock lock( mLock );
    bool result{ false };
    if (mClientConnection.isValid() == false)
    {
        if ((mConnectTypes & static_cast<unsigned int>(NERemoteService::eConnectionTypes::ConnectTcpip)) != 0)
        {
            ConnectionConfiguration config(mService, NERemoteService::eConnectionTypes::ConnectTcpip);
            if (config.isConfigured() && config.getConnectionEnableFlag())
            {
                result = true;
                sendCommand(ServiceEventData::eServiceEventCommands::CMD_StartService);
            }
        }
    }

    return result;
}

bool ServiceClientConnectionBase::reconnectServiceHost(void)
{
    disconnectServiceHost( );
    sendCommand(ServiceEventData::eServiceEventCommands::CMD_StartService );

    return true;
}

void ServiceClientConnectionBase::disconnectServiceHost(void)
{
    sendCommand(ServiceEventData::eServiceEventCommands::CMD_ServiceExit, Event::eEventPriority::EventPriorityNormal);
}

bool ServiceClientConnectionBase::isServiceHostConnected(void) const
{
    Lock lock( mLock );
    return isConnectionStarted();
}

bool ServiceClientConnectionBase::isServiceHostSetup( void ) const
{
    Lock lock(mLock);
    return mClientConnection.getAddress().isValid();
}

RemoteMessage ServiceClientConnectionBase::createServiceConnectMessage(const ITEM_ID & /*source*/, const ITEM_ID & target, NEService::eMessageSource msgSource) const
{
    return NERemoteService::createConnectRequest(target, msgSource);
}

RemoteMessage ServiceClientConnectionBase::createServiceDisconnectMessage(const ITEM_ID & source, const ITEM_ID & target) const
{
    return NERemoteService::createDisconnectRequest(source, target);
}

void ServiceClientConnectionBase::onServiceReconnectTimerExpired( void )
{
    TRACE_SCOPE( areg_ipc_private_ServiceClientConnectionBase_onServiceReconnectTimerExpired );
    onServiceStart( );
}

void ServiceClientConnectionBase::onServiceStart(void)
{
    TRACE_SCOPE(areg_ipc_private_ServiceClientConnectionBase_onServiceConnectionStart);
    TRACE_DBG("Starting remove servicing");;

    mChannel.setCookie( NEService::COOKIE_LOCAL );
    mChannel.setSource( NEService::SOURCE_UNKNOWN );
    mChannel.setTarget( NEService::TARGET_UNKNOWN );
    if ( startConnection( ) )
    {
        setConnectionState( ServiceClientConnectionBase::eConnectionState::ConnectionStarting );
    }
}

void ServiceClientConnectionBase::onServiceStop(void)
{
    TRACE_SCOPE(areg_ipc_private_ServiceClientConnectionBase_onServiceConnectionStop);
    TRACE_DBG("Stopping remote servicing");

    setConnectionState(ServiceClientConnectionBase::eConnectionState::ConnectionStopping);

    mTimerConnect.stopTimer( );

    Channel channel{ mChannel };

    mChannel.setCookie( NEService::COOKIE_UNKNOWN );
    mChannel.setSource( NEService::SOURCE_UNKNOWN );
    mChannel.setTarget( NEService::TARGET_UNKNOWN );

    mThreadReceive.triggerExit( );

    if ((channel.getTarget() > NEService::COOKIE_LOCAL) && (channel.getTarget() < NEService::COOKIE_REMOTE_SERVICE))
    {
        sendMessage(createServiceDisconnectMessage(channel.getCookie(), mTarget));
    }

    disconnectService( Event::eEventPriority::EventPriorityNormal );

    mThreadSend.completionWait( NECommon::WAIT_INFINITE );
    mThreadSend.shutdownThread( NECommon::DO_NOT_WAIT );
    mClientConnection.closeSocket( );
    mThreadReceive.shutdownThread( NECommon::WAIT_INFINITE );

    mConnectionConsumer.disconnectedRemoteServiceChannel( channel );
}

void ServiceClientConnectionBase::onServiceRestart( void )
{
    onServiceStop( );
    onServiceStart( );
}

void ServiceClientConnectionBase::onServiceConnectionStarted(void)
{
    TRACE_SCOPE(areg_ipc_private_ServiceClientConnectionBase_onServiceConnectionStarted);
    ASSERT(isConnectionStarted());
    if ( mClientConnection.getCookie() != NEService::COOKIE_LOCAL )
    {
        TRACE_DBG("Succeeded to start router service client, cookie [ %llu ]", mClientConnection.getCookie());

        mChannel.setCookie( mClientConnection.getCookie() );
        mChannel.setSource( mMessageDispatcher.getId());
        mChannel.setTarget( mTarget );
        setConnectionState(ServiceClientConnectionBase::eConnectionState::ConnectionStarted);
        mConnectionConsumer.connectedRemoteServiceChannel(mChannel);
    }
}

void ServiceClientConnectionBase::onServiceConnectionStopped(void)
{
    TRACE_SCOPE(areg_ipc_private_ServiceClientConnectionBase_onServiceConnectionStopped);
    TRACE_DBG("Client service is stopped. Resetting cookie");

    setConnectionState(ServiceClientConnectionBase::eConnectionState::ConnectionStopped);
    mTimerConnect.stopTimer( );

    Channel channel = mChannel;
    mChannel.setCookie( NEService::COOKIE_UNKNOWN );
    mChannel.setSource( NEService::SOURCE_UNKNOWN );
    mChannel.setTarget( NEService::TARGET_UNKNOWN );

    cancelConnection( );

    mThreadReceive.shutdownThread( NECommon::WAIT_INFINITE );
    mThreadSend.shutdownThread( NECommon::WAIT_INFINITE );
    mConnectionConsumer.disconnectedRemoteServiceChannel( channel );

    if ( Application::isServicingReady( ) )
    {
        mTimerConnect.startTimer(NEConnection::DEFAULT_RETRY_CONNECT_TIMEOUT, mMessageDispatcher, 1 );
    }
}

void ServiceClientConnectionBase::onServiceConnectionLost(void)
{
    TRACE_SCOPE(areg_ipc_private_ServiceClientConnectionBase_onServiceConnectionLost);
    TRACE_WARN("Client service lost connection. Resetting cookie and trying to restart, current connection state [ %s ]"
                , ServiceClientConnectionBase::getString(getConnectionState()));

    Channel channel = mChannel;
    mChannel.setCookie( NEService::COOKIE_UNKNOWN );
    mChannel.setSource( NEService::SOURCE_UNKNOWN );
    mChannel.setTarget( NEService::TARGET_UNKNOWN );

    if ( Application::isServicingReady( ) && mTimerConnect.isStopped( ) )
    {
        TRACE_DBG( "Restarting lost connection with remote service" );

        mThreadReceive.shutdownThread( NECommon::WAIT_INFINITE );
        mThreadSend.shutdownThread( NECommon::WAIT_INFINITE );
        mConnectionConsumer.lostRemoteServiceChannel( channel );

        mTimerConnect.startTimer(NEConnection::DEFAULT_RETRY_CONNECT_TIMEOUT, mMessageDispatcher, 1 );
    }
    else
    {
        ASSERT( mThreadReceive.isRunning( ) == false );
        ASSERT( mThreadSend.isRunning( ) == false );
        ASSERT( mClientConnection.isValid( ) == false );

        TRACE_WARN("Ignoring lost connection event, either servicing state is not allowed, or application is closing.");
    }
}

void ServiceClientConnectionBase::onServiceExit( void )
{
    onServiceStop( );
}

void ServiceClientConnectionBase::onServiceMessageReceived( const RemoteMessage & msgReceived )
{
}

void ServiceClientConnectionBase::onServiceMessageSend( const RemoteMessage & msgSend )
{
}

bool ServiceClientConnectionBase::startConnection(void)
{
    TRACE_SCOPE(areg_ipc_private_ServiceClientConnectionBase_startConnection);

    ASSERT(mClientConnection.getAddress().isValid());
    ASSERT(mClientConnection.isValid() == false);
    ASSERT(mThreadReceive.isRunning() == false);
    ASSERT(mThreadSend.isRunning() == false);

    bool result = false;
    mTimerConnect.stopTimer();

    if ( mClientConnection.createSocket() )
    {
        if ( mThreadReceive.createThread( NECommon::WAIT_INFINITE ) && mThreadSend.createThread( NECommon::WAIT_INFINITE ) )
        {
            VERIFY( mThreadReceive.waitForDispatcherStart( NECommon::WAIT_INFINITE ) );
            VERIFY( mThreadSend.waitForDispatcherStart( NECommon::WAIT_INFINITE ) );
            TRACE_DBG("Client service starting connection with remote routing service.");
            result = mClientConnection.sendMessage(createServiceConnectMessage(NEService::COOKIE_UNKNOWN, mTarget, mMessageSource));
        }
    }

    if ( result == false )
    {
        TRACE_WARN("Client service failed to start connection, going to repeat connection in [ %u ] ms", NEConnection::DEFAULT_RETRY_CONNECT_TIMEOUT);
        mThreadSend.shutdownThread( NECommon::DO_NOT_WAIT );
        mThreadReceive.shutdownThread( NECommon::DO_NOT_WAIT );
        mClientConnection.closeSocket();
        mTimerConnect.startTimer(NEConnection::DEFAULT_RETRY_CONNECT_TIMEOUT, mMessageDispatcher, 1);
    }

    return result;
}

void ServiceClientConnectionBase::cancelConnection(void)
{
    TRACE_SCOPE(areg_ipc_private_ServiceClientConnectionBase_cancelConnection);
    TRACE_WARN("Canceling client service connection");

    mClientConnection.closeSocket();
    mClientConnection.setCookie( NEService::COOKIE_UNKNOWN );

    mThreadReceive.shutdownThread( NECommon::DO_NOT_WAIT );
    mThreadSend.shutdownThread( NECommon::DO_NOT_WAIT );
}
