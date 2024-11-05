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
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Service Manager implementation.
 *
 ************************************************************************/
#include "areg/component/private/ServiceManager.hpp"

#include "areg/base/Process.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/NERegistry.hpp"
#include "areg/component/private/ServerList.hpp"

#include "areg/trace/GETrace.h"

#include <string_view>

DEF_TRACE_SCOPE(areg_component_private_ServiceManager_processEvent);
DEF_TRACE_SCOPE(areg_component_private_ServiceManager_requestRegisterServer);
DEF_TRACE_SCOPE(areg_component_private_ServiceManager_requestUnregisterServer);
DEF_TRACE_SCOPE(areg_component_private_ServiceManager_requestRegisterClient);
DEF_TRACE_SCOPE(areg_component_private_ServiceManager_requestUnregisterClient);
DEF_TRACE_SCOPE(areg_component_private_ServiceManager_requestRecreateThread);
DEF_TRACE_SCOPE(areg_component_private_ServiceManager_extractRemoteServiceAddresses);

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
    return getInstance()._startServiceManagerThread( );
}

void ServiceManager::_stopServiceManager(bool waitComplete)
{
    getInstance()._stopServiceManagerThread(waitComplete);
}

void ServiceManager::_waitServiceManager(void)
{
    getInstance()._waitServiceManagerThread();
}

bool ServiceManager::isServiceManagerStarted( void )
{
    return ServiceManager::getInstance().isReady();
}

void ServiceManager::queryCommunicationData( unsigned int & OUT sizeSend, unsigned int & OUT sizeReceive )
{
    ServiceManager & serviceManager = ServiceManager::getInstance( );
    sizeSend    = serviceManager.mServiceClient.queryBytesSent( );
    sizeReceive = serviceManager.mServiceClient.queryBytesReceived( );
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

void ServiceManager::requestUnregisterServer( const StubAddress & whichServer, const NEService::eDisconnectReason reason )
{
    TRACE_SCOPE(areg_component_private_ServiceManager_requestUnregisterServer);

    TRACE_DBG( "Request to unregister server [ %s ] of interface [ %s ]"
                    , whichServer.getRoleName( ).getString( )
                    , whichServer.getServiceName( ).getString( ) );
    
    ASSERT(whichServer.isValid());
    
    ServiceManager & serviceManager = ServiceManager::getInstance();
    ServiceManagerEvent::sendEvent( ServiceManagerEventData::unregisterStub(whichServer, reason)
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

void ServiceManager::requestUnregisterClient( const ProxyAddress & whichClient, const NEService::eDisconnectReason reason )
{
    TRACE_SCOPE(areg_component_private_ServiceManager_requestUnregisterClient);
    TRACE_DBG( "Request to register proxy [ %s ] of interface [ %s ]"
                    , whichClient.getRoleName( ).getString( )
                    , whichClient.getServiceName( ).getString( ) );
    
    ASSERT(whichClient.isValid());
    
    ServiceManager & serviceManager = ServiceManager::getInstance();
    ServiceManagerEvent::sendEvent( ServiceManagerEventData::unregisterProxy(whichClient, reason)
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

bool ServiceManager::_routingServiceConfigure( void )
{
    ServiceManager & serviceManager = ServiceManager::getInstance();
    ServiceManagerEventData data(ServiceManagerEventData::configureConnection(NERemoteService::eRemoteServices::ServiceRouter, static_cast<unsigned int>(NERemoteService::eConnectionTypes::ConnectTcpip)));

    return ServiceManagerEvent::sendEvent( data
                                         , static_cast<IEServiceManagerEventConsumer &>(serviceManager) 
                                         , static_cast<DispatcherThread &>(serviceManager));
}

bool ServiceManager::_routingServiceStart( unsigned int connectTypes )
{
    ServiceManager & serviceManager = ServiceManager::getInstance();
    ServiceManagerEventData data(ServiceManagerEventData::startConnection(NERemoteService::eRemoteServices::ServiceRouter, connectTypes));
    return ServiceManagerEvent::sendEvent( data
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

bool ServiceManager::_isRoutingServiceStarted(void)
{
    return ServiceManager::getInstance().getServiceConnectionProvider().isServiceHostConnected( );
}

bool ServiceManager::_isRoutingServiceConfigured(void)
{
    return ServiceManager::getInstance().getServiceConnectionProvider().isServiceHostSetup( );
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
    , IEServiceConnectionConsumer   ( )
    , IEServiceRegisterConsumer     ( )

    , mEventProcessor   ( self() )
    , mServiceClient    ( static_cast<IEServiceConnectionConsumer&>(self()), static_cast<IEServiceRegisterConsumer&>(self()) )
    , mLock             (  )
{
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

void ServiceManager::processEvent( const ServiceManagerEventData & data )
{
    TRACE_SCOPE(areg_component_private_ServiceManager_processEvent);
    ServiceManagerEventData::eServiceManagerCommands cmdService { data.getCommand( ) };
    TRACE_DBG( "Service Manager is going to execute command [ %s ]", ServiceManagerEventData::getString( cmdService ) );

    mEventProcessor.processServiceEvent( cmdService, data.getReadStream( ), getServiceConnectionProvider( ), getServiceRegisterProvider() );
}

bool ServiceManager::postEvent(Event & eventElem)
{
    return (RUNTIME_CAST(&eventElem, ServiceManagerEvent) != nullptr) && EventDispatcher::postEvent(eventElem);
}

void ServiceManager::readyForEvents( bool isReady )
{
    if ( isReady )
    {
        ServiceManagerEvent::addListener( static_cast<IEServiceManagerEventConsumer &>(self( )), static_cast<DispatcherThread &>(self( )) );
    }
    else
    {
        ServiceManagerEvent::removeListener( static_cast<IEServiceManagerEventConsumer &>(self( )), static_cast<DispatcherThread &>(self( )) );
    }

    DispatcherThread::readyForEvents( isReady );
}

bool ServiceManager::_startServiceManagerThread( void )
{
    Lock lock(mLock);
    ASSERT(isReady() || (isRunning() == false));
    return (isReady() || (createThread(NECommon::WAIT_INFINITE) && waitForDispatcherStart(NECommon::WAIT_INFINITE)));
}

void ServiceManager::_stopServiceManagerThread(bool waitComplete)
{
    ServiceManagerEvent::sendEvent( ServiceManagerEventData::shutdownServiceManager()
                                  , static_cast<IEServiceManagerEventConsumer &>(self())
                                  , static_cast<DispatcherThread &>(self()));

    if (waitComplete)
    {
        completionWait(NECommon::WAIT_INFINITE);
        shutdownThread(NECommon::DO_NOT_WAIT);
    }
}

void ServiceManager::_waitServiceManagerThread(void)
{
    completionWait(NECommon::WAIT_INFINITE);
    shutdownThread(NECommon::DO_NOT_WAIT);
}

void ServiceManager::extractRemoteServiceAddresses(const ITEM_ID & cookie, TEArrayList<StubAddress> & OUT out_listStubs, TEArrayList<ProxyAddress> & OUT out_lisProxies ) const
{
    TRACE_SCOPE(areg_component_private_ServiceManager_extractRemoteServiceAddresses);
    Lock lock( mLock );

    out_listStubs.clear();
    out_lisProxies.clear();

    const ServerList & serverList{ mEventProcessor.getRegisteredServiceList( ) };

    for (ServerList::MAPPOS posMap = serverList.firstPosition(); serverList.isValidPosition(posMap); posMap = serverList.nextPosition(posMap) )
    {
        const StubAddress & server      = serverList.keyAtPosition(posMap).getAddress();
        const ClientList & clientList   = serverList.valueAtPosition(posMap);

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

void ServiceManager::registeredRemoteServiceProvider( const StubAddress & stub )
{
    ServiceManager::requestRegisterServer(stub);
}

void ServiceManager::registeredRemoteServiceConsumer(const ProxyAddress & proxy)
{
    ServiceManager::requestRegisterClient(proxy);
}

void ServiceManager::unregisteredRemoteServiceProvider(const StubAddress & stub, NEService::eDisconnectReason reason, const ITEM_ID & /*cookie*/ /*= NEService::COOKIE_ANY*/ )
{
    ServiceManager::requestUnregisterServer(stub, reason);
}

void ServiceManager::unregisteredRemoteServiceConsumer(const ProxyAddress & proxy, NEService::eDisconnectReason reason, const ITEM_ID & /* cookie */ /*= NEService::COOKIE_ANY*/ )
{
    ServiceManager::requestUnregisterClient(proxy, reason);
}

void ServiceManager::connectedRemoteServiceChannel(const Channel & channel)
{
    ServiceManagerEvent::sendEvent( ServiceManagerEventData::registerConnection(channel)
                                  , static_cast<IEServiceManagerEventConsumer &>(self())
                                  , static_cast<DispatcherThread &>(self()));
}

void ServiceManager::disconnectedRemoteServiceChannel(const Channel & channel)
{
    ServiceManagerEvent::sendEvent( ServiceManagerEventData::unregisterConnection(channel)
                                  , static_cast<IEServiceManagerEventConsumer &>(self())
                                  , static_cast<DispatcherThread &>(self()));
}

void ServiceManager::lostRemoteServiceChannel(const Channel & channel)
{
    ServiceManagerEvent::sendEvent( ServiceManagerEventData::lostConnection(channel)
                                  , static_cast<IEServiceManagerEventConsumer &>(self())
                                  , static_cast<DispatcherThread &>(self()));
}
