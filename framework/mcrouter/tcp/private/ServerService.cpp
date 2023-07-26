/************************************************************************
 * \file        mcrouter/tcp/private/ServerService.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Service Connection server declaration
 ************************************************************************/
#include "mcrouter/tcp/ServerService.hpp"

#include "areg/ipc/NEConnection.hpp"
#include "areg/ipc/ConnectionConfiguration.hpp"
#include "areg/trace/GETrace.h"

DEF_TRACE_SCOPE(mcrouter_tcp_private_ServerService_connectServiceHost);
DEF_TRACE_SCOPE(mcrouter_tcp_private_ServerService_reconnectServiceHost);
DEF_TRACE_SCOPE(mcrouter_tcp_private_ServerService_disconnectServiceHost);
DEF_TRACE_SCOPE(mcrouter_tcp_private_ServerService_registerServiceProvider);
DEF_TRACE_SCOPE(mcrouter_tcp_private_ServerService_unregisterServiceProvider);
DEF_TRACE_SCOPE(mcrouter_tcp_private_ServerService_registerServiceConsumer);
DEF_TRACE_SCOPE(mcrouter_tcp_private_ServerService_unregisterServiceConsumer);
DEF_TRACE_SCOPE(mcrouter_tcp_private_ServerService_connectionLost);
DEF_TRACE_SCOPE(mcrouter_tcp_private_ServerService_registeredRemoteServiceProvider);
DEF_TRACE_SCOPE(mcrouter_tcp_private_ServerService_registeredRemoteServiceConsumer);
DEF_TRACE_SCOPE(mcrouter_tcp_private_ServerService_unregisteredRemoteServiceProvider);
DEF_TRACE_SCOPE(mcrouter_tcp_private_ServerService_unregisteredRemoteServiceConsumer);
DEF_TRACE_SCOPE(mcrouter_tcp_private_ServerService_processReceivedMessage);

DEF_TRACE_SCOPE(mcrouter_tcp_private_ServerService_onServiceStart);
DEF_TRACE_SCOPE(mcrouter_tcp_private_ServerService_onServiceStop);
DEF_TRACE_SCOPE(mcrouter_tcp_private_ServerService_onServiceRestart);
DEF_TRACE_SCOPE(mcrouter_tcp_private_ServerService_onServiceMessageReceived);
DEF_TRACE_SCOPE(mcrouter_tcp_private_ServerService_onServiceMessageSend);

DEF_TRACE_SCOPE(mcrouter_tcp_private_ServerService_startConnection);
DEF_TRACE_SCOPE(mcrouter_tcp_private_ServerService_restartConnection);
DEF_TRACE_SCOPE(mcrouter_tcp_private_ServerService_stopConnection);

DEF_TRACE_SCOPE(mcrouter_tcp_private_ServerService_failedSendMessage);
DEF_TRACE_SCOPE(mcrouter_tcp_private_ServerService_failedReceiveMessage);

//////////////////////////////////////////////////////////////////////////
// ServerService::ServerServiceEventConsumer class implementation
//////////////////////////////////////////////////////////////////////////

ServerService::ServerServiceEventConsumer::ServerServiceEventConsumer( ServerService & service )
    : IEServerServiceEventConsumer  ( )
    , mService                      ( service )
{
}

void ServerService::ServerServiceEventConsumer::processEvent( const ServerServiceEventData & data )
{
    switch ( data.getCommand() )
    {
    case ServerServiceEventData::eServerServiceCommands::CMD_StartService:
        mService.onServiceStart( );
        break;

    case ServerServiceEventData::eServerServiceCommands::CMD_StopService:
        mService.onServiceStop( );
        break;

    case ServerServiceEventData::eServerServiceCommands::CMD_RestartService:
        mService.onServiceRestart( );
        break;

    case ServerServiceEventData::eServerServiceCommands::CMD_ServiceReceivedMsg:
        mService.onServiceMessageReceived( data.getMessage() );
        break;

    case ServerServiceEventData::eServerServiceCommands::CMD_ServiceSendMsg:
        mService.onServiceMessageSend( data.getMessage() );
        break;

    case ServerServiceEventData::eServerServiceCommands::CMD_ExitService:
        mService.onServiceStop( );
        mService.triggerExitEvent( );
        break;

    default:
        ASSERT(false);
        break;
    }
}

//////////////////////////////////////////////////////////////////////////
// ServerService::TimerConsumer class implementation
//////////////////////////////////////////////////////////////////////////

ServerService::TimerConsumer::TimerConsumer( ServerService & service )
    : IETimerConsumer   ( )
    , mService          ( service )
{
}

void ServerService::TimerConsumer::processTimer( Timer & timer )
{
    if (&timer == &mService.mTimerConnect)
    {
        mService.onTimerExpired();
    }
}

//////////////////////////////////////////////////////////////////////////
// ServerService class implementation
//////////////////////////////////////////////////////////////////////////

ServerService::ServerService( void )
    : IERemoteServiceConnection               ( )
    , DispatcherThread              ( NEConnection::SERVER_DISPATCH_MESSAGE_THREAD.data( ) )
    , IEServerConnectionHandler     ( )
    , IERemoteServiceConsumer       ( )
    , IERemoteServiceMessageHandler        ( )

    , mServerConnection ( )
    , mTimerConnect     ( static_cast<IETimerConsumer &>(mTimerConsumer), NEConnection::SERVER_CONNECT_TIMER_NAME.data( ) )
    , mThreadSend       ( static_cast<IERemoteServiceMessageHandler &>(self()), mServerConnection )
    , mThreadReceive    ( static_cast<IEServerConnectionHandler &>(self()), static_cast<IERemoteServiceMessageHandler &>(self()), mServerConnection )
    , mServiceRegistry  ( )
    , mIsServiceEnabled ( true )    // TODO: by default it should be disabled and enabled via init file
    , mConfigFile       ( )
    , mWhiteList        ( )
    , mBlackList        ( )
    , mEventConsumer    ( self() )
    , mTimerConsumer    ( self() )
    , mEventSendStop    ( false, false )
    , mLock             ( )
{
}

bool ServerService::setupServiceConnectionHost(const String &configFile)
{
    ConnectionConfiguration configConnect;
     if ( configConnect.loadConfiguration(configFile) )
    {
        mConfigFile             = configConnect.getConfigFileName();
        mIsServiceEnabled       = configConnect.getConnectionEnableFlag(CONNECT_TYPE);
        String hostName         = configConnect.getConnectionHost(CONNECT_TYPE);
        unsigned short hostPort = configConnect.getConnectionPort(CONNECT_TYPE);

        return mServerConnection.setAddress( hostName, hostPort );
    }
    else
    {
        mIsServiceEnabled       = NEConnection::DEFAULT_REMOVE_SERVICE_ENABLED;
        return mServerConnection.setAddress( NEConnection::DEFAULT_REMOTE_SERVICE_HOST.data( ), NEConnection::DEFAULT_REMOTE_SERVICE_PORT );
    }
}

void ServerService::applyServiceConnectionData(const String & hostName, unsigned short portNr)
{
    mServerConnection.setAddress( hostName, portNr );
}

bool ServerService::connectServiceHost(void)
{
    TRACE_SCOPE(mcrouter_tcp_private_ServerService_connectServiceHost);

    Lock lock(mLock);

    bool result = true;
    if ( mServerConnection.isValid() == false && isRunning() == false )
    {
        if ( createThread( NECommon::WAIT_INFINITE ) && waitForDispatcherStart(NECommon::WAIT_INFINITE) )
        {
            sendCommand( ServerServiceEventData::eServerServiceCommands::CMD_StartService );
        }

        TRACE_DBG( "Created remote servicing thread with [ %s ]", result ? "SUCCESS" : "FAIL" );
    }
#ifdef DEBUG
    else
    {
        ASSERT(isRunning());
    }
#endif // DEBUG

    return result;
}

bool ServerService::reconnectServiceHost(void)
{
    TRACE_SCOPE(mcrouter_tcp_private_ServerService_reconnectServiceHost);

    Lock lock(mLock);
    bool result = true;
    if (isRunning() == false)
    {
        if (createThread(NECommon::WAIT_INFINITE) && waitForDispatcherStart(NECommon::WAIT_INFINITE))
        {
            result = sendCommand( ServerServiceEventData::eServerServiceCommands::CMD_RestartService );
        }

        TRACE_DBG("Created remote servicing thread with [ %s ]", result ? "SUCCESS" : "FAIL");
    }
    else
    {
        TRACE_WARN("The servicing thread is running, restarting servicing.");
        result = sendCommand( ServerServiceEventData::eServerServiceCommands::CMD_RestartService );
    }

    return result;
}

void ServerService::disconnectServiceHost(void)
{
    TRACE_SCOPE(mcrouter_tcp_private_ServerService_disconnectServiceHost);
    if ( isRunning() )
    {
        sendCommand( ServerServiceEventData::eServerServiceCommands::CMD_ExitService, Event::eEventPriority::EventPriorityHigh );
        mEventSendStop.lock();
    }
}

bool ServerService::isServiceHostConnected(void) const
{
    Lock lock( mLock );
    return (mServerConnection.isValid() && isRunning());
}

bool ServerService::isServiceHostSetup(void) const
{
    Lock lock(mLock);
    return mServerConnection.getAddress().isValid();
}

bool ServerService::isRemoteServicingEnabled(void) const
{
    Lock lock(mLock);
    return mIsServiceEnabled;
}

void ServerService::enableRemoteServicing( bool enable )
{
    Lock lock( mLock );
    if ( isRunning( ) && (enable == false) )
    {
        disconnectServiceHost();
    }

    mIsServiceEnabled = enable;
}

bool ServerService::registerServiceProvider(const StubAddress & /* stubService */)
{
    TRACE_SCOPE(mcrouter_tcp_private_ServerService_registerServiceProvider);
    TRACE_ERR("Method is not implemented, this should not be called");
    return false;
}

void ServerService::unregisterServiceProvider(const StubAddress & /* stubService */, const NEService::eDisconnectReason /*reason*/ )
{
    TRACE_SCOPE(mcrouter_tcp_private_ServerService_unregisterServiceProvider);
    TRACE_ERR("Method is not implemented, this should not be called");
}

bool ServerService::registerServiceConsumer(const ProxyAddress & /* proxyService */)
{
    TRACE_SCOPE(mcrouter_tcp_private_ServerService_registerServiceConsumer);
    TRACE_ERR("Method is not implemented, this should not be called");
    return false;
}

void ServerService::unregisterServiceConsumer(const ProxyAddress & /* proxyService */, const NEService::eDisconnectReason /*reason*/ )
{
    TRACE_SCOPE(mcrouter_tcp_private_ServerService_unregisterServiceConsumer);
    TRACE_ERR("Method is not implemented, this should not be called");
}

bool ServerService::canAcceptConnection(const SocketAccepted & clientSocket)
{
    return clientSocket.isValid() ? canAcceptConnection( clientSocket.getAddress() ) : false;
}

void ServerService::connectionLost( SocketAccepted & clientSocket )
{
    TRACE_SCOPE(mcrouter_tcp_private_ServerService_connectionLost);
    ITEM_ID cookie = mServerConnection.getCookie(clientSocket);

    TRACE_WARN("Client lost connection: cookie [ %u ], socket [ %d ], host [ %s : %d ], closing connection"
                , static_cast<uint32_t>(cookie)
                , clientSocket.getHandle()
                , clientSocket.getAddress().getHostAddress().getString()
                , clientSocket.getAddress().getHostPort());

    mServerConnection.closeConnection(clientSocket);
    if ( cookie != NEService::COOKIE_UNKNOWN )
    {
        RemoteMessage msgDisconnect = NEConnection::createDisconnectRequest(cookie);
        sendCommunicationMessage(ServerServiceEventData::eServerServiceCommands::CMD_ServiceReceivedMsg, msgDisconnect, Event::eEventPriority::EventPriorityHigh);
    }
}

void ServerService::connectionFailure(void)
{
    if ( isServiceHostConnected())
    {
        reconnectServiceHost();
    }
}

void ServerService::onTimerExpired( void )
{
    Lock lock( mLock );
    startConnection();
}

void ServerService::onServiceStart(void)
{
    TRACE_SCOPE(mcrouter_tcp_private_ServerService_onServiceStart);
    Lock lock( mLock );
    mEventSendStop.resetEvent();
    startConnection();
}

void ServerService::onServiceStop(void)
{
    TRACE_SCOPE(mcrouter_tcp_private_ServerService_onServiceStop);

    do
    {
        Lock lock(mLock);
        stopConnection();
    } while (false);

    mThreadReceive.completionWait( NECommon::DO_NOT_WAIT);
    mThreadSend.completionWait( NECommon::DO_NOT_WAIT);
    mThreadReceive.shutdownThread();
    mThreadSend.shutdownThread();

    mEventSendStop.setEvent();
}

void ServerService::onServiceRestart( void )
{
    TRACE_SCOPE(mcrouter_tcp_private_ServerService_onServiceRestart);
    Lock lock(mLock);
    restartConnection();
}

void ServerService::onServiceMessageReceived(const RemoteMessage &msgReceived)
{
    TRACE_SCOPE(mcrouter_tcp_private_ServerService_onServiceMessageReceived);

    ASSERT( msgReceived.isValid() );
    NEService::eFuncIdRange msgId = static_cast<NEService::eFuncIdRange>( msgReceived.getMessageId() );
    ITEM_ID source= static_cast<ITEM_ID>(msgReceived.getSource());

    TRACE_DBG("Processing received valid message [ %s ] of id [ 0x%X ] from source [ %u ] to target [ %u ]"
                    , NEService::getString(msgId)
                    , static_cast<uint32_t>(msgId)
                    , static_cast<uint32_t>(source)
                    , static_cast<uint32_t>(msgReceived.getTarget()));

    switch( msgId )
    {
    case NEService::eFuncIdRange::ServiceRouterRegister:
        {
            NEService::eServiceRequestType reqType;
            msgReceived >> reqType;
            TRACE_DBG("Routing service received registration request message [ %s ]", NEService::getString(reqType));
            switch ( reqType )
            {
            case NEService::eServiceRequestType::RegisterStub:
                {
                    StubAddress stubService(msgReceived);
                    stubService.setSource(source);
                    registeredRemoteServiceProvider(stubService);
                }
                break;

            case NEService::eServiceRequestType::RegisterClient:
                {
                    ProxyAddress proxyService(msgReceived);
                    proxyService.setSource(source);
                    registeredRemoteServiceConsumer(proxyService);
                }
                break;

            case NEService::eServiceRequestType::UnregisterStub:
                {
                    StubAddress stubService(msgReceived);
                    NEService::eDisconnectReason reason{NEService::eDisconnectReason::ReasonUndefined};
                    msgReceived >> reason;
                    stubService.setSource(source);
                    unregisteredRemoteServiceProvider(stubService, reason, stubService.getCookie());
                }
                break;

            case NEService::eServiceRequestType::UnregisterClient:
                {
                    ProxyAddress proxyService(msgReceived);
                    NEService::eDisconnectReason reason { NEService::eDisconnectReason::ReasonUndefined };
                    msgReceived >> reason;
                    proxyService.setSource(source);
                    unregisteredRemoteServiceConsumer(proxyService, reason, proxyService.getCookie());
                }
                break;

            default:
                ASSERT(false);
                break;  // do nothing
            }
        }
        break;

    case NEService::eFuncIdRange::ServiceRouterDisconnect:
        {
            ITEM_ID cookie = NEService::COOKIE_UNKNOWN;
            msgReceived >> cookie;
            mServerConnection.closeConnection(cookie);

            TEArrayList<StubAddress>  listStubs;
            TEArrayList<ProxyAddress> listProxies;
            mServiceRegistry.getServiceSources(cookie, listStubs, listProxies);

            TRACE_DBG("Routing service received disconnect message from cookie [ %u ], [ %d ] stubs and [ %d ] proxies are going to be disconnected"
                        , static_cast<unsigned int>(cookie)
                        , listStubs.getSize()
                        , listProxies.getSize());

            for (uint32_t i = 0; i < listProxies.getSize(); ++i)
            {
                unregisteredRemoteServiceConsumer(listProxies[i], NEService::eDisconnectReason::ReasonConsumerDisconnected, cookie);
            }

            for (uint32_t i = 0; i < listStubs.getSize(); ++i)
            {
                unregisteredRemoteServiceProvider(listStubs[i], NEService::eDisconnectReason::ReasonProviderDisconnected, cookie);
            }
        }
        break;

    case NEService::eFuncIdRange::ServiceLastId:
    case NEService::eFuncIdRange::ServiceRouterQuery:
    case NEService::eFuncIdRange::ServiceRouterConnect:
    case NEService::eFuncIdRange::ServiceRequestConnection:
    case NEService::eFuncIdRange::ServiceNotifyVersion:
    case NEService::eFuncIdRange::ServiceRequestVersion:
    case NEService::eFuncIdRange::ServiceRequestRegister:
    case NEService::eFuncIdRange::ComponentCleanup:
    case NEService::eFuncIdRange::ServiceRouterNotifyRegister:
    case NEService::eFuncIdRange::ServiceRouterNotify:
        break;

    case NEService::eFuncIdRange::ServiceNotifyConnection:
    case NEService::eFuncIdRange::AttributeLastId:
    case NEService::eFuncIdRange::AttributeFirstId:
    case NEService::eFuncIdRange::ResponseLastId:
    case NEService::eFuncIdRange::ResponseFirstId:
    case NEService::eFuncIdRange::RequestLastId:
    case NEService::eFuncIdRange::RequestFirstId:
    case NEService::eFuncIdRange::EmptyFunctionId:
    default:
        if ( NEService::isExecutableId(static_cast<uint32_t>(msgId)) || NEService::isConnectNotifyId( static_cast<uint32_t>(msgId)) )
        {
            TRACE_DBG("Message [ %u ] is executable, going to forward to target [ %u ], received from source [ %u ]"
                       , static_cast<uint32_t>(msgId)
                       , msgReceived.getTarget()
                       , source);

            if ( msgReceived.getTarget() != NEService::TARGET_UNKNOWN )
            {
                _sendMessage( msgReceived );
            }
        }
        else
        {
            TRACE_ERR("Message [ %u ] is not executable, ignoring to forward to target [ %u ] from source [ %u ]"
                            , static_cast<uint32_t>(msgId)
                            , static_cast<uint32_t>(msgReceived.getTarget())
                            , static_cast<uint32_t>(source));
            ASSERT(false);
        }
        break;
    }
}

void ServerService::onServiceMessageSend(const RemoteMessage &msgSend)
{
    TRACE_SCOPE(mcrouter_tcp_private_ServerService_onServiceMessageSend);

    NEService::eFuncIdRange msgId = static_cast<NEService::eFuncIdRange>( msgSend.getMessageId() );
    TRACE_DBG("Sending message [ %s ] of id [ 0x%X ] is going to send to target [ %u ] from source [ %u ]"
                    , NEService::getString(msgId)
                    , static_cast<uint32_t>(msgId)
                    , static_cast<uint32_t>(msgSend.getTarget())
                    , static_cast<uint32_t>(msgSend.getSource()));

    if ( NEService::isExecutableId( static_cast<uint32_t>(msgId)) )
    {
        if ( msgSend.getTarget( ) != NEService::TARGET_UNKNOWN )
        {
            _sendMessage( msgSend );
        }
    }
    else
    {
        TRACE_ERR("The message [ %u ] is neither executable, nor router notification. Ignoring sending message", static_cast<uint32_t>(msgId));
        ASSERT(false);
    }
}

bool ServerService::canAcceptConnection(const NESocket::SocketAddress & addrConnect) const
{
    Lock lock(mLock);
    return (isAddressInBlackList( addrConnect ) == false);
}

bool ServerService::startConnection(void)
{
    TRACE_SCOPE(mcrouter_tcp_private_ServerService_startConnection);
    TRACE_DBG("Going to start connection. Address [ %u ], port [ %d ]"
                , mServerConnection.getAddress().getHostAddress().getString()
                , mServerConnection.getAddress().getHostPort());


    ASSERT(mServerConnection.getAddress().isValid());
    ASSERT(mServerConnection.isValid() == false);
    ASSERT(mThreadReceive.isRunning() == false);
    ASSERT(mThreadSend.isRunning() == false);

    bool result = false;
    mTimerConnect.stopTimer();

    if ( mServerConnection.createSocket() )
    {
        TRACE_DBG("Created socket [ %d ], going to create send-receive threads", static_cast<uint32_t>(mServerConnection.getSocketHandle()));

        if ( mThreadReceive.createThread( NECommon::WAIT_INFINITE ) && mThreadSend.createThread( NECommon::WAIT_INFINITE ) )
        {
            VERIFY( mThreadReceive.waitForDispatcherStart( NECommon::WAIT_INFINITE ) );
            VERIFY( mThreadSend.waitForDispatcherStart( NECommon::WAIT_INFINITE ) );
            result = true;

            TRACE_DBG("The threads are created. Ready to send-receive messages.");
        }
        else
        {
            TRACE_ERR("Failed to create send-receive threads, cannot communicate. Stop remote service");
            mServerConnection.closeSocket();
        }
    }
    else
    {
        TRACE_ERR("Failed to create remote servicing socket.");
    }

    if ( result == false )
    {
        TRACE_WARN("Remote servicing failed, trigger timer with [ %u ] ms timeout to re-establish remote servicing", NEConnection::DEFAULT_RETRY_CONNECT_TIMEOUT);
        mTimerConnect.startTimer( NEConnection::DEFAULT_RETRY_CONNECT_TIMEOUT, static_cast<DispatcherThread &>(self()), 1);
    }

    return result;
}

bool ServerService::restartConnection( void )
{
    TRACE_SCOPE(mcrouter_tcp_private_ServerService_restartConnection);
    TRACE_DBG("Going to start connection. Address [ %u ], port [ %d ]"
                , mServerConnection.getAddress().getHostAddress().getString()
                , mServerConnection.getAddress().getHostPort());

    stopConnection();
    mThreadReceive.completionWait(NECommon::WAIT_INFINITE);
    mThreadSend.completionWait(NECommon::WAIT_INFINITE);
    mThreadReceive.destroyThread(NECommon::DO_NOT_WAIT);
    mThreadSend.destroyThread(NECommon::DO_NOT_WAIT);

    return startConnection();
}

void ServerService::stopConnection(void)
{
    TRACE_SCOPE(mcrouter_tcp_private_ServerService_stopConnection);
    TRACE_WARN("Stopping remote servicing connection");

    mThreadReceive.triggerExitEvent();
    mThreadReceive.destroyThread( NECommon::DO_NOT_WAIT );
    mServerConnection.disableReceive( );

    TEArrayList<StubAddress>  stubList;
    TEArrayList<ProxyAddress> proxyList;
    extractRemoteServiceAddresses(NEService::COOKIE_ANY, stubList, proxyList);

    for ( uint32_t i = 0; i < stubList.getSize(); ++ i )
    {
        unregisteredRemoteServiceProvider( stubList[i], NEService::eDisconnectReason::ReasonRouterDisconnected, NEService::COOKIE_ANY );
    }

    for ( uint32_t i = 0; i < proxyList.getSize(); ++ i )
    {
        unregisteredRemoteServiceConsumer( proxyList[i], NEService::eDisconnectReason::ReasonRouterDisconnected, NEService::COOKIE_ANY );
    }

    _disconnectService( Event::eEventPriority::EventPriorityHigh );

    mThreadSend.destroyThread( NECommon::WAIT_INFINITE );
    mServerConnection.closeSocket();
}

void ServerService::extractRemoteServiceAddresses( ITEM_ID cookie, TEArrayList<StubAddress> & OUT out_listStubs, TEArrayList<ProxyAddress> & OUT out_lisProxies ) const
{
    mServiceRegistry.getServiceList(cookie, out_listStubs, out_lisProxies);
}

void ServerService::registeredRemoteServiceProvider(const StubAddress & stub)
{
    TRACE_SCOPE(mcrouter_tcp_private_ServerService_registeredRemoteServiceProvider);
    ASSERT(stub.isServicePublic());

    TRACE_DBG("Going to register remote stub [ %s ]", StubAddress::convAddressToPath(stub).getString());
    if ( mServiceRegistry.getServiceStatus(stub) != NEService::eServiceConnection::ServiceConnected )
    {
        ListServiceProxies listProxies;
        const ServiceStub & stubService = mServiceRegistry.registerServiceStub(stub, listProxies);
        if ( stubService.getServiceStatus() == NEService::eServiceConnection::ServiceConnected && listProxies.isEmpty() == false )
        {
            TRACE_DBG("Stub [ %s ] is connected, sending notification messages to [ %d ] waiting proxies"
                        , StubAddress::convAddressToPath(stubService.getServiceAddress()).getString()
                        , listProxies.getSize());

            TEArrayList<ITEM_ID> sendList;
            for (ListServiceProxiesBase::LISTPOS pos = listProxies.firstPosition(); listProxies.isValidPosition(pos); pos = listProxies.nextPosition(pos) )
            {
                const ServiceProxy & proxyService = listProxies.valueAtPosition(pos);
                const ProxyAddress & addrProxy    = proxyService.getServiceAddress();
                if ( (proxyService.getServiceStatus() == NEService::eServiceConnection::ServiceConnected) && (addrProxy.getSource() != stub.getSource()) )
                {
                    RemoteMessage msgRegisterProxy = NEConnection::createServiceClientRegisteredNotification(addrProxy, stub.getSource());
                    _sendMessage(msgRegisterProxy);

                    TRACE_DBG("Send to stub [ %s ] the proxy [ %s ] registration notification. Send message [ %s ] of id [ 0x%X ] from source [ %u ] to target [ %u ]"
                                , stub.convToString().getString()
                                , addrProxy.convToString().getString()
                                , NEService::getString( static_cast<NEService::eFuncIdRange>(msgRegisterProxy.getMessageId()))
                                , static_cast<uint32_t>(msgRegisterProxy.getMessageId())
                                , static_cast<uint32_t>(msgRegisterProxy.getSource())
                                , static_cast<uint32_t>(msgRegisterProxy.getTarget()));

                    if ( sendList.addIfUnique(addrProxy.getSource()) )
                    {
                        RemoteMessage msgRegisterStub  = NEConnection::createServiceRegisteredNotification(stub, addrProxy.getSource());
                        _sendMessage(msgRegisterStub);

                        TRACE_DBG("Send to proxy [ %s ] the stub [ %s ] registration notification. Send message [ %s ] of id [ 0x%X ] from source [ %u ] to target [ %u ]"
                                    , addrProxy.convToString().getString()
                                    , stub.convToString().getString()
                                    , NEService::getString( static_cast<NEService::eFuncIdRange>(msgRegisterStub.getMessageId()))
                                    , static_cast<uint32_t>(msgRegisterStub.getMessageId())
                                    , static_cast<uint32_t>(msgRegisterStub.getSource())
                                    , static_cast<uint32_t>(msgRegisterStub.getTarget()));
                    }
                    else
                    {
                        // ignore, it already has registered stub
                        TRACE_DBG("Ignoring sending stub registration message to target [ %u ], target already notified", static_cast<uint32_t>(addrProxy.getSource()));
                    }
                }
                else
                {
                     // ignore, it already has registered stub locally or proxy is not connected
                    TRACE_DBG("ignoring sending stub registration message, Stub [ %s ] and Proxy [ %s ] have same origin."
                                    , StubAddress::convAddressToPath(stub).getString()
                                    , ProxyAddress::convAddressToPath(addrProxy).getString());
                }
            }
        }
        else
        {
            // ignore, stub is not connected or list is empty
            TRACE_DBG("Ignoring sending stub [ %s ] connected notification, the proxy list is empty [ %s ]"
                            , StubAddress::convAddressToPath(stub).getString()
                            , listProxies.isEmpty() ? "YES" : "NO");
        }
    }
    else
    {
        // ignore, stub is already connected
        TRACE_DBG("Stub [ %s ] is already marked as connected, ignoring registration", StubAddress::convAddressToPath(stub).getString());
    }
}

void ServerService::registeredRemoteServiceConsumer(const ProxyAddress & proxy)
{
    TRACE_SCOPE(mcrouter_tcp_private_ServerService_registeredRemoteServiceConsumer);
    if ( mServiceRegistry.getServiceStatus(proxy) != NEService::eServiceConnection::ServiceConnected )
    {
        ServiceProxy proxyService;
        const ServiceStub & stubService   = mServiceRegistry.registerServiceProxy(proxy, proxyService);
        const StubAddress & addrStub      = stubService.getServiceAddress();

        TRACE_DBG("Registered proxy [ %s ], for connection with stub [ %s ], connection status is [ %s ]"
                    , ProxyAddress::convAddressToPath(proxy).getString()
                    , StubAddress::convAddressToPath(addrStub).getString()
                    , NEService::getString( proxyService.getServiceStatus()));

        if ( (proxyService.getServiceStatus() == NEService::eServiceConnection::ServiceConnected) && (proxy.getSource() != addrStub.getSource()) )
        {
            RemoteMessage msgRegisterProxy = NEConnection::createServiceClientRegisteredNotification(proxy, addrStub.getSource());
            _sendMessage(msgRegisterProxy);

            TRACE_DBG("Send to stub [ %s ] the proxy [ %s ] registration notification. Send message [ %s ] of id [ 0x%X ] from source [ %u ] to target [ %u ]"
                        , addrStub.convToString().getString()
                        , proxy.convToString().getString()
                        , NEService::getString( static_cast<NEService::eFuncIdRange>(msgRegisterProxy.getMessageId()))
                        , static_cast<uint32_t>(msgRegisterProxy.getMessageId())
                        , static_cast<uint32_t>(msgRegisterProxy.getSource())
                        , static_cast<uint32_t>(msgRegisterProxy.getTarget()));

            RemoteMessage msgRegisterStub  = NEConnection::createServiceRegisteredNotification(addrStub, proxy.getSource());
            _sendMessage(msgRegisterStub);

            TRACE_DBG("Send to proxy [ %s ] the stub [ %s ] registration notification. Send message [ %s ] of id [ 0x%X ] from source [ %u ] to target [ %u ]"
                        , proxy.convToString().getString()
                        , addrStub.convToString().getString()
                        , NEService::getString( static_cast<NEService::eFuncIdRange>(msgRegisterStub.getMessageId()))
                        , static_cast<uint32_t>(msgRegisterStub.getMessageId())
                        , static_cast<uint32_t>(msgRegisterStub.getSource())
                        , static_cast<uint32_t>(msgRegisterStub.getTarget()));
        }
        else
        {
            // ignore, it is done locally
            TRACE_DBG("Ignore send stub registration message, Stub [ %s ] and Proxy [ %s ] have same origin."
                            , StubAddress::convAddressToPath(addrStub).getString()
                            , ProxyAddress::convAddressToPath(proxy).getString());
        }
    }
    else
    {
        TRACE_DBG("Proxy [ %s ] is already having connected status, ignoring registration", ProxyAddress::convAddressToPath(proxy).getString());
    }
}

void ServerService::unregisteredRemoteServiceProvider(const StubAddress & stub, NEService::eDisconnectReason reason, ITEM_ID cookie /*= NEService::COOKIE_ANY*/ )
{
    TRACE_SCOPE(mcrouter_tcp_private_ServerService_unregisteredRemoteServiceProvider);
    if ( mServiceRegistry.getServiceStatus(stub) == NEService::eServiceConnection::ServiceConnected )
    {
        ListServiceProxies listProxies;
        mServiceRegistry.unregisterServiceStub(stub, listProxies);
        TRACE_DBG("Unregistered stub [ %s ], [ %d ] proxies are going to be notified"
                        , stub.convToString().getString()
                        , listProxies.getSize());

        TRACE_DBG("Filter sources [ %u ] of proxy list", static_cast<unsigned int>(cookie));

        TEArrayList<ITEM_ID> sendList;
        for (ListServiceProxiesBase::LISTPOS pos = listProxies.firstPosition(); listProxies.isValidPosition(pos); pos = listProxies.nextPosition(pos) )
        {
            const ServiceProxy & proxyService = listProxies.valueAtPosition(pos);
            const ProxyAddress & addrProxy    = proxyService.getServiceAddress();

            if ( (cookie == NEService::COOKIE_ANY) || (addrProxy.getSource() != cookie) )
            {
                // no need to send message to unregistered stub, only to proxy side
                if (sendList.addIfUnique(addrProxy.getSource()) )
                {
                    RemoteMessage msgRegisterStub = NEConnection::createServiceUnregisteredNotification( stub, reason, addrProxy.getSource( ) );
                    _sendMessage(msgRegisterStub, Event::eEventPriority::EventPriorityHigh);

                    TRACE_INFO("Send stub [ %s ] disconnect message to proxy [ %s ]"
                                    , stub.convToString().getString()
                                    , addrProxy.convToString().getString());
                }
                else
                {
                    // ignore, it already has unregistered stub
                    TRACE_DBG("Ignore unregistered stub message for proxy [ %s ], it was already notified", addrProxy.convToString().getString());
                }
            }
            else
            {
                // ignore, it already has unregistered stub locally or proxy status did not changed
                ServiceProxy dummy;
                mServiceRegistry.unregisterServiceProxy(addrProxy, dummy);
                TRACE_DBG("Proxy [ %s ] is marked as ignored by source [ %u ], remove and skip", addrProxy.convToString().getString(), static_cast<unsigned int>(cookie));
            }
        }
    }
    else
    {
        // ignore, stub is already disconnected
        TRACE_DBG("Ignore unregistering stub [ %s ], it is already unregistered", stub.convToString().getString());
    }
}

void ServerService::unregisteredRemoteServiceConsumer(const ProxyAddress & proxy, NEService::eDisconnectReason reason, ITEM_ID cookie /*= NEService::COOKIE_ANY*/ )
{
    TRACE_SCOPE(mcrouter_tcp_private_ServerService_unregisteredRemoteServiceConsumer);
    TRACE_DBG("Unregistering services of proxy [ %s ] related to cookie [ %u ]"
                    , ProxyAddress::convAddressToPath(proxy).getString()
                    , static_cast<unsigned int>(cookie));

    RemoteMessage msgRegisterProxy;
    ServiceProxy svcProxy;
    const ServiceStub * svcStub     = nullptr;

    if (proxy.getSource() == cookie)
    {
        svcStub = &mServiceRegistry.unregisterServiceProxy(proxy, svcProxy);
    }
    else
    {
        svcStub = &mServiceRegistry.disconnectProxy(proxy);
    }

    ASSERT(svcStub != nullptr);
    const StubAddress & addrStub    = svcStub->getServiceAddress();

    if ((svcStub->getServiceStatus() == NEService::eServiceConnection::ServiceConnected) && (proxy.getSource() != addrStub.getSource()))
    {
        msgRegisterProxy = NEConnection::createServiceClientUnregisteredNotification(proxy, reason, addrStub.getSource());
        _sendMessage(msgRegisterProxy, Event::eEventPriority::EventPriorityHigh);

        TRACE_INFO("Send proxy [ %s ] disconnect message to stub [ %s ]"
                        , proxy.convToString().getString()
                        , addrStub.convToString().getString());
    }
    else
    {
        TRACE_DBG("Ignore notifying proxy [ %s ] disconnect"
                        , proxy.convToString().getString());
    }
}

void ServerService::connectedRemoteServiceChannel(const Channel & /* channel */)
{

}

void ServerService::disconnectedRemoteServiceChannel(const Channel & /* channel */)
{

}

void ServerService::lostRemoteServiceChannel(const Channel & /* channel */)
{
}

void ServerService::failedSendMessage(const RemoteMessage & msgFailed, Socket & whichTarget )
{
    TRACE_SCOPE(mcrouter_tcp_private_ServerService_failedSendMessage);

#ifdef DEBUG

    ITEM_ID cookie = msgFailed.getTarget( );
    SocketAccepted client = mServerConnection.getClientByCookie( cookie );
    ASSERT( whichTarget.getHandle( ) == client.getHandle( ) );

#endif // DEBUG

    TRACE_WARN("Failed to send message to [ %s ] client [ %d ], probably the connection is lost, closing connection"
                    , whichTarget.isValid() ? "VALID" : "INVALID"
                    , static_cast<int32_t>(whichTarget.getHandle()));

    if ( whichTarget.isValid())
    {
        connectionLost( static_cast<SocketAccepted &>(whichTarget) );
    }
}

void ServerService::failedReceiveMessage(Socket & whichSource)
{
    TRACE_SCOPE(mcrouter_tcp_private_ServerService_failedReceiveMessage);

    SocketAccepted client = mServerConnection.getClientByHandle(whichSource.getHandle());
    TRACE_WARN("Failed to receive message from [ %s ] client [ %d ], probably the connection with socket [ %d ] is lost, closing connection"
                        , client.isValid() ? "VALID" : "INVALID"
                        , static_cast<int32_t>(client.getHandle())
                        , static_cast<int32_t>(whichSource.getHandle()));

    if (client.isValid())
    {
        connectionLost(client);
    }
}

void ServerService::failedProcessMessage(const RemoteMessage & /* msgUnprocessed */)
{

}

void ServerService::processReceivedMessage(const RemoteMessage & msgReceived, Socket & whichSource)
{
    TRACE_SCOPE(mcrouter_tcp_private_ServerService_processReceivedMessage);
    if ( msgReceived.isValid() )
    {
        ITEM_ID cookie = mServerConnection.getCookie(whichSource.getHandle());
        ITEM_ID source = static_cast<ITEM_ID>(msgReceived.getSource());
        ITEM_ID target = static_cast<ITEM_ID>(msgReceived.getTarget());
        NEService::eFuncIdRange msgId  = static_cast<NEService::eFuncIdRange>( msgReceived.getMessageId() );

        TRACE_DBG("Received message [ %s ] of id [ 0x%X ] from source [ %u ] ( connection cookie = %u ) of client host [ %s : %d ] for target [ %u ]"
                        , NEService::getString(msgId)
                        , static_cast<uint32_t>(msgId)
                        , static_cast<uint32_t>(source)
                        , static_cast<uint32_t>(cookie)
                        , static_cast<const char *>(whichSource.getAddress().getHostAddress())
                        , static_cast<int>(whichSource.getAddress().getHostPort( ))
                        , static_cast<id_type>(target));

        if ( (source > NEService::COOKIE_ROUTER) && NEService::isExecutableId(static_cast<uint32_t>(msgId)) )
        {
            TRACE_DBG("Forwarding message [ 0x%X ] to send to target [ %u ]", static_cast<uint32_t>(msgId), static_cast<uint32_t>(target));
            if ( target != NEService::TARGET_UNKNOWN )
            {
                _sendMessage(msgReceived);
            }
        }
        else if ( (source == cookie) && (msgId != NEService::eFuncIdRange::ServiceRouterConnect) )
        {
            TRACE_DBG("Going to process received message [ 0x%X ]", static_cast<uint32_t>(msgId));
            sendCommunicationMessage( ServerServiceEventData::eServerServiceCommands::CMD_ServiceReceivedMsg, msgReceived );
        }
        else if ( (source == NEService::SOURCE_UNKNOWN) && (msgId == NEService::eFuncIdRange::ServiceRouterConnect) )
        {
            RemoteMessage msgConnect = NEConnection::createConnectNotify(cookie);
            TRACE_DBG("Received request connect message, sending response [ %s ] of id [ 0x%X ], to new target [ %u ], connection socket [ %u ], checksum [ %u ]"
                        , NEService::getString( static_cast<NEService::eFuncIdRange>(msgConnect.getMessageId()))
                        , static_cast<uint32_t>(msgConnect.getMessageId())
                        , static_cast<uint32_t>(msgConnect.getTarget())
                        , static_cast<uint32_t>(whichSource.getHandle())
                        , msgConnect.getChecksum());

            _sendMessage( msgConnect );
        }
        else
        {
            TRACE_WARN("Ignoring to process message [ %s ] of id [ 0x%X ] from source [ %u ]"
                        , NEService::getString(msgId)
                        , static_cast<uint32_t>(msgId)
                        , static_cast<uint32_t>(source));
            ASSERT(false);
        }
    }
    else
    {
        TRACE_WARN("Received invalid message from source [ %u ], ignoring to process", static_cast<uint32_t>(msgReceived.getSource()));
    }
}

bool ServerService::runDispatcher(void)
{
    startEventListener( );

    bool result = DispatcherThread::runDispatcher();

    stopEventListener( );

    return result;
}

bool ServerService::postEvent(Event & eventElem)
{
    return EventDispatcher::postEvent(eventElem);
}

inline bool ServerService::_sendMessage( const RemoteMessage & data, Event::eEventPriority eventPrio /*= Event::eEventPriority::EventPriorityNormal*/ )
{
    return SendMessageEvent::sendEvent( SendMessageEventData( data )
                                        , static_cast<IESendMessageEventConsumer &>(mThreadSend)
                                        , static_cast<DispatcherThread &>(mThreadSend)
                                        , eventPrio );
}

inline void ServerService::_disconnectService( Event::eEventPriority eventPrio )
{
    SendMessageEvent::sendEvent( SendMessageEventData( false )
                                 , static_cast<IESendMessageEventConsumer &>(mThreadSend)
                                 , static_cast<DispatcherThread &>(mThreadSend)
                                 , eventPrio );
}
