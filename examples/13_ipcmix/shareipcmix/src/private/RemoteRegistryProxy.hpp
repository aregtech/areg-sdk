//////////////////////////////////////////////////////////////////////////
// Begin generate shareipcmix/src/private/RemoteRegistryProxy.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  SHAREIPCMIX_SRC_PRIVATE_REMOTEREGISTRYPROXY_HPP
#define  SHAREIPCMIX_SRC_PRIVATE_REMOTEREGISTRYPROXY_HPP
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source RemoteRegistry.
 * Generated at     11.06.2021  21:11:05 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shareipcmix/src/private/RemoteRegistryProxy.hpp
 * \ingroup         RemoteRegistry Service Interface
 * \brief           This is an automatic generated code of RemoteRegistry Service Interface Proxy class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "shareipcmix/src/NERemoteRegistry.hpp"
#include "areg/component/ProxyBase.hpp"
 
/************************************************************************
 * Dependencies
 ************************************************************************/
class RemoteRegistryResponseEvent;
 
//////////////////////////////////////////////////////////////////////////
// RemoteRegistryProxy class declaration Begin
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       RemoteRegistry Service Interface proxy class.
 *              Automatically instantiated per thread at client startup.
 *              Used to communicate with stub object and sending notifications
 *              to each client running in the thread instance.
 *
 *              Simple Service Interface to demonstrate working features of AREG SDK.
 *              This interface demonstrates how to use multiple remote services in the system.
 *              Each service is unique in the network.
 *              This service is used by each remote servicing component to register clients.
 *              Before using services, the clients should be registered.
 **/
class RemoteRegistryProxy   : public ProxyBase
{

private:
    //////////////////////////////////////////////////////////////////////////
    // RemoteRegistryProxy::RemoteRegistryServiceAvailableEvent class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief       The event is used to immediately notify new client
     *              about service availability and escape duplicating 
     *              notification for other, already connected clients.
     *  
     **/
    class RemoteRegistryServiceAvailableEvent : public ProxyBase::ServiceAvailableEvent
    {
    //////////////////////////////////////////////////////////////////////////
    // Runtime internals
    //////////////////////////////////////////////////////////////////////////
        DECLARE_RUNTIME_EVENT(RemoteRegistryProxy::RemoteRegistryServiceAvailableEvent)

    //////////////////////////////////////////////////////////////////////////
    // Constructor/ Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Sets event consumer object to deliver notification.
         **/
        RemoteRegistryServiceAvailableEvent( IENotificationEventConsumer & consumer );
        /**
         * \brief   Destructor
         **/
        virtual ~RemoteRegistryServiceAvailableEvent( void );
    //////////////////////////////////////////////////////////////////////////
    // Forbidden calls
    //////////////////////////////////////////////////////////////////////////
    private:
        RemoteRegistryServiceAvailableEvent( void );
        RemoteRegistryServiceAvailableEvent( const RemoteRegistryServiceAvailableEvent & /*src*/ );
        const RemoteRegistryServiceAvailableEvent & operator = ( const RemoteRegistryServiceAvailableEvent & /*src*/ );
    };
//////////////////////////////////////////////////////////////////////////
// RemoteRegistryProxy predefined constants and static members.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns existing or creates new proxy object if it is not existing
     *          in the current thread scope.
     * \param   roleName        The role name of stub component object
     * \param   connectListener The reference to connection listener
     * \param   ownerThread     The instance of thread to dispatch messages.
     * \return  Returns pointer to instantiated proxy object.
     **/
    static RemoteRegistryProxy * createProxy( const char * roleName, IEProxyListener & connectListener, DispatcherThread & ownerThread );

    /**
     * \brief   Returns existing or creates new proxy object if it is not existing
     *          in the current thread scope.
     * \param   roleName        The role name of stub component object
     * \param   connectListener The reference to connection listener
     * \param   ownerThread     The name of thread instance to dispatch messages.
     *                          If NULL, current dispatching thread is used to dispatch messages.
     * \return  Returns pointer to instantiated proxy object.
     **/
    static RemoteRegistryProxy * createProxy( const char * roleName, IEProxyListener & connectListener, const char * ownerThread = static_cast<const char *>(NULL) );

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Constructor. Creates Proxy object by given role name. 
     *          Do not create proxy object directly, instead use CreateProxy function.
     * \param   roleName    The role name of proxy, i.e. role name of server component object.
     * \param   ownerThread The instance of thread to dispatch messages.
     *                      If NULL, current dispatching thread is used to dispatch messages.
     **/
    RemoteRegistryProxy( const char * roleName, DispatcherThread * ownerThread = static_cast<DispatcherThread *>(NULL) );
    /**
     * \brief   Destructor
     **/
    virtual ~RemoteRegistryProxy( void );

//////////////////////////////////////////////////////////////////////////
// Attributes and operations.
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************
 * Attributes
 ************************************************************************/
    /**
     * \brief   Returns true if value of RegistryList attribute is valid.
     *          If Update Notification is disabled, this method will return false.
     **/
    inline bool isRegistryListValid( void ) const;
    /**
     * \brief   Returns the value of RegistryList attribute.
     *          To get valid value, the Update Notification should be enabled. 
     *          Attribute RegistryList description: 
     *          List of registered clients.
     * \param   state   On returns, contains the validation flag of attribute data. 
     *                  Check validation flag before use attribute value.
     **/
    inline const NERemoteRegistry::ListRegistry & getRegistryList( NEService::eDataStateType & state ) const;

    /**
     * \brief   Returns true if value of RemainOutputs attribute is valid.
     *          If Update Notification is disabled, this method will return false.
     **/
    inline bool isRemainOutputsValid( void ) const;
    /**
     * \brief   Returns the value of RemainOutputs attribute.
     *          To get valid value, the Update Notification should be enabled. 
     *          Attribute RemainOutputs description: 
     *          Remaining outputs before reaching maximum.
     * \param   state   On returns, contains the validation flag of attribute data. 
     *                  Check validation flag before use attribute value.
     **/
    inline unsigned int getRemainOutputs( NEService::eDataStateType & state ) const;

/************************************************************************
 * Parameters
 ************************************************************************/
    /**
     * \brief   Returns value of client of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter client description: 
     *          The client registration object that contains unique ID.
     **/
    inline const NERemoteRegistry::sClientRegister & getParamclient( void ) const;

    /**
     * \brief   Returns value of clientID of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter clientID description: 
     *          Indicates the ID client that made message output
     **/
    inline unsigned int getParamclientID( void ) const;

/************************************************************************
 * Requests
 ************************************************************************/
    /**
     * \brief   Request call.
     *          Call to register client. Each client should be registered before starting communication.
     * \param   caller  The reference of caller object to get response.
     * \param   name    The name of the client.
     * \param   service The service address of the client.
     * \param   thread  The thread name where client is running. Required to provide uniqueness.
     * \param   process The name of process. Optional parameter, used to make output in logs.
     * \return  The sequence count number of call
     * \see     responseRegister
     **/
    unsigned int requestRegister( IENotificationEventConsumer & caller, const String & name, const ServiceAddress & service, const String & thread, const String & process );
    
    /**
     * \brief   Request call.
     *          Sent to unregister connected client.
     * \param   client  The client registration object indicating the unregistered client.
     * \see     Has no Response.
     **/
    void requestUnregister( const NERemoteRegistry::sClientRegister & client );

    /**
     * \brief   Request call.
     *          Outputs message on console. If additional message is not empty, outputs the additional message as well.
     * \param   caller      The reference of caller object to get response.
     * \param   clientID    The ID of registered client to make message output
     * \param   addMessage  The additional message to output. Ignored if empty.
     * \return  The sequence count number of call
     * \see     responseHelloWorld
     **/
    unsigned int requestHelloWorld( IENotificationEventConsumer & caller, unsigned int clientID, const String & addMessage );
    
//////////////////////////////////////////////////////////////////////////
// Operations.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Checks whether there is already listener of Notification Event
     *          exists. If does not exit, adds new listener entry in the listener
     *          list. If need, send appropriate message to Stub to start 
     *          sending attribute update messages. If already listener
     *          exists in listener list and notifyAlways flag is true, 
     *          sends immediate update notification based on existing update data status.
     * \param   msgId           The Notification Message ID
     * \param   consumer        The pointer of Notification Event consumer
     * \param   notifyAlways    The flag, indicating whether the notification event
     *                          should be send to client immediately if the notification
     *                          already exists. By default it is false. 
     **/
    inline void setNotification( NERemoteRegistry::eMessageIDs notifyId, IENotificationEventConsumer & listener, bool notifyAlways = false );
    /**
     * \brief   Clears listener entries of specified Notification Event consumer
     * \param   msgId       The Notification Message ID
     * \param   consumer    The pointer of Notification Event Consumer.
     * \return  
     **/
    inline void clearNotification( NERemoteRegistry::eMessageIDs notifyId, IENotificationEventConsumer & listener );
    /**
     * \brief   Clears all notification for specified listener and unregisters it
     * \param   listener    The notification consumer object, which should be unregistered.
     **/
    inline void clearAllNotifications( IENotificationEventConsumer & listener );

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// IEProxyEventConsumer interface overrides.
/************************************************************************/
    /**
     * \brief   Method derived from IEProxyEventConsumer interface.
     *          Triggered when on the request to execute function
     *          on server side, Proxy have got response message.
     * \param   eventElem   The Service Response event object.
     *                      Contains response message and information
     *                      sent by Stub
     **/
    virtual void processResponseEvent( ServiceResponseEvent & eventElem );
    /**
     * \brief   Method derived from IEProxyEventConsumer interface.
     *          Triggered when on server side a certain Attribute 
     *          value has been updated.
     * \param   eventElem   The Service Response event object.
     *                      Contains new updated value of Attribute
     *                      and validation flag.
     **/
    virtual void processAttributeEvent( ServiceResponseEvent & eventElem );

/************************************************************************/
// ProxyBase interface overrides.
/************************************************************************/

    /**
     * \brief   Creates notification event to send to client objects. 
     *          All Notification Events should be internal events and 
     *          should be instances of NotificationEvent class.
     * \param   data    The Notification Event data object containing
     *                  notification information.
     * \return  Returns new created notification event object.
     **/
    virtual NotificationEvent * createNotificationEvent( const NotificationEventData & data ) const;

    /**
     * \brief   Create Request event to send to Stub object. 
     *          Request events are triggering request calls on Stub side.
     * \param   args    The buffer containing serialized arguments of request call.
     * \param   reqId   The ID of request call.
     * \return  Return pointer of valid Request event.
     **/
    virtual ServiceRequestEvent * createRequestEvent( const EventDataStream & args, unsigned int reqId );

    /**
     * \brief   Creates event requesting to receive update notification events.
     *          The caller should be address of current proxy and the target 
     *          should address of appropriate sub address.
     * \param   msgId       The message ID to send. Should be either attribute or response (info)
     * \param   reqType     The type of request.
     * \return  Returns valid pointer of created service request event object.
     **/
    virtual ServiceRequestEvent * createNotificationRequestEvent( unsigned int msgId, NEService::eRequestType reqType );

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

    /**
     * \brief   Overwrite this method to create service available event to new instantiated clients.
     * \param   consumer    The instance of consumer, which receives service available event.
     * \return  If succeeds, returns valid pointer to service available event object.
     **/
    virtual ProxyBase::ServiceAvailableEvent * createServiceAvailableEvent( IENotificationEventConsumer & consumer );

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
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************
 * Attribute members
 ************************************************************************/
    /**
     * \brief   RegistryList attribute value.
     *          List of registered clients.
     **/
    NERemoteRegistry::ListRegistry      mRegistryList;

    /**
     * \brief   RemainOutputs attribute value.
     *          Remaining outputs before reaching maximum.
     **/
    unsigned int                        mRemainOutputs;

/************************************************************************
 * Parameter members
 ************************************************************************/
    /**
     * \brief   client parameter value.
     *          The client registration object that contains unique ID.
     **/
    NERemoteRegistry::sClientRegister   mParamclient;

    /**
     * \brief   clientID parameter value.
     *          Indicates the ID client that made message output
     **/
    unsigned int                        mParamclientID;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************
 * Hidden operations
 ************************************************************************/
    /**
     * \brief   Updates attribute and response call variable data received from Stub
     * \param   eventElem   The event object containing buffer with serialized data
     * \param   respId      The response message ID, which received. 
     *                      It is either attribute or response message ID
     **/
    void updateData( RemoteRegistryResponseEvent & eventElem, NERemoteRegistry::eMessageIDs respId );
    /**
     * \brief   Starts processing response message. The received event contains
     *          ID of appropriate message to update and contains result flag.
     *          If request calls failed (busy), the ID in event is a request call ID.
     *          In all other cases, the ID is either response call, info call, 
     *          or attribute update notification call. 
     *          It will also update parameters and states.
     * \param   evenElem    The response object containing message ID, result and data.
     **/
    void processResponse( RemoteRegistryResponseEvent & evenElem );

    /**
     * \brief   returns reference RemoteRegistryProxy object
     **/
    inline RemoteRegistryProxy & self( void );

/************************************************************************
 * Hidden static operations
 ************************************************************************/
private:
    /**
     * \brief   Create proxy object.
     * \param   roleName    The server component role name
     * \param   ownerThread The instance of thread to dispatch messages.
     *                      If NULL, current dispatching thread is used to dispatch messages.
     * \return  Pointer to instantiated proxy object.
     **/
    static ProxyBase * _createProxy( const char * roleName, DispatcherThread * ownerThread = static_cast<DispatcherThread *>(NULL) );
};

//////////////////////////////////////////////////////////////////////////
// RemoteRegistryProxy class declaration End
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// RemoteRegistryProxy class inline functions implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * Inline operations
 ************************************************************************/

inline void RemoteRegistryProxy::setNotification( NERemoteRegistry::eMessageIDs notifyId, IENotificationEventConsumer & listener, bool notifyAlways /* = false */ )
{
    ProxyBase::setNotification(static_cast<unsigned int>(notifyId), &listener, notifyAlways);
}

inline void RemoteRegistryProxy::clearNotification( NERemoteRegistry::eMessageIDs notifyId, IENotificationEventConsumer & listener )
{
    ProxyBase::clearNotification(static_cast<unsigned int>(notifyId), &listener);
}

inline void RemoteRegistryProxy::clearAllNotifications( IENotificationEventConsumer & listener )
{
    ProxyBase::unregisterListener( &listener );
}

inline RemoteRegistryProxy & RemoteRegistryProxy::self( void )
{
    return (*this);
}

/************************************************************************
 * Inline attributes
 ************************************************************************/

inline bool RemoteRegistryProxy::isRegistryListValid( void ) const
{
    return (getProxyData().getAttributeState(NERemoteRegistry::MSG_ID_RegistryList) == NEService::DATA_OK);
}

inline const NERemoteRegistry::ListRegistry & RemoteRegistryProxy::getRegistryList( NEService::eDataStateType & state ) const
{
    state = getProxyData().getAttributeState(NERemoteRegistry::MSG_ID_RegistryList);
    return mRegistryList;
}

inline bool RemoteRegistryProxy::isRemainOutputsValid( void ) const
{
    return (getProxyData().getAttributeState(NERemoteRegistry::MSG_ID_RemainOutputs) == NEService::DATA_OK);
}

inline unsigned int RemoteRegistryProxy::getRemainOutputs( NEService::eDataStateType & state ) const
{
    state = getProxyData().getAttributeState(NERemoteRegistry::MSG_ID_RemainOutputs);
    return mRemainOutputs;
}

/************************************************************************
 * Inline parameters
 ************************************************************************/

inline const NERemoteRegistry::sClientRegister & RemoteRegistryProxy::getParamclient( void ) const
{
    return mParamclient;
}

inline unsigned int RemoteRegistryProxy::getParamclientID( void ) const
{
    return mParamclientID;
}

#endif   // SHAREIPCMIX_SRC_PRIVATE_REMOTEREGISTRYPROXY_HPP

//////////////////////////////////////////////////////////////////////////
// End generate shareipcmix/src/private/RemoteRegistryProxy.hpp file
//////////////////////////////////////////////////////////////////////////
