/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/ipc/private/ServiceClientConnection.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Service Connection client implementation
 ************************************************************************/
#include "areg/ipc/private/ServiceClientConnection.hpp"

#include "areg/ipc/IEServiceConnectionConsumer.hpp"
#include "areg/ipc/IEServiceRegisterConsumer.hpp"
#include "areg/ipc/ConnectionConfiguration.hpp"
#include "areg/ipc/NEConnection.hpp"

#include "areg/component/RemoteEventFactory.hpp"
#include "areg/component/DispatcherThread.hpp"
#include "areg/component/StreamableEvent.hpp"
#include "areg/component/ResponseEvents.hpp"
#include "areg/component/RequestEvents.hpp"
#include "areg/component/NEService.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/File.hpp"
#include "areg/trace/GETrace.h"

DEF_TRACE_SCOPE(areg_ipc_private_ServiceClientConnection_startRemotingService);
DEF_TRACE_SCOPE(areg_ipc_private_ServiceClientConnection_stopRemotingService);

DEF_TRACE_SCOPE(areg_ipc_private_ServiceClientConnection_onServiceReconnectTimerExpired);
DEF_TRACE_SCOPE(areg_ipc_private_ServiceClientConnection_onServiceConnectionStart);
DEF_TRACE_SCOPE(areg_ipc_private_ServiceClientConnection_onServiceConnectionStop);
DEF_TRACE_SCOPE(areg_ipc_private_ServiceClientConnection_onServiceConnectionStarted);
DEF_TRACE_SCOPE(areg_ipc_private_ServiceClientConnection_onServiceConnectionStopped);
DEF_TRACE_SCOPE(areg_ipc_private_ServiceClientConnection_onServiceConnectionLost);

DEF_TRACE_SCOPE(areg_ipc_private_ServiceClientConnection__startConnection);
DEF_TRACE_SCOPE(areg_ipc_private_ServiceClientConnection__stopReceiveData);
DEF_TRACE_SCOPE(areg_ipc_private_ServiceClientConnection__stopSendData);
DEF_TRACE_SCOPE(areg_ipc_private_ServiceClientConnection__cancelConnection);
DEF_TRACE_SCOPE(areg_ipc_private_ServiceClientConnection_failedSendMessage);
DEF_TRACE_SCOPE(areg_ipc_private_ServiceClientConnection_failedReceiveMessage);
DEF_TRACE_SCOPE(areg_ipc_private_ServiceClientConnection_failedProcessMessage);
DEF_TRACE_SCOPE(areg_ipc_private_ServiceClientConnection_processReceivedMessage);
DEF_TRACE_SCOPE(areg_ipc_private_ServiceClientConnection_processRemoteRequestEvent);
DEF_TRACE_SCOPE(areg_ipc_private_ServiceClientConnection_processRemoteResponseEvent);
DEF_TRACE_SCOPE(areg_ipc_private_ServiceClientConnection_processRemoteNotifyRequestEvent);
DEF_TRACE_SCOPE(areg_ipc_private_ServiceClientConnection_runDispatcher);

DEF_TRACE_SCOPE(areg_ipc_private_ServiceClientConnection_registerServiceProvider);
DEF_TRACE_SCOPE(areg_ipc_private_ServiceClientConnection_unregisterServiceProvider);
DEF_TRACE_SCOPE(areg_ipc_private_ServiceClientConnection_registerServiceConsumer);
DEF_TRACE_SCOPE(areg_ipc_private_ServiceClientConnection_unregisterServiceConsumer);

//////////////////////////////////////////////////////////////////////////
// ServiceClientConnection class implementation
//////////////////////////////////////////////////////////////////////////

ServiceClientConnection::ServiceClientConnection(IEServiceConnectionConsumer& connectionConsumer, IEServiceRegisterConsumer& registerConsumer)
    : IEServiceConnectionProvider   ( )
    , IEServiceRegisterProvider     ( )
    , DispatcherThread              ( NEConnection::CLIENT_DISPATCH_MESSAGE_THREAD.data() )
    , IERemoteMessageHandler        ( )
    , IERemoteEventConsumer         ( )
    , IEServiceEventConsumerBase    ( )

    , mClientConnection     ( )
    , mConnectionConsumer   (connectionConsumer)
    , mRegisterConsumer     (registerConsumer)
    , mTimerConnect         ( static_cast<IETimerConsumer &>(mTimerConsumer), NEConnection::CLIENT_CONNECT_TIMER_NAME )
    , mThreadReceive        ( static_cast<IERemoteMessageHandler&>(self()), mClientConnection )
    , mThreadSend           ( static_cast<IERemoteMessageHandler&>(self()), mClientConnection )
    , mIsServiceEnabled     ( NEConnection::DEFAULT_REMOTE_SERVICE_ENABLED )    // TODO: by default, should be false and read out from configuration file.
    , mConfigFile           ( "" )
    , mChannel              ( )
    , mConnectionState      ( eConnectionState::ConnectionStopped )
    , mEventConsumer        ( static_cast<IEServiceEventConsumerBase &>(self()) )
    , mTimerConsumer        ( static_cast<IEServiceEventConsumerBase &>(self()) )
    , mLock                 ( )
{
}

bool ServiceClientConnection::setupServiceConnectionHost( const String & configFile )
{
    Lock lock( mLock );
    ConnectionConfiguration configConnect;
    if ( configConnect.loadConfiguration( configFile ) )
    {
        mConfigFile             = configConnect.getConfigFileName( );
        mIsServiceEnabled       = configConnect.getConnectionEnableFlag( CONNECT_TYPE );
        String hostName         = configConnect.getConnectionHost( CONNECT_TYPE );
        unsigned short hostPort = configConnect.getConnectionPort( CONNECT_TYPE );

        return mClientConnection.setAddress( hostName, hostPort );
    }
    else
    {
        mIsServiceEnabled       = NEConnection::DEFAULT_REMOTE_SERVICE_ENABLED;
        return mClientConnection.setAddress( NEConnection::DEFAULT_REMOTE_SERVICE_HOST.data(), NEConnection::DEFAULT_REMOTE_SERVICE_PORT );
    }
}

void ServiceClientConnection::applyServiceConnectionData( const String & hostName, unsigned short portNr )
{
    Lock lock( mLock );
    mClientConnection.setAddress( hostName, portNr );
}

bool ServiceClientConnection::connectServiceHost(void)
{
    TRACE_SCOPE(areg_ipc_private_ServiceClientConnection_startRemotingService);
    Lock lock( mLock );
    bool result = true;
    if ( mClientConnection.isValid() == false && isRunning() == false )
    {
        if ( isRemoteServicingEnabled() )
        {
            lock.unlock();

            TRACE_DBG("The remote servicing is enabled, going to start remote servicing");
            result = createThread(NECommon::WAIT_INFINITE) && waitForDispatcherStart(NECommon::WAIT_INFINITE);

            lock.lock();
        }
        else
        {
            TRACE_DBG("The remote servicing is disabled, nothing to start");
            result = true;
        }
    }
    else
    {
        TRACE_DBG("Ignoring to start remote servicing. The client connection is [ %s ], the servicing thread is [ %s ]"
                    , mClientConnection.isValid() ? "VALID" : "INVALID"
                    , isRunning() ? "RUNNING" : "NOT RUNNING");
        ASSERT(isRunning());
    }

    return result;
}

bool ServiceClientConnection::reconnectServiceHost(void)
{
    disconnectServiceHost( );
    _sendCommand(ServiceEventData::eServiceEventCommands::CMD_StartService );

    return true;
}

void ServiceClientConnection::disconnectServiceHost(void)
{
    TRACE_SCOPE(areg_ipc_private_ServiceClientConnection_stopRemotingService);
    TRACE_DBG( "Stopping remote servicing client connection, current state is [ %s ]", isRunning() ? "RUNNING" : "NOT RUNNING" );
    if ( isRunning() )
    {
        _sendCommand(ServiceEventData::eServiceEventCommands::CMD_ServiceExit, Event::eEventPriority::EventPriorityCritical);
        completionWait(NECommon::WAIT_INFINITE);
        destroyThread( NECommon::DO_NOT_WAIT );
    }
}

bool ServiceClientConnection::isServiceHostConnected(void) const
{
    Lock lock( mLock );
    return _isStarted();
}

bool ServiceClientConnection::isServiceHostSetup( void ) const
{
    Lock lock(mLock);
    return mClientConnection.getAddress().isValid();
}

bool ServiceClientConnection::isRemoteServicingEnabled(void) const
{
    Lock lock(mLock);
    return mIsServiceEnabled;
}

void ServiceClientConnection::enableRemoteServicing( bool enable )
{
    Lock lock( mLock );
    if ( (enable == false) && isRunning( ) )
    {
        disconnectServiceHost( );
    }

    mIsServiceEnabled = enable;
}

bool ServiceClientConnection::registerServiceProvider( const StubAddress & stubService )
{
    TRACE_SCOPE(areg_ipc_private_ServiceClientConnection_registerServiceProvider);
    Lock lock( mLock );
    bool result = false;
    if ( _isStarted() )
    {
        TRACE_DBG("Queuing to send register [ %s ] service message by connection [ %d ]"
                   , StubAddress::convAddressToPath(stubService).getString()
                   , mClientConnection.getCookie());

        result = _sendMessage( NEConnection::createRouterRegisterService(stubService, mClientConnection.getCookie()), Event::eEventPriority::EventPriorityHigh );
    }
    return result;
}

void ServiceClientConnection::unregisterServiceProvider(const StubAddress & stubService, const NEService::eDisconnectReason reason )
{
    TRACE_SCOPE(areg_ipc_private_ServiceClientConnection_unregisterServiceProvider);

    Lock lock( mLock );
    if ( _isStarted() )
    {
        TRACE_DBG("Queuing to send unregister [ %s ] service message by connection [ %d ]"
                   , StubAddress::convAddressToPath(stubService).getString()
                   , mClientConnection.getCookie());

        _sendMessage( NEConnection::createRouterUnregisterService(stubService, reason, mClientConnection.getCookie()), Event::eEventPriority::EventPriorityHigh);
    }
}

bool ServiceClientConnection::registerServiceConsumer(const ProxyAddress & proxyService)
{
    TRACE_SCOPE(areg_ipc_private_ServiceClientConnection_registerServiceConsumer );
    Lock lock( mLock );
    bool result = false;
    if ( _isStarted() )
    {
        TRACE_DBG("Queuing to send register [ %s ] service client message by connection [ %d ]"
                   , ProxyAddress::convAddressToPath(proxyService).getString()
                   , mClientConnection.getCookie());

        result = _sendMessage( NEConnection::createRouterRegisterClient(proxyService, mClientConnection.getCookie()), Event::eEventPriority::EventPriorityHigh);
    }

    return result;
}

void ServiceClientConnection::unregisterServiceConsumer(const ProxyAddress & proxyService, const NEService::eDisconnectReason reason )
{
    TRACE_SCOPE(areg_ipc_private_ServiceClientConnection_unregisterServiceConsumer);

    Lock lock( mLock );
    if ( _isStarted() )
    {
        TRACE_DBG("Queuing to send unregister [ %s ] service client message by connection [ %d ]"
                   , ProxyAddress::convAddressToPath(proxyService).getString()
                   , mClientConnection.getCookie());

        _sendMessage( NEConnection::createRouterUnregisterClient(proxyService, reason, mClientConnection.getCookie()), Event::eEventPriority::EventPriorityHigh);
    }
}

void ServiceClientConnection::onServiceReconnectTimerExpired( void )
{
    TRACE_SCOPE( areg_ipc_private_ServiceClientConnection_onServiceReconnectTimerExpired );
    onServiceStart( );
}

void ServiceClientConnection::onServiceStart(void)
{
    TRACE_SCOPE(areg_ipc_private_ServiceClientConnection_onServiceConnectionStart);
    TRACE_DBG("Starting remove servicing");;

    mChannel.setCookie( NEService::COOKIE_LOCAL );
    mChannel.setSource( NEService::SOURCE_UNKNOWN );
    mChannel.setTarget( NEService::TARGET_UNKNOWN );
    if ( _startConnection( ) )
    {
        _setConnectionState( ServiceClientConnection::eConnectionState::ConnectionStarting );
    }
}

void ServiceClientConnection::onServiceStop(void)
{
    TRACE_SCOPE(areg_ipc_private_ServiceClientConnection_onServiceConnectionStop);
    TRACE_DBG("Stopping remote servicing");

    _setConnectionState(ServiceClientConnection::eConnectionState::ConnectionStopping);

    Channel channel = mChannel;
    mChannel.setCookie( NEService::COOKIE_UNKNOWN );
    mChannel.setSource( NEService::SOURCE_UNKNOWN );
    mChannel.setTarget( NEService::TARGET_UNKNOWN );

    _stopReceiveData( );
    _stopSendData( );
    mThreadSend.completionWait( NECommon::WAIT_INFINITE );
    mThreadReceive.completionWait( NECommon::WAIT_INFINITE );

    mClientConnection.closeSocket();
    mConnectionConsumer.disconnectedRemoteServiceChannel( channel );

    mThreadReceive.destroyThread( NECommon::DO_NOT_WAIT );
    mThreadSend.destroyThread( NECommon::DO_NOT_WAIT );
}

void ServiceClientConnection::onServiceRestart( void )
{
    onServiceStop( );
    onServiceStart( );
}

void ServiceClientConnection::onServiceConnectionStarted(void)
{
    TRACE_SCOPE(areg_ipc_private_ServiceClientConnection_onServiceConnectionStarted);
    ASSERT(_isStarted());
    if ( mClientConnection.getCookie() != NEService::COOKIE_LOCAL )
    {
        TRACE_DBG("Client service succeeded to start, client cookie is [ 0x%X ]", static_cast<unsigned int>(mClientConnection.getCookie()));

        _setConnectionState(ServiceClientConnection::eConnectionState::ConnectionStarted);
        mChannel.setCookie( mClientConnection.getCookie() );
        mChannel.setSource( getCurrentThreadId() );
        mChannel.setTarget( NEService::TARGET_LOCAL );
        mConnectionConsumer.connectedRemoteServiceChannel(mChannel);
    }
}

void ServiceClientConnection::onServiceConnectionStopped(void)
{
    TRACE_SCOPE(areg_ipc_private_ServiceClientConnection_onServiceConnectionStopped);
    TRACE_DBG("Client service is stopped. Resetting cookie");

    _setConnectionState(ServiceClientConnection::eConnectionState::ConnectionStopped);
    mTimerConnect.stopTimer( );

    Channel channel = mChannel;
    mChannel.setCookie( NEService::COOKIE_UNKNOWN );
    mChannel.setSource( NEService::SOURCE_UNKNOWN );
    mChannel.setTarget( NEService::TARGET_UNKNOWN );

    _cancelConnection( );

    mThreadReceive.completionWait( NECommon::WAIT_INFINITE );
    mThreadSend.completionWait( NECommon::WAIT_INFINITE );
    mConnectionConsumer.disconnectedRemoteServiceChannel( channel );

    if ( Application::isServicingReady( ) )
    {
        mTimerConnect.startTimer( NEConnection::DEFAULT_RETRY_CONNECT_TIMEOUT, static_cast<DispatcherThread &>(self( )), 1 );
    }
}

void ServiceClientConnection::onServiceConnectionLost(void)
{
    TRACE_SCOPE(areg_ipc_private_ServiceClientConnection_onServiceConnectionLost);
    TRACE_WARN("Client service lost connection. Resetting cookie and trying to restart, current connection state [ %s ]"
                , ServiceClientConnection::getString(_getConnectionState()));

    Channel channel = mChannel;
    mChannel.setCookie( NEService::COOKIE_UNKNOWN );
    mChannel.setSource( NEService::SOURCE_UNKNOWN );
    mChannel.setTarget( NEService::TARGET_UNKNOWN );

    if ( Application::isServicingReady( ) && mTimerConnect.isStopped( ) )
    {
        TRACE_DBG( "Restarting lost connection with remote service" );

        mThreadReceive.completionWait( NECommon::WAIT_INFINITE );
        mThreadSend.completionWait( NECommon::WAIT_INFINITE );
        mConnectionConsumer.lostRemoteServiceChannel( channel );

        // _setConnectionState( ServiceClientConnection::eConnectionState::ConnectionStarting );
        mTimerConnect.startTimer( NEConnection::DEFAULT_RETRY_CONNECT_TIMEOUT, static_cast<DispatcherThread &>(self( )), 1 );
    }
    else
    {
        ASSERT( mThreadReceive.isRunning( ) == false );
        ASSERT( mThreadSend.isRunning( ) == false );
        ASSERT( mClientConnection.isValid( ) == false );

        TRACE_WARN("Ignoring lost connection event, either servicing state is not allowed, or application is closing.");
    }
}

void ServiceClientConnection::onServiceExit( void )
{
    onServiceStop( );
    triggerExitEvent( );
}

void ServiceClientConnection::onServiceMessageReceived( const RemoteMessage & msgReceived )
{
}

void ServiceClientConnection::onServiceMessageSend( const RemoteMessage & msgSend )
{
}

inline bool ServiceClientConnection::_isStarted(void) const
{
    ITEM_ID cookie = mClientConnection.getCookie();
    return (mClientConnection.isValid() && (cookie != NEService::COOKIE_LOCAL) && (cookie != NEService::COOKIE_UNKNOWN));
}

inline void ServiceClientConnection::_setConnectionState(const ServiceClientConnection::eConnectionState newState)
{
    mConnectionState    = newState;
}

inline ServiceClientConnection::eConnectionState ServiceClientConnection::_getConnectionState( void ) const
{
    return mConnectionState;
}

inline bool ServiceClientConnection::_startConnection( void )
{
    TRACE_SCOPE(areg_ipc_private_ServiceClientConnection__startConnection);

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
            result = mClientConnection.requestConnectServer();
        }
    }

    if ( result == false )
    {
        TRACE_WARN("Client service failed to start connection, going to repeat connection in [ %u ] ms", NEConnection::DEFAULT_RETRY_CONNECT_TIMEOUT);
        mThreadSend.destroyThread( NECommon::DO_NOT_WAIT );
        mThreadReceive.destroyThread( NECommon::DO_NOT_WAIT );
        mClientConnection.closeSocket();
        mTimerConnect.startTimer( NEConnection::DEFAULT_RETRY_CONNECT_TIMEOUT, static_cast<DispatcherThread &>(self()), 1);
    }

    return result;
}

inline void ServiceClientConnection::_stopReceiveData(void)
{
    TRACE_SCOPE(areg_ipc_private_ServiceClientConnection__stopReceiveData);
    TRACE_WARN("Stopping service client to receive");
    mTimerConnect.stopTimer();

    mThreadReceive.triggerExitEvent();
    mClientConnection.disableReceive();
}

inline void ServiceClientConnection::_stopSendData(void)
{
    TRACE_SCOPE(areg_ipc_private_ServiceClientConnection__stopSendData);
    TRACE_WARN("Stopping client service connection");

    _disconnectService( Event::eEventPriority::EventPriorityHigh );
}

inline void ServiceClientConnection::_cancelConnection(void)
{
    TRACE_SCOPE(areg_ipc_private_ServiceClientConnection__cancelConnection);
    TRACE_WARN("Canceling client service connection");

    mClientConnection.closeSocket();
    mClientConnection.setCookie( NEService::COOKIE_UNKNOWN );

    mThreadReceive.triggerExitEvent();
    mThreadSend.triggerExitEvent();

    mThreadReceive.destroyThread( NECommon::DO_NOT_WAIT );
    mThreadSend.destroyThread( NECommon::DO_NOT_WAIT );
}

void ServiceClientConnection::failedSendMessage(const RemoteMessage & msgFailed, Socket & whichTarget )
{
    TRACE_SCOPE(areg_ipc_private_ServiceClientConnection_failedSendMessage);
    TRACE_WARN("Failed to send message [ 0x%X ] to target [ %u ], source is [ %u ]"
                    , static_cast<uint32_t>(msgFailed.getMessageId())
                    , static_cast<uint32_t>(msgFailed.getTarget())
                    , static_cast<uint32_t>(msgFailed.getSource()));

    if (Application::isServicingReady())
    {
        unsigned int msgId = msgFailed.getMessageId();
        if ( NEService::isExecutableId(msgId) || NEService::isConnectNotifyId(msgId) )
        {
            TRACE_DBG("Failed message [ %u ] is executable or connection notification", msgId);
            msgFailed.moveToBegin();
            StreamableEvent * eventError = RemoteEventFactory::createRequestFailedEvent(msgFailed, mChannel);
            if ( eventError != nullptr )
            {
                eventError->deliverEvent();
            }

            if ( whichTarget.isValid( ) )
            {
                _cancelConnection( );
                _sendCommand( ServiceEventData::eServiceEventCommands::CMD_ServiceLost, Event::eEventPriority::EventPriorityHigh );
            }
        }
        else
        {
            TRACE_WARN("The failed message is neither executable, nor connection notification. Ignoring to generate request failed event.");
        }
    }
    else
    {
        TRACE_WARN("Ignore send message failure, the application is closing");
    }
}

void ServiceClientConnection::failedReceiveMessage( Socket & whichSource )
{
    TRACE_SCOPE(areg_ipc_private_ServiceClientConnection_failedReceiveMessage);
    TRACE_WARN( "Failed to receive message from socket [ %u ]. Going to stop service", static_cast<uint32_t>(whichSource.getHandle( )) );
    ASSERT( (mClientConnection.isValid( ) == false) || (whichSource.getHandle( ) == mClientConnection.getSocket( ).getHandle( )) );

    if (Application::isServicingReady())
    {
        _cancelConnection( );
        _sendCommand( ServiceEventData::eServiceEventCommands::CMD_ServiceLost, Event::eEventPriority::EventPriorityHigh );
    }
    else
    {
        TRACE_WARN("Ignore receive message failure, the application is closing");
    }
}

void ServiceClientConnection::failedProcessMessage( const RemoteMessage & msgUnprocessed )
{
    TRACE_SCOPE(areg_ipc_private_ServiceClientConnection_failedProcessMessage);
    TRACE_DBG("The message [ 0x%X ] for target [ %u ] and from source [ %u ] is unprocessed, going to create failed event"
                    , static_cast<uint32_t>(msgUnprocessed.getMessageId())
                    , static_cast<uint32_t>(msgUnprocessed.getTarget())
                    , static_cast<uint32_t>(msgUnprocessed.getSource()));

    if (Application::isServicingReady())
    {
        unsigned int msgId = msgUnprocessed.getMessageId();
        if ( NEService::isExecutableId(msgId) )
        {
            msgUnprocessed.moveToBegin();
            StreamableEvent * eventError = RemoteEventFactory::createRequestFailedEvent(msgUnprocessed, mChannel);
            if ( eventError != nullptr )
            {
                RemoteMessage data;
                if ( RemoteEventFactory::createStreamFromEvent( data, *eventError, mChannel) )
                {
                    _sendMessage(data);
                }
            }
        }
        else
        {
            TRACE_WARN("The unprocessed message is neither executable, nor connection notification. Ignoring to generate request failed event.");
        }
    }
    else
    {
        TRACE_WARN("Ignore processing failure message, the application is closing");
    }
}

void ServiceClientConnection::processReceivedMessage( const RemoteMessage & msgReceived, Socket & whichSource )
{
    TRACE_SCOPE(areg_ipc_private_ServiceClientConnection_processReceivedMessage);
    if ( msgReceived.isValid() && whichSource.isValid() )
    {
        NEService::eFuncIdRange msgId = static_cast<NEService::eFuncIdRange>( msgReceived.getMessageId());
        NEMemory::eMessageResult result = static_cast<NEMemory::eMessageResult>(msgReceived.getResult());
        TRACE_DBG("Processing received message [ %s ] with id [ 0x%X ], received message is valid, message result [ %s ]"
                        , NEService::getString( msgId)
                        , static_cast<uint32_t>(msgId)
                        , NEMemory::getString(result));

        switch ( msgId )
        {
        case NEService::eFuncIdRange::ServiceRouterNotify:
            {
                NEService::eServiceConnection connection = NEService::eServiceConnection::ServiceConnectionUnknown;
                ITEM_ID cookie = NEService::COOKIE_UNKNOWN;
                msgReceived >> connection;
                msgReceived >> cookie;
                TRACE_DBG("Router connection notification. Connection status [ %s ], cookie [ %u ]", NEService::getString(connection), static_cast<uint32_t>(cookie));

                switch ( connection )
                {
                case NEService::eServiceConnection::ServiceConnected:
                case NEService::eServiceConnection::ServicePending:
                    {
                        if ( msgReceived.getResult( ) == NEMemory::MESSAGE_SUCCESS )
                        {
                            Lock lock( mLock );
                            ASSERT( cookie == msgReceived.getTarget( ) );
                            mClientConnection.setCookie( cookie );
                            _sendCommand( ServiceEventData::eServiceEventCommands::CMD_ServiceStarted );
                        }
                        else
                        {
                            _cancelConnection( );
                            _sendCommand( ServiceEventData::eServiceEventCommands::CMD_ServiceLost );
                        }
                    }
                    break;

                case NEService::eServiceConnection::ServiceConnectionLost:
                    {
                        _cancelConnection( );
                        _sendCommand( ServiceEventData::eServiceEventCommands::CMD_ServiceLost );
                    }
                    break;

                default:
                    {
                        _cancelConnection( );
                        _sendCommand( ServiceEventData::eServiceEventCommands::CMD_ServiceStopped );
                    }
                    break;
                }
            }
            break;

        case NEService::eFuncIdRange::ServiceRouterNotifyRegister:
            {
                ASSERT( mClientConnection.getCookie() == msgReceived.getTarget() );
                NEService::eServiceRequestType reqType;
                msgReceived >> reqType;
                TRACE_DBG("Remote routing service registration notification of type [ %s ]", NEService::getString(reqType));

                switch ( reqType )
                {
                case NEService::eServiceRequestType::RegisterClient:
                    {
                        ProxyAddress proxy(msgReceived);
                        NEService::eDisconnectReason reason { NEService::eDisconnectReason::ReasonUndefined };
                        msgReceived >> reason;
                        proxy.setSource( mChannel.getSource() );
                        if ( result == NEMemory::eMessageResult::ResultSucceed )
                        {
                            mRegisterConsumer.registeredRemoteServiceConsumer(proxy);
                        }
                        else
                        {
                            mRegisterConsumer.unregisteredRemoteServiceConsumer(proxy, reason, NEService::COOKIE_ANY);
                        }
                    }
                    break;

                case NEService::eServiceRequestType::RegisterStub:
                    {
                        StubAddress stub(msgReceived);
                        stub.setSource( mChannel.getSource() );
                        if ( result == NEMemory::eMessageResult::ResultSucceed )
                        {
                            mRegisterConsumer.registeredRemoteServiceProvider( stub );
                        }
                        else
                        {
                            mRegisterConsumer.unregisteredRemoteServiceProvider( stub, NEService::eDisconnectReason::ReasonUndefined, NEService::COOKIE_ANY );
                        }
                    }
                    break;

                case NEService::eServiceRequestType::UnregisterClient:
                    {
                        ProxyAddress proxy(msgReceived);
                        NEService::eDisconnectReason reason { NEService::eDisconnectReason::ReasonUndefined };
                        msgReceived >> reason;
                        proxy.setSource( mChannel.getSource() );
                        mRegisterConsumer.unregisteredRemoteServiceConsumer(proxy, reason, NEService::COOKIE_ANY);
                    }
                    break;

                case NEService::eServiceRequestType::UnregisterStub:
                    {
                        StubAddress stub(msgReceived);
                        NEService::eDisconnectReason reason{NEService::eDisconnectReason::ReasonUndefined};
                        msgReceived >> reason;
                        stub.setSource( mChannel.getSource() );
                        mRegisterConsumer.unregisteredRemoteServiceProvider(stub, reason, NEService::COOKIE_ANY);
                    }
                    break;

                default:
                    ASSERT(false);
                    break;
                }
            }
            break;

        case NEService::eFuncIdRange::ServiceLastId:            // fall through
        case NEService::eFuncIdRange::ServiceRouterQuery:       // fall through
        case NEService::eFuncIdRange::ServiceRouterRegister:    // fall through
        case NEService::eFuncIdRange::ServiceRouterDisconnect:  // fall through
        case NEService::eFuncIdRange::ServiceRouterConnect:     // fall through
        case NEService::eFuncIdRange::ServiceNotifyConnection:  // fall through
        case NEService::eFuncIdRange::ServiceRequestConnection: // fall through
        case NEService::eFuncIdRange::ServiceNotifyVersion:     // fall through
        case NEService::eFuncIdRange::ServiceRequestVersion:    // fall through
        case NEService::eFuncIdRange::ServiceRequestRegister:   // fall through
        case NEService::eFuncIdRange::ComponentCleanup:
            break;

        case NEService::eFuncIdRange::AttributeLastId:          // fall through
        case NEService::eFuncIdRange::AttributeFirstId:         // fall through
        case NEService::eFuncIdRange::ResponseLastId:           // fall through
        case NEService::eFuncIdRange::ResponseFirstId:          // fall through
        case NEService::eFuncIdRange::RequestLastId:            // fall through
        case NEService::eFuncIdRange::RequestFirstId:           // fall through
        case NEService::eFuncIdRange::EmptyFunctionId:          // fall through
        default:
            {
                if ( NEService::isExecutableId(static_cast<unsigned int>(msgId)) )
                {
                    TRACE_DBG("Processing executable remote message with ID [ 0x%X ]", static_cast<unsigned int>(msgId));
                    StreamableEvent * eventRemote = RemoteEventFactory::createEventFromStream(msgReceived, mChannel);
                    if ( eventRemote != nullptr )
                    {
                        eventRemote->deliverEvent();
                    }
                    else
                    {
                        failedProcessMessage(msgReceived);
                    }
                }
                else
                {
                    TRACE_WARN("The message [ %s ] with id [ 0x%X ] was not processed on client service side", NEService::getString(msgId), static_cast<unsigned int>(msgId));
                }
            }
            break;
        }
    }
    else
    {
        TRACE_WARN("Invalid message from host [ %s : %u ], ignore processing"
                    , whichSource.getAddress().getHostAddress().getString()
                    , whichSource.getAddress().getHostPort( ) );
    }
}

void ServiceClientConnection::processRemoteRequestEvent( RemoteRequestEvent & requestEvent)
{
    TRACE_SCOPE(areg_ipc_private_ServiceClientConnection_processRemoteRequestEvent);
    TRACE_DBG("Processing request event [ %s ] with message id [ 0x%X ] of runtime object [ %s ], target stub [ %s ], source proxy [ %s ], request type [ %s ]"
                , Event::getString( requestEvent.getEventType() )
                , static_cast<uint32_t>(requestEvent.getRequestId())
                , requestEvent.getRuntimeClassName().getString()
                , StubAddress::convAddressToPath(requestEvent.getTargetStub()).getString()
                , ProxyAddress::convAddressToPath(requestEvent.getEventSource()).getString()
                , NEService::getString(requestEvent.getRequestType()) );

    if ( requestEvent.isRemote() )
    {
        RemoteMessage data;
        if ( RemoteEventFactory::createStreamFromEvent( data, requestEvent, mChannel) )
        {
            TRACE_DBG("Send request, remote message of id [ 0x%X ] ( %s ) from source [ %u ] to target [ %u ]"
                            , static_cast<uint32_t>(data.getMessageId())
                            , NEService::getString( static_cast<NEService::eFuncIdRange>(data.getMessageId()))
                            , data.getSource()
                            , data.getTarget());

            _sendMessage(data);
        }
        else
        {
            TRACE_ERR("Failed to create remote request message data with ID [ 0x%X ]", static_cast<uint32_t>(requestEvent.getRequestId()) );
        }
    }
    else
    {
        TRACE_WARN("Request event with ID [ 0x%X ] is not remote, ignoring sending event", static_cast<uint32_t>(requestEvent.getRequestId()));
    }
}

void ServiceClientConnection::processRemoteNotifyRequestEvent( RemoteNotifyRequestEvent & requestNotifyEvent )
{
    TRACE_SCOPE(areg_ipc_private_ServiceClientConnection_processRemoteNotifyRequestEvent);
    TRACE_DBG("Processing notify request event [ %s ] with message id [ 0x%X ] of runtime object [ %s ], target stub [ %s ], source proxy [ %s ], request type [ %s ]"
                , Event::getString( requestNotifyEvent.getEventType() )
                , static_cast<uint32_t>(requestNotifyEvent.getRequestId())
                , requestNotifyEvent.getRuntimeClassName().getString()
                , StubAddress::convAddressToPath(requestNotifyEvent.getTargetStub()).getString()
                , ProxyAddress::convAddressToPath(requestNotifyEvent.getEventSource()).getString()
                , NEService::getString(requestNotifyEvent.getRequestType()) );


    if ( requestNotifyEvent.isRemote() )
    {
        RemoteMessage data;
        if ( RemoteEventFactory::createStreamFromEvent( data, requestNotifyEvent, mChannel) )
        {
            TRACE_DBG("Send notify request remote message of id [ 0x%X ] ( %s ) from source [ %p ] to target [ %p ]"
                            , static_cast<uint32_t>(data.getMessageId())
                            , NEService::getString( static_cast<NEService::eFuncIdRange>(data.getMessageId()))
                            , static_cast<uint32_t>(data.getSource())
                            , static_cast<uint32_t>(data.getTarget()));

            _sendMessage(data);
        }
        else
        {
            TRACE_ERR("Failed to create remote notify request message data with ID [ 0x%X ]", static_cast<uint32_t>(requestNotifyEvent.getRequestId()) );
        }
    }
    else
    {
        TRACE_WARN("Notify request event with id [ 0x%X ] is not remote, ignoring sending event", static_cast<uint32_t>(requestNotifyEvent.getRequestId()));
    }
}


void ServiceClientConnection::processRemoteResponseEvent(RemoteResponseEvent & responseEvent)
{
    TRACE_SCOPE(areg_ipc_private_ServiceClientConnection_processRemoteResponseEvent);
    TRACE_DBG("Processing response event [ %s ] with message ID [ 0x%X ] of runtime object [ %s ], target proxy [ %s ], data type [ %s ]"
                , Event::getString( responseEvent.getEventType() )
                , static_cast<uint32_t>(responseEvent.getResponseId())
                , responseEvent.getRuntimeClassName().getString()
                , ProxyAddress::convAddressToPath(responseEvent.getTargetProxy()).getString()
                , NEService::getString(responseEvent.getDataType()) );

    if ( responseEvent.isRemote() )
    {
        RemoteMessage data;
        if ( RemoteEventFactory::createStreamFromEvent( data, responseEvent, mChannel) )
        {
            TRACE_DBG("Send response remote message of ID [ 0x%X ] ( %s ) from source [ 0x%X ] to target [ 0x%X ]"
                            , static_cast<uint32_t>(data.getMessageId())
                            , NEService::getString( static_cast<NEService::eFuncIdRange>(data.getMessageId()))
                            , static_cast<uint32_t>(data.getSource())
                            , static_cast<uint32_t>(data.getTarget()));

            _sendMessage(data);
        }
        else
        {
            TRACE_ERR("Failed to create remote response message data with ID [ 0x%X ]", static_cast<uint32_t>(responseEvent.getResponseId()) );
        }
    }
    else
    {
        TRACE_WARN("Response event with ID [ 0x%X ] is not remote, ignoring sending event", static_cast<uint32_t>(responseEvent.getResponseId()));
    }
}

bool ServiceClientConnection::runDispatcher(void)
{
    TRACE_SCOPE(areg_ipc_private_ServiceClientConnection_runDispatcher);
    ServiceClientEvent::addListener( static_cast<IEServiceClientEventConsumer &>(mEventConsumer), static_cast<DispatcherThread &>(self( )) );
    _sendCommand(ServiceEventData::eServiceEventCommands::CMD_StartService);

    _setConnectionState(ServiceClientConnection::eConnectionState::DisconnectState);
    bool result = DispatcherThread::runDispatcher();
    _setConnectionState(ServiceClientConnection::eConnectionState::ConnectionStopped);

    ServiceClientEvent::removeListener( static_cast<IEServiceClientEventConsumer &>(mEventConsumer), static_cast<DispatcherThread &>(self( )) );

    return result;
}

bool ServiceClientConnection::postEvent(Event & eventElem)
{
    if ( eventElem.isRemote() )
    {
        eventElem.setEventConsumer( static_cast<IERemoteEventConsumer *>(this) );
    }

    return EventDispatcher::postEvent(eventElem);
}

inline void ServiceClientConnection::_sendCommand( ServiceEventData::eServiceEventCommands cmd
                                                 , Event::eEventPriority eventPrio /*= Event::eEventPriority::EventPriorityNormal*/ )
{
    ServiceClientEvent::sendEvent(ServiceEventData( cmd )
                                , static_cast<IEServiceClientEventConsumer &>(mEventConsumer)
                                , static_cast<DispatcherThread &>(self( ))
                                , eventPrio );
}

inline bool ServiceClientConnection::_sendMessage(const RemoteMessage & data, Event::eEventPriority eventPrio /*= Event::eEventPriority::EventPriorityNormal*/ )
{
    return SendMessageEvent::sendEvent( SendMessageEventData(data)
                                      , static_cast<IESendMessageEventConsumer &>(mThreadSend)
                                      , static_cast<DispatcherThread &>(mThreadSend)
                                      , eventPrio);
}

inline void ServiceClientConnection::_disconnectService( Event::eEventPriority eventPrio )
{
    SendMessageEvent::sendEvent( SendMessageEventData()
                               , static_cast<IESendMessageEventConsumer &>(mThreadSend)
                               , static_cast<DispatcherThread &>(mThreadSend)
                               , eventPrio );
}
