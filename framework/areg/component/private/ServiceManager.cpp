/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ServiceManager.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Service Manager implementation.
 *
 ************************************************************************/
#include "areg/component/private/ServiceManager.hpp"

#include "areg/base/Process.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/NERegistry.hpp"
#include "areg/component/ProxyAddress.hpp"
#include "areg/component/StubAddress.hpp"
#include "areg/component/ServiceRequestEvent.hpp"
#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/component/private/ProxyConnectEvent.hpp"
#include "areg/component/private/StubConnectEvent.hpp"

#include "areg/trace/GETrace.h"

#include <string_view>

DEF_TRACE_SCOPE(areg_component_private_ServiceManager_processEvent);
DEF_TRACE_SCOPE(areg_component_private_ServiceManager__registerServer);
DEF_TRACE_SCOPE(areg_component_private_ServiceManager__unregisterServer);
DEF_TRACE_SCOPE(areg_component_private_ServiceManager__registerClient);
DEF_TRACE_SCOPE(areg_component_private_ServiceManager__unregisterClient);
DEF_TRACE_SCOPE(areg_component_private_ServiceManager__sendClientConnectedEvent);
DEF_TRACE_SCOPE(areg_component_private_ServiceManager__sendClientDisconnectedEvent);
DEF_TRACE_SCOPE(areg_component_private_ServiceManager__terminateComponentThread);
DEF_TRACE_SCOPE(areg_component_private_ServiceManager__startComponentThread);
DEF_TRACE_SCOPE(areg_component_private_ServiceManager_requestRegisterServer);
DEF_TRACE_SCOPE(areg_component_private_ServiceManager_requestUnregisterServer);
DEF_TRACE_SCOPE(areg_component_private_ServiceManager_requestRegisterClient);
DEF_TRACE_SCOPE(areg_component_private_ServiceManager_requestUnregisterClient);
DEF_TRACE_SCOPE(areg_component_private_ServiceManager_requestRecreateThread);
DEF_TRACE_SCOPE(areg_component_private_ServiceManager_getServiceList);

namespace
{
    /**
     * \brief   Predefined Service Manager Thread name.
     **/
    constexpr std::string_view SERVICE_MANAGER_THREAD_NAME      { "_AREG_SERVICE_MANAGER_THREAD_" };
}

//////////////////////////////////////////////////////////////////////////
// ServiceManager class Implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Implement Runtime
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME(ServiceManager, DispatcherThread)

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////

ServiceManager & ServiceManager::getInstance( void )
{
    static ServiceManager	_theServiceManager;
    return _theServiceManager;
}

bool ServiceManager::_startServiceManager( void )
{
    OUTPUT_DBG("Starting Service Manager");
    return getInstance()._startServiceManagerThread( );
}

void ServiceManager::_stopServiceManager( void )
{
    OUTPUT_DBG( "Stopping Service Manager" );
    getInstance()._stopServiceManagerThread();
}

bool ServiceManager::isServiceManagerStarted( void )
{
    return ServiceManager::getInstance().isReady();
}

void ServiceManager::queryCommunicationData( unsigned int & OUT sizeSend, unsigned int & OUT sizeReceive )
{
    ServiceManager & serviceManager = ServiceManager::getInstance( );
    sizeSend    = serviceManager.mConnectService.queryBytesSent( );
    sizeReceive = serviceManager.mConnectService.queryBytesReceived( );
}

void ServiceManager::requestRegisterServer( const StubAddress & whichServer )
{
    TRACE_SCOPE(areg_component_private_ServiceManager_requestRegisterServer);
    TRACE_DBG("Request to register server [ %s ] of interface [ %s ]"
                    , whichServer.getRoleName().getString()
                    , whichServer.getServiceName().getString());

    ASSERT(whichServer.isValid());

    ServiceManager & serviceManager = ServiceManager::getInstance();
    ServiceManagerEvent::sendEvent( ServiceManagerEventData::registerStub(whichServer)
                                  , static_cast<IEServiceManagerEventConsumer &>(serviceManager)
                                  , static_cast<DispatcherThread &>(serviceManager));
}

void ServiceManager::requestUnregisterServer( const StubAddress & whichServer )
{
    TRACE_SCOPE(areg_component_private_ServiceManager_requestUnregisterServer);

    TRACE_DBG( "Request to unregister server [ %s ] of interface [ %s ]"
                    , whichServer.getRoleName( ).getString( )
                    , whichServer.getServiceName( ).getString( ) );
    
    ASSERT(whichServer.isValid());
    
    ServiceManager & serviceManager = ServiceManager::getInstance();
    ServiceManagerEvent::sendEvent( ServiceManagerEventData::unregisterStub(whichServer)
                                  , static_cast<IEServiceManagerEventConsumer &>(serviceManager)
                                  , static_cast<DispatcherThread &>(serviceManager));
}

void ServiceManager::requestRegisterClient( const ProxyAddress & whichClient )
{
    TRACE_SCOPE(areg_component_private_ServiceManager_requestRegisterClient);

    TRACE_DBG( "Request to register proxy [ %s ] of interface [ %s ]"
                    , whichClient.getRoleName( ).getString( )
                    , whichClient.getServiceName( ).getString( ) );
    
    ASSERT(whichClient.isValid());
    
    ServiceManager & serviceManager = ServiceManager::getInstance();
    ServiceManagerEvent::sendEvent( ServiceManagerEventData::registerProxy(whichClient)
                                  , static_cast<IEServiceManagerEventConsumer &>(serviceManager)
                                  , static_cast<DispatcherThread &>(serviceManager));
}

void ServiceManager::requestUnregisterClient( const ProxyAddress & whichClient )
{
    TRACE_SCOPE(areg_component_private_ServiceManager_requestUnregisterClient);
    TRACE_DBG( "Request to register proxy [ %s ] of interface [ %s ]"
                    , whichClient.getRoleName( ).getString( )
                    , whichClient.getServiceName( ).getString( ) );
    
    ASSERT(whichClient.isValid());
    
    ServiceManager & serviceManager = ServiceManager::getInstance();
    ServiceManagerEvent::sendEvent( ServiceManagerEventData::unregisterProxy(whichClient)
                                  , static_cast<IEServiceManagerEventConsumer &>(serviceManager)
                                  , static_cast<DispatcherThread &>(serviceManager));
}

void ServiceManager::requestRecreateThread(const ComponentThread& whichThread)
{
    TRACE_SCOPE(areg_component_private_ServiceManager_requestRecreateThread);
    TRACE_DBG("Request to re-create component thread [ %s ]", whichThread.getName().getString());

    ServiceManager & serviceManager = ServiceManager::getInstance();
    ServiceManagerEvent::sendEvent(ServiceManagerEventData::terminateComponentThread(whichThread.getName())
                                  , static_cast<IEServiceManagerEventConsumer &>(serviceManager)
                                  , static_cast<DispatcherThread &>(serviceManager));
}

bool ServiceManager::_routingServiceConfigure( const String & configFile /*= String::getEmptyString()*/ )
{
    ServiceManager & serviceManager = ServiceManager::getInstance();
    return ServiceManagerEvent::sendEvent( ServiceManagerEventData::configureConnection(configFile)
                                         , static_cast<IEServiceManagerEventConsumer &>(serviceManager) 
                                         , static_cast<DispatcherThread &>(serviceManager));
}

bool ServiceManager::_routingServiceStart(const String & configFile /*= String::getEmptyString() */)
{
    ServiceManager & serviceManager = ServiceManager::getInstance();
    return ServiceManagerEvent::sendEvent( ServiceManagerEventData::startConnection(configFile)
                                         , static_cast<IEServiceManagerEventConsumer &>(serviceManager)
                                         , static_cast<DispatcherThread &>(serviceManager));
}

bool ServiceManager::_routingServiceStart( const String & ipAddress, unsigned short portNr )
{
    bool result = false;
    if ( (ipAddress.isEmpty() == false) && (portNr != NESocket::InvalidPort) )
    {
        ServiceManager & serviceManager = ServiceManager::getInstance( );
        result =ServiceManagerEvent::sendEvent( ServiceManagerEventData::startNetConnection( ipAddress, portNr )
                                              , static_cast<IEServiceManagerEventConsumer &>(serviceManager)
                                              , static_cast<DispatcherThread &>(serviceManager) );
    }
    return result;
}

void ServiceManager::_routingServiceStop(void)
{
    ServiceManager & serviceManager = ServiceManager::getInstance();
    ServiceManagerEvent::sendEvent( ServiceManagerEventData::stopConnection()
                                  , static_cast<IEServiceManagerEventConsumer &>(serviceManager)
                                  , static_cast<DispatcherThread &>(serviceManager));
}

void ServiceManager::_routingServiceEnable( bool enable )
{
    ServiceManager & serviceManager = ServiceManager::getInstance( );
    ServiceManagerEvent::sendEvent( ServiceManagerEventData::enableRemoteService(enable)
                                  , static_cast<IEServiceManagerEventConsumer &>(serviceManager)
                                  , static_cast<DispatcherThread &>(serviceManager) );
}

bool ServiceManager::_isRoutingServiceStarted(void)
{
    return ServiceManager::getInstance().mConnectService.isRemoteServicingStarted();
}

bool ServiceManager::_isRoutingServiceConfigured(void)
{
    return ServiceManager::getInstance().mConnectService.isRemoteServicingConfigured();
}

bool ServiceManager::_isRoutingServiceEnabled(void)
{
    return ServiceManager::getInstance().mConnectService.isRemoteServicingEnabled();
}

void ServiceManager::_requestCreateThread(const String& componentThread)
{
    ServiceManager& serviceManager = ServiceManager::getInstance();
    ServiceManagerEvent::sendEvent( ServiceManagerEventData::createComponentThread(componentThread)
                                  , static_cast<IEServiceManagerEventConsumer&>(serviceManager)
                                  , static_cast<DispatcherThread&>(serviceManager) );
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
ServiceManager::ServiceManager( void )
    : DispatcherThread              ( SERVICE_MANAGER_THREAD_NAME )
    , IEServiceManagerEventConsumer ( )
    , IERemoteServiceConsumer       ( )

    , mServerList       ( )
    , mConnectService   ( static_cast<IERemoteServiceConsumer &>(self()) )
    , mLock             (  )
{
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////
void ServiceManager::_registerServer( const StubAddress & whichServer )
{
    TRACE_SCOPE(areg_component_private_ServiceManager__registerServer);

    if (whichServer.isLocalAddress() && whichServer.isServicePublic())
    {
        mConnectService.registerService(whichServer);
    }

    ClientList clientList;

#if AREG_LOGS
    const ServerInfo & server = mServerList.registerServer(whichServer, clientList);
    TRACE_DBG("Server [ %s ] is registered. Connection status [ %s ], there are [ %d ] waiting clients"
                , StubAddress::convAddressToPath(server.getAddress()).getString()
                , NEService::getString(server.getConnectionStatus())
                , clientList.getSize());
#else   // !AREG_LOGS
    mServerList.registerServer(whichServer, clientList);
#endif  // !AREG_LOGS

    for (ClientList::LISTPOS pos = clientList.firstPosition(); clientList.isValidPosition(pos); pos = clientList.nextPosition(pos))
    {
        _sendClientConnectedEvent(clientList.valueAtPosition(pos), whichServer);
    }
}

void ServiceManager::_unregisterServer( const StubAddress & whichServer )
{
    TRACE_SCOPE(areg_component_private_ServiceManager__unregisterServer);

    if (whichServer.isLocalAddress() && whichServer.isServicePublic())
    {
        mConnectService.unregisterService(whichServer);
    }

    ClientList clientList;
    ServerInfo server = mServerList.unregisterServer(whichServer, clientList);

    TRACE_DBG("Server [ %s ] is unregistered. Connection status [ %s ], there are [ %d ] waiting clients"
                , StubAddress::convAddressToPath(server.getAddress()).getString()
                , NEService::getString(server.getConnectionStatus())
                , clientList.getSize());

    for (ClientList::LISTPOS pos = clientList.firstPosition(); clientList.isValidPosition(pos); pos = clientList.nextPosition(pos))
    {
        _sendClientDisconnectedEvent(clientList.valueAtPosition(pos), whichServer);
    }
}

void ServiceManager::_registerClient( const ProxyAddress & whichClient )
{
    TRACE_SCOPE(areg_component_private_ServiceManager__registerClient);

    if (whichClient.isLocalAddress() && whichClient.isServicePublic())
    {
        mConnectService.registerServiceClient(whichClient);
    }

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

    if (whichClient.isLocalAddress() && whichClient.isServicePublic())
    {
        mConnectService.unregisterServiceClient(whichClient);
    }

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
            
            StubConnectEvent * clientConnect  = DEBUG_NEW StubConnectEvent(addrProxy, addrStub, NEService::eServiceConnection::ServiceConnected);
            if (clientConnect != nullptr)
            {
                addrStub.deliverServiceEvent(*clientConnect);
            }
        }

        if ( addrProxy.isLocalAddress() && addrProxy.getSource() != NEService::SOURCE_UNKNOWN )
        {
            TRACE_DBG("Sending to Proxy [ %s ] notification of connection to server [ %s ]"
                            , ProxyAddress::convAddressToPath(addrProxy).getString()
                            , StubAddress::convAddressToPath(addrStub).getString());
            
            ProxyConnectEvent * proxyConnect  = DEBUG_NEW ProxyConnectEvent(addrProxy, addrStub, NEService::eServiceConnection::ServiceConnected);
            if (proxyConnect != nullptr)
            {
                addrProxy.deliverServiceEvent(*proxyConnect);
            }
        }
    }
    else
    {
        TRACE_INFO("The client Proxy [ %s ] has NO CONNECTION yet. Noting to send", ProxyAddress::convAddressToPath(client.getAddress()).getString());
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
            
            StubConnectEvent * clientConnect  = DEBUG_NEW StubConnectEvent(addrProxy, addrStub, NEService::eServiceConnection::ServiceDisconnected);
            if (clientConnect != nullptr)
            {
                addrStub.deliverServiceEvent(*clientConnect);
            }
        }

        if ( addrProxy.isLocalAddress() )
        {
            TRACE_DBG("Sending to Proxy [ %s ] notification of disconnection from server [ %s ]"
                            , ProxyAddress::convAddressToPath(addrProxy).getString()
                            , StubAddress::convAddressToPath(addrStub).getString());
            
            ProxyConnectEvent * proxyConnect  = DEBUG_NEW ProxyConnectEvent(addrProxy, addrStub, NEService::eServiceConnection::ServiceDisconnected);
            if (proxyConnect != nullptr)
            {
                addrProxy.deliverServiceEvent(*proxyConnect);
            }
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
    case ServiceManagerEventData::eServiceManagerCommands::CMD_ShutdownService:
        {
            removeAllEvents();
            mServerList.clear();
            mConnectService.stopRemoteServicing();
            DispatcherThread::removeEvents(false);
            DispatcherThread::triggerExitEvent();
        }
        break;

    case ServiceManagerEventData::eServiceManagerCommands::CMD_StopRoutingClient:
        {
            for (ServerList::MAPPOS pos = mServerList.firstPosition(); mServerList.isValidPosition(pos); pos = mServerList.nextPosition(pos) )
            {
                ServerInfo si;
                ClientList clientList;

                mServerList.getAtPosition(pos, si, clientList);
                for (ClientList::LISTPOS pos = clientList.firstPosition(); clientList.isValidPosition(pos); pos = clientList.nextPosition(pos))
                {
                    const ClientInfo & client = clientList.valueAtPosition(pos);
                    _sendClientDisconnectedEvent(client, si.getAddress());
                }
            }

            mServerList.clear();
            mConnectService.stopRemoteServicing();
            DispatcherThread::removeEvents(false);
            pulseExit();
        }
        break;

    case ServiceManagerEventData::eServiceManagerCommands::CMD_RegisterProxy:
        {
            ProxyAddress  addrProxy;
            Channel       channel;
            stream >> addrProxy;
            stream >> channel;
            addrProxy.setChannel(channel);
            _registerClient(addrProxy);
        }
        break;

    case ServiceManagerEventData::eServiceManagerCommands::CMD_UnregisterProxy:
        {
            ProxyAddress  addrProxy;
            Channel       channel;
            stream >> addrProxy;
            stream >> channel;
            addrProxy.setChannel(channel);
            _unregisterClient(addrProxy);
        }
        break;

    case ServiceManagerEventData::eServiceManagerCommands::CMD_RegisterStub:
        {
            StubAddress   addrstub;
            Channel       channel;
            stream >> addrstub;
            stream >> channel;
            addrstub.setChannel(channel);
            _registerServer(addrstub);
        }
        break;

    case ServiceManagerEventData::eServiceManagerCommands::CMD_UnregisterStub:
        {
            StubAddress   addrstub;
            Channel       channel;
            stream >> addrstub;
            stream >> channel;
            addrstub.setChannel(channel);
            _unregisterServer(addrstub);
        }
        break;

    case ServiceManagerEventData::eServiceManagerCommands::CMD_ConfigureConnection:
        {
            String   configFile;
            stream >> configFile;
            mConnectService.enableRemoteServicing(true);
            if (configFile.isEmpty() == false)
            {
                mConnectService.configureRemoteServicing(configFile);
            }
            else if (mConnectService.isRemoteServicingConfigured() == false)
            {
                mConnectService.configureRemoteServicing(String::getEmptyString());
            }
        }
        break;

    case ServiceManagerEventData::eServiceManagerCommands::CMD_StartConnection:
        {
            String   configFile;
            stream >> configFile;
            bool isConfigured = false;
            mConnectService.enableRemoteServicing( true );
            if (configFile.isEmpty() == false)
            {
                isConfigured = mConnectService.configureRemoteServicing(configFile);
            }
            else if (mConnectService.isRemoteServicingConfigured() == false)
            {
                isConfigured = mConnectService.configureRemoteServicing(String::getEmptyString());
            }
            else
            {
                isConfigured = true;
            }

            if (isConfigured)
            {
                mConnectService.startRemoteServicing();
            }
        }
        break;

    case ServiceManagerEventData::eServiceManagerCommands::CMD_StartNetConnection:
        {
            String   ipAddress;
            unsigned short portNr = 0;
            stream >> ipAddress;
            stream >> portNr;

            mConnectService.enableRemoteServicing( true );
            mConnectService.setRemoteServiceAddress(ipAddress, portNr);
            if (mConnectService.isRemoteServicingConfigured())
            {
                mConnectService.startRemoteServicing();
            }
        }
        break;

    case ServiceManagerEventData::eServiceManagerCommands::CMD_StopConnection:
        {
            mConnectService.stopRemoteServicing();
        }
        break;

    case ServiceManagerEventData::eServiceManagerCommands::CMD_SetEnableService:
        {
            bool enable = false;
            stream >> enable;
            mConnectService.enableRemoteServicing(enable);
        }
        break;

    case ServiceManagerEventData::eServiceManagerCommands::CMD_RegisterConnection:
        {
            for (ServerList::MAPPOS posMap = mServerList.firstPosition(); mServerList.isValidPosition(posMap); posMap = mServerList.nextPosition(posMap) )
            {
                const StubAddress & server = mServerList.keyAtPosition(posMap).getAddress();
                const ClientList & clientList = mServerList.valueAtPosition(posMap);

                if (server.isServicePublic() && server.isLocalAddress() && server.isValid())
                {
                    mConnectService.registerService(server);
                }

                for (ClientList::LISTPOS pos = clientList.firstPosition(); clientList.isValidPosition(pos); pos = clientList.nextPosition(pos))
                {
                    const ProxyAddress & proxy = clientList.valueAtPosition(pos).getAddress();
                    if (proxy.isServicePublic() && proxy.isLocalAddress() && proxy.isValid())
                    {
                        mConnectService.registerServiceClient(proxy);
                    }
                }
            }
        }
        break;

    case ServiceManagerEventData::eServiceManagerCommands::CMD_UnregisterConnection:
    case ServiceManagerEventData::eServiceManagerCommands::CMD_LostConnection:
        {
            TEArrayList<StubAddress> stubList;
            TEArrayList<ProxyAddress> proxyList;
            for (ServerList::MAPPOS posMap = mServerList.firstPosition(); mServerList.isValidPosition(posMap); posMap = mServerList.nextPosition(posMap) )
            {
                const StubAddress & server = mServerList.keyAtPosition(posMap).getAddress();
                const ClientList & clientList = mServerList.valueAtPosition(posMap);

                if (server.isServicePublic() && server.isRemoteAddress() && server.isValid())
                {
                    stubList.add(server);
                }

                for (ClientList::LISTPOS pos = clientList.firstPosition(); clientList.isValidPosition(pos); pos = clientList.nextPosition(pos))
                {
                    const ProxyAddress & proxy = clientList.valueAtPosition(pos).getAddress();
                    if (proxy.isServicePublic() && proxy.isRemoteAddress() && proxy.isValid())
                    {
                        proxyList.add(proxy);
                    }
                }
            }

            for (uint32_t i = 0; i < stubList.getSize(); ++i)
            {
                _unregisterServer(stubList[i]);
            }

            for (uint32_t i = 0; i < proxyList.getSize(); ++i)
            {
                _unregisterClient(proxyList[i]);
            }
        }
        break;

    case ServiceManagerEventData::eServiceManagerCommands::CMD_TerminateComponentThread:
        {
            String threadName;
            stream >> threadName;
            if (_terminateComponentThread(threadName))
            {
                ServiceManager::_requestCreateThread(threadName);
            }
        }
        break;

    case ServiceManagerEventData::eServiceManagerCommands::CMD_StartComponentThread:
        {
            String threadName;
            stream >> threadName;
            _startComponentThread(threadName);
        }
        break;

    default:
        ASSERT(false);
        break;
    }
}

bool ServiceManager::postEvent(Event & eventElem)
{
    bool result = false;
    if (RUNTIME_CAST(&eventElem, ServiceManagerEvent) != nullptr)
    {
        result = EventDispatcher::postEvent(eventElem);
    }
    else
    {
        OUTPUT_ERR("Not a ServiceManagerEvent type event, cannot Post. Destroying event type [ %s ]", eventElem.getRuntimeClassName().getString());
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
        if ( createThread(NECommon::WAIT_INFINITE) && waitForDispatcherStart( NECommon::WAIT_INFINITE ) )
        {
            result = true;
        }
        else
        {
            OUTPUT_ERR("Failed to create [ %s ] Service Manager thread.", SERVICE_MANAGER_THREAD_NAME.data());
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
    ServiceManagerEvent::sendEvent( ServiceManagerEventData::shutdownServiceManager()
                                  , static_cast<IEServiceManagerEventConsumer &>(self())
                                  , static_cast<DispatcherThread &>(self()));
    Thread::switchThread( );
    completionWait( NECommon::WAIT_INFINITE );
}

bool ServiceManager::_terminateComponentThread(const String& threadName)
{
    TRACE_SCOPE(areg_component_private_ServiceManager__terminateComponentThread);
    
    bool result{ false };

    Thread * thread = Thread::findThreadByName(threadName);
    ComponentThread* compThread = RUNTIME_CAST(thread, ComponentThread);
    if (compThread != nullptr)
    {
        TRACE_WARN("Terminating component thread [ %s ]", compThread->getName().getString());
        result = true;
        compThread->terminateSelf();
    }
    else
    {
        TRACE_INFO("Was not able to find component thread [ %s ] to terminate", threadName.getString());
    }

    return result;
}

inline void ServiceManager::_startComponentThread(const String& threadName)
{
    TRACE_SCOPE(areg_component_private_ServiceManager__startComponentThread);

    const NERegistry::ComponentThreadEntry& entry = ComponentLoader::findThreadEntry(threadName);
    Thread* thread = Thread::findThreadByName(threadName);
    if (entry.isValid() && (thread == nullptr))
    {
        ComponentThread* compThread = DEBUG_NEW ComponentThread(entry.mThreadName, entry.mWatchdogTimeout);
        if ((compThread != nullptr) && compThread->createThread(NECommon::WAIT_INFINITE))
        {
            TRACE_DBG("Succeeded to create and start component thread [ %s ]", threadName.getString());
        }
        else
        {
            TRACE_WARN("Failed to create and start component thread [ %s ]", threadName.getString());
        }
    }
    else
    {
        TRACE_ERR("The thread [ %s ] is registered in the system, ignoring to create seconds instance of the thread", threadName.getString());
    }
}


void ServiceManager::getServiceList( ITEM_ID cookie, TEArrayList<StubAddress> & OUT out_listStubs, TEArrayList<ProxyAddress> & OUT out_lisProxies ) const
{
    TRACE_SCOPE(areg_component_private_ServiceManager_getServiceList);
    Lock lock( mLock );

    out_listStubs.clear();
    out_lisProxies.clear();

    for (ServerList::MAPPOS posMap = mServerList.firstPosition(); mServerList.isValidPosition(posMap); posMap = mServerList.nextPosition(posMap) )
    {
        const StubAddress & server      = mServerList.keyAtPosition(posMap).getAddress();
        const ClientList & clientList   = mServerList.valueAtPosition(posMap);

        if ( server.isValid() && ((cookie == NEService::COOKIE_ANY) || (server.getCookie() == cookie)) )
        {
            TRACE_DBG("Found stub [ %s ] of cookie [ %u ]", StubAddress::convAddressToPath(server).getString(), static_cast<uint32_t>(cookie));
            out_listStubs.add(server);
        }

        for (ClientList::LISTPOS pos = clientList.firstPosition(); clientList.isValidPosition(pos); pos = clientList.nextPosition(pos))
        {
            const ProxyAddress & proxy = clientList.valueAtPosition(pos).getAddress();
            if ( proxy.isValid() && ((cookie == NEService::COOKIE_ANY) || (proxy.getCookie() == cookie)) )
            {
                TRACE_DBG("Found proxy [ %s ] of cookie [ %u ]", ProxyAddress::convAddressToPath(proxy).getString(), cookie);
                out_lisProxies.add(proxy);
            }
        }
    }

    TRACE_DBG("Found [ %d ] servers and [ %d ] proxies of cookie [ %u ]", out_listStubs.getSize(), out_lisProxies.getSize(), cookie);
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
    ServiceManagerEvent::sendEvent( ServiceManagerEventData::registerConnection(channel)
                                  , static_cast<IEServiceManagerEventConsumer &>(self())
                                  , static_cast<DispatcherThread &>(self()));
}

void ServiceManager::remoteServiceStopped(const Channel & channel)
{
    ServiceManagerEvent::sendEvent( ServiceManagerEventData::unregisterConnection(channel)
                                  , static_cast<IEServiceManagerEventConsumer &>(self())
                                  , static_cast<DispatcherThread &>(self()));
}

void ServiceManager::remoteServiceConnectionLost(const Channel & channel)
{
    ServiceManagerEvent::sendEvent( ServiceManagerEventData::lostConnection(channel)
                                  , static_cast<IEServiceManagerEventConsumer &>(self())
                                  , static_cast<DispatcherThread &>(self()));
}
