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
#include "areg/base/TEResourceMap.hpp"
#include "areg/component/StubEvent.hpp"
#include "areg/component/ProxyAddress.hpp"
#include "areg/component/StubAddress.hpp"
#include "areg/component/NEService.hpp"

#include <utility>

/************************************************************************
 * Definition of Session ID. Used when unblocking request
 ************************************************************************/
using SessionID = unsigned int;

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
 * \brief   A Stub (or service implementation) base class to extend for
 *          every service implementation object. Here it is provided
 *          main basic logic of asynchronous communication. It keeps 
 *          track of requests and contains list of listeners
 **/
class AREG_API StubBase    : public IEStubEventConsumer
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
    static constexpr unsigned int   INVALID_MESSAGE_ID  { static_cast<unsigned int>(NEService::INVALID_MESSAGE_ID) };

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
         * \brief   Creates undefined listener.
         **/
        inline Listener( void );

        /**
         * \brief   Initialize Message ID. The Proxy Address should be set. If needed, update sequence number.
         **/
        inline Listener(unsigned int reqId);

        /**
         * \brief   Initialize message ID and sequence number. The target Proxy Address should be set manually.
         **/
        inline Listener(unsigned int reqId, const SequenceNumber & seqId);

        /**
         * \brief   Creates Listener from given parameters.
         * \param   reqId   The message ID.
         * \param   seqId   The Sequence number.
         * \param   proxy   The target proxy address.
         **/
        inline Listener(unsigned int reqId, const SequenceNumber & seqId, const ProxyAddress & proxy);

        /**
         * \brief   Copies listener data from given source.
         * \param   src     The source of data to copy.
         **/
        inline Listener(const StubBase::Listener & src);

        /**
         * \brief   Moves listener data from given source.
         * \param   src     The source of data to move.
         **/
        inline Listener(StubBase::Listener && src) noexcept;

    //////////////////////////////////////////////////////////////////////////
    // Operators
    //////////////////////////////////////////////////////////////////////////
    public:

        /**
         * \brief   Copies listener data from given source.
         **/
        StubBase::Listener & operator = (const StubBase::Listener & src);

        /**
         * \brief   Moves listener data from given source.
         **/
        StubBase::Listener & operator = ( StubBase::Listener && src ) noexcept;

        /**
         * \brief   Comparing operator, compares 2 listener objects.
         *          2 listeners are equal if message ID is same and
         *          if either sequence number is equal to 
         *          StubBase::Listener::ANY_SEQUENCE_NR, or if
         *          they have same message sequence number and same
         *          target proxy address.
         **/
        bool operator == ( const StubBase::Listener & other ) const;

    //////////////////////////////////////////////////////////////////////////
    // Member variables.
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   The message ID of Listener object.
         **/
        unsigned int    mMessageId;
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
    using StubListenerList  = TELinkedList<StubBase::Listener>;

    //////////////////////////////////////////////////////////////////////////
    // StubBase session tracking
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   StubBase::StubSessionMap class defines list of Session IDs and unblocked requests.
     **/
    using MapStubSession     = TEIntegerMap<StubBase::Listener>;

    //////////////////////////////////////////////////////////////////////////
    // StubBase resource tracking
    //////////////////////////////////////////////////////////////////////////
    using MapStub           = TEHashMap<StubAddress, StubBase *>;
    /**
     * \brief   Stub resource helper definition.
     **/
    using ImplStubResource  = TEResourceMapImpl<StubAddress, StubBase *>;
    /**
     * \brief   Resource Map definition.
     **/
    using MapStubResource   = TELockResourceMap<StubAddress, StubBase *, MapStub, ImplStubResource>;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Constructor. Protected. Requires holder component reference and 
     *          the name of service interface
     * \param   masterComp      The master component object of Stub service interface
     * \param   siData          The service interface data, containing service name
     *                          service version and service type
     **/
    StubBase( Component & masterComp, const NEService::SInterfaceData & siData );

    /**
     * \brief   Destructor.
     **/
    virtual ~StubBase( void );

//////////////////////////////////////////////////////////////////////////
// Operations. Public
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns Component Master thread object.
     **/
    ComponentThread & getComponentThread( void ) const;

    /**
     * \brief   Returns the address of Stub object.
     **/
    inline const StubAddress & getAddress( void ) const;

    /**
     * \brief   Returns the role name of the implemented service interface.
     **/
    inline const String & getServiceRole( void ) const;

    /**
     * \brief   Returns the name of the implemented service.
     **/
    inline const String & getServiceName( void ) const;

    /**
     * \brief   Sends error event to all pending responses and notification updates
     **/
    void errorAllRequests( void );

    /**
     * \brief   Sends error event to all pending responses and unlocks requests.
     **/
    void cancelAllRequests( void );

    /**
     * \brief   Search stub object by given stub address and if
     *          found, returns valid pointer of stub object.
     * \param   address     The Address of Stub object.
     * \return  If found, returns valid pointer of Stub object.
     *          Otherwise returns nullptr.
     **/
    static StubBase * findStubByAddress(const StubAddress& address);

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// StubBase overrides. Triggered by Component on startup.
/************************************************************************/

    /**
     * \brief   This function is triggered by Component when starts up.
     *          Overwrite this method and set appropriate request and
     *          attribute update notification event listeners here
     * \param   holder  The holder component of service interface of Stub,
     *                  which started up.
     **/
    virtual void startupServiceInterface(Component& holder);

    /**
     * \brief   This function is triggered by Component when shuts down.
     *          Overwrite this method to remove listeners and stub cleanup
     * \param   holder  The holder component of service interface of Stub,
     *                  which shuts down.
     **/
    virtual void shutdownServiceInterface(Component& holder);

/************************************************************************/
// StubBase overrides. 
/************************************************************************/
    /**
     * \brief   This function unblocks current request to be able to get
     *          same response again and return unique session ID.
     *          Use this session ID to prepare response to send response.
     *          The caller of this function should save Session ID
     *          for later use to prepare response.
     **/
    virtual SessionID unblockCurrentRequest( void );

    /**
     * \brief   By given unique Session ID, prepares response to send.
     * \param   sessionId   The Session ID returned by calling
     *                      UnblockCurrentRequest()
     * \return  Return true if specified session exist in the list.
     *          Otherwise, return false.
     **/
    virtual bool prepareResponse(SessionID sessionId);

    /**
     * \brief   Triggered when proxy client either connected or disconnected to stub.
     * \param   client  The address of proxy client, which connection status is changed.
     * \param   status  The service consumer connection status.
     * \return  Returns true if connected service consumer is relevant to the provider.
     **/
    virtual bool clientConnected( const ProxyAddress & client, NEService::eServiceConnection status );

/************************************************************************/
// StubBase overrides. Public pure virtual methods 
/************************************************************************/

    /**
     * \brief   Sends update notification message to all clients. 
     *          This method can be called manually to send update 
     *          notification message after updating attribute value.
     *
     *          Overwrite to implement method
     *
     * \param   msgId   The attribute message ID to notify clients.
     **/
    virtual void sendNotification( unsigned int msgId ) = 0;

    /**
     * \brief   Sends error message to clients.
     *          If message ID is a request, it should send result NEService::RequestError or NEService::RequestCanceled, depending on msgCancel flag.
     *          If message ID is a response, it should send result NEService::RequestInvalid.
     *          If message ID is an attribute, it should send result NEService::ResultDataInvalid
     *          and invalidate attribute data value.
     *
     *          Overwrite to implement method
     *
     * \param   msgId       The message ID to send error message
     * \param   msgCancel   Indicates whether the request is canceled or should respond with error.
     *                      This parameter has sense only for request IDs.
     *                      It is ignored for response and attributes IDs.
     **/
    virtual void errorRequest( unsigned int msgId, bool msgCancel ) = 0;

protected:

    /**
     * \brief   Overwrite method to create Response event object to pass of client.
     *
     * \remark  Overwrite to implement method
     *
     * \param   proxy   The address of proxy object to send response event
     * \param   msgId   The message ID to send to client
     * \param   result  The result of message
     * \param   data    The buffer of data to send to client. Can be Invalid buffer
     * \return  Returns valid pointer to Response event object
     **/
    virtual ResponseEvent * createResponseEvent( const ProxyAddress & proxy, unsigned int msgId, NEService::eResultType result, const EventDataStream & data ) const;

    /**
     * \brief   Overwrite method to create remote service request event from streaming object for 
     *          further dispatching by stub.
     * \param   stream  Streaming object, which contains event data.
     * \return  If operation succeeds, returns valid pointer to Service Request event object.
     *          Otherwise, it returns nullptr.
     **/
    virtual RemoteRequestEvent * createRemoteRequestEvent( const IEInStream & stream ) const;

    /**
     * \brief   Overwrite method to create remote notify request event from streaming object for 
     *          further dispatching by stub.
     * \param   stream  Streaming object, which contains event data.
     * \return  If operation succeeds, returns valid pointer to Service Request event object.
     *          Otherwise, it returns nullptr.
     **/
    virtual RemoteNotifyRequestEvent * createRemoteNotifyRequestEvent( const IEInStream & stream ) const;

/************************************************************************/
// IEStubEventConsumer interface overrides.
/************************************************************************/

    /**
     * \brief   Triggered to process service request event.
     *          Overwrite method to process every service request event.
     * \param   eventElem   Service Request Event object, contains request
     *                      call ID and parameters.
     **/
    virtual void processRequestEvent( ServiceRequestEvent & eventElem ) override = 0;
    
    /**
     * \brief   Triggered to process attribute update notification event.
     *          Override method to process request to get attribute value and
     *          process notification request of attribute update.
     * \param   eventElem   Service Request Event object, contains attribute ID.
     **/
    virtual void processAttributeEvent( ServiceRequestEvent & eventElem ) override = 0;

    /**
     * \brief   Triggered by system when stub is registered in service. The connection status indicated
     *          registration status. If succeeded, the value is NEService::ServiceConnected
     * \param   stubTarget  The address of registered service provider
     * \param   status      The connection status of the service provider.
     **/
    virtual void processStubRegisteredEvent( const StubAddress & stubTarget, NEService::eServiceConnection status ) override;

    /**
     * \brief   Send by system when client is requested connect / disconnect
     * \param   proxyAddress    The address of the service consumer proxy.
     * \param   status          The service consumer connection status.
     **/
    virtual void processClientConnectEvent( const ProxyAddress & proxyAddress, NEService::eServiceConnection status ) override;

    /**
     * \brief   Triggered to process generic stub event.
     *          Usually should not be triggered.
     **/
    virtual void processStubEvent( StubEvent & eventElem ) override;

    /**
     * \brief   Triggered to process generic event. Usually is not triggered.
     **/
    virtual void processGenericEvent(Event & eventElem) override;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations. Protected.
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns implemented version of service interface.
     **/
    const Version & getImplVersion( void ) const;

    /**
     * \brief   Returns number of requests of Service Interface
     *
     * \remark  Overwrite to implement method
     **/
    unsigned int getNumberOfRequests( void ) const;

    /**
     * \brief   Returns number of responses of Service Interface
     *
     * \remark  Overwrite to implement method
     **/
    unsigned int getNumberOfResponses( void ) const;

    /**
     * \brief   Returns number of attributes of Service Interface
     *
     * \remark  Overwrite to implement method
     **/
    unsigned int getNumberOfAttributes( void ) const;

    /**
     * \brief   Returns pointer of array of requests IDs of Service Interface
     *
     * \remark  Overwrite to implement method
     **/
    const unsigned int * getRequestIds( void ) const;

    /**
     * \brief   Returns pointer of array of response IDs of Service Interface
     *
     * \remark  Overwrite to implement method
     **/
    const unsigned int * getResponseIds( void ) const;

    /**
     * \brief   Returns pointer of array of attribute IDs of Service Interface
     *
     * \remark  Overwrite to implement method
     **/
    const unsigned int * getAttributeIds( void ) const;

    /**
     * \brief   Returns true if specified request is in pending list,
     *          is not released and marked as busy.
     *          Used internally.
     **/
    bool isBusy(unsigned int requestId) const;

    /**
     * \brief   Prepares Request, adds request listener to th listener list
     *          and waits for release by calling response. Only requests, which
     *          have responses are added to request listener list.
     * \param   listener    The listener object to prepare.
     * \param   seqNr       The sequence number of call request.
     * \param   requestId   The triggered request ID .
     **/
    void prepareRequest(StubBase::Listener & listener, const SequenceNumber & seqNr, unsigned int requestId);

    /**
     * \brief   Search and add all listeners, which have same specified request ID 
     *          and returns the size of filled listener list. If no listener could find,
     *          returns empty list.
     * \param   requestId       The request ID to filter listeners in listener list
     * \param   out_listners    On output, this contains list of listeners having
     *                          specified request ID.
     * \return  Returns the size of filtered listener list.
     **/
    uint32_t findListeners(unsigned int requestId, StubListenerList & out_listners) const;

    /**
     * \brief   Searches notification listener in the list of listeners and returns true
     *          if listener is assigned. 2 notification listeners are equal if 
     *          they are marked as notification listener (sequence number is notification),
     *          the message ID and the Proxy addresses are same.
     * \param   msgId           The ID of notification message (normally, either attribute or response message ID).
     * \param   notifySource    The address of Proxy source.
     * \return  Returns true if notification listener is already registered in the list of listeners.
     **/
    bool existNotificationListener( unsigned int msgId, const ProxyAddress & notifySource ) const;

    /**
     * \brief   Adds new notification listener in the list.
     *          The function first will check whether notification listener is already exists or not.
     *          The notification message listener exists in the list, if its sequence number is notification,
     *          message ID and proxy address are same. If there is no entry in the list, it will add
     *          new notification listener entry in the list and return true. Otherwise, it will ignore and return false.
     * \param   msgId           The ID of notification message (normally, either attribute or response message ID).
     * \param   notifySource    The address of Proxy source.
     * \return  Returns true, if list does not contain notification listener entry and new entry was added with success.
     *          Otherwise function returns false.
     **/
    bool addNotificationListener( unsigned int msgId, const ProxyAddress & notifySource );

    /**
     * \brief   Removes notification listener from the listener list.
     *          The notification listener exist in the list, it its  sequence number is notification,
     *          message ID and proxy address are same. If entry found in the list, it will be removed.
     *          Otherwise, there will be no operation performed.
     * \param   msgId           The ID of notification message (normally, either attribute or response message ID).
     * \param   notifySource    The address of Proxy source.
     **/
    void removeNotificationListener( unsigned int msgId, const ProxyAddress & notifySource );

    /**
     * \brief   Returns all listeners for specified proxy and on output returns
     *          list of remove request IDs.
     * \param   whichProxy  The address of request source proxy to remove.
     * \param   removedIDs  The list of removed request IDs
     **/
    void clearAllListeners(const ProxyAddress & whichProxy, IntegerArray& removedIDs);
    /**
     * \brief   Returns all listeners for specified proxy and on output returns
     *          list of remove request IDs.
     * \param   whichProxy  The address of request source proxy to remove.
     **/
    void clearAllListeners(const ProxyAddress & whichProxy);

    /**
     * \brief   Sends attribute notification response to all target proxy objects
     *          listed in given listener list.
     * \param   whichListeners  The list of listeners, containing target proxy address,
     *                          to send update notification event.
     * \param   masterEvent     The event, containing updated object ID and the new
     *                          value of attribute.
     **/
    void sendResponseNotification( const StubBase::StubListenerList & whichListeners, const ServiceResponseEvent & masterEvent );

    /**
     * \brief   Sends error message for requested to get attribute.
     * \param   whichListeners  The list of listeners containing target
     *                          proxy address to send error notification.
     * \param   masterEvent     The event, containing error type and 
     *                          attribute object ID.
     **/
    void sendErrorNotification( const StubBase::StubListenerList & whichListeners, const ServiceResponseEvent & masterEvent );

    /**
     * \brief   Sends attribute update notification message to all
     *          listed listeners, containing target proxy addresses.
     * \param   whichListeners  The list of listeners, containing
     *                          address of proxy to send update notification.
     * \param   masterEvent     The event message to send, which contains 
     *                          attribute object ID, update type and new 
     *                          updated  value of attribute.
     **/
    void sendUpdateNotification( const StubBase::StubListenerList & whichListeners, const ServiceResponseEvent & masterEvent ) const;

    /**
     * \brief   Sends Service Response message to trigger response call 
     *          on Proxy and Clients side
     * \param   eventElem   Service response event to send.
     **/
    void sendServiceResponse( ServiceResponseEvent & eventElem ) const;

    /**
     * \brief   Cancel current request.
     **/
    void cancelCurrentRequest( void );

    /**
     * \brief   Invalidates specified attribute, which should send error notification to clients.
     * \param   attrId  The ID of attribute to invalidate.
     * \note    The function does not check the validity of ID
     **/
    void invalidateAttribute(unsigned int attrId);


    /**
     * \brief   Sends update event to all proxy objects. The list of proxy listeners is selected
     *          by message ID.
     * \param   msgId   The message ID to send to proxy objects
     * \param   data    The buffer of serialized data to send to proxy. Can be Invalid buffer
     * \param   result  The result to send to proxy objects.
     **/
    void sendUpdateEvent(unsigned int msgId, const EventDataStream & data, NEService::eResultType result) const;

    /**
     * \brief   Send once the data update notification event to the specified target.
     * \param   target  The target to send the event.
     * \param   msgId   The message ID to send to the target object.
     * \param   data    The data to send to the target. Can be Invalid buffer
     * \param   result  The result of data update to send to the target object.
     **/
    void sendUpdateNotificationOnce( const ProxyAddress & target, unsigned int msgId, const EventDataStream & data, NEService::eResultType result ) const;

    /**
     * \brief   Sends response event to proxy. The list of proxy listeners is selected by message ID.
     * \param   respId  The ID of response to send to proxy objects
     * \param   data    The buffer of serialized data to send to proxy (should be serialized arguments of response call)
     **/
    void sendResponseEvent(unsigned int respId, const EventDataStream & data);

    /**
     * \brief   Sends busy response on request from client side. If the stub already processing request, 
     *          this request is pending (response is pending) and is not unblocked, it send busy message 
     *          to the proxy triggered request call.
     * \param   whichListener   The listener object containing proxy address and message ID to send busy message.
     **/
    void sendBusyRespone(const StubBase::Listener & whichListener);

    /**
     * \brief   Indicates whether request can be executed or not.
     *          The request cannot be executed if its response still is pending and it is blocked.
     *          If request cannot be executed, it send busy message to the proxy. Otherwise, it
     *          prepares request and places response (listener) in pending list.
     * \param   whichListener   The listener object containing message ID and proxy address to check.
     * \param   whichResponse   The response message ID to check.
     * \param   seqNr           The sequence number of call.
     * \return  Returns true if request can be executed and the appropriate response is prepared.
     **/
    bool canExecuteRequest( StubBase::Listener & whichListener, unsigned int whichResponse, const SequenceNumber & seqNr);

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
    const NEService::SInterfaceData &   mInterface;

    /**
     * \brief   The address object of stub
     **/
    StubAddress                         mAddress;

    /**
     * \brief   The service connection status
     **/
    NEService::eServiceConnection       mConnectionStatus;

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
    unsigned int                        mSessionId;

private:
#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
    /**
     * \brief   Session map object, contains list of unblock requests
     **/
    MapStubSession                      mMapSessions;

    /**
     * \brief   Stub object resource map.
     **/
    static MapStubResource              _mapRegisteredStubs;
#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(default: 4251)
#endif  // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns reference to Stub object
     **/
    inline StubBase & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    StubBase( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( StubBase );
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

inline StubBase::Listener::Listener( void )
    : mMessageId ( 0 )
    , mSequenceNr( 0 )
    , mProxy     ( ProxyAddress::getInvalidProxyAddress() )
{
}

inline StubBase::Listener::Listener( unsigned int reqId )
    : mMessageId ( reqId )
    , mSequenceNr( 0 )
    , mProxy     ( ProxyAddress::getInvalidProxyAddress() )
{
}

inline StubBase::Listener::Listener( unsigned int reqId, const SequenceNumber & seqId )
    : mMessageId ( reqId )
    , mSequenceNr( seqId )
    , mProxy     ( ProxyAddress::getInvalidProxyAddress() )
{
}

inline StubBase::Listener::Listener( unsigned int reqId, const SequenceNumber & seqId, const ProxyAddress& proxy )
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

inline StubBase & StubBase::self( void )
{
    return (*this);
}

inline const StubAddress& StubBase::getAddress(void) const
{
    return mAddress;
}

inline const String & StubBase::getServiceRole( void ) const
{
    return mAddress.getRoleName( );
}

inline const String& StubBase::getServiceName(void) const
{
    return mAddress.getServiceName();
}

#endif  // AREG_COMPONENT_STUBBASE_HPP
