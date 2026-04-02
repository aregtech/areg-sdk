/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ServiceManager.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Service Manager implementation.
 *
 ************************************************************************/
#include "areg/component/private/ServiceManager.hpp"

#include "areg/base/Process.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/Model.hpp"
#include "areg/component/private/ServerList.hpp"

#include "areg/logging/areg_log.h"

#include <string_view>

namespace
{
    /// Predefined Service Manager Thread name.
    constexpr std::string_view SERVICE_MANAGER_THREAD_NAME{ "_AREG_SERVICE_MANAGER_THREAD_" };
}

namespace areg {

DEF_LOG_SCOPE(areg_component_private_ServiceManager, process_event);
DEF_LOG_SCOPE(areg_component_private_ServiceManager, request_register_provider);
DEF_LOG_SCOPE(areg_component_private_ServiceManager, request_unregister_provider);
DEF_LOG_SCOPE(areg_component_private_ServiceManager, request_register_consumer);
DEF_LOG_SCOPE(areg_component_private_ServiceManager, request_unregister_consumer);
DEF_LOG_SCOPE(areg_component_private_ServiceManager, request_recreate_thread);
DEF_LOG_SCOPE(areg_component_private_ServiceManager, extract_service_addresses);

//////////////////////////////////////////////////////////////////////////
// ServiceManager class Implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Implement Runtime
//////////////////////////////////////////////////////////////////////////
AREG_IMPLEMENT_RUNTIME(ServiceManager, DispatcherThread)

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////

ServiceManager & ServiceManager::instance() noexcept
{
    static ServiceManager   _service_manager;
    return _service_manager;
}

bool ServiceManager::_start_service_manager()
{
    return instance()._start_manager_thread( );
}

void ServiceManager::_stop_service_manager(bool waitComplete)
{
    instance()._stop_manager_thread(waitComplete);
}

void ServiceManager::_wait_service_manager()
{
    instance()._wait_manager_thread();
}

bool ServiceManager::is_manager_started()
{
    return ServiceManager::instance().is_ready();
}

void ServiceManager::query_communication_data( uint32_t & sizeSend, uint32_t & sizeReceive )
{
    ServiceManager & serviceManager = ServiceManager::instance( );
    sizeSend    = serviceManager.mServiceClient.query_bytes_sent( );
    sizeReceive = serviceManager.mServiceClient.query_bytes_received( );
}

void ServiceManager::request_register_provider( const StubAddress & whichServer )
{
    LOG_SCOPE( areg_component_private_ServiceManager, request_register_provider);
    LOG_DBG("Request to register server [ %s ] of interface [ %s ]"
                    , whichServer.role_name().as_string()
                    , whichServer.service_name().as_string());

    ASSERT(whichServer.is_valid());

    ServiceManager & serviceManager = ServiceManager::instance();
    ServiceManagerEvent::send_event( ServiceManagerEventData::register_stub(whichServer)
                                  , static_cast<ServiceManagerEventConsumer &>(serviceManager)
                                  , static_cast<DispatcherThread &>(serviceManager));
}

void ServiceManager::request_unregister_provider( const StubAddress & whichServer, const areg::DisconnectReason reason )
{
    LOG_SCOPE( areg_component_private_ServiceManager, request_unregister_provider);

    LOG_DBG( "Request to unregister server [ %s ] of interface [ %s ]"
                    , whichServer.role_name( ).as_string( )
                    , whichServer.service_name( ).as_string( ) );
    
    ASSERT(whichServer.is_valid());
    
    ServiceManager & serviceManager = ServiceManager::instance();
    ServiceManagerEvent::send_event( ServiceManagerEventData::unregister_stub(whichServer, reason)
                                  , static_cast<ServiceManagerEventConsumer &>(serviceManager)
                                  , static_cast<DispatcherThread &>(serviceManager));
}

void ServiceManager::request_register_consumer( const ProxyAddress & whichClient )
{
    LOG_SCOPE( areg_component_private_ServiceManager, request_register_consumer);

    LOG_DBG( "Request to register proxy [ %s ] of interface [ %s ]"
                    , whichClient.role_name( ).as_string( )
                    , whichClient.service_name( ).as_string( ) );
    
    ASSERT(whichClient.is_valid());
    
    ServiceManager & serviceManager = ServiceManager::instance();
    ServiceManagerEvent::send_event( ServiceManagerEventData::register_proxy(whichClient)
                                  , static_cast<ServiceManagerEventConsumer &>(serviceManager)
                                  , static_cast<DispatcherThread &>(serviceManager));
}

void ServiceManager::request_unregister_consumer( const ProxyAddress & whichClient, const areg::DisconnectReason reason )
{
    LOG_SCOPE( areg_component_private_ServiceManager, request_unregister_consumer);
    LOG_DBG( "Request to unregister proxy [ %s ] of interface [ %s ]"
                    , whichClient.role_name( ).as_string( )
                    , whichClient.service_name( ).as_string( ) );
    
    ASSERT(whichClient.is_valid());
    
    ServiceManager & serviceManager = ServiceManager::instance();
    ServiceManagerEvent::send_event( ServiceManagerEventData::unregister_proxy(whichClient, reason)
                                  , static_cast<ServiceManagerEventConsumer &>(serviceManager)
                                  , static_cast<DispatcherThread &>(serviceManager));
}

void ServiceManager::request_recreate_thread(const ComponentThread& whichThread)
{
    LOG_SCOPE( areg_component_private_ServiceManager, request_recreate_thread );
    LOG_DBG("Request to re-create component thread [ %s ]", whichThread.name().as_string());

    ServiceManager & serviceManager = ServiceManager::instance();
    ServiceManagerEvent::send_event(ServiceManagerEventData::terminate_component_thread(whichThread.name())
                                  , static_cast<ServiceManagerEventConsumer &>(serviceManager)
                                  , static_cast<DispatcherThread &>(serviceManager));
}

bool ServiceManager::_routing_service_configure()
{
    ServiceManager & serviceManager = ServiceManager::instance();
    ServiceManagerEventData data(ServiceManagerEventData::configure_connection(areg::RemoteServiceKind::Router, static_cast<uint32_t>(areg::ConnectionType::Tcpip)));

    return ServiceManagerEvent::send_event( data
                                         , static_cast<ServiceManagerEventConsumer &>(serviceManager) 
                                         , static_cast<DispatcherThread &>(serviceManager));
}

bool ServiceManager::_routing_service_start( uint32_t connectTypes )
{
    ServiceManager & serviceManager = ServiceManager::instance();
    ServiceManagerEventData data(ServiceManagerEventData::start_connection(areg::RemoteServiceKind::Router, connectTypes));
    return ServiceManagerEvent::send_event( data
                                         , static_cast<ServiceManagerEventConsumer &>(serviceManager)
                                         , static_cast<DispatcherThread &>(serviceManager));
}

bool ServiceManager::_routing_service_start( const String & ipAddress, uint16_t portNr )
{
    bool result = false;
    if ( !ipAddress.is_empty() && (portNr != areg::InvalidPort) )
    {
        ServiceManager & serviceManager = ServiceManager::instance( );
        result = ServiceManagerEvent::send_event( ServiceManagerEventData::start_net_connection( ipAddress, portNr )
                                              , static_cast<ServiceManagerEventConsumer &>(serviceManager)
                                              , static_cast<DispatcherThread &>(serviceManager) );
    }
    return result;
}

void ServiceManager::_routing_service_stop()
{
    ServiceManager & serviceManager = ServiceManager::instance();
    ServiceManagerEvent::send_event( ServiceManagerEventData::stop_connection()
                                  , static_cast<ServiceManagerEventConsumer &>(serviceManager)
                                  , static_cast<DispatcherThread &>(serviceManager));
}

bool ServiceManager::_is_routing_started() noexcept
{
    return ServiceManager::instance().service_connection_provider().is_host_connected( );
}

bool ServiceManager::_is_routing_pending() noexcept
{
    return ServiceManager::instance().service_connection_provider().is_host_pending();
}

bool ServiceManager::_is_routing_configured() noexcept
{
    return ServiceManager::instance().service_connection_provider().is_host_setup( );
}

void ServiceManager::_request_create_thread(const String& componentThread)
{
    ServiceManager& serviceManager = ServiceManager::instance();
    ServiceManagerEvent::send_event( ServiceManagerEventData::create_component_thread(componentThread)
                                  , static_cast<ServiceManagerEventConsumer&>(serviceManager)
                                  , static_cast<DispatcherThread&>(serviceManager) );
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
ServiceManager::ServiceManager()
    : DispatcherThread           ( SERVICE_MANAGER_THREAD_NAME, areg::SYSTEM_THREAD_STACK_NORMAL, areg::QUEUE_SIZE_MAXIMUM )
    , ServiceManagerEventConsumer( )
    , ConnectionConsumer         ( )
    , RegistrationConsumer       ( )

    , mEventProcessor   ( self() )
    , mServiceClient    ( static_cast<ConnectionConsumer&>(self()), static_cast<RegistrationConsumer&>(self()) )
    , mLock             (  )
{
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

void ServiceManager::process_event( const ServiceManagerEventData & data )
{
    LOG_SCOPE( areg_component_private_ServiceManager, process_event );
    ServiceManagerEventData::ServiceManagerCommand cmdService { data.command( ) };
    LOG_DBG( "Service Manager is going to execute command [ %s ]", ServiceManagerEventData::as_string( cmdService ) );

    mEventProcessor.process_service_event( cmdService, data.read_stream( ), service_connection_provider( ), service_register_provider() );
}

bool ServiceManager::post_event(Event & eventElem)
{
    return (AREG_RUNTIME_CAST(&eventElem, ServiceManagerEvent) != nullptr) && EventDispatcher::post_event(eventElem);
}

void ServiceManager::ready_for_events( bool is_ready )
{
    if ( is_ready )
    {
        ServiceManagerEvent::add_listener( static_cast<ServiceManagerEventConsumer &>(self( )), static_cast<DispatcherThread &>(self( )) );
    }
    else
    {
        ServiceManagerEvent::remove_listener( static_cast<ServiceManagerEventConsumer &>(self( )), static_cast<DispatcherThread &>(self( )) );
    }

    DispatcherThread::ready_for_events( is_ready );
}

bool ServiceManager::_start_manager_thread()
{
    Lock lock(mLock);
    ASSERT(is_ready() || (is_running() == false));
    return (is_ready() || (start(areg::WAIT_INFINITE) && wait_start(areg::WAIT_INFINITE)));
}

void ServiceManager::_stop_manager_thread(bool waitComplete)
{
    ServiceManagerEvent::send_event( ServiceManagerEventData::shutdown_service_manager()
                                  , static_cast<ServiceManagerEventConsumer &>(self())
                                  , static_cast<DispatcherThread &>(self()));

    if (waitComplete)
    {
        wait_completion(areg::WAIT_INFINITE);
        shutdown(areg::DO_NOT_WAIT);
    }
}

void ServiceManager::_wait_manager_thread()
{
    wait_completion(areg::WAIT_INFINITE);
    shutdown(areg::DO_NOT_WAIT);
}

void ServiceManager::extract_service_addresses(const ITEM_ID & cookie, ArrayList<StubAddress> & listProviders, ArrayList<ProxyAddress> & listConsumers ) const
{
    LOG_SCOPE( areg_component_private_ServiceManager, extract_service_addresses );
    Lock lock( mLock );

    listProviders.clear();
    listConsumers.clear();

    const ServerList & serverList{ mEventProcessor.registered_service_list( ) };

    for (ServerList::MAPPOS posMap = serverList.first_position(); serverList.is_valid_position(posMap); posMap = serverList.next_position(posMap) )
    {
        const StubAddress & server      = serverList.key_at(posMap).address();
        const ClientList & clientList   = serverList.value_at(posMap);

        if ( server.is_valid() && ((cookie == areg::COOKIE_ANY) || (server.cookie() == cookie)) )
        {
            LOG_DBG("Found stub [ %s ] of cookie [ %u ]", StubAddress::to_path(server).as_string(), static_cast<uint32_t>(cookie));
            listProviders.add(server);
        }

        for (ClientList::LISTPOS pos = clientList.first_position(); clientList.is_valid_position(pos); pos = clientList.next_position(pos))
        {
            const ProxyAddress & proxy = clientList.value_at(pos).address();
            if ( proxy.is_valid() && ((cookie == areg::COOKIE_ANY) || (proxy.cookie() == cookie)) )
            {
                LOG_DBG("Found proxy [ %s ] of cookie [ %u ]", ProxyAddress::to_path(proxy).as_string(), cookie);
                listConsumers.add(proxy);
            }
        }
    }

    LOG_DBG("Found [ %d ] servers and [ %d ] proxies of cookie [ %u ]", listProviders.size(), listConsumers.size(), cookie);
}

void ServiceManager::on_provider_registered( const StubAddress & stub )
{
    ServiceManager::request_register_provider(stub);
}

void ServiceManager::on_consumer_registered(const ProxyAddress & proxy)
{
    ServiceManager::request_register_consumer(proxy);
}

void ServiceManager::on_provider_unregistered(const StubAddress & stub, areg::DisconnectReason reason, const ITEM_ID & /*cookie*/ /*= areg::COOKIE_ANY*/ )
{
    ServiceManager::request_unregister_provider(stub, reason);
}

void ServiceManager::on_consumer_unregistered(const ProxyAddress & proxy, areg::DisconnectReason reason, const ITEM_ID & /* cookie */ /*= areg::COOKIE_ANY*/ )
{
    ServiceManager::request_unregister_consumer(proxy, reason);
}

void ServiceManager::on_service_channel_connected(const Channel & channel)
{
    ServiceManagerEvent::send_event( ServiceManagerEventData::register_connection(channel)
                                  , static_cast<ServiceManagerEventConsumer &>(self())
                                  , static_cast<DispatcherThread &>(self()));
}

void ServiceManager::on_service_channel_disconnected(const Channel & channel)
{
    ServiceManagerEvent::send_event( ServiceManagerEventData::unregister_connection(channel)
                                  , static_cast<ServiceManagerEventConsumer &>(self())
                                  , static_cast<DispatcherThread &>(self()));
}

void ServiceManager::on_service_channel_lost(const Channel & channel)
{
    ServiceManagerEvent::send_event( ServiceManagerEventData::lost_connection(channel)
                                  , static_cast<ServiceManagerEventConsumer &>(self())
                                  , static_cast<DispatcherThread &>(self()));
}

} // namespace areg
