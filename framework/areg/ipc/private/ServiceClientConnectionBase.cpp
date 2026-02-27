/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/ipc/private/ServiceClientConnectionBase.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Service Connection client implementation
 ************************************************************************/
#include "areg/ipc/ServiceClientConnectionBase.hpp"

#include "areg/component/ServiceDefs.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/ipc/ConnectionConsumer.hpp"
#include "areg/ipc/ConnectionConfiguration.hpp"
#include "areg/ipc/RemoteServiceDefs.hpp"
#include "areg/ipc/private/ConnectionDefs.hpp"
#include "areg/logging/GELog.h"

namespace areg
{

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
                                                            , RemoteServiceKind service
                                                            , uint32_t connectTypes
                                                            , MessageSource msgSource
                                                            , ConnectionConsumer& connectionConsumer
                                                            , RemoteMessageHandler & messageHandler
                                                            , DispatcherThread & messageDispatcher
                                                            , const String & prefixName)
        : ConnectionProvider   ( )
        , ServiceEventConsumer    ( )

        , mTarget               (target)
        , mService              (service)
        , mConnectTypes         (connectTypes)
        , mMessageSource        (msgSource)
        , mClientConnection     ( )
        , mConnectionConsumer   (connectionConsumer)
        , mMessageDispatcher    (messageDispatcher)
        , mChannel              ( )
        , mConnectionState      ( ConnectionPhase::ConnectionStopped )
        , mEventConsumer        ( static_cast<ServiceEventConsumer &>(self()) )
        , mLock                 ( )

        , mTimerConnect         ( static_cast<TimerConsumer &>(mTimerConsumer), prefixName + CLIENT_CONNECT_TIMER_NAME )
        , mThreadReceive        (messageHandler, mClientConnection, prefixName)
        , mThreadSend           (messageHandler, mClientConnection, prefixName)
        , mTimerConsumer        ( static_cast<ServiceEventConsumer &>(self()) )
    {
        ASSERT((target > TARGET_LOCAL) && (target < COOKIE_REMOTE_SERVICE));
    }

    ServiceClientConnectionBase::~ServiceClientConnectionBase()
    {
    }

    void ServiceClientConnectionBase::serviceConnectionEvent(const RemoteMessage& msgReceived)
    {
        LOG_SCOPE(areg_ipc_private_ServiceClientConnectionBase_serviceConnectionEvent);

        msgReceived.moveToBegin();
        ITEM_ID cookie{ COOKIE_UNKNOWN };
        ServiceConnectionState connection{ ServiceConnectionState::Unknown };
        msgReceived >> cookie;
        msgReceived >> connection;
        LOG_DBG("Remote service connection notification: status [ %s ], cookie [ %llu ]", areg::getString(connection), cookie);

        switch (connection)
        {
        case ServiceConnectionState::Connected:
        case ServiceConnectionState::Pending:
            {
                if (msgReceived.getResult() == MESSAGE_SUCCESS)
                {
                    Lock lock(mLock);
                    ASSERT(cookie == msgReceived.getTarget());
                    mClientConnection.setCookie(cookie);
                    onChannelConnected(cookie);
                    sendCommand(ServiceEventData::ServiceCommand::CMD_ServiceStarted);
                }
                else
                {
                    cancelConnection();
                    onChannelConnected(COOKIE_UNKNOWN);
                    sendCommand(ServiceEventData::ServiceCommand::CMD_ServiceLost);
                }
            }
            break;

        case ServiceConnectionState::ConnectionLost:
        case ServiceConnectionState::Disconnected:
        case ServiceConnectionState::Failed:
            {
                cancelConnection();
                onChannelConnected(COOKIE_UNKNOWN);
                sendCommand(ServiceEventData::ServiceCommand::CMD_ServiceLost);
            }
            break;

        case ServiceConnectionState::Unknown:   // fall through
        case ServiceConnectionState::Rejected:            // fall through
        case ServiceConnectionState::Shutdown:            // fall through
        default:
            {
                cancelConnection();
                onChannelConnected(COOKIE_UNKNOWN);
                sendCommand(ServiceEventData::ServiceCommand::CMD_ServiceStopped);
            }
            break;
        }
    }

    bool ServiceClientConnectionBase::setupServiceConnectionData(RemoteServiceKind service, uint32_t connectTypes)
    {
        Lock lock( mLock );

        bool result{ false };
        if ((mService == service) && ((mConnectTypes & connectTypes) != 0))
        {
            if ((mConnectTypes & static_cast<uint32_t>(ConnectionType::Tcpip)) != 0)
            {
                ConnectionConfiguration config(service, ConnectionType::Tcpip);
                if (config.isConfigured() && config.getConnectionEnableFlag())
                {
                    String address{ config.getConnectionAddress() };
                    uint16_t port{ config.getConnectionPort() };
                    result = mClientConnection.setAddress(address, port);
                }
            }
        }

        return result;
    }

    void ServiceClientConnectionBase::applyServiceConnectionData( const String & hostName, uint16_t portNr )
    {
        Lock lock( mLock );
        mClientConnection.setAddress( hostName, portNr );
    }

    bool ServiceClientConnectionBase::connectServiceHost()
    {
        Lock lock( mLock );
        bool result{ false };
        if (mClientConnection.isValid() == false)
        {
            if ((mConnectTypes & static_cast<uint32_t>(ConnectionType::Tcpip)) != 0)
            {
                ConnectionConfiguration config(mService, ConnectionType::Tcpip);
                if (config.isConfigured() && config.getConnectionEnableFlag())
                {
                    result = true;
                    sendCommand(ServiceEventData::ServiceCommand::CMD_StartService);
                }
            }
        }

        return result;
    }

    bool ServiceClientConnectionBase::reconnectServiceHost()
    {
        disconnectServiceHost( );
        sendCommand(ServiceEventData::ServiceCommand::CMD_StartService );

        return true;
    }

    void ServiceClientConnectionBase::disconnectServiceHost()
    {
        sendCommand(ServiceEventData::ServiceCommand::CMD_ServiceExit, Event::EventPriority::NormalPrio);
    }

    bool ServiceClientConnectionBase::isServiceHostConnected() const
    {
        Lock lock( mLock );
        return isConnectionStarted();
    }

    bool ServiceClientConnectionBase::isServiceHostPending() const
    {
        Lock lock(mLock);
        return ((mClientConnection.isValid() == false) && (getConnectionState() == ServiceClientConnectionBase::ConnectionPhase::ConnectionStarting));
    }

    bool ServiceClientConnectionBase::isServiceHostSetup() const
    {
        Lock lock(mLock);
        return mClientConnection.getAddress().isValid();
    }

    RemoteMessage ServiceClientConnectionBase::createServiceConnectMessage(const ITEM_ID & source, const ITEM_ID & target, MessageSource msgSource) const
    {
        return createConnectRequest(source, target, msgSource);
    }

    RemoteMessage ServiceClientConnectionBase::createServiceDisconnectMessage(const ITEM_ID & source, const ITEM_ID & target) const
    {
        return createDisconnectRequest(source, target);
    }

    void ServiceClientConnectionBase::onServiceReconnectTimerExpired()
    {
        LOG_SCOPE( areg_ipc_private_ServiceClientConnectionBase_onServiceReconnectTimerExpired );
        onServiceStart( );
    }

    void ServiceClientConnectionBase::onServiceStart()
    {
        LOG_SCOPE(areg_ipc_private_ServiceClientConnectionBase_onServiceConnectionStart);
        LOG_DBG("Starting remove servicing");

        mChannel.setCookie( COOKIE_LOCAL );
        mChannel.setSource( SOURCE_UNKNOWN );
        mChannel.setTarget( TARGET_UNKNOWN );
        if ( startConnection( ) )
        {
            setConnectionState( ServiceClientConnectionBase::ConnectionPhase::ConnectionStarting );
        }
    }

    void ServiceClientConnectionBase::onServiceStop()
    {
        LOG_SCOPE(areg_ipc_private_ServiceClientConnectionBase_onServiceConnectionStop);
        LOG_DBG("Stopping remote servicing");

        setConnectionState(ServiceClientConnectionBase::ConnectionPhase::ConnectionStopping);

        mTimerConnect.stopTimer( );

        Channel channel{ mChannel };
        mChannel.invalidate();

        mThreadReceive.triggerExit( );

        if ((channel.getTarget() > COOKIE_LOCAL) && (channel.getTarget() < COOKIE_REMOTE_SERVICE))
        {
            sendMessage(createServiceDisconnectMessage(channel.getCookie(), mTarget));
        }

        disconnectService( Event::EventPriority::NormalPrio );

        mThreadSend.completionWait( WAIT_INFINITE );
        mThreadSend.shutdownThread( DO_NOT_WAIT );
        mClientConnection.closeSocket( );
        mThreadReceive.shutdownThread( WAIT_INFINITE );

        mConnectionConsumer.disconnectedRemoteServiceChannel( channel );
    }

    void ServiceClientConnectionBase::onServiceRestart()
    {
        onServiceStop( );
        onServiceStart( );
    }

    void ServiceClientConnectionBase::onServiceConnectionStarted()
    {
        LOG_SCOPE(areg_ipc_private_ServiceClientConnectionBase_onServiceConnectionStarted);
        ASSERT(isConnectionStarted());
        if ( mClientConnection.getCookie() != COOKIE_LOCAL )
        {
            LOG_DBG("Succeeded to start router service client, cookie [ %llu ]", mClientConnection.getCookie());

            mChannel.setCookie( mClientConnection.getCookie() );
            mChannel.setSource( mMessageDispatcher.getId());
            mChannel.setTarget( mTarget );
            setConnectionState(ServiceClientConnectionBase::ConnectionPhase::ConnectionStarted);
            mConnectionConsumer.connectedRemoteServiceChannel(mChannel);
        }
    }

    void ServiceClientConnectionBase::onServiceConnectionStopped()
    {
        LOG_SCOPE(areg_ipc_private_ServiceClientConnectionBase_onServiceConnectionStopped);
        LOG_DBG("Client service is stopped. Resetting cookie");

        setConnectionState(ServiceClientConnectionBase::ConnectionPhase::ConnectionStopped);
        mTimerConnect.stopTimer( );

        Channel channel = mChannel;
        mChannel.invalidate();

        cancelConnection( );

        mThreadReceive.shutdownThread( WAIT_INFINITE );
        mThreadSend.shutdownThread( WAIT_INFINITE );
        mConnectionConsumer.disconnectedRemoteServiceChannel( channel );

        if ( Application::isServicingReady( ) )
        {
            mTimerConnect.startTimer(DEFAULT_RETRY_CONNECT_TIMEOUT, mMessageDispatcher, 1 );
        }
    }

    void ServiceClientConnectionBase::onServiceConnectionLost()
    {
        LOG_SCOPE(areg_ipc_private_ServiceClientConnectionBase_onServiceConnectionLost);
        LOG_WARN("Client service lost connection. Resetting cookie and trying to restart, current connection state [ %s ]"
                    , ServiceClientConnectionBase::getString(getConnectionState()));

        setConnectionState(ServiceClientConnectionBase::ConnectionPhase::ConnectionStopped);
        Channel channel = mChannel;
        mChannel.invalidate();

        if ( Application::isServicingReady( ) && mTimerConnect.isStopped( ) )
        {
            LOG_DBG( "Restarting lost connection with remote service" );

            mThreadReceive.shutdownThread( WAIT_INFINITE );
            mThreadSend.shutdownThread( WAIT_INFINITE );
            mConnectionConsumer.lostRemoteServiceChannel( channel );

            mTimerConnect.startTimer(DEFAULT_RETRY_CONNECT_TIMEOUT, mMessageDispatcher, 1 );
        }
        else
        {
            ASSERT( mThreadReceive.isRunning( ) == false );
            ASSERT( mThreadSend.isRunning( ) == false );
            ASSERT( mClientConnection.isValid( ) == false );

            LOG_WARN("Ignoring lost connection event, either servicing state is not allowed, or application is closing.");
        }
    }

    void ServiceClientConnectionBase::onServiceExit()
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

        if (cookie >= COOKIE_REMOTE_SERVICE)
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

    bool ServiceClientConnectionBase::startConnection()
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
            if ( mThreadReceive.createThread( WAIT_INFINITE ) && mThreadSend.createThread( WAIT_INFINITE ) )
            {
                VERIFY( mThreadReceive.waitForDispatcherStart( WAIT_INFINITE ) );
                VERIFY( mThreadSend.waitForDispatcherStart( WAIT_INFINITE ) );
                LOG_DBG("Client service starting connection with remote routing service.");
                result = mClientConnection.sendMessage(createServiceConnectMessage(COOKIE_UNKNOWN, mTarget, mMessageSource));
            }
        }

        if ( result == false )
        {
            LOG_WARN("Client service failed to start connection, going to repeat connection in [ %u ] ms", DEFAULT_RETRY_CONNECT_TIMEOUT);
            mThreadSend.shutdownThread( DO_NOT_WAIT );
            mThreadReceive.shutdownThread( DO_NOT_WAIT );
            mClientConnection.closeSocket();
            mTimerConnect.startTimer(DEFAULT_RETRY_CONNECT_TIMEOUT, mMessageDispatcher, 1);
        }

        return result;
    }

    void ServiceClientConnectionBase::cancelConnection()
    {
        LOG_SCOPE(areg_ipc_private_ServiceClientConnectionBase_cancelConnection);
        LOG_WARN("Canceling client service connection");

        mClientConnection.closeSocket();
        mClientConnection.setCookie( COOKIE_UNKNOWN );

        mThreadReceive.shutdownThread( DO_NOT_WAIT );
        mThreadSend.shutdownThread( DO_NOT_WAIT );
    }
    
} // namespace areg