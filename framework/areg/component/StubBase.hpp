#ifndef AREG_COMPONENT_STUBBASE_HPP
#define AREG_COMPONENT_STUBBASE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/component/StubBase.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Stub Base class.
 *              This is Base class of all Stub objects. Derive class and
 *              implement pure virtual methods.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/ResourceMap.hpp"
#include "areg/component/StubEvent.hpp"
#include "areg/component/ProxyAddress.hpp"
#include "areg/component/StubAddress.hpp"
#include "areg/component/ServiceDefs.hpp"

#include <limits>
#include <utility>

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {
    class RemoteNotifyRequestEvent;
    class ServiceResponseEvent;
    class RemoteRequestEvent;
    class ComponentThread;
    class SharedBuffer;
    class Component;
} // namespace areg

namespace areg {

/************************************************************************
 * Definition of Session ID. Used when unblocking request
 ************************************************************************/
using SessionID = uint32_t;


//////////////////////////////////////////////////////////////////////////
// StubBase class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Base class for stub (service implementation) objects that handle asynchronous
 *          request-response communication. Manages client connections, pending request tracking,
 *          and event processing for service interfaces.
 **/
class AREG_API StubBase    : public StubEventConsumer
{
//////////////////////////////////////////////////////////////////////////
// friend classes
//////////////////////////////////////////////////////////////////////////
    friend class RemoteEventFactory;
    friend class StubEventConsumer;
    friend class SystemController;
    friend class Component;

//////////////////////////////////////////////////////////////////////////
// Internal constants and definitions
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Constant. Defines Invalid Session ID
     **/
    AREG_API_CONST static constexpr SessionID      INVALID_SESSION_ID  { std::numeric_limits<SessionID>::max() };

    /**
     * \brief   Constant. Defines Invalid Message ID
     */
    AREG_API_CONST static constexpr uint32_t   INVALID_SLOT        { areg::INVALID_MESSAGE_INDEX };

protected:
    //////////////////////////////////////////////////////////////////////////
    // StubBase::Listener class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Compact listener entry of a message ID. A subscription has the sequence number
     *          SEQUENCE_NUMBER_NOTIFY; any other sequence number is a request waiting for its response.
     **/
    class AREG_API Listener
    {
    public:
        /**
         * \brief   Flag of a request entry restored by prepare_response(); its response targets it first.
         **/
        AREG_API_CONST static constexpr uint16_t   FLAG_PREPARED   { 1u };
        /**
         * \brief   Flag of a request entry whose proxy is disconnected; its response reaches only the subscribers.
         **/
        AREG_API_CONST static constexpr uint16_t   FLAG_ORPHANED   { 2u };

    //////////////////////////////////////////////////////////////////////////
    // Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:

        inline Listener() noexcept;

        Listener(const StubBase::Listener& src) noexcept = default;

        Listener(StubBase::Listener&& src) noexcept = default;

        /**
         * \brief   Initializes listener with message ID; proxy fields zeroed.
         **/
        inline Listener(uint32_t reqId) noexcept;

        /**
         * \brief   Initializes listener with message ID and sequence number; proxy fields zeroed.
         **/
        inline Listener(uint32_t reqId, const SequenceNumber & seqId) noexcept;

        /**
         * \brief   Initializes listener with message ID, sequence number, and proxy address.
         *          The proxy is stored in compact integer form via ProxyAddress::to_endpoint.
         *
         * \param   reqId       The message ID.
         * \param   seqId       The sequence number.
         * \param   proxy       The target proxy address to extract routing integers from.
         **/
        inline Listener(uint32_t reqId, const SequenceNumber & seqId, const ProxyAddress & proxy) noexcept;

    //////////////////////////////////////////////////////////////////////////
    // Operators
    //////////////////////////////////////////////////////////////////////////
    public:

        StubBase::Listener & operator = (const StubBase::Listener & src) noexcept = default;

        StubBase::Listener & operator = ( StubBase::Listener && src ) noexcept = default;

        /**
         * \brief   Returns true if the sequence number of \a other is SEQUENCE_NUMBER_ANY, or if both
         *          listeners have the same sequence number and the same proxy.
         **/
        [[nodiscard]]
        bool operator == ( const StubBase::Listener & other ) const noexcept;

    //////////////////////////////////////////////////////////////////////////
    // Proxy access
    //////////////////////////////////////////////////////////////////////////
    public:

        /**
         * \brief   Reconstructs a ProxyAddress from the compact routing integers.
         **/
        [[nodiscard]]
        inline ProxyAddress proxy() const noexcept;

        /**
         * \brief   Returns true if the stored proxy identity matches addr without
         *          constructing a full ProxyAddress.
         **/
        [[nodiscard]]
        inline bool proxy_matches(const ProxyAddress & addr) const noexcept;

        /**
         * \brief   Returns true if both listeners are registered for the same proxy.
         **/
        [[nodiscard]]
        inline bool same_proxy(const StubBase::Listener & other) const noexcept;

        /**
         * \brief   Returns true if the listener is a request waiting for its response, not a subscription.
         **/
        [[nodiscard]]
        inline bool is_request() const noexcept;

    //////////////////////////////////////////////////////////////////////////
    // Member variables.
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   The message ID of the entry: the request a waiting entry answers, or the subscribed message.
         **/
        uint32_t        mMessageId  { 0 };
        /**
         * \brief   Bits FLAG_PREPARED and FLAG_ORPHANED of a request entry; 0 for a subscription.
         **/
        uint16_t        mFlags      { 0 };
        /**
         * \brief   The listener slot the entry is registered in; the slot of the response for a waiting request.
         **/
        uint16_t        mSlot       { 0 };
        /**
         * \brief   Sequence number of Listener.
         **/
        SequenceNumber  mSequenceNr { 0 };

        /**
         * \brief   Compact proxy service identity
         **/
        areg::RawService    mRawService { };
        /**
         * \brief   Compact proxy endpoint.
         **/
        areg::Endpoint      mConsumer   { };
    };

    //////////////////////////////////////////////////////////////////////////
    // StubBase::StubListenerList and StubListenerSlots declarations
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Flat list of listeners for a single message ID. Backed by contiguous vector for
     *          cache-friendly traversal.
     **/
    using StubListenerList  = ArrayList<StubBase::Listener>;

    static_assert(sizeof(StubBase::Listener) <= 48, "A stub listener must not grow.");

    /**
     * \brief   One listener list per response, broadcast and attribute, by listener slot.
     **/
    using StubListenerSlots = ListenerSlots<StubListenerList>;

    //////////////////////////////////////////////////////////////////////////
    // StubBase session tracking
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   StubBase::StubSessionMap class defines list of Session IDs and unblocked requests.
     **/
    using MapProviderSession     = IntegerMap<StubBase::Listener>;

    //////////////////////////////////////////////////////////////////////////
    // StubBase resource tracking
    //////////////////////////////////////////////////////////////////////////
    using MapProvider = HashMap<uint32_t, StubBase *>;
    /**
     * \brief   Stub resource helper definition.
     **/
    using ImplProviderResource  = ResourceMapImpl<uint32_t, StubBase *>;
    /**
     * \brief   Resource Map definition.
     **/
    using MapProviderResource   = ConcurrentResourceMap<uint32_t, StubBase *, MapProvider, ImplProviderResource>;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initializes the stub with its master component and service interface metadata.
     *
     * \param   masterComp      The component that owns this service implementation.
     * \param   siData          The service interface data containing service name, version, and type.
     **/
    StubBase( Component & masterComp, const areg::InterfaceData & siData );

    virtual ~StubBase();

//////////////////////////////////////////////////////////////////////////
// Operations. Public
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns the component thread that owns this stub.
     **/
    [[nodiscard]]
    ComponentThread & component_thread() const noexcept;

    /**
     * \brief   Returns the component that owns this stub. A stub is a part of its component,
     *          so the reference is valid for the entire lifetime of the stub.
     **/
    [[nodiscard]]
    inline Component & component() const noexcept;

    /**
     * \brief   Returns the address of this stub object.
     **/
    [[nodiscard]]
    inline const StubAddress & address() const noexcept;

    /**
     * \brief   Returns the role name of the implemented service interface.
     **/
    [[nodiscard]]
    inline const String & service_role() const noexcept;

    /**
     * \brief   Returns the name of the implemented service.
     **/
    [[nodiscard]]
    inline const String & service_name() const noexcept;

    /**
     * \brief   Calls error_request() with false for every attribute, response and request ID.
     *          Attribute subscribers receive the invalidation; a response or request error reaches
     *          only the callers waiting for it, and the subscriptions stay.
     **/
    void error_all_requests();

    /**
     * \brief   Calls error_request() with true for every request ID: every waiting caller, including
     *          the unblocked ones, receives RequestCanceled and every session ID becomes invalid.
     *          The subscriptions stay.
     **/
    void cancel_all_requests();

    /**
     * \brief   Searches for and returns the stub object at the specified address.
     *
     * \param   address     The address of the stub to find.
     * \return  Returns a pointer to the stub if found; otherwise nullptr.
     **/
    [[nodiscard]]
    static inline StubBase * find_stub(const StubAddress& address) noexcept;

    /**
     * \brief   Searches for and returns the stub object at the specified unique stub ID.
     *
     * \param   stubId  The unique ID of the stub to find.
     * \return  Returns a pointer to the stub if found; otherwise nullptr.
     **/
    [[nodiscard]]
    static inline StubBase * find_stub( const UniqueNumber stubId ) noexcept;

    /**
     * \brief   Returns a copy of the address of the registered stub that has the key of the
     *          specified address, taken under the registry lock.
     *
     * \param   address     The address whose key is searched.
     * \return  Returns the address of the registered stub; otherwise a copy of \a address.
     **/
    [[nodiscard]]
    static StubAddress registered_address( const StubAddress & address );

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// StubBase overrides. Triggered by Component on startup.
/************************************************************************/

    /**
     * \brief   Called when the component starts up. Derived classes should override to register
     *          request and attribute listeners.
     *
     * \param   holder      The component that started up.
     **/
    virtual void startup_service_interface(areg::Component& holder);

    /**
     * \brief   Called when the component shuts down. Derived classes should override to clean up
     *          listeners and resources.
     *
     * \param   holder      The component that is shutting down.
     **/
    virtual void shutdown_service_interface(areg::Component& holder) noexcept;

/************************************************************************/
// StubBase overrides. 
/************************************************************************/
    /**
     * \brief   Unblocks the request being processed and returns a session ID for answering it later.
     *          The same request may be accepted again while the session is open.
     *
     * \return  Returns a session ID to use with prepare_response(), or INVALID_SESSION_ID when no
     *          request is being processed.
     **/
    [[nodiscard]]
    virtual SessionID unblock_current_request();

    /**
     * \brief   Makes the request of the session the target of the next call of its response. Until
     *          that response is sent, the request is busy again.
     *
     * \param   sessionId       The session ID returned by unblock_current_request().
     * \return  Returns true if the session is restored; false if the session ID is unknown or
     *          cancelled, or another session of the same response is already prepared.
     **/
    virtual bool prepare_response(SessionID sessionId);

    /**
     * \brief   Called when a proxy client connects to or disconnects from this stub.
     *
     * \param   client      The address of the connecting or disconnecting client.
     * \param   status      The new connection status.
     * \return  Returns true if the client is relevant to this service; false otherwise.
     **/
    virtual bool consumer_connected( const areg::ProxyAddress & client, areg::ServiceConnectionState status );

/************************************************************************/
// StubBase overrides
/************************************************************************/

    /**
     * \brief   Sends an attribute update notification to all subscribed clients. Must be overridden by derived classes.
     *
     * \param   attrIndex   The index of the attribute to notify.
     **/
    virtual void send_notification( uint32_t attrIndex ) = 0;

    /**
     * \brief   Sends an error of the entry. Must be overridden by derived classes.
     *          See send_error_notification() for who receives it.
     *
     * \param   kind            Request, Response, Broadcast or Attribute.
     * \param   index           The index of the entry in its kind.
     * \param   msgCancel       If true, every waiting caller of the request is cancelled; if false,
     *                          only the caller the next response would answer gets RequestError.
     *                          Applies only to requests.
     **/
    virtual void error_request( areg::CallType kind, uint32_t index, bool msgCancel ) = 0;

protected:

    /**
     * \brief   Creates a response event to send to a client. Override in derived stubs.
     *          Base returns an invalid ServiceResponseEvent (is_valid() == false).
     *
     * \param   proxy       The address of the client proxy to receive the response.
     * \param   msgId       The message ID of the response.
     * \param   kind        The kind of the message ID.
     * \param   result      The result code of the response.
     * \param   data        The serialized response data. Can be an empty/invalid buffer.
     * \return  A ServiceResponseEvent value; check is_valid() before use.
     **/
    [[nodiscard]]
    virtual ServiceResponseEvent create_response( const areg::ProxyAddress & proxy, uint32_t msgId, areg::CallType kind, areg::ResultType result, const areg::SharedBuffer & data ) const;

    /**
     * \brief   Creates an empty (no-payload) response event of the stub's concrete type, with the
     *          buffer allocated for the header plus \a reserve payload bytes.
     *
     * \param   proxy       The address of the client proxy to receive the response.
     * \param   msgId       The message ID of the response.
     * \param   kind        The kind of the message ID.
     * \param   result      The result code of the response.
     * \param   reserve     Payload bytes to reserve after the header for the serialized parameters.
     * \return  A typed empty ServiceResponseEvent; check is_valid() before use.
     **/
    [[nodiscard]]
    virtual ServiceResponseEvent create_response_event( const areg::ProxyAddress & proxy, uint32_t msgId, areg::CallType kind, areg::ResultType result, uint32_t reserve ) const;

/************************************************************************/
// StubEventConsumer interface overrides.
/************************************************************************/

    /**
     * \brief   Processes a service request event. Must be overridden by derived classes to handle specific requests.
     *
     * \param   eventElem       The request event containing the request ID and serialized parameters.
     **/
    void process_request_event( areg::ServiceRequestEvent & eventElem ) override = 0;
    
    /**
     * \brief   Processes a request to get or subscribe to attribute updates. Must be overridden by derived classes.
     *
     * \param   eventElem       The event containing the attribute ID and subscription request.
     **/
    void process_attribute_event( areg::ServiceRequestEvent & eventElem ) override = 0;

    /**
     * \brief   Called when the stub is registered in the service registry. The status indicates success or failure.
     *
     * \param   stubTarget      The address of this registered stub.
     * \param   status          The registration status. Connected indicates successful registration.
     **/
    void process_registered_event( const areg::StubAddress & stubTarget, areg::ServiceConnectionState status ) override;

    /**
     * \brief   Returns true if this provider is registered and may process requests, which is
     *          the case only when its connection status is 'Connected'. Before the registration,
     *          while it is in progress, and after the provider unregistered, a request is refused
     *          and answered with a failure instead of being served.
     **/
    [[nodiscard]]
    bool can_process_requests( ) const override;

    /**
     * \brief   Called when a client requests connection or disconnection.
     *
     * \param   proxyAddress    The address of the client proxy.
     * \param   status          The connection status (connected or disconnected).
     **/
    void process_connect_event( const areg::ProxyAddress & proxyAddress, areg::ServiceConnectionState status ) override;

    /**
     * \brief   Processes a stub-specific event. Not typically triggered during normal operation.
     **/
    void process_stub_event( areg::StubEvent & eventElem ) override;

    /**
     * \brief   Processes a generic event. Not typically triggered during normal operation.
     **/
    void process_generic_event(areg::Event & eventElem) override;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations. Protected.
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns the version of this service interface implementation.
     **/
    [[nodiscard]]
    const Version & impl_version() const noexcept;

    /**
     * \brief   Returns the number of request methods in the service interface.
     **/
    [[nodiscard]]
    uint32_t number_of_requests() const noexcept;

    /**
     * \brief   Returns the number of responses in the service interface. Broadcasts are not counted.
     **/
    [[nodiscard]]
    uint32_t number_of_responses() const noexcept;

    /**
     * \brief   Returns the number of broadcasts in the service interface.
     **/
    [[nodiscard]]
    uint32_t number_of_broadcasts() const noexcept;

    /**
     * \brief   Returns the number of attributes in the service interface.
     **/
    [[nodiscard]]
    uint32_t number_of_attributes() const noexcept;

    /**
     * \brief   Returns an array of all request IDs in the service interface.
     **/
    [[nodiscard]]
    const uint32_t * request_ids() const noexcept;

    /**
     * \brief   Returns an array of all response IDs in the service interface, by response index.
     **/
    [[nodiscard]]
    const uint32_t * response_ids() const noexcept;

    /**
     * \brief   Returns an array of all broadcast IDs in the service interface, by broadcast index.
     **/
    [[nodiscard]]
    const uint32_t * broadcast_ids() const noexcept;

    /**
     * \brief   Returns an array of all attribute IDs in the service interface.
     **/
    [[nodiscard]]
    const uint32_t * attribute_ids() const noexcept;

    /**
     * \brief   Returns true if a request waits for the response at the index and is not unblocked.
     **/
    [[nodiscard]]
    bool is_busy(uint32_t respIndex) const noexcept;

    /**
     * \brief   Marks the request as pending and adds its listener to the list of its response.
     *          Only called for requests that have a response.
     *
     * \param   listener    The listener object for this request (contains source proxy and request ID).
     * \param   seqNr       The sequence number of the request call.
     * \param   respIndex   The index of the response of the request.
     **/
    void prepare_request(StubBase::Listener & listener, const SequenceNumber & seqNr, uint32_t respIndex);

    /**
     * \brief   Collects the listeners of the entry into the output list. A request is listened
     *          by the waiting callers on the list of its response.
     *
     * \param      kind         Request, Response, Broadcast or Attribute.
     * \param      index        The index of the entry in its kind.
     * \param[out] listeners    Receives the listeners of the entry.
     * \return  Returns the number of listeners found.
     **/
    uint32_t find_listeners(areg::CallType kind, uint32_t index, StubListenerList & listeners) const;

    /**
     * \brief   Collects the targets of the response or broadcast. A broadcast reaches every
     *          subscriber. A response answers one waiting request, the prepared one first, and reaches
     *          that caller and the subscribers of every other proxy; the answered request is removed.
     *          A response with no waiting request is dropped.
     *
     * \param   kind            Response or Broadcast.
     * \param   index           The index of the entry in its kind.
     * \param   result          The result code to stamp into the response.
     * \param   reserve         Payload bytes to reserve after the header for serialized parameters.
     * \param[out] listeners    Receives the targets, the answered caller first (empty if none).
     * \return  A typed response event; invalid (is_valid()==false) when there are no targets.
     **/
    [[nodiscard]]
    ServiceResponseEvent prepare_response_event( areg::CallType kind, uint32_t index, areg::ResultType result, uint32_t reserve, StubBase::StubListenerList & listeners );

    /**
     * \brief   Returns true if the proxy subscribed to the entry.
     *
     * \param   kind            Response, Broadcast or Attribute.
     * \param   index           The index of the entry in its kind.
     * \param   notifySource    The address of the subscribing proxy.
     **/
    [[nodiscard]]
    bool exist( areg::CallType kind, uint32_t index, const ProxyAddress & notifySource ) const noexcept;

    /**
     * \brief   Adds a subscription of the proxy to the entry. Returns false if it already exists
     *          or the entry is not a response, broadcast or attribute of the interface.
     *
     * \param   kind            Response, Broadcast or Attribute.
     * \param   index           The index of the entry in its kind.
     * \param   notifySource    The address of the subscribing proxy.
     **/
    bool add_notification_listener( areg::CallType kind, uint32_t index, const ProxyAddress & notifySource );

    /**
     * \brief   Removes the subscription of the proxy to the entry if it exists.
     *
     * \param   kind            Response, Broadcast or Attribute.
     * \param   index           The index of the entry in its kind.
     * \param   notifySource    The address of the proxy to unsubscribe.
     **/
    void remove_notification_listener( areg::CallType kind, uint32_t index, const ProxyAddress & notifySource ) noexcept;

    /**
     * \brief   Removes every subscription of the specified proxy, as remove_notification_listener()
     *          does for one entry. The waiting requests of the proxy are not changed.
     *
     * \param   notifySource    The address of the proxy to unsubscribe.
     **/
    void remove_notification_listeners( const ProxyAddress & notifySource ) noexcept;

    /**
     * \brief   Removes the subscriptions of the specified proxy and marks its waiting requests and
     *          sessions orphaned. An orphaned request stays busy until it is answered, and its
     *          response reaches only the subscribers.
     *
     * \param   whichProxy      The address of the disconnected proxy.
     **/
    void clear_all_listeners(const ProxyAddress & whichProxy) noexcept;

    /**
     * \brief   Sends a response event to all specified listeners.
     *          Ownership of masterEvent is transferred to the dispatcher.
     *
     * \param   whichListeners      The list of listeners containing target proxy addresses.
     * \param   masterEvent         The event to dispatch; targeted at whichListeners[0] by the
     *                              caller. Ownership is transferred to the dispatcher.
     **/
    void send_response_notification( const StubBase::StubListenerList & whichListeners, ServiceResponseEvent & masterEvent );

    /**
     * \brief   Sends the error \a masterEvent of the message ID of \a whichListeners. An attribute error
     *          reaches every subscriber. RequestCanceled reaches every waiting caller of the request,
     *          including the unblocked ones, whose sessions become invalid. RequestError reaches only
     *          the caller the next response would answer. A response error reaches every waiting
     *          caller as RequestError. Subscriptions of responses never receive a request error, and
     *          every request that receives an error is removed.
     *
     * \param   whichListeners      The listeners of the entry, as find_listeners() returns them.
     * \param   masterEvent         The template event; cloned per target. Caller destroys it.
     **/
    void send_error_notification( const StubBase::StubListenerList & whichListeners, const ServiceResponseEvent & masterEvent );

    /**
     * \brief   Broadcasts an attribute update to all specified listeners.
     *          Ownership of masterEvent is transferred to the dispatcher
     *
     * \param   whichListeners      The list of listeners containing proxy addresses.
     * \param   masterEvent         The event to broadcast. Ownership is transferred to the dispatcher.
     **/
    void send_update_notification( const StubBase::StubListenerList & whichListeners, ServiceResponseEvent & masterEvent ) const;

    /**
     * \brief   Sends a service response event to trigger response handlers on client side.
     *
     * \param   eventElem       The response event to send.
     **/
    void send_service_response( ServiceResponseEvent & eventElem ) const;

    /**
     * \brief   Cancels the current pending request and sends a cancellation response.
     **/
    void cancel_current_request() noexcept;

    /**
     * \brief   Marks an attribute as invalid and sends error notifications to all subscribed clients.
     *          An index out of range is ignored.
     *
     * \param   attrIndex   The index of the attribute to invalidate.
     **/
    void invalidate_attribute(uint32_t attrIndex);


    /**
     * \brief   Sends an update event to all proxy clients subscribed to the entry.
     *
     * \param   kind        Response, Broadcast or Attribute.
     * \param   index       The index of the entry in its kind.
     * \param   data        The serialized update data. Can be an empty/invalid buffer.
     * \param   result      The result code of the update.
     **/
    void send_update_event(areg::CallType kind, uint32_t index, const areg::SharedBuffer & data, areg::ResultType result) const;

    /**
     * \brief   Sends an update notification to a single target proxy.
     *
     * \param   target      The address of the proxy to notify.
     * \param   msgId       The message ID of the update.
     * \param   kind        The kind of the message ID.
     * \param   data        The serialized update data. Can be an empty/invalid buffer.
     * \param   result      The result code of the update.
     **/
    void send_notify_once( const ProxyAddress & target, uint32_t msgId, areg::CallType kind, const areg::SharedBuffer & data, areg::ResultType result ) const;

    /**
     * \brief   Sends a response event to the targets prepare_response_event() would collect.
     *
     * \param   respIndex   The index of the response.
     * \param   data        The serialized response data (response call arguments).
     **/
    void send_response_event(uint32_t respIndex, const areg::SharedBuffer & data);

    /**
     * \brief   Sends a busy response to indicate that a request is already being processed.
     *
     * \param   whichListener       The listener containing the proxy address and message ID to send
     *                              the busy response to.
     **/
    void send_busy_response(const StubBase::Listener & whichListener);

    /**
     * \brief   Determines if a request can be executed. Returns false and sends busy response if a
     *          previous response is still pending. Returns false and drops the request if the
     *          sequence number is SEQUENCE_NUMBER_NOTIFY or SEQUENCE_NUMBER_ANY.
     *
     * \param   whichListener       The listener containing the request ID and proxy address.
     * \param   respIndex           The index of the response of the request.
     * \param   seqNr               The sequence number of the request call.
     * \return  Returns true if the request can proceed; false if it is busy or dropped.
     **/
    [[nodiscard]]
    bool can_execute_request( StubBase::Listener & whichListener, uint32_t respIndex, const SequenceNumber & seqNr);

    /**
     * \brief   Creates the response that tells a consumer the provider has no such request.
     *
     * \param   proxy   The address of the consumer proxy that sent the request.
     * \param   msgId   The ID of the unknown request.
     * \param   seqNr   The sequence number of the request.
     * \return  A RequestUnsupported response; check is_valid() before use.
     **/
    [[nodiscard]]
    ServiceResponseEvent create_unsupported_response( const ProxyAddress & proxy, uint32_t msgId, const SequenceNumber & seqNr ) const;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns a reference to this stub object.
     **/
    [[nodiscard]]
    inline StubBase & self() noexcept;

    /**
     * \brief   Returns the global registry of all stub service implementations in the system.
     **/
    [[nodiscard]]
    static MapProviderResource& map_providers() noexcept;

    /**
     * \brief   Takes the providers of the threads out of the registry, under its lock. The
     *          objects are not touched; a provider removes only its own entry when it is deleted.
     * \param   threads     The names of the threads.
     **/
    static void _detach_thread_providers( const ThreadAddress & thread );

    /**
     * \brief   Removes this provider from the registry, if the entry is this provider.
     **/
    void _leave_registry();

    /**
     * \brief   Registers this provider, unless a provider of other names holds its number.
     **/
    void _enter_registry();

    /**
     * \brief   Creates the response that tells a consumer its request was not delivered.
     *
     * \param   proxy   The address of the consumer proxy that sent the request.
     * \param   msgId   The ID of the refused request.
     * \param   seqNr   The sequence number of the refused request.
     * \return  A MessageUndelivered response; check is_valid() before use.
     **/
    [[nodiscard]]
    ServiceResponseEvent create_undelivered_response( const ProxyAddress & proxy, uint32_t msgId, const SequenceNumber & seqNr ) const;

    /**
     * \brief   Removes the entry at \a index by moving the last entry into its place. The caller holds the lock.
     **/
    static void _remove_at( StubListenerList & subVec, uint32_t index ) noexcept;

    /**
     * \brief   Returns the index of the request a response answers: the prepared one, otherwise the
     *          one not unblocked. Returns the size of \a subVec if no request waits.
     *          If \a anyRequest is false, looks only at the entries of the request \a reqId.
     **/
    [[nodiscard]]
    static uint32_t _find_target( const StubListenerList & subVec, bool anyRequest = true, uint32_t reqId = 0u ) noexcept;

    /**
     * \brief   Returns the index of the request not unblocked and not prepared, checking \a hint first.
     *          Returns the size of \a subVec if there is none.
     **/
    [[nodiscard]]
    static uint32_t _find_unprepared( const StubListenerList & subVec, uint32_t hint ) noexcept;

    /**
     * \brief   Removes the sessions of the request \a reqId waiting in the listener slot \a respSlot and
     *          sends RequestCanceled of \a reqId to their callers.
     **/
    void _cancel_sessions( uint32_t reqId, uint32_t respSlot );

    /**
     * \brief   Collects the targets of the response or broadcast and removes the answered request.
     *
     * \param   kind            Response or Broadcast.
     * \param   index           The index of the entry in its kind.
     * \param[out] listeners    Receives the targets, the answered caller first.
     **/
    void collect_and_strip_response_listeners( areg::CallType kind, uint32_t index, StubListenerList & listeners );

    /**
     * \brief   Returns the listener slot of the entry, StubBase::INVALID_SLOT if it has none.
     **/
    [[nodiscard]]
    inline uint32_t _slot( areg::CallType kind, uint32_t index ) const noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Holder component object reference.
     **/
    Component &                     mComponent;

    /**
     * \brief   Instance of Servicing interface data.
     **/
    const areg::InterfaceData &     mInterface;

    /**
     * \brief   The address object of stub
     **/
    StubAddress                     mAddress;

    /**
     * \brief   The service connection status
     **/
    areg::ServiceConnectionState    mConnectionStatus;
    /**
     * \brief   True if started (between startup_service_interface() and shutdown_service_interface())
     **/
    bool                            mIsStarted;

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

private:
    /**
     * \brief   The listeners of every response, broadcast and attribute, by listener slot.
     **/
    StubBase::StubListenerSlots     mListeners;

    /**
     * \brief   Guards all accesses to mListeners.
     **/
    mutable SpinLock                mListenerLock;

    /**
     * \brief   The listener slot of the request currently being processed, INVALID_SLOT if none.
     **/
    uint32_t                        mCurrSlot;

    /**
     * \brief   Index of the current listener.
     **/
    uint32_t                        mCurrIndex;

    /**
     * \brief   Session map object, contains list of unblock requests
     **/
    MapProviderSession              mMapSessions;

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif  // _MSC_VER

protected:
    /**
     * \brief   The last generated session ID; a new one skips INVALID_SESSION_ID and every open session.
     **/
    SessionID                       mSessionId;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief
     * \note    Default constructor is deleted; use parameterized constructor instead.
     **/
    StubBase() = delete;
    AREG_NOCOPY_NOMOVE( StubBase );
};

//////////////////////////////////////////////////////////////////////////
// Inline function implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * StubBase::Listener class
 ************************************************************************/
//////////////////////////////////////////////////////////////////////////
// StubBase::Listener class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline StubBase::Listener::Listener() noexcept
    : mMessageId  ( 0 )
    , mFlags      ( 0 )
    , mSequenceNr ( 0 )
    , mRawService ( )
    , mConsumer   ( )
{
}

inline StubBase::Listener::Listener( uint32_t reqId ) noexcept
    : mMessageId  ( reqId )
    , mFlags      ( 0 )
    , mSequenceNr ( 0 )
    , mRawService ( )
    , mConsumer   ( )
{
}

inline StubBase::Listener::Listener( uint32_t reqId, const SequenceNumber & seqId ) noexcept
    : mMessageId  ( reqId )
    , mFlags      ( 0 )
    , mSequenceNr ( seqId )
    , mRawService ( )
    , mConsumer   ( )
{
}

inline StubBase::Listener::Listener( uint32_t reqId, const SequenceNumber & seqId, const ProxyAddress & proxy ) noexcept
    : mMessageId  ( reqId )
    , mFlags      ( 0 )
    , mSequenceNr ( seqId )
    , mRawService ( )
    , mConsumer   ( )
{
    proxy.to_endpoint( mRawService, mConsumer );
}

inline ProxyAddress StubBase::Listener::proxy() const noexcept
{
    return ProxyAddress( mRawService, mConsumer );
}

inline bool StubBase::Listener::proxy_matches( const ProxyAddress & addr ) const noexcept
{
    return (addr.cookie() == mConsumer.id) && (static_cast<uint32_t>(addr) == mConsumer.number);
}

inline bool StubBase::Listener::same_proxy( const StubBase::Listener & other ) const noexcept
{
    return (mConsumer.id == other.mConsumer.id) && (mConsumer.number == other.mConsumer.number);
}

inline bool StubBase::Listener::is_request() const noexcept
{
    return (mSequenceNr != areg::SEQUENCE_NUMBER_NOTIFY);
}

/************************************************************************
 * StubBase class
 ************************************************************************/
//////////////////////////////////////////////////////////////////////////
// StubBase class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline StubBase* StubBase::find_stub(const StubAddress& address) noexcept
{
    return map_providers().find_resource_object(static_cast<uint32_t>(address));
}

inline StubBase* StubBase::find_stub(const UniqueNumber stubId) noexcept
{
    return map_providers().find_resource_object(static_cast<uint32_t>(stubId));
}

inline StubBase & StubBase::self() noexcept
{
    return (*this);
}

inline uint32_t StubBase::_slot( areg::CallType kind, uint32_t index ) const noexcept
{
    return areg::listener_slot(mInterface, kind, index);
}

inline const StubAddress& StubBase::address() const noexcept
{
    return mAddress;
}

inline Component & StubBase::component() const noexcept
{
    return mComponent;
}

inline const String & StubBase::service_role() const noexcept
{
    return mAddress.role_name( );
}

inline const String& StubBase::service_name() const noexcept
{
    return mAddress.service_name();
}

} // namespace areg
#endif  // AREG_COMPONENT_STUBBASE_HPP
