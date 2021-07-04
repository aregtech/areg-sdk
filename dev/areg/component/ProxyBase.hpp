#ifndef AREG_COMPONENT_PROXYBASE_HPP
#define AREG_COMPONENT_PROXYBASE_HPP
/************************************************************************
 * \file        areg/component/ProxyBase.hpp
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
#include "areg/base/GEGlobal.h"
#include "areg/base/TEHashMap.hpp"
#include "areg/base/TEArrayList.hpp"
#include "areg/base/TELinkedList.hpp"
#include "areg/base/TEResourceMap.hpp"
#include "areg/base/TEResourceListMap.hpp"
#include "areg/component/ProxyEvent.hpp"
#include "areg/component/ProxyAddress.hpp"

#include "areg/component/NEService.hpp"
#include "areg/component/StubAddress.hpp"


/************************************************************************
 * Dependencies
 ************************************************************************/
class IENotificationEventConsumer;
class NotificationEventData;
class ServiceResponseEvent;
class RemoteResponseEvent;
class ServiceRequestEvent;
class NotificationEvent;
class DispatcherThread;
class EventDataStream;
class IEProxyListener;
class ProxyEvent;
class ProxyBase;
class Version;

/************************************************************************
 * Global types
 ************************************************************************/
/**
 * \brief   Function type to create a Proxy object.
 * \param   roleName    The role name of servicing component to connect.
 * \param   ownerThread The instance of thread to dispatch messages.
 *                      If NULL, uses current component thread.
 **/
typedef ProxyBase* (*FuncCreateProxy)( const char* /*roleName*/, DispatcherThread * /*ownerThread*/ );

//////////////////////////////////////////////////////////////////////////
// ProxyBase class declaration
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
class AREG_API ProxyBase  : public    IEProxyEventConsumer
{
    friend class RemoteEventFactory;
//////////////////////////////////////////////////////////////////////////
// Internal classes, types and constants
//////////////////////////////////////////////////////////////////////////
private:
    //////////////////////////////////////////////////////////////////////////
    // ProxyBase::Listener class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   This is internal listener class to forward response and 
     *          update notification events to clients.
     *          It contains information of message ID, sequence number and
     *          pointer of client object. Every proxy has list of client
     *          listeners only instantiated within same thread.
     **/
    class AREG_API Listener
    {
    //////////////////////////////////////////////////////////////////////////
    // ProxyBase::Listener class, Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Default constructor.
         **/
        Listener( void );

        /**
         * \brief   Copy constructor.
         * \param   src     The source of data to copy.
         **/
        Listener( const Listener & src );

        /**
         * \brief   Creates Listener and sets message ID.
         * \param   msgId   Message ID
         **/
        Listener( unsigned int msgId );

        /**
         * \brief   Creates Listener, sets message ID and sequence number.
         * \param	msgId	Message ID.
         * \param	seqNr	Sequence Number.
         **/
        Listener( unsigned int msgId, unsigned int seqNr );


        /**
         * \brief   Creates Listener, sets message ID, sequence number and client listener pointer.
         * \param   msgId       Message ID.
         * \param   seqNr       Sequence Number
         * \param   consumer    Client listener pointer 
         **/
        Listener(unsigned int msgId, unsigned int seqNr, IENotificationEventConsumer * caller);

    //////////////////////////////////////////////////////////////////////////
    // ProxyBase::Listener class, Basic operators
    //////////////////////////////////////////////////////////////////////////
    public:

        /**
         * \brief   Copies listener data from given source.
         * \param   src     The source of listener object.
         **/
        const ProxyBase::Listener & operator = ( const ProxyBase::Listener & src );

        /**
         * \brief   Checks equality of 2 listener objects. 2 listener objects are equal if either they have identical data,
         *          or if have same message ID, but the sequence number of one of listeners is SEQUENCE_NR_ANY (0xFFFFFFFF).
         * \param   other   The listener object to compare.
         * \return  Returns true, if 2 objects are equal.
         **/
        bool operator == ( const ProxyBase::Listener & other ) const;

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
        IENotificationEventConsumer *   mListener;
    };

    //////////////////////////////////////////////////////////////////////////
    // ProxyBase::ProxyListenerList class declaration
    //////////////////////////////////////////////////////////////////////////
    /************************************************************************
     * \brief   Proxy Listener List class to save list of listener objects.
     *          Every Proxy class has list of listeners.
     ************************************************************************/
    class AREG_API ProxyListenerList   : public TEArrayList<ProxyBase::Listener, const ProxyBase::Listener &>
    {
    //////////////////////////////////////////////////////////////////////////
    // ProxyBase::ProxyListenerList class, Constructor / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Default Constructor.
         * \param   capacity    Initial size of Array List.
         **/
        ProxyListenerList( int capacity = 0 );
        /**
         * \brief   Destructor.
         **/
        ~ProxyListenerList( void );
    };

    //////////////////////////////////////////////////////////////////////////
    // ProxyBase::ProxyConnectList class declaration
    //////////////////////////////////////////////////////////////////////////
    /************************************************************************
     * \brief   Proxy Connected client List class to handle connect and 
     *          disconnect service.
     ************************************************************************/
    class AREG_API ProxyConnectList : public TEArrayList<IEProxyListener *, IEProxyListener *>
    {
    //////////////////////////////////////////////////////////////////////////
    // ProxyBase::ProxyConnectList class, Constructor / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Constructor.
         **/
        ProxyConnectList( void );
        /**
         * \brief   Destructor.
         **/
        ~ProxyConnectList( void );
    };

    //////////////////////////////////////////////////////////////////////////
    // ProxyBase::ProxyMap class declaration.
    //////////////////////////////////////////////////////////////////////////
    /************************************************************************
     * \brief   Proxy map is used to keep control of instantiated resource.
     *          As well as to lookup for already existing instance or proxy
     *          in the same thread. As a Key, it is using Proxy Address
     *          and value is instance of Proxy.
     ************************************************************************/
    /**
     * \brief   Proxy map helper functions implementation.
     **/
    class AREG_API ProxyMapImpl   : public TEHashMapImpl<const ProxyAddress &, const ProxyBase *>
    {
    public:
        /**
         * \brief   Called to calculate the 32-bit hash key value.
         * \ param  Key     The object to calculate 32-bit hash key.
         * \return  Returns 32-bit hash key value.
         **/
        inline unsigned int implHashKey( const ProxyAddress & Key ) const
        {
            return static_cast<unsigned int>(Key);
        }

        /**
         * \brief   Compares 2 keys, returns true if they are equal.
         * \param   Value1  The key of right-side object to compare.
         * \param   Value2  The key of left-side object to compare.
         * \return  Returns true if 2 keys are equal.
         **/
        inline bool implEqualKeys( const ProxyAddress & Key1, const ProxyAddress & Key2 ) const
        {
            return ( (static_cast<const ServiceAddress &>(Key1) == static_cast<const ServiceAddress &>(Key2)) && (Key1.getThread() == Key2.getThread()) );
        }
    };

    /**
     * \brief   Proxy hash map
     **/
    typedef TEHashMap<ProxyAddress, ProxyBase*, const ProxyAddress&, const ProxyBase*, ProxyMapImpl>    MapProxy;
    /**
     * \brief   Proxy resource map helper.
     **/
    typedef TEResourceMapImpl<ProxyAddress, ProxyBase>                                                  ImplProxyResource;

    /**
     * \brief   ProxyBase::MapProxyResource
     *          Proxy Resource Map declaration to keep controlling of all instantiated Proxy objects.
     * \tparam  ProxyAddress  The Key of Resource map is a Proxy address object.
     * \tparam  ProxyBase     The Values are pointers of Proxy object.
     * \tparam  ProxyMap      The type of Hash Mapping object used as container
     **/
    typedef TELockResourceMap<ProxyAddress, ProxyBase, MapProxy, ImplProxyResource>                     MapProxyResource;

    //////////////////////////////////////////////////////////////////////////
    // ProxyBase::ThreadProxyList internal class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   The list of proxies. Used to save in Map List.
     **/
    class AREG_API ThreadProxyList  : public TEArrayList<ProxyBase *, ProxyBase *>
    {
    //////////////////////////////////////////////////////////////////////////
    // Constructors / Destructor / operators.
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Constructor.
         **/
        ThreadProxyList( void );
        /**
         * \brief   Copies entries from given source.
         **/
        ThreadProxyList( const ThreadProxyList & src);

        /**
         * \brief   Destructor.
         **/
        ~ThreadProxyList( void );

        /**
         * \brief   Copies entries from given source.
         **/
        const ThreadProxyList & operator = ( const ThreadProxyList & src );
    };

    //////////////////////////////////////////////////////////////////////////
    // ProxyBase::ThreadProxyMapImpl internal class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   The helper class used in the map of lists..
     **/
    class ThreadProxyMapImpl    : public TEResourceListMapImpl<String, ProxyBase, ThreadProxyList>
    {
    public:
        /**
         * \brief	Called when all resources are removed.
         *          This function is called from RemoveAllResources() for every single
         *          resource being unregistered.
         * \param	Key	    The String as a Key of resource.
         * \param	List    The list of proxy objects.
         **/
        inline void implCleanResourceList( const String & Key, ThreadProxyList & List );

        /**
         * \brief	Called when need to add resource object to the list.
         * \param	List        The list of proxy objects.
         * \param   Resource    The proxy object to add to the list.
         **/
        inline void implAddResource( ThreadProxyList & List, ProxyBase * Resource );
        
        /**
         * \brief	Called when need to remove resource object from the list.
         * \param	List        The list of proxy objects.
         * \param   Resource    The proxy object to remove from the list.
         **/
        inline bool implRemoveResource( ThreadProxyList & List, ProxyBase * Resource );
    };

    /**
     * \brief   ProxyBase::MapThreadProxy
     *          The string hash map which values are list of proxies.
     **/
    typedef TEStringHashMap<ThreadProxyList, const ThreadProxyList>                                         MapThreadProxy;

    /**
     * \brief   ProxyBase::MapThreadProxyList
     *          The Map of the lits, where the key is a string and values are list of proxies.
     **/
    typedef TELockResourceListMap<String, ProxyBase, MapThreadProxy, ThreadProxyList, ThreadProxyMapImpl>   MapThreadProxyList;

protected:
    //////////////////////////////////////////////////////////////////////////
    // ProxyBase::ServiceAvailableEvent internal class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Proxy Service available event to notify new connected client.
     *          If a service is already connected and a new client is instantiated,
     *          the Proxy should be able immediately inform service availability.
     *          This event is used to be able to notify client within component
     *          thread context even if client was instantiated in other thread.
     **/
    class AREG_API ServiceAvailableEvent   : public ThreadEventBase
    {
    //////////////////////////////////////////////////////////////////////////
    // Runtime internals
    //////////////////////////////////////////////////////////////////////////
        DECLARE_RUNTIME_EVENT( ServiceAvailableEvent )
    //////////////////////////////////////////////////////////////////////////
    // Constructor/ Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Sets event consumer object to deliver notification.
         **/
        ServiceAvailableEvent( IENotificationEventConsumer & consumer );
        /**
         * \brief   Destructor
         **/
        virtual ~ServiceAvailableEvent( void );

    //////////////////////////////////////////////////////////////////////////
    // Attributes
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Returns instance of consumer to send notification
         **/
        IENotificationEventConsumer & getConsumer( void ) const;
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
        ServiceAvailableEvent( void );
        ServiceAvailableEvent( const ServiceAvailableEvent & /*src*/ );
        const ServiceAvailableEvent & operator = ( const ServiceAvailableEvent & /*src*/ );
    };
//////////////////////////////////////////////////////////////////////////
// ProxyBase class static methods
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
    static ProxyBase * findOrCreateProxy( const char * roleName
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
    static ProxyBase * findOrCreateProxy( const char * roleName
                                        , const NEService::SInterfaceData & serviceIfData
                                        , IEProxyListener & connect
                                        , FuncCreateProxy funcCreate
                                        , DispatcherThread & ownerThread );

    /**
     * \brief   Lookup in registries for instantiated proxy object and
     *          if found, returns valid pointer to proxy object.
     * \param   proxyAddress    The Address of Proxy object.
     * \return  Returns pointer to Proxy object.
     **/
    static ProxyBase * findProxyByAddress( const ProxyAddress & proxyAddress );

    /**
     * \brief   Searches all created proxies in the specified thread. On output, the 
     *          parameter 'threadProxyList' contains list of proxies created in the
     *          thread 'ownerThread'.
     * \param   ownerThread     The thread, which proxies should be returned.
     * \param   threadProxyList On output, which contains list of proxies created in specified thread.
     * \return  Returns number of proxies added to the list.
     **/
    static int findThreadProxies( DispatcherThread & ownerThread, TEArrayList<ProxyBase *, ProxyBase *> & OUT threadProxyList );

    /**
     * \brief   Creates the request failure event to send to remote proxy. This may happen when either the request of client
     *          was not delivered to the target, or when could not find the appropriate request call to process on Stub.
     * \param   target  The address of target proxy to send the message.
     * \param   msgId   The failed message ID, should be request method.
     * \param   errCode The error code to set in message.
     * \param   seqNr   The sequence number generated by system.
     * \return  Returns valid pointer to the object if operation succeeded.
     **/
    static RemoteResponseEvent * createRequestFailureEvent(const ProxyAddress & target, unsigned int msgId, NEService::eResultType errCode, unsigned int seqNr);

//////////////////////////////////////////////////////////////////////////
// ProxyBase class, Constructor / Destructor.
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
    ProxyBase( const char* roleName, const NEService::SInterfaceData & serviceIfData, DispatcherThread * ownerThread = static_cast<DispatcherThread *>(NULL) );

    /**
     * \brief   Destructor.
     **/
    virtual ~ProxyBase( void );

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
    void freeProxy( IEProxyListener & connect );

    /**
     * \brief   Function is called when thread completes job and makes cleanups.
     *          This call notifies all related to proxy clients that it has disconnected
     *          from service, as well as removes all listeners and frees the resources.
     **/
    void stopProxy( void );

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
    virtual void processResponseEvent(ServiceResponseEvent & eventElem) = 0;

    /**
     * \brief   Method derived from IEProxyEventConsumer interface.
     *          Triggered when on server side a certain Attribute 
     *          value has been updated.
     * \param   eventElem   The Service Response event object.
     *                      Contains new updated value of Attribute
     *                      and validation flag.
     **/
    virtual void processAttributeEvent(ServiceResponseEvent & eventElem) = 0;

/************************************************************************/
// ProxyBase overrides. Should be implemented.
/************************************************************************/

    /**
     * \brief   Overwrite this method to create service available event to new instantiated clients.
     * \param   consumer    The instance of consumer, which receives service available event.
     * \return  If succeeds, returns valid pointer to service available event object.
     **/
    virtual ProxyBase::ServiceAvailableEvent * createServiceAvailableEvent( IENotificationEventConsumer & consumer ) = 0;

    /**
     * \brief   Creates notification event to send to client objects. 
     *          All Notification Events should be internal events and 
     *          should be instances of NotificationEvent class.
     *
     *          Overwrite this method.
     *
     * \param   data    The Notification Event data object containing
     *                  notification information.
     * \return  Returns new created notification event object.
     **/
    virtual NotificationEvent * createNotificationEvent( const NotificationEventData & data ) const = 0;

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
    virtual ServiceRequestEvent * createRequestEvent( const EventDataStream & args, unsigned int reqId ) = 0;

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
    virtual ServiceRequestEvent * createNotificationRequestEvent( unsigned int msgId, NEService::eRequestType reqType ) = 0;

    /**
     * \brief   Overwrite method to create response event from streaming object for 
     *          further dispatching by proxy.
     * \param   stream  Streaming object, which contains event data.
     * \return  If operation succeeds, returns valid pointer to Service Response event object.
     *          Otherwise, it returns NULL.
     **/
    virtual RemoteResponseEvent * createRemoteResponseEvent( const IEInStream & stream ) const;

    /**
     * \brief   Overwrite method to create error remote response event.
     *          Function is triggered when remote request sent by proxy failed and did not reach target.
     * \param   addrProxy   Address of Proxy object, which sent request event
     * \param   msgId       Message ID, which was requested to process
     * \param   reason      Failure reason set by system
     * \param   seqNr       The sequence number of processing message.
     **/
    virtual RemoteResponseEvent * createRemoteRequestFailedEvent( const ProxyAddress & addrProxy, unsigned int msgId, NEService::eResultType reason, unsigned int seqNr ) const;

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
    virtual void processProxyEvent( ProxyEvent & eventElem );

    /**
     * \brief   Triggered, when current dispatching event is not an instance of
     *          Proxy Event and should be processed by Proxy object.
     * \param   eventElem   Event object to process.
     **/
    virtual void processGenericEvent( Event & eventElem );

    /**
     * \brief   Triggered, when received server connection status changed.
     * \param   server      The address of connected service stub server.
     * \param   channel     Communication channel object to deliver events.
     * \param   status      The service connection status. 
     *                      The connection status should be NEService::ServiceConnected
     *                      To be able to send message to service target from Proxy client.
     **/
    virtual void serviceConnectionUpdated( const StubAddress & server, const Channel & channel, NEService::eServiceConnection status );

    /**
     * \brief   Triggered when service available event is processed.
     *          The system checks validity of consumer and trigger appropriate
     *          service available method with appropriated availability flag.
     **/
    virtual void processServiceAvailableEvent( IENotificationEventConsumer & consumer );

/************************************************************************/
// ProxyBase interface overrides
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
     * \remark  Use of ProxyBase::unregisterListener()
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
    virtual void unregisterListener( IENotificationEventConsumer * consumer );

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
    virtual void sendNotificationEvent( unsigned int msgId, NEService::eResultType resType, unsigned int seqNr, IENotificationEventConsumer * caller );

    /**
     * \brief   Called to register all servicing listeners. It is called when proxy is instantiated.
     *          Overwrite method to add service event listeners.
     **/
    virtual void registerServiceListeners( void );

    /**
     * \brief   Called to unregister all servicing listeners. It is called when proxy is freed.
     *          Overwrite method to remove service event listeners.
     **/
    virtual void unregisterServiceListeners( void );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns the address of Proxy.
     **/
    inline const ProxyAddress & getProxyAddress( void ) const;

    /**
     * \brief   Returns the address of target Stub object.
     **/
    inline const StubAddress & getStubAddress( void ) const;

    /**
     * \brief   Returns true if Proxy have got server connected notification.
     **/
    inline bool isConnected( void ) const;

    /**
     * \brief   Checks whether there are more listener objects
     *          assigned for specified message ID.
     *          The sequence number will be ignored.
     * \param   msgId   The message ID to check.
     * \return  Returns true if there are more listeners assigned.
     *          for specified message ID.
     **/
    inline bool hasAnyListener( unsigned int msgId ) const;

    /**
     * \brief   Checks whether there are more listener objects
     *          assigned for specified attribute update message ID.
     *          Only sequence number equal to zero, i.e. only
     *          attribute update notifications.
     * \param   msgId       The message ID to check.
     * \return  Returns true if there are more listeners assigned.
     *          for specified message ID.
     **/
    inline bool hasNotificationListener( unsigned int msgId ) const;

    /**
     * \brief   Returns the Proxy dispatcher thread.
     **/
    inline DispatcherThread & getProxyDispatcherThread( void ) const;

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
    inline void startNotification( unsigned int msgId );

    /**
     * \brief   Sends Request Event to Stub, to stop specific notification.
     *          Can be ether Attribute update or response message ID.
     * \param   msgId   The message ID to stop notification.
     **/
    inline void stopNotification( unsigned int msgId );

    /**
     * \brief   Sends remove all notification event message to Stub and
     *          stops all notifications. 
     **/
    inline void stopAllServiceNotifications( void );

    /**
     * \brief   Stops list of specified notifications
     * \param   notifyIds   The list of notification IDs to stop.
     * \param   count       The size of notification ID list
     **/
    inline void stopNotifications( const unsigned int notifyIds[], int count );

    /**
     * \brief   Returns reference of read-only Proxy Data object
     **/
    inline const NEService::ProxyData & getProxyData( void ) const;

    /**
     * \brief   Returns reference of Proxy Data object
     **/
    inline NEService::ProxyData & getProxyData( void );

    /**
     * \brief   Register Proxy object for certain event type.
     * \param   eventClass  Runtime Class ID of Event
     **/
    void registerForEvent( const RuntimeClassID & eventClass );

    /**
     * \brief   Unregister Proxy object out of certain event type.
     * \param   eventClass  Runtime Class ID of Event
     **/
    void unregisterForEvent( const RuntimeClassID & eventClass );

    /**
     * \brief   Remove Proxy Listener entry from listener list.
     * \param   msgId       The message ID of listener to remove.
     * \param   seqNr       The sequence number of listener to remove.
     * \param   consumer    Notification Event consumer.
     **/
    inline void removeListener( unsigned int msgId, unsigned int seqNr, IENotificationEventConsumer * caller );

    /**
     * \brief   Add Proxy Listener entry to listener list.
     * \param   msgId       Message ID of listener
     * \param   seqNr       Sequence number of listener
     * \param   consumer    Pointer to Notification Event consumer object.
     * \return  Returns true if new listener has been added.
     *          If listener already exists, returns false.
     **/
    bool addListener( unsigned int msgId, unsigned int seqNr, IENotificationEventConsumer * caller );

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
    void setNotification( unsigned int msgId, IENotificationEventConsumer * caller, bool alwaysNotify = false );

    /**
     * \brief   Clears listener entries of specified Notification Event consumer
     * \param   msgId       The Notification Message ID
     * \param   consumer    The pointer of Notification Event Consumer.
     **/
    void clearNotification( unsigned int msgId, IENotificationEventConsumer * caller );

    /**
     * \brief   Sets Data state of specified message ID in Proxy Data object
     * \param   msgId       Message ID, which data state should be set
     * \param   newState    The state to set.
     **/
    void setState( unsigned int msgId, NEService::eDataStateType newState );

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
    void notifyListeners( unsigned int respId, NEService::eResultType result, unsigned int seqNrToSearch );

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
    int prepareListeners( ProxyBase::ProxyListenerList & out_listenerList, unsigned int msgId, unsigned int seqNrToSearch );

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
    void sendRequestEvent( unsigned int reqId, const EventDataStream & args, IENotificationEventConsumer * caller );

    /**
     * \brief   Sends request events to Stub object to start or stop receiving update notifications.
     * \param   msgId       The message ID to start or stop receiving updates. It should be either attribute ID or response (info). 
     * \param   reqType     The type of request. Should be either request to 
     *                      call function or to get attribute update notification.
     *                      See details in NEService::eRequestType
     **/
    void sendNotificationRequestEvent( unsigned int msgId, NEService::eRequestType reqType );

    /**
     * \brief   Returns true if specified consumer is registered in the listener list.
     **/
    bool isServiceListenerRegistered( IENotificationEventConsumer & caller ) const;

    /**
     * \brief   Called to instantiate service available event to send to client.
     *          Override the method to instantiate appropriate event object.
     * \param   eventInstance   The instance of event to send.
     **/
    void sendServiceAvailableEvent( ProxyBase::ServiceAvailableEvent * eventInstance );

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
    unsigned int            mSequenceCount;

    /**
     * \brief   The list of notification listeners.
     **/
    ProxyListenerList       mListenerList;

    /**
     * \brief   The list of connected clients of the proxy.
     **/
    ProxyConnectList        mListConnect;

    /**
     * \brief   Flag, indicating whether Proxy is connected to server
     *          component or not. If it is not connected, no event
     *          message will be sent to Stub.
     **/
    bool                    mIsConnected;

    /**
     * \brief   Flag, indicating whether the proxy is stopper or not.
     *          Stopped proxy is inactive and cannot neither receive, nor respond on message.
     *          The stopped proxy should be recreated again. This flag for internal use.
     **/
    bool                    mIsStopped;

    /**
     * \brief   Proxy data, containing service interface information
     *          attribute and parameter update state.
     **/
    NEService::ProxyData    mProxyData;

    /**
     * \brief   The Proxy dispatcher thread object
     **/
    DispatcherThread &      mDispatcherThread;

    /**
     * \brief   Proxy instance reference counter. 
     *          On every request to start Proxy, this counter will 
     *          increase value. On request to free Proxy, it will
     *          decrease value. And when reaches zero, will delete
     *          Proxy object.
     **/
    unsigned int            mProxyInstCount;

private:
#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

    /**
     * \brief   Resource of registered Proxies in the system.
     **/
    static MapProxyResource     _mapRegisteredProxies;
    /**
     * \brief   The list of proxies per thread.
     **/
    static MapThreadProxyList   _mapThreadProxies;

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(default: 4251)
#endif  // _MSC_VER

private:
    /**
     * \brief   Return reference to Proxy object
     **/
    inline ProxyBase & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ProxyBase( void );
    ProxyBase( const ProxyBase & /*src*/ );
    const ProxyBase & operator = ( const ProxyBase & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// ProxyBase::ThreadProxyMapImpl class template inline function implementation
//////////////////////////////////////////////////////////////////////////

inline void ProxyBase::ThreadProxyMapImpl::implCleanResourceList(const String & /*Key*/, ThreadProxyList & /*List*/)
{
}

inline void ProxyBase::ThreadProxyMapImpl::implAddResource(ThreadProxyList & List, ProxyBase * Resource)
{
    if (Resource != NULL)
    {
        List.addUnique(Resource);
    }
}

inline bool ProxyBase::ThreadProxyMapImpl::implRemoveResource(ThreadProxyList & List, ProxyBase * Resource)
{
    return (Resource != NULL ? List.remove(Resource, 0) : false);
}

//////////////////////////////////////////////////////////////////////////
// ProxyBase class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline ProxyBase & ProxyBase::self( void )
{
    return (*this);
}

inline const ProxyAddress& ProxyBase::getProxyAddress( void ) const
{
    return mProxyAddress;
}

inline const StubAddress& ProxyBase::getStubAddress( void ) const
{
    return mStubAddress;
}

inline bool ProxyBase::isConnected( void ) const
{
    return mIsConnected;
}

inline bool ProxyBase::hasAnyListener(unsigned int msgId) const
{
    return mListenerList.exist(ProxyBase::Listener(msgId, NEService::SEQUENCE_NUMBER_ANY));
}

inline bool ProxyBase::hasNotificationListener(unsigned int msgId) const
{
    return mListenerList.exist(ProxyBase::Listener(msgId, NEService::SEQUENCE_NUMBER_NOTIFY));
}

inline void ProxyBase::removeListener( unsigned int msgId, unsigned int seqNr, IENotificationEventConsumer* caller )
{
    static_cast<void>(mListenerList.remove(ProxyBase::Listener(msgId, seqNr, caller)));
}

inline void ProxyBase::startNotification( unsigned int msgId )
{
    if (isConnected()) 
        sendNotificationRequestEvent(msgId, NEService::REQUEST_START_NOTIFY);
}

inline void ProxyBase::stopNotification( unsigned int msgId )
{
    if (isConnected()) 
        sendNotificationRequestEvent(msgId, NEService::REQUEST_STOP_NOTIFY);
}

inline void ProxyBase::stopAllServiceNotifications( void )
{
    if (isConnected()) 
        sendNotificationRequestEvent(NEService::EMPTY_FUNCTION_ID, NEService::REQUEST_REMOVE_ALL_NOTIFY);
}

inline void ProxyBase::stopNotifications( const unsigned int notifyIds[], int count )
{
    for ( int i = 0; i < count; ++ i ) 
        stopNotification(notifyIds[i]);
}

inline const NEService::ProxyData & ProxyBase::getProxyData( void ) const
{
    return mProxyData;
}

inline NEService::ProxyData & ProxyBase::getProxyData( void )
{
    return mProxyData;
}

inline DispatcherThread & ProxyBase::getProxyDispatcherThread( void ) const
{
    return mDispatcherThread;
}

#endif  // AREG_COMPONENT_PROXYBASE_HPP
