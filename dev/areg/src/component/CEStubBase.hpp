#ifndef AREG_COMPONENT_CESTUBBASE_HPP
#define AREG_COMPONENT_CESTUBBASE_HPP
/************************************************************************
 * \file        areg/src/component/CEStubBase.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Stub Base class.
 *              This is Base class of all Stub objects. Derive class and
 *              implement pure virtual methods.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "areg/src/base/TEResourceMap.hpp"
#include "areg/src/component/CEStubEvent.hpp"
#include "areg/src/component/CEProxyAddress.hpp"
#include "areg/src/component/CEStubAddress.hpp"
#include "areg/src/component/NEService.hpp"

/************************************************************************
 * Definition of Session ID. Used when unblocking request
 ************************************************************************/
typedef unsigned int    SessionID;

/************************************************************************
 * Dependencies
 ************************************************************************/
class CERemoteNotifyRequestEvent;
class CEServiceResponseEvent;
class CERemoteRequestEvent;
class CEComponentThread;
class CEEventDataStream;
class CEResponseEvent;
class CEComponent;

//////////////////////////////////////////////////////////////////////////
// CEStubBase class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       This is pure virtual base class for all Stub objects 
 *              derived from Component Event Consumer interface. 
 *              Overwrite pure virtual methods to instantiate Stub objects
 * 
 * \details     In this class it is provided main basic logic of 
 *              asynchronous communication. It keeps track of requests,
 *              contains list of update notification listeners and has.
 *              The full logic of Service Interface implementation should
 *              be done in every Stub object separately.
 *
 **/
class AREG_API CEStubBase    : public IEStubEventConsumer
{
//////////////////////////////////////////////////////////////////////////
// friend classes
//////////////////////////////////////////////////////////////////////////
    friend class CERemoteEventFactory;

//////////////////////////////////////////////////////////////////////////
// Internal constants and definitions
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Constant. Defines Invalid Session ID
     **/
    static const SessionID  INVALID_SESSION_ID  /*= static_cast<SessionID>(-1)*/;   /*0xFFFFFFFF*/

protected:
    /**
     * \brief   This is internal class to track list of assigned listeners
     *          for requests and attribute update notifications. 
     *          It contains message ID (request or attribute ID), 
     *          message sequence number and address of Proxy object to send
     *          response message
     *
     **/
    //////////////////////////////////////////////////////////////////////////
    // CEStubBase::CEListener class declaration
    //////////////////////////////////////////////////////////////////////////
    class AREG_API CEListener
    {
    //////////////////////////////////////////////////////////////////////////
    // Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Creates undefined listener.
         **/
        inline CEListener( void );

        /**
         * \brief   Initialize Message ID. The Proxy Address should be set. If needed, update sequence number.
         **/
        inline CEListener(unsigned int reqId);

        /**
         * \brief   Initialize message ID and sequence number. The target Proxy Address should be set manually.
         **/
        inline CEListener(unsigned int reqId, unsigned int seqId);

        /**
         * \brief   Creates Listener from given parameters.
         * \param   reqId   The message ID.
         * \param   seqId   The Sequence number.
         * \param   proxy   The target proxy address.
         **/
        inline CEListener(unsigned int reqId, unsigned int seqId, const CEProxyAddress & proxy);

        /**
         * \brief   Copy constructor.
         * \param   src     The source of data to copy.
         **/
        inline CEListener(const CEStubBase::CEListener & src);

    //////////////////////////////////////////////////////////////////////////
    // Operators
    //////////////////////////////////////////////////////////////////////////
    public:

        /**
         * \brief   Assigning operator. Copies listener data from given source.
         **/
        const CEStubBase::CEListener & operator = (const CEStubBase::CEListener& src);

        /**
         * \brief   Comparing operator, compares 2 listener objects.
         *          2 listeners are equal if message ID is same and
         *          if either sequence number is equal to 
         *          CEStubBase::CEListener::ANY_SEQUENCE_NR, or if
         *          they have same message sequence number and same
         *          target proxy address.
         **/
        bool operator == ( const CEStubBase::CEListener & other ) const;

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
        unsigned int    mSequenceNr;
        /**
         * \brief   The address of target Proxy object.
         **/
        CEProxyAddress  mProxy;
    };

    /**
     * \brief   CEStubBase::CEStubListenerList class defines list of pending listeners.
     **/
    //////////////////////////////////////////////////////////////////////////
    // CEStubBase::CEStubListenerList class declaration
    //////////////////////////////////////////////////////////////////////////
    class AREG_API CEStubListenerList    : public TELinkedList<CEStubBase::CEListener, const CEStubBase::CEListener &>
    {
    //////////////////////////////////////////////////////////////////////////
    // Constructor / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Default constructor.
         **/
        CEStubListenerList( void );

        /**
         * \brief   Destructor
         **/
        virtual ~CEStubListenerList( void );
    };

    /**
     * \brief   CEStubBase::CEStubSessionMap class defines list of Session IDs and unblocked requests.
     **/
    //////////////////////////////////////////////////////////////////////////
    // CEStubBase::CEStubSessionMap class declaration
    //////////////////////////////////////////////////////////////////////////
    typedef TEIntegerHashMapImpl<const CEStubBase::CEListener &>                                            ImplStubSessionMap;
    typedef TEIntegerHashMap<CEStubBase::CEListener, const CEStubBase::CEListener &, ImplStubSessionMap>    MapStubSession;

    /**
     * \brief   CEStubBase::CEStubMap class is a Stub resource map class to access Stub objects by address
     **/
    //////////////////////////////////////////////////////////////////////////
    // CEStubBase::CEStubMap class declaration.
    //////////////////////////////////////////////////////////////////////////
    class AREG_API CEStubMapImpl    : public TEHashMapImpl<const CEStubAddress &, CEStubBase *>
    {
    public:
        /**
         * \brief   Called to calculate the 32-bit hash key value.
         * \ param  Key     The object to calculate 32-bit hash key.
         * \return  Returns 32-bit hash key value.
         **/
        inline unsigned int ImplHashKey( const CEStubAddress & Key ) const
        {
            return static_cast<unsigned int>(Key);
        }

        /**
         * \brief   Compares 2 keys, returns true if they are equal.
         * \param   Value1  The key of right-side object to compare.
         * \param   Value2  The key of left-side object to compare.
         * \return  Returns true if 2 keys are equal.
         **/
        inline bool ImplEqualKeys( const CEStubAddress & Key1, const CEStubAddress & Key2 ) const
        {
            return ( static_cast<const CEServiceAddress &>(Key1) == static_cast<const CEServiceAddress &>(Key2) && Key1.GetThread() == Key2.GetThread() );
        }
    };

    typedef TEHashMap<CEStubAddress, CEStubBase*, const CEStubAddress&, CEStubBase*, CEStubMapImpl>     MapStub;
    typedef TEResourceMapImpl<CEStubAddress, CEStubBase>                                                ImplStubResource;
    /**
     * \brief   Resource Map definition.
     **/
    typedef TELockResourceMap<CEStubAddress, CEStubBase, MapStub, ImplStubResource>                     MapStubResource;

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
    CEStubBase( CEComponent & masterComp, const NEService::SInterfaceData & siData );

    /**
     * \brief   Destructor.
     **/
    virtual ~CEStubBase( void );

//////////////////////////////////////////////////////////////////////////
// Operations. Public
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns Component Master thread object.
     **/
    CEComponentThread & GetMasterThread( void ) const;

    /**
     * \brief   Returns the address of Stub object.
     **/
    const CEStubAddress & GetAddress( void ) const;

    /**
     * \brief   Sends error event to all pending responses and notification updates
     **/
    void ErrorAllRequests( void );

    /**
     * \brief   Sends error event to all pending responses and unlocks requests.
     **/
    void CancelAllRequests( void );

    /**
     * \brief   Search stub object by given stub address and if
     *          found, returns valid pointer of stub object.
     * \param   address     The Address of Stub object.
     * \return  If found, returns valid pointer of Stub object.
     *          Otherwise returns NULL.
     **/
    static CEStubBase * FindStubByAddress(const CEStubAddress& address);

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// CEStubBase overrides. Triggered by Component on startup.
/************************************************************************/

    /**
     * \brief   This function is triggered by Component when starts up.
     *          Overwrite this method and set appropriate request and
     *          attribute update notification event listeners here
     * \param   holder  The holder component of service interface of Stub,
     *                  which started up.
     **/
    virtual void StartupServiceInterface(CEComponent& holder);

    /**
     * \brief   This function is triggered by Component when shuts down.
     *          Overwrite this method to remove listeners and stub cleanup
     * \param   holder  The holder component of service interface of Stub,
     *                  which shuts down.
     **/
    virtual void ShutdownServiceIntrface(CEComponent& holder);

/************************************************************************/
// CEStubBase overrides. 
/************************************************************************/
    /**
     * \brief   This function unblocks current request to be able to get
     *          same response again and return unique session ID.
     *          Use this session ID to prepare response to send response.
     *          The caller of this function should save Session ID
     *          for later use to prepare response.
     **/
    virtual SessionID UnblockCurrentRequest( void );

    /**
     * \brief   By given unique Session ID, prepares response to send.
     * \param   sessionId   The Session ID returned by calling
     *                      UnblockCurrentRequest()
     **/
    virtual void PrepareResponse(SessionID sessionId);

    /**
     * \brief   Triggered when proxy client either connected or disconnected to stub.
     * \param   client      The address of proxy client, which connection status is changed.
     * \param   isConnected Flag, indicating whether client is connected or disconnected.
     *                      When client disconnects, all listeners are removed.
     **/
    virtual void ClientConnected( const CEProxyAddress & client, bool isConnected );

/************************************************************************/
// CEStubBase overrides. Public pure virtual methods 
/************************************************************************/

    /**
     * \brief   Returns implemented version of service interface.
     **/
    virtual const CEVersion & GetImplVersion( void ) const = 0;

    /**
     * \brief   Sends update notification message to all clients. 
     *          This method can be called manually to send update 
     *          notification message after updating attribute value.
     *
     *          Overwrite to implement method
     *
     * \param   msgId   The attribute message ID to notify clients.
     **/
    virtual void SendNotification( unsigned int msgId ) = 0;

    /**
     * \brief   Sends error message to clients.
     *          If message ID is a request, it should send result NEService::RESULT_REQUEST_ERROR or NEService::RESULT_REQUEST_CANCELED, depending on msgCancel flag.
     *          If message ID is a response, it should send result NEService::RESULT_INVALID.
     *          If message ID is an attribute, it should send result NEService::RESULT_DATA_INVALID
     *          and invalidate attribute data value.
     *
     *          Overwrite to implement method
     *
     * \param   msgId       The message ID to send error message
     * \param   msgCancel   Indicates whether the request is canceled or should respond with error.
     *                      This parameter has sense only for request IDs.
     *                      It is ignored for response and attributes IDs.
     **/
    virtual void ErrorRequest( unsigned int msgId, bool msgCancel ) = 0;

protected:
/************************************************************************/
// CEStubBase overrides. Protected pure virtual methods 
/************************************************************************/
    /**
     * \brief   Returns number of requests of Service Interface
     *
     * \remark  Overwrite to implement method
     **/
    virtual unsigned int GetNumberOfRequests( void ) const = 0;

    /**
     * \brief   Returns number of responses of Service Interface
     *
     * \remark  Overwrite to implement method
     **/
    virtual unsigned int GetNumberOfResponses( void ) const = 0;

    /**
     * \brief   Returns number of attributes of Service Interface
     *
     * \remark  Overwrite to implement method
     **/
    virtual unsigned int GetNumberOfAttributes( void ) const = 0;

    /**
     * \brief   Returns pointer of array of requests IDs of Service Interface
     *
     * \remark  Overwrite to implement method
     **/
    virtual const unsigned int * GetRequestIds( void ) const = 0;

    /**
     * \brief   Returns pointer of array of response IDs of Service Interface
     *
     * \remark  Overwrite to implement method
     **/
    virtual const unsigned int * GetResponseIds( void ) const = 0;

    /**
     * \brief   Returns pointer of array of attribute IDs of Service Interface
     *
     * \remark  Overwrite to implement method
     **/
    virtual const unsigned int * GetAttributeIds( void ) const = 0;

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
    virtual CEResponseEvent * CreateResponseEvent( const CEProxyAddress & proxy, unsigned int msgId, NEService::eResultType result, const CEEventDataStream & data ) const = 0;

    /**
     * \brief   Overwrite method to create remote service request event from streaming object for 
     *          further dispatching by stub.
     * \param   stream  Streaming object, which contains event data.
     * \return  If operation succeeds, returns valid pointer to Service Request event object.
     *          Otherwise, it returns NULL.
     **/
    virtual CERemoteRequestEvent * CreateRemoteServiceRequestEvent( const IEInStream & stream ) const = 0;

    /**
     * \brief   Overwrite method to create remote notify request event from streaming object for 
     *          further dispatching by stub.
     * \param   stream  Streaming object, which contains event data.
     * \return  If operation succeeds, returns valid pointer to Service Request event object.
     *          Otherwise, it returns NULL.
     **/
    virtual CERemoteNotifyRequestEvent * CreateRemoteNotifyRequestEvent( const IEInStream & stream ) const = 0;

protected:
/************************************************************************/
// IEStubEventConsumer interface overrides.
/************************************************************************/

    /**
     * \brief   Triggered by system when stub is registered in service. The connection status indicated
     *          registration status. If succeeded, the value is NEService::ServiceConnected
     * \param   stubTarget          The address of registered Stub
     * \param   connectionStatus    Stub registration status.
     **/
    virtual void ProcessStubRegisteredEvent( const CEStubAddress & stubTarget, NEService::eServiceConnection connectionStatus );

    /**
     * \brief   Send by system when client is requested connect / disconnect
     * \param   proxyAddress        The address of source proxy
     * \param   connectionStatus    Connection status of specified client
     **/
    virtual void ProcessClientConnectEvent( const CEProxyAddress & proxyAddress, NEService::eServiceConnection connectionStatus );

    /**
     * \brief   Triggered to process generic stub event.
     *          Usually should not be triggered.
     **/
    virtual void ProcessStubEvent( CEStubEvent & eventElem ) = 0;

    /**
     * \brief   Triggered to process service request event.
     *          Overwrite method to process every service request event.
     * \param   eventElem   Service Request Event object, contains request
     *                      call ID and parameters.
     **/
    virtual void ProcessRequestEvent( CEServiceRequestEvent & eventElem ) = 0;
    
    /**
     * \brief   Triggered to process attribute update notification event.
     *          Override method to process request to get attribute value and
     *          process notification request of attribute update.
     * \param   eventElem   Service Request Event object, contains attribute ID.
     **/
    virtual void ProcessAttributeEvent( CEServiceRequestEvent & eventElem ) = 0;

    /**
     * \brief   Triggered to process generic event. Usually is not triggered.
     **/
    virtual void ProcessGenericEvent(CEEvent & eventElem) = 0;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations. Protected.
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Returns true if specified request is in pending list,
     *          is not released and marked as busy.
     *          Used internally.
     **/
    bool IsBusy(unsigned int requestId) const;

    /**
     * \brief   Prepares Request, adds request listener to th listener list
     *          and waits for release by calling response. Only requests, which
     *          have responses are added to request listener list.
     * \param   listener    The listener object to prepare.
     * \param   seqNr       The sequence number of call request.
     * \param   requestId   The triggered request ID .
     **/
    void PrepareRequest(CEStubBase::CEListener & listener, unsigned int seqNr, unsigned int requestId);

    /**
     * \brief   Search and add all listeners, which have same specified request ID 
     *          and returns the size of filled listener list. If no listener could find,
     *          returns empty list.
     * \param   requestId       The request ID to filter listeners in listener list
     * \param   out_listners    On output, this contains list of listeners having
     *                          specified request ID.
     * \return  Returns the size of filtered listener list.
     **/
    int FindListeners(unsigned int requestId, CEStubListenerList & out_listners) const;

    /**
     * \brief   Searches notification listener in the list of listeners and returns true
     *          if listener is assigned. 2 notification listeners are equal if 
     *          they are marked as notification listener (sequence number is notification),
     *          the message ID and the Proxy addresses are same.
     * \param   msgId           The ID of notification message (normally, either attribute or response message ID).
     * \param   notifySource    The address of Proxy source.
     * \return  Returns true if notification listener is already registered in the list of listeners.
     **/
    bool NotificationListenerExist( unsigned int msgId, const CEProxyAddress & notifySource ) const;

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
    bool AddNotificationListener( unsigned int msgId, const CEProxyAddress & notifySource );

    /**
     * \brief   Removes notification listener from the listener list.
     *          The notification listener exist in the list, it its  sequence number is notification,
     *          message ID and proxy address are same. If entry found in the list, it will be removed.
     *          Otherwise, there will be no operation performed.
     * \param   msgId           The ID of notification message (normally, either attribute or response message ID).
     * \param   notifySource    The address of Proxy source.
     **/
    void RemoveNotificationListener( unsigned int msgId, const CEProxyAddress & notifySource );

    /**
     * \brief   Returns all listeners for specified proxy and on output returns
     *          list of remove request IDs.
     * \param   whichProxy  The address of request source proxy to remove.
     * \param   removedIDs  The list of removed request IDs
     **/
    void ClearAllListeners(const CEProxyAddress & whichProxy, CEIntegerArray& removedIDs);
    /**
     * \brief   Returns all listeners for specified proxy and on output returns
     *          list of remove request IDs.
     * \param   whichProxy  The address of request source proxy to remove.
     * \param   removedIDs  The list of removed request IDs
     **/
    void ClearAllListeners(const CEProxyAddress & whichProxy);

    /**
     * \brief   Sends attribute notification response to all target proxy objects
     *          listed in given listener list.
     * \param   whichListeners  The list of listeners, containing target proxy address,
     *                          to send update notification event.
     * \param   eventElem       The event, containing updated object ID and the new
     *                          value of attribute.
     **/
    void SendResponseNotification( const CEStubBase::CEStubListenerList & whichListeners, const CEServiceResponseEvent & masterEvent );

    /**
     * \brief   Sends error message for requested to get attribute.
     * \param   whichListeners  The list of listeners containing target
     *                          proxy address to send error notification.
     * \param   eventElem       The event, containing error type and 
     *                          attribute object ID.
     **/
    void SendErrorNotification( const CEStubBase::CEStubListenerList & whichListeners, const CEServiceResponseEvent & masterEvent );

    /**
     * \brief   Sends attribute update notification message to all
     *          listed listeners, containing target proxy addresses.
     * \param   whichListeners  The list of listeners, containing
     *                          address of proxy to send update notification.
     * \param   eventElem       The event message to send, which contains 
     *                          attribute object ID, update type and new 
     *                          updated  value of attribute.
     **/
    void SendUpdateNotification( const CEStubBase::CEStubListenerList & whichListeners, const CEServiceResponseEvent & masterEvent ) const;

    /**
     * \brief   Sends Service Response message to trigger response call 
     *          on Proxy and Clients side
     * \param   eventElem   Service response event to send.
     **/
    void SendServiceResponse( CEServiceResponseEvent & eventElem ) const;

    /**
     * \brief   Cancel current request.
     **/
    void CancelCurrentRequest( void );

    /**
     * \brief   Invalidates specified attribute, which should send error notification to clients.
     * \param   attrId  The ID of attribute to invalidate.
     * \note    The function does not check the validity of ID
     **/
    void InvalidateAttribute(unsigned int attrId);


    /**
     * \brief   Sends update event to all proxy objects. The list of proxy listeners is selected
     *          by message ID.
     * \param   msgId   The message ID to send to proxy objects
     * \param   data    The buffer of serialized data to send to proxy. Can be Invalid buffer
     * \param   result  The result to send to proxy objects.
     **/
    void SendUpdateEvent(unsigned int msgId, const CEEventDataStream & data, NEService::eResultType result) const;

    /**
     * \brief   Sends response event to proxy. The list of proxy listeners is selected by message ID.
     * \param   respId  The ID of response to send to proxy objects
     * \param   data    The buffer of serialized data to send to proxy (should be serialized arguments of response call)
     **/
    void SendResponseEvent(unsigned int respId, const CEEventDataStream & data);

    /**
     * \brief   Sends busy response on request from client side. If the stub already processing request, 
     *          this request is pending (response is pending) and is not unblocked, it send busy message 
     *          to the proxy triggered request call.
     * \param   whichListener   The listener object containing proxy address and message ID to send busy message.
     **/
    void SendBusyRespone(const CEStubBase::CEListener & whichListener);

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
    bool CanExecuteRequest( CEStubBase::CEListener & whichListener, unsigned int whichResponse, unsigned int seqNr);

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Holder component object reference.
     **/
    CEComponent &                   mComponent;

    /**
     * \brief   The address object of stub
     **/
    CEStubAddress                   mAddress;

    /**
     * \brief   The service connection status
     **/
    NEService::eServiceConnection   mConnectionStatus;

    /**
     * \brief   The list of listeners
     **/
    CEStubBase::CEStubListenerList  mListListener;

private:
    /**
     * \brief   The position of current listener, which is processing. When canceled, it sets NULL.
     **/
    LISTPOS                         mCurrListener;

    /**
     * \brief   Used to generate unique session ID. The uniqueness is provided within single stub object scope
     **/
    unsigned int                    mSessionId;

private:
#if _MSC_VER
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
    /**
     * \brief   Session map object, contains list of unblock requests
     **/
    CEStubBase::MapStubSession      mMapSessions;

    /**
     * \brief   Stub object resource map.
     **/
    static MapStubResource          _mapRegisteredStubs;
#if _MSC_VER
    #pragma warning(default: 4251)
#endif  // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns reference to Stub object
     **/
    inline CEStubBase & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CEStubBase( void );
    CEStubBase(const CEStubBase & /*src*/ );
    const CEStubBase& operator = (const CEStubBase & /*src*/);
};

//////////////////////////////////////////////////////////////////////////
// Inline function implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * CEStubBase::CEListener class
 ************************************************************************/
//////////////////////////////////////////////////////////////////////////
// CEStubBase::CEListener class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline CEStubBase::CEListener::CEListener( void )
    : mMessageId(0), mSequenceNr(0), mProxy(CEProxyAddress::INVALID_PROXY_ADDRESS)
{   ;   }

inline CEStubBase::CEListener::CEListener( unsigned int reqId )
    : mMessageId(reqId), mSequenceNr(0), mProxy(CEProxyAddress::INVALID_PROXY_ADDRESS)
{   ;   }

inline CEStubBase::CEListener::CEListener( unsigned int reqId, unsigned int seqId )
    : mMessageId(reqId), mSequenceNr(seqId), mProxy(CEProxyAddress::INVALID_PROXY_ADDRESS)
{   ;   }

inline CEStubBase::CEListener::CEListener( unsigned int reqId, unsigned int seqId, const CEProxyAddress& proxy )
    : mMessageId(reqId), mSequenceNr(seqId), mProxy(proxy)
{   ;   }

inline CEStubBase::CEListener::CEListener( const CEStubBase::CEListener& src )
    : mMessageId(src.mMessageId), mSequenceNr(src.mSequenceNr), mProxy(src.mProxy)
{   ;   }

/************************************************************************
 * CEStubBase class
 ************************************************************************/
//////////////////////////////////////////////////////////////////////////
// CEStubBase class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline CEStubBase & CEStubBase::self( void )
{   return (*this);         }

#endif  // AREG_COMPONENT_CESTUBBASE_HPP
