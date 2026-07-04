#ifndef AREG_COMPONENT_STUBBASE_HPP
#define AREG_COMPONENT_STUBBASE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
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

//////////////////////////////////////////////////////////////////////////
// Internal constants and definitions
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Constant. Defines Invalid Session ID
     **/
    static constexpr SessionID      INVALID_SESSION_ID  { std::numeric_limits<SessionID>::max() };

    /**
     * \brief   Constant. Defines Invalid Message ID
     */
    static constexpr uint32_t   INVALID_MESSAGE_ID  { static_cast<uint32_t>(areg::INVALID_MESSAGE_ID) };

protected:
    //////////////////////////////////////////////////////////////////////////
    // StubBase::Listener class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Compact listener entry that tracks a registered proxy client for a message ID.
     **/
    class AREG_API Listener
    {
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
         * \brief   Returns true if listeners are equal; matching message ID and either matching
         *          sequence number or one sequence number is ANY.
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

    //////////////////////////////////////////////////////////////////////////
    // Member variables.
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   The message ID of Listener object.
         **/
        uint32_t        mMessageId  { 0 };
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
    // StubBase::StubListenerList and StubListenerMap declarations
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Flat list of listeners for a single message ID. Backed by contiguous vector for
     *          cache-friendly traversal.
     **/
    using StubListenerList  = ArrayList<StubBase::Listener>;

    /**
     * \brief   HashMap keyed by message ID mapping to its listener list. Provides O(1) average
     *          lookup, replacing the O(n) linked-list scan.
     **/
    using StubListenerMap   = HashMap<uint32_t, StubListenerList>;

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
     * \brief   Sends error responses to all pending requests and notification subscriptions.
     **/
    void error_all_requests();

    /**
     * \brief   Cancels all pending requests, sends error responses, and unblocks blocked requests.
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
     * \brief   Unblocks the current pending request and returns a unique session ID for manual
     *          response delivery.
     *
     * \return  Returns a session ID to use with prepare_response() for sending the response.
     **/
    [[nodiscard]]
    virtual SessionID unblock_current_request();

    /**
     * \brief   Prepares a response for the request identified by the given session ID.
     *
     * \param   sessionId       The session ID returned by unblock_current_request().
     * \return  Returns true if the session exists; false if the session ID is invalid or expired.
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
// StubBase overrides. Public pure virtual methods 
/************************************************************************/

    /**
     * \brief   Sends an attribute update notification to all subscribed clients. Must be overridden
     *          by derived classes.
     *
     * \param   msgId       The ID of the attribute to notify.
     **/
    virtual void send_notification( uint32_t msgId ) = 0;

    /**
     * \brief   Sends an error response to clients for the specified message ID. Must be overridden
     *          by derived classes.
     *
     * \param   msgId           The message ID (request, response, or attribute) to error.
     * \param   msgCancel       If true, marks request as canceled; if false, marks as error.
     *                          Applies only to request IDs.
     **/
    virtual void error_request( uint32_t msgId, bool msgCancel ) = 0;

protected:

    /**
     * \brief   Creates a response event to send to a client. Override in derived stubs.
     *          Base returns an invalid ServiceResponseEvent (is_valid() == false).
     *
     * \param   proxy       The address of the client proxy to receive the response.
     * \param   msgId       The message ID of the response.
     * \param   result      The result code of the response.
     * \param   data        The serialized response data. Can be an empty/invalid buffer.
     * \return  A ServiceResponseEvent value; check is_valid() before use.
     **/
    [[nodiscard]]
    virtual ServiceResponseEvent create_response( const areg::ProxyAddress & proxy, uint32_t msgId, areg::ResultType result, const areg::SharedBuffer & data ) const;

    /**
     * \brief   Creates an empty (no-payload) response event of the stub's concrete type, with the
     *          buffer allocated for the header plus \a reserve payload bytes.
     *
     * \param   proxy       The address of the client proxy to receive the response.
     * \param   msgId       The message ID of the response.
     * \param   result      The result code of the response.
     * \param   reserve     Payload bytes to reserve after the header for the serialized parameters.
     * \return  A typed empty ServiceResponseEvent; check is_valid() before use.
     **/
    [[nodiscard]]
    virtual ServiceResponseEvent create_response_event( const areg::ProxyAddress & proxy, uint32_t msgId, areg::ResultType result, uint32_t reserve ) const;

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
     * \brief   Returns the number of response types in the service interface.
     **/
    [[nodiscard]]
    uint32_t number_of_responses() const noexcept;

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
     * \brief   Returns an array of all response IDs in the service interface.
     **/
    [[nodiscard]]
    const uint32_t * response_ids() const noexcept;

    /**
     * \brief   Returns an array of all attribute IDs in the service interface.
     **/
    [[nodiscard]]
    const uint32_t * attribute_ids() const noexcept;

    /**
     * \brief   Returns true if the specified request is pending, not released, and marked as busy.
     **/
    [[nodiscard]]
    bool is_busy(uint32_t reqId) const noexcept;

    /**
     * \brief   Marks the request as pending and adds its listener to the list. Only called for
     *          requests that have associated responses.
     *
     * \param   listener    The listener object for this request (contains source proxy and message ID).
     * \param   seqNr       The sequence number of the request call.
     * \param   reqId       The ID of the request to prepare.
     **/
    void prepare_request(StubBase::Listener & listener, const SequenceNumber & seqNr, uint32_t reqId);

    /**
     * \brief   Collects all listeners matching the specified request ID into the output list.
     *
     * \param      reqId        The request ID to filter listeners.
     * \param[out] listeners    Receives the list of listeners for the request ID.
     * \return  Returns the number of listeners found.
     **/
    uint32_t find_listeners(uint32_t reqId, StubListenerList & listeners) const;

    /**
     * \brief   Finds the listeners subscribed to \a respId.
     *
     * \param   respId          The response / broadcast message ID.
     * \param   result          The result code to stamp into the response.
     * \param   reserve         Payload bytes to reserve after the header for serialized parameters.
     * \param[out] listeners    Receives the listeners subscribed to respId (empty if none).
     * \return  A typed response event; invalid (is_valid()==false) when there are no listeners.
     **/
    [[nodiscard]]
    ServiceResponseEvent prepare_response_event( uint32_t respId, areg::ResultType result, uint32_t reserve, StubBase::StubListenerList & listeners );

    /**
     * \brief   Returns true if a notification listener with the specified message ID and proxy address exists.
     *
     * \param   msgId           The notification message ID (attribute or response).
     * \param   notifySource    The address of the subscribing proxy.
     * \return  Returns true if the listener exists; false otherwise.
     **/
    [[nodiscard]]
    bool exist( uint32_t msgId, const ProxyAddress & notifySource ) const noexcept;

    /**
     * \brief   Adds a notification listener for the specified message ID and proxy. Returns false
     *          if the listener already exists.
     *
     * \param   msgId           The notification message ID (attribute or response).
     * \param   notifySource    The address of the subscribing proxy.
     * \return  Returns true if the listener was added; false if it already existed.
     **/
    bool add_notification_listener( uint32_t msgId, const ProxyAddress & notifySource );

    /**
     * \brief   Removes a notification listener if it exists.
     *
     * \param   msgId           The notification message ID (attribute or response).
     * \param   notifySource    The address of the proxy to unsubscribe.
     **/
    void remove_notification_listener( uint32_t msgId, const ProxyAddress & notifySource ) noexcept;

    /**
     * \brief   Removes all listeners for the specified proxy and returns the removed request IDs.
     *
     * \param   whichProxy      The address of the proxy whose listeners should be removed.
     * \param[out] removedIDs      Receives the list of request IDs that had listeners removed.
     **/
    void clear_all_listeners(const ProxyAddress & whichProxy, IntegerArray& removedIDs);

    /**
     * \brief   Removes all listeners for the specified proxy.
     *
     * \param   whichProxy      The address of the proxy whose listeners should be removed.
     * \note    Overload that does not return the list of removed IDs.
     **/
    void clear_all_listeners(const ProxyAddress & whichProxy) noexcept;

    /**
     * \brief   Dispatches a response event to all specified listeners. For the first listener the
     *          master event is sent directly (no clone), eliminating one heap allocation per
     *          dispatch. Clones are created for remaining listeners before the master is sent.
     *          Ownership of masterEvent is transferred to the dispatcher; callers must NOT call
     *          destroy() on it after this returns.
     *
     * \param   whichListeners      The list of listeners containing target proxy addresses.
     * \param   masterEvent         The event to dispatch; targeted at whichListeners[0] by the
     *                              caller. Ownership is transferred to the dispatcher.
     **/
    void send_response_notification( const StubBase::StubListenerList & whichListeners, ServiceResponseEvent & masterEvent );

    /**
     * \brief   Sends an error response for a failed attribute request to specified listeners.
     *          The master event is cloned for each listener; the caller retains ownership and must
     *          call destroy() on masterEvent after this returns.
     *
     * \param   whichListeners      The list of listeners to receive the error notification.
     * \param   masterEvent         The template event; cloned per listener. Caller destroys it.
     **/
    void send_error_notification( const StubBase::StubListenerList & whichListeners, const ServiceResponseEvent & masterEvent );

    /**
     * \brief   Broadcasts an attribute update to all specified listeners. For the first listener
     *          the master event is sent directly; subsequent listeners receive clones.
     *          Ownership of masterEvent is transferred to the dispatcher; callers must NOT call
     *          destroy() on it after this returns.
     *
     * \param   whichListeners      The list of listeners containing proxy addresses.
     * \param   masterEvent         The event to broadcast. Ownership is transferred to the
     *                              dispatcher.
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
     * \brief   Marks an attribute as invalid and sends error notifications to all subscribed
     *          clients.
     *
     * \param   attrId      The ID of the attribute to invalidate.
     * \note    Does not validate the attribute ID.
     **/
    void invalidate_attribute(uint32_t attrId);


    /**
     * \brief   Sends an update event to all proxy clients subscribed to the specified message ID.
     *
     * \param   msgId       The message ID of the update.
     * \param   data        The serialized update data. Can be an empty/invalid buffer.
     * \param   result      The result code of the update.
     **/
    void send_update_event(uint32_t msgId, const areg::SharedBuffer & data, areg::ResultType result) const;

    /**
     * \brief   Sends an update notification to a single target proxy.
     *
     * \param   target      The address of the proxy to notify.
     * \param   msgId       The message ID of the update.
     * \param   data        The serialized update data. Can be an empty/invalid buffer.
     * \param   result      The result code of the update.
     **/
    void send_notify_once( const ProxyAddress & target, uint32_t msgId, const areg::SharedBuffer & data, areg::ResultType result ) const;

    /**
     * \brief   Sends a response event to proxy clients subscribed to the specified response ID.
     *
     * \param   respId      The ID of the response.
     * \param   data        The serialized response data (response call arguments).
     **/
    void send_response_event(uint32_t respId, const areg::SharedBuffer & data);

    /**
     * \brief   Sends a busy response to indicate that a request is already being processed.
     *
     * \param   whichListener       The listener containing the proxy address and message ID to send
     *                              the busy response to.
     **/
    void send_busy_response(const StubBase::Listener & whichListener);

    /**
     * \brief   Determines if a request can be executed. Returns false and sends busy response if a
     *          previous response is still pending.
     *
     * \param   whichListener       The listener containing the message ID and proxy address.
     * \param   whichResponse       The associated response message ID.
     * \param   seqNr               The sequence number of the request call.
     * \return  Returns true if the request can proceed; false if it is blocked due to pending
     *          response.
     **/
    [[nodiscard]]
    bool can_execute_request( StubBase::Listener & whichListener, uint32_t whichResponse, const SequenceNumber & seqNr);

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
     * \brief   Removes one listener matching toRemove from the sub-vector for msgId using
     *          swap-and-pop. No-op if msgId or the matching listener is not found.
     *
     * \param   msgId       The message ID whose sub-vector is searched.
     * \param   toRemove    The listener to find and remove (matched via Listener::operator==).
     **/
    void remove_from_map( uint32_t msgId, const StubBase::Listener & toRemove ) noexcept;

    /**
     * \brief   Removes one listener matching toRemove from an already resolved sub-vector using swap-and-pop. 
     *          It performs NO map lookup and acquires NO lock, so it must be called by a caller that
     *          already holds the Lock and the target sub-vector.
     *
     * \param   subVec      The sub-vector to search and compact in place.
     * \param   toRemove    The listener to find and remove (matched via Listener::operator==).
     **/
    static void remove_from_list( StubListenerList & subVec, const StubBase::Listener & toRemove ) noexcept;

    /**
     * \brief   Collects all listeners for respId into listeners and, in the same locked
     *          region, removes the one-shot entries that the response path would consume:
     *          - seqNr > 0 (one-shot request): removed from the map.
     *          - seqNr < 0 (unblocked request with existing notify): the matching seqNr==0
     *            (notify subscription) entry is removed; the negated entry itself is kept.
     *          - seqNr == 0 (notify subscription): kept in the map.
     *
     * \param   respId          The response / broadcast message ID to collect listeners for.
     * \param[out] listeners    Receives all listeners for respId.
     **/
    void collect_and_strip_response_listeners( uint32_t respId, StubListenerList & listeners );

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

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

private:
    /**
     * \brief   Per-message-ID listener map. Each sub-vector holds all listeners registered for a specific message ID.
     **/
    StubBase::StubListenerMap       mListenerMap;

    /**
     * \brief   Guards all accesses to mListenerMap.
     **/
    mutable SpinLock                mListenerLock;

    /**
     * \brief   Message ID of the listener currently being processed.
     **/
    uint32_t                        mCurrMsgId;

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

    /**
     * \brief   Used to generate unique session ID. The uniqueness is provided within single stub object scope
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
    , mSequenceNr ( 0 )
    , mRawService ( )
    , mConsumer   ( )
{
}

inline StubBase::Listener::Listener( uint32_t reqId ) noexcept
    : mMessageId  ( reqId )
    , mSequenceNr ( 0 )
    , mRawService ( )
    , mConsumer   ( )
{
}

inline StubBase::Listener::Listener( uint32_t reqId, const SequenceNumber & seqId ) noexcept
    : mMessageId  ( reqId )
    , mSequenceNr ( seqId )
    , mRawService ( )
    , mConsumer   ( )
{
}

inline StubBase::Listener::Listener( uint32_t reqId, const SequenceNumber & seqId, const ProxyAddress & proxy ) noexcept
    : mMessageId  ( reqId )
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

inline const StubAddress& StubBase::address() const noexcept
{
    return mAddress;
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
