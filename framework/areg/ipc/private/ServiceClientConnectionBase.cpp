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
                                                            , areg::RemoteServiceKind service
                                                            , uint32_t connectTypes
                                                            , areg::MessageSource msgSource
                                                            , areg::ConnectionConsumer& connectionConsumer
                                                            , areg::RemoteMessageHandler & messageHandler
                                                            , areg::DispatcherThread & messageDispatcher
                                                            , const areg::String & prefixName)
        : areg::ConnectionProvider   ( )
        , areg::ServiceEventConsumer    ( )

        , mTarget               (target)
        , mService              (service)
        , mConnectTypes         (connectTypes)
        , mMessageSource        (msgSource)
        , mClientConnection     ( )
        , mConnectionConsumer   (connectionConsumer)
        , mMessageDispatcher    (messageDispatcher)
        , mChannel              ( )
        , mConnectionState      ( ConnectionPhase::ConnectionStopped )
        , mEventConsumer        ( static_cast<areg::ServiceEventConsumer &>(self()) )
        , mLock                 ( )

        , mTimerConnect         ( static_cast<areg::TimerConsumer &>(mTimerConsumer), prefixName + areg::CLIENT_CONNECT_TIMER_NAME )
        , mThreadReceive        (messageHandler, mClientConnection, prefixName)
        , mThreadSend           (messageHandler, mClientConnection, prefixName)
        , mTimerConsumer        ( static_cast<areg::ServiceEventConsumer &>(self()) )
    {
        ASSERT((target > areg::TARGET_LOCAL) && (target < areg::COOKIE_REMOTE_SERVICE));
    }

    ServiceClientConnectionBase::~ServiceClientConnectionBase()
    {
    }

    void ServiceClientConnectionBase::serviceConnectionEvent(const areg::RemoteMessage& msgReceived)
    {
        LOG_SCOPE(areg_ipc_private_ServiceClientConnectionBase_serviceConnectionEvent);

        msgReceived.moveToBegin();
        ITEM_ID cookie{ areg::COOKIE_UNKNOWN };
        areg::ServiceConnectionState connection{ areg::ServiceConnectionState::Unknown };
        msgReceived >> cookie;
        msgReceived >> connection;
        LOG_DBG("Remote service connection notification: status [ %s ], cookie [ %llu ]", areg::getString(connection), cookie);

        switch (connection)
        {
        case areg::ServiceConnectionState::Connected:
        case areg::ServiceConnectionState::Pending:
            {
                if (msgReceived.getResult() == areg::MESSAGE_SUCCESS)
                {
                    Lock lock(mLock);
                    ASSERT(cookie == msgReceived.getTarget());
                    mClientConnection.setCookie(cookie);
                    onChannelConnected(cookie);
                    sendCommand(areg::ServiceEventData::ServiceCommand::CMD_ServiceStarted);
                }
                else
                {
                    cancelConnection();
                    onChannelConnected(areg::COOKIE_UNKNOWN);
                    sendCommand(areg::ServiceEventData::ServiceCommand::CMD_ServiceLost);
                }
            }
            break;

        case areg::ServiceConnectionState::ConnectionLost:
        case areg::ServiceConnectionState::Disconnected:
        case areg::ServiceConnectionState::Failed:
            {
                cancelConnection();
                onChannelConnected(areg::COOKIE_UNKNOWN);
                sendCommand(areg::ServiceEventData::ServiceCommand::CMD_ServiceLost);
            }
            break;

        case areg::ServiceConnectionState::Unknown:   // fall through
        case areg::ServiceConnectionState::Rejected:            // fall through
        case areg::ServiceConnectionState::Shutdown:            // fall through
        default:
            {
                cancelConnection();
                onChannelConnected(areg::COOKIE_UNKNOWN);
                sendCommand(areg::ServiceEventData::ServiceCommand::CMD_ServiceStopped);
            }
            break;
        }
    }

    bool ServiceClientConnectionBase::setupServiceConnectionData(areg::RemoteServiceKind service, uint32_t connectTypes)
    {
        Lock lock( mLock );

        bool result{ false };
        if ((mService == service) && ((mConnectTypes & connectTypes) != 0))
        {
            if ((mConnectTypes & static_cast<uint32_t>(areg::ConnectionType::Tcpip)) != 0)
            {
                areg::ConnectionConfiguration config(service, areg::ConnectionType::Tcpip);
                if (config.isConfigured() && config.getConnectionEnableFlag())
                {
                    areg::String address{ config.getConnectionAddress() };
                    uint16_t port{ config.getConnectionPort() };
                    result = mClientConnection.setAddress(address, port);
                }
            }
        }

        return result;
    }

    void ServiceClientConnectionBase::applyServiceConnectionData( const areg::String & hostName, uint16_t portNr )
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
            if ((mConnectTypes & static_cast<uint32_t>(areg::ConnectionType::Tcpip)) != 0)
            {
                areg::ConnectionConfiguration config(mService, areg::ConnectionType::Tcpip);
                if (config.isConfigured() && config.getConnectionEnableFlag())
                {
                    result = true;
                    sendCommand(areg::ServiceEventData::ServiceCommand::CMD_StartService);
                }
            }
        }

        return result;
    }

    bool ServiceClientConnectionBase::reconnectServiceHost()
    {
        disconnectServiceHost( );
        sendCommand(areg::ServiceEventData::ServiceCommand::CMD_StartService );

        return true;
    }

    void ServiceClientConnectionBase::disconnectServiceHost()
    {
        sendCommand(areg::ServiceEventData::ServiceCommand::CMD_ServiceExit, areg::Event::EventPriority::NormalPrio);
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

    areg::RemoteMessage ServiceClientConnectionBase::createServiceConnectMessage(const ITEM_ID & source, const ITEM_ID & target, areg::MessageSource msgSource) const
    {
        return areg::createConnectRequest(source, target, msgSource);
    }

    areg::RemoteMessage ServiceClientConnectionBase::createServiceDisconnectMessage(const ITEM_ID & source, const ITEM_ID & target) const
    {
        return areg::createDisconnectRequest(source, target);
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

        mChannel.setCookie( areg::COOKIE_LOCAL );
        mChannel.setSource( areg::SOURCE_UNKNOWN );
        mChannel.setTarget( areg::TARGET_UNKNOWN );
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

        areg::Channel channel{ mChannel };
        mChannel.invalidate();

        mThreadReceive.triggerExit( );

        if ((channel.getTarget() > areg::COOKIE_LOCAL) && (channel.getTarget() < areg::COOKIE_REMOTE_SERVICE))
        {
            sendMessage(createServiceDisconnectMessage(channel.getCookie(), mTarget));
        }

        disconnectService( areg::Event::EventPriority::NormalPrio );

        mThreadSend.completionWait( areg::WAIT_INFINITE );
        mThreadSend.shutdownThread( areg::DO_NOT_WAIT );
        mClientConnection.closeSocket( );
        mThreadReceive.shutdownThread( areg::WAIT_INFINITE );

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
        if ( mClientConnection.getCookie() != areg::COOKIE_LOCAL )
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

        areg::Channel channel = mChannel;
        mChannel.invalidate();

        cancelConnection( );

        mThreadReceive.shutdownThread( areg::WAIT_INFINITE );
        mThreadSend.shutdownThread( areg::WAIT_INFINITE );
        mConnectionConsumer.disconnectedRemoteServiceChannel( channel );

        if ( areg::Application::isServicingReady( ) )
        {
            mTimerConnect.startTimer(areg::DEFAULT_RETRY_CONNECT_TIMEOUT, mMessageDispatcher, 1 );
        }
    }

    void ServiceClientConnectionBase::onServiceConnectionLost()
    {
        LOG_SCOPE(areg_ipc_private_ServiceClientConnectionBase_onServiceConnectionLost);
        LOG_WARN("Client service lost connection. Resetting cookie and trying to restart, current connection state [ %s ]"
                    , ServiceClientConnectionBase::getString(getConnectionState()));

        setConnectionState(ServiceClientConnectionBase::ConnectionPhase::ConnectionStopped);
        areg::Channel channel = mChannel;
        mChannel.invalidate();

        if ( areg::Application::isServicingReady( ) && mTimerConnect.isStopped( ) )
        {
            LOG_DBG( "Restarting lost connection with remote service" );

            mThreadReceive.shutdownThread( areg::WAIT_INFINITE );
            mThreadSend.shutdownThread( areg::WAIT_INFINITE );
            mConnectionConsumer.lostRemoteServiceChannel( channel );

            mTimerConnect.startTimer(areg::DEFAULT_RETRY_CONNECT_TIMEOUT, mMessageDispatcher, 1 );
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

    void ServiceClientConnectionBase::onServiceMessageReceived( const areg::RemoteMessage & /* msgReceived */ )
    {
    }

    void ServiceClientConnectionBase::onServiceMessageSend( const areg::RemoteMessage & /* msgSend */ )
    {
    }

    void ServiceClientConnectionBase::onChannelConnected(const ITEM_ID& cookie)
    {
        LOG_SCOPE(areg_ipc_private_ServiceClientConnectionBase_onChannelConnected);
        Lock lock(mLock);

        if (cookie >= areg::COOKIE_REMOTE_SERVICE)
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
            if ( mThreadReceive.createThread( areg::WAIT_INFINITE ) && mThreadSend.createThread( areg::WAIT_INFINITE ) )
            {
                VERIFY( mThreadReceive.waitForDispatcherStart( areg::WAIT_INFINITE ) );
                VERIFY( mThreadSend.waitForDispatcherStart( areg::WAIT_INFINITE ) );
                LOG_DBG("Client service starting connection with remote routing service.");
                result = mClientConnection.sendMessage(createServiceConnectMessage(areg::COOKIE_UNKNOWN, mTarget, mMessageSource));
            }
        }

        if ( result == false )
        {
            LOG_WARN("Client service failed to start connection, going to repeat connection in [ %u ] ms", areg::DEFAULT_RETRY_CONNECT_TIMEOUT);
            mThreadSend.shutdownThread( areg::DO_NOT_WAIT );
            mThreadReceive.shutdownThread( areg::DO_NOT_WAIT );
            mClientConnection.closeSocket();
            mTimerConnect.startTimer(areg::DEFAULT_RETRY_CONNECT_TIMEOUT, mMessageDispatcher, 1);
        }

        return result;
    }

    void ServiceClientConnectionBase::cancelConnection()
    {
        LOG_SCOPE(areg_ipc_private_ServiceClientConnectionBase_cancelConnection);
        LOG_WARN("Canceling client service connection");

        mClientConnection.closeSocket();
        mClientConnection.setCookie( areg::COOKIE_UNKNOWN );

        mThreadReceive.shutdownThread( areg::DO_NOT_WAIT );
        mThreadSend.shutdownThread( areg::DO_NOT_WAIT );
    }
    
} // namespace areg