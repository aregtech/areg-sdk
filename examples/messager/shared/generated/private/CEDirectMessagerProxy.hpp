//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/private/CEDirectMessagerProxy.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  SHARED_GENERATED_PRIVATE_CEDIRECTMESSAGERPROXY_HPP
#define  SHARED_GENERATED_PRIVATE_CEDIRECTMESSAGERPROXY_HPP
/************************************************************************
 * (c) copyright    2019
 *                  Create by AREGtech code generator tool from source DirectMessager.
 * Generated at     03.09.2019  02:48:11 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shared/generated/private/CEDirectMessagerProxy.hpp
 * \ingroup         DirectMessager Service Interface
 * \brief           This is an automatic generated code of DirectMessager Service Interface Proxy class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "shared/generated/NEDirectMessager.hpp"
#include "areg/component/CEProxyBase.hpp"
 
/************************************************************************
 * Dependencies
 ************************************************************************/
class CEDirectMessagerResponseEvent;
 
//////////////////////////////////////////////////////////////////////////
// CEDirectMessagerProxy class declaration Begin
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       DirectMessager Service Interface proxy class.
 *              Automatically instantiated per thread at client startup.
 *              Used to communicate with stub object and sending notifications
 *              to each client running in the thread instance.
 *
 *              The service interface of direct connection and messaging used in chat-room.
 **/
class CEDirectMessagerProxy   : public CEProxyBase
{

private:
    //////////////////////////////////////////////////////////////////////////
    // CEDirectMessagerProxy::CEDirectMessagerServiceAvailableEvent class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief       The event is used to immediately notify new client
     *              about service availability and escape duplicating 
     *              notification for other, already connected clients.
     *  
     **/
    class CEDirectMessagerServiceAvailableEvent : public CEProxyBase::CEServiceAvailableEvent
    {
    //////////////////////////////////////////////////////////////////////////
    // Runtime internals
    //////////////////////////////////////////////////////////////////////////
        DECLARE_RUNTIME_EVENT(CEDirectMessagerProxy::CEDirectMessagerServiceAvailableEvent);

    //////////////////////////////////////////////////////////////////////////
    // Constructor/ Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Sets event consumer object to deliver notification.
         **/
        CEDirectMessagerServiceAvailableEvent( IENotificationEventConsumer & consumer );
        /**
         * \brief   Destructor
         **/
        virtual ~CEDirectMessagerServiceAvailableEvent( void );
    //////////////////////////////////////////////////////////////////////////
    // Forbidden calls
    //////////////////////////////////////////////////////////////////////////
    private:
        CEDirectMessagerServiceAvailableEvent( void );
        CEDirectMessagerServiceAvailableEvent( const CEDirectMessagerServiceAvailableEvent & /*src*/ );
        const CEDirectMessagerServiceAvailableEvent & operator = ( const CEDirectMessagerServiceAvailableEvent & /*src*/ );
    };
//////////////////////////////////////////////////////////////////////////
// CEDirectMessagerProxy predefined constants and static members.
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
    static CEDirectMessagerProxy * CreateProxy( const char * const roleName, IEProxyListener & connectListener, CEDispatcherThread & ownerThread );

    /**
     * \brief   Returns existing or creates new proxy object if it is not existing
     *          in the current thread scope.
     * \param   roleName        The role name of stub component object
     * \param   connectListener The reference to connection listener
     * \param   ownerThread     The name of thread instance to dispatch messages.
     *                          If NULL, current dispatching thread is used to dispatch messages.
     * \return  Returns pointer to instantiated proxy object.
     **/
    static CEDirectMessagerProxy * CreateProxy( const char * const roleName, IEProxyListener & connectListener, const char * ownerThread = static_cast<const char *>(NULL) );

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
    CEDirectMessagerProxy( const char * const roleName, CEDispatcherThread * ownerThread = static_cast<CEDispatcherThread *>(NULL) );
    /**
     * \brief   Destructor
     **/
    virtual ~CEDirectMessagerProxy( void );

//////////////////////////////////////////////////////////////////////////
// Attributes and operations.
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************
 * Attributes
 ************************************************************************/
    /**
     * \brief   Returns true if value of ChatParticipants attribute is valid.
     *          If Update Notification is disabled, this method will return false.
     **/
    inline bool IsChatParticipantsValid( void ) const;
    /**
     * \brief   Returns the value of ChatParticipants attribute.
     *          To get valid value, the Update Notification should be enabled. 
     *          Attribute ChatParticipants description: 
     *          The list of char-room participants
     * \param   state   On returns, contains the validation flag of attribute data. 
     *                  Check validation flag before use attribute value.
     **/
    inline const NEDirectMessager::ListParticipants & GetChatParticipants( NEService::eDataStateType & state ) const;

/************************************************************************
 * Parameters
 ************************************************************************/
    /**
     * \brief   Returns value of succeed of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter succeed description: 
     *          Flag, indicating whether connection is accepted or not.
     **/
    inline const bool & GetParamsucceed( void ) const;

    /**
     * \brief   Returns value of listParticipant of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter listParticipant description: 
     *          The list of participants.
     **/
    inline const NEDirectMessager::ListParticipants & GetParamlistParticipant( void ) const;

    /**
     * \brief   Returns value of timeConnect of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter timeConnect description: 
     *          Timestamp when it was requested to join chat
     **/
    inline const CEDateTime & GetParamtimeConnect( void ) const;

    /**
     * \brief   Returns value of timeConnected of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter timeConnected description: 
     *          Timestamp when the request to join was accepted and new participants was registered.
     **/
    inline const CEDateTime & GetParamtimeConnected( void ) const;

    /**
     * \brief   Returns value of sender of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter sender description: 
     *          The structure of participant, who sent the message.
     **/
    inline const NEDirectMessager::sParticipant & GetParamsender( void ) const;

    /**
     * \brief   Returns value of msgText of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter msgText description: 
     *          The message, which was sent.
     **/
    inline const CEString & GetParammsgText( void ) const;

    /**
     * \brief   Returns value of timeSent of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter timeSent description: 
     *          The timestamp when the message was sent.
     **/
    inline const CEDateTime & GetParamtimeSent( void ) const;

    /**
     * \brief   Returns value of participant of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter participant description: 
     *          The structure of participant, who initiated message during typing.
     **/
    inline const NEDirectMessager::sParticipant & GetParamparticipant( void ) const;

    /**
     * \brief   Returns value of timeJoined of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter timeJoined description: 
     *          Timestamp when participant joined the chat-room
     **/
    inline const CEDateTime & GetParamtimeJoined( void ) const;

    /**
     * \brief   Returns value of timeLeft of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter timeLeft description: 
     *          The timestamp when the participant left chat-room.
     **/
    inline const CEDateTime & GetParamtimeLeft( void ) const;

/************************************************************************
 * Requests
 ************************************************************************/
    /**
     * \brief   Request call.
     *          Request to join chat. The participant should be in the list of connections
     * \param   caller      The reference of caller object to get response.
     * \param   participant The participant to join chat. The participant should be in the connection list.
     * \param   timeConnect The timestamp when the request was sent.
     * \return  The sequence count number of call
     * \see     ResponseChatJoin
     **/
    unsigned int RequestChatJoin( IENotificationEventConsumer & caller, const NEDirectMessager::sParticipant & participant, const CEDateTime & timeConnect );
    
    /**
     * \brief   Request call.
     *          Request to send a message, has no response
     * \param   sender      The participant structure of message sender
     * \param   msgText     The text message to send
     * \param   timeSent    The timestamp when the message is requested to send.
     * \see     Has no Response.
     **/
    void RequestMessageSend( const NEDirectMessager::sParticipant & sender, const CEString & msgText, const CEDateTime & timeSent );

    /**
     * \brief   Request call.
     *          The request to send message while typing a text. Has no response.
     * \param   participant The structure of participant while sending message during typing.
     * \param   msgText     The text message while typing.
     * \see     Has no Response.
     **/
    void RequestMessageType( const NEDirectMessager::sParticipant & participant, const CEString & msgText );

    /**
     * \brief   Request call.
     *          The request to leave chat-room. Has no response.
     * \param   participant The structure of chat-room participant.
     * \param   timeLeave   Timestamp when it was requested to leave chat-room.
     * \see     Has no Response.
     **/
    void RequestChatLeave( const NEDirectMessager::sParticipant & participant, const CEDateTime & timeLeave );

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
    inline void SetNotification( const NEDirectMessager::eMessageIDs notifyId, IENotificationEventConsumer & listener, bool notifyAlways = false );
    /**
     * \brief   Clears listener entries of specified Notification Event consumer
     * \param   msgId       The Notification Message ID
     * \param   consumer    The pointer of Notification Event Consumer.
     * \return  
     **/
    inline void ClearNotification( const NEDirectMessager::eMessageIDs notifyId, IENotificationEventConsumer & listener );
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
    virtual CEServiceRequestEvent * CreateRequestEvent( const CEEventDataStream & args, const unsigned int reqId );

    /**
     * \brief   Creates event requesting to receive update notification events.
     *          The caller should be address of current proxy and the target 
     *          should address of appropriate sub address.
     * \param   msgId       The message ID to send. Should be either attribute or response (info)
     * \param   reqType     The type of request.
     * \return  Returns valid pointer of created service request event object.
     **/
    virtual CEServiceRequestEvent * CreateNotificationRequestEvent( const unsigned int msgId, const NEService::eRequestType reqType );

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
    virtual CERemoteResponseEvent * CreateRemoteRequestFailedEvent( const CEProxyAddress & addrProxy, unsigned int msgId, const NEService::eResultType reason, unsigned int seqNr ) const;

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
    /**
     * \brief   ChatParticipants attribute value.
     *          The list of char-room participants
     **/
    NEDirectMessager::ListParticipants  mChatParticipants;

/************************************************************************
 * Parameter members
 ************************************************************************/
    /**
     * \brief   succeed parameter value.
     *          Flag, indicating whether connection is accepted or not.
     **/
    bool                                mParamsucceed;

    /**
     * \brief   listParticipant parameter value.
     *          The list of participants.
     **/
    NEDirectMessager::ListParticipants  mParamlistParticipant;

    /**
     * \brief   timeConnect parameter value.
     *          Timestamp when it was requested to join chat
     **/
    CEDateTime                          mParamtimeConnect;

    /**
     * \brief   timeConnected parameter value.
     *          Timestamp when the request to join was accepted and new participants was registered.
     **/
    CEDateTime                          mParamtimeConnected;

    /**
     * \brief   sender parameter value.
     *          The structure of participant, who sent the message.
     **/
    NEDirectMessager::sParticipant      mParamsender;

    /**
     * \brief   msgText parameter value.
     *          The message, which was sent.
     **/
    CEString                            mParammsgText;

    /**
     * \brief   timeSent parameter value.
     *          The timestamp when the message was sent.
     **/
    CEDateTime                          mParamtimeSent;

    /**
     * \brief   participant parameter value.
     *          The structure of participant, who initiated message during typing.
     **/
    NEDirectMessager::sParticipant      mParamparticipant;

    /**
     * \brief   timeJoined parameter value.
     *          Timestamp when participant joined the chat-room
     **/
    CEDateTime                          mParamtimeJoined;

    /**
     * \brief   timeLeft parameter value.
     *          The timestamp when the participant left chat-room.
     **/
    CEDateTime                          mParamtimeLeft;

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
    void UpdateData( CEDirectMessagerResponseEvent & eventElem, const NEDirectMessager::eMessageIDs respId );
    /**
     * \brief   Starts processing response message. The received event contains
     *          ID of appropriate message to update and contains result flag.
     *          If request calls failed (busy), the ID in event is a request call ID.
     *          In all other cases, the ID is either response call, info call, 
     *          or attribute update notification call. 
     *          It will also update parameters and states.
     * \param   evenElem    The response object containing message ID, result and data.
     **/
    void ProcessResponse( CEDirectMessagerResponseEvent & evenElem );

    /**
     * \brief   returns reference CEDirectMessagerProxy object
     **/
    inline CEDirectMessagerProxy & self( void );

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
    static CEProxyBase * _createProxy( const char * const roleName, CEDispatcherThread * ownerThread = static_cast<CEDispatcherThread *>(NULL) );
    /**
     * \brief   Creates and returns service interface data object.
     **/
    static const NEService::SInterfaceData & _createInterfaceData( void );
};

//////////////////////////////////////////////////////////////////////////
// CEDirectMessagerProxy class declaration End
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEDirectMessagerProxy class inline functions implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * Inline operations
 ************************************************************************/

inline void CEDirectMessagerProxy::SetNotification( const NEDirectMessager::eMessageIDs notifyId, IENotificationEventConsumer & listener, bool notifyAlways /* = false */ )
{   CEProxyBase::SetNotification(static_cast<unsigned int>(notifyId), &listener, notifyAlways); }

inline void CEDirectMessagerProxy::ClearNotification( const NEDirectMessager::eMessageIDs notifyId, IENotificationEventConsumer & listener )
{   CEProxyBase::ClearNotification(static_cast<unsigned int>(notifyId), &listener); }

inline void CEDirectMessagerProxy::ClearAllNotifications( IENotificationEventConsumer & listener )
{   CEProxyBase::UnregisterListener( &listener ); }

inline CEDirectMessagerProxy & CEDirectMessagerProxy::self( void )
{   return (*this); }

/************************************************************************
 * Inline attributes
 ************************************************************************/

inline bool CEDirectMessagerProxy::IsChatParticipantsValid( void ) const
{   return (GetProxyData().GetAttributeState(NEDirectMessager::MSG_ID_ChatParticipants) == NEService::DATA_OK);   }

inline const NEDirectMessager::ListParticipants & CEDirectMessagerProxy::GetChatParticipants( NEService::eDataStateType & state ) const
{   state = GetProxyData().GetAttributeState(NEDirectMessager::MSG_ID_ChatParticipants); return mChatParticipants;  }

/************************************************************************
 * Inline parameters
 ************************************************************************/

inline const bool & CEDirectMessagerProxy::GetParamsucceed( void ) const
{   return mParamsucceed;  }

inline const NEDirectMessager::ListParticipants & CEDirectMessagerProxy::GetParamlistParticipant( void ) const
{   return mParamlistParticipant;  }

inline const CEDateTime & CEDirectMessagerProxy::GetParamtimeConnect( void ) const
{   return mParamtimeConnect;  }

inline const CEDateTime & CEDirectMessagerProxy::GetParamtimeConnected( void ) const
{   return mParamtimeConnected;  }

inline const NEDirectMessager::sParticipant & CEDirectMessagerProxy::GetParamsender( void ) const
{   return mParamsender;  }

inline const CEString & CEDirectMessagerProxy::GetParammsgText( void ) const
{   return mParammsgText;  }

inline const CEDateTime & CEDirectMessagerProxy::GetParamtimeSent( void ) const
{   return mParamtimeSent;  }

inline const NEDirectMessager::sParticipant & CEDirectMessagerProxy::GetParamparticipant( void ) const
{   return mParamparticipant;  }

inline const CEDateTime & CEDirectMessagerProxy::GetParamtimeJoined( void ) const
{   return mParamtimeJoined;  }

inline const CEDateTime & CEDirectMessagerProxy::GetParamtimeLeft( void ) const
{   return mParamtimeLeft;  }

#endif   // SHARED_GENERATED_PRIVATE_CEDIRECTMESSAGERPROXY_HPP

//////////////////////////////////////////////////////////////////////////
// End generate shared/generated/private/CEDirectMessagerProxy.hpp file
//////////////////////////////////////////////////////////////////////////
