#ifndef AREG_COMPONENT_PROXYBASE_HPP
#define AREG_COMPONENT_PROXYBASE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/ProxyBase.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Proxy Base class.
 *              All Proxy classes should derive from this class and
 *              extend functionalities.
 *              This class is pure virtual and cannot be instantiated.
 *              Derive this class and override pure virtual methods.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/CommonDefs.hpp"
#include "areg/base/HashMap.hpp"
#include "areg/base/ArrayList.hpp"
#include "areg/base/ResourceListMap.hpp"
#include "areg/base/ResourceMap.hpp"
#include "areg/component/ProxyEvent.hpp"
#include "areg/component/ProxyAddress.hpp"

#include "areg/component/ServiceDefs.hpp"
#include "areg/component/StubAddress.hpp"

#include <atomic>
#include <functional>
#include <memory>

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {
    class NotificationConsumer;
    class NotificationEventData;
    class ServiceResponseEvent;
    class RemoteResponseEvent;
    class ServiceRequestEvent;
    class NotificationEvent;
    class DispatcherThread;
    class SharedBuffer;
    class ProxyListener;
    class ProxyEvent;
    class ProxyBase;
    class Version;
}

/************************************************************************
 * Global types
 ************************************************************************/
/**
 * \brief   Function type to create a Proxy object.
 *          roleName    The role name of servicing component to connect.
 *          ownerThread The instance of thread to dispatch messages.
 *                      If nullptr, uses current component thread.
 **/
using FuncCreateProxy = std::function< areg::ProxyBase* ( const areg::String & /*roleName*/, areg::DispatcherThread * /*ownerThread*/ ) >;

namespace areg {
//////////////////////////////////////////////////////////////////////////
// ProxyBase class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Base class for proxy objects that mediate client communication with remote services.
 *          Manages client-server connections, notification subscriptions, response routing, and
 *          attribute caching across multiple clients within a single thread.
 **/
class AREG_API ProxyBase  : public    ProxyEventConsumer
{
    friend class RemoteEventFactory;

    static constexpr uint32_t CONNECTION_ID{ static_cast<uint32_t>(areg::FuncIdRange::ResponseServiceProviderConnection) };

//////////////////////////////////////////////////////////////////////////
// Internal classes, types and constants
//////////////////////////////////////////////////////////////////////////
private:
    //////////////////////////////////////////////////////////////////////////
    // ProxyBase::Listener class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Internal listener object for forwarding response and update notifications to
     *          clients; contains message ID, sequence number, and client reference.
     **/
    struct AREG_API Listener
    {
    //////////////////////////////////////////////////////////////////////////
    // ProxyBase::Listener class, Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        Listener() = default;

        Listener(const Listener& src) = default;

        Listener(Listener&& src) = default;

        ~Listener() = default;

        /**
         * \brief   Initializes listener with message ID and sequence number.
         **/
        inline constexpr Listener( const SequenceNumber & seqNr ) noexcept;

        /**
         * \brief   Initializes listener with message ID, sequence number, and client callback.
         **/
        inline constexpr Listener(const SequenceNumber & seqNr, NotificationConsumer * caller) noexcept;

    // ProxyBase::Listener class, Basic operators
    //////////////////////////////////////////////////////////////////////////
    public:

        ProxyBase::Listener & operator = ( const ProxyBase::Listener & src ) noexcept = default;

        ProxyBase::Listener & operator = ( ProxyBase::Listener && src ) noexcept = default;

        [[nodiscard]]
        inline constexpr bool operator == ( const ProxyBase::Listener & other ) const noexcept;

        [[nodiscard]]
        inline constexpr bool operator != ( const ProxyBase::Listener & other ) const noexcept;

    //////////////////////////////////////////////////////////////////////////
    // Member variables
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Sequence number of listener. Attribute update listeners should have zero sequence number
         **/
        SequenceNumber          mSequenceNr{ areg::SEQUENCE_NUMBER_NOTIFY };
        /**
         * \brief   Pointer to notification event listener object, which should be instance of Proxy client.
         **/
        NotificationConsumer*   mListener{ nullptr };
    };

    //////////////////////////////////////////////////////////////////////////
    // ProxyBase::ProxyListenerList and ProxyListenerMap definitions
    //////////////////////////////////////////////////////////////////////////
    /************************************************************************
     * \brief   Flat list of listeners for a single message ID.
     ************************************************************************/
    using ProxyListenerList = ArrayList<ProxyBase::Listener>;

    //////////////////////////////////////////////////////////////////////////
    // ProxyBase::ProxyConnectList definition
    //////////////////////////////////////////////////////////////////////////
    /************************************************************************
     * \brief   Proxy Connected client List class to handle connect and 
     *          disconnect service.
     ************************************************************************/
    using ProxyConnectList  = ArrayList<ProxyListener *>;

    /************************************************************************
     * \brief   HashMap keyed by message ID mapping to per-ID listener lists.
     *          Provides O(1) average lookup in prepare_listeners(), replacing
     *          the O(n) flat-scan across all registered listeners.
     ************************************************************************/
    using ProxyListenerMap  = HashMap<uint32_t, ProxyListenerList>;

    //////////////////////////////////////////////////////////////////////////
    // ProxyBase::ProxyMap class declaration.
    //////////////////////////////////////////////////////////////////////////
    /************************************************************************
     * \details Proxy map is used to keep control of instantiated resource.
     *          As well as to lookup for already existing instance or proxy
     *          in the same thread. As a Key, it is using Proxy Address
     *          and value is instance of Proxy.
     ************************************************************************/
    /**
     * \brief   Proxy hash map
     **/
    using MapProxy          = HashMap<uint32_t, std::shared_ptr<ProxyBase>>;
    /**
     * \brief   Proxy resource map helper.
     **/
    using ImplProxyResource = ResourceMapImpl<uint32_t, std::shared_ptr<ProxyBase>>;

    /**
     * \brief   ProxyBase::MapProxyResource
     *          Proxy Resource Map declaration to keep controlling of all instantiated Proxy objects.
     *          ProxyAddress  The Key of Resource map is a Proxy address object.
     *          ProxyBase     The Values are pointers of Proxy object.
     *          ProxyMap      The type of Hash Mapping object used as container
     **/
    using MapProxyResource  = ConcurrentResourceMap<uint32_t, std::shared_ptr<ProxyBase>, MapProxy, ImplProxyResource>;

    //////////////////////////////////////////////////////////////////////////
    // ProxyBase::ThreadProxyList internal class declaration
    //////////////////////////////////////////////////////////////////////////
    /************************************************************************
     * \brief   The list of proxies. Used to save in Map List.
     ************************************************************************/
    using ThreadProxyList   = ArrayList<std::shared_ptr<ProxyBase>>;

    //////////////////////////////////////////////////////////////////////////
    // ProxyBase::ImplThreadProxyMap internal class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Helper class for managing maps of thread-local proxy lists.
     **/
    class ImplThreadProxyMap : public ResourceListMapImpl<uint32_t, std::shared_ptr<ProxyBase>, ThreadProxyList>
    {
    public:
        /**
         * \brief   Called when all resources are removed from the map.
         **/
        inline void impl_clean_list( const uint32_t & /* Key */, ThreadProxyList & /* List */ ) noexcept
        {
        }

        /**
         * \brief   Adds a proxy object to the given list.
         *
         * \param   List        The list of proxy objects to add to.
         * \param   Resource    The proxy object to add.
         **/
        inline void impl_add_resource( ThreadProxyList & List, std::shared_ptr<ProxyBase> Resource )
        {
            if ( Resource != nullptr )
            {
                List.add_if_unique( Resource );
            }
        }
        
        /**
         * \brief   Removes a proxy object from the given list.
         *
         * \param   List        The list of proxy objects to remove from.
         * \param   Resource    The proxy object to remove.
         * \return  True if the resource was found and removed; false otherwise.
         **/
        inline bool impl_remove_resource( ThreadProxyList & List, std::shared_ptr<ProxyBase> Resource ) noexcept
        {
            return (Resource != nullptr ? List.remove_elem( Resource, 0 ) : false);
        }
    };

    /**
     * \brief   The integer hash map which values are list of proxies.
     **/
    using MapThreadProxy    = IntegerHashMap<ThreadProxyList>;

    /**
     * \brief   The Map of the list, where the key is a magic number and values are list of proxies.
     **/
    using MapThreadProxyList= ConcurrentResourceListMap<uint32_t, std::shared_ptr<ProxyBase>, ThreadProxyList, MapThreadProxy, ImplThreadProxyMap>;

protected:
    //////////////////////////////////////////////////////////////////////////
    // ProxyBase::ServiceAvailableEvent constants
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Internal constant defining the minimum delay in milliseconds
     *          when processing service available event. The event delay is used
     *          in the cases if a thread already has a proxy, and the new instance
     *          of a client is created in other thread, but for the thread
     *          where the proxy runs. This triggers service available event, which may
     *          processed earlier than the client object is created, so that the
     *          wrong service available method can be called.
     **/
    static constexpr uint32_t MINIMAL_DELAY_TIME_MS { areg::WAIT_10_MILLISECONDS };

    static constexpr uint32_t MAXIMAL_DELAY_TIME_MS { 250 * 10 };

    //////////////////////////////////////////////////////////////////////////
    // ProxyBase::ServiceAvailableEvent internal class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Event sent to notify a client when a service becomes available, even if instantiated
     *          in a different thread.
     **/
    class AREG_API ServiceAvailableEvent : public Event
    {
    //////////////////////////////////////////////////////////////////////////
    // Constructor/ Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Initializes the event to notify the given consumer of service availability.
         **/
        explicit ServiceAvailableEvent( NotificationConsumer & consumer );

        ServiceAvailableEvent(const ServiceAvailableEvent& /*src*/) = default;

        ServiceAvailableEvent(ServiceAvailableEvent&& /*src*/) noexcept = default;

        ~ServiceAvailableEvent() override = default;

    //////////////////////////////////////////////////////////////////////////
    // Attributes
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Returns the notification consumer to receive the event.
         **/
        [[nodiscard]]
        inline NotificationConsumer& consumer() const noexcept;

        /**
         * \brief   Sets the delay before the service available event is processed.
         *
         * \param   msDelay     Delay in milliseconds. Minimum is 10ms (MINIMAL_DELAY_TIME_MS)
         *                      and the maximum delay is 2500ms / 2.5s (MAXIMAL_DELAY_TIME_MS).
         *                      Use 0 for no delay. The delay allows the client object to be fully created.
         *                      The delay is computed to modulo 10. Meaning, 11ms delay is computed to 10,
         *                      19ms delay is computed 10ms.
         **/
        inline void set_event_delay(uint32_t msDelay) noexcept;

        /**
         * \brief   Returns the service available event delay timeout in milliseconds.
         **/
        [[nodiscard]]
        inline uint32_t event_delay() const noexcept;

        /**
         * \brief   Returns true if the service available event should be delayed.
         **/
        [[nodiscard]]
        inline bool should_delay_event() const noexcept;

    //////////////////////////////////////////////////////////////////////////
    // Forbidden calls
    //////////////////////////////////////////////////////////////////////////
    private:
        ServiceAvailableEvent() = delete;
    };

//////////////////////////////////////////////////////////////////////////
// ProxyBase class static methods
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Finds or creates a proxy object, registering it in the current thread or specified
     *          owner thread, and increments the reference count on subsequent calls.
     *
     * \param   roleName            The role name of the server component.
     * \param   serviceIfData       The service interface data containing name and version.
     * \param   connect             The listener to notify when server accepts the proxy connection.
     * \param   funcCreate          The function pointer to instantiate the proxy object if not yet
     *                              registered.
     * \param   ownerThread         The dispatcher thread name where messages are dispatched. If
     *                              empty, searches in the current thread.
     * \return  Returns a shared pointer to the proxy object.
     **/
    [[nodiscard]]
    static std::shared_ptr<ProxyBase> acquire_proxy( const String & roleName
                                                   , const areg::InterfaceData & serviceIfData
                                                   , ProxyListener & connect
                                                   , FuncCreateProxy funcCreate
                                                   , const String & ownerThread = String::empty_string() );

    /**
     * \brief   Finds or creates a proxy object, registering it in the current thread or specified
     *          owner thread, and increments the reference count on subsequent calls.
     *
     * \param   roleName            The role name of the server component.
     * \param   serviceIfData       The service interface data containing name and version.
     * \param   connect             The listener to notify when server accepts the proxy connection.
     * \param   funcCreate          The function pointer to instantiate the proxy object if not yet
     *                              registered.
     * \param   ownerThread         The dispatcher thread unique number. If 0, searches in the current thread.
     * \return  Returns a shared pointer to the proxy object.
     **/
    [[nodiscard]]
    static std::shared_ptr<ProxyBase> acquire_proxy( const String & roleName
                                                   , const areg::InterfaceData & serviceIfData
                                                   , ProxyListener & connect
                                                   , FuncCreateProxy funcCreate
                                                   , const UniqueNumber ownerThread );

    /**
     * \brief   Finds or creates a proxy object in the specified dispatcher thread, and increments
     *          the reference count on subsequent calls.
     *
     * \param   roleName            The role name of the server component.
     * \param   serviceIfData       The service interface data containing name and version.
     * \param   connect             The listener to notify when server accepts the proxy connection.
     * \param   funcCreate          The function pointer to instantiate the proxy object if not yet
     *                              registered.
     * \param   ownerThread         The dispatcher thread where messages are dispatched.
     * \return  Returns a shared pointer to the proxy object.
     * \note    Overload that takes a thread reference instead of thread name.
     **/
    [[nodiscard]]
    static std::shared_ptr<ProxyBase> acquire_proxy( const String & roleName
                                                   , const areg::InterfaceData & serviceIfData
                                                   , ProxyListener & connect
                                                   , FuncCreateProxy funcCreate
                                                   , DispatcherThread & ownerThread );

    /**
     * \brief   Searches the proxy registry and returns the proxy object at the specified address.
     *
     * \param   proxyAddress    The address of the proxy object to find.
     * \return  Returns a shared pointer to the proxy object if found; otherwise returns nullptr.
     **/
    [[nodiscard]]
    static std::shared_ptr<ProxyBase> find_proxy( const ProxyAddress & proxyAddress );

    /**
     * \brief   Populates the output list with all proxy objects registered in the specified thread.
     *
     * \param   ownerThread         The thread from which to retrieve proxies.
     * \param[out] threadProxyList     Receives the list of proxies created in the specified thread.
     * \return  Returns the number of proxies added to the list.
     **/
    static int32_t find_thread_proxies( DispatcherThread & ownerThread, ArrayList<std::shared_ptr<ProxyBase>> & threadProxyList );

    /**
     * \brief   Creates a remote response event indicating request delivery or processing failure.
     *          Returns an invalid Event if no proxy is found for the given target address.
     *
     * \param   target      The address of the target proxy to receive the failure event.
     * \param   msgId       The message ID of the failed request.
     * \param   errCode     The error code indicating the failure reason.
     * \param   seqNr       The sequence number associated with the request.
     * \return  A RemoteResponseEvent value; check is_valid() before use.
     **/
    [[nodiscard]]
    static RemoteResponseEvent request_failure_event(const ProxyAddress & target, uint32_t msgId, areg::ResultType errCode, const SequenceNumber & seqNr);

    /**
     * \brief   Acquires a lock on proxy resources for thread-safe access to the proxy registry.
     **/
    static inline void lock_resource() noexcept;

    /**
     * \brief   Releases the lock on proxy resources.
     **/
    static inline void unlock_resource() noexcept;

//////////////////////////////////////////////////////////////////////////
// ProxyBase class, Constructor / Destructor.
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initializes the proxy with the server component role name, service interface data,
     *          and optional owner thread.
     *
     * \param   roleName            The role name of the server component to connect to.
     * \param   serviceIfData       The service interface structure containing interface metadata.
     * \param   ownerThread         The thread that owns this proxy and dispatches its messages. If
     *                              nullptr, uses the current thread.
     **/
    ProxyBase( const String & roleName, const areg::InterfaceData & serviceIfData, DispatcherThread * ownerThread = nullptr );

public:
    virtual ~ProxyBase() = default;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns the address of this proxy.
     **/
    [[nodiscard]]
    inline const ProxyAddress & proxy_address() const noexcept;

    /**
     * \brief   Returns the address of the target stub service.
     **/
    [[nodiscard]]
    inline const StubAddress & stub_address() const noexcept;

    /**
     * \brief   Returns true if the server has accepted the proxy connection; false otherwise.
     **/
    [[nodiscard]]
    inline bool is_connected() const noexcept;

    /**
     * \brief   Returns the current connection status of the proxy.
     **/
    [[nodiscard]]
    inline areg::ServiceConnectionState connection_status() const noexcept;

    /**
     * \brief   Returns true if at least one listener is registered for the specified message ID.
     *
     * \param   msgId       The message ID to check for listeners.
     * \return  Returns true if listeners are assigned for the message ID; false otherwise.
     **/
    [[nodiscard]]
    inline bool has_any_listener(uint32_t msgId) const noexcept;

    /**
     * \brief   Returns true if at least one listener is registered for attribute update
     *          notifications of the specified message ID.
     *
     * \param   msgId       The attribute or response message ID to check for notification
     *                      listeners.
     * \return  Returns true if notification listeners are assigned for the message ID; false
     *          otherwise.
     **/
    [[nodiscard]]
    inline bool has_notification_listener(uint32_t msgId) const noexcept;

    /**
     * \brief   Returns the dispatcher thread that owns this proxy.
     **/
    [[nodiscard]]
    inline DispatcherThread & proxy_dispatcher_thread() const noexcept;

#ifdef DEBUG

    /**
     * \brief   Returns the number of listeners registered in this proxy.
     **/
    [[nodiscard]]
    inline uint32_t listener_count() const noexcept;

#endif // DEBUG


//////////////////////////////////////////////////////////////////////////
// Public Operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Decrements the proxy reference count. When the count reaches zero, unregisters the
     *          proxy, notifies the server of disconnection, and deletes the proxy.
     *
     * \param   connect     The listener to notify when the proxy is fully disconnected.
     **/
    void free_proxy( ProxyListener & connect );

    /**
     * \brief   Stops the proxy when the owning thread completes. Notifies all clients of
     *          disconnection, removes all listeners, and releases resources.
     **/
    void stop_proxy();

    /**
     * \brief   Forces immediate termination of the proxy. The proxy becomes inoperable after this
     *          call.
     **/
    void terminate_self();

    /**
     * \brief   Registers or updates a notification listener. Requests the stub to start
     *          notifications if this is the first listener, or sends an immediate update if
     *          listeners already exist.
     *
     * \param   msgId           The notification message ID (attribute or response).
     * \param   caller          The notification consumer to register.
     * \param   alwaysNotify    If true, sends a notification even if one is already pending; if
     *                          false, skips redundant notifications.
     **/
    void set_notification(uint32_t msgId, NotificationConsumer* caller, bool alwaysNotify = false);

    /**
     * \brief   Removes all notification listeners for the specified consumer and message ID.
     *
     * \param   msgId       The notification message ID.
     * \param   caller      The notification consumer to remove.
     **/
    void clear_notification(uint32_t msgId, NotificationConsumer* caller);

    /**
     * \brief   Clears all notifications for the specified listener and unregisters it.
     * \param   caller      The notification consumer object to unregister.
     **/
    inline void clear_all_notifications(NotificationConsumer& caller);

protected:
/************************************************************************/
// ProxyEventConsumer interface overrides. Should be implemented
/************************************************************************/

    /**
     * \brief   Processes a response event received from the stub. Must be overridden by derived
     *          classes.
     *
     * \param   eventElem       The service response event containing response data and metadata
     *                          from the stub.
     **/
    void process_response_event(ServiceResponseEvent & eventElem) override = 0;

    /**
     * \brief   Processes an attribute update event from the stub. Must be overridden by derived
     *          classes.
     *
     * \param   eventElem       The service response event containing the updated attribute value.
     **/
    void process_attribute_event(ServiceResponseEvent & eventElem) override = 0;

/************************************************************************/
// ProxyBase overrides. Should be implemented.
/************************************************************************/

    /**
     * \brief   Creates a service availability event for a new client. Must be overridden by derived
     *          classes.
     *
     * \param   consumer    The consumer to receive the service availability event.
     * \return  Returns a valid pointer to the created event; otherwise nullptr.
     **/
    [[nodiscard]]
    virtual ProxyBase::ServiceAvailableEvent create_service_available( NotificationConsumer & consumer ) = 0;

    /**
     * \brief   Creates a notification event to deliver to client objects. Must be overridden by derived classes.
     *
     * \param   data    The notification event data containing client notification information.
     * \return  Returns the newly created notification event object.
     **/
    [[nodiscard]]
    virtual NotificationEvent create_client_notification( const NotificationEventData & data ) const = 0;

    /**
     * \brief   Creates a request event to send to the stub. Must be overridden by derived classes.
     *
     * \param   args        The buffer containing serialized request arguments.
     * \param   reqId       The ID of the request call.
     * \return  Returns a valid pointer to the created request event.
     **/
    [[nodiscard]]
    virtual ServiceRequestEvent create_request( const areg::SharedBuffer & args, uint32_t reqId ) = 0;

    /**
     * \brief   Creates a request event to start or stop receiving update notifications. Must be
     *          overridden by derived classes.
     *
     * \param   msgId       The message ID to subscribe or unsubscribe. Should be an attribute or
     *                      response ID.
     * \param   reqType     The type of notification request (subscribe or unsubscribe).
     * \return  Returns a valid pointer to the created request event.
     **/
    [[nodiscard]]
    virtual ServiceRequestEvent create_notification_request( uint32_t msgId, areg::RequestType reqType ) = 0;

    /**
     * \brief   Creates an error response event when a remote request fails to reach the target.
     *          Base returns an invalid RemoteResponseEvent. Override in generated proxies.
     *
     * \param   addrProxy       The address of the proxy that sent the failed request.
     * \param   msgId           The message ID of the failed request.
     * \param   reason          The failure reason code.
     * \param   seqNr           The sequence number of the failed request.
     * \return  A RemoteResponseEvent value; check is_valid() before use.
     **/
    [[nodiscard]]
    virtual RemoteResponseEvent create_request_failed( const ProxyAddress & addrProxy, uint32_t msgId, areg::ResultType reason, const SequenceNumber & seqNr ) const;

/************************************************************************/
// ProxyEventConsumer interface overrides.
/************************************************************************/
    /**
     * \brief   Processes a proxy-specific event. Called when a proxy event is dispatched.
     *
     * \param   eventElem       The proxy event to process.
     **/
    void process_proxy_event( ProxyEvent & eventElem ) override;

    /**
     * \brief   Processes a generic event that is not a proxy event. Called when such an event is
     *          dispatched.
     *
     * \param   eventElem       The generic event to process.
     **/
    void process_generic_event( Event & eventElem ) override;

    /**
     * \brief   Called when the server connection status changes. Updates proxy connection state.
     *
     * \param   server      The address of the stub service.
     * \param   channel     The communication channel used for delivery.
     * \param   status      The new connection status. Must be Connected to send messages.
     **/
    void service_connection_updated( const StubAddress & server, const Channel & channel, areg::ServiceConnectionState status ) override;

/************************************************************************/
// ProxyBase interface overrides
/************************************************************************/

    /**
     * \brief   Processes a service availability notification for a client with optional delay.
     *
     * \param   consumer        The consumer to notify of service availability.
     * \param   delayEvent      The delay in milliseconds before processing the availability event.
     **/
    virtual void process_available_event( NotificationConsumer & consumer, uint32_t delayEvent );

    /**
     * \brief   Removes a listener and all its associated notifications. Notifies the stub to stop
     *          sending events if no other listeners remain.
     *
     * \param   consumer    The listener to unregister.
     **/
    virtual void unregister_listener( NotificationConsumer * consumer );

    /**
     * \brief   Creates and sends a notification event to a specific consumer. Can be overridden to
     *          add custom behavior.
     *
     * \param   msgId       The message ID of the notification.
     * \param   resType     The result type to include in the notification.
     * \param   seqNr       The sequence number used to identify the listener.
     * \param   caller      The consumer to be notified.
     **/
    virtual void send_notification_event( uint32_t msgId, areg::ResultType resType, const SequenceNumber & seqNr, NotificationConsumer * caller );

    /**
     * \brief   Registers all service event listeners. Called when the proxy is instantiated.
     *          Derived classes should override to register listeners.
     **/
    virtual void register_service_listeners();

    /**
     * \brief   Unregisters all service event listeners. Called when the proxy is freed. Derived
     *          classes should override to unregister listeners.
     **/
    virtual void unregister_service_listeners() noexcept;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Sends a request to the stub to start sending update notifications for the specified
     *          message ID.
     *
     * \param   msgId       The attribute or response message ID to start receiving updates for.
     **/
    inline void start_notification( uint32_t msgId );

    /**
     * \brief   Sends a request to the stub to stop sending update notifications for the specified
     *          message ID.
     *
     * \param   msgId       The attribute or response message ID to stop receiving updates for.
     **/
    inline void stop_notification( uint32_t msgId );

    /**
     * \brief   Stops all attribute and response update notifications from the stub.
     **/
    inline void stop_all_notifications();

    /**
     * \brief   Stops update notifications for a list of specified message IDs.
     *
     * \param   notifyIds       The array of attribute or response message IDs to stop receiving
     *                          updates for.
     * \param   count           The number of IDs in the array.
     **/
    inline void stop_notifications( const uint32_t notifyIds[], int32_t count );

    /**
     * \brief   Returns read-only access to the proxy data containing attribute and response state.
     **/
    [[nodiscard]]
    inline const areg::ProxyData & proxy_data() const noexcept;

    /**
     * \brief   Returns modifiable access to the proxy data containing attribute and response state.
     * \note    Non-const overload allowing modification.
     **/
    [[nodiscard]]
    inline areg::ProxyData & proxy_data() noexcept;

    /**
     * \brief   Registers the proxy to receive events of the specified type.
     *
     * \param   eventClass      The runtime class ID of the event type to register for.
     **/
    inline void register_for_event( const uint32_t eventClass );

    /**
     * \brief   Unregisters the proxy from receiving events of the specified type.
     *
     * \param   eventClass      The runtime class ID of the event type to unregister from.
     **/
    inline void unregister_for_event( const uint32_t eventClass ) noexcept;

    /**
     * \brief   Removes a specific listener from the listener list.
     *
     * \param   msgId       The message ID of the listener.
     * \param   seqNr       The sequence number of the listener.
     * \param   caller      The notification consumer to remove.
     **/
    inline void remove_listener( uint32_t msgId, const SequenceNumber & seqNr, NotificationConsumer * caller ) noexcept;

    /**
     * \brief   Adds a listener to the listener list, optionally checking for duplicates.
     *
     * \param   msgId       The message ID of the listener.
     * \param   seqNr       The sequence number of the listener.
     * \param   caller      The notification consumer to add.
     * \param   unique      If true, adds only if the listener does not already exist; if false,
     *                      always adds.
     * \return  Returns true if the listener was added; false if it already existed and unique was
     *          true.
     **/
    inline bool add_listener( uint32_t msgId, const SequenceNumber & seqNr, NotificationConsumer * caller, bool unique );

    /**
     * \brief   Sets the data state for the specified message ID in the proxy data.
     *
     * \param   msgId       The message ID whose state should be updated.
     * \param   newState    The new data state to set.
     **/
    inline void set_state( uint32_t msgId, areg::DataState newState ) noexcept;

    /**
     * \brief   Updates the proxy connection status.
     **/
    inline void set_connection_status(areg::ServiceConnectionState status) noexcept;

    /**
     * \brief   Sends notifications to all listeners registered for the specified message ID and
     *          sequence number.
     *
     * \param   respId              The message ID to notify.
     * \param   result              The result code to include in notifications.
     * \param   seqNrToSearch       The sequence number to match. If SEQUENCE_NUMBER_NOTIFY,
     *                              notifies all listeners for the message ID.
     **/
    void notify_listeners( uint32_t respId, areg::ResultType result, const SequenceNumber & seqNrToSearch );

    /**
     * \brief   Collects all listeners matching the message ID and sequence number into the output
     *          list.
     *
     * \param[out] out_listenerList    Receives the list of listeners matching the criteria.
     * \param   msgId               The message ID to filter listeners.
     * \param   seqNrToSearch       The sequence number to filter listeners. If
     *                              SEQUENCE_NUMBER_NOTIFY, matches all listeners.
     * \return  Returns the number of listeners added to the output list.
     **/
    uint32_t prepare_listeners( ProxyBase::ProxyListenerList & out_listenerList, uint32_t msgId, const SequenceNumber & seqNrToSearch );

    /**
     * \brief   Sends a request event to the stub with serialized arguments and an optional response
     *          listener.
     *
     * \param   reqId       The ID of the request.
     * \param   args        The buffer containing serialized request arguments. Can be empty if the
     *                      request has no parameters.
     * \param   caller      The consumer to receive the response. Must be non-null if the request
     *                      has a response; can be null if the request is one-way.
     **/
    void send_request_event( uint32_t reqId, const areg::SharedBuffer & args, NotificationConsumer * caller );

    void send_request_event(ServiceRequestEvent& reqEvent, NotificationConsumer* caller);

    /**
     * \brief   Sends a request to the stub to start or stop sending update notifications.
     *
     * \param   msgId       The attribute or response message ID to subscribe or unsubscribe.
     * \param   reqType     The request type indicating whether to start or stop notifications.
     **/
    void send_notify_request( uint32_t msgId, areg::RequestType reqType );

    void send_notify_request(ServiceRequestEvent& notifyEvent);

    /**
     * \brief   Returns true if the specified consumer is registered in the listener list.
     **/
    [[nodiscard]]
    bool is_listener_registered( NotificationConsumer & caller ) const;

    /**
     * \brief   Sends a service availability event to a client.
     *
     * \param   eventInstance       The service availability event to send.
     **/
    void send_service_event( ProxyBase::ServiceAvailableEvent& eventInstance );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   The address of Proxy
     **/
    ProxyAddress            mProxyAddress;

    /**
     * \brief   The address of Implemented Stub Service Interface
     **/
    StubAddress             mStubAddress;

    /**
     * \brief   Sequence number counter. Changed on ever request send
     **/
    SequenceNumber          mSequenceCount;

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

    /**
     * \brief   Per-message-ID listener map. O(1) average lookup replaces the former O(n) flat scan.
     *          Each sub-vector holds all listeners registered for a specific message ID.
     **/
    ProxyListenerMap        mListenerMap;

    /**
     * \brief   The list of connected clients of the proxy.
     **/
    ProxyConnectList        mListConnect;

    /**
     * \brief   Proxy instance reference counter.
     *          On every request to start Proxy, this counter will
     *          increase value. On request to free Proxy, it will
     *          decrease value. And when reaches zero, will delete
     *          Proxy object.
     **/
    std::atomic_uint32_t    mProxyInstCount;

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif  // _MSC_VER

    /**
     * \brief   Flag, indicating whether the proxy is stopped or not.
     *          Stopped proxy is inactive and cannot neither receive, nor respond on message.
     *          The stopped proxy should be recreated again. This flag for internal use.
     **/
    bool                    mIsStopped;

    /**
     * \brief   Proxy data, containing service interface information
     *          attribute and parameter update state.
     **/
    areg::ProxyData         mProxyData;

    /**
     * \brief   The Proxy dispatcher thread object
     **/
    DispatcherThread &      mDispatcherThread;

private:

    /**
     * \brief   Indicates the Service connection status.
     **/
    areg::ServiceConnectionState   mConnectionStatus;

    /**
     * \brief   Flag, indicating whether the proxy is connected or not.
     **/
    bool                    mIsConnected;

//////////////////////////////////////////////////////////////////////////
// Hidden calls
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns a reference to this proxy object.
     **/
    [[nodiscard]]
    inline ProxyBase & self() noexcept;

    /**
     * \brief   Returns the global registry of all proxy objects in the system.
     **/
    [[nodiscard]]
    static MapProxyResource&     map_proxies() noexcept;

    /**
     * \brief   Returns the map of thread-local proxy lists indexed by dispatcher thread.
     **/
    [[nodiscard]]
    static MapThreadProxyList&   thread_proxies() noexcept;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief
     * \note    Default constructor is deleted; use parameterized constructor instead.
     **/
    ProxyBase() = delete;
    AREG_NOCOPY_NOMOVE( ProxyBase );
};

//////////////////////////////////////////////////////////////////////////
// ProxyBase::Listener class implementation
//////////////////////////////////////////////////////////////////////////

inline constexpr ProxyBase::Listener::Listener(const SequenceNumber& seqNr) noexcept
    : mSequenceNr(seqNr)
    , mListener(nullptr)
{
}

inline constexpr ProxyBase::Listener::Listener(const SequenceNumber& seqNr, NotificationConsumer* caller) noexcept
    : mSequenceNr(seqNr)
    , mListener(caller)
{
}

inline constexpr bool ProxyBase::Listener::operator == (const ProxyBase::Listener& other) const noexcept
{
    if (this == &other)
        return true;

    if (other.mSequenceNr == areg::SEQUENCE_NUMBER_ANY)
        return true;
    else if ((other.mSequenceNr == mSequenceNr) && (other.mListener == mListener))
        return true;
    else if ((other.mSequenceNr == areg::SEQUENCE_NUMBER_NOTIFY) && (other.mListener == nullptr))
        return true;
    else
        return false;
}

inline constexpr bool ProxyBase::Listener::operator != (const ProxyBase::Listener& other) const noexcept
{
    return !(*this == other);
}

//////////////////////////////////////////////////////////////////////////
// ProxyBase::ServiceAvailableEvent class inline function implementation
//////////////////////////////////////////////////////////////////////////
inline NotificationConsumer& ProxyBase::ServiceAvailableEvent::consumer() const noexcept
{
    return *reinterpret_cast<NotificationConsumer* const>(event_consumer());
}

inline void ProxyBase::ServiceAvailableEvent::set_event_delay(uint32_t msDelay) noexcept
{
    set_call_type((msDelay == 0u) || (msDelay >= MINIMAL_DELAY_TIME_MS) ? static_cast<uint8_t>(msDelay / 10u) : static_cast<uint8_t>(MINIMAL_DELAY_TIME_MS / 10u));
}

inline uint32_t ProxyBase::ServiceAvailableEvent::event_delay() const noexcept
{
    return (call_type() * 10u);
}

inline bool ProxyBase::ServiceAvailableEvent::should_delay_event() const noexcept
{
    return (call_type() != 0u);
}

//////////////////////////////////////////////////////////////////////////
// ProxyBase class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline void ProxyBase::clear_all_notifications(NotificationConsumer & listener)
{
    unregister_listener(&listener);
}

inline ProxyBase & ProxyBase::self() noexcept
{
    return (*this);
}

inline void ProxyBase::lock_resource() noexcept
{
    ProxyBase::map_proxies().lock();
}

inline void ProxyBase::unlock_resource() noexcept
{
    ProxyBase::map_proxies().unlock();
}

inline const ProxyAddress& ProxyBase::proxy_address() const noexcept
{
    return mProxyAddress;
}

inline const StubAddress& ProxyBase::stub_address() const noexcept
{
    return mStubAddress;
}

inline bool ProxyBase::is_connected() const noexcept
{
    return mIsConnected;
}

inline void ProxyBase::set_connection_status(areg::ServiceConnectionState status) noexcept
{
    mConnectionStatus = status;
    mIsConnected = areg::is_service_connected(status);
}

inline areg::ServiceConnectionState ProxyBase::connection_status() const noexcept
{
    return mConnectionStatus;
}

inline bool ProxyBase::has_any_listener(uint32_t msgId) const noexcept
{
    ProxyListenerMap::MAPPOS pos = mListenerMap.find(msgId);
    return mListenerMap.is_valid_position(pos) && (pos->second.size() != 0u);
}

inline bool ProxyBase::has_notification_listener(uint32_t msgId) const noexcept
{
    ProxyListenerMap::MAPPOS pos = mListenerMap.find(msgId);
    if (!mListenerMap.is_valid_position(pos))
        return false;

    const ProxyListenerList & subVec = pos->second;
    for (uint32_t i = 0; i < subVec.size(); ++i)
    {
        if (subVec.value_at(i).mSequenceNr == areg::SEQUENCE_NUMBER_NOTIFY)
            return true;
    }
    return false;
}

inline void ProxyBase::start_notification( uint32_t msgId )
{
    if (is_connected())
    {
        send_notify_request( msgId, areg::RequestType::StartNotify );
    }
}

inline void ProxyBase::stop_notification( uint32_t msgId )
{
    if (is_connected()) 
    {
        send_notify_request( msgId, areg::RequestType::StopNotify );
    }
}

inline void ProxyBase::stop_all_notifications()
{
    if (is_connected()) 
    {
        send_notify_request( static_cast<uint32_t>(areg::FuncIdRange::EmptyFunctionId), areg::RequestType::RemoveAllNotify );
    }
}

inline void ProxyBase::stop_notifications( const uint32_t notifyIds[], int32_t count )
{
    for ( int i = 0; i < count; ++ i ) 
    {
        stop_notification( notifyIds[i] );
    }
}

inline const areg::ProxyData & ProxyBase::proxy_data() const noexcept
{
    return mProxyData;
}

inline areg::ProxyData & ProxyBase::proxy_data() noexcept
{
    return mProxyData;
}

inline bool ProxyBase::add_listener( uint32_t msgId, const SequenceNumber & seqNr, NotificationConsumer* caller, bool unique)
{
    ProxyBase::Listener listener{ seqNr, caller };
    ProxyListenerList & subVec = mListenerMap[msgId];
    if (unique)
    {
        return subVec.add_if_unique(listener);
    }
    else
    {
        subVec.add(listener);
        return true;
    }
}

inline void ProxyBase::remove_listener( uint32_t msgId, const SequenceNumber & seqNr, NotificationConsumer* caller ) noexcept
{
    ProxyListenerMap::MAPPOS pos = mListenerMap.find(msgId);
    if (mListenerMap.is_valid_position(pos))
        static_cast<void>(pos->second.remove_elem(ProxyBase::Listener{ seqNr, caller }));
}


inline void ProxyBase::register_for_event( const uint32_t eventClass )
{
    Event::add_listener( eventClass, static_cast<EventConsumer &>(self( )), static_cast<uint32_t>(mProxyAddress.thread()) );
}

inline void ProxyBase::unregister_for_event( const uint32_t eventClass ) noexcept
{
    Event::remove_listener( eventClass, static_cast<EventConsumer &>(self( )), static_cast<uint32_t>(mProxyAddress.thread()) );
}

inline void ProxyBase::set_state( uint32_t msgId, areg::DataState newState ) noexcept
{
    mProxyData.set_data_state( msgId, newState );
}

inline DispatcherThread & ProxyBase::proxy_dispatcher_thread() const noexcept
{
    return mDispatcherThread;
}

#ifdef DEBUG

inline uint32_t ProxyBase::listener_count() const noexcept
{
    uint32_t total = 0;
    for (ProxyListenerMap::MAPPOS pos = mListenerMap.first_position(); mListenerMap.is_valid_position(pos); pos = mListenerMap.next_position(pos))
        total += pos->second.size();
    return total;
}

#endif // DEBUG

} // namespace areg
#endif  // AREG_COMPONENT_PROXYBASE_HPP
