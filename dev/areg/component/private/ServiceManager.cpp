/************************************************************************
 * \file        areg/component/private/ServiceManager.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Service Manager implementation.
 *
 ************************************************************************/
#include "areg/component/private/ServiceManager.hpp"

#include "areg/component/ProxyAddress.hpp"
#include "areg/component/StubAddress.hpp"
#include "areg/component/ServiceRequestEvent.hpp"
#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/component/private/ProxyConnectEvent.hpp"
#include "areg/component/private/StubConnectEvent.hpp"
#include "areg/base/Process.hpp"

#include "areg/trace/GETrace.h"

DEF_TRACE_SCOPE(areg_component_private_ServiceManager_processEvent);
DEF_TRACE_SCOPE(areg_component_private_ServiceManager__registerServer);
DEF_TRACE_SCOPE(areg_component_private_ServiceManager__unregisterServer);
DEF_TRACE_SCOPE(areg_component_private_ServiceManager__registerClient);
DEF_TRACE_SCOPE(areg_component_private_ServiceManager__unregisterClient);
DEF_TRACE_SCOPE(areg_component_private_ServiceManager__sendClientConnectedEvent);
DEF_TRACE_SCOPE(areg_component_private_ServiceManager__sendClientDisconnectedEvent);

//////////////////////////////////////////////////////////////////////////
// ServiceManager class Implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Implement Runtime
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME(ServiceManager, DispatcherThread)

//////////////////////////////////////////////////////////////////////////
// Constants and types
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Service Manager Thread name.
 **/
const char* const   ServiceManager::SERVICE_MANAGER_THREAD_NAME   = "_AREG_SERVICE_MANAGER_THREAD_";

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////

ServiceManager & ServiceManager::_getServiceManager( void )
{
    static ServiceManager  _ServiceManager;
    return _ServiceManager;
}

bool ServiceManager::_startServiceManager( void )
{
    OUTPUT_DBG("Starting Service Manager");
    return _getServiceManager()._startServiceManagerThread( );
}

void ServiceManager::_stopServiceManager( void )
{
    OUTPUT_DBG( "Stopping Service Manager" );
    _getServiceManager()._stopServiceManagerThread();
}

bool ServiceManager::isServiceManagerStarted( void )
{
    return ServiceManager::_getServiceManager().isReady();
}

void ServiceManager::requestRegisterServer( const StubAddress & whichServer )
{
    OUTPUT_DBG("Request to register server [ %s ] of interface [ %s ]"
                    , whichServer.getRoleName().getString()
                    , whichServer.getServiceName().getString());
    
    ASSERT(whichServer.isValid());
    
    ServiceManager & serviceManager = ServiceManager::_getServiceManager();
    ServiceManagerEvent::sendEvent( ServiceManagerEventData::registerStub(whichServer), static_cast<IEServiceManagerEventConsumer &>(serviceManager) , static_cast<DispatcherThread &>(serviceManager));
}

void ServiceManager::requestUnregisterServer( const StubAddress & whichServer )
{
    OUTPUT_DBG( "Request to unregister server [ %s ] of interface [ %s ]"
                    , whichServer.getRoleName( ).getString( )
                    , whichServer.getServiceName( ).getString( ) );
    
    ASSERT(whichServer.isValid());
    
    ServiceManager & serviceManager = ServiceManager::_getServiceManager();
    ServiceManagerEvent::sendEvent( ServiceManagerEventData::unregisterStub(whichServer), static_cast<IEServiceManagerEventConsumer &>(serviceManager) , static_cast<DispatcherThread &>(serviceManager));
}

void ServiceManager::requestRegisterClient( const ProxyAddress & whichClient )
{
    OUTPUT_DBG( "Request to register proxy [ %s ] of interface [ %s ]"
                    , whichClient.getRoleName( ).getString( )
                    , whichClient.getServiceName( ).getString( ) );
    
    ASSERT(whichClient.isValid());
    
    ServiceManager & serviceManager = ServiceManager::_getServiceManager();
    ServiceManagerEvent::sendEvent( ServiceManagerEventData::registerProxy(whichClient), static_cast<IEServiceManagerEventConsumer &>(serviceManager) , static_cast<DispatcherThread &>(serviceManager));
}

void ServiceManager::requestUnregisterClient( const ProxyAddress & whichClient )
{
    OUTPUT_DBG( "Request to register proxy [ %s ] of interface [ %s ]"
                    , whichClient.getRoleName( ).getString( )
                    , whichClient.getServiceName( ).getString( ) );
    
    ASSERT(whichClient.isValid());
    
    ServiceManager & serviceManager = ServiceManager::_getServiceManager();
    ServiceManagerEvent::sendEvent( ServiceManagerEventData::unregisterProxy(whichClient), static_cast<IEServiceManagerEventConsumer &>(serviceManager) , static_cast<DispatcherThread &>(serviceManager));
}

bool ServiceManager::_routingServiceConfigure( const char * configFile /*= NULL*/ )
{
    ServiceManager & serviceManager = ServiceManager::_getServiceManager();
    return ServiceManagerEvent::sendEvent( ServiceManagerEventData::configureConnection(String(configFile)), static_cast<IEServiceManagerEventConsumer &>(serviceManager) , static_cast<DispatcherThread &>(serviceManager));
}

bool ServiceManager::_routingServiceStart(const char * configFile /*= NULL */)
{
    ServiceManager & serviceManager = ServiceManager::_getServiceManager();
    return ServiceManagerEvent::sendEvent( ServiceManagerEventData::startConnection(String(configFile)), static_cast<IEServiceManagerEventConsumer &>(serviceManager) , static_cast<DispatcherThread &>(serviceManager));
}

bool ServiceManager::_routingServiceStart( const char * ipAddress, unsigned short portNr )
{
    bool result = false;
    if ( (NEString::isEmpty<char>(ipAddress) == false) && (portNr != NESocket::InvalidPort) )
    {
        ServiceManager & serviceManager = ServiceManager::_getServiceManager( );
        result =ServiceManagerEvent::sendEvent( ServiceManagerEventData::startNetConnection( String( ipAddress ), portNr ), static_cast<IEServiceManagerEventConsumer &>(serviceManager), static_cast<DispatcherThread &>(serviceManager) );
    }
    return result;
}

void ServiceManager::_routingServiceStop(void)
{
    ServiceManager & serviceManager = ServiceManager::_getServiceManager();
    ServiceManagerEvent::sendEvent( ServiceManagerEventData::stopConnection(), static_cast<IEServiceManagerEventConsumer &>(serviceManager) , static_cast<DispatcherThread &>(serviceManager));
}

void ServiceManager::_routingServiceEnable( bool enable )
{
    ServiceManager & serviceManager = ServiceManager::_getServiceManager( );
    ServiceManagerEvent::sendEvent( ServiceManagerEventData::enableRemoteService(enable), static_cast<IEServiceManagerEventConsumer &>(serviceManager), static_cast<DispatcherThread &>(serviceManager) );
}

bool ServiceManager::_isRoutingServiceStarted(void)
{
    return ServiceManager::_getServiceManager().mConnectService.isRemoteServicingStarted();
}

bool ServiceManager::_isRoutingServiceConfigured(void)
{
    return ServiceManager::_getServiceManager().mConnectService.isRemoteServicingConfigured();
}

bool ServiceManager::_isRoutingServiceEnabled(void)
{
    return ServiceManager::_getServiceManager().mConnectService.isRemoteServicingEnabled();
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
ServiceManager::ServiceManager( void )
    : DispatcherThread              ( ServiceManager::SERVICE_MANAGER_THREAD_NAME )
    , IEServiceManagerEventConsumer ( )
    , IERemoteServiceConsumer       ( )

    , mServerList       ( )
    , mConnectService   ( static_cast<IERemoteServiceConsumer &>(self()) )
    , mLock             (  )
{
    ; // do nothing
}

ServiceManager::~ServiceManager( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////
void ServiceManager::_registerServer( const StubAddress & whichServer )
{
    TRACE_SCOPE(areg_component_private_ServiceManager__registerServer);

    if ( whichServer.isLocalAddress() && whichServer.isServiceRemote() )
        mConnectService.registerService(whichServer);

    ClientList clientList;

    DECLARE_TRACE_VARIABLE(const ServerInfo &, server, mServerList.registerServer(whichServer, clientList));
    TRACE_DBG("Server [ %s ] is registered. Connection status [ %s ], there are [ %d ] waiting clients"
                , StubAddress::convAddressToPath(server.getAddress()).getString()
                , NEService::getString(server.getConnectionStatus())
                , clientList.getSize());

    for ( LISTPOS pos = clientList.firstPosition(); pos != NULL; pos = clientList.nextPosition(pos) )
        _sendClientConnectedEvent( clientList.getAt(pos), whichServer );
}

void ServiceManager::_unregisterServer( const StubAddress & whichServer )
{
    TRACE_SCOPE(areg_component_private_ServiceManager__unregisterServer);

    if ( whichServer.isLocalAddress() && whichServer.isServiceRemote() )
        mConnectService.unregisterService(whichServer);

    ClientList clientList;
    ServerInfo server = mServerList.unregisterServer(whichServer, clientList);

    TRACE_DBG("Server [ %s ] is unregistered. Connection status [ %s ], there are [ %d ] waiting clients"
                , StubAddress::convAddressToPath(server.getAddress()).getString()
                , NEService::getString(server.getConnectionStatus())
                , clientList.getSize());

    for ( LISTPOS pos = clientList.firstPosition(); pos != NULL; pos = clientList.nextPosition(pos))
        _sendClientDisconnectedEvent(clientList.getAt(pos), whichServer);
}

void ServiceManager::_registerClient( const ProxyAddress & whichClient )
{
    TRACE_SCOPE(areg_component_private_ServiceManager__registerClient);

    if ( whichClient.isLocalAddress() && whichClient.isServiceRemote() )
        mConnectService.registerServiceClient(whichClient);

    ClientInfo client;
    const ServerInfo & server = mServerList.registerClient(whichClient, client);

    TRACE_DBG("Client [ %s ] is registered for server [ %s ], connection status [ %s ]"
                , ProxyAddress::convAddressToPath(client.getAddress()).getString()
                , StubAddress::convAddressToPath(server.getAddress()).getString()
                , NEService::getString(client.getConnectionStatus()));

    _sendClientConnectedEvent( client, server.getAddress() );
}

void ServiceManager::_unregisterClient( const ProxyAddress & whichClient )
{
    TRACE_SCOPE(areg_component_private_ServiceManager__unregisterClient);

    if ( whichClient.isLocalAddress() && whichClient.isServiceRemote() )
        mConnectService.unregisterServiceClient(whichClient);

    ClientInfo client;
    ServerInfo server = mServerList.unregisterClient(whichClient, client);

    TRACE_DBG("Client [ %s ] is unregistered from server [ %s ], connection status [ %s ]"
                , ProxyAddress::convAddressToPath(client.getAddress()).getString()
                , StubAddress::convAddressToPath(server.getAddress()).getString()
                , NEService::getString(client.getConnectionStatus()));

    // Unregister client first, then send event that client does not receive notification
    _sendClientDisconnectedEvent(client, server.getAddress() );
}

void ServiceManager::_sendClientConnectedEvent( const ClientInfo & client, const StubAddress & addrStub ) const
{
    TRACE_SCOPE(areg_component_private_ServiceManager__sendClientConnectedEvent);

    if ( client.isConnected() )
    {
        const ProxyAddress & addrProxy = client.getAddress();
        if ( addrStub.isLocalAddress() && addrStub.getSource() != NEService::SOURCE_UNKNOWN)
        {
            TRACE_DBG("Sending to Stub [ %s ] notification of connected client [ %s ]"
                            , StubAddress::convAddressToPath(addrStub).getString()
                            , ProxyAddress::convAddressToPath(addrProxy).getString());
            
            StubConnectEvent * clientConnect  = DEBUG_NEW StubConnectEvent(addrProxy, addrStub, NEService::ServiceConnected);
            if ( clientConnect != NULL )
                addrStub.deliverServiceEvent(*clientConnect);
        }

        if ( addrProxy.isLocalAddress() && addrProxy.getSource() != NEService::SOURCE_UNKNOWN )
        {
            TRACE_DBG("Sending to Proxy [ %s ] notification of connection to server [ %s ]"
                            , ProxyAddress::convAddressToPath(addrProxy).getString()
                            , StubAddress::convAddressToPath(addrStub).getString());
            
            ProxyConnectEvent * proxyConnect  = DEBUG_NEW ProxyConnectEvent(addrProxy, addrStub, NEService::ServiceConnected);
            if ( proxyConnect != NULL )
                addrProxy.deliverServiceEvent(*proxyConnect);
        }
    }
    else
    {
        TRACE_INFO("The client Proxy [ %s ] has NO CONNECTION yet. Notingto send", ProxyAddress::convAddressToPath(client.getAddress()).getString());
    }
}

void ServiceManager::_sendClientDisconnectedEvent( const ClientInfo & client, const StubAddress & addrStub ) const
{
    TRACE_SCOPE(areg_component_private_ServiceManager__sendClientDisconnectedEvent);

    if ( client.isWaitingConnection() )
    {
        const ProxyAddress & addrProxy = client.getAddress();
        if ( addrStub.isLocalAddress() && addrStub.getSource() != NEService::SOURCE_UNKNOWN)
        {
            TRACE_DBG("Sending to Stub [ %s ] notification of disconnected client [ %s ]"
                            , StubAddress::convAddressToPath(addrStub).getString()
                            , ProxyAddress::convAddressToPath(addrProxy).getString());
            
            StubConnectEvent * clientConnect  = DEBUG_NEW StubConnectEvent(addrProxy, addrStub, NEService::ServiceDisconnected);
            if ( clientConnect != NULL )
                addrStub.deliverServiceEvent(*clientConnect);
        }

        if ( addrProxy.isLocalAddress() )
        {
            TRACE_DBG("Sending to Proxy [ %s ] notification of disconnection from server [ %s ]"
                            , ProxyAddress::convAddressToPath(addrProxy).getString()
                            , StubAddress::convAddressToPath(addrStub).getString());
            
            ProxyConnectEvent * proxyConnect  = DEBUG_NEW ProxyConnectEvent(addrProxy, addrStub, NEService::ServiceDisconnected);
            if ( proxyConnect != NULL )
                addrProxy.deliverServiceEvent(*proxyConnect);
        }
    }
}

void ServiceManager::processEvent( const ServiceManagerEventData & data )
{
    TRACE_SCOPE(areg_component_private_ServiceManager_processEvent);
    ServiceManagerEventData::eServiceManagerCommands cmdService = data.getCommand();
    const IEInStream & stream = data.getReadStream();

    TRACE_DBG("Service Manager is going to execute command [ %s ]", ServiceManagerEventData::getString(cmdService));

    switch ( cmdService )
    {
    case ServiceManagerEventData::CMD_ShutdownService:
        {
            removeAllEvents();
            mServerList.removeAll();
            mConnectService.stopRemoteServicing();
            DispatcherThread::removeEvents(false);
            DispatcherThread::triggerExitEvent();
        }
        break;

    case ServiceManagerEventData::CMD_StopRoutingClient:
        {
            for ( MAPPOS pos = mServerList.firstPosition(); pos != NULL; pos = mServerList.nextPosition(pos) )
            {
                ServerInfo si;
                ClientList cList;

                mServerList.getAtPosition(pos, si, cList);
                for ( LISTPOS pos = cList.firstPosition(); pos != NULL; pos = cList.nextPosition(pos))
                {
                    const ClientInfo & client = cList.getAt(pos);
                    _sendClientDisconnectedEvent(client, si.getAddress());
                }
            }

            mServerList.removeAll();
            mConnectService.stopRemoteServicing();
            DispatcherThread::removeEvents(false);
            pulseExit();
        }
        break;

    case ServiceManagerEventData::CMD_RegisterProxy:
        {
            ProxyAddress  addrProxy;
            Channel       channel;
            stream >> addrProxy;
            stream >> channel;
            addrProxy.setChannel(channel);
            _registerClient(addrProxy);
        }
        break;

    case ServiceManagerEventData::CMD_UnregisterProxy:
        {
            ProxyAddress  addrProxy;
            Channel       channel;
            stream >> addrProxy;
            stream >> channel;
            addrProxy.setChannel(channel);
            _unregisterClient(addrProxy);
        }
        break;

    case ServiceManagerEventData::CMD_RegisterStub:
        {
            StubAddress   addrstub;
            Channel       channel;
            stream >> addrstub;
            stream >> channel;
            addrstub.setChannel(channel);
            _registerServer(addrstub);
        }
        break;

    case ServiceManagerEventData::CMD_UnregisterStub:
        {
            StubAddress   addrstub;
            Channel       channel;
            stream >> addrstub;
            stream >> channel;
            addrstub.setChannel(channel);
            _unregisterServer(addrstub);
        }
        break;

    case ServiceManagerEventData::CMD_ConfigureConnection:
        {
            String   configFile;
            stream >> configFile;
            mConnectService.enableRemoteServicing(true);
            if ( NEString::isEmpty<char>(configFile) == false )
                mConnectService.configureRemoteServicing(configFile);
            else if (mConnectService.isRemoteServicingConfigured() == false)
                mConnectService.configureRemoteServicing(NULL);
        }
        break;

    case ServiceManagerEventData::CMD_StartConnection:
        {
            String   configFile;
            stream >> configFile;
            bool isConfigured = false;
            mConnectService.enableRemoteServicing( true );
            if ( NEString::isEmpty<char>(configFile) == false )
                isConfigured = mConnectService.configureRemoteServicing(configFile);
            else if (mConnectService.isRemoteServicingConfigured() == false)
                isConfigured = mConnectService.configureRemoteServicing(NULL);
            else 
                isConfigured = true;
            if ( isConfigured )
                mConnectService.startRemoteServicing();
        }
        break;

    case ServiceManagerEventData::CMD_StartNetConnection:
        {
            String   ipAddress;
            unsigned short portNr = 0;
            stream >> ipAddress;
            stream >> portNr;

            mConnectService.enableRemoteServicing( true );
            mConnectService.setRemoteServiceAddress(ipAddress, portNr);
            if ( mConnectService.isRemoteServicingConfigured() )
                mConnectService.startRemoteServicing( );
        }
        break;

    case ServiceManagerEventData::CMD_StopConnection:
        {
            mConnectService.stopRemoteServicing();
        }
        break;

    case ServiceManagerEventData::CMD_SetEnableService:
        {
            bool enable = false;
            stream >> enable;
            mConnectService.enableRemoteServicing(enable);
        }
        break;

    case ServiceManagerEventData::CMD_RegisterConnection:
        {
            for ( MAPPOS posMap = mServerList.firstPosition(); posMap != NULL; posMap = mServerList.nextPosition(posMap) )
            {
                const StubAddress & server = mServerList.keyAtPosition(posMap).getAddress();
                const ClientList & listClients = mServerList.valueAtPosition(posMap);

                if ( server.isServiceRemote() && server.isLocalAddress() && server.isValid() )
                    mConnectService.registerService(server);

                for ( LISTPOS posList = listClients.firstPosition(); posList != NULL; posList = listClients.nextPosition(posList) )
                {
                    const ProxyAddress & proxy = listClients.getAt(posList).getAddress();
                    if ( proxy.isServiceRemote() && proxy.isLocalAddress() && proxy.isValid() )
                        mConnectService.registerServiceClient(proxy);
                }
            }
        }
        break;

    case ServiceManagerEventData::CMD_UnregisterConnection:
    case ServiceManagerEventData::CMD_LostConnection:
        {
            TEArrayList<StubAddress, const StubAddress &> stubList;
            TEArrayList<ProxyAddress, const ProxyAddress &> proxyList;
            for ( MAPPOS posMap = mServerList.firstPosition(); posMap != NULL; posMap = mServerList.nextPosition(posMap) )
            {
                const StubAddress & server = mServerList.keyAtPosition(posMap).getAddress();
                const ClientList & listClients = mServerList.valueAtPosition(posMap);

                if ( server.isServiceRemote() && server.isRemoteAddress() && server.isValid() )
                    stubList.add(server);

                for ( LISTPOS posList = listClients.firstPosition(); posList != NULL; posList = listClients.nextPosition(posList) )
                {
                    const ProxyAddress & proxy = listClients.getAt(posList).getAddress();
                    if ( proxy.isServiceRemote() && proxy.isRemoteAddress() && proxy.isValid() )
                        proxyList.add(proxy);
                }
            }

            int index = 0;
            for ( index = 0; index < stubList.getSize(); ++ index )
                _unregisterServer( stubList[index]);
            for ( index = 0; index < proxyList.getSize(); ++ index )
                _unregisterClient(proxyList[index]);
        }
        break;

    default:
        ASSERT(false);
    }
}

bool ServiceManager::postEvent(Event & eventElem)
{
    bool result = false;
    if (RUNTIME_CAST(&eventElem, ServiceManagerEvent) != NULL)
    {
        result = EventDispatcher::postEvent(eventElem);
    }
    else
    {
        OUTPUT_ERR("Not a ServiceManagerEvent type event, cannot Post. Destroying event type [ %s ]", eventElem.getRuntimeClassName());
        eventElem.destroy();
    }
    return result;
}

bool ServiceManager::runDispatcher( void )
{
    ServiceManagerEvent::addListener(static_cast<IEServiceManagerEventConsumer &>(self()), static_cast<DispatcherThread &>(self()));

    bool result = DispatcherThread::runDispatcher();

    ServiceManagerEvent::removeListener(static_cast<IEServiceManagerEventConsumer &>(self()), static_cast<DispatcherThread &>(self()));

    return result;
}

bool ServiceManager::_startServiceManagerThread( void )
{
    bool result = false;
    Lock lock(mLock);
    if ( isReady() == false )
    {
        ASSERT(isRunning() == false);
        if ( createThread(Thread::WAIT_INFINITE) && waitForDispatcherStart( Thread::WAIT_INFINITE ) )
        {
            result = true;
        }
        else
        {
            OUTPUT_ERR("Failed to create [ %s ] Service Manage thread.", ServiceManager::SERVICE_MANAGER_THREAD_NAME);
        }
    }
    else
    {
        result = true;
    }
    return result;
}

void ServiceManager::_stopServiceManagerThread( void )
{
    // ServiceManagerEvent::sendEvent(ServiceManagerEventData::stopMessageRouterClient(), static_cast<IEServiceManagerEventConsumer &>(self()), static_cast<DispatcherThread &>(self()));
    ServiceManagerEvent::sendEvent(ServiceManagerEventData::shutdownServiceManager(), static_cast<IEServiceManagerEventConsumer &>(self()), static_cast<DispatcherThread &>(self()));
    completionWait( Thread::WAIT_INFINITE );
}

void ServiceManager::getRemoteServiceList( TEArrayList<StubAddress, const StubAddress &> & OUT /*out_listStubs*/, TEArrayList<ProxyAddress, const ProxyAddress &> & OUT /*out_lisProxies*/) const
{
}

void ServiceManager::getServiceList( ITEM_ID cookie, TEArrayList<StubAddress, const StubAddress &> & OUT out_listStubs, TEArrayList<ProxyAddress, const ProxyAddress &> & OUT out_lisProxies ) const
{
    Lock lock( mLock );

    out_listStubs.removeAll();
    out_lisProxies.removeAll();

    for ( MAPPOS posMap = mServerList.firstPosition(); posMap != NULL; posMap = mServerList.nextPosition(posMap) )
    {
        const StubAddress & server      = mServerList.keyAtPosition(posMap).getAddress();
        const ClientList & listClients  = mServerList.valueAtPosition(posMap);

        if ( (server.getCookie() == cookie) && server.isValid() )
        {
            out_listStubs.add(server);
        }

        for ( LISTPOS posList = listClients.firstPosition(); posList != NULL; posList = listClients.nextPosition(posList) )
        {
            const ProxyAddress & proxy = listClients.getAt(posList).getAddress();
            if ( (proxy.getCookie() == cookie) && proxy.isValid() )
            {
                out_lisProxies.add(proxy);
            }
        }
    }
}

void ServiceManager::registerRemoteStub( const StubAddress & stub )
{
    ServiceManager::requestRegisterServer(stub);
}

void ServiceManager::registerRemoteProxy(const ProxyAddress & proxy)
{
    ServiceManager::requestRegisterClient(proxy);
}

void ServiceManager::unregisterRemoteStub(const StubAddress & stub, ITEM_ID /*cookie*/ /*= NEService::COOKIE_ANY*/ )
{
    ServiceManager::requestUnregisterServer(stub);
}

void ServiceManager::unregisterRemoteProxy(const ProxyAddress & proxy, ITEM_ID /* cookie */ /*= NEService::COOKIE_ANY*/ )
{
    ServiceManager::requestUnregisterClient(proxy);
}

void ServiceManager::remoteServiceStarted(const Channel & channel)
{
    ServiceManagerEvent::sendEvent( ServiceManagerEventData::registerConnection(channel), static_cast<IEServiceManagerEventConsumer &>(self()), static_cast<DispatcherThread &>(self()));
}

void ServiceManager::remoteServiceStopped(const Channel & channel)
{
    ServiceManagerEvent::sendEvent( ServiceManagerEventData::unregisterConnection(channel), static_cast<IEServiceManagerEventConsumer &>(self()), static_cast<DispatcherThread &>(self()));
}

void ServiceManager::remoteServiceConnectionLost(const Channel & channel)
{
    ServiceManagerEvent::sendEvent( ServiceManagerEventData::lostConnection(channel), static_cast<IEServiceManagerEventConsumer &>(self()), static_cast<DispatcherThread &>(self()));
}
