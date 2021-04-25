/************************************************************************
 * \file        areg/src/component/private/CEServiceManager.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Service Manager implementation.
 *
 ************************************************************************/
#include "areg/src/component/private/CEServiceManager.hpp"

#include "areg/src/component/CEProxyAddress.hpp"
#include "areg/src/component/CEStubAddress.hpp"
#include "areg/src/component/CEServiceRequestEvent.hpp"
#include "areg/src/component/CEServiceResponseEvent.hpp"
#include "areg/src/component/private/CEProxyConnectEvent.hpp"
#include "areg/src/component/private/CEStubConnectEvent.hpp"
#include "areg/src/base/CEProcess.hpp"

#include "areg/src/trace/GETrace.h"
DEF_TRACE_SCOPE(areg_component_private_CEServiceManager_ProcessEvent);
DEF_TRACE_SCOPE(areg_component_private_CEServiceManager_RegisterServer);
DEF_TRACE_SCOPE(areg_component_private_CEServiceManager_UnregisterServer);
DEF_TRACE_SCOPE(areg_component_private_CEServiceManager_RegisterClient);
DEF_TRACE_SCOPE(areg_component_private_CEServiceManager_UnregisterClient);
DEF_TRACE_SCOPE(areg_component_private_CEServiceManager_SendClientConnectedEvent);
DEF_TRACE_SCOPE(areg_component_private_CEServiceManager_SendClientDisconnectedEvent);

//////////////////////////////////////////////////////////////////////////
// CEServiceManager class Implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Implement Runtime
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME(CEServiceManager, CEDispatcherThread)

//////////////////////////////////////////////////////////////////////////
// Constants and types
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Service Manager Thread name.
 **/
const char* const   CEServiceManager::SERVICE_MANAGER_THREAD_NAME   = "_AREG_SERVICE_MANAGER_THREAD_";

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////

CEServiceManager & CEServiceManager::GetServiceManager( void )
{
    static CEServiceManager  _ServiceManager;
    return _ServiceManager;
}

bool CEServiceManager::StartServiceManager( void )
{
    OUTPUT_DBG("Starting Service Manager");
    return GetServiceManager().StartServiceManagerThread( );
}

void CEServiceManager::StopServiceManager( void )
{
    OUTPUT_DBG( "Stopping Service Manager" );
    GetServiceManager().StopServiceManagerThread();
}

bool CEServiceManager::IsServiceManagerStarted( void )
{
    bool result = false;
    CEServiceManager & ServiceManager = GetServiceManager();
    do 
    {
        CELock lock(ServiceManager.mLock);
        result = ServiceManager.IsReady();
    } while (false);

    return result;
}

void CEServiceManager::RequestRegisterServer( const CEStubAddress & whichServer )
{
    OUTPUT_DBG("Request to register server [ %s ] of interface [ %s ]", whichServer.GetRoleName().GetBuffer(), whichServer.GetServiceName().GetBuffer());
    ASSERT(whichServer.IsValid());
    CEServiceManager & ServiceManager = CEServiceManager::GetServiceManager();
    CEServiceManagerEvent::SendEvent( CEServiceManagerEventData::RegisterStub(whichServer), static_cast<IEServiceManagerEventConsumer &>(ServiceManager) , static_cast<CEDispatcherThread &>(ServiceManager));
}

void CEServiceManager::RequestUnregisterServer( const CEStubAddress & whichServer )
{
    OUTPUT_DBG( "Request to unregister server [ %s ] of interface [ %s ]", whichServer.GetRoleName( ).GetBuffer( ), whichServer.GetServiceName( ).GetBuffer( ) );
    ASSERT(whichServer.IsValid());
    CEServiceManager & ServiceManager = CEServiceManager::GetServiceManager();
    CEServiceManagerEvent::SendEvent( CEServiceManagerEventData::UnregisterStub(whichServer), static_cast<IEServiceManagerEventConsumer &>(ServiceManager) , static_cast<CEDispatcherThread &>(ServiceManager));
}

void CEServiceManager::RequestRegisterClient( const CEProxyAddress & whichClient )
{
    OUTPUT_DBG( "Request to register proxy [ %s ] of interface [ %s ]", whichClient.GetRoleName( ).GetBuffer( ), whichClient.GetServiceName( ).GetBuffer( ) );
    ASSERT(whichClient.IsValid());
    CEServiceManager & ServiceManager = CEServiceManager::GetServiceManager();
    CEServiceManagerEvent::SendEvent( CEServiceManagerEventData::RegisterProxy(whichClient), static_cast<IEServiceManagerEventConsumer &>(ServiceManager) , static_cast<CEDispatcherThread &>(ServiceManager));
}

void CEServiceManager::RequestUnregisterClient( const CEProxyAddress & whichClient )
{
    OUTPUT_DBG( "Request to register proxy [ %s ] of interface [ %s ]", whichClient.GetRoleName( ).GetBuffer( ), whichClient.GetServiceName( ).GetBuffer( ) );
    ASSERT(whichClient.IsValid());
    CEServiceManager & ServiceManager = CEServiceManager::GetServiceManager();
    CEServiceManagerEvent::SendEvent( CEServiceManagerEventData::UnregisterProxy(whichClient), static_cast<IEServiceManagerEventConsumer &>(ServiceManager) , static_cast<CEDispatcherThread &>(ServiceManager));
}

bool CEServiceManager::RoutingServiceClientConfigure( const char * configFile /*= NULL*/ )
{
    CEServiceManager & ServiceManager = CEServiceManager::GetServiceManager();
    return CEServiceManagerEvent::SendEvent( CEServiceManagerEventData::ConfigureConnection(CEString(configFile)), static_cast<IEServiceManagerEventConsumer &>(ServiceManager) , static_cast<CEDispatcherThread &>(ServiceManager));
}

bool CEServiceManager::RoutingServiceClientStart(const char * configFile /*= NULL */)
{
    CEServiceManager & ServiceManager = CEServiceManager::GetServiceManager();
    return CEServiceManagerEvent::SendEvent( CEServiceManagerEventData::StartConnection(CEString(configFile)), static_cast<IEServiceManagerEventConsumer &>(ServiceManager) , static_cast<CEDispatcherThread &>(ServiceManager));
}

bool CEServiceManager::RoutingServiceClientStart( const char * ipAddress, unsigned short portNr )
{
    bool result = false;
    if ( (NEString::isEmpty<char>(ipAddress) == false) && (portNr != NESocket::InvalidPort) )
    {
        CEServiceManager & ServiceManager = CEServiceManager::GetServiceManager( );
        result =CEServiceManagerEvent::SendEvent( CEServiceManagerEventData::StartNetConnection( CEString( ipAddress ), portNr ), static_cast<IEServiceManagerEventConsumer &>(ServiceManager), static_cast<CEDispatcherThread &>(ServiceManager) );
    }
    return result;
}

void CEServiceManager::RoutingServiceClientStop(void)
{
    CEServiceManager & ServiceManager = CEServiceManager::GetServiceManager();
    CEServiceManagerEvent::SendEvent( CEServiceManagerEventData::StopConnection(), static_cast<IEServiceManagerEventConsumer &>(ServiceManager) , static_cast<CEDispatcherThread &>(ServiceManager));
}

void CEServiceManager::RoutingServiceClientEnable( bool enable )
{
    CEServiceManager & ServiceManager = CEServiceManager::GetServiceManager( );
    CEServiceManagerEvent::SendEvent( CEServiceManagerEventData::EnableRemoteService(enable), static_cast<IEServiceManagerEventConsumer &>(ServiceManager), static_cast<CEDispatcherThread &>(ServiceManager) );
}

bool CEServiceManager::IsRoutingServiceClientStarted(void)
{
    return CEServiceManager::GetServiceManager().mConnectService.IsServiceStarted();
}

bool CEServiceManager::IsRoutingServiceClientConfigured(void)
{
    return CEServiceManager::GetServiceManager().mConnectService.IsServiceConfigured();
}

bool CEServiceManager::IsRoutingServiceEnabled(void)
{
    return CEServiceManager::GetServiceManager().mConnectService.IsServiceEnabled();
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CEServiceManager::CEServiceManager( void )
    : CEDispatcherThread            ( CEServiceManager::SERVICE_MANAGER_THREAD_NAME )
    , IEServiceManagerEventConsumer ( )
    , IERemoteServiceConsumer      ( )

    , mServerList       ( )
    , mConnectService   ( static_cast<IERemoteServiceConsumer &>(self()) )
    , mLock             (  )
{
    ; // do nothing
}

CEServiceManager::~CEServiceManager( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////
void CEServiceManager::RegisterServer( const CEStubAddress & whichServer )
{
    TRACE_SCOPE(areg_component_private_CEServiceManager_RegisterServer);

    if ( whichServer.IsLocalAddress() && whichServer.IsServiceRemote() )
        mConnectService.RegisterService(whichServer);

    CEClientList clientList;

    DECLARE_TRACE_VARIABLE(const CEServerInfo &, server, mServerList.RegisterServer(whichServer, clientList));
    TRACE_DBG("Server [ %s ] is registered. Connection status [ %s ], there are [ %d ] waiting clients"
                , CEStubAddress::ConvertAddressToPath(server.GetAddress()).GetBuffer()
                , NEService::GetString(server.GetConnectionState())
                , clientList.GetSize());

    for ( LISTPOS pos = clientList.GetHeadPosition(); pos != NULL; pos = clientList.GetNextPosition(pos) )
        SendClientConnectedEvent( clientList.GetAt(pos), whichServer );
}

void CEServiceManager::UnregisterServer( const CEStubAddress & whichServer )
{
    TRACE_SCOPE(areg_component_private_CEServiceManager_UnregisterServer);

    if ( whichServer.IsLocalAddress() && whichServer.IsServiceRemote() )
        mConnectService.UnregisterService(whichServer);

    CEClientList clientList;
    CEServerInfo server = mServerList.UnregisterServer(whichServer, clientList);

    TRACE_DBG("Server [ %s ] is unregistered. Connection status [ %s ], there are [ %d ] waiting clients"
                , CEStubAddress::ConvertAddressToPath(server.GetAddress()).GetBuffer()
                , NEService::GetString(server.GetConnectionState())
                , clientList.GetSize());

    for ( LISTPOS pos = clientList.GetHeadPosition(); pos != NULL; pos = clientList.GetNextPosition(pos))
        SendClientDisconnectedEvent(clientList.GetAt(pos), whichServer);
}

void CEServiceManager::RegisterClient( const CEProxyAddress & whichClient )
{
    TRACE_SCOPE(areg_component_private_CEServiceManager_RegisterClient);

    if ( whichClient.IsLocalAddress() && whichClient.IsServiceRemote() )
        mConnectService.RegisterServiceClient(whichClient);

    CEClientInfo client;
    const CEServerInfo & server = mServerList.RegisterClient(whichClient, client);

    TRACE_DBG("Client [ %s ] is registered for server [ %s ], connection status [ %s ]"
                , CEProxyAddress::ConvertAddressToPath(client.GetAddress()).GetBuffer()
                , CEStubAddress::ConvertAddressToPath(server.GetAddress()).GetBuffer()
                , NEService::GetString(client.GetConnectionState()));

    SendClientConnectedEvent( client, server.GetAddress() );
}

void CEServiceManager::UnregisterClient( const CEProxyAddress & whichClient )
{
    TRACE_SCOPE(areg_component_private_CEServiceManager_UnregisterClient);

    if ( whichClient.IsLocalAddress() && whichClient.IsServiceRemote() )
        mConnectService.UnregisterServiceClient(whichClient);

    CEClientInfo client;
    CEServerInfo server = mServerList.UnregisterClient(whichClient, client);

    TRACE_DBG("Client [ %s ] is unregistered from server [ %s ], connection status [ %s ]"
                , CEProxyAddress::ConvertAddressToPath(client.GetAddress()).GetBuffer()
                , CEStubAddress::ConvertAddressToPath(server.GetAddress()).GetBuffer()
                , NEService::GetString(client.GetConnectionState()));

    // Unregister client first, then send event that client does not receive notification
    SendClientDisconnectedEvent(client, server.GetAddress() );
}

void CEServiceManager::SendClientConnectedEvent( const CEClientInfo & client, const CEStubAddress & addrStub ) const
{
    TRACE_SCOPE(areg_component_private_CEServiceManager_SendClientConnectedEvent);

    if ( client.IsConnected() )
    {
        const CEProxyAddress & addrProxy = client.GetAddress();
        if ( addrStub.IsLocalAddress() && addrStub.GetSource() != NEService::SOURCE_UNKNOWN)
        {
            TRACE_DBG("Sending to Stub [ %s ] notification of connected client [ %s ]", CEStubAddress::ConvertAddressToPath(addrStub).GetBuffer(), CEProxyAddress::ConvertAddressToPath(addrProxy).GetBuffer());
            CEStubConnectEvent * clientConnect  = DEBUG_NEW CEStubConnectEvent(addrProxy, addrStub, NEService::ServiceConnected);
            if ( clientConnect != NULL )
                addrStub.DeliverServiceEvent(*clientConnect);
        }

        if ( addrProxy.IsLocalAddress() && addrProxy.GetSource() != NEService::SOURCE_UNKNOWN )
        {
            TRACE_DBG("Sending to Proxy [ %s ] notification of connection to server [ %s ]", CEProxyAddress::ConvertAddressToPath(addrProxy).GetBuffer(), CEStubAddress::ConvertAddressToPath(addrStub).GetBuffer());
            CEProxyConnectEvent * proxyConnect  = DEBUG_NEW CEProxyConnectEvent(addrProxy, addrStub, NEService::ServiceConnected);
            if ( proxyConnect != NULL )
                addrProxy.DeliverServiceEvent(*proxyConnect);
        }
    }
}

void CEServiceManager::SendClientDisconnectedEvent( const CEClientInfo & client, const CEStubAddress & addrStub ) const
{
    TRACE_SCOPE(areg_component_private_CEServiceManager_SendClientDisconnectedEvent);

    if ( client.IsWaiting() )
    {
        const CEProxyAddress & addrProxy = client.GetAddress();
        if ( addrStub.IsLocalAddress() && addrStub.GetSource() != NEService::SOURCE_UNKNOWN)
        {
            TRACE_DBG("Sending to Stub [ %s ] notification of disconnected client [ %s ]", CEStubAddress::ConvertAddressToPath(addrStub).GetBuffer(), CEProxyAddress::ConvertAddressToPath(addrProxy).GetBuffer());
            CEStubConnectEvent * clientConnect  = DEBUG_NEW CEStubConnectEvent(addrProxy, addrStub, NEService::ServiceDisconnected);
            if ( clientConnect != NULL )
                addrStub.DeliverServiceEvent(*clientConnect);
        }

        if ( addrProxy.IsLocalAddress() )
        {
            TRACE_DBG("Sending to Proxy [ %s ] notification of disconnection from server [ %s ]", CEProxyAddress::ConvertAddressToPath(addrProxy).GetBuffer(), CEStubAddress::ConvertAddressToPath(addrStub).GetBuffer());
            CEProxyConnectEvent * proxyConnect  = DEBUG_NEW CEProxyConnectEvent(addrProxy, addrStub, NEService::ServiceDisconnected);
            if ( proxyConnect != NULL )
                addrProxy.DeliverServiceEvent(*proxyConnect);
        }
    }
}

void CEServiceManager::ProcessEvent( const CEServiceManagerEventData & data )
{
    TRACE_SCOPE(areg_component_private_CEServiceManager_ProcessEvent);
    CEServiceManagerEventData::eServiceManagerCommands cmdService = data.GetCommand();
    const IEInStream & stream = data.GetReadStream();

    TRACE_DBG("Service Manager is going to execute command [ %s ]", CEServiceManagerEventData::GetString(cmdService));

    switch ( cmdService )
    {
    case CEServiceManagerEventData::CMD_StopRoutingClient:
        {
            for ( MAPPOS pos = mServerList.GetStartPosition(); pos != NULL; pos = mServerList.GetNextPosition(pos) )
            {
                CEServerInfo si;
                CEClientList cList;

                mServerList.GetAt(pos, si, cList);
                for ( LISTPOS pos = cList.GetHeadPosition(); pos != NULL; pos = cList.GetNextPosition(pos))
                {
                    const CEClientInfo & client = cList.GetAt(pos);
                    SendClientDisconnectedEvent(client, si.GetAddress());
                }
            }

            mServerList.RemoveAll();
            mConnectService.StopRemotingService();
            CEDispatcherThread::RemoveEvents(false);
            PulseExit();
        }
        break;

    case CEServiceManagerEventData::CMD_RegisterProxy:
        {
            CEProxyAddress  addrProxy;
            CEChannel       channel;
            stream >> addrProxy;
            stream >> channel;
            addrProxy.SetChannel(channel);
            RegisterClient(addrProxy);
        }
        break;

    case CEServiceManagerEventData::CMD_UnregisterProxy:
        {
            CEProxyAddress  addrProxy;
            CEChannel       channel;
            stream >> addrProxy;
            stream >> channel;
            addrProxy.SetChannel(channel);
            UnregisterClient(addrProxy);
        }
        break;

    case CEServiceManagerEventData::CMD_RegisterStub:
        {
            CEStubAddress   addrstub;
            CEChannel       channel;
            stream >> addrstub;
            stream >> channel;
            addrstub.SetChannel(channel);
            RegisterServer(addrstub);
        }
        break;

    case CEServiceManagerEventData::CMD_UnregisterStub:
        {
            CEStubAddress   addrstub;
            CEChannel       channel;
            stream >> addrstub;
            stream >> channel;
            addrstub.SetChannel(channel);
            UnregisterServer(addrstub);
        }
        break;

    case CEServiceManagerEventData::CMD_ConfigureConnection:
        {
            CEString   configFile;
            stream >> configFile;
            mConnectService.EnableService(true);
            if ( NEString::isEmpty<char>(configFile) == false )
                mConnectService.ServiceConfigure(configFile);
            else if (mConnectService.IsServiceConfigured() == false)
                mConnectService.ServiceConfigure(NULL);
        }
        break;

    case CEServiceManagerEventData::CMD_StartConnection:
        {
            CEString   configFile;
            stream >> configFile;
            bool isConfigured = false;
            mConnectService.EnableService( true );
            if ( NEString::isEmpty<char>(configFile) == false )
                isConfigured = mConnectService.ServiceConfigure(configFile);
            else if (mConnectService.IsServiceConfigured() == false)
                isConfigured = mConnectService.ServiceConfigure(NULL);
            else 
                isConfigured = true;
            if ( isConfigured )
                mConnectService.StartRemotingService();
        }
        break;

    case CEServiceManagerEventData::CMD_StartNetConnection:
        {
            CEString   ipAddress;
            unsigned short portNr = 0;
            stream >> ipAddress;
            stream >> portNr;

            mConnectService.EnableService( true );
            mConnectService.SetRouterServiceAddress(ipAddress, portNr);
            if ( mConnectService.IsServiceConfigured() )
                mConnectService.StartRemotingService( );
        }
        break;

    case CEServiceManagerEventData::CMD_StopConnection:
        {
            mConnectService.StopRemotingService();
        }
        break;

    case CEServiceManagerEventData::CMD_SetEnableService:
        {
            bool enable = false;
            stream >> enable;
            mConnectService.EnableService(enable);
        }
        break;

    case CEServiceManagerEventData::CMD_RegisterConnection:
        {
            for ( MAPPOS posMap = mServerList.GetStartPosition(); posMap != NULL; posMap = mServerList.GetNextPosition(posMap) )
            {
                const CEStubAddress & server = mServerList.GetKeyAt(posMap).GetAddress();
                const CEClientList & listClients = mServerList.GetValueAt(posMap);

                if ( server.IsServiceRemote() && server.IsLocalAddress() && server.IsValid() )
                    mConnectService.RegisterService(server);

                for ( LISTPOS posList = listClients.GetHeadPosition(); posList != NULL; posList = listClients.GetNextPosition(posList) )
                {
                    const CEProxyAddress & proxy = listClients.GetAt(posList).GetAddress();
                    if ( proxy.IsServiceRemote() && proxy.IsLocalAddress() && proxy.IsValid() )
                        mConnectService.RegisterServiceClient(proxy);
                }
            }
        }
        break;

    case CEServiceManagerEventData::CMD_UnregisterConnection:
    case CEServiceManagerEventData::CMD_LostConnection:
        {
            TEArrayList<CEStubAddress, const CEStubAddress &> stubList;
            TEArrayList<CEProxyAddress, const CEProxyAddress &> proxyList;
            for ( MAPPOS posMap = mServerList.GetStartPosition(); posMap != NULL; posMap = mServerList.GetNextPosition(posMap) )
            {
                const CEStubAddress & server = mServerList.GetKeyAt(posMap).GetAddress();
                const CEClientList & listClients = mServerList.GetValueAt(posMap);

                if ( server.IsServiceRemote() && server.IsRemoteAddress() && server.IsValid() )
                    stubList.Add(server);

                for ( LISTPOS posList = listClients.GetHeadPosition(); posList != NULL; posList = listClients.GetNextPosition(posList) )
                {
                    const CEProxyAddress & proxy = listClients.GetAt(posList).GetAddress();
                    if ( proxy.IsServiceRemote() && proxy.IsRemoteAddress() && proxy.IsValid() )
                        proxyList.Add(proxy);
                }
            }

            int index = 0;
            for ( index = 0; index < stubList.GetSize(); ++ index )
                UnregisterServer( stubList[index]);
            for ( index = 0; index < proxyList.GetSize(); ++ index )
                UnregisterClient(proxyList[index]);
        }
        break;

    default:
        ASSERT(false);
    }
}

bool CEServiceManager::PostEvent(CEEvent & eventElem)
{
    bool result = false;
    if (RUNTIME_CAST(&eventElem, CEServiceManagerEvent) != NULL)
    {
        result = CEEventDispatcher::PostEvent(eventElem);
    }
    else
    {
        OUTPUT_ERR("Not a CEServiceManagerEvent type event, cannot Post. Destroying event type [ %s ]", eventElem.GetRuntimeClassName());
        eventElem.Destroy();
    }
    return result;
}

bool CEServiceManager::RunDispatcher( void )
{
    CEServiceManagerEvent::AddListener(static_cast<IEServiceManagerEventConsumer &>(self()), static_cast<CEDispatcherThread &>(self()));
    // CEManageServerEvent::AddListener(static_cast<IEManageServerEventConsumer &>(self()), static_cast<CEDispatcherThread &>(self()));

    bool result = CEDispatcherThread::RunDispatcher();

    CEServiceManagerEvent::RemoveListener(static_cast<IEServiceManagerEventConsumer &>(self()), static_cast<CEDispatcherThread &>(self()));
    // CEManageServerEvent::RemoveListener(static_cast<IEManageServerEventConsumer &>(self()), static_cast<CEDispatcherThread &>(self()));

    return result;
}

bool CEServiceManager::StartServiceManagerThread( void )
{
    bool result = false;
    CELock lock(mLock);
    if ( IsReady() == false )
    {
        ASSERT(IsRunning() == false);
        if ( CreateThread(CEThread::WAIT_INFINITE) && WaitForDispatcherStart( CEThread::WAIT_INFINITE ) )
        {
            result = true;
        }
        else
        {
            OUTPUT_ERR("Failed to create [ %s ] Service Manage thread.", CEServiceManager::SERVICE_MANAGER_THREAD_NAME);
        }
    }
    else
    {
        result = true;
    }
    return result;
}

void CEServiceManager::StopServiceManagerThread( void )
{
    CEServiceManagerEvent::SendEvent(CEServiceManagerEventData::StopMessageRouterClient(), static_cast<IEServiceManagerEventConsumer &>(self()), static_cast<CEDispatcherThread &>(self()));
    CompletionWait( CEThread::WAIT_INFINITE );
}

void CEServiceManager::GetRemoteServiceList( TEArrayList<CEStubAddress, const CEStubAddress &> & /*out_listStubs*/ ) const
{
}

void CEServiceManager::GetRemoteServiceList( TEArrayList<CEProxyAddress, const CEProxyAddress &> & /*out_lisProxies*/ ) const
{
}

void CEServiceManager::GetServiceList(ITEM_ID cookie, TEArrayList<CEStubAddress, const CEStubAddress &> out_listStubs) const
{
    CELock lock( mLock );
    out_listStubs.RemoveAll();

    for ( MAPPOS pos = mServerList.GetStartPosition(); pos != NULL; pos = mServerList.GetNextPosition(pos) )
    {
        const CEStubAddress & server = mServerList.GetKeyAt(pos).GetAddress();
        if ( server.GetCookie() == cookie && server.IsValid() )
            out_listStubs.Add(server);
    }
}

void CEServiceManager::GetServiceList(ITEM_ID cookie, TEArrayList<CEProxyAddress, const CEProxyAddress &> out_lisProxies) const
{
    CELock lock( mLock );
    out_lisProxies.RemoveAll();

    for ( MAPPOS posMap = mServerList.GetStartPosition(); posMap != NULL; posMap = mServerList.GetNextPosition(posMap) )
    {
        const CEClientList & listClients = mServerList.GetValueAt(posMap);
        for ( LISTPOS posList = listClients.GetHeadPosition(); posList != NULL; posList = listClients.GetNextPosition(posList) )
        {
            const CEProxyAddress & proxy = listClients.GetAt(posList).GetAddress();
            if ( proxy.GetCookie() == cookie && proxy.IsValid() )
                out_lisProxies.Add(proxy);
        }
    }
}

void CEServiceManager::RegisterRemoteStub( const CEStubAddress & stub )
{
    CEServiceManager::RequestRegisterServer(stub);
}

void CEServiceManager::RegisterRemoteProxy(const CEProxyAddress & proxy)
{
    CEServiceManager::RequestRegisterClient(proxy);
}

void CEServiceManager::UnregisterRemoteStub(const CEStubAddress & stub)
{
    CEServiceManager::RequestUnregisterServer(stub);
}

void CEServiceManager::UnregisterRemoteProxy(const CEProxyAddress & proxy)
{
    CEServiceManager::RequestUnregisterClient(proxy);
}

void CEServiceManager::RemoteServiceStarted(const CEChannel & channel)
{
    CEServiceManagerEvent::SendEvent( CEServiceManagerEventData::RegisterConnection(channel), static_cast<IEServiceManagerEventConsumer &>(self()), static_cast<CEDispatcherThread &>(self()));
}

void CEServiceManager::RemoteServiceStopped(const CEChannel & channel)
{
    CEServiceManagerEvent::SendEvent( CEServiceManagerEventData::UnregisterConnection(channel), static_cast<IEServiceManagerEventConsumer &>(self()), static_cast<CEDispatcherThread &>(self()));
}

void CEServiceManager::RemoveServiceLostConnection(const CEChannel & channel)
{
    CEServiceManagerEvent::SendEvent( CEServiceManagerEventData::LostConnection(channel), static_cast<IEServiceManagerEventConsumer &>(self()), static_cast<CEDispatcherThread &>(self()));
}
