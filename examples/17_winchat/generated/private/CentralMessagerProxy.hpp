//////////////////////////////////////////////////////////////////////////
// Begin generate generated/private/CentralMessagerProxy.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_PRIVATE_CENTRALMESSAGERPROXY_HPP
#define  GENERATED_PRIVATE_CENTRALMESSAGERPROXY_HPP

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:10:01 GMT+01:00
 *                  Create by AREG SDK code generator tool from source CentralMessager.
 *
 * \file            generated/private/CentralMessagerProxy.hpp
 * \ingroup         CentralMessager Service Interface
 * \brief           This is an automatic generated code of CentralMessager
 *                  Service Interface Proxy class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "generated/NECentralMessager.hpp"
#include "areg/component/ProxyBase.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class CentralMessagerResponseEvent;

//////////////////////////////////////////////////////////////////////////
// CentralMessagerProxy class declaration Begin
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       CentralMessager Service Interface proxy class.
 *              Automatically instantiated per thread at client startup.
 *              Used to communicate with stub object and sending notifications
 *              to each client running in the thread instance.
 *
 *              This service is used to receive messages from connected edge when typing or 
 *              sending messages. The service receives text message from initiator and
 *              broadcast to all clients, which have subscribed on event.
 *              It as well broadcasts own text message to all connected clients.
 **/
class CentralMessagerProxy   : public ProxyBase
{
    friend class CentralMessagerClientBase;
    
private:
    //////////////////////////////////////////////////////////////////////////
    // CentralMessagerProxy::CentralMessagerServiceAvailableEvent class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   The event is used immediately to notify new client about service
     *          availability and escape duplicating notification for other,
     *          already connected clients.
     **/
    class CentralMessagerServiceAvailableEvent : public ProxyBase::ServiceAvailableEvent
    {
    //////////////////////////////////////////////////////////////////////////
    // Runtime internals
    //////////////////////////////////////////////////////////////////////////
        DECLARE_RUNTIME_EVENT(CentralMessagerProxy::CentralMessagerServiceAvailableEvent)

    //////////////////////////////////////////////////////////////////////////
    // Constructor/ Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Sets event consumer object to deliver notification.
         **/
        explicit CentralMessagerServiceAvailableEvent( IENotificationEventConsumer & consumer );
        /**
         * \brief   Destructor.
         **/
        virtual ~CentralMessagerServiceAvailableEvent( void ) = default;
    //////////////////////////////////////////////////////////////////////////
    // Forbidden calls
    //////////////////////////////////////////////////////////////////////////
    private:
        CentralMessagerServiceAvailableEvent( void ) = delete;
        DECLARE_NOCOPY_NOMOVE( CentralMessagerServiceAvailableEvent );
    };
//////////////////////////////////////////////////////////////////////////
// CentralMessagerProxy predefined constants and static members.
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
    static CentralMessagerProxy * createProxy( const String & roleName, IEProxyListener & connectListener, DispatcherThread & ownerThread );

    /**
     * \brief   Returns existing or creates new proxy object if it is not existing
     *          in the current thread scope.
     * \param   roleName        The role name of stub component object.
     * \param   connectListener The reference to connection listener
     * \param   ownerThread     The name of thread instance to dispatch messages.
     *                          If nullptr, current dispatching thread is used to dispatch messages.
     * \return  Returns pointer to instantiated proxy object.
     **/
    static CentralMessagerProxy * createProxy( const String & roleName, IEProxyListener & connectListener, const String & ownerThread = String::getEmptyString() );

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
    explicit CentralMessagerProxy( const String & roleName, DispatcherThread * ownerThread = nullptr );
    /**
     * \brief   Destructor
     **/
    virtual ~CentralMessagerProxy( void );

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
     * \brief   Returns value of nickName of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter nickName description:
     *          The nick name of initiator
     **/
    inline const String & getParamnickName( void ) const;

    /**
     * \brief   Returns value of cookie of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter cookie description:
     *          The cookie assigned to initiator
     **/
    inline unsigned int getParamcookie( void ) const;

    /**
     * \brief   Returns value of newMessage of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter newMessage description:
     *          The message sent by initiator
     **/
    inline const String & getParamnewMessage( void ) const;

    /**
     * \brief   Returns value of dateTime of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter dateTime description:
     *          The local time-stamp of initiator
     **/
    inline const DateTime & getParamdateTime( void ) const;

    /**
     * \brief   Returns value of serverMessage of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter serverMessage description:
     *          The message sent by servicing server
     **/
    inline const String & getParamserverMessage( void ) const;

/************************************************************************
 * Requests
 ************************************************************************/
    /**
     * \brief   Request call.
     *          Request to sends message that all clients can see.
     * \param   nickName    The nick name of initiator
     * \param   cookie      The cookie given by connection manager. Should not be invalid.
     * \param   newMessage  The message sent by connected initiator
     * \param   dateTime    The time-stamp create on local host of initiator
     * \see     Has no Response.
     **/
    void requestSendMessage( const String & nickName, unsigned int cookie, const String & newMessage, const DateTime & dateTime );

    /**
     * \brief   Request call.
     *          Sent each time when typing a key
     * \param   nickName    The nick name of initiator
     * \param   cookie      The cookie assigned to initiator
     * \param   newMessage  The message typed.
     * \see     Has no Response.
     **/
    void requestKeyTyping( const String & nickName, unsigned int cookie, const String & newMessage );

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
    inline void setNotification( NECentralMessager::eMessageIDs notifyId, IENotificationEventConsumer & listener, bool notifyAlways = false );
    /**
     * \brief   Clears listener entries of specified Notification Event consumer
     * \param   msgId       The Notification Message ID
     * \param   consumer    The pointer of Notification Event Consumer.
     **/
    inline void clearNotification( NECentralMessager::eMessageIDs notifyId, IENotificationEventConsumer & listener );
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
     * \brief   nickName parameter value.
     *          The nick name of initiator
     **/
    String          mParamnickName;

    /**
     * \brief   cookie parameter value.
     *          The cookie assigned to initiator
     **/
    unsigned int    mParamcookie;

    /**
     * \brief   newMessage parameter value.
     *          The message sent by initiator
     **/
    String          mParamnewMessage;

    /**
     * \brief   dateTime parameter value.
     *          The local time-stamp of initiator
     **/
    DateTime        mParamdateTime;

    /**
     * \brief   serverMessage parameter value.
     *          The message sent by servicing server
     **/
    String          mParamserverMessage;

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
    void updateData( CentralMessagerResponseEvent & eventElem, NECentralMessager::eMessageIDs respId );
    /**
     * \brief   Starts processing response message. The received event contains
     *          ID of appropriate message to update and contains result flag.
     *          If request calls failed (busy), the ID in event is a request call ID.
     *          In all other cases, the ID is either response call, info call,
     *          or attribute update notification call.
     *          It will also update parameters and states.
     * \param   evenElem    The response object containing message ID, result and data.
     **/
    void processResponse( CentralMessagerResponseEvent & evenElem );

    /**
     * \brief   returns reference CentralMessagerProxy object
     **/
    inline CentralMessagerProxy & self( void );

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
    CentralMessagerProxy( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( CentralMessagerProxy );
};

//////////////////////////////////////////////////////////////////////////
// CentralMessagerProxy class declaration End
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CentralMessagerProxy class inline functions implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * Inline operations
 ************************************************************************/

inline void CentralMessagerProxy::setNotification( NECentralMessager::eMessageIDs notifyId, IENotificationEventConsumer & listener, bool notifyAlways /* = false */ )
{
    ProxyBase::setNotification(static_cast<unsigned int>(notifyId), &listener, notifyAlways);
}

inline void CentralMessagerProxy::clearNotification( NECentralMessager::eMessageIDs notifyId, IENotificationEventConsumer & listener )
{
    ProxyBase::clearNotification(static_cast<unsigned int>(notifyId), &listener);
}

inline void CentralMessagerProxy::clearAllNotifications( IENotificationEventConsumer & listener )
{
    ProxyBase::unregisterListener( &listener );
}

inline CentralMessagerProxy & CentralMessagerProxy::self( void )
{
    return (*this);
}

/************************************************************************
 * Inline parameters
 ************************************************************************/

inline const String & CentralMessagerProxy::getParamnickName( void ) const
{
    return mParamnickName;
}

inline unsigned int CentralMessagerProxy::getParamcookie( void ) const
{
    return mParamcookie;
}

inline const String & CentralMessagerProxy::getParamnewMessage( void ) const
{
    return mParamnewMessage;
}

inline const DateTime & CentralMessagerProxy::getParamdateTime( void ) const
{
    return mParamdateTime;
}

inline const String & CentralMessagerProxy::getParamserverMessage( void ) const
{
    return mParamserverMessage;
}

#endif   // GENERATED_PRIVATE_CENTRALMESSAGERPROXY_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/private/CentralMessagerProxy.hpp file
//////////////////////////////////////////////////////////////////////////
