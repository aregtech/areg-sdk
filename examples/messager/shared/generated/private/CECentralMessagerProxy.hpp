//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/private/CECentralMessagerProxy.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  SHARED_GENERATED_PRIVATE_CECENTRALMESSAGERPROXY_HPP
#define  SHARED_GENERATED_PRIVATE_CECENTRALMESSAGERPROXY_HPP
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREGtech code generator tool from source CentralMessager.
 * Generated at     25.04.2021  23:50:42 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shared/generated/private/CECentralMessagerProxy.hpp
 * \ingroup         CentralMessager Service Interface
 * \brief           This is an automatic generated code of CentralMessager Service Interface Proxy class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "shared/generated/NECentralMessager.hpp"
#include "areg/src/component/CEProxyBase.hpp"
 
/************************************************************************
 * Dependencies
 ************************************************************************/
class CECentralMessagerResponseEvent;
 
//////////////////////////////////////////////////////////////////////////
// CECentralMessagerProxy class declaration Begin
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
class CECentralMessagerProxy   : public CEProxyBase
{

private:
    //////////////////////////////////////////////////////////////////////////
    // CECentralMessagerProxy::CECentralMessagerServiceAvailableEvent class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief       The event is used to immediately notify new client
     *              about service availability and escape duplicating 
     *              notification for other, already connected clients.
     *  
     **/
    class CECentralMessagerServiceAvailableEvent : public CEProxyBase::CEServiceAvailableEvent
    {
    //////////////////////////////////////////////////////////////////////////
    // Runtime internals
    //////////////////////////////////////////////////////////////////////////
        DECLARE_RUNTIME_EVENT(CECentralMessagerProxy::CECentralMessagerServiceAvailableEvent)

    //////////////////////////////////////////////////////////////////////////
    // Constructor/ Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Sets event consumer object to deliver notification.
         **/
        CECentralMessagerServiceAvailableEvent( IENotificationEventConsumer & consumer );
        /**
         * \brief   Destructor
         **/
        virtual ~CECentralMessagerServiceAvailableEvent( void );
    //////////////////////////////////////////////////////////////////////////
    // Forbidden calls
    //////////////////////////////////////////////////////////////////////////
    private:
        CECentralMessagerServiceAvailableEvent( void );
        CECentralMessagerServiceAvailableEvent( const CECentralMessagerServiceAvailableEvent & /*src*/ );
        const CECentralMessagerServiceAvailableEvent & operator = ( const CECentralMessagerServiceAvailableEvent & /*src*/ );
    };
//////////////////////////////////////////////////////////////////////////
// CECentralMessagerProxy predefined constants and static members.
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
    static CECentralMessagerProxy * CreateProxy( const char * roleName, IEProxyListener & connectListener, CEDispatcherThread & ownerThread );

    /**
     * \brief   Returns existing or creates new proxy object if it is not existing
     *          in the current thread scope.
     * \param   roleName        The role name of stub component object
     * \param   connectListener The reference to connection listener
     * \param   ownerThread     The name of thread instance to dispatch messages.
     *                          If NULL, current dispatching thread is used to dispatch messages.
     * \return  Returns pointer to instantiated proxy object.
     **/
    static CECentralMessagerProxy * CreateProxy( const char * roleName, IEProxyListener & connectListener, const char * ownerThread = static_cast<const char *>(NULL) );

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
    CECentralMessagerProxy( const char * roleName, CEDispatcherThread * ownerThread = static_cast<CEDispatcherThread *>(NULL) );
    /**
     * \brief   Destructor
     **/
    virtual ~CECentralMessagerProxy( void );

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
    inline const CEString & GetParamnickName( void ) const;

    /**
     * \brief   Returns value of cookie of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter cookie description: 
     *          The cookie assigned to initiator
     **/
    inline const unsigned int & GetParamcookie( void ) const;

    /**
     * \brief   Returns value of newMessage of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter newMessage description: 
     *          The message sent by initiator
     **/
    inline const CEString & GetParamnewMessage( void ) const;

    /**
     * \brief   Returns value of dateTime of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter dateTime description: 
     *          The local timestamp of initiator
     **/
    inline const CEDateTime & GetParamdateTime( void ) const;

    /**
     * \brief   Returns value of serverMessage of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter serverMessage description: 
     *          The message sent by servicing server
     **/
    inline const CEString & GetParamserverMessage( void ) const;

/************************************************************************
 * Requests
 ************************************************************************/
    /**
     * \brief   Request call.
     *          Request to sends message that all clients can see.
     * \param   nickName    The nick name of initiator
     * \param   cookie      Cookie given by connection manager. Should not be invalid.
     * \param   newMessage  The message sent by connected initiator
     * \param   dateTime    The timestamp create on local host of initiator
     * \see     Has no Response.
     **/
    void RequestSendMessage( const CEString & nickName, const unsigned int & cookie, const CEString & newMessage, const CEDateTime & dateTime );

    /**
     * \brief   Request call.
     *          Sent each time when typing a key
     * \param   nickName    The nick name of initiator
     * \param   cookie      The cookie assigned to initiator
     * \param   newMessage  The message typed.
     * \see     Has no Response.
     **/
    void RequestKeyTyping( const CEString & nickName, const unsigned int & cookie, const CEString & newMessage );

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
    inline void SetNotification( NECentralMessager::eMessageIDs notifyId, IENotificationEventConsumer & listener, bool notifyAlways = false );
    /**
     * \brief   Clears listener entries of specified Notification Event consumer
     * \param   msgId       The Notification Message ID
     * \param   consumer    The pointer of Notification Event Consumer.
     * \return  
     **/
    inline void ClearNotification( NECentralMessager::eMessageIDs notifyId, IENotificationEventConsumer & listener );
    /**
     * \brief   Clears all notification for specified listener and unregisters it
     * \param   listener    The notification consumer object, which should be unregistered.
     **/
    inline void ClearAllNotifications( IENotificationEventConsumer & listener );

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
    virtual void ProcessResponseEvent( CEServiceResponseEvent & eventElem );
    /**
     * \brief   Method derived from IEProxyEventConsumer interface.
     *          Triggered when on server side a certain Attribute 
     *          value has been updated.
     * \param   eventElem   The Service Response event object.
     *                      Contains new updated value of Attribute
     *                      and validation flag.
     **/
    virtual void ProcessAttributeEvent( CEServiceResponseEvent & eventElem );

/************************************************************************/
// CEProxyBase interface overrides.
/************************************************************************/

    /**
     * \brief   Creates notification event to send to client objects. 
     *          All Notification Events should be internal events and 
     *          should be instances of CENotificationEvent class.
     * \param   data    The Notification Event data object containing
     *                  notification information.
     * \return  Returns new created notification event object.
     **/
    virtual CENotificationEvent * CreateNotificationEvent( const CENotificationEventData & data ) const;

    /**
     * \brief   Create Request event to send to Stub object. 
     *          Request events are triggering request calls on Stub side.
     * \param   args    The buffer containing serialized arguments of request call.
     * \param   reqId   The ID of request call.
     * \return  Return pointer of valid Request event.
     **/
    virtual CEServiceRequestEvent * CreateRequestEvent( const CEEventDataStream & args, unsigned int reqId );

    /**
     * \brief   Creates event requesting to receive update notification events.
     *          The caller should be address of current proxy and the target 
     *          should address of appropriate sub address.
     * \param   msgId       The message ID to send. Should be either attribute or response (info)
     * \param   reqType     The type of request.
     * \return  Returns valid pointer of created service request event object.
     **/
    virtual CEServiceRequestEvent * CreateNotificationRequestEvent( unsigned int msgId, NEService::eRequestType reqType );

    /**
     * \brief   Overwrite method to create response event from streaming object for 
     *          further dispatching by proxy.
     * \param   stream  Streaming object, which contains event data.
     * \return  If operation succeeds, returns valid pointer to Service Response event object.
     *          Otherwise, it returns NULL.
     **/
    virtual CERemoteResponseEvent * CreateRemoteResponseEvent( const IEInStream & stream ) const;

    /**
     * \brief   Overwrite method to create error remote response event.
     *          Function is triggered when remote request sent by proxy failed and did not reach target.
     * \param   addrProxy   Address of Proxy object, which sent request event
     * \param   msgId       Message ID, which was requested to process
     * \param   reason      Failure reason set by system
     * \param   seqNr       The sequence number of processing message.
     **/
    virtual CERemoteResponseEvent * CreateRemoteRequestFailedEvent( const CEProxyAddress & addrProxy, unsigned int msgId, NEService::eResultType reason, unsigned int seqNr ) const;

    /**
     * \brief   Overwrite this method to create service available event to new instantiated clients.
     * \param   consumer    The instance of consumer, which receives service available event.
     * \return  If succeeds, returns valid pointer to service available event object.
     **/
    virtual CEProxyBase::CEServiceAvailableEvent * CreateServiceAvailableEvent( IENotificationEventConsumer & consumer );

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
    CEString        mParamnickName;

    /**
     * \brief   cookie parameter value.
     *          The cookie assigned to initiator
     **/
    unsigned int    mParamcookie;

    /**
     * \brief   newMessage parameter value.
     *          The message sent by initiator
     **/
    CEString        mParamnewMessage;

    /**
     * \brief   dateTime parameter value.
     *          The local timestamp of initiator
     **/
    CEDateTime      mParamdateTime;

    /**
     * \brief   serverMessage parameter value.
     *          The message sent by servicing server
     **/
    CEString        mParamserverMessage;

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
    void UpdateData( CECentralMessagerResponseEvent & eventElem, NECentralMessager::eMessageIDs respId );
    /**
     * \brief   Starts processing response message. The received event contains
     *          ID of appropriate message to update and contains result flag.
     *          If request calls failed (busy), the ID in event is a request call ID.
     *          In all other cases, the ID is either response call, info call, 
     *          or attribute update notification call. 
     *          It will also update parameters and states.
     * \param   evenElem    The response object containing message ID, result and data.
     **/
    void ProcessResponse( CECentralMessagerResponseEvent & evenElem );

    /**
     * \brief   returns reference CECentralMessagerProxy object
     **/
    inline CECentralMessagerProxy & self( void );

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
    static CEProxyBase * _createProxy( const char * roleName, CEDispatcherThread * ownerThread = static_cast<CEDispatcherThread *>(NULL) );
    /**
     * \brief   Creates and returns service interface data object.
     **/
    static const NEService::SInterfaceData & _createInterfaceData( void );
};

//////////////////////////////////////////////////////////////////////////
// CECentralMessagerProxy class declaration End
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CECentralMessagerProxy class inline functions implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * Inline operations
 ************************************************************************/

inline void CECentralMessagerProxy::SetNotification( NECentralMessager::eMessageIDs notifyId, IENotificationEventConsumer & listener, bool notifyAlways /* = false */ )
{   CEProxyBase::SetNotification(static_cast<unsigned int>(notifyId), &listener, notifyAlways); }

inline void CECentralMessagerProxy::ClearNotification( NECentralMessager::eMessageIDs notifyId, IENotificationEventConsumer & listener )
{   CEProxyBase::ClearNotification(static_cast<unsigned int>(notifyId), &listener); }

inline void CECentralMessagerProxy::ClearAllNotifications( IENotificationEventConsumer & listener )
{   CEProxyBase::UnregisterListener( &listener ); }

inline CECentralMessagerProxy & CECentralMessagerProxy::self( void )
{   return (*this); }

/************************************************************************
 * Inline parameters
 ************************************************************************/

inline const CEString & CECentralMessagerProxy::GetParamnickName( void ) const
{   return mParamnickName;  }

inline const unsigned int & CECentralMessagerProxy::GetParamcookie( void ) const
{   return mParamcookie;  }

inline const CEString & CECentralMessagerProxy::GetParamnewMessage( void ) const
{   return mParamnewMessage;  }

inline const CEDateTime & CECentralMessagerProxy::GetParamdateTime( void ) const
{   return mParamdateTime;  }

inline const CEString & CECentralMessagerProxy::GetParamserverMessage( void ) const
{   return mParamserverMessage;  }

#endif   // SHARED_GENERATED_PRIVATE_CECENTRALMESSAGERPROXY_HPP

//////////////////////////////////////////////////////////////////////////
// End generate shared/generated/private/CECentralMessagerProxy.hpp file
//////////////////////////////////////////////////////////////////////////
