/************************************************************************
 * \file        areg/ipc/private/ClientService.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Service Connection client implementation
 ************************************************************************/
#include "areg/ipc/private/ClientService.hpp"

#include "areg/ipc/ConnectionConfiguration.hpp"
#include "areg/ipc/IERemoteServiceConsumer.hpp"
#include "areg/ipc/NEConnection.hpp"

#include "areg/ipc/RemoteServiceEvent.hpp"
#include "areg/component/private/ServiceManager.hpp"
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

DEF_TRACE_SCOPE(areg_ipc_private_ClientService_startRemotingService);
DEF_TRACE_SCOPE(areg_ipc_private_ClientService_stopRemotingService);
DEF_TRACE_SCOPE(areg_ipc_private_ClientService_processEvent);
DEF_TRACE_SCOPE(areg_ipc_private_ClientService_startConnection);
DEF_TRACE_SCOPE(areg_ipc_private_ClientService_stopConnection);
DEF_TRACE_SCOPE(areg_ipc_private_ClientService_cancelConnection);
DEF_TRACE_SCOPE(areg_ipc_private_ClientService_failedSendMessage);
DEF_TRACE_SCOPE(areg_ipc_private_ClientService_failedReceiveMessage);
DEF_TRACE_SCOPE(areg_ipc_private_ClientService_failedProcessMessage);
DEF_TRACE_SCOPE(areg_ipc_private_ClientService_processReceivedMessage);
DEF_TRACE_SCOPE(areg_ipc_private_ClientService_processRemoteRequestEvent);
DEF_TRACE_SCOPE(areg_ipc_private_ClientService_processRemoteResponseEvent);
DEF_TRACE_SCOPE(areg_ipc_private_ClientService_processRemoteNotifyRequestEvent);
DEF_TRACE_SCOPE(areg_ipc_private_ClientService_runDispatcher);

DEF_TRACE_SCOPE(areg_ipc_private_ClientService_registerService);
DEF_TRACE_SCOPE(areg_ipc_private_ClientService_unregisterService);
DEF_TRACE_SCOPE(areg_ipc_private_ClientService_registerServiceClient);
DEF_TRACE_SCOPE(areg_ipc_private_ClientService_unregisterServiceClient);

const NERemoteService::eServiceConnection   ClientService::CONNECT_TYPE   = NERemoteService::ConnectionTcpip;

ClientService::ClientService( IERemoteServiceConsumer & serviceConsumer )
    : IERemoteService              ( )
    , DispatcherThread           ( NEConnection::CLIENT_DISPATCH_MESSAGE_THREAD )
    , IERemoteServiceHandler       ( )
    , IEClientServiceEventConsumer ( )
    , IERemoteEventConsumer        ( )
    , IETimerConsumer              ( )

    , mClientConnection ( )
    , mServiceConsumer  ( serviceConsumer )
    , mTimerConnect     ( static_cast<IETimerConsumer &>(self()), NEConnection::CLIENT_CONNECT_TIMER_NAME)
    , mThreadReceive    ( static_cast<IERemoteServiceHandler &>(self()), mClientConnection )
    , mThreadSend       ( static_cast<IERemoteServiceHandler &>(self()), mClientConnection )
    , mIsServiceEnabled ( NEConnection::DEFAULT_REMOVE_SERVICE_ENABLED )    // TODO: by default, should be false and read out from configuration file.
    , mConfigFile       ( "" )
    , mChannel          ( )
    , mConnectionState  ( ConnectionStopped )
    , mLock             ( )
{
    ; // do nothing
}

ClientService::~ClientService(void)
{
    ; // do nothing
}

bool ClientService::configureRemoteServicing( const char * configFile )
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
        mIsServiceEnabled       = NEConnection::DEFAULT_REMOVE_SERVICE_ENABLED;
        return mClientConnection.setAddress( NEConnection::DEFAULT_REMOTE_SERVICE_HOST, NEConnection::DEFAULT_REMOTE_SERVICE_PORT );
    }
}

void ClientService::setRemoteServiceAddress( const char * hostName, unsigned short portNr )
{
    Lock lock( mLock );
    mClientConnection.setAddress( hostName, portNr );
}

bool ClientService::startRemoteServicing(void)
{
    TRACE_SCOPE(areg_ipc_private_ClientService_startRemotingService);
    Lock lock( mLock );
    bool result = true;
    if ( mClientConnection.isValid() == false && isRunning() == false )
    {
        if ( isRemoteServicingEnabled() )
        {
            lock.unlock();

            TRACE_DBG("The remote servicing is enabled, going to start remote servicing");
            result = createThread(Thread::WAIT_INFINITE) && waitForDispatcherStart(IESynchObject::WAIT_INFINITE);

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

void ClientService::stopRemoteServicing(void)
{
    TRACE_SCOPE(areg_ipc_private_ClientService_stopRemotingService);
    TRACE_DBG( "Stopping remote servicing client connection, current state is [ %s ]", isRunning() ? "RUNNING" : "NOT RUNNING" );
    if ( isRunning() )
    {
        ClientServiceEvent::sendEvent( ClientServiceEventData(ClientServiceEventData::CMD_StopService), static_cast<IEClientServiceEventConsumer &>(self()), static_cast<DispatcherThread &>(self()) );
        DispatcherThread::triggerExitEvent();
        completionWait(Thread::WAIT_INFINITE);
        destroyThread( Thread::DO_NOT_WAIT );
    }
}

bool ClientService::isRemoteServicingStarted(void) const
{
    Lock lock( mLock );
    return isStarted();
}

bool ClientService::isRemoteServicingConfigured( void ) const
{
    Lock lock(mLock);
    return mClientConnection.getAddress().isValid();
}

bool ClientService::isRemoteServicingEnabled(void) const
{
    Lock lock(mLock);
    return mIsServiceEnabled;
}

void ClientService::enableRemoteServicing( bool enable )
{
    Lock lock( mLock );
    if ( isRunning() )
    {
        if ( enable == false )
            stopRemoteServicing();
    }
    mIsServiceEnabled = enable;
}

bool ClientService::registerService( const StubAddress & stubService )
{
    TRACE_SCOPE(areg_ipc_private_ClientService_registerService);
    Lock lock( mLock );
    bool result = false;
    if ( isStarted() )
    {
        TRACE_DBG("Queueing to send register [ %s ] service message by connection [ %d ]", StubAddress::convAddressToPath(stubService).getString(), mClientConnection.getCookie());
        result = queueSendMessage( NEConnection::createRouterRegisterService(stubService, mClientConnection.getCookie()) );
    }
    return result;
}

void ClientService::unregisterService(const StubAddress & stubService)
{
    TRACE_SCOPE(areg_ipc_private_ClientService_unregisterService);
    
    Lock lock( mLock );
    if ( isStarted() )
    {
        TRACE_DBG("Queueing to send unregister [ %s ] service message by connection [ %d ]", StubAddress::convAddressToPath(stubService).getString(), mClientConnection.getCookie());
        queueSendMessage( NEConnection::createRouterUnregisterService(stubService, mClientConnection.getCookie()) );
    }
}

bool ClientService::registerServiceClient(const ProxyAddress & proxyService)
{
    TRACE_SCOPE(areg_ipc_private_ClientService_registerServiceClient);
    Lock lock( mLock );
    bool result = false;
    if ( isStarted() )
    {
        TRACE_DBG("Queueing to send register [ %s ] service client message by connection [ %d ]", ProxyAddress::convAddressToPath(proxyService).getString(), mClientConnection.getCookie());
        result = queueSendMessage( NEConnection::createRouterRegisterClient(proxyService, mClientConnection.getCookie()) );
    }

    return result;
}

void ClientService::unregisterServiceClient(const ProxyAddress & proxyService)
{
    TRACE_SCOPE(areg_ipc_private_ClientService_unregisterServiceClient);

    Lock lock( mLock );
    if ( isStarted() )
    {
        TRACE_DBG("Queueing to send unregister [ %s ] service client message by connection [ %d ]", ProxyAddress::convAddressToPath(proxyService).getString(), mClientConnection.getCookie());
        queueSendMessage( NEConnection::createRouterUnregisterClient(proxyService, mClientConnection.getCookie()) );
    }
}

void ClientService::processTimer(Timer & timer)
{
    if ( &timer == &mTimerConnect )
        startConnection();
}

void ClientService::processEvent( const ClientServiceEventData & data )
{
    TRACE_SCOPE(areg_ipc_private_ClientService_processEvent);
    
    ClientServiceEventData::eClientServiceCommands cmdService = data.getCommand();
    TRACE_DBG("Client service is executing command [ %s ]", ClientServiceEventData::getString(cmdService));

    switch ( cmdService )
    {
    case ClientServiceEventData::CMD_StartService:
        {
            TRACE_DBG("Starting remove servicing");;
            mChannel.setCookie( NEService::COOKIE_LOCAL );
            mChannel.setSource( NEService::SOURCE_UNKNOWN );
            mChannel.setTarget( NEService::TARGET_UNKNOWN );
            startConnection();
        }
        break;

    case ClientServiceEventData::CMD_StopService:
        {
            TRACE_DBG("Stopping remote servicing");
            setConnectionState(ClientService::ConnectionStopping);
            Channel channel = mChannel;
            mChannel.setCookie( NEService::COOKIE_UNKNOWN );
            mChannel.setSource( NEService::SOURCE_UNKNOWN );
            mChannel.setTarget( NEService::TARGET_UNKNOWN );

            stopConnection();

            mThreadReceive.completionWait( Thread::WAIT_INFINITE );
            mThreadSend.completionWait( Thread::WAIT_INFINITE );

            mClientConnection.closeSocket();
            mServiceConsumer.remoteServiceStopped( channel );

            mThreadReceive.destroyThread( Thread::DO_NOT_WAIT );
            mThreadSend.destroyThread( Thread::DO_NOT_WAIT );
        }
        break;

    case ClientServiceEventData::CMD_ServiceStarted:
        {
            ASSERT(isStarted());
            if ( mClientConnection.getCookie() != NEService::COOKIE_LOCAL )
            {
                TRACE_DBG("Client service succeeded to start, client cookie is [ %p ]", static_cast<id_type>(mClientConnection.getCookie()));
                setConnectionState(ClientService::ConnectionStarted);
                mChannel.setCookie( mClientConnection.getCookie() );
                mChannel.setSource( getCurrentThreadId() );
                mChannel.setTarget( NEService::TARGET_LOCAL );
                mServiceConsumer.remoteServiceStarted(mChannel);
            }
        }
        break;

    case ClientServiceEventData::CMD_ServiceStopped:
        {
            TRACE_DBG("Client service is stopped. Resetting cookie");
            setConnectionState(ClientService::ConnectionStopping);
            Channel channel = mChannel;
            mChannel.setCookie( NEService::COOKIE_UNKNOWN );
            mChannel.setSource( NEService::SOURCE_UNKNOWN );
            mChannel.setTarget( NEService::TARGET_UNKNOWN );
            // stopConnection();
            cancelConnection();

            mThreadReceive.completionWait( Thread::WAIT_INFINITE );
            mThreadSend.completionWait( Thread::WAIT_INFINITE );
            mServiceConsumer.remoteServiceStopped( channel );
        }
        break;

    case ClientServiceEventData::CMD_ServiceLost:
    {
        TRACE_WARN("Client service is lost connection. Resetting cookie and trying to restart, current connection state [ %s ]", ClientService::getString(getConnectionState()));
        Channel channel = mChannel;
        mChannel.setCookie( NEService::COOKIE_UNKNOWN );
        mChannel.setSource( NEService::SOURCE_UNKNOWN );
        mChannel.setTarget( NEService::TARGET_UNKNOWN );
        cancelConnection();

        mThreadReceive.completionWait( Thread::WAIT_INFINITE );
        mThreadSend.completionWait( Thread::WAIT_INFINITE );
        mServiceConsumer.remoteServiceStopped( channel );

        if (Application::isServicingReady() && (getConnectionState() == ClientService::ConnectionStarted))
        {
            TRACE_DBG("Restarting lost connection with remote service");
            setConnectionState(ClientService::ConnectionStarting);
            startConnection();
        }
        else
        {
            TRACE_WARN("Ignoring lost connection event, either servising state is not allowed, or application is closing.");
        }
    }
    break;

    default:
        ASSERT(false);
        break;
    }
}

inline bool ClientService::isStarted(void) const
{
    ITEM_ID cookie = mClientConnection.getCookie();
    return (mClientConnection.isValid() && (cookie != NEService::COOKIE_LOCAL) && (cookie != NEService::COOKIE_UNKNOWN));
}

inline void ClientService::setConnectionState(const ClientService::eConnectionState newState)
{
    mConnectionState    = newState;
}

inline ClientService::eConnectionState ClientService::getConnectionState( void ) const
{
    return mConnectionState;
}

inline bool ClientService::startConnection( void )
{
    TRACE_SCOPE(areg_ipc_private_ClientService_startConnection);

    ASSERT(mClientConnection.getAddress().isValid());
    ASSERT(mClientConnection.isValid() == false);
    ASSERT(mThreadReceive.isRunning() == false);
    ASSERT(mThreadSend.isRunning() == false);

    bool result = false;
    mTimerConnect.stopTimer();

    if ( mClientConnection.createSocket() )
    {
        if ( mThreadReceive.createThread( Thread::WAIT_INFINITE ) && mThreadSend.createThread( Thread::WAIT_INFINITE ) )
        {
            VERIFY( mThreadReceive.waitForDispatcherStart( IESynchObject::WAIT_INFINITE ) );
            VERIFY( mThreadSend.waitForDispatcherStart( IESynchObject::WAIT_INFINITE ) );
            TRACE_DBG("Client service starting connection with remote routing service.");
            result = mClientConnection.requestConnectServer();
        }
    }

    if ( result == false )
    {
        TRACE_WARN("Client service failed to start connection, going to repeat connection in [ %u ] ms", NEConnection::DEFAULT_RETRY_CONNECT_TIMEOUT);
        mThreadSend.destroyThread( Thread::DO_NOT_WAIT );
        mThreadReceive.destroyThread( Thread::DO_NOT_WAIT );
        mClientConnection.closeSocket();
        mTimerConnect.startTimer( NEConnection::DEFAULT_RETRY_CONNECT_TIMEOUT, static_cast<DispatcherThread &>(self()), 1);
    }

    return result;
}

inline void ClientService::stopConnection(void)
{
    TRACE_SCOPE(areg_ipc_private_ClientService_stopConnection);
    TRACE_WARN("Stopping client service connection");
    mTimerConnect.stopTimer();

    mThreadReceive.removeAllEvents();
    mThreadReceive.triggerExitEvent();
    mClientConnection.disableReceive();

    mThreadSend.removeAllEvents();
    RemoteMessage msgDisconnect = mClientConnection.getDisconnectMessage();
    if ( msgDisconnect.isValid() )
    {
        queueSendMessage(msgDisconnect);
    }

    mClientConnection.setCookie( NEService::COOKIE_UNKNOWN );
    mThreadSend.triggerExitEvent();
}

inline void ClientService::cancelConnection(void)
{
    TRACE_SCOPE(areg_ipc_private_ClientService_cancelConnection);
    TRACE_WARN("Canceling client service connection");
    mTimerConnect.stopTimer();

    mClientConnection.closeSocket();
    mClientConnection.setCookie( NEService::COOKIE_UNKNOWN );

    mThreadReceive.triggerExitEvent();
    mThreadSend.triggerExitEvent();

    mThreadReceive.destroyThread( Thread::DO_NOT_WAIT );
    mThreadSend.destroyThread( Thread::DO_NOT_WAIT );
}

void ClientService::failedSendMessage(const RemoteMessage & msgFailed)
{
    TRACE_SCOPE(areg_ipc_private_ClientService_failedSendMessage);
    TRACE_WARN("Failed to send message [ %p ] to target [ %p ], source is [ %p ]"
                    , static_cast<id_type>(msgFailed.getMessageId())
                    , static_cast<id_type>(msgFailed.getTarget())
                    , static_cast<id_type>(msgFailed.getSource()));

    if (Application::isServicingReady())
    {
        unsigned int msgId = msgFailed.getMessageId();
        if ( NEService::isExecutableId(msgId) || NEService::isConnectNotifyId(msgId) )
        {
            TRACE_DBG("Failed message [ %u ] is executable or connection notification", msgId);
            StreamableEvent * eventError = RemoteEventFactory::createRequestFailedEvent(msgFailed, mChannel);
            if ( eventError != NULL )
            {
                eventError->deliverEvent();
            }

            ClientServiceEvent::sendEvent( ClientServiceEventData(ClientServiceEventData::CMD_ServiceLost), static_cast<IEClientServiceEventConsumer &>(self()), static_cast<DispatcherThread &>(self()) );
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

void ClientService::failedReceiveMessage(SOCKETHANDLE whichSource)
{
    TRACE_SCOPE(areg_ipc_private_ClientService_failedReceiveMessage);
    TRACE_WARN("Failed to receive message from socket [ %u ], current client socket is [ %u ]. Going to stop service",  whichSource, mClientConnection.getSocketHandle());
    if (Application::isServicingReady())
    {
        if ( whichSource == mClientConnection.getSocketHandle() )
        {
            ClientServiceEvent::sendEvent( ClientServiceEventData(ClientServiceEventData::CMD_ServiceLost), static_cast<IEClientServiceEventConsumer &>(self()), static_cast<DispatcherThread &>(self()) );
        }
    }
    else
    {
        TRACE_WARN("Ignore receive message failure, the application is closing");
    }
}

void ClientService::failedProcessMessage( const RemoteMessage & msgUnprocessed )
{
    TRACE_SCOPE(areg_ipc_private_ClientService_failedProcessMessage);
    TRACE_DBG("The message [ %p ] for target [ %p ] and from source [ %p ] is unprocessed, going to create failed event"
                    , static_cast<id_type>(msgUnprocessed.getMessageId())
                    , static_cast<id_type>(msgUnprocessed.getTarget())
                    , static_cast<id_type>(msgUnprocessed.getSource()));

    if (Application::isServicingReady())
    {
        unsigned int msgId = msgUnprocessed.getMessageId();
        if ( NEService::isExecutableId(msgId) )
        {
            StreamableEvent * eventError = RemoteEventFactory::createRequestFailedEvent(msgUnprocessed, mChannel);
            if ( eventError != NULL )
            {
                RemoteMessage data;
                if ( RemoteEventFactory::createStreamFromEvent( data, *eventError, mChannel) )
                {
                    queueSendMessage(data);
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
        TRACE_WARN("Ignore processing failure message, te application is closing");
    }
}

void ClientService::processReceivedMessage( const RemoteMessage & msgReceived, const NESocket::InterlockedValue & addrHost, SOCKETHANDLE whichSource )
{
    TRACE_SCOPE(areg_ipc_private_ClientService_processReceivedMessage);
    if ( msgReceived.isValid() && whichSource != NESocket::InvalidSocketHandle )
    {
        NEService::eFuncIdRange msgId = static_cast<NEService::eFuncIdRange>( msgReceived.getMessageId());
        NEMemory::eMessageResult result = static_cast<NEMemory::eMessageResult>(msgReceived.getResult());
        TRACE_DBG("Processing received message [ %s ] ( ID = %p ), received message is valid, message result [ %s ]"
                        , NEService::getString( msgId)
                        , static_cast<id_type>(msgId)
                        , NEMemory::getString(result));

        switch ( msgId )
        {
        case NEService::SI_ROUTER_NOTIFY:
            {
                NEService::eServiceConnection connection = NEService::ServiceConnectionUnknown;
                ITEM_ID cookie = NEService::COOKIE_UNKNOWN;
                msgReceived >> connection;
                msgReceived >> cookie;
                TRACE_DBG("Router connection notification. Connection status [ %s ], cookie [ %p ]", NEService::getString(connection), static_cast<id_type>(cookie));

                if ( (msgReceived.getResult() == static_cast<unsigned int>(NEMemory::ResultSucceed)) && (connection == NEService::ServiceConnected))
                {
                    Lock lock(mLock);
                    ASSERT(cookie == msgReceived.getTarget());
                    mClientConnection.setCookie(cookie);
                    ClientServiceEvent::sendEvent( ClientServiceEventData(ClientServiceEventData::CMD_ServiceStarted), static_cast<IEClientServiceEventConsumer &>(self()), static_cast<DispatcherThread &>(self()) );
                }
                else if ( (static_cast<unsigned int>(connection) & static_cast<unsigned int>(NEService::ServiceConnected)) == 0 )
                {
                    ClientServiceEvent::sendEvent( ClientServiceEventData(ClientServiceEventData::CMD_ServiceStopped), static_cast<IEClientServiceEventConsumer &>(self()), static_cast<DispatcherThread &>(self()) );
                }
                else
                {
                    ; // ignore
                }
            }
            break;

        case NEService::SI_ROUTER_REGISTER_NOTIFY:
            {
                ASSERT( mClientConnection.getCookie() == msgReceived.getTarget() );
                NEService::eServiceRequestType reqType;
                msgReceived >> reqType;
                TRACE_DBG("Remote routing service registration notification of type [ %s ]", NEService::getString(reqType));

                switch ( reqType )
                {
                case NEService::SERVICE_REQUEST_REGISTER_CLIENT:
                    {
                        ProxyAddress proxy(msgReceived);
                        proxy.setSource( mChannel.getSource() );
                        if ( result == NEMemory::ResultSucceed )
                            mServiceConsumer.registerRemoteProxy(proxy);
                        else
                            mServiceConsumer.unregisterRemoteProxy(proxy, NEService::COOKIE_ANY);
                    }
                    break;

                case NEService::SERVICE_REQUEST_REGISTER_STUB:
                    {
                        StubAddress stub(msgReceived);
                        stub.setSource( mChannel.getSource() );
                        if ( result == NEMemory::ResultSucceed )
                            mServiceConsumer.registerRemoteStub(stub);
                        else
                            mServiceConsumer.unregisterRemoteStub(stub, NEService::COOKIE_ANY);
                    }
                    break;

                case NEService::SERVICE_REQUEST_UNREGISTER_CLIENT:
                    {
                        ProxyAddress proxy(msgReceived);
                        proxy.setSource( mChannel.getSource() );
                        mServiceConsumer.unregisterRemoteProxy(proxy, NEService::COOKIE_ANY);
                    }
                    break;

                case NEService::SERVICE_REQUEST_UNREGISTER_STUB:
                    {
                        StubAddress stub(msgReceived);
                        stub.setSource( mChannel.getSource() );
                        mServiceConsumer.unregisterRemoteStub(stub, NEService::COOKIE_ANY);
                    }
                    break;

                default:
                    ASSERT(false);
                    break;
                }
            }
            break;

        case NEService::SI_REGISTER_ID_LAST:            // fall through
        case NEService::SI_ROUTER_QUERY:                // fall through
        case NEService::SI_ROUTER_REGISTER:             // fall through
        case NEService::SI_ROUTER_DISCONNECT:           // fall through
        case NEService::SI_ROUTER_CONNECT:              // fall through
        case NEService::SI_SERVICE_CONNECTION_NOTIFY:   // fall through
        case NEService::SI_SERVICE_CONNECTION_REQUEST:  // fall through
        case NEService::SI_NOTIFY_VERSION:              // fall through
        case NEService::SI_REQUEST_VERSION:             // fall through
        case NEService::SI_REGISTER_ID_FIRST:           // fall through
        case NEService::COMPONENT_MAKE_CLEANUP:         // fall through
            break;

        case NEService::ATTRIBUTE_ID_LAST:              // fall through
        case NEService::ATTRIBUTE_ID_FIRST:             // fall through
        case NEService::RESPONSE_ID_LAST:               // fall through
        case NEService::RESPONSE_ID_FIRST:              // fall through
        case NEService::REQUEST_ID_LAST:                // fall through
        case NEService::REQUEST_ID_FIRST:               // fall through
        case NEService::EMPTY_FUNCTION_ID:              // fall through
        default:
            {
                if ( NEService::isExecutableId(msgId) )
                {
                    TRACE_DBG("Processing executable remote message with ID [ %p ]", static_cast<id_type>(msgId));
                    StreamableEvent * eventRemote = RemoteEventFactory::createEventFromStream(msgReceived, mChannel);
                    if ( eventRemote != NULL )
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
                    TRACE_WARN("The message [ %s ] ( ID = %p )was not processed on client service side", NEService::getString(msgId), static_cast<id_type>(msgId));
                }
            }
            break;
        }
    }
    else
    {
        TRACE_WARN("Invalid message from host [ %s : %u ], ignore processing", addrHost.getHostAddress().getString(), addrHost.getHostPort());
    }
}

void ClientService::processRemoteRequestEvent( RemoteRequestEvent & requestEvent)
{
    TRACE_SCOPE(areg_ipc_private_ClientService_processRemoteRequestEvent);
    TRACE_DBG("Processing request event [ %s ] with message ID [ %p ] of runtime object [ %s ], target stub [ %s ], source proxy [ %s ], request type [ %s ]"
                , Event::getString( requestEvent.getEventType() )
                , static_cast<id_type>(requestEvent.getRequestId())
                , requestEvent.getRuntimeClassName()
                , StubAddress::convAddressToPath(requestEvent.getTargetStub()).getString()
                , ProxyAddress::convAddressToPath(requestEvent.getEventSource()).getString()
                , NEService::getString(requestEvent.getRequestType()) );

    if ( requestEvent.isRemote() )
    {
        RemoteMessage data;
        if ( RemoteEventFactory::createStreamFromEvent( data, requestEvent, mChannel) )
        {
            TRACE_DBG("Send request, remote message of ID [ %p ] ( %s ) from source [ %u ] to target [ %u ]"
                            , static_cast<id_type>(data.getMessageId())
                            , NEService::getString( static_cast<NEService::eFuncIdRange>(data.getMessageId()))
                            , data.getSource()
                            , data.getTarget());

            queueSendMessage(data);
        }
        else
        {
            TRACE_ERR("Failed to create remote request message data with ID [ %p ]", static_cast<id_type>(requestEvent.getRequestId()) );
        }
    }
    else
    {
        TRACE_WARN("Request event with ID [ %p ] is not remote, ignoring sending event", static_cast<id_type>(requestEvent.getRequestId()));
    }
}

void ClientService::processRemoteNotifyRequestEvent( RemoteNotifyRequestEvent & requestNotifyEvent )
{
    TRACE_SCOPE(areg_ipc_private_ClientService_processRemoteNotifyRequestEvent);
    TRACE_DBG("Processing notify request event [ %s ] with message ID [ %p ] of runtime object [ %s ], target stub [ %s ], source proxy [ %s ], request type [ %s ]"
                , Event::getString( requestNotifyEvent.getEventType() )
                , static_cast<id_type>(requestNotifyEvent.getRequestId())
                , requestNotifyEvent.getRuntimeClassName()
                , StubAddress::convAddressToPath(requestNotifyEvent.getTargetStub()).getString()
                , ProxyAddress::convAddressToPath(requestNotifyEvent.getEventSource()).getString()
                , NEService::getString(requestNotifyEvent.getRequestType()) );


    if ( requestNotifyEvent.isRemote() )
    {
        RemoteMessage data;
        if ( RemoteEventFactory::createStreamFromEvent( data, requestNotifyEvent, mChannel) )
        {
            TRACE_DBG("Send notify request remote message of ID [ %p ] ( %s ) from source [ %p ] to target [ %p ]"
                            , static_cast<id_type>(data.getMessageId())
                            , NEService::getString( static_cast<NEService::eFuncIdRange>(data.getMessageId()))
                            , static_cast<id_type>(data.getSource())
                            , static_cast<id_type>(data.getTarget()));

            queueSendMessage(data);
        }
        else
        {
            TRACE_ERR("Failed to create remote notify request message data with ID [ %p ]", static_cast<id_type>(requestNotifyEvent.getRequestId()) );
        }
    }
    else
    {
        TRACE_WARN("Notify request event with ID [ %p ] is not remote, ignoring sending event", static_cast<id_type>(requestNotifyEvent.getRequestId()));
    }
}


void ClientService::processRemoteResponseEvent(RemoteResponseEvent & responseEvent)
{
    TRACE_SCOPE(areg_ipc_private_ClientService_processRemoteResponseEvent);
    TRACE_DBG("Processing response event [ %s ] with message ID [ %p ] of runtime object [ %s ], target proxy [ %s ], data type [ %s ]"
                , Event::getString( responseEvent.getEventType() )
                , static_cast<id_type>(responseEvent.getResponseId())
                , responseEvent.getRuntimeClassName()
                , ProxyAddress::convAddressToPath(responseEvent.getTargetProxy()).getString()
                , NEService::getString(responseEvent.getDataType()) );

    if ( responseEvent.isRemote() )
    {
        RemoteMessage data;
        if ( RemoteEventFactory::createStreamFromEvent( data, responseEvent, mChannel) )
        {
            TRACE_DBG("Send response remote message of ID [ %p ] ( %s ) from source [ %p ] to target [ %p ]"
                            , static_cast<id_type>(data.getMessageId())
                            , NEService::getString( static_cast<NEService::eFuncIdRange>(data.getMessageId()))
                            , static_cast<id_type>(data.getSource())
                            , static_cast<id_type>(data.getTarget()));

            queueSendMessage(data);
        }
        else
        {
            TRACE_ERR("Failed to create remote response message data with ID [ %p ]", static_cast<id_type>(responseEvent.getResponseId()) );
        }
    }
    else
    {
        TRACE_WARN("Response event with ID [ %p ] is not remote, ignoring sending event", static_cast<id_type>(responseEvent.getResponseId()));
    }
}

bool ClientService::runDispatcher(void)
{
    TRACE_SCOPE(areg_ipc_private_ClientService_runDispatcher);
    ClientServiceEvent::addListener( static_cast<IEClientServiceEventConsumer &>(self()), static_cast<DispatcherThread &>(self()) );
    ClientServiceEvent::sendEvent( ClientServiceEventData(ClientServiceEventData::CMD_StartService), static_cast<IEClientServiceEventConsumer &>(self()), static_cast<DispatcherThread &>(self()) );

    setConnectionState(ClientService::ConnectionStarting);
    bool result = DispatcherThread::runDispatcher();
    setConnectionState(ClientService::ConnectionStopped);

    ClientServiceEvent::removeListener( static_cast<IEClientServiceEventConsumer &>(self()), static_cast<DispatcherThread &>(self()) );

    return result;
}

bool ClientService::postEvent(Event & eventElem)
{
    if ( eventElem.isRemote() )
        eventElem.setEventConsumer( static_cast<IERemoteEventConsumer *>(this) );
    return EventDispatcher::postEvent(eventElem);
}

inline bool ClientService::queueSendMessage(const RemoteMessage & data)
{
    return SendMessageEvent::sendEvent( SendMessageEventData(data), static_cast<IESendMessageEventConsumer &>(mThreadSend), static_cast<DispatcherThread &>(mThreadSend) );
}
