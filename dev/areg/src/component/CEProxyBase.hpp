#ifndef AREG_COMPONENT_CEPROXYBASE_HPP
#define AREG_COMPONENT_CEPROXYBASE_HPP
/************************************************************************
 * \file        areg/src/component/CEProxyBase.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Proxy Base class.
 *              All Proxy classes should derive from this class and
 *              extend functionalities.
 *              This class is pure virtual and cannot be instantiated.
 *              Derive this class and override pure virtual methods.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "areg/src/base/TEHashMap.hpp"
#include "areg/src/base/TEArrayList.hpp"
#include "areg/src/base/TEResourceMap.hpp"
#include "areg/src/component/CEProxyEvent.hpp"
#include "areg/src/component/CEProxyAddress.hpp"

#include "areg/src/component/NEService.hpp"
#include "areg/src/component/CEStubAddress.hpp"


/************************************************************************
 * Dependencies
 ************************************************************************/
class IENotificationEventConsumer;
class CENotificationEventData;
class CEServiceResponseEvent;
class CERemoteResponseEvent;
class CEServiceRequestEvent;
class CENotificationEvent;
class CEDispatcherThread;
class CEEventDataStream;
class IEProxyListener;
class CEProxyEvent;
class CEProxyBase;
class CEVersion;

/************************************************************************
 * Global types
 ************************************************************************/
/**
 * \brief   Function type to create a Proxy object.
 * \param   roleName    The role name of servicing component to connect.
 * \param   ownerThread The instance of thread to dispatch messages.
 *                      If NULL, uses current component thread.
 **/
typedef CEProxyBase* (*FuncCreateProxy)( const char* /*roleName*/, CEDispatcherThread * /*ownerThread*/ );

//////////////////////////////////////////////////////////////////////////
// CEProxyBase class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       Proxy Base class is a base class for all instantiated
 *              proxies in application. It provides communication
 *              functionalities with server side and functionalities
 *              to send notifications to its clients. All communications
 *              between clients and servers are passing via their 
 *              Proxy objects.
 * 
 * \details     There is only one certain instance of proxy is instantiated
 *              in every thread, even if there are more clients in thread.
 *              By the first client request to create Proxy, the Proxy
 *              object is instantiated. In all other cases, if other
 *              clients are in the same thread and they request to create
 *              Proxy, the system will return pointer of already created
 *              Proxy object. On instantiation, Proxy is sending message
 *              to Service Manager object to check whether server object
 *              is already instantiated and exists in the system.
 *              If server exists, Service Manager will send connect message,
 *              that Proxy can setup Listener objects and operate.
 *              Before connect message, there will be no communication
 *              message sent from client side to server. All messages
 *              will be dropped on Proxy side, assuming that there is
 *              no server available yet.
 *              The Proxy also tracks request and its response communication
 *              mechanism that the right client gets response notification,
 *              as well as forwards attribute update notification messages
 *              from server side to all clients assigned for update
 *              notification. All attribute data and parameter data should
 *              be saved on Proxy side to share between multiple instances
 *              of client in the same thread.
 *
 **/
class AREG_API CEProxyBase  : public    IEProxyEventConsumer
{
    friend class CERemoteEventFactory;
//////////////////////////////////////////////////////////////////////////
// Internal classes, types and constants
//////////////////////////////////////////////////////////////////////////
private:
    //////////////////////////////////////////////////////////////////////////
    // CEProxyBase::CEListener class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   This is internal listener class to forward response and 
     *          update notification events to clients.
     *          It contains information of message ID, sequence number and
     *          pointer of client object. Every proxy has list of client
     *          listeners only instantiated within same thread.
     **/
    class AREG_API CEListener
    {
    //////////////////////////////////////////////////////////////////////////
    // CEProxyBase::CEListener class, Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Default constructor.
         **/
        CEListener( void );

        /**
         * \brief   Copy constructor.
         * \param   src     The source of data to copy.
         **/
        CEListener( const CEListener & src );

        /**
         * \brief   Creates Listener and sets message ID.
         * \param   msgId   Message ID
         **/
        CEListener( unsigned int msgId );

        /**
         * \brief   Creates Listener, sets message ID and sequence number.
         * \param	msgId	Message ID.
         * \param	seqNr	Sequence Number.
         **/
        CEListener( unsigned int msgId, unsigned int seqNr );


        /**
         * \brief   Creates Listener, sets message ID, sequence number and client listener pointer.
         * \param   msgId       Message ID.
         * \param   seqNr       Sequence Number
         * \param   consumer    Client listener pointer 
         **/
        CEListener(unsigned int msgId, unsigned int seqNr, IENotificationEventConsumer * caller);

    //////////////////////////////////////////////////////////////////////////
    // CEProxyBase::CEListener class, Basic operators
    //////////////////////////////////////////////////////////////////////////
    public:

        /**
         * \brief   Copies listener data from given source.
         * \param   src     The source of listener object.
         **/
        const CEProxyBase::CEListener & operator = ( const CEProxyBase::CEListener & src );

        /**
         * \brief   Checks equality of 2 listener objects. 2 listener objects are equal if either they have identical data,
         *          or if have same message ID, but the sequence number of one of listeners is SEQUENCE_NR_ANY (0xFFFFFFFF).
         * \param   other   The listener object to compare.
         * \return  Returns true, if 2 objects are equal.
         **/
        bool operator == ( const CEProxyBase::CEListener & other ) const;

    //////////////////////////////////////////////////////////////////////////
    // Member variables
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Message ID
         **/
        unsigned int                    mMessageId;
        /**
         * \brief   Sequence number of listener. Attribute update listeners should have zero sequence number
         **/
        unsigned int                    mSequenceNr;
        /**
         * \brief   Pointer to notification event listener object, which should be instance of Proxy client.
         **/
        IENotificationEventConsumer *  mListener;
    };

    //////////////////////////////////////////////////////////////////////////
    // CEProxyBase::CEProxyListenerList class declaration
    //////////////////////////////////////////////////////////////////////////
    /************************************************************************
     * \brief   Proxy Listener List class to save list of listener objects.
     *          Every Proxy class has list of listeners.
     ************************************************************************/
    class AREG_API CEProxyListenerList   : public TEArrayList<CEProxyBase::CEListener, const CEProxyBase::CEListener &>
    {
    //////////////////////////////////////////////////////////////////////////
    // CEProxyBase::CEProxyListenerList class, Constructor / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Default Constructor.
         * \param   capacity    Initial size of Array List.
         **/
        CEProxyListenerList( int capacity = 0 );
        /**
         * \brief   Destructor.
         **/
        virtual ~CEProxyListenerList( void );
    };

    //////////////////////////////////////////////////////////////////////////
    // CEProxyBase::CEProxyMap class declaration.
    //////////////////////////////////////////////////////////////////////////
    /************************************************************************
     * \brief   Proxy map is used to keep control of instantiated resource.
     *          As well as to lookup for already existing instance or proxy
     *          in the same thread. As a Key, it is using Proxy Address
     *          and value is instance of Proxy.
     ************************************************************************/
    class AREG_API CEProxyMapImpl   : public TEHashMapImpl<const CEProxyAddress &, const CEProxyBase *>
    {
    public:
        /**
         * \brief   Called to calculate the 32-bit hash key value.
         * \ param  Key     The object to calculate 32-bit hash key.
         * \return  Returns 32-bit hash key value.
         **/
        inline unsigned int ImplHashKey( const CEProxyAddress & Key ) const
        {
            return static_cast<unsigned int>( Key );
        }

        /**
         * \brief   Compares 2 keys, returns true if they are equal.
         * \param   Value1  The key of right-side object to compare.
         * \param   Value2  The key of left-side object to compare.
         * \return  Returns true if 2 keys are equal.
         **/
        inline bool ImplEqualKeys( const CEProxyAddress & Key1, const CEProxyAddress & Key2 ) const
        {
            return ( static_cast<const CEServiceAddress &>(Key1) == static_cast<const CEServiceAddress &>(Key2) && Key1.GetThread() == Key2.GetThread() );
        }
    };

    typedef TEHashMap<CEProxyAddress, CEProxyBase*, const CEProxyAddress&, const CEProxyBase*, CEProxyMapImpl>  MapProxy;
    typedef TEResourceMapImpl<CEProxyAddress, CEProxyBase>                                                      ImplProxyResource;

    /**
     * \brief   CEProxyBase::MapProxyResource
     *          Proxy Resource Map declaration to keep controlling of all instantiated Proxy objects.
     * \tparam  CEProxyAddress  The Key of Resource map is a Proxy address object.
     * \tparam  CEProxyBase     The Values are pointers of Proxy object.
     * \tparam  CEProxyMap      The type of Hash Mapping object used as container
     **/
    typedef TELockResourceMap<CEProxyAddress, CEProxyBase, MapProxy, ImplProxyResource>                         MapProxyResource;

protected:
    //////////////////////////////////////////////////////////////////////////
    // CEProxyBase::CEServiceAvailableEvent internal class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Proxy Service available event to notify new connected client.
     *          If a service is already connected and a new client is instantiated,
     *          the Proxy should be able immediately inform service availability.
     *          This event is used to be able to notify client within component
     *          thread context even if client was instantiated in other thread.
     **/
    class AREG_API CEServiceAvailableEvent   : public CEThreadEventBase
    {
    //////////////////////////////////////////////////////////////////////////
    // Runtime internals
    //////////////////////////////////////////////////////////////////////////
        DECLARE_RUNTIME_EVENT( CEServiceAvailableEvent )
    //////////////////////////////////////////////////////////////////////////
    // Constructor/ Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Sets event consumer object to deliver notification.
         **/
        CEServiceAvailableEvent( IENotificationEventConsumer & consumer );
        /**
         * \brief   Destructor
         **/
        virtual ~CEServiceAvailableEvent( void );

    //////////////////////////////////////////////////////////////////////////
    // Attributes
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Returns instance of consumer to send notification
         **/
        IENotificationEventConsumer & GetConsumer( void ) const;
    //////////////////////////////////////////////////////////////////////////
    // Attributes
    //////////////////////////////////////////////////////////////////////////
    private:
        /**
         * \brief   Instance of consumer to send service available notification.
         **/
        IENotificationEventConsumer & mConsumer;
    //////////////////////////////////////////////////////////////////////////
    // Forbidden calls
    //////////////////////////////////////////////////////////////////////////
    private:
        CEServiceAvailableEvent( void );
        CEServiceAvailableEvent( const CEServiceAvailableEvent & /*src*/ );
        const CEServiceAvailableEvent & operator = ( const CEServiceAvailableEvent & /*src*/ );
    };
//////////////////////////////////////////////////////////////////////////
// CEProxyBase class static methods
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Finds already existing proxy object or creates new one.
     *          By calling this function, the system first will lookup
     *          for existing proxy object, instantiated int current thread.
     *          If proxy is not existing, it will create new object or
     *          return pointer to already instantiated proxy object.
     *          If Proxy already exists, on every call of this function
     *          it will increase reference count.
     * \param   roleName    The role name of server component
     * \param   serviceName Implemented Service Name. 
     *                      Every Service Interface should have name
     * \param   version     The version number of implemented service.
     * \param   connect     The object, which should be notified when
     *                      server component accepts proxy connection
     * \param   funcCreate  The pointer to function which should instantiate
     *                      Proxy object if it is not existing in system.
     * \param   ownerThread The owner dispatcher thread name where the messages are dispatched.
     *                      If NULL, it searches Proxy instance in current thread.
     * \return  Returns pointer to Proxy object.
     **/
    static CEProxyBase * FindOrCreateProxy(  const char * roleName
                                            , const NEService::SInterfaceData & serviceIfData
                                            , IEProxyListener & connect
                                            , FuncCreateProxy funcCreate
                                            , const char * ownerThread = static_cast<const char *>(NULL) );

    /**
     * \brief   Finds already existing proxy object or creates new one.
     *          By calling this function, the system first will lookup
     *          for existing proxy object, instantiated int current thread.
     *          If proxy is not existing, it will create new object or
     *          return pointer to already instantiated proxy object.
     *          If Proxy already exists, on every call of this function
     *          it will increase reference count.
     * \param   roleName    The role name of server component
     * \param   serviceName Implemented Service Name. 
     *                      Every Service Interface should have name
     * \param   version     The version number of implemented service.
     * \param   connect     The object, which should be notified when
     *                      server component accepts proxy connection
     * \param   funcCreate  The pointer to function which should instantiate
     *                      Proxy object if it is not existing in system.
     * \param   ownerThread The instance of owner thread where the messages are dispatched.
     * \return  Returns pointer to Proxy object.
     **/
    static CEProxyBase * FindOrCreateProxy(  const char * roleName
                                            , const NEService::SInterfaceData & serviceIfData
                                            , IEProxyListener & connect
                                            , FuncCreateProxy funcCreate
                                            , CEDispatcherThread & ownerThread );

    /**
     * \brief   Lookup in registries for instantiated proxy object and
     *          if found, returns valid pointer to proxy object.
     * \param   proxyAddress    The Address of Proxy object.
     * \return  Returns pointer to Proxy object.
     **/
    static CEProxyBase * FindProxyByAddress( const CEProxyAddress & proxyAddress );

//////////////////////////////////////////////////////////////////////////
// CEProxyBase class, Constructor / Destructor.
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Receives the role name of server component and Service Interface 
     *          data structure to initialize Proxy internals.
     * \param   roleName        The role name of server component to connect
     * \param   serviceIfData   The Service Interface structure. Every proxy
     *                          should have defined Service Interface structure.
     * \param   ownerThread     The instance of Proxy owner thread to dispatch messages.
     *                          If NULL, the messages are dispatched in current thread.
     **/
    CEProxyBase( const char* roleName, const NEService::SInterfaceData & serviceIfData, CEDispatcherThread * ownerThread = static_cast<CEDispatcherThread *>(NULL) );

    /**
     * \brief   Destructor.
     **/
    virtual ~CEProxyBase( void );

//////////////////////////////////////////////////////////////////////////
// Public Operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Frees Proxy. Every client object which was requesting to 
     *          create Proxy object, should call this method to free resources.
     *          On every request to free Proxy object, this will reduce 
     *          reference counter. And when reference counter is zero,
     *          it will unregister Proxy object in the system, send
     *          disconnect notification to component server object
     *          and delete Proxy object.
     * \param   connect The object to notify when Proxy is disconnected.
     **/
    void FreeProxy( IEProxyListener & connect );

//////////////////////////////////////////////////////////////////////////
// Pure virtual methods to implement
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// IEProxyEventConsumer interface overrides. Should be implemented
/************************************************************************/

    /**
     * \brief   Method derived from IEProxyEventConsumer interface.
     *          Triggered when on the request to execute function
     *          on server side, Proxy have got response message.
     * \param   eventElem   The Service Response event object.
     *                      Contains response message and information
     *                      sent by Stub
     **/
    virtual void ProcessResponseEvent(CEServiceResponseEvent & eventElem) = 0;

    /**
     * \brief   Method derived from IEProxyEventConsumer interface.
     *          Triggered when on server side a certain Attribute 
     *          value has been updated.
     * \param   eventElem   The Service Response event object.
     *                      Contains new updated value of Attribute
     *                      and validation flag.
     **/
    virtual void ProcessAttributeEvent(CEServiceResponseEvent & eventElem) = 0;

/************************************************************************/
// CEProxyBase overrides. Should be implemented.
/************************************************************************/

    /**
     * \brief   Creates notification event to send to client objects. 
     *          All Notification Events should be internal events and 
     *          should be instances of CENotificationEvent class.
     *
     *          Overwrite this method.
     *
     * \param   data    The Notification Event data object containing
     *                  notification information.
     * \return  Returns new created notification event object.
     **/
    virtual CENotificationEvent * CreateNotificationEvent( const CENotificationEventData & data ) const = 0;

    /**
     * \brief   Create Request event to send to Stub object. 
     *          Request events are triggering request calls on Stub side.
     *
     *          Overwrite this method.
     *
     * \param   args    The buffer containing serialized arguments of request cal.
     * \param   reqId   The ID of request call.
     * \return  Return pointer of valid Request event.
     **/
    virtual CEServiceRequestEvent * CreateRequestEvent( const CEEventDataStream & args, unsigned int reqId ) = 0;

    /**
     * \brief   Creates event requesting to receive update notification events.
     *          The caller should be address of current proxy and the target 
     *          should address of appropriate sub address.
     *
     *          Overwrite this method.
     *
     * \param   msgId       The message ID to send. Should be either attribute or response (info)
     * \param   reqType     The type of request.
     * \return  Returns valid pointer of created service request event object.
     **/
    virtual CEServiceRequestEvent * CreateNotificationRequestEvent( unsigned int msgId, NEService::eRequestType reqType ) = 0;

    /**
     * \brief   Overwrite method to create response event from streaming object for 
     *          further dispatching by proxy.
     * \param   stream  Streaming object, which contains event data.
     * \return  If operation succeeds, returns valid pointer to Service Response event object.
     *          Otherwise, it returns NULL.
     **/
    virtual CERemoteResponseEvent * CreateRemoteResponseEvent( const IEInStream & stream ) const = 0;

    /**
     * \brief   Overwrite method to create error remote response event.
     *          Function is triggered when remote request sent by proxy failed and did not reach target.
     * \param   addrProxy   Address of Proxy object, which sent request event
     * \param   msgId       Message ID, which was requested to process
     * \param   reason      Failure reason set by system
     * \param   seqNr       The sequence number of processing message.
     **/
    virtual CERemoteResponseEvent * CreateRemoteRequestFailedEvent( const CEProxyAddress & addrProxy, unsigned int msgId, NEService::eResultType reason, unsigned int seqNr ) const = 0;

    /**
     * \brief   Overwrite this method to create service available event to new instantiated clients.
     * \param   consumer    The instance of consumer, which receives service available event.
     * \return  If succeeds, returns valid pointer to service available event object.
     **/
    virtual CEProxyBase::CEServiceAvailableEvent * CreateServiceAvailableEvent( IENotificationEventConsumer & consumer ) = 0;

//////////////////////////////////////////////////////////////////////////
// Overrides.
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// IEProxyEventConsumer interface overrides.
/************************************************************************/
    /**
     * \brief   Triggered, when current dispatching event is an instance of Proxy Event
     *          and should processed by proxy object.
     * \param   eventElem   Proxy event to process
     **/
    virtual void ProcessProxyEvent( CEProxyEvent & eventElem );

    /**
     * \brief   Triggered, when current dispatching event is not an instance of
     *          Proxy Event and should be processed by Proxy object.
     * \param   eventElem   Event object to process.
     **/
    virtual void ProcessGenericEvent( CEEvent & eventElem );

    /**
     * \brief   Triggered, when received server connection status changed.
     * \param   Server      The address of connected service stub server.
     * \param   Channel     Communication channel object to deliver events.
     * \param   Status      The service connection status. 
     *                      The connection status should be NEService::ServiceConnected
     *                      To be able to send message to service target from Proxy client.
     **/
    virtual void ServiceConnectionUpdated( const CEStubAddress & Server, const CEChannel & Channel, NEService::eServiceConnection Status );

    /**
     * \brief   Triggered when service available event is processed.
     *          The system checks validity of consumer and trigger appropriate
     *          service available method with appropriated availability flag.
     **/
    virtual void ProcessServiceAvailableEvent( IENotificationEventConsumer & consumer );

/************************************************************************/
// CEProxyBase interface overrides
/************************************************************************/

    /**
     * \brief	Unregisters listener and removes from list, clear all
     *          notifications related to this listener. For every removed
     *          listener, it checks whether there is any other listener
     *          assigned for removed object listener ID. If for removed
     *          message ID there is no other listener existing, it
     *          will send event to Stub object to stop sending event
     *          for particular message ID.
     * \param	listener    Pointer to listener object to unregister
     *
     * \remark  Use of CEProxyBase::UnregisterListener()
     *
     *          Following is a description of use case in which case
     *          Proxy is sending event to Stub to start and stop sending
     *          event. Assume service interface has an Attribute. On the
     *          request of client to be notified on attribute update,
     *          Proxy will create event and forward to Stub to start
     *          getting attribute update notifications. Then client object
     *          is request Proxy to stop notification. When Proxy is removing
     *          listener from the listener list, it will check whether there
     *          any other listeners assigned for removed listener message ID.
     *          If no more listener object in Listener List, it will create
     *          event object and send to Stop with stop notification flag.
     **/
    virtual void UnregisterListener( IENotificationEventConsumer * consumer );

    /**
     * \brief	Sends the notification event.
     *          By given parameters it instantiate notification event data,
     *          calls method to create notification event object, sets
     *          appropriate consumer to event object and sends event
     *          to push into queue of internal events of current dispatcher thread.
     *          Overwrite this method if additional job should be performed
     *          to send notification event.
     * \param	msgId	    The message ID of notification.
     * \param	resType	    Type of result to notify the listener
     * \param	seqNr	    Sequence number to use for listener searching
     * \param	consumer	Pointer to Listener object to be notified.
     **/
    virtual void SendNotificationEvent( unsigned int msgId, NEService::eResultType resType, unsigned int seqNr, IENotificationEventConsumer * caller );

    /**
     * \brief   Called to register all servicing listeners. It is called when proxy is instantiated.
     *          Overwrite method to add service event listeners.
     **/
    virtual void RegisterServiceListeners( void );

    /**
     * \brief   Called to unregister all servicing listeners. It is called when proxy is freed.
     *          Overwrite method to remove service event listeners.
     **/
    virtual void UnregisterServiceListeners( void );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns the address of Proxy.
     **/
    inline const CEProxyAddress & GetProxyAddress( void ) const;

    /**
     * \brief   Returns the address of target Stub object.
     **/
    inline const CEStubAddress & GetStubAddress( void ) const;

    /**
     * \brief   Returns true if Proxy have got server connected notification.
     **/
    inline bool IsConnected( void ) const;

    /**
     * \brief   Checks whether there are more listener objects
     *          assigned for specified message ID.
     *          The sequence number will be ignored.
     * \param   msgId   The message ID to check.
     * \return  Returns true if there are more listeners assigned.
     *          for specified message ID.
     **/
    inline bool HasAnyListener( unsigned int msgId ) const;

    /**
     * \brief   Checks whether there are more listener objects
     *          assigned for specified attribute update message ID.
     *          Only sequence number equal to zero, i.e. only
     *          attribute update notifications.
     * \param   msgId       The message ID to check.
     * \return  Returns true if there are more listeners assigned.
     *          for specified message ID.
     **/
    inline bool HasNotificationListener( unsigned int msgId ) const;

    /**
     * \brief   Returns the Proxy dispatcher thread.
     **/
    inline CEDispatcherThread & GetProxyDispatcherThread( void ) const;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Send Request Event to Stub, to start specific notification
     *          of specified message ID update. Can be either Attribute update or
     *          response message ID.
     * \param   msgId   The message ID to start notification.
     **/
    inline void StartNotification( unsigned int msgId );

    /**
     * \brief   Sends Request Event to Stub, to stop specific notification.
     *          Can be ether Attribute update or response message ID.
     * \param   msgId   The message ID to stop notification.
     **/
    inline void StopNotification( unsigned int msgId );

    /**
     * \brief   Sends remove all notification event message to Stub and
     *          stops all notifications. 
     **/
    inline void StopAllServiceNotifications( void );

    /**
     * \brief   Stops list of specified notifications
     * \param   notifyIds   The list of notification IDs to stop.
     * \param   count       The size of notification ID list
     **/
    inline void StopNotifications( const unsigned int notifyIds[], int count );

    /**
     * \brief   Returns reference of read-only Proxy Data object
     **/
    inline const NEService::CEProxyData & GetProxyData( void ) const;

    /**
     * \brief   Returns reference of Proxy Data object
     **/
    inline NEService::CEProxyData & GetProxyData( void );

    /**
     * \brief   Register Proxy object for certain event type.
     * \param   eventClass  Runtime Class ID of Event
     **/
    void RegisterForEvent( const CERuntimeClassID & eventClass );

    /**
     * \brief   Unregister Proxy object out of certain event type.
     * \param   eventClass  Runtime Class ID of Event
     **/
    void UnregisterForEvent( const CERuntimeClassID & eventClass );

    /**
     * \brief   Remove Proxy Listener entry from listener list.
     * \param   msgId       The message ID of listener to remove.
     * \param   seqNr       The sequence number of listener to remove.
     * \param   consumer    Notification Event consumer.
     **/
    inline void RemoveListener( unsigned int msgId, unsigned int seqNr, IENotificationEventConsumer * caller );

    /**
     * \brief   Add Proxy Listener entry to listener list.
     * \param   msgId       Message ID of listener
     * \param   seqNr       Sequence number of listener
     * \param   consumer    Pointer to Notification Event consumer object.
     * \return  Returns true if new listener has been added.
     *          If listener already exists, returns false.
     **/
    bool AddListener( unsigned int msgId, unsigned int seqNr, IENotificationEventConsumer * caller );

    /**
     * \brief   Checks whether there is already listener of Notification Event
     *          exists. If does not exit, adds new listener entry in the listener
     *          list. If need, send appropriate message to Stub to start 
     *          sending attribute update messages. If already listener
     *          exists in listener list, sends immediate update notification
     *          based on existing update data status.
     * \param   msgId           The Notification Message ID
     * \param   consumer        The pointer of Notification Event consumer
     * \param   alwaysNotify    The flag indicating whether notification message
     *                          should be sent if the the notification already is pending.
     **/
    void SetNotification( unsigned int msgId, IENotificationEventConsumer * caller, bool alwaysNotify = false );

    /**
     * \brief   Clears listener entries of specified Notification Event consumer
     * \param   msgId       The Notification Message ID
     * \param   consumer    The pointer of Notification Event Consumer.
     **/
    void ClearNotification( unsigned int msgId, IENotificationEventConsumer * caller );

    /**
     * \brief   Sets Data state of specified message ID in Proxy Data object
     * \param   msgId       Message ID, which data state should be set
     * \param   newState    The state to set.
     **/
    void SetState( unsigned int msgId, NEService::eDataStateType newState );

    /**
     * \brief   Sends notification events to notification listeners.
     *          Should be called after processing response event
     *          sent by Stub.
     * \param   respId          The actual message ID of notification message to send.
     * \param   result          The notification message result
     * \param   seqNrToSearch   Sequence number to notify.
     *                          If NEService::SEQUENCE_NUMBER_NOTIFY,
     *                          all notification listeners assigned for specified
     *                          message ID will get notification.
     **/
    void NotifyListeners( int respId, NEService::eResultType result, unsigned int seqNrToSearch );

    /**
     * \brief   Prepares list of listeners assigned for specified message ID.
     *          On output out_listenerList will contain list of listeners.
     * \param   out_listenerList    On output, it will contain the list
     *                              of listeners assigned for specified message ID.
     * \param   msgId               The message ID to get listeners.
     * \param   seqNrToSearch   Sequence number to notify.
     *                          If NEService::SEQUENCE_NUMBER_NOTIFY,
     *                          all notification listeners assigned for specified
     *                          message ID will get notification.
     * \return  Returns the size of listeners in output listener list.
     **/
    int PrepareListeners( CEProxyBase::CEProxyListenerList & out_listenerList, unsigned int msgId, unsigned int seqNrToSearch );

    /**
     * \brief   Sends request event
     * \param   reqId   The ID of request message. Should be valid ID.
     * \param   args    The buffer of serialized request call arguments. 
     *                  If request has not parameter, this can be Invalid / empty buffer.
     * \param   caller  The pointer of notification consumer. 
     *                  This parameter can be NULL only if request has not appropriate response.
     *                  Otherwise this should be valid pointer.
     * \return  
     **/
    void SendRequestEvent( unsigned int reqId, const CEEventDataStream & args, IENotificationEventConsumer * caller );

    /**
     * \brief   Sends request events to Stub object to start or stop receiving update notifications.
     * \param   msgId       The message ID to start or stop receiving updates. It should be either attribute ID or response (info). 
     * \param   reqType     The type of request. Should be either request to 
     *                      call function or to get attribute update notification.
     *                      See details in NEService::eRequestType
     **/
    void SendNotificationRequestEvent( unsigned int msgId, NEService::eRequestType reqType );

    /**
     * \brief   Returns true if specified consumer is registered in the listener list.
     **/
    bool IsServiceListenerRegistered( IENotificationEventConsumer & caller ) const;

    /**
     * \brief   Called to instantiate service available event to send to client.
     *          Override the method to instantiate appropriate event object.
     * \param   eventInstance   The instance of event to send.
     **/
    void SendServiceAvailableEvent( CEProxyBase::CEServiceAvailableEvent * eventInstance );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   The address of Proxy
     **/
    CEProxyAddress              mProxyAddress;

    /**
     * \brief   The address of Implemented Stub Service Interface
     **/
    CEStubAddress               mImplStubAddress;

    /**
     * \brief   Sequence number counter. Changed on ever request send
     **/
    unsigned int                mSequenceCount;

    /**
     * \brief   The list of notification listeners.
     **/
    CEProxyListenerList         mListenerList;

    /**
     * \brief   Flag, indicating whether Proxy is connected to server
     *          component or not. If it is not connected, no event
     *          message will be sent to Stub.
     **/
    bool                        mIsConnected;

    /**
     * \brief   Proxy data, containing service interface information
     *          attribute and parameter update state.
     **/
    NEService::CEProxyData     mProxyData;

    /**
     * \brief   The Proxy dispatcher thread object
     **/
    CEDispatcherThread &        mDispatcherThread;

    /**
     * \brief   Proxy instance reference counter. 
     *          On every request to start Proxy, this counter will 
     *          increase value. On request to free Proxy, it will
     *          decrease value. And when reaches zero, will delete
     *          Proxy object.
     **/
    unsigned int                mProxyInstCount;

private:
#if _MSC_VER
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
    /**
     * \brief   Resource of registered Proxies in the system.
     **/
    static MapProxyResource     _mapRegisteredProxies;
#if _MSC_VER
    #pragma warning(default: 4251)
#endif  // _MSC_VER

private:
    /**
     * \brief   Return reference to Proxy object
     **/
    inline CEProxyBase & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CEProxyBase( void );
    CEProxyBase( const CEProxyBase & /*src*/ );
    const CEProxyBase & operator = ( const CEProxyBase & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// CEProxyBase class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline CEProxyBase & CEProxyBase::self( void )
{   return (*this);                     }

inline const CEProxyAddress& CEProxyBase::GetProxyAddress( void ) const
{   return mProxyAddress;     }

inline const CEStubAddress& CEProxyBase::GetStubAddress( void ) const
{   return mImplStubAddress; }

inline bool CEProxyBase::IsConnected( void ) const
{   return mIsConnected;                }

inline bool CEProxyBase::HasAnyListener(unsigned int msgId) const
{   return mListenerList.ElementExist(CEProxyBase::CEListener(msgId, NEService::SEQUENCE_NUMBER_ANY)); }

inline bool CEProxyBase::HasNotificationListener(unsigned int msgId) const
{   return mListenerList.ElementExist(CEProxyBase::CEListener(msgId, NEService::SEQUENCE_NUMBER_NOTIFY)); }

inline void CEProxyBase::RemoveListener( unsigned int msgId, unsigned int seqNr, IENotificationEventConsumer* caller )
{   static_cast<void>(mListenerList.RemoveElement(CEProxyBase::CEListener(msgId, seqNr, caller)));      }

inline void CEProxyBase::StartNotification( unsigned int msgId )
{   if (IsConnected()) SendNotificationRequestEvent(msgId, NEService::REQUEST_START_NOTIFY);    }

inline void CEProxyBase::StopNotification( unsigned int msgId )
{   if (IsConnected()) SendNotificationRequestEvent(msgId, NEService::REQUEST_STOP_NOTIFY);     }

inline void CEProxyBase::StopAllServiceNotifications( void )
{   if (IsConnected()) SendNotificationRequestEvent(NEService::EMPTY_FUNCTION_ID, NEService::REQUEST_REMOVE_ALL_NOTIFY);     }

inline void CEProxyBase::StopNotifications( const unsigned int notifyIds[], int count )
{   for ( int i = 0; i < count; ++ i ) StopNotification(notifyIds[i]);                                  }

inline const NEService::CEProxyData & CEProxyBase::GetProxyData( void ) const
{   return mProxyData;  }

inline NEService::CEProxyData & CEProxyBase::GetProxyData( void )
{   return mProxyData;  }

inline CEDispatcherThread & CEProxyBase::GetProxyDispatcherThread( void ) const
{   return mDispatcherThread;   }

#endif  // AREG_COMPONENT_CEPROXYBASE_HPP
