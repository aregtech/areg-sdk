//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/HelloWorldProxy.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_PRIVATE_HELLOWORLDPROXY_HPP
#define  GENERATED_SRC_PRIVATE_HELLOWORLDPROXY_HPP

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:08:39 GMT+01:00
 *                  Create by AREG SDK code generator tool from source HelloWorld.
 *
 * \file            generated/src/private/HelloWorldProxy.hpp
 * \ingroup         HelloWorld Service Interface
 * \brief           This is an automatic generated code of HelloWorld
 *                  Service Interface Proxy class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "generated/src/NEHelloWorld.hpp"
#include "areg/component/ProxyBase.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class HelloWorldResponseEvent;

//////////////////////////////////////////////////////////////////////////
// HelloWorldProxy class declaration Begin
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       HelloWorld Service Interface proxy class.
 *              Automatically instantiated per thread at client startup.
 *              Used to communicate with stub object and sending notifications
 *              to each client running in the thread instance.
 *
 *              Simple Service Interface to demonstrate working features of AREG SDK.
 **/
class HelloWorldProxy   : public ProxyBase
{
    friend class HelloWorldClientBase;
    
private:
    //////////////////////////////////////////////////////////////////////////
    // HelloWorldProxy::HelloWorldServiceAvailableEvent class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   The event is used immediately to notify new client about service
     *          availability and escape duplicating notification for other,
     *          already connected clients.
     **/
    class HelloWorldServiceAvailableEvent : public ProxyBase::ServiceAvailableEvent
    {
    //////////////////////////////////////////////////////////////////////////
    // Runtime internals
    //////////////////////////////////////////////////////////////////////////
        DECLARE_RUNTIME_EVENT(HelloWorldProxy::HelloWorldServiceAvailableEvent)

    //////////////////////////////////////////////////////////////////////////
    // Constructor/ Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Sets event consumer object to deliver notification.
         **/
        explicit HelloWorldServiceAvailableEvent( IENotificationEventConsumer & consumer );
        /**
         * \brief   Destructor.
         **/
        virtual ~HelloWorldServiceAvailableEvent( void ) = default;
    //////////////////////////////////////////////////////////////////////////
    // Forbidden calls
    //////////////////////////////////////////////////////////////////////////
    private:
        HelloWorldServiceAvailableEvent( void ) = delete;
        DECLARE_NOCOPY_NOMOVE( HelloWorldServiceAvailableEvent );
    };
//////////////////////////////////////////////////////////////////////////
// HelloWorldProxy predefined constants and static members.
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Returns existing or creates new proxy object if it is not existing
     *          in the current thread scope.
     * \param   roleName        The role name of stub component object.
     * \param   connectListener The reference to connection listener
     * \param   ownerThread     The instance of thread to dispatch messages.
     * \return  Returns pointer to instantiated proxy object.
     **/
    static HelloWorldProxy * createProxy( const String & roleName, IEProxyListener & connectListener, DispatcherThread & ownerThread );

    /**
     * \brief   Returns existing or creates new proxy object if it is not existing
     *          in the current thread scope.
     * \param   roleName        The role name of stub component object.
     * \param   connectListener The reference to connection listener
     * \param   ownerThread     The name of thread instance to dispatch messages.
     *                          If nullptr, current dispatching thread is used to dispatch messages.
     * \return  Returns pointer to instantiated proxy object.
     **/
    static HelloWorldProxy * createProxy( const String & roleName, IEProxyListener & connectListener, const String & ownerThread = String::getEmptyString() );

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Creates Proxy object by given role name.
     *          Do not create proxy object directly, instead use CreateProxy function.
     * \param   roleName    The role name of proxy, i.e. role name of server component object.
     * \param   ownerThread The instance of thread to dispatch messages.
     *                      If nullptr, current dispatching thread is used to dispatch messages.
     **/
    explicit HelloWorldProxy( const String & roleName, DispatcherThread * ownerThread = nullptr );
    /**
     * \brief   Destructor
     **/
    virtual ~HelloWorldProxy( void );

//////////////////////////////////////////////////////////////////////////
// Attributes and operations.
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************
 * Attributes
 ************************************************************************/
/************************************************************************
 * Parameters
 ************************************************************************/
    /**
     * \brief   Returns value of clientInfo of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter clientInfo description:
     *          The client information set by servicing component. If empty or invalid ID, the message output failed.
     **/
    inline const NEHelloWorld::sConnectedClient & getParamclientInfo( void ) const;

    /**
     * \brief   Returns value of maxNumber of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter maxNumber description:
     *          The maximum number of reqeusts.
     **/
    inline int getParammaxNumber( void ) const;

/************************************************************************
 * Requests
 ************************************************************************/
    /**
     * \brief   Request call.
     *          Request to print hello world
     * \param   caller      The reference of caller object to get response.
     * \param   roleName    The role name of client component that requested to print hello world
     * \return  The sequence count number of call
     * \see     responseHelloWorld
     **/
    unsigned int requestHelloWorld( IENotificationEventConsumer & caller, const String & roleName );

    /**
     * \brief   Request call.
     *          Sent by client to request to shutdown service and the application. This request has no response.
     * \param   clientID    The ID of client that requests to shutdown. The ID is given by service when first time client requests to output message.
     * \param   roleName    Service client component role name
     * \see     Has no Response.
     **/
    void requestShutdownService( unsigned int clientID, const String & roleName );

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
    inline void setNotification( NEHelloWorld::eMessageIDs notifyId, IENotificationEventConsumer & listener, bool notifyAlways = false );
    /**
     * \brief   Clears listener entries of specified Notification Event consumer
     * \param   msgId       The Notification Message ID
     * \param   consumer    The pointer of Notification Event Consumer.
     **/
    inline void clearNotification( NEHelloWorld::eMessageIDs notifyId, IENotificationEventConsumer & listener );
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
    virtual void processResponseEvent( ServiceResponseEvent & eventElem ) override;
    /**
     * \brief   Method derived from IEProxyEventConsumer interface.
     *          Triggered when on server side a certain Attribute
     *          value has been updated.
     * \param   eventElem   The Service Response event object.
     *                      Contains new updated value of Attribute
     *                      and validation flag.
     **/
    virtual void processAttributeEvent( ServiceResponseEvent & eventElem ) override;

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
    virtual NotificationEvent * createNotificationEvent( const NotificationEventData & data ) const override;

    /**
     * \brief   Create Request event to send to Stub object.
     *          Request events are triggering request calls on Stub side.
     * \param   args    The buffer containing serialized arguments of request call.
     * \param   reqId   The ID of request call.
     * \return  Return pointer of valid Request event.
     **/
    virtual ServiceRequestEvent * createRequestEvent( const EventDataStream & args, unsigned int reqId ) override;

    /**
     * \brief   Creates event requesting to receive update notification events.
     *          The caller should be address of current proxy and the target
     *          should address of appropriate sub address.
     * \param   msgId       The message ID to send. Should be either attribute or response (info)
     * \param   reqType     The type of request.
     * \return  Returns valid pointer of created service request event object.
     **/
    virtual ServiceRequestEvent * createNotificationRequestEvent( unsigned int msgId, NEService::eRequestType reqType ) override;

    /**
     * \brief   Overwrite method to create response event from streaming object for
     *          further dispatching by proxy.
     * \param   stream  Streaming object, which contains event data.
     * \return  If operation succeeds, returns valid pointer to Service Response event object.
     *          Otherwise, it returns nullptr.
     **/
    virtual RemoteResponseEvent * createRemoteResponseEvent( const IEInStream & stream ) const override;

    /**
     * \brief   Overwrite method to create error remote response event.
     *          Function is triggered when remote request sent by proxy failed and did not reach target.
     * \param   addrProxy   Address of Proxy object, which sent request event
     * \param   msgId       Message ID, which was requested to process
     * \param   reason      Failure reason set by system
     * \param   seqNr       The sequence number of processing message.
     **/
    virtual RemoteResponseEvent * createRemoteRequestFailedEvent( const ProxyAddress & addrProxy, unsigned int msgId, NEService::eResultType reason, unsigned int seqNr ) const override;

    /**
     * \brief   Overwrite this method to create service available event to new instantiated clients.
     * \param   consumer    The instance of consumer, which receives service available event.
     * \return  If succeeds, returns valid pointer to service available event object.
     **/
    virtual ProxyBase::ServiceAvailableEvent * createServiceAvailableEvent( IENotificationEventConsumer & consumer ) override;

    /**
     * \brief   Called to register all servicing listeners. It is called when proxy is instantiated.
     *          Overwrite method to add service event listeners.
     **/
    virtual void registerServiceListeners( void ) override;

    /**
     * \brief   Called to unregister all servicing listeners. It is called when proxy is freed.
     *          Overwrite method to remove service event listeners.
     **/
    virtual void unregisterServiceListeners( void ) override;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************
 * Attribute members
 ************************************************************************/
/************************************************************************
 * Parameter members
 ************************************************************************/
    /**
     * \brief   clientInfo parameter value.
     *          The client information set by servicing component. If empty or invalid ID, the message output failed.
     **/
    NEHelloWorld::sConnectedClient  mParamclientInfo;

    /**
     * \brief   maxNumber parameter value.
     *          The maximum number of reqeusts.
     **/
    int                             mParammaxNumber;

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
    void updateData( HelloWorldResponseEvent & eventElem, NEHelloWorld::eMessageIDs respId );
    /**
     * \brief   Starts processing response message. The received event contains
     *          ID of appropriate message to update and contains result flag.
     *          If request calls failed (busy), the ID in event is a request call ID.
     *          In all other cases, the ID is either response call, info call,
     *          or attribute update notification call.
     *          It will also update parameters and states.
     * \param   evenElem    The response object containing message ID, result and data.
     **/
    void processResponse( HelloWorldResponseEvent & evenElem );

    /**
     * \brief   returns reference HelloWorldProxy object
     **/
    inline HelloWorldProxy & self( void );

/************************************************************************
 * Hidden static operations
 ************************************************************************/
private:
    /**
     * \brief   Create proxy object.
     * \param   roleName    The server component role name
     * \param   ownerThread The instance of thread to dispatch messages.
     *                      If nullptr, current dispatching thread is used to dispatch messages.
     * \return  Pointer to instantiated proxy object.
     **/
    static ProxyBase * _createProxy( const String & roleName, DispatcherThread * ownerThread = nullptr );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    HelloWorldProxy( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( HelloWorldProxy );
};

//////////////////////////////////////////////////////////////////////////
// HelloWorldProxy class declaration End
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// HelloWorldProxy class inline functions implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * Inline operations
 ************************************************************************/

inline void HelloWorldProxy::setNotification( NEHelloWorld::eMessageIDs notifyId, IENotificationEventConsumer & listener, bool notifyAlways /* = false */ )
{
    ProxyBase::setNotification(static_cast<unsigned int>(notifyId), &listener, notifyAlways);
}

inline void HelloWorldProxy::clearNotification( NEHelloWorld::eMessageIDs notifyId, IENotificationEventConsumer & listener )
{
    ProxyBase::clearNotification(static_cast<unsigned int>(notifyId), &listener);
}

inline void HelloWorldProxy::clearAllNotifications( IENotificationEventConsumer & listener )
{
    ProxyBase::unregisterListener( &listener );
}

inline HelloWorldProxy & HelloWorldProxy::self( void )
{
    return (*this);
}

/************************************************************************
 * Inline parameters
 ************************************************************************/

inline const NEHelloWorld::sConnectedClient & HelloWorldProxy::getParamclientInfo( void ) const
{
    return mParamclientInfo;
}

inline int HelloWorldProxy::getParammaxNumber( void ) const
{
    return mParammaxNumber;
}

#endif   // GENERATED_SRC_PRIVATE_HELLOWORLDPROXY_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/HelloWorldProxy.hpp file
//////////////////////////////////////////////////////////////////////////
