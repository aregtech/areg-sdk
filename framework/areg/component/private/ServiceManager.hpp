#ifndef AREG_COMPONENT_PRIVATE_SERVICEMANAGER_HPP
#define AREG_COMPONENT_PRIVATE_SERVICEMANAGER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
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
#include "areg/component/private/SystemController.hpp"
#include "areg/ipc/private/RouterClient.hpp"
namespace areg {

/************************************************************************
 * Dependencies
 ************************************************************************/
class StubAddress;
class ProxyAddress;
class ServiceRequestEvent;
class ServiceResponseEvent;
class MessageEnvelope;
class Channel;

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
    friend class SystemController;
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
     * \brief   Registers a Stub server at Service Manager and notifies all proxy clients
     *          of service availability.
     *
     * \param   whichServer     The address of Stub Server object requesting registration.
     **/
    static void request_register_provider( const StubAddress & whichServer );

    /**
     * \brief   Unregisters a Stub server from Service Manager and notifies all proxy clients
     *          of service unavailability.
     *
     * \param   whichServer     The address of Stub Server object requesting unregistration.
     * \param   reason          The reason for service unregistration.
     **/
    static void request_unregister_provider( const StubAddress & whichServer, const areg::DisconnectReason reason );

    /**
     * \brief   Registers a Proxy client at Service Manager and notifies it of available Stub services.
     *
     * \param   whichClient     The address of Proxy client object requesting registration.
     **/
    static void request_register_consumer( const ProxyAddress & whichClient );

    /**
     * \brief   Unregisters a Proxy client from Service Manager and notifies the Stub server.
     *
     * \param   whichClient     The address of Proxy client object requesting unregistration.
     * \param   reason          The reason for client unregistration.
     **/
    static void request_unregister_consumer( const ProxyAddress & whichClient, const areg::DisconnectReason reason );

/************************************************************************/
// Lifecycle of the models and of the component threads
/************************************************************************/

    /**
     * \brief   Asks the system controller to load the model, or all models if the name is empty,
     *          and waits for the result. Called from a thread of the model or from the service
     *          manager thread, the request is posted and not waited for.
     * \param   modelName       The name of the model.
     * \param   waitComponents  If true, waits until the components started; otherwise until the threads started.
     * \return  True if the model is loaded, or the request was posted without waiting.
     **/
    static bool request_load_model( const String & modelName, bool waitComponents );

    /**
     * \brief   Asks the system controller to unload the model, or all models if the name is empty.
     * \param   modelName       The name of the model.
     * \param   waitComplete    If true, waits until the model is unloaded. Called from a thread
     *                          of the model or from the service manager thread, it never waits.
     **/
    static void request_unload_model( const String & modelName, bool waitComplete );

    /**
     * \brief   Reports that the components of the component thread started.
     **/
    static void report_thread_running( const ComponentThread & whichThread );

    /**
     * \brief   Reports that the component thread exited. Called as its last act.
     **/
    static void report_thread_stopped( const ComponentThread & whichThread );

    /**
     * \brief   Reports that the watchdog of a component thread or of one of its workers expired.
     * \param   threadName  The name of the component thread.
     * \param   instance    The address of the component thread object.
     * \param   guardedName The name of the thread whose event took too long.
     **/
    static void report_watchdog_expired( const String & threadName, uint64_t instance, const String & guardedName );

    /**
     * \brief   Reports that the lifecycle deadline guard expired.
     **/
    static void report_deadline_expired();

    /**
     * \brief   Returns the state of the component thread of the name.
     **/
    [[nodiscard]]
    static areg::ThreadState thread_state( const String & threadName );

    /**
     * \brief   Asks the system controller to set the user state and to announce it to every
     *          component thread. Without a running service manager the value is only stored.
     **/
    static void request_user_state( uint32_t userState );

    /**
     * \brief   Returns true if Service Manager is running and ready to process events.
     **/
    [[nodiscard]]
    static bool is_manager_started();

    /**
     * \brief   Queries the amount of sent data in bytes and sent messages since the last call, and resets counters.
     *
     * \param[out] sizeSend     On output, contains the size of data in bytes sent since the last call.
     * \param[out] msgSent      On output, contains the size of messages sent since the last call.
     * \note    Accumulate calls to measure total data. Call each second to measure data rate.
     **/
    static void query_data_sent(uint64_t& sizeSent, uint32_t& msgSent) noexcept;

    /**
     * \brief   Queries the amount of received data in bytes and received messages since the last call, and resets counters.
     *
     * \param[out] sizeSend     On output, contains the size of data in bytes received since the last call.
     * \param[out] msgSent      On output, contains the size of messages received since the last call.
     * \note    Accumulate calls to measure total data. Call each second to measure data rate.
     **/
    static void query_data_received(uint64_t& sizeRecv, uint32_t& msgRecv) noexcept;

    /**
     * \brief   Enables or disables data and message rate verbosity.
     **/
    static void enable_data_rate(bool enable) noexcept;

    /**
     * \brief   Sends a pre-serialized MessageEnvelope directly to the IPC send thread,
     *          bypassing all event dispatch and serialization overhead.
     *          The caller is responsible for ensuring the message was built while the
     *          connection was valid and the target cookie is still active.
     *
     * \param   msg     The pre-built message to send.
     * \return  Returns true if the message was accepted by the send thread.
     **/
    static bool send_raw_message(const MessageEnvelope& msg) noexcept;

    /**
     * \brief   Returns the active IPC connection channel used to route messages
     *          to the message router.  Valid only after the connection handshake
     *          completes; check is_manager_started() before using.
     *
     * \return  Const reference to the current Channel object.
     **/
    static const Channel& connection_channel() noexcept;

private:
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////

    ServiceManager();

    ~ServiceManager() override = default;

//////////////////////////////////////////////////////////////////////////
// Private static methods
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns the singleton ServiceManager instance.
     **/
    [[nodiscard]]
    static ServiceManager & instance() noexcept;

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
     * \brief   Starts the routing service client connection.
     *
     * \return  Returns true if routing service client started successfully.
     **/
    static bool _routing_service_start(uint32_t connectTypes);

    /**
     * \brief   Stops the routing service client.
     **/
    static void _routing_service_stop();

    /**
     * \brief   Returns true if routing service client is started and ready to operate.
     **/
    [[nodiscard]]
    static bool _is_routing_started() noexcept;

    /**
     * \brief   Returns true if routing service client is started but not yet ready to operate.
     **/
    [[nodiscard]]
    static bool _is_routing_pending() noexcept;


    /**
     * \brief   Posts a lifecycle event to the service manager thread.
     **/
    static bool _post_lifecycle_event( const ServiceManagerEventData & data );

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// ServiceManagerEventConsumer overrides
/************************************************************************/

    /**
     * \brief   Processes a dispatched service manager event.
     *
     * \param   data    The event data object. Must have default constructor and assignment operator.
     **/
    void process_event( const ServiceManagerEventData & data ) final;

/************************************************************************/
// EventRouter Interface overrides.
/************************************************************************/

    /**
     * \brief   Posts an event for delivery to its target thread or process.
     *
     * \param   eventElem       The event to post.
     * \return  Returns true if the event was delivered to its target successfully; false otherwise.
     **/
    bool post_event( Event & eventElem ) final;

    /**
     * \brief   Enables or disables event dispatching for receiver threads.
     *
     * \param   is_ready    True to enable event dispatching; false to disable.
     **/
    void ready_for_events( bool is_ready ) final;

/************************************************************************/
// ConnectionConsumer overrides
/************************************************************************/

    /**
     * \brief   Called when a remote service connection channel is established.
     *
     * \param   channel     The established communication channel.
     **/
    void on_service_channel_connected( const Channel & channel ) final;

    /**
     * \brief   Called when a remote service connection channel is disconnected.
     *
     * \param   channel     The disconnected communication channel.
     **/
    void on_service_channel_disconnected( const Channel & channel ) final;

    /**
     * \brief   Called when a remote service connection is lost unexpectedly (not explicitly stopped).
     *
     * \param   channel     The lost communication channel.
     **/
    void on_service_channel_lost( const Channel & channel ) final;

/************************************************************************/
// RegistrationConsumer overrides
/************************************************************************/

    /**
     * \brief   Extracts lists of remote service providers and consumers, optionally filtered by cookie.
     *
     * \param      cookie           The cookie to filter by; pass areg::COOKIE_ANY to include all services.
     * \param[out] listProviders    On output contains the list of remote service provider addresses.
     * \param[out] listConsumers    On output contains the list of remote service consumer addresses.
     **/
    void extract_service_addresses(const ITEM_ID & cookie, ArrayList<StubAddress> & listProviders, ArrayList<ProxyAddress> & listConsumers ) const final;

    /**
     * \brief   Called when a remote service provider is registered in the system.
     *
     * \param   stub    The address of the registered remote service provider.
     **/
    void on_provider_registered( const StubAddress & stub ) final;

    /**
     * \brief   Called when a remote service consumer is registered in the system.
     *
     * \param   proxy       The address of the registered remote service consumer.
     **/
    void on_consumer_registered( const ProxyAddress & proxy ) final;

    /**
     * \brief   Called when a remote service provider is unregistered from the system.
     *
     * \param   stub        The address of the unregistered remote service provider.
     * \param   reason      The reason for unregistration.
     * \param   cookie      The cookie of the initiating source; ignored if areg::COOKIE_ANY.
     **/
    void on_provider_unregistered( const StubAddress & stub, areg::DisconnectReason reason, const ITEM_ID & cookie /*= areg::COOKIE_ANY*/ ) final;

    /**
     * \brief   Called when a remote service consumer is unregistered from the system.
     *
     * \param   proxy       The address of the unregistered remote service consumer.
     * \param   reason      The reason for unregistration.
     * \param   cookie      The cookie of the initiating source; ignored if areg::COOKIE_ANY.
     **/
    void on_consumer_unregistered( const ProxyAddress & proxy, areg::DisconnectReason reason, const ITEM_ID & cookie /*= areg::COOKIE_ANY*/ ) final;

//////////////////////////////////////////////////////////////////////////
// Operations and attributes
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns the remote service connection handler instance.
     **/
    [[nodiscard]]
    inline ConnectionProvider& service_connection_provider() noexcept;

    /**
     * \brief   Returns the remote service registration handler instance.
     **/
    [[nodiscard]]
    inline RegistrationProvider& service_register_provider() noexcept;

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
    [[nodiscard]]
    inline ServiceManager & self() noexcept;


//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    ServiceManagerEventProcessor    mEventProcessor;
    /**
     * \brief   The owner of the models and of the component threads.
     **/
    SystemController                mController;
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

inline ConnectionProvider& ServiceManager::service_connection_provider() noexcept
{
    return static_cast<ConnectionProvider&>(mServiceClient);
}

inline RegistrationProvider& ServiceManager::service_register_provider() noexcept
{
    return static_cast<RegistrationProvider&>(mServiceClient);
}

inline ServiceManager & ServiceManager::self() noexcept
{
    return (*this);
}

} // namespace areg
#endif  // AREG_COMPONENT_PRIVATE_SERVICEMANAGER_HPP
