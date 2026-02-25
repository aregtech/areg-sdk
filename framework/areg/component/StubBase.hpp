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
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
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
#include "areg/base/GEGlobal.h"
#include "areg/base/ResourceMap.hpp"
#include "areg/component/StubEvent.hpp"
#include "areg/component/ProxyAddress.hpp"
#include "areg/component/StubAddress.hpp"
#include "areg/component/ServiceDefs.hpp"

#include <utility>

/************************************************************************
 * Definition of Session ID. Used when unblocking request
 ************************************************************************/
using SessionID = uint32_t;

/************************************************************************
 * Dependencies
 ************************************************************************/
class RemoteNotifyRequestEvent;
class ServiceResponseEvent;
class RemoteRequestEvent;
class ComponentThread;
class EventDataStream;
class ResponseEvent;
class Component;

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
    static constexpr SessionID      INVALID_SESSION_ID  { static_cast<SessionID>(~0) };

    /**
     * \brief   Constant. Defines Invalid Message ID
     */
    static constexpr uint32_t   INVALID_MESSAGE_ID  { static_cast<uint32_t>(NEService::INVALID_MESSAGE_ID) };

protected:
    //////////////////////////////////////////////////////////////////////////
    // StubBase::Listener class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   This is internal class to track list of assigned listeners
     *          for requests and attribute update notifications. 
     *          It contains message ID (request or attribute ID), 
     *          message sequence number and address of Proxy object to send
     *          response message.
     **/
    class AREG_API Listener
    {
    //////////////////////////////////////////////////////////////////////////
    // Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Creates an undefined listener.
         **/
        inline Listener();

        /**
         * \brief   Initializes listener with message ID; proxy address must be set separately.
         *
         * \param   reqId       The message ID.
         **/
        inline Listener(uint32_t reqId);

        /**
         * \brief   Initializes listener with message ID and sequence number; proxy address must be
         *          set separately.
         *
         * \param   reqId       The message ID.
         * \param   seqId       The sequence number.
         **/
        inline Listener(uint32_t reqId, const SequenceNumber & seqId);

        /**
         * \brief   Initializes listener with message ID, sequence number, and proxy address.
         *
         * \param   reqId       The message ID.
         * \param   seqId       The sequence number.
         * \param   proxy       The target proxy address.
         **/
        inline Listener(uint32_t reqId, const SequenceNumber & seqId, const ProxyAddress & proxy);

        /**
         * \brief   Copies listener data from the given source.
         *
         * \param   src     The source listener object.
         **/
        inline Listener(const StubBase::Listener & src);

        /**
         * \brief   Moves listener data from the given source.
         *
         * \param   src     The source listener object.
         **/
        inline Listener(StubBase::Listener && src) noexcept;

    //////////////////////////////////////////////////////////////////////////
    // Operators
    //////////////////////////////////////////////////////////////////////////
    public:

        /**
         * \brief   Copies listener data from the given source.
         *
         * \param   src     The source listener object.
         **/
        StubBase::Listener & operator = (const StubBase::Listener & src);

        /**
         * \brief   Moves listener data from the given source.
         *
         * \param   src     The source listener object.
         **/
        StubBase::Listener & operator = ( StubBase::Listener && src ) noexcept;

        /**
         * \brief   Returns true if listeners are equal; matching message ID and either matching
         *          sequence number, proxy address, or one sequence number is ANY.
         *
         * \param   other       The listener object to compare.
         **/
        bool operator == ( const StubBase::Listener & other ) const;

    //////////////////////////////////////////////////////////////////////////
    // Member variables.
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   The message ID of Listener object.
         **/
        uint32_t    mMessageId;
        /**
         * \brief   Sequence number of Listener
         **/
        SequenceNumber  mSequenceNr;
        /**
         * \brief   The address of target Proxy object.
         **/
        ProxyAddress    mProxy;
    };

    //////////////////////////////////////////////////////////////////////////
    // StubBase::StubListenerList class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   StubBase::StubListenerList class defines list of pending listeners.
     **/
    using StubListenerList  = LinkedList<StubBase::Listener>;

    //////////////////////////////////////////////////////////////////////////
    // StubBase session tracking
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   StubBase::StubSessionMap class defines list of Session IDs and unblocked requests.
     **/
    using MapStubSession     = IntegerMap<StubBase::Listener>;

    //////////////////////////////////////////////////////////////////////////
    // StubBase resource tracking
    //////////////////////////////////////////////////////////////////////////
    using MapStub           = HashMap<StubAddress, StubBase *>;
    /**
     * \brief   Stub resource helper definition.
     **/
    using ImplStubResource  = ResourceMapImpl<StubAddress, StubBase *>;
    /**
     * \brief   Resource Map definition.
     **/
    using MapStubResource   = ConcurrentResourceMap<StubAddress, StubBase *, MapStub, ImplStubResource>;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initializes the stub with its master component and service interface metadata.
     *
     * \param   masterComp      The component that owns this service implementation.
     * \param   siData          The service interface data containing service name, version, and
     *                          type.
     **/
    StubBase( Component & masterComp, const NEService::InterfaceData & siData );

    /**
     * \brief   Destructor.
     **/
    virtual ~StubBase();

//////////////////////////////////////////////////////////////////////////
// Operations. Public
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns the component thread that owns this stub.
     **/
    ComponentThread & component_thread() const;

    /**
     * \brief   Returns the address of this stub object.
     **/
    inline const StubAddress & address() const;

    /**
     * \brief   Returns the role name of the implemented service interface.
     **/
    inline const String & service_role() const;

    /**
     * \brief   Returns the name of the implemented service.
     **/
    inline const String & service_name() const;

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
    static StubBase * find_stub(const StubAddress& address);

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
    virtual void startup_service_interface(Component& holder);

    /**
     * \brief   Called when the component shuts down. Derived classes should override to clean up
     *          listeners and resources.
     *
     * \param   holder      The component that is shutting down.
     **/
    virtual void shutdown_service_interface(Component& holder);

/************************************************************************/
// StubBase overrides. 
/************************************************************************/
    /**
     * \brief   Unblocks the current pending request and returns a unique session ID for manual
     *          response delivery.
     *
     * \return  Returns a session ID to use with prepare_response() for sending the response.
     **/
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
    virtual bool client_connected( const ProxyAddress & client, NEService::ServiceConnectionState status );

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
     * \brief   Creates a response event to send to a client. Must be overridden by derived classes.
     *
     * \param   proxy       The address of the client proxy to receive the response.
     * \param   msgId       The message ID of the response.
     * \param   result      The result code of the response.
     * \param   data        The serialized response data. Can be an empty/invalid buffer.
     * \return  Returns a valid pointer to the created response event.
     **/
    virtual ResponseEvent * create_response_event( const ProxyAddress & proxy, uint32_t msgId, NEService::ResultType result, const EventDataStream & data ) const;

    /**
     * \brief   Creates a request event from a data stream for stub processing. Must be overridden
     *          by derived classes.
     *
     * \param   stream      The stream containing serialized request data.
     * \return  Returns a valid request event pointer on success; otherwise nullptr.
     **/
    virtual RemoteRequestEvent * remote_request_event( const InStream & stream ) const;

    /**
     * \brief   Creates a notification request event from a data stream. Must be overridden by
     *          derived classes.
     *
     * \param   stream      The stream containing serialized notification request data.
     * \return  Returns a valid notification request event pointer on success; otherwise nullptr.
     **/
    virtual RemoteNotifyRequestEvent * notify_request_event( const InStream & stream ) const;

/************************************************************************/
// StubEventConsumer interface overrides.
/************************************************************************/

    /**
     * \brief   Processes a service request event. Must be overridden by derived classes to handle
     *          specific requests.
     *
     * \param   eventElem       The request event containing the request ID and serialized
     *                          parameters.
     **/
    virtual void process_request_event( ServiceRequestEvent & eventElem ) override = 0;
    
    /**
     * \brief   Processes a request to get or subscribe to attribute updates. Must be overridden by
     *          derived classes.
     *
     * \param   eventElem       The event containing the attribute ID and subscription request.
     **/
    virtual void process_attribute_event( ServiceRequestEvent & eventElem ) override = 0;

    /**
     * \brief   Called when the stub is registered in the service registry. The status indicates
     *          success or failure.
     *
     * \param   stubTarget      The address of this registered stub.
     * \param   status          The registration status. Connected indicates successful
     *                          registration.
     **/
    void process_registered_event( const StubAddress & stubTarget, NEService::ServiceConnectionState status ) override;

    /**
     * \brief   Called when a client requests connection or disconnection.
     *
     * \param   proxyAddress    The address of the client proxy.
     * \param   status          The connection status (connected or disconnected).
     **/
    void process_connect_event( const ProxyAddress & proxyAddress, NEService::ServiceConnectionState status ) override;

    /**
     * \brief   Processes a stub-specific event. Not typically triggered during normal operation.
     **/
    void process_stub_event( StubEvent & eventElem ) override;

    /**
     * \brief   Processes a generic event. Not typically triggered during normal operation.
     **/
    void process_generic_event(Event & eventElem) override;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations. Protected.
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns the version of this service interface implementation.
     **/
    const Version & impl_version() const;

    /**
     * \brief   Returns the number of request methods in the service interface.
     **/
    uint32_t number_of_requests() const;

    /**
     * \brief   Returns the number of response types in the service interface.
     **/
    uint32_t number_of_responses() const;

    /**
     * \brief   Returns the number of attributes in the service interface.
     **/
    uint32_t number_of_attributes() const;

    /**
     * \brief   Returns an array of all request IDs in the service interface.
     **/
    const uint32_t * request_ids() const;

    /**
     * \brief   Returns an array of all response IDs in the service interface.
     **/
    const uint32_t * response_ids() const;

    /**
     * \brief   Returns an array of all attribute IDs in the service interface.
     **/
    const uint32_t * attribute_ids() const;

    /**
     * \brief   Returns true if the specified request is pending, not released, and marked as busy.
     **/
    bool is_busy(uint32_t requestId) const;

    /**
     * \brief   Marks the request as pending and adds its listener to the list. Only called for
     *          requests that have associated responses.
     *
     * \param   listener        The listener object for this request (contains source proxy and
     *                          message ID).
     * \param   seqNr           The sequence number of the request call.
     * \param   requestId       The ID of the request to prepare.
     **/
    void prepare_request(StubBase::Listener & listener, const SequenceNumber & seqNr, uint32_t requestId);

    /**
     * \brief   Collects all listeners matching the specified request ID into the output list.
     *
     * \param   requestId       The request ID to filter listeners.
     * \param[out] out_listners    Receives the list of listeners for the request ID.
     * \return  Returns the number of listeners found.
     **/
    uint32_t find_listeners(uint32_t requestId, StubListenerList & out_listners) const;

    /**
     * \brief   Returns true if a notification listener with the specified message ID and proxy
     *          address exists.
     *
     * \param   msgId           The notification message ID (attribute or response).
     * \param   notifySource    The address of the subscribing proxy.
     * \return  Returns true if the listener exists; false otherwise.
     **/
    bool exist( uint32_t msgId, const ProxyAddress & notifySource ) const;

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
    void remove_notification_listener( uint32_t msgId, const ProxyAddress & notifySource );

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
    void clear_all_listeners(const ProxyAddress & whichProxy);

    /**
     * \brief   Sends an attribute update notification to all specified proxy clients.
     *
     * \param   whichListeners      The list of listeners containing target proxy addresses.
     * \param   masterEvent         The event containing the attribute ID and updated value.
     **/
    void send_response_notification( const StubBase::StubListenerList & whichListeners, const ServiceResponseEvent & masterEvent );

    /**
     * \brief   Sends an error response for a failed attribute request to specified listeners.
     *
     * \param   whichListeners      The list of listeners to receive the error notification.
     * \param   masterEvent         The event containing the error type and attribute ID.
     **/
    void send_error_notification( const StubBase::StubListenerList & whichListeners, const ServiceResponseEvent & masterEvent );

    /**
     * \brief   Sends an attribute update notification to all specified listeners.
     *
     * \param   whichListeners      The list of listeners containing proxy addresses.
     * \param   masterEvent         The event containing the attribute ID, update type, and new
     *                              value.
     **/
    void send_update_notification( const StubBase::StubListenerList & whichListeners, const ServiceResponseEvent & masterEvent ) const;

    /**
     * \brief   Sends a service response event to trigger response handlers on client side.
     *
     * \param   eventElem       The response event to send.
     **/
    void send_service_response( ServiceResponseEvent & eventElem ) const;

    /**
     * \brief   Cancels the current pending request and sends a cancellation response.
     **/
    void cancel_current_request();

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
    void send_update_event(uint32_t msgId, const EventDataStream & data, NEService::ResultType result) const;

    /**
     * \brief   Sends an update notification to a single target proxy.
     *
     * \param   target      The address of the proxy to notify.
     * \param   msgId       The message ID of the update.
     * \param   data        The serialized update data. Can be an empty/invalid buffer.
     * \param   result      The result code of the update.
     **/
    void send_notify_once( const ProxyAddress & target, uint32_t msgId, const EventDataStream & data, NEService::ResultType result ) const;

    /**
     * \brief   Sends a response event to proxy clients subscribed to the specified response ID.
     *
     * \param   respId      The ID of the response.
     * \param   data        The serialized response data (response call arguments).
     **/
    void send_response_event(uint32_t respId, const EventDataStream & data);

    /**
     * \brief   Sends a busy response to indicate that a request is already being processed.
     *
     * \param   whichListener       The listener containing the proxy address and message ID to send
     *                              the busy response to.
     **/
    void send_busy_respone(const StubBase::Listener & whichListener);

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
    bool can_execute_request( StubBase::Listener & whichListener, uint32_t whichResponse, const SequenceNumber & seqNr);

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Holder component object reference.
     **/
    Component &                         mComponent;

    /**
     * \brief   Instance of Servicing interface data.
     **/
    const NEService::InterfaceData &   mInterface;

    /**
     * \brief   The address object of stub
     **/
    StubAddress                         mAddress;

    /**
     * \brief   The service connection status
     **/
    NEService::ServiceConnectionState       mConnectionStatus;

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
    /**
     * \brief   The list of listeners
     **/
    StubBase::StubListenerList          mListListener;

private:
    /**
     * \brief   The position of current listener, which is processing. When canceled, it sets nullptr.
     **/
    StubListenerList::LISTPOS           mCurrListener;

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(default: 4251)
#endif  // _MSC_VER

    /**
     * \brief   Used to generate unique session ID. The uniqueness is provided within single stub object scope
     **/
    uint32_t                        mSessionId;

private:
#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
    /**
     * \brief   Session map object, contains list of unblock requests
     **/
    MapStubSession                      mMapSessions;

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(default: 4251)
#endif  // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns a reference to this stub object.
     **/
    inline StubBase & self();

    /**
     * \brief   Returns the global registry of all stub service implementations in the system.
     **/
    inline static MapStubResource& map_providers();

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

inline StubBase::Listener::Listener()
    : mMessageId ( 0 )
    , mSequenceNr( 0 )
    , mProxy     ( ProxyAddress::invalid_proxy_address() )
{
}

inline StubBase::Listener::Listener( uint32_t reqId )
    : mMessageId ( reqId )
    , mSequenceNr( 0 )
    , mProxy     ( ProxyAddress::invalid_proxy_address() )
{
}

inline StubBase::Listener::Listener( uint32_t reqId, const SequenceNumber & seqId )
    : mMessageId ( reqId )
    , mSequenceNr( seqId )
    , mProxy     ( ProxyAddress::invalid_proxy_address() )
{
}

inline StubBase::Listener::Listener( uint32_t reqId, const SequenceNumber & seqId, const ProxyAddress& proxy )
    : mMessageId ( reqId )
    , mSequenceNr( seqId )
    , mProxy     ( proxy )
{
}

inline StubBase::Listener::Listener( const StubBase::Listener& src )
    : mMessageId ( src.mMessageId )
    , mSequenceNr( src.mSequenceNr)
    , mProxy     ( src.mProxy )
{
}

inline StubBase::Listener::Listener( StubBase::Listener && src ) noexcept
    : mMessageId ( src.mMessageId )
    , mSequenceNr( src.mSequenceNr )
    , mProxy     ( std::move(src.mProxy) )
{
}

inline StubBase::Listener& StubBase::Listener::operator = ( const StubBase::Listener& src )
{
    mMessageId  = src.mMessageId;
    mSequenceNr = src.mSequenceNr;
    mProxy      = src.mProxy;

    return (*this);
}

inline StubBase::Listener& StubBase::Listener::operator = ( StubBase::Listener && src ) noexcept
{
    mMessageId  = src.mMessageId;
    mSequenceNr = src.mSequenceNr;
    mProxy      = std::move(src.mProxy);
    
    return (*this);
}

/************************************************************************
 * StubBase class
 ************************************************************************/
//////////////////////////////////////////////////////////////////////////
// StubBase class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline StubBase & StubBase::self()
{
    return (*this);
}

inline const StubAddress& StubBase::address() const
{
    return mAddress;
}

inline const String & StubBase::service_role() const
{
    return mAddress.role_name( );
}

inline const String& StubBase::service_name() const
{
    return mAddress.service_name();
}

#endif  // AREG_COMPONENT_STUBBASE_HPP
