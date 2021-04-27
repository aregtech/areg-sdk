/************************************************************************
 * \file        areg/ipc/private/CEServerService.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Service Connection server declaration
 ************************************************************************/
#include "areg/ipc/CEServerService.hpp"

#include "areg/ipc/private/NEConnection.hpp"
#include "areg/ipc/CEConnectionConfiguration.hpp"
#include "areg/trace/GETrace.h"

DEF_TRACE_SCOPE(areg_ipc_private_CEServerService_StartRemotingService);
DEF_TRACE_SCOPE(areg_ipc_private_CEServerService_StopRemotingService);
DEF_TRACE_SCOPE(areg_ipc_private_CEServerService_RegisterService);
DEF_TRACE_SCOPE(areg_ipc_private_CEServerService_UnregisterService);
DEF_TRACE_SCOPE(areg_ipc_private_CEServerService_RegisterServiceClient);
DEF_TRACE_SCOPE(areg_ipc_private_CEServerService_UnregisterServiceClient);
DEF_TRACE_SCOPE(areg_ipc_private_CEServerService_ConnectionLost);
DEF_TRACE_SCOPE(areg_ipc_private_CEServerService_ProcessEvent);
DEF_TRACE_SCOPE(areg_ipc_private_CEServerService_RegisterRemoteStub);
DEF_TRACE_SCOPE(areg_ipc_private_CEServerService_RegisterRemoteProxy);
DEF_TRACE_SCOPE(areg_ipc_private_CEServerService_UnregisterRemoteStub);
DEF_TRACE_SCOPE(areg_ipc_private_CEServerService_UnregisterRemoteProxy);
DEF_TRACE_SCOPE(areg_ipc_private_CEServerService_ProcessReceivedMessage);

DEF_TRACE_SCOPE(areg_ipc_private_CEServerService_startConnection);
DEF_TRACE_SCOPE(areg_ipc_private_CEServerService_stopConnection);

const NERemoteService::eServiceConnection   CEServerService::CONNECT_TYPE   = NERemoteService::ConnectionTcpip;

CEServerService::CEServerService( void )
    : IERemoteService              ( )
    , CEDispatcherThread           ( NEConnection::SERVER_DISPATCH_MESSAGE_THREAD )
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

CEServerService::~CEServerService(void)
{
    ; // do nothing
}

bool CEServerService::ServiceConfigure(const char * configFile)
{
    CEConnectionConfiguration configConnect;
    if ( configConnect.LoadConfiguration(configFile) )
    {
        mConfigFile             = configConnect.GetConfigFileName();
        mIsServiceEnabled       = configConnect.GetConnectionEnabled(CONNECT_TYPE);
        CEString hostName       = configConnect.GetConnectionHost(CONNECT_TYPE);
        unsigned short hostPort = configConnect.GetConnectionPort(CONNECT_TYPE);

        return mServerConnection.SetAddress( hostName, hostPort );
    }
    else
    {
        return mServerConnection.SetAddress( NEConnection::DEFAULT_REMOTE_SERVICE_HOST, NEConnection::DEFAULT_REMOTE_SERVICE_PORT );
    }
}

void CEServerService::SetRouterServiceAddress(const char * hostName, unsigned short portNr)
{
    mServerConnection.SetAddress( hostName, portNr );
}

bool CEServerService::StartRemotingService(void)
{
    TRACE_SCOPE(areg_ipc_private_CEServerService_StartRemotingService);

    CELock lock(mLock);
    bool result = true;
    if ( mServerConnection.IsValid() == false && IsRunning() == false )
    {
        result = CreateThread( CEThread::WAIT_INFINITE ) && WaitForDispatcherStart(IESynchObject::WAIT_INFINITE);
        TRACE_DBG("Created remote servicing thread with [ %s ]", result ? "SUCCESS" : "FAIL");
    }
#ifdef DEBUG
    else
    {
        ASSERT(IsRunning());
    }
#endif // DEBUG

    return result;
}

void CEServerService::StopRemotingService(void)
{
    TRACE_SCOPE(areg_ipc_private_CEServerService_StopRemotingService);
    if ( IsRunning() )
    {
        CEServerServiceEvent::SendEvent( CEServerServiceEventData(CEServerServiceEventData::CMD_StopService), static_cast<IEServerServiceEventConsumer &>(self()), static_cast<CEDispatcherThread &>(self()) );
        CEDispatcherThread::SetExitEvent();
        CompletionWait(CEThread::WAIT_INFINITE);

        DestroyThread( CEThread::DO_NOT_WAIT );
    }
}

bool CEServerService::IsServiceStarted(void) const
{
    CELock lock( mLock );
    return (mServerConnection.IsValid() && IsRunning());
}

bool CEServerService::IsServiceConfigured(void) const
{
    CELock lock(mLock);
    return mServerConnection.GetAddress().IsValid();
}

bool CEServerService::IsServiceEnabled(void) const
{
    CELock lock(mLock);
    return mIsServiceEnabled;
}

void CEServerService::EnableService( bool enable )
{
    CELock lock( mLock );
    if ( IsRunning() && (enable == false) )
        StopRemotingService();

    mIsServiceEnabled = enable;
}

bool CEServerService::RegisterService(const CEStubAddress & /* stubService */)
{
    TRACE_SCOPE(areg_ipc_private_CEServerService_RegisterService);
    TRACE_ERR("Method is not implemented, this should be called");
    return false;
}

void CEServerService::UnregisterService(const CEStubAddress & /* stubService */)
{
    TRACE_SCOPE(areg_ipc_private_CEServerService_UnregisterService);
    TRACE_ERR("Method is not implemented, this should be called");
}

bool CEServerService::RegisterServiceClient(const CEProxyAddress & /* proxyService */)
{
    TRACE_SCOPE(areg_ipc_private_CEServerService_RegisterServiceClient);
    TRACE_ERR("Method is not implemented, this should be called");
    return false;
}

void CEServerService::UnregisterServiceClient(const CEProxyAddress & /* proxyService */)
{
    TRACE_SCOPE(areg_ipc_private_CEServerService_UnregisterServiceClient);
    TRACE_ERR("Method is not implemented, this should be called");
}

bool CEServerService::CanAcceptConnection(const CESocketAccepted & clientSocket)
{
    return clientSocket.IsValid() ? canAcceptConnection( clientSocket.GetAddress() ) : false;
}

void CEServerService::ConnectionLost( CESocketAccepted & clientSocket )
{
    TRACE_SCOPE(areg_ipc_private_CEServerService_ConnectionLost);
    ITEM_ID cookie = mServerConnection.GetCookie(clientSocket);

    TRACE_WARN("Client lost connection: cookie [ %p ], socket [ %d ], host [ %s : %d ], closing connection"
                , cookie
                , clientSocket.GetHandle()
                , clientSocket.GetAddress().GetHostAddress().GetBuffer()
                , clientSocket.GetAddress().GetHostPort());

    mServerConnection.CloseConnection(clientSocket);
    if ( cookie != NEService::COOKIE_UNKNOWN )
    {
        CERemoteMessage msgDisconnect = NEConnection::CreateDisconnectRequest(cookie);
        CEServerServiceEvent::SendEvent( CEServerServiceEventData(CEServerServiceEventData::CMD_ServiceReceivedMsg, msgDisconnect), static_cast<IEServerServiceEventConsumer &>(self()), static_cast<CEDispatcherThread &>(self()) );
    }
}

void CEServerService::ProcessTimer(CETimer & timer)
{
    if ( &timer == &mTimerConnect )
    {
        CELock lock( mLock );
        startConnection();
    }
}

void CEServerService::ProcessEvent(const CEServerServiceEventData & data)
{
    TRACE_SCOPE(areg_ipc_private_CEServerService_ProcessEvent);
    TRACE_DBG("Going to process event [ %s ]", CEServerServiceEventData::GetString(data.GetCommand()));

    switch ( data.GetCommand() )
    {
    case CEServerServiceEventData::CMD_StartService:
        {
            CELock lock( mLock );
            startConnection();
        }
        break;

    case CEServerServiceEventData::CMD_StopService:
        {
            CELock lock(mLock);
            stopConnection();
        }
        mThreadReceive.CompletionWait( CEThread::WAIT_INFINITE );
        mThreadSend.CompletionWait( CEThread::WAIT_INFINITE );
        mThreadReceive.DestroyThread( CEThread::DO_NOT_WAIT );
        mThreadSend.DestroyThread( CEThread::DO_NOT_WAIT );
        break;

    case CEServerServiceEventData::CMD_ServiceReceivedMsg:
        {
            const CERemoteMessage & msgReceived = data.GetMessage();
            ASSERT( msgReceived.IsValid() );
            NEService::eFuncIdRange msgId = static_cast<NEService::eFuncIdRange>( msgReceived.GetMessageId() );
            ITEM_ID source= msgReceived.GetSource();

            TRACE_DBG("Processing received valid message [ %s ] ( ID = %p )from source [ %p ] to target [ %p ]", NEService::GetString(msgId), msgId, source, msgReceived.GetTarget());

            switch( msgId )
            {
            case NEService::SI_ROUTER_REGISTER:
                {
                    NEService::eServiceRequestType reqType;
                    msgReceived >> reqType;
                    TRACE_DBG("Broker service received registration request message [ %s ]", NEService::GetString(reqType));
                    switch ( reqType )
                    {
                    case NEService::SERVICE_REQUEST_REGISTER_STUB:
                        {
                            CEStubAddress stubService(msgReceived);
                            stubService.SetSource(source);
                            RegisterRemoteStub(stubService);
                        }
                        break;

                    case NEService::SERVICE_REQUEST_REGISTER_CLIENT:
                        {
                            CEProxyAddress proxyService(msgReceived);
                            proxyService.SetSource(source);
                            RegisterRemoteProxy(proxyService);
                        }
                        break;

                    case NEService::SERVICE_REQUEST_UNREGISTER_STUB:
                        {
                            CEStubAddress stubService(msgReceived);
                            stubService.SetSource(source);
                            UnregisterRemoteStub(stubService);
                        }
                        break;

                    case NEService::SERVICE_REQUEST_UNREGISTER_CLIENT:
                        {
                            CEProxyAddress proxyService(msgReceived);
                            proxyService.SetSource(source);
                            UnregisterRemoteProxy(proxyService);
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
                    CESocketAccepted client = mServerConnection.GetClientByCookie(cookie);
                    if ( client.IsValid() )
                        mServerConnection.CloseConnection(client);

                    TEArrayList<CEStubAddress, const CEStubAddress &>   listStubs;
                    TEArrayList<CEProxyAddress, const CEProxyAddress &> listProxies;

                    GetServiceList(cookie, listStubs);
                    GetServiceList(cookie, listProxies);
                    
                    TRACE_DBG("Broker service received disconnect message from cookie [ %p ], [ %d ] stubs and [ %d ] proxies are going to be disconnected"
                                , cookie
                                , listStubs.GetSize()
                                , listProxies.GetSize());

                    int index = 0;
                    for ( index = 0; index < listProxies.GetSize(); ++ index )
                        UnregisterRemoteProxy( listProxies[index] );
                    for ( index = 0; index < listStubs.GetSize(); ++ index )
                        UnregisterRemoteStub( listStubs[index] );
                }
                break;

            default:
                if ( NEService::IsExecutableId(msgId) || NEService::IsConnectNotifyId(msgId) )
                {
                    TRACE_DBG("Message [ %u ] is executable, going to forward to target [ %u ], received from source [ %u ]", msgId, msgReceived.GetTarget(), source);
                    if ( msgReceived.GetTarget() != NEService::TARGET_UNKNOWN )
                    {
                        CESendMessageEvent::SendEvent( CESendMessageEventData(msgReceived), static_cast<IESendMessageEventConsumer &>(mThreadSend), static_cast<CEDispatcherThread &>(mThreadSend));
                    }
                }
                else
                {
                    TRACE_ERR("Message [ %u ] is not executable, ignoring to forward to target [ %p ] from source [ %p ]", msgId, msgReceived.GetTarget(), source);
                    ASSERT(false);  // do nothing
                }
                break;
            }
        }
        break;

    case CEServerServiceEventData::CMD_ServiceSendMsg:
        {
            const CERemoteMessage & msgSend = data.GetMessage();
            NEService::eFuncIdRange msgId = static_cast<NEService::eFuncIdRange>( msgSend.GetMessageId() );
            TRACE_DBG("Sending message [ %s ] ( ID = %p ) is going to send to target [ %p ] from source [ %p ]", NEService::GetString(msgId), msgId, msgSend.GetTarget(), msgSend.GetSource());

            if ( NEService::IsExecutableId(msgId) )
            {
                if ( msgSend.GetTarget() != NEService::TARGET_UNKNOWN )
                    CESendMessageEvent::SendEvent( CESendMessageEventData(msgSend), static_cast<IESendMessageEventConsumer &>(mThreadSend), static_cast<CEDispatcherThread &>(mThreadSend));
            }
            else
            {
                TRACE_ERR("The message [ %u ] is neither executable, nor broker notification. ignoring sending message", msgId);
                ASSERT(false); // do nothing
            }
        }
        break;

    default:
        ASSERT(false);
        break;
    }
}

bool CEServerService::canAcceptConnection(const NESocket::CEInterlockedValue & addrConnect) const
{
    CELock lock(mLock);

    bool result = true;
    if ( mWhiteList.IsEmpty() == false )
        result = IsAddressInWhiteList(addrConnect);
    else if ( mBlackList.IsEmpty() == false )
        result = IsAddressInBlackList(addrConnect) == false;
    return result;
}

bool CEServerService::startConnection(void)
{
    TRACE_SCOPE(areg_ipc_private_CEServerService_startConnection);
    TRACE_DBG("Going to start connection. Address [ %u ], port [ %d ]"
                , mServerConnection.GetAddress().GetHostAddress().GetBuffer()
                , mServerConnection.GetAddress().GetHostPort());


    ASSERT(mServerConnection.GetAddress().IsValid());
    ASSERT(mServerConnection.IsValid() == false);
    ASSERT(mThreadReceive.IsRunning() == false);
    ASSERT(mThreadSend.IsRunning() == false);

    bool result = false;
    mTimerConnect.StopTimer();

    if ( mServerConnection.CreateSocket() )
    {
        TRACE_DBG("Created socket [ %d ], going to create send-receive threads", mServerConnection.GetSocketHandle());

        if ( mThreadReceive.CreateThread( CEThread::WAIT_INFINITE ) && mThreadSend.CreateThread( CEThread::WAIT_INFINITE ) )
        {
            VERIFY( mThreadReceive.WaitForDispatcherStart( IESynchObject::WAIT_INFINITE ) );
            VERIFY( mThreadSend.WaitForDispatcherStart( IESynchObject::WAIT_INFINITE ) );
            result = true;

            TRACE_DBG("The threads are created. Ready to send-receive messages.");
        }
        else
        {
            TRACE_ERR("Failed to create send-receive threads, cannot communicate. Stop remote service");
            mServerConnection.CloseSocket();
        }
    }
    else
    {
        TRACE_ERR("Failed to create remote servicing socket.");
    }

    if ( result == false )
    {
        TRACE_WARN("Remote servicing failed, trigger timer with [ %u ] ms timeout to re-establish remote servicing", NEConnection::DEFAULT_RETRY_CONNECT_TIMEOUT);
        mTimerConnect.StartTimer( NEConnection::DEFAULT_RETRY_CONNECT_TIMEOUT, static_cast<CEDispatcherThread &>(self()), 1);
    }

    return result;
}

void CEServerService::stopConnection(void)
{
    TRACE_SCOPE(areg_ipc_private_CEServerService_stopConnection);
    TRACE_WARN("Stopping remote servicing connection");

    mThreadReceive.SetExitEvent();
    mThreadReceive.DestroyThread( CEThread::DO_NOT_WAIT );

    TEArrayList<CEStubAddress, const CEStubAddress &> stubList;
    GetRemoteServiceList(stubList);
    for ( int i = 0; i < stubList.GetSize(); ++ i )
        UnregisterRemoteStub(stubList[i]);

    TEArrayList<CEProxyAddress, const CEProxyAddress &> proxyList;
    GetRemoteServiceList(proxyList);
    for ( int i = 0; i < proxyList.GetSize(); ++ i )
        UnregisterRemoteProxy( proxyList[i] );

    mThreadSend.SetExitEvent();
    mThreadSend.DestroyThread( CEThread::WAIT_INFINITE );

    mServerConnection.CloseSocket();
}

void CEServerService::GetRemoteServiceList(TEArrayList<CEStubAddress, const CEStubAddress &> & out_listStubs) const
{
    mServiceRegistry.GetRemoteServiceList(out_listStubs);
}

void CEServerService::GetRemoteServiceList(TEArrayList<CEProxyAddress, const CEProxyAddress &> & out_lisProxies) const
{
    mServiceRegistry.GetRemoteServiceList(out_lisProxies);
}

void CEServerService::GetServiceList(ITEM_ID cookie, TEArrayList<CEStubAddress, const CEStubAddress &> out_listStubs) const
{
    mServiceRegistry.GetServiceList(cookie, out_listStubs);
}

void CEServerService::GetServiceList(ITEM_ID cookie, TEArrayList<CEProxyAddress, const CEProxyAddress &> out_lisProxies) const
{
    mServiceRegistry.GetServiceList(cookie, out_lisProxies);
}

void CEServerService::RegisterRemoteStub(const CEStubAddress & stub)
{
    TRACE_SCOPE(areg_ipc_private_CEServerService_RegisterRemoteStub);
    ASSERT(stub.IsServiceRemote());

    TRACE_DBG("Going to register remote stub [ %s ]", CEStubAddress::ConvertAddressToPath(stub).GetBuffer());
    if ( mServiceRegistry.GetServiceStatus(stub) != NEService::ServiceConnected )
    {
        CEListServiceProxies listProxies;
        const CEServiceStub & stubService = mServiceRegistry.RegisterServiceStub(stub, listProxies);
        if ( stubService.GetServiceStatus() == NEService::ServiceConnected && listProxies.IsEmpty() == false )
        {
            TRACE_DBG("Stub [ %s ] is connected, sending notification messages to [ %d ] waiting proxies", CEStubAddress::ConvertAddressToPath(stubService.GetServiceAddress()).GetBuffer(), listProxies.GetSize());
            TEArrayList<ITEM_ID> sendList;

            for ( LISTPOS pos = listProxies.GetHeadPosition(); pos != NULL; pos = listProxies.GetNextPosition(pos) )
            {
                const CEServiceProxy & proxyService = listProxies.GetAt(pos);
                const CEProxyAddress & addrProxy    = proxyService.GetServiceAddress();
                if ( (proxyService.GetServiceStatus() == NEService::ServiceConnected) && (addrProxy.GetCookie() != stub.GetCookie()) )
                {
                    CERemoteMessage msgRegisterProxy = NEConnection::CreateServiceClientRegisteredNotification(addrProxy, stub.GetSource());

                    TRACE_DBG("Sending [ %s ] proxy registration notification, stub [ %s ] on target [ %p ] is connected. Created message [ %s ] ( ID = %p ) from source [ %p ]"
                                    , CEProxyAddress::ConvertAddressToPath(addrProxy).GetBuffer()
                                    , CEStubAddress::ConvertAddressToPath(stub).GetBuffer()
                                    , msgRegisterProxy.GetTarget()
                                    , NEService::GetString( static_cast<NEService::eFuncIdRange>(msgRegisterProxy.GetMessageId()))
                                    , msgRegisterProxy.GetMessageId()
                                    , msgRegisterProxy.GetSource());

                    CESendMessageEvent::SendEvent( CESendMessageEventData(msgRegisterProxy), static_cast<IESendMessageEventConsumer &>(mThreadSend), static_cast<CEDispatcherThread &>(mThreadSend));

                    if (sendList.FindElement(addrProxy.GetSource()) < 0 )
                    {
                        sendList.Add(addrProxy.GetSource());
                        CERemoteMessage msgRegisterStub  = NEConnection::CreateServiceRegisteredNotification(stub, addrProxy.GetSource());

                        TRACE_DBG("Sending [ %s ] stub registration notification, proxy [ %s ] on target [ %p ] is waiting. Created message [ %s ] ( ID = %p ) from source [ %p ]"
                                        , CEStubAddress::ConvertAddressToPath(stub).GetBuffer()
                                        , CEProxyAddress::ConvertAddressToPath(addrProxy).GetBuffer()
                                        , msgRegisterStub.GetTarget()
                                        , NEService::GetString( static_cast<NEService::eFuncIdRange>(msgRegisterStub.GetMessageId()))
                                        , msgRegisterStub.GetMessageId()
                                        , msgRegisterStub.GetSource());

                        CESendMessageEvent::SendEvent( CESendMessageEventData(msgRegisterStub), static_cast<IESendMessageEventConsumer &>(mThreadSend), static_cast<CEDispatcherThread &>(mThreadSend));
                    }
                    else
                    {
                        // ignore, it already has registered stub
                        TRACE_DBG("Ignoring sending stub registration message to target [ %p ], target already notified", addrProxy.GetSource());
                    }
                }
                else
                {
                     // ignore, it already has registered stub locally or proxy is not connected
                    TRACE_DBG("ignoring sending stub registration message, Stub [ %s ] and Proxy [ %s ] have same origin.", CEStubAddress::ConvertAddressToPath(stub).GetBuffer(), CEProxyAddress::ConvertAddressToPath(addrProxy).GetBuffer());
                }
            }
        }
        else
        {
            // ignore, stub is not connected or list is empty
            TRACE_DBG("Ignoring sending stub [ %s ] connected notification, the proxy list is empty [ %s ]", CEStubAddress::ConvertAddressToPath(stub).GetBuffer(), listProxies.IsEmpty() ? "YES" : "NO");
        }
    }
    else
    {
        // ignore, stub is already connected
        TRACE_DBG("Stub [ %s ] is already marked as connected, ignoring registration", CEStubAddress::ConvertAddressToPath(stub).String());
    }
}

void CEServerService::RegisterRemoteProxy(const CEProxyAddress & proxy)
{
    TRACE_SCOPE(areg_ipc_private_CEServerService_RegisterRemoteProxy);
    if ( mServiceRegistry.GetServiceStatus(proxy) != NEService::ServiceConnected )
    {
        CEServiceProxy proxyService;
        const CEServiceStub & stubService   = mServiceRegistry.RegisterServiceProxy(proxy, proxyService);
        const CEStubAddress & addrStub      = stubService.GetServiceAddress();

        TRACE_DBG("Registered proxy [ %s ], for connection with stub [ %s ], connection status is [ %s ]"
                    , CEProxyAddress::ConvertAddressToPath(proxy).GetBuffer()
                    , CEStubAddress::ConvertAddressToPath(addrStub).GetBuffer()
                    , NEService::GetString( proxyService.GetServiceStatus()));

        if ( (proxyService.GetServiceStatus() == NEService::ServiceConnected) && (proxy.GetCookie() != addrStub.GetCookie()) )
        {
            CERemoteMessage msgRegisterProxy = NEConnection::CreateServiceClientRegisteredNotification(proxy, addrStub.GetSource());

            TRACE_DBG("Sending [ %s ] proxy registration notification, stub [ %s ] on target [ %p ] is connected. Created message [ %s ] ( ID = %p ) from source [ %p ]"
                        , CEProxyAddress::ConvertAddressToPath(proxy).GetBuffer()
                        , CEStubAddress::ConvertAddressToPath(addrStub).GetBuffer()
                        , msgRegisterProxy.GetTarget()
                        , NEService::GetString( static_cast<NEService::eFuncIdRange>(msgRegisterProxy.GetMessageId()))
                        , msgRegisterProxy.GetMessageId()
                        , msgRegisterProxy.GetSource());

            CESendMessageEvent::SendEvent( CESendMessageEventData(msgRegisterProxy), static_cast<IESendMessageEventConsumer &>(mThreadSend), static_cast<CEDispatcherThread &>(mThreadSend));

            CERemoteMessage msgRegisterStub  = NEConnection::CreateServiceRegisteredNotification(addrStub, proxy.GetSource());

            TRACE_DBG("Sending [ %s ] stub registration notification, proxy [ %s ] on target [ %p ] is waiting. Created message [ %s ] ( ID = %p ) from source [ %p ]"
                        , CEStubAddress::ConvertAddressToPath(addrStub).GetBuffer()
                        , CEProxyAddress::ConvertAddressToPath(proxy).GetBuffer()
                        , msgRegisterStub.GetTarget()
                        , NEService::GetString( static_cast<NEService::eFuncIdRange>(msgRegisterStub.GetMessageId()))
                        , msgRegisterStub.GetMessageId()
                        , msgRegisterStub.GetSource());

            CESendMessageEvent::SendEvent( CESendMessageEventData(msgRegisterStub), static_cast<IESendMessageEventConsumer &>(mThreadSend), static_cast<CEDispatcherThread &>(mThreadSend));
        }
        else
        {
            // ignore, it is done locally
            TRACE_DBG("ignoring sending stub registration message, Stub [ %s ] and Proxy [ %s ] have same origin.", CEStubAddress::ConvertAddressToPath(addrStub).GetBuffer(), CEProxyAddress::ConvertAddressToPath(proxy).GetBuffer());
        }
    }
    else
    {
        TRACE_DBG("Proxy [ %s ] is already having connected status, ignoring registration", CEProxyAddress::ConvertAddressToPath(proxy).GetBuffer());
    }
}

void CEServerService::UnregisterRemoteStub(const CEStubAddress & stub)
{
    TRACE_SCOPE(areg_ipc_private_CEServerService_UnregisterRemoteStub);
    if ( mServiceRegistry.GetServiceStatus(stub) == NEService::ServiceConnected )
    {
        CEListServiceProxies listProxies;
        const CEServiceStub & stubService = mServiceRegistry.UnregisterServiceStub(stub, listProxies);
        TRACE_DBG("Unregistered stub [ %s ], [ %d ] proxies are going to be notified", CEStubAddress::ConvertAddressToPath(stub).GetBuffer(), listProxies.GetSize());

        if ( stubService.GetServiceStatus() != NEService::ServiceConnected && listProxies.IsEmpty() == false )
        {
            TEArrayList<ITEM_ID> sendList;

            for ( LISTPOS pos = listProxies.GetHeadPosition(); pos != NULL; pos = listProxies.GetNextPosition(pos) )
            {
                const CEServiceProxy & proxyService = listProxies.GetAt(pos);
                const CEProxyAddress & addrProxy    = proxyService.GetServiceAddress();
                if ( (proxyService.GetServiceStatus() != NEService::ServiceConnected) && (addrProxy.GetCookie() != stub.GetCookie()) )
                {
                    // no need to send message to unregistered stub, only to proxy side
                    if (sendList.FindElement(addrProxy.GetSource()) < 0 )
                    {
                        sendList.Add(addrProxy.GetSource());

                        CERemoteMessage msgRegisterStub = NEConnection::CreateServiceUnregisteredNotification( stub, addrProxy.GetSource( ) );
                        TRACE_DBG("Sending [ %s ] stub unregistered notification, proxy [ %s ] on target [ %p ] was connected. Created message [ %s ] ( ID = %p ) from source [ %p ]"
                                    , CEStubAddress::ConvertAddressToPath(stub).GetBuffer()
                                    , CEProxyAddress::ConvertAddressToPath(addrProxy).GetBuffer()
                                    , msgRegisterStub.GetTarget()
                                    , NEService::GetString( static_cast<NEService::eFuncIdRange>(msgRegisterStub.GetMessageId()))
                                    , msgRegisterStub.GetMessageId()
                                    , msgRegisterStub.GetSource());

                        CESendMessageEvent::SendEvent( CESendMessageEventData(msgRegisterStub), static_cast<IESendMessageEventConsumer &>(mThreadSend), static_cast<CEDispatcherThread &>(mThreadSend));
                    }
                    else
                    {
                        // ignore, it already has unregistered stub
                        TRACE_DBG("ignoring sending stub unregistered message, proxy [ %s ] on target [ %p ] was already notified"
                                        , CEProxyAddress::ConvertAddressToPath(addrProxy)
                                        , addrProxy.GetSource());
                    }
                }
                else
                {
                    // ignore, it already has unregistered stub locally or proxy status did not changed
                    TRACE_DBG("Ignore sending stub unregistered notification message, stub [ %s ] and proxy [ %s ] have same origin"
                                    , CEStubAddress::ConvertAddressToPath(stub)
                                    , CEProxyAddress::ConvertAddressToPath(addrProxy));
                }
            }
        }
        else
        {
            // ignore, stub is status did not change or list is empty
            TRACE_DBG("Ignore sending stub unregistered message, the proxy list is empty");
        }
    }
    else
    {
        // ignore, stub is already disconnected
        TRACE_DBG("Ignore unregistering stub [ %s ], it is already unregistered", CEStubAddress::ConvertAddressToPath(stub));
    }
}

void CEServerService::UnregisterRemoteProxy(const CEProxyAddress & proxy)
{
    TRACE_SCOPE(areg_ipc_private_CEServerService_UnregisterRemoteProxy);
    CEServiceProxy proxyService;
    const CEServiceStub & stubService   = mServiceRegistry.UnregisterServiceProxy(proxy, proxyService);
    const CEStubAddress & addrStub      = stubService.GetServiceAddress();

    if ( (proxyService.GetServiceStatus() == NEService::ServiceConnected) && (proxy.GetCookie() != addrStub.GetCookie()) )
    {
        // no need to send message to proxy side, it is already unregistered
        // CERemoteMessage msgRegisterProxy = NEConnection::CreateServiceClientRegisteredNotification(proxy, addrStub.GetSource());
        CERemoteMessage msgRegisterProxy = NEConnection::CreateServiceClientUnregisteredNotification(proxy, addrStub.GetSource());
        TRACE_DBG("Sending [ %s ] proxy unregistered notification, stub [ %s ] on target [ %p ] is connected. Created message [ %s ] ( ID = %p ) from source [ %p ]"
                    , CEProxyAddress::ConvertAddressToPath(proxy).GetBuffer()
                    , CEStubAddress::ConvertAddressToPath(addrStub).GetBuffer()
                    , msgRegisterProxy.GetTarget()
                    , NEService::GetString( static_cast<NEService::eFuncIdRange>(msgRegisterProxy.GetMessageId()))
                    , msgRegisterProxy.GetMessageId()
                    , msgRegisterProxy.GetSource());

        CESendMessageEvent::SendEvent( CESendMessageEventData(msgRegisterProxy), static_cast<IESendMessageEventConsumer &>(mThreadSend), static_cast<CEDispatcherThread &>(mThreadSend));
    }
    else
    {
        // ignore, it is done locally
        TRACE_DBG("Ignore sending proxy unregistered notification message, stub [ %s ] and proxy [ %s ] have same origin", CEStubAddress::ConvertAddressToPath(addrStub).GetBuffer(), CEProxyAddress::ConvertAddressToPath(proxy).GetBuffer());
    }
}

void CEServerService::RemoteServiceStarted(const CEChannel & /* channel */)
{

}

void CEServerService::RemoteServiceStopped(const CEChannel & /* channel */)
{

}

void CEServerService::RemoveServiceLostConnection(const CEChannel & /* channel */)
{
}

void CEServerService::FailedSendMessage(const CERemoteMessage & /* msgFailed */)
{

}

void CEServerService::FailedReceiveMessage(SOCKETHANDLE /* whichSource */)
{

}

void CEServerService::FailedProcessMessage(const CERemoteMessage & /* msgUnprocessed */)
{

}

void CEServerService::ProcessReceivedMessage(const CERemoteMessage & msgReceived, const NESocket::CEInterlockedValue & addrHost, SOCKETHANDLE whichSource)
{
    TRACE_SCOPE(areg_ipc_private_CEServerService_ProcessReceivedMessage);
    if ( msgReceived.IsValid() )
    {
        ITEM_ID cookie = mServerConnection.GetCookie(whichSource);
        ITEM_ID source = msgReceived.GetSource();
        ITEM_ID target = msgReceived.GetTarget();
        NEService::eFuncIdRange msgId  = static_cast<NEService::eFuncIdRange>( msgReceived.GetMessageId() );

        TRACE_DBG("Received message [ %s ] ( ID = %p ) from source [ %p ] ( connection cookie = %p ) of client host [ %s : %d ] for target [ %p ]"
                        , NEService::GetString(msgId)
                        , msgId
                        , source
                        , cookie
                        , addrHost.GetHostAddress().GetBuffer()
                        , addrHost.GetHostPort()
                        , target);

        if ( (source > NEService::COOKIE_ROUTER) && NEService::IsExecutableId(msgId) )
        {
            TRACE_DBG("Forwarding message [ %p ] to send to target [ %p ]", msgId, target);
            if ( target != NEService::TARGET_UNKNOWN )
                CESendMessageEvent::SendEvent( CESendMessageEventData(msgReceived), static_cast<IESendMessageEventConsumer &>(mThreadSend), static_cast<CEDispatcherThread &>(mThreadSend));
        }
        else if ( (source == cookie) && (msgId != NEService::SI_ROUTER_CONNECT) )
        {
            TRACE_DBG("Going to process received message [ %p ]", msgId);
            CEServerServiceEvent::SendEvent( CEServerServiceEventData(CEServerServiceEventData::CMD_ServiceReceivedMsg, msgReceived), static_cast<IEServerServiceEventConsumer &>(self()), static_cast<CEDispatcherThread &>(self()) );
        }
        else if ( (source == NEService::SOURCE_UNKNOWN) && (msgId == NEService::SI_ROUTER_CONNECT) )
        {
            CERemoteMessage msgConnect = NEConnection::CreateConnectNotify(cookie);
            TRACE_DBG("Received request connect message, sending response [ %s ] ( ID = %p ), to new target [ %p ], connection socket [ %p ], checksum [ %u ]"
                        , NEService::GetString( static_cast<NEService::eFuncIdRange>(msgConnect.GetMessageId()))
                        , msgConnect.GetMessageId()
                        , msgConnect.GetTarget()
                        , whichSource
                        , msgConnect.GetChecksum());

            if ( msgConnect.IsValid() )
                CESendMessageEvent::SendEvent( msgConnect, static_cast<IESendMessageEventConsumer &>(mThreadSend), static_cast<CEDispatcherThread &>(mThreadSend));
        }
        else
        {
            TRACE_WARN("Ignoring to process message [ %s ] ( ID = %p ) from source [ %p ]", NEService::GetString(msgId), msgId, source);
            ASSERT(false);
        }
    }
    else
    {
        TRACE_WARN("Received invalid message from source [ %p ], ignoring to process", msgReceived.GetSource());
    }
}

bool CEServerService::RunDispatcher(void)
{
    CEServerServiceEvent::AddListener( static_cast<IEServerServiceEventConsumer &>(self()), static_cast<CEDispatcherThread &>(self()) );
    CEServerServiceEvent::SendEvent( CEServerServiceEventData(CEServerServiceEventData::CMD_StartService), static_cast<IEServerServiceEventConsumer &>(self()), static_cast<CEDispatcherThread &>(self()) );

    bool result = CEDispatcherThread::RunDispatcher();

    CEServerServiceEvent::RemoveListener( static_cast<IEServerServiceEventConsumer &>(self()), static_cast<CEDispatcherThread &>(self()) );

    return result;
}

bool CEServerService::PostEvent(CEEvent & eventElem)
{
    return CEEventDispatcher::PostEvent(eventElem);
}
