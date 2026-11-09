/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
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

#include <cstdlib>
#include <string_view>

namespace
{
    //!< Predefined Service Manager Thread name.
    constexpr std::string_view SERVICE_MANAGER_THREAD_NAME{ "_AREG_SERVICE_MANAGER_THREAD_" };
}

namespace areg {

DEF_LOG_SCOPE(areg_component_private_ServiceManager, process_event);
DEF_LOG_SCOPE(areg_component_private_ServiceManager, _wait_manager_thread);
DEF_LOG_SCOPE(areg_component_private_ServiceManager, request_register_provider);
DEF_LOG_SCOPE(areg_component_private_ServiceManager, request_unregister_provider);
DEF_LOG_SCOPE(areg_component_private_ServiceManager, request_register_consumer);
DEF_LOG_SCOPE(areg_component_private_ServiceManager, request_unregister_consumer);
DEF_LOG_SCOPE(areg_component_private_ServiceManager, request_load_model);
DEF_LOG_SCOPE(areg_component_private_ServiceManager, request_unload_model);
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

void ServiceManager::query_data_sent(uint64_t& sizeSent, uint32_t& msgSent) noexcept
{
    ServiceManager::instance().mServiceClient.query_data_sent(sizeSent, msgSent);
}

void ServiceManager::query_data_received(uint64_t& sizeRecv, uint32_t& msgRecv) noexcept
{
    ServiceManager::instance().mServiceClient.query_data_received(sizeRecv, msgRecv);
}

void ServiceManager::enable_data_rate(bool enable) noexcept
{
    ServiceManager::instance().mServiceClient.enable_data_rate(enable);
}

bool ServiceManager::send_raw_message(const MessageEnvelope& msg) noexcept
{
    return ServiceManager::instance().mServiceClient.send_raw_message(msg);
}

const Channel& ServiceManager::connection_channel() noexcept
{
    return ServiceManager::instance().mServiceClient.connection_channel();
}

void ServiceManager::request_register_provider( const StubAddress & whichServer )
{
    if ( SystemController::is_caller_abandoned( ) )
        return;

    LOG_SCOPE( areg_component_private_ServiceManager, request_register_provider);
    LOG_DBG("Request to register server [ %s ] of interface [ %s ]", whichServer.role_name().as_string(), whichServer.service_name().as_string());

    ASSERT(whichServer.is_valid());

    ServiceManager & serviceManager = ServiceManager::instance();
    ServiceManagerEvent::send_event( ServiceManagerEventData::register_stub(whichServer)
                                  , static_cast<ServiceManagerEventConsumer &>(serviceManager)
                                  , static_cast<DispatcherThread &>(serviceManager));
}

void ServiceManager::request_unregister_provider( const StubAddress & whichServer, const areg::DisconnectReason reason )
{
    if ( SystemController::is_caller_abandoned( ) )
        return;

    LOG_SCOPE( areg_component_private_ServiceManager, request_unregister_provider);
    LOG_DBG( "Request to unregister server [ %s ] of interface [ %s ]", whichServer.role_name( ).as_string( ), whichServer.service_name( ).as_string( ) );
    
    ASSERT(whichServer.is_valid());
    
    ServiceManager & serviceManager = ServiceManager::instance();
    ServiceManagerEvent::send_event( ServiceManagerEventData::unregister_stub(whichServer, reason)
                                  , static_cast<ServiceManagerEventConsumer &>(serviceManager)
                                  , static_cast<DispatcherThread &>(serviceManager));
}

void ServiceManager::request_register_consumer( const ProxyAddress & whichClient )
{
    if ( SystemController::is_caller_abandoned( ) )
        return;

    LOG_SCOPE( areg_component_private_ServiceManager, request_register_consumer);

    LOG_DBG( "Request to register proxy [ %s ] of interface [ %s ]", whichClient.role_name( ).as_string( ), whichClient.service_name( ).as_string( ) );
    
    ASSERT(whichClient.is_valid());
    
    ServiceManager & serviceManager = ServiceManager::instance();
    ServiceManagerEvent::send_event( ServiceManagerEventData::register_proxy(whichClient)
                                  , static_cast<ServiceManagerEventConsumer &>(serviceManager)
                                  , static_cast<DispatcherThread &>(serviceManager));
}

void ServiceManager::request_unregister_consumer( const ProxyAddress & whichClient, const areg::DisconnectReason reason )
{
    if ( SystemController::is_caller_abandoned( ) )
        return;

    LOG_SCOPE( areg_component_private_ServiceManager, request_unregister_consumer);
    LOG_DBG( "Request to unregister proxy [ %s ] of interface [ %s ]", whichClient.role_name( ).as_string( ), whichClient.service_name( ).as_string( ) );
    
    ASSERT(whichClient.is_valid());
    
    ServiceManager & serviceManager = ServiceManager::instance();
    ServiceManagerEvent::send_event( ServiceManagerEventData::unregister_proxy(whichClient, reason)
                                  , static_cast<ServiceManagerEventConsumer &>(serviceManager)
                                  , static_cast<DispatcherThread &>(serviceManager));
}

bool ServiceManager::request_load_model( const String & modelName, bool waitComponents )
{
    LOG_SCOPE( areg_component_private_ServiceManager, request_load_model );

    ServiceManager & serviceManager = ServiceManager::instance();
    if ( serviceManager.is_ready( ) == false )
    {
        LOG_ERR( "The service manager does not run, the model [ %s ] is not loaded", modelName.as_string( ) );
        return false;
    }

    const uint32_t modelId{ SystemController::find_model_id( modelName ) };
    if ( (Thread::current_thread( ) == static_cast<Thread *>(&serviceManager)) || SystemController::is_caller_in_model( modelName ) )
    {
        // A caller that belongs to the model does not wait.
        const ServiceManagerEventData data{ ServiceManagerEventData::model_request( true, modelId, 0u ) };
        return data.is_valid( ) && ServiceManager::_post_lifecycle_event( data );
    }

    return ServiceManager::_post_request( true, modelId, waitComponents );
}

bool ServiceManager::request_unload_model( const String & modelName, bool waitComplete )
{
    LOG_SCOPE( areg_component_private_ServiceManager, request_unload_model );

    ServiceManager & serviceManager = ServiceManager::instance();
    if ( serviceManager.is_ready( ) == false )
        return false;

    const uint32_t modelId{ SystemController::find_model_id( modelName ) };
    if ( (waitComplete == false) || (Thread::current_thread( ) == static_cast<Thread *>(&serviceManager)) || SystemController::is_caller_in_model( modelName ) )
    {
        const ServiceManagerEventData data{ ServiceManagerEventData::model_request( false, modelId, 0u ) };
        return data.is_valid( ) && ServiceManager::_post_lifecycle_event( data );
    }

    return ServiceManager::_post_request( false, modelId, false );
}

bool ServiceManager::build_thread_report( bool running, const ComponentThread & whichThread, Event & report )
{
    const ServiceManagerEventData data{ ServiceManagerEventData::thread_report( running, static_cast<uint64_t>(reinterpret_cast<uintptr_t>(&whichThread)) ) };
    if ( data.is_valid( ) == false )
        return false;

    ServiceManager & serviceManager = ServiceManager::instance();
    ServiceManagerEvent event( data, static_cast<ServiceManagerEventConsumer &>(serviceManager) );
    if ( event.is_valid( ) == false )
        return false;

    report = std::move( event );
    return true;
}

bool ServiceManager::post_thread_report( Event & report )
{
    Event event{ std::move( report ) };
    ServiceManager & serviceManager = ServiceManager::instance();
    return event.is_valid( ) && event.register_for_thread( static_cast<DispatcherThread *>(&serviceManager) ) && event.deliver_event( );
}

bool ServiceManager::report_watchdog_expired( uint64_t instance, bool isWorker )
{
    const ServiceManagerEventData data{ ServiceManagerEventData::watchdog_expired( instance, isWorker ) };
    return data.is_valid( ) && ServiceManager::_post_lifecycle_event( data );
}

bool ServiceManager::report_deadline_expired()
{
    return ServiceManager::_post_lifecycle_event( ServiceManagerEventData::deadline_expired( ) );
}

areg::ThreadState ServiceManager::thread_state( const String & threadName )
{
    return ServiceManager::instance( ).mController.thread_state( threadName );
}

bool ServiceManager::request_user_state( uint32_t userState )
{
    if ( ServiceManager::instance( ).is_ready( ) == false )
    {
        SystemController::store_user_state( userState );
        return true;
    }

    const ServiceManagerEventData data{ ServiceManagerEventData::user_state( userState ) };
    return data.is_valid( ) && ServiceManager::_post_lifecycle_event( data );
}

bool ServiceManager::_post_request( bool load, uint32_t modelId, bool waitComponents )
{
    SystemController::Request * request{ SystemController::Request::create( waitComponents ) };
    if ( request == nullptr )
        return false;

    const ServiceManagerEventData data{ ServiceManagerEventData::model_request( load, modelId, static_cast<uint64_t>(reinterpret_cast<uintptr_t>(request)) ) };
    if ( (data.is_valid( ) == false) || (ServiceManager::_post_lifecycle_event( data ) == false) )
    {
        request->release( );
        request->release( );
        return false;
    }

    return request->wait( );
}

bool ServiceManager::_post_lifecycle_event( const ServiceManagerEventData & data )
{
    ServiceManager & serviceManager = ServiceManager::instance();
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
    ServiceManager & serviceManager{ ServiceManager::instance() };
    return (serviceManager.mRouterStopping.load(std::memory_order_acquire) == false) && serviceManager.service_connection_provider().is_host_pending();
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
ServiceManager::ServiceManager()
    : DispatcherThread           ( SERVICE_MANAGER_THREAD_NAME, areg::SYSTEM_THREAD_STACK_NORMAL, areg::QUEUE_MANAGER_RING_CAPACITY )
    , ServiceManagerEventConsumer( )
    , ConnectionConsumer         ( )
    , RegistrationConsumer       ( )
    , RouterClientOwner          ( )

    , mEventProcessor   ( )
    , mController       ( )
    , mServiceClient    ( static_cast<ConnectionConsumer&>(self()), static_cast<RegistrationConsumer&>(self()), static_cast<RouterClientOwner&>(self()) )
    , mRouterStopping   ( false )
    , mDeferredStart    ( )
    , mExitAfterRouter  ( false )
    , mShutdownEvent    ( )
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

    if ( _process_router_command( data ) )
        return;

    const InStream & stream{ data.read_stream( ) };
    switch ( cmdService )
    {
    case ServiceManagerEventData::ServiceManagerCommand::CMD_LoadModel:
    case ServiceManagerEventData::ServiceManagerCommand::CMD_UnloadModel:
        {
            uint32_t modelId{ SystemController::MODEL_NONE };
            uint64_t holder{ 0u };
            stream >> modelId;
            stream >> holder;

            SystemController::Request * const request{ reinterpret_cast<SystemController::Request *>(static_cast<uintptr_t>(holder)) };

            if ( cmdService == ServiceManagerEventData::ServiceManagerCommand::CMD_LoadModel )
            {
                mController.load_model( modelId, request );
            }
            else
            {
                mController.unload_model( modelId, request );
            }
        }
        break;

    case ServiceManagerEventData::ServiceManagerCommand::CMD_ThreadRunning:
    case ServiceManagerEventData::ServiceManagerCommand::CMD_ThreadStopped:
        {
            uint64_t instance{ 0u };
            stream >> instance;
            if ( cmdService == ServiceManagerEventData::ServiceManagerCommand::CMD_ThreadRunning )
            {
                mController.thread_running( instance );
            }
            else
            {
                mController.thread_stopped( instance );
            }
        }
        break;

    case ServiceManagerEventData::ServiceManagerCommand::CMD_WatchdogExpired:
        {
            uint64_t instance{ 0u };
            uint32_t isWorker{ 0u };
            stream >> instance;
            stream >> isWorker;
            mController.watchdog_expired( instance, isWorker != 0u );
        }
        break;

    case ServiceManagerEventData::ServiceManagerCommand::CMD_DeadlineExpired:
        mController.deadline_expired( );
        break;

    case ServiceManagerEventData::ServiceManagerCommand::CMD_UserState:
        {
            uint32_t userState{ 0u };
            stream >> userState;
            mController.set_user_state( userState );
        }
        break;

    default:
        mEventProcessor.process_service_event( cmdService, stream, service_connection_provider( ), service_register_provider() );
        break;
    }
}

bool ServiceManager::post_event(Event & eventElem)
{
    return (eventElem.event_id() == ServiceManagerEvent::CLASS_ID) && EventDispatcher::post_event(eventElem);
}

void ServiceManager::ready_for_events( bool is_ready )
{
    if ( is_ready )
    {
        ServiceManagerEvent::add_listener( static_cast<ServiceManagerEventConsumer &>(self( )), static_cast<DispatcherThread &>(self( )) );
    }
    else
    {
        mController.stop( );
        ServiceManagerEvent::remove_listener( static_cast<ServiceManagerEventConsumer &>(self( )), static_cast<DispatcherThread &>(self( )) );
    }

    DispatcherThread::ready_for_events( is_ready );
}

bool ServiceManager::_start_manager_thread()
{
    if (is_ready())
        return true;

    ServiceManagerEvent shutdown( ServiceManagerEventData::shutdown_service_manager( ), static_cast<ServiceManagerEventConsumer &>(self( )) );
    if ((shutdown.is_valid() == false) || (mController.start() == false))
        return false;

    mShutdownEvent = std::move(shutdown);
    return (start(areg::DO_NOT_WAIT) && wait_start(areg::WAIT_INFINITE));
}

void ServiceManager::_stop_manager_thread(bool waitComplete)
{
    Event shutdown{ std::move(mShutdownEvent) };
    if (shutdown.is_valid() && shutdown.register_for_thread(static_cast<DispatcherThread *>(this)))
    {
        static_cast<void>(shutdown.deliver_event());
    }

    if (waitComplete)
    {
        _wait_manager_thread();
    }
}

void ServiceManager::_wait_manager_thread()
{
    LOG_SCOPE( areg_component_private_ServiceManager, _wait_manager_thread );
    if (wait_completion(areg::LIFECYCLE_TIMEOUT_DEFAULT) == false)
    {
        LOG_ERR("The service manager did not stop within [ %u ] ms, the process exits with the code [ %d ]"
                    , areg::LIFECYCLE_TIMEOUT_DEFAULT, areg::LIFECYCLE_FAILURE_EXIT_CODE);
        areg::stop_logging(true);
        std::_Exit(areg::LIFECYCLE_FAILURE_EXIT_CODE);
    }

    shutdown(areg::DO_NOT_WAIT);
}

bool ServiceManager::_process_router_command( const ServiceManagerEventData & data )
{
    switch ( data.command( ) )
    {
    case ServiceManagerEventData::ServiceManagerCommand::CMD_ShutdownService:
        mDeferredStart.reset( );
        mExitAfterRouter = true;
        if ( mRouterStopping.load( std::memory_order_acquire ) )
            return true;

        if ( mServiceClient.is_client_running( ) )
        {
            // The event processor requests the stop; the stop report ends this thread.
            mRouterStopping.store( true, std::memory_order_release );
            return false;
        }

        mEventProcessor.process_service_event( data.command( ), data.read_stream( ), service_connection_provider( ), service_register_provider( ) );
        trigger_exit_drained( );
        return true;

    case ServiceManagerEventData::ServiceManagerCommand::CMD_StopConnection:
        mDeferredStart.reset( );
        if ( mRouterStopping.load( std::memory_order_acquire ) || (mServiceClient.is_client_running( ) == false) )
            return true;

        mRouterStopping.store( true, std::memory_order_release );
        return false;

    case ServiceManagerEventData::ServiceManagerCommand::CMD_StartConnection:
    case ServiceManagerEventData::ServiceManagerCommand::CMD_StartNetConnection:
        if ( mRouterStopping.load( std::memory_order_acquire ) == false )
            return false;

        if ( mExitAfterRouter == false )
        {
            mDeferredStart.emplace( data );
        }

        return true;

    case ServiceManagerEventData::ServiceManagerCommand::CMD_RouterClientStopped:
        if ( mServiceClient.is_client_running( ) == false )
        {
            mServiceClient.release_client( );
        }

        mRouterStopping.store( false, std::memory_order_release );
        if ( mExitAfterRouter )
        {
            trigger_exit_drained( );
        }
        else if ( mDeferredStart.has_value( ) )
        {
            const ServiceManagerEventData start{ std::move( *mDeferredStart ) };
            mDeferredStart.reset( );
            process_event( start );
        }

        return true;

    default:
        return false;
    }
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
                                  , static_cast<DispatcherThread &>(self())
                                  , areg::EventPriority::HighPrio );
}

void ServiceManager::on_service_channel_disconnected(const Channel & channel)
{
    ServiceManagerEvent::send_event( ServiceManagerEventData::unregister_connection(channel)
                                  , static_cast<ServiceManagerEventConsumer &>(self())
                                  , static_cast<DispatcherThread &>(self())
                                  , areg::EventPriority::HighPrio );
}

void ServiceManager::on_service_channel_lost(const Channel & channel)
{
    ServiceManagerEvent::send_event( ServiceManagerEventData::lost_connection(channel)
                                  , static_cast<ServiceManagerEventConsumer &>(self())
                                  , static_cast<DispatcherThread &>(self())
                                  , areg::EventPriority::HighPrio );
}

void ServiceManager::on_router_client_stopped() noexcept
{
    ServiceManagerEvent::send_event( ServiceManagerEventData::router_client_stopped()
                                  , static_cast<ServiceManagerEventConsumer &>(self())
                                  , static_cast<DispatcherThread &>(self())
                                  , areg::EventPriority::HighPrio );
}

} // namespace areg
