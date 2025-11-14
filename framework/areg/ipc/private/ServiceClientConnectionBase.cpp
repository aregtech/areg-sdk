/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/ipc/private/ServiceClientConnectionBase.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Service Connection client implementation
 ************************************************************************/
#include "areg/ipc/ServiceClientConnectionBase.hpp"

#include "areg/component/NEService.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/ipc/IEServiceConnectionConsumer.hpp"
#include "areg/ipc/ConnectionConfiguration.hpp"
#include "areg/ipc/NERemoteService.hpp"
#include "areg/ipc/private/NEConnection.hpp"
#include "areg/logging/GELog.h"

DEF_LOG_SCOPE(areg_ipc_private_ServiceClientConnectionBase_onServiceReconnectTimerExpired);
DEF_LOG_SCOPE(areg_ipc_private_ServiceClientConnectionBase_onServiceConnectionStart);
DEF_LOG_SCOPE(areg_ipc_private_ServiceClientConnectionBase_onServiceConnectionStop);
DEF_LOG_SCOPE(areg_ipc_private_ServiceClientConnectionBase_onServiceConnectionStarted);
DEF_LOG_SCOPE(areg_ipc_private_ServiceClientConnectionBase_onServiceConnectionStopped);
DEF_LOG_SCOPE(areg_ipc_private_ServiceClientConnectionBase_onServiceConnectionLost);
DEF_LOG_SCOPE(areg_ipc_private_ServiceClientConnectionBase_onChannelConnected);

DEF_LOG_SCOPE(areg_ipc_private_ServiceClientConnectionBase_serviceConnectionEvent);
DEF_LOG_SCOPE(areg_ipc_private_ServiceClientConnectionBase_startConnection);
DEF_LOG_SCOPE(areg_ipc_private_ServiceClientConnectionBase_cancelConnection);

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

ServiceClientConnectionBase::~ServiceClientConnectionBase(void)
{
}

void ServiceClientConnectionBase::serviceConnectionEvent(const RemoteMessage& msgReceived)
{
    LOG_SCOPE(areg_ipc_private_ServiceClientConnectionBase_serviceConnectionEvent);

    msgReceived.moveToBegin();
    ITEM_ID cookie{ NEService::COOKIE_UNKNOWN };
    NEService::eServiceConnection connection{ NEService::eServiceConnection::ServiceConnectionUnknown };
    msgReceived >> cookie;
    msgReceived >> connection;
    LOG_DBG("Remote service connection notification: status [ %s ], cookie [ %llu ]", NEService::getString(connection), cookie);

    switch (connection)
    {
    case NEService::eServiceConnection::ServiceConnected:
    case NEService::eServiceConnection::ServicePending:
        {
            if (msgReceived.getResult() == NEMemory::MESSAGE_SUCCESS)
            {
                Lock lock(mLock);
                ASSERT(cookie == msgReceived.getTarget());
                mClientConnection.setCookie(cookie);
                onChannelConnected(cookie);
                sendCommand(ServiceEventData::eServiceEventCommands::CMD_ServiceStarted);
            }
            else
            {
                cancelConnection();
                onChannelConnected(NEService::COOKIE_UNKNOWN);
                sendCommand(ServiceEventData::eServiceEventCommands::CMD_ServiceLost);
            }
        }
        break;

    case NEService::eServiceConnection::ServiceConnectionLost:
    case NEService::eServiceConnection::ServiceDisconnected:
    case NEService::eServiceConnection::ServiceFailed:
        {
            cancelConnection();
            onChannelConnected(NEService::COOKIE_UNKNOWN);
            sendCommand(ServiceEventData::eServiceEventCommands::CMD_ServiceLost);
        }
        break;

    case NEService::eServiceConnection::ServiceConnectionUnknown:   // fall through
    case NEService::eServiceConnection::ServiceRejected:            // fall through
    case NEService::eServiceConnection::ServiceShutdown:            // fall through
    default:
        {
            cancelConnection();
            onChannelConnected(NEService::COOKIE_UNKNOWN);
            sendCommand(ServiceEventData::eServiceEventCommands::CMD_ServiceStopped);
        }
        break;
    }
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

RemoteMessage ServiceClientConnectionBase::createServiceConnectMessage(const ITEM_ID & source, const ITEM_ID & target, NEService::eMessageSource msgSource) const
{
    return NERemoteService::createConnectRequest(source, target, msgSource);
}

RemoteMessage ServiceClientConnectionBase::createServiceDisconnectMessage(const ITEM_ID & source, const ITEM_ID & target) const
{
    return NERemoteService::createDisconnectRequest(source, target);
}

void ServiceClientConnectionBase::onServiceReconnectTimerExpired( void )
{
    LOG_SCOPE( areg_ipc_private_ServiceClientConnectionBase_onServiceReconnectTimerExpired );
    onServiceStart( );
}

void ServiceClientConnectionBase::onServiceStart(void)
{
    LOG_SCOPE(areg_ipc_private_ServiceClientConnectionBase_onServiceConnectionStart);
    LOG_DBG("Starting remove servicing");

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
    LOG_SCOPE(areg_ipc_private_ServiceClientConnectionBase_onServiceConnectionStop);
    LOG_DBG("Stopping remote servicing");

    setConnectionState(ServiceClientConnectionBase::eConnectionState::ConnectionStopping);

    mTimerConnect.stopTimer( );

    Channel channel{ mChannel };
    mChannel.invalidate();

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
    LOG_SCOPE(areg_ipc_private_ServiceClientConnectionBase_onServiceConnectionStarted);
    ASSERT(isConnectionStarted());
    if ( mClientConnection.getCookie() != NEService::COOKIE_LOCAL )
    {
        LOG_DBG("Succeeded to start router service client, cookie [ %llu ]", mClientConnection.getCookie());

        mChannel.setCookie( mClientConnection.getCookie() );
        mChannel.setSource( mMessageDispatcher.getId());
        mChannel.setTarget( mTarget );
        setConnectionState(ServiceClientConnectionBase::eConnectionState::ConnectionStarted);
        mConnectionConsumer.connectedRemoteServiceChannel(mChannel);
    }
}

void ServiceClientConnectionBase::onServiceConnectionStopped(void)
{
    LOG_SCOPE(areg_ipc_private_ServiceClientConnectionBase_onServiceConnectionStopped);
    LOG_DBG("Client service is stopped. Resetting cookie");

    setConnectionState(ServiceClientConnectionBase::eConnectionState::ConnectionStopped);
    mTimerConnect.stopTimer( );

    Channel channel = mChannel;
    mChannel.invalidate();

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
    LOG_SCOPE(areg_ipc_private_ServiceClientConnectionBase_onServiceConnectionLost);
    LOG_WARN("Client service lost connection. Resetting cookie and trying to restart, current connection state [ %s ]"
                , ServiceClientConnectionBase::getString(getConnectionState()));

    setConnectionState(ServiceClientConnectionBase::eConnectionState::ConnectionStopped);
    Channel channel = mChannel;
    mChannel.invalidate();

    if ( Application::isServicingReady( ) && mTimerConnect.isStopped( ) )
    {
        LOG_DBG( "Restarting lost connection with remote service" );

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

        LOG_WARN("Ignoring lost connection event, either servicing state is not allowed, or application is closing.");
    }
}

void ServiceClientConnectionBase::onServiceExit( void )
{
    onServiceStop( );
}

void ServiceClientConnectionBase::onServiceMessageReceived( const RemoteMessage & /* msgReceived */ )
{
}

void ServiceClientConnectionBase::onServiceMessageSend( const RemoteMessage & /* msgSend */ )
{
}

void ServiceClientConnectionBase::onChannelConnected(const ITEM_ID& cookie)
{
    LOG_SCOPE(areg_ipc_private_ServiceClientConnectionBase_onChannelConnected);
    Lock lock(mLock);

    if (cookie >= NEService::COOKIE_REMOTE_SERVICE)
    {
        mChannel.setCookie(cookie);
        mChannel.setSource(mMessageDispatcher.getId());
        mChannel.setTarget(mTarget);

        LOG_DBG("Connected remote channel [ source = %llu, target = %llu, cookie = %llu ]", mChannel.getSource(), mChannel.getTarget(), mChannel.getCookie());
    }
    else
    {
        LOG_INFO("Disconnecting remote channel [ source = %llu, target = %llu, cookie = %llu ]", mChannel.getSource(), mChannel.getTarget(), mChannel.getCookie());
        mChannel.invalidate();
    }
}

bool ServiceClientConnectionBase::startConnection(void)
{
    LOG_SCOPE(areg_ipc_private_ServiceClientConnectionBase_startConnection);

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
            LOG_DBG("Client service starting connection with remote routing service.");
            result = mClientConnection.sendMessage(createServiceConnectMessage(NEService::COOKIE_UNKNOWN, mTarget, mMessageSource));
        }
    }

    if ( result == false )
    {
        LOG_WARN("Client service failed to start connection, going to repeat connection in [ %u ] ms", NEConnection::DEFAULT_RETRY_CONNECT_TIMEOUT);
        mThreadSend.shutdownThread( NECommon::DO_NOT_WAIT );
        mThreadReceive.shutdownThread( NECommon::DO_NOT_WAIT );
        mClientConnection.closeSocket();
        mTimerConnect.startTimer(NEConnection::DEFAULT_RETRY_CONNECT_TIMEOUT, mMessageDispatcher, 1);
    }

    return result;
}

void ServiceClientConnectionBase::cancelConnection(void)
{
    LOG_SCOPE(areg_ipc_private_ServiceClientConnectionBase_cancelConnection);
    LOG_WARN("Canceling client service connection");

    mClientConnection.closeSocket();
    mClientConnection.setCookie( NEService::COOKIE_UNKNOWN );

    mThreadReceive.shutdownThread( NECommon::DO_NOT_WAIT );
    mThreadSend.shutdownThread( NECommon::DO_NOT_WAIT );
}
