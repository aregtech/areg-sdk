/************************************************************************
 * \file        areg/ipc/private/ServerService.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Service Connection server declaration
 ************************************************************************/
#include "areg/ipc/ServerService.hpp"

#include "areg/ipc/private/NEConnection.hpp"
#include "areg/ipc/ConnectionConfiguration.hpp"
#include "areg/trace/GETrace.h"

DEF_TRACE_SCOPE(areg_ipc_private_ServerService_startRemoteServicing);
DEF_TRACE_SCOPE(areg_ipc_private_ServerService_stopRemoteServicing);
DEF_TRACE_SCOPE(areg_ipc_private_ServerService_registerService);
DEF_TRACE_SCOPE(areg_ipc_private_ServerService_unregisterService);
DEF_TRACE_SCOPE(areg_ipc_private_ServerService_registerServiceClient);
DEF_TRACE_SCOPE(areg_ipc_private_ServerService_unregisterServiceClient);
DEF_TRACE_SCOPE(areg_ipc_private_ServerService_connectionLost);
DEF_TRACE_SCOPE(areg_ipc_private_ServerService_processEvent);
DEF_TRACE_SCOPE(areg_ipc_private_ServerService_registerRemoteStub);
DEF_TRACE_SCOPE(areg_ipc_private_ServerService_registerRemoteProxy);
DEF_TRACE_SCOPE(areg_ipc_private_ServerService_unregisterRemoteStub);
DEF_TRACE_SCOPE(areg_ipc_private_ServerService_unregisterRemoteProxy);
DEF_TRACE_SCOPE(areg_ipc_private_ServerService_processReceivedMessage);

DEF_TRACE_SCOPE(areg_ipc_private_ServerService_startConnection);
DEF_TRACE_SCOPE(areg_ipc_private_ServerService_stopConnection);

DEF_TRACE_SCOPE(areg_ipc_private_ServerService_failedSendMessage);
DEF_TRACE_SCOPE(areg_ipc_private_ServerService_failedReceiveMessage);

const NERemoteService::eServiceConnection   ServerService::CONNECT_TYPE   = NERemoteService::ConnectionTcpip;

ServerService::ServerService( void )
    : IERemoteService              ( )
    , DispatcherThread             ( NEConnection::SERVER_DISPATCH_MESSAGE_THREAD )
    , IEServerConnectionHandler    ( )
    , IERemoteServiceConsumer      ( )
    , IERemoteServiceHandler       ( )
    , IEServerServiceEventConsumer ( )
    , IETimerConsumer              ( )

    , mServerConnection ( )
    , mTimerConnect     ( static_cast<IETimerConsumer &>(self()), NEConnection::SERVER_CONNECT_TIMER_NAME)
    , mThreadSend       ( static_cast<IERemoteServiceHandler &>(self()), mServerConnection )
    , mThreadReceive    ( static_cast<IEServerConnectionHandler &>(self()), static_cast<IERemoteServiceHandler &>(self()), mServerConnection )
    , mServiceRegistry  ( )
    , mIsServiceEnabled ( true )    // TODO: by default it should be disabled and enabled via init file
    , mConfigFile       ( )
    , mWhiteList        ( )
    , mBlackList        ( )
    , mLock             ( )
{

}

ServerService::~ServerService(void)
{
    ; // do nothing
}

bool ServerService::configureRemoteServicing(const char * configFile)
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
        return mServerConnection.setAddress( NEConnection::DEFAULT_REMOTE_SERVICE_HOST, NEConnection::DEFAULT_REMOTE_SERVICE_PORT );
    }
}

void ServerService::setRemoteServiceAddress(const char * hostName, unsigned short portNr)
{
    mServerConnection.setAddress( hostName, portNr );
}

bool ServerService::startRemoteServicing(void)
{
    TRACE_SCOPE(areg_ipc_private_ServerService_startRemoteServicing);

    Lock lock(mLock);
    bool result = true;
    if ( mServerConnection.isValid() == false && isRunning() == false )
    {
        result = createThread( Thread::WAIT_INFINITE ) && waitForDispatcherStart(IESynchObject::WAIT_INFINITE);
        TRACE_DBG("Created remote servicing thread with [ %s ]", result ? "SUCCESS" : "FAIL");
    }
#ifdef DEBUG
    else
    {
        ASSERT(isRunning());
    }
#endif // DEBUG

    return result;
}

void ServerService::stopRemoteServicing(void)
{
    TRACE_SCOPE(areg_ipc_private_ServerService_stopRemoteServicing);
    if ( isRunning() )
    {
        ServerServiceEvent::sendEvent( ServerServiceEventData(ServerServiceEventData::CMD_StopService), static_cast<IEServerServiceEventConsumer &>(self()), static_cast<DispatcherThread &>(self()) );
        DispatcherThread::triggerExitEvent();
        completionWait(Thread::WAIT_INFINITE);

        destroyThread( Thread::DO_NOT_WAIT );
    }
}

bool ServerService::isRemoteServicingStarted(void) const
{
    Lock lock( mLock );
    return (mServerConnection.isValid() && isRunning());
}

bool ServerService::isRemoteServicingConfigured(void) const
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
    if ( isRunning() && (enable == false) )
        stopRemoteServicing();

    mIsServiceEnabled = enable;
}

bool ServerService::registerService(const StubAddress & /* stubService */)
{
    TRACE_SCOPE(areg_ipc_private_ServerService_registerService);
    TRACE_ERR("Method is not implemented, this should be called");
    return false;
}

void ServerService::unregisterService(const StubAddress & /* stubService */)
{
    TRACE_SCOPE(areg_ipc_private_ServerService_unregisterService);
    TRACE_ERR("Method is not implemented, this should be called");
}

bool ServerService::registerServiceClient(const ProxyAddress & /* proxyService */)
{
    TRACE_SCOPE(areg_ipc_private_ServerService_registerServiceClient);
    TRACE_ERR("Method is not implemented, this should be called");
    return false;
}

void ServerService::unregisterServiceClient(const ProxyAddress & /* proxyService */)
{
    TRACE_SCOPE(areg_ipc_private_ServerService_unregisterServiceClient);
    TRACE_ERR("Method is not implemented, this should be called");
}

bool ServerService::canAcceptConnection(const SocketAccepted & clientSocket)
{
    return clientSocket.isValid() ? canAcceptConnection( clientSocket.getAddress() ) : false;
}

void ServerService::connectionLost( SocketAccepted & clientSocket )
{
    TRACE_SCOPE(areg_ipc_private_ServerService_connectionLost);
    ITEM_ID cookie = mServerConnection.getCookie(clientSocket);

    TRACE_WARN("Client lost connection: cookie [ %p ], socket [ %d ], host [ %s : %d ], closing connection"
                , static_cast<id_type>(cookie)
                , clientSocket.getHandle()
                , clientSocket.getAddress().getHostAddress().getString()
                , clientSocket.getAddress().getHostPort());

    mServerConnection.closeConnection(clientSocket);
    if ( cookie != NEService::COOKIE_UNKNOWN )
    {
        RemoteMessage msgDisconnect = NEConnection::createDisconnectRequest(cookie);
        ServerServiceEvent::sendEvent( ServerServiceEventData(ServerServiceEventData::CMD_ServiceReceivedMsg, msgDisconnect), static_cast<IEServerServiceEventConsumer &>(self()), static_cast<DispatcherThread &>(self()) );
    }
}

void ServerService::processTimer(Timer & timer)
{
    if ( &timer == &mTimerConnect )
    {
        Lock lock( mLock );
        startConnection();
    }
}

void ServerService::processEvent(const ServerServiceEventData & data)
{
    TRACE_SCOPE(areg_ipc_private_ServerService_processEvent);
    TRACE_DBG("Going to process event [ %s ]", ServerServiceEventData::getString(data.getCommand()));

    switch ( data.getCommand() )
    {
    case ServerServiceEventData::CMD_StartService:
        {
            Lock lock( mLock );
            startConnection();
        }
        break;

    case ServerServiceEventData::CMD_StopService:
        {
            Lock lock(mLock);
            stopConnection();
        }
        mThreadReceive.completionWait( Thread::WAIT_INFINITE );
        mThreadSend.completionWait( Thread::WAIT_INFINITE );
        mThreadReceive.destroyThread( Thread::DO_NOT_WAIT );
        mThreadSend.destroyThread( Thread::DO_NOT_WAIT );
        break;

    case ServerServiceEventData::CMD_ServiceReceivedMsg:
        {
            const RemoteMessage & msgReceived = data.getMessage();
            ASSERT( msgReceived.isValid() );
            NEService::eFuncIdRange msgId = static_cast<NEService::eFuncIdRange>( msgReceived.getMessageId() );
            ITEM_ID source= static_cast<ITEM_ID>(msgReceived.getSource());

            TRACE_DBG("Processing received valid message [ %s ] ( ID = %p )from source [ %p ] to target [ %p ]"
                            , NEService::getString(msgId)
                            , static_cast<id_type>(msgId)
                            , static_cast<id_type>(source)
                            , static_cast<id_type>(msgReceived.getTarget()));

            switch( msgId )
            {
            case NEService::SI_ROUTER_REGISTER:
                {
                    NEService::eServiceRequestType reqType;
                    msgReceived >> reqType;
                    TRACE_DBG("Routing service received registration request message [ %s ]", NEService::getString(reqType));
                    switch ( reqType )
                    {
                    case NEService::SERVICE_REQUEST_REGISTER_STUB:
                        {
                            StubAddress stubService(msgReceived);
                            stubService.setSource(source);
                            registerRemoteStub(stubService);
                        }
                        break;

                    case NEService::SERVICE_REQUEST_REGISTER_CLIENT:
                        {
                            ProxyAddress proxyService(msgReceived);
                            proxyService.setSource(source);
                            registerRemoteProxy(proxyService);
                        }
                        break;

                    case NEService::SERVICE_REQUEST_UNREGISTER_STUB:
                        {
                            StubAddress stubService(msgReceived);
                            stubService.setSource(source);
                            unregisterRemoteStub(stubService, stubService.getCookie());
                        }
                        break;

                    case NEService::SERVICE_REQUEST_UNREGISTER_CLIENT:
                        {
                            ProxyAddress proxyService(msgReceived);
                            proxyService.setSource(source);
                            unregisterRemoteProxy(proxyService, proxyService.getCookie());
                        }
                        break;

                    default:
                        ASSERT(false);
                        break;  // do nothing
                    }
                }
                break;

            case NEService::SI_ROUTER_DISCONNECT:
                {
                    ITEM_ID cookie = NEService::COOKIE_UNKNOWN;
                    msgReceived >> cookie;
                    mServerConnection.closeConnection(cookie);

                    TEArrayList<StubAddress, const StubAddress &>   listStubs;
                    TEArrayList<ProxyAddress, const ProxyAddress &> listProxies;
                    mServiceRegistry.getServiceSources(cookie, listStubs, listProxies);

                    TRACE_DBG("Routing service received disconnect message from cookie [ %u ], [ %d ] stubs and [ %d ] proxies are going to be disconnected"
                                , static_cast<unsigned int>(cookie)
                                , listStubs.getSize()
                                , listProxies.getSize());

                    int index = 0;
                    for ( index = 0; index < listProxies.getSize(); ++ index )
                        unregisterRemoteProxy( listProxies[index], cookie );

                    for ( index = 0; index < listStubs.getSize(); ++ index )
                        unregisterRemoteStub( listStubs[index], cookie );
                }
                break;

            case NEService::SI_REGISTER_ID_LAST:            // fall through
            case NEService::SI_ROUTER_QUERY:                // fall through
            case NEService::SI_ROUTER_CONNECT:              // fall through
            case NEService::SI_SERVICE_CONNECTION_REQUEST:  // fall through
            case NEService::SI_NOTIFY_VERSION:              // fall through
            case NEService::SI_REQUEST_VERSION:             // fall through
            case NEService::SI_REGISTER_ID_FIRST:           // fall through
            case NEService::COMPONENT_MAKE_CLEANUP:         // fall through
            case NEService::SI_ROUTER_REGISTER_NOTIFY:      // fall through
            case NEService::SI_ROUTER_NOTIFY:               // fall through
                break;

            case NEService::SI_SERVICE_CONNECTION_NOTIFY:   // fall through
            case NEService::ATTRIBUTE_ID_LAST:              // fall through
            case NEService::ATTRIBUTE_ID_FIRST:             // fall through
            case NEService::RESPONSE_ID_LAST:               // fall through
            case NEService::RESPONSE_ID_FIRST:              // fall through
            case NEService::REQUEST_ID_LAST:                // fall through
            case NEService::REQUEST_ID_FIRST:               // fall through
            case NEService::EMPTY_FUNCTION_ID:              // fall through
            default:
                if ( NEService::isExecutableId(msgId) || NEService::isConnectNotifyId(msgId) )
                {
                    TRACE_DBG("Message [ %u ] is executable, going to forward to target [ %u ], received from source [ %u ]", msgId, msgReceived.getTarget(), source);
                    if ( msgReceived.getTarget() != NEService::TARGET_UNKNOWN )
                    {
                        SendMessageEvent::sendEvent( SendMessageEventData(msgReceived), static_cast<IESendMessageEventConsumer &>(mThreadSend), static_cast<DispatcherThread &>(mThreadSend));
                    }
                }
                else
                {
                    TRACE_ERR("Message [ %u ] is not executable, ignoring to forward to target [ %p ] from source [ %p ]"
                                    , msgId
                                    , static_cast<id_type>(msgReceived.getTarget())
                                    , static_cast<id_type>(source));
                    ASSERT(false);  // do nothing
                }
                break;
            }
        }
        break;

    case ServerServiceEventData::CMD_ServiceSendMsg:
        {
            const RemoteMessage & msgSend = data.getMessage();
            NEService::eFuncIdRange msgId = static_cast<NEService::eFuncIdRange>( msgSend.getMessageId() );
            TRACE_DBG("Sending message [ %s ] ( ID = %p ) is going to send to target [ %p ] from source [ %p ]"
                            , NEService::getString(msgId)
                            , static_cast<id_type>(msgId)
                            , static_cast<id_type>(msgSend.getTarget())
                            , static_cast<id_type>(msgSend.getSource()));

            if ( NEService::isExecutableId(msgId) )
            {
                if ( msgSend.getTarget() != NEService::TARGET_UNKNOWN )
                    SendMessageEvent::sendEvent( SendMessageEventData(msgSend), static_cast<IESendMessageEventConsumer &>(mThreadSend), static_cast<DispatcherThread &>(mThreadSend));
            }
            else
            {
                TRACE_ERR("The message [ %u ] is neither executable, nor router notification. Ignoring sending message", msgId);
                ASSERT(false); // do nothing
            }
        }
        break;

    default:
        ASSERT(false);
        break;
    }
}

bool ServerService::canAcceptConnection(const NESocket::InterlockedValue & addrConnect) const
{
    Lock lock(mLock);

    bool result = true;
    if ( mWhiteList.isEmpty() == false )
        result = isAddressInWhiteList(addrConnect);
    else if ( mBlackList.isEmpty() == false )
        result = isAddressInBlackList(addrConnect) == false;
    return result;
}

bool ServerService::startConnection(void)
{
    TRACE_SCOPE(areg_ipc_private_ServerService_startConnection);
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
        TRACE_DBG("Created socket [ %d ], going to create send-receive threads", mServerConnection.getSocketHandle());

        if ( mThreadReceive.createThread( Thread::WAIT_INFINITE ) && mThreadSend.createThread( Thread::WAIT_INFINITE ) )
        {
            VERIFY( mThreadReceive.waitForDispatcherStart( IESynchObject::WAIT_INFINITE ) );
            VERIFY( mThreadSend.waitForDispatcherStart( IESynchObject::WAIT_INFINITE ) );
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

void ServerService::stopConnection(void)
{
    TRACE_SCOPE(areg_ipc_private_ServerService_stopConnection);
    TRACE_WARN("Stopping remote servicing connection");

    mThreadReceive.triggerExitEvent();
    mThreadReceive.destroyThread( Thread::DO_NOT_WAIT );

    TEArrayList<StubAddress, const StubAddress &> stubList;
    TEArrayList<ProxyAddress, const ProxyAddress &> proxyList;
    getServiceList(NEService::COOKIE_ANY, stubList, proxyList);

    for ( int i = 0; i < stubList.getSize(); ++ i )
        unregisterRemoteStub(stubList[i], NEService::COOKIE_ANY);

    for ( int i = 0; i < proxyList.getSize(); ++ i )
        unregisterRemoteProxy( proxyList[i], NEService::COOKIE_ANY );

    mThreadSend.triggerExitEvent();
    mThreadSend.destroyThread( Thread::WAIT_INFINITE );

    mServerConnection.closeSocket();
}

void ServerService::getServiceList(ITEM_ID cookie, TEArrayList<StubAddress, const StubAddress &> & out_listStubs, TEArrayList<ProxyAddress, const ProxyAddress &> & out_lisProxies) const
{
    mServiceRegistry.getServiceList(cookie, out_listStubs, out_lisProxies);
}

void ServerService::registerRemoteStub(const StubAddress & stub)
{
    TRACE_SCOPE(areg_ipc_private_ServerService_registerRemoteStub);
    ASSERT(stub.isServiceRemote());

    TRACE_DBG("Going to register remote stub [ %s ]", StubAddress::convAddressToPath(stub).getString());
    if ( mServiceRegistry.getServiceStatus(stub) != NEService::ServiceConnected )
    {
        ListServiceProxies listProxies;
        const ServiceStub & stubService = mServiceRegistry.registerServiceStub(stub, listProxies);
        if ( stubService.getServiceStatus() == NEService::ServiceConnected && listProxies.isEmpty() == false )
        {
            TRACE_DBG("Stub [ %s ] is connected, sending notification messages to [ %d ] waiting proxies"
                        , StubAddress::convAddressToPath(stubService.getServiceAddress()).getString()
                        , listProxies.getSize());
            
            TEArrayList<ITEM_ID> sendList;

            for ( LISTPOS pos = listProxies.firstPosition(); pos != NULL; pos = listProxies.nextPosition(pos) )
            {
                const ServiceProxy & proxyService = listProxies.getAt(pos);
                const ProxyAddress & addrProxy    = proxyService.getServiceAddress();
                if ( (proxyService.getServiceStatus() == NEService::ServiceConnected) && (addrProxy.getSource() != stub.getSource()) )
                {
                    RemoteMessage msgRegisterProxy = NEConnection::createServiceClientRegisteredNotification(addrProxy, stub.getSource());
                    SendMessageEvent::sendEvent( SendMessageEventData(msgRegisterProxy), static_cast<IESendMessageEventConsumer &>(mThreadSend), static_cast<DispatcherThread &>(mThreadSend));
                    TRACE_DBG("Send to stub [ %s ] the proxy [ %s ] registration notification. Send message [ %s ] (ID = [ %u ] ) from source [ %u ] to target [ %u ]"
                                , stub.convToString().getString()
                                , addrProxy.convToString().getString()
                                , NEService::getString( static_cast<NEService::eFuncIdRange>(msgRegisterProxy.getMessageId()))
                                , static_cast<unsigned int>(msgRegisterProxy.getMessageId())
                                , static_cast<unsigned int>(msgRegisterProxy.getSource())
                                , static_cast<unsigned int>(msgRegisterProxy.getTarget()));

                    if ( sendList.addUnique(addrProxy.getSource()) )
                    {
                        RemoteMessage msgRegisterStub  = NEConnection::createServiceRegisteredNotification(stub, addrProxy.getSource());
                        SendMessageEvent::sendEvent( SendMessageEventData(msgRegisterStub), static_cast<IESendMessageEventConsumer &>(mThreadSend), static_cast<DispatcherThread &>(mThreadSend));
                        TRACE_DBG("Send to proxy [ %s ] the stub [ %s ] registration notification. Send message [ %s ] (ID = [ %u ] ) from source [ %u ] to target [ %u ]"
                                    , addrProxy.convToString().getString()
                                    , stub.convToString().getString()
                                    , NEService::getString( static_cast<NEService::eFuncIdRange>(msgRegisterStub.getMessageId()))
                                    , static_cast<unsigned int>(msgRegisterStub.getMessageId())
                                    , static_cast<unsigned int>(msgRegisterStub.getSource())
                                    , static_cast<unsigned int>(msgRegisterStub.getTarget()));
                    }
                    else
                    {
                        // ignore, it already has registered stub
                        TRACE_DBG("Ignoring sending stub registration message to target [ %p ], target already notified", addrProxy.getSource());
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

void ServerService::registerRemoteProxy(const ProxyAddress & proxy)
{
    TRACE_SCOPE(areg_ipc_private_ServerService_registerRemoteProxy);
    if ( mServiceRegistry.getServiceStatus(proxy) != NEService::ServiceConnected )
    {
        ServiceProxy proxyService;
        const ServiceStub & stubService   = mServiceRegistry.registerServiceProxy(proxy, proxyService);
        const StubAddress & addrStub      = stubService.getServiceAddress();

        TRACE_DBG("Registered proxy [ %s ], for connection with stub [ %s ], connection status is [ %s ]"
                    , ProxyAddress::convAddressToPath(proxy).getString()
                    , StubAddress::convAddressToPath(addrStub).getString()
                    , NEService::getString( proxyService.getServiceStatus()));

        if ( (proxyService.getServiceStatus() == NEService::ServiceConnected) && (proxy.getSource() != addrStub.getSource()) )
        {
            RemoteMessage msgRegisterProxy = NEConnection::createServiceClientRegisteredNotification(proxy, addrStub.getSource());
            SendMessageEvent::sendEvent( SendMessageEventData(msgRegisterProxy), static_cast<IESendMessageEventConsumer &>(mThreadSend), static_cast<DispatcherThread &>(mThreadSend));
            TRACE_DBG("Send to stub [ %s ] the proxy [ %s ] registration notification. Send message [ %s ] (ID = [ %u ] ) from source [ %u ] to target [ %u ]"
                        , addrStub.convToString().getString()
                        , proxy.convToString().getString()
                        , NEService::getString( static_cast<NEService::eFuncIdRange>(msgRegisterProxy.getMessageId()))
                        , static_cast<unsigned int>(msgRegisterProxy.getMessageId())
                        , static_cast<unsigned int>(msgRegisterProxy.getSource())
                        , static_cast<unsigned int>(msgRegisterProxy.getTarget()));

            RemoteMessage msgRegisterStub  = NEConnection::createServiceRegisteredNotification(addrStub, proxy.getSource());
            SendMessageEvent::sendEvent( SendMessageEventData(msgRegisterStub), static_cast<IESendMessageEventConsumer &>(mThreadSend), static_cast<DispatcherThread &>(mThreadSend));
            TRACE_DBG("Send to proxy [ %s ] the stub [ %s ] registration notification. Send message [ %s ] (ID = [ %u ] ) from source [ %u ] to target [ %u ]"
                        , proxy.convToString().getString()
                        , addrStub.convToString().getString()
                        , NEService::getString( static_cast<NEService::eFuncIdRange>(msgRegisterStub.getMessageId()))
                        , static_cast<unsigned int>(msgRegisterStub.getMessageId())
                        , static_cast<unsigned int>(msgRegisterStub.getSource())
                        , static_cast<unsigned int>(msgRegisterStub.getTarget()));
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

void ServerService::unregisterRemoteStub(const StubAddress & stub, ITEM_ID cookie /*= NEService::COOKIE_ANY*/ )
{
    TRACE_SCOPE(areg_ipc_private_ServerService_unregisterRemoteStub);
    if ( mServiceRegistry.getServiceStatus(stub) == NEService::ServiceConnected )
    {
        ListServiceProxies listProxies;
        mServiceRegistry.unregisterServiceStub(stub, listProxies);
        TRACE_DBG("Unregistered stub [ %s ], [ %d ] proxies are going to be notified"
                        , stub.convToString().getString()
                        , listProxies.getSize());

        TRACE_DBG("Filter sources [ %u ] of proxy list", static_cast<unsigned int>(cookie));

        TEArrayList<ITEM_ID> sendList;
        for ( LISTPOS pos = listProxies.firstPosition(); pos != NULL; pos = listProxies.nextPosition(pos) )
        {
            const ServiceProxy & proxyService = listProxies.getAt(pos);
            const ProxyAddress & addrProxy    = proxyService.getServiceAddress();

            if ( (cookie == NEService::COOKIE_ANY) || (addrProxy.getSource() != cookie) )
            {
                // no need to send message to unregistered stub, only to proxy side
                if (sendList.addUnique(addrProxy.getSource()) )
                {
                    RemoteMessage msgRegisterStub = NEConnection::createServiceUnregisteredNotification( stub, addrProxy.getSource( ) );
                    SendMessageEvent::sendEvent( SendMessageEventData(msgRegisterStub), static_cast<IESendMessageEventConsumer &>(mThreadSend), static_cast<DispatcherThread &>(mThreadSend));
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

void ServerService::unregisterRemoteProxy(const ProxyAddress & proxy, ITEM_ID cookie /*= NEService::COOKIE_ANY*/ )
{
    TRACE_SCOPE(areg_ipc_private_ServerService_unregisterRemoteProxy);
    TRACE_DBG("Unregistering services of proxy [ %s ] related to cookie [ %u ]"
                    , ProxyAddress::convAddressToPath(proxy).getString()
                    , static_cast<unsigned int>(cookie));

    RemoteMessage msgRegisterProxy;
    ServiceProxy svcProxy;
    const ServiceStub * svcStub     = NULL;
    
    if (proxy.getSource() == cookie)
    {
        svcStub = &mServiceRegistry.unregisterServiceProxy(proxy, svcProxy);
    }
    else
    {
        svcStub = &mServiceRegistry.disconnectProxy(proxy);
    }

    ASSERT(svcStub != NULL);
    const StubAddress & addrStub    = svcStub->getServiceAddress();

    if ((svcStub->getServiceStatus() == NEService::ServiceConnected) && (proxy.getSource() != addrStub.getSource()))
    {
        msgRegisterProxy = NEConnection::createServiceClientUnregisteredNotification(proxy, addrStub.getSource());
        SendMessageEvent::sendEvent( SendMessageEventData(msgRegisterProxy), static_cast<IESendMessageEventConsumer &>(mThreadSend), static_cast<DispatcherThread &>(mThreadSend));
        TRACE_INFO("Send proxy [ %s ] disconnect message to stub [ %s ]"
                        , proxy.convToString().getString()
                        , addrStub.convToString().getString());
    }
    else
    {
        TRACE_DBG("Ignore notifying stub [ %s ] proxy [ %s ] disconnect"
                        , addrStub.convToString().getString()
                        , proxy.convToString().getString());
    }
}

void ServerService::remoteServiceStarted(const Channel & /* channel */)
{

}

void ServerService::remoteServiceStopped(const Channel & /* channel */)
{

}

void ServerService::remoteServiceConnectionLost(const Channel & /* channel */)
{
}

void ServerService::failedSendMessage(const RemoteMessage & msgFailed)
{
    TRACE_SCOPE(areg_ipc_private_ServerService_failedSendMessage);
    
    ITEM_ID cookie = msgFailed.getTarget();
    SocketAccepted client = mServerConnection.getClientByCookie( cookie );
    TRACE_WARN("Failed to send message to [ %s ] client [ %p ], probably the connection is lost, closing connection"
                    , client.isValid() ? "VALID" : "INVALID"
                    , client.getHandle());
    if (client.isValid())
    {
        connectionLost(client);
    }
}

void ServerService::failedReceiveMessage(SOCKETHANDLE whichSource)
{
    TRACE_SCOPE(areg_ipc_private_ServerService_failedReceiveMessage);

    SocketAccepted client = mServerConnection.getClientByHandle(whichSource);
    TRACE_WARN("Failed to receive message from [ %s ] client [ %p ], probably the connection with socket [ %p ] is lost, closing connection"
                        , client.isValid() ? "VALID" : "INVALID"
                        , client.getHandle()
                        , whichSource);

    if (client.isValid())
    {
        connectionLost(client);
    }
}

void ServerService::failedProcessMessage(const RemoteMessage & /* msgUnprocessed */)
{

}

void ServerService::processReceivedMessage(const RemoteMessage & msgReceived, const NESocket::InterlockedValue & addrHost, SOCKETHANDLE whichSource)
{
    TRACE_SCOPE(areg_ipc_private_ServerService_processReceivedMessage);
    if ( msgReceived.isValid() )
    {
        ITEM_ID cookie = mServerConnection.getCookie(whichSource);
        ITEM_ID source = static_cast<ITEM_ID>(msgReceived.getSource());
        ITEM_ID target = static_cast<ITEM_ID>(msgReceived.getTarget());
        NEService::eFuncIdRange msgId  = static_cast<NEService::eFuncIdRange>( msgReceived.getMessageId() );

        TRACE_DBG("Received message [ %s ] ( ID = %p ) from source [ %p ] ( connection cookie = %p ) of client host [ %s : %d ] for target [ %p ]"
                        , NEService::getString(msgId)
                        , static_cast<id_type>(msgId)
                        , static_cast<id_type>(source)
                        , static_cast<id_type>(cookie)
                        , addrHost.getHostAddress().getString()
                        , static_cast<int>(addrHost.getHostPort())
                        , static_cast<id_type>(target));

        if ( (source > NEService::COOKIE_ROUTER) && NEService::isExecutableId(msgId) )
        {
            TRACE_DBG("Forwarding message [ %p ] to send to target [ %p ]", static_cast<id_type>(msgId), static_cast<id_type>(target));
            if ( target != NEService::TARGET_UNKNOWN )
            {
                SendMessageEvent::sendEvent( SendMessageEventData(msgReceived), static_cast<IESendMessageEventConsumer &>(mThreadSend), static_cast<DispatcherThread &>(mThreadSend));
            }
        }
        else if ( (source == cookie) && (msgId != NEService::SI_ROUTER_CONNECT) )
        {
            TRACE_DBG("Going to process received message [ %p ]", static_cast<id_type>(msgId));
            ServerServiceEvent::sendEvent( ServerServiceEventData(ServerServiceEventData::CMD_ServiceReceivedMsg, msgReceived), static_cast<IEServerServiceEventConsumer &>(self()), static_cast<DispatcherThread &>(self()) );
        }
        else if ( (source == NEService::SOURCE_UNKNOWN) && (msgId == NEService::SI_ROUTER_CONNECT) )
        {
            RemoteMessage msgConnect = NEConnection::createConnectNotify(cookie);
            TRACE_DBG("Received request connect message, sending response [ %s ] ( ID = %p ), to new target [ %p ], connection socket [ %p ], checksum [ %u ]"
                        , NEService::getString( static_cast<NEService::eFuncIdRange>(msgConnect.getMessageId()))
                        , static_cast<id_type>(msgConnect.getMessageId())
                        , static_cast<id_type>(msgConnect.getTarget())
                        , static_cast<id_type>(whichSource)
                        , msgConnect.getChecksum());

            if ( msgConnect.isValid() )
            {
                SendMessageEvent::sendEvent( msgConnect, static_cast<IESendMessageEventConsumer &>(mThreadSend), static_cast<DispatcherThread &>(mThreadSend));
            }
        }
        else
        {
            TRACE_WARN("Ignoring to process message [ %s ] ( ID = %p ) from source [ %p ]"
                        , NEService::getString(msgId)
                        , static_cast<id_type>(msgId)
                        , static_cast<id_type>(source));
            ASSERT(false);
        }
    }
    else
    {
        TRACE_WARN("Received invalid message from source [ %p ], ignoring to process", static_cast<id_type>(msgReceived.getSource()));
    }
}

bool ServerService::runDispatcher(void)
{
    ServerServiceEvent::addListener( static_cast<IEServerServiceEventConsumer &>(self()), static_cast<DispatcherThread &>(self()) );
    ServerServiceEvent::sendEvent( ServerServiceEventData(ServerServiceEventData::CMD_StartService), static_cast<IEServerServiceEventConsumer &>(self()), static_cast<DispatcherThread &>(self()) );

    bool result = DispatcherThread::runDispatcher();

    ServerServiceEvent::removeListener( static_cast<IEServerServiceEventConsumer &>(self()), static_cast<DispatcherThread &>(self()) );

    return result;
}

bool ServerService::postEvent(Event & eventElem)
{
    return EventDispatcher::postEvent(eventElem);
}
