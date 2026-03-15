#ifndef AREG_COMPONENT_PRIVATE_SERVICEMANAGER_HPP
#define AREG_COMPONENT_PRIVATE_SERVICEMANAGER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ServiceManager.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Service Manager declaration.
 *
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/areg_global.h"

#include "areg/component/DispatcherThread.hpp"
#include "areg/component/private/ServiceManagerEvents.hpp"
#include "areg/ipc/ConnectionConsumer.hpp"
#include "areg/ipc/RegistrationConsumer.hpp"

#include "areg/base/SyncPrimitives.hpp"
#include "areg/component/private/ServiceManagerEventProcessor.hpp"
#include "areg/ipc/private/RouterClient.hpp"
namespace areg {

/************************************************************************
 * Dependencies
 ************************************************************************/
class StubAddress;
class ProxyAddress;
class ServiceRequestEvent;
class ServiceResponseEvent;

//////////////////////////////////////////////////////////////////////////
// ServiceManager class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Singleton service manager module responsible for service registration, discovery, and
 *          client-server lifecycle management. Automatically registers all system proxies and
 *          servers, generates service availability events, manages connections, and coordinates the
 *          timer service.
 **/
class ServiceManager final  : private   DispatcherThread
                            , private   ServiceManagerEventConsumer
                            , private   ConnectionConsumer
                            , private   RegistrationConsumer
{
    friend class Application;
    friend class ServiceManagerEventProcessor;
//////////////////////////////////////////////////////////////////////////
// Declare Runtime
//////////////////////////////////////////////////////////////////////////
    AREG_DECLARE_RUNTIME(ServiceManager)

//////////////////////////////////////////////////////////////////////////
// Static public methods to access globally
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************/
// Service Manager Server and Client registration functions
/************************************************************************/

    /**
     * \brief   Registers a Stub server at Service Manager and notifies all proxy clients of service
     *          availability.
     *
     * \param   whichServer     The address of Stub Server object requesting registration.
     **/
    static void request_register_server( const StubAddress & whichServer );

    /**
     * \brief   Unregisters a Stub server from Service Manager and notifies all proxy clients of
     *          service unavailability.
     *
     * \param   whichServer     The address of Stub Server object requesting unregistration.
     * \param   reason          The reason for service unregistration.
     **/
    static void request_unregister_server( const StubAddress & whichServer, const areg::DisconnectReason reason );

    /**
     * \brief   Registers a Proxy client at Service Manager and notifies it of available Stub
     *          services.
     *
     * \param   whichClient     The address of Proxy client object requesting registration.
     **/
    static void request_register_client( const ProxyAddress & whichClient );

    /**
     * \brief   Unregisters a Proxy client from Service Manager and notifies the Stub server.
     *
     * \param   whichClient     The address of Proxy client object requesting unregistration.
     * \param   reason          The reason for client unregistration.
     **/
    static void request_unregister_client( const ProxyAddress & whichClient, const areg::DisconnectReason reason );

    /**
     * \brief   Terminates and restarts a component thread, stopping all associated workers,
     *          components, services, and proxies.
     *
     * \param   whichThread     The component thread to recreate.
     **/
    static void request_recreate_thread( const ComponentThread & whichThread );

    /**
     * \brief   Returns true if Service Manager is running and ready to process events.
     **/
    [[nodiscard]]
    static bool is_manager_started();

    /**
     * \brief   Queries and resets send and receive data counters since the last call.
     *
     * \param[out] sizeSend        On output contains the size of data in bytes sent since the last
     *                             call.
     * \param[out] sizeReceive     On output contains the size of data in bytes received since the
     *                             last call.
     **/
    static void query_communication_data( uint32_t & sizeSend, uint32_t & sizeReceive );

private:
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////

    ServiceManager();

    virtual ~ServiceManager() = default;

//////////////////////////////////////////////////////////////////////////
// Private static methods
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns the singleton ServiceManager instance.
     **/
    static inline ServiceManager & instance();

/************************************************************************/
// Service Manager start / stop functions
/************************************************************************/

    /**
     * \brief   Starts the Service Manager thread if not already running.
     *
     * \return  Returns true if Service Manager started or was already running and is ready to
     *          receive events.
     **/
    static bool _start_service_manager();

    /**
     * \brief   Stops Service Manager thread and optionally waits for completion.
     *
     * \param   waitComplete    If true, waits for Service Manager to complete jobs and exit.
     *                          Otherwise, triggers stop and returns immediately.
     **/
    static void _stop_service_manager( bool waitComplete);

    /**
     * \brief   Blocks the calling thread until Service Manager thread completes and exits.
     **/
    static void _wait_service_manager();

/************************************************************************/
// Message router client start / stop functions
/************************************************************************/

    /**
     * \brief   Configures the routing service client for remote connections.
     *
     * \return  Returns true if configuration succeeded; false otherwise.
     **/
    static bool _routing_service_configure();

    /**
     * \brief   Starts the routing service client connection.
     *
     * \return  Returns true if routing service client started successfully.
     **/
    static bool _routing_service_start(uint32_t connectTypes);

    /**
     * \brief   Starts connection to a remote routing service at the specified address.
     *
     * \param   ipAddress       The IP address of the remote routing service.
     * \param   portNr          The port number of the remote routing service.
     * \return  Returns true if routing service client started successfully.
     **/
    static bool _routing_service_start( const String & ipAddress, uint16_t portNr );

    /**
     * \brief   Stops the routing service client.
     **/
    static void _routing_service_stop();

    /**
     * \brief   Returns true if routing service client is started and ready to operate.
     **/
    static bool _is_routing_started();

    /**
     * \brief   Returns true if routing service client is started but not yet ready to operate.
     **/
    static bool _is_routing_pending();

    /**
     * \brief   Returns true if routing service client is configured and ready to start.
     **/
    static bool _is_routing_configured();

    /**
     * \brief   Generates an event to create and start a component thread. Internal use only.
     *
     * \param   componentThread     The name of the thread to create and start.
     **/
    static void _request_create_thread( const String & componentThread );

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// ServiceManagerEventConsumer overrides
/************************************************************************/

    /**
     * \brief   Processes a dispatched service manager event.
     *
     * \param   data    The event data object. Must have default constructor and assignment
     *                  operator.
     **/
    void process_event( const ServiceManagerEventData & data ) override;

/************************************************************************/
// EventRouter Interface overrides.
/************************************************************************/

    /**
     * \brief   Posts an event for delivery to its target thread or process.
     *
     * \param   eventElem       The event to post.
     * \return  Returns true if the event was delivered to its target successfully; false otherwise.
     **/
    bool post_event( Event & eventElem ) override;

    /**
     * \brief   Enables or disables event dispatching for receiver threads.
     *
     * \param   is_ready    True to enable event dispatching; false to disable.
     **/
    void ready_for_events( bool is_ready ) override;

/************************************************************************/
// ConnectionConsumer overrides
/************************************************************************/

    /**
     * \brief   Called when a remote service connection channel is established.
     *
     * \param   channel     The established communication channel.
     **/
    void on_service_channel_connected( const Channel & channel ) override;

    /**
     * \brief   Called when a remote service connection channel is disconnected.
     *
     * \param   channel     The disconnected communication channel.
     **/
    void on_service_channel_disconnected( const Channel & channel ) override;

    /**
     * \brief   Called when a remote service connection is lost unexpectedly (not explicitly
     *          stopped).
     *
     * \param   channel     The lost communication channel.
     **/
    void on_service_channel_lost( const Channel & channel ) override;

/************************************************************************/
// RegistrationConsumer overrides
/************************************************************************/

    /**
     * \brief   Extracts lists of remote service providers and consumers, optionally filtered by
     *          cookie.
     *
     * \param   cookie              The cookie to filter by; pass areg::COOKIE_ANY to include
     *                              all services.
     * \param[out] listProviders       On output contains the list of remote service provider
     *                                 addresses.
     * \param[out] listConsumer        On output contains the list of remote service consumer
     *                                 addresses.
     **/
    void extract_service_addresses(const ITEM_ID & cookie, ArrayList<StubAddress> & listProviders, ArrayList<ProxyAddress> & listConsumer ) const override;

    /**
     * \brief   Called when a remote service provider is registered in the system.
     *
     * \param   stub    The address of the registered remote service provider.
     **/
    void on_provider_registered( const StubAddress & stub ) override;

    /**
     * \brief   Called when a remote service consumer is registered in the system.
     *
     * \param   proxy       The address of the registered remote service consumer.
     **/
    void on_consumer_registered( const ProxyAddress & proxy ) override;

    /**
     * \brief   Called when a remote service provider is unregistered from the system.
     *
     * \param   stub        The address of the unregistered remote service provider.
     * \param   reason      The reason for unregistration.
     * \param   cookie      The cookie of the initiating source; ignored if areg::COOKIE_ANY.
     **/
    void on_provider_unregistered( const StubAddress & stub, areg::DisconnectReason reason, const ITEM_ID & cookie /*= areg::COOKIE_ANY*/ ) override;

    /**
     * \brief   Called when a remote service consumer is unregistered from the system.
     *
     * \param   proxy       The address of the unregistered remote service consumer.
     * \param   reason      The reason for unregistration.
     * \param   cookie      The cookie of the initiating source; ignored if areg::COOKIE_ANY.
     **/
    void on_consumer_unregistered( const ProxyAddress & proxy, areg::DisconnectReason reason, const ITEM_ID & cookie /*= areg::COOKIE_ANY*/ ) override;

//////////////////////////////////////////////////////////////////////////
// Operations and attributes
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns the remote service connection handler instance.
     **/
    inline ConnectionProvider& service_connection_provider();

    /**
     * \brief   Returns the remote service registration handler instance.
     **/
    inline RegistrationProvider& service_register_provider();

    /**
     * \brief   Starts the Service Manager thread and automatically starts the Timer Server.
     *
     * \return  Returns true if Service Manager thread started and is ready to receive events.
     **/
    bool _start_manager_thread();

    /**
     * \brief   Stops the Service Manager thread, Timer Server, and all pending services, optionally
     *          waiting for completion.
     *
     * \param   waitComplete    If true, waits for Service Manager to complete jobs. Otherwise,
     *                          triggers stop and returns immediately.
     **/
    void _stop_manager_thread( bool waitComplete);

    /**
     * \brief   Blocks the calling thread until Service Manager thread completes and exits.
     **/
    void _wait_manager_thread();

    /**
     * \brief   Returns a reference to this ServiceManager instance.
     **/
    inline ServiceManager & self();

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    ServiceManagerEventProcessor    mEventProcessor;
    /**
     * \brief   The connection service.
     **/
    RouterClient                    mServiceClient;
    /**
     * \brief   Synchronization object, for multi-threading access.
     **/
    mutable ResourceLock            mLock;

//////////////////////////////////////////////////////////////////////////
// Forbidden method calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( ServiceManager );
};

//////////////////////////////////////////////////////////////////////////
// ServiceManager class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline ConnectionProvider& ServiceManager::service_connection_provider()
{
    return static_cast<ConnectionProvider&>(mServiceClient);
}

inline RegistrationProvider& ServiceManager::service_register_provider()
{
    return static_cast<RegistrationProvider&>(mServiceClient);
}

inline ServiceManager & ServiceManager::self()
{
    return (*this);
}

} // namespace areg
#endif  // AREG_COMPONENT_PRIVATE_SERVICEMANAGER_HPP
