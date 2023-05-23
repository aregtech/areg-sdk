//////////////////////////////////////////////////////////////////////////
// Begin generate generated/private/DirectMessagerProxy.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_PRIVATE_DIRECTMESSAGERPROXY_HPP
#define  GENERATED_PRIVATE_DIRECTMESSAGERPROXY_HPP

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:10:05 GMT+01:00
 *                  Create by AREG SDK code generator tool from source DirectMessager.
 *
 * \file            generated/private/DirectMessagerProxy.hpp
 * \ingroup         DirectMessager Service Interface
 * \brief           This is an automatic generated code of DirectMessager
 *                  Service Interface Proxy class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "generated/NEDirectMessager.hpp"
#include "areg/component/ProxyBase.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class DirectMessagerResponseEvent;

//////////////////////////////////////////////////////////////////////////
// DirectMessagerProxy class declaration Begin
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       DirectMessager Service Interface proxy class.
 *              Automatically instantiated per thread at client startup.
 *              Used to communicate with stub object and sending notifications
 *              to each client running in the thread instance.
 *
 *              The service interface of direct connection and messaging used in chat-room.
 **/
class DirectMessagerProxy   : public ProxyBase
{
    friend class DirectMessagerClientBase;
    
private:
    //////////////////////////////////////////////////////////////////////////
    // DirectMessagerProxy::DirectMessagerServiceAvailableEvent class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   The event is used immediately to notify new client about service
     *          availability and escape duplicating notification for other,
     *          already connected clients.
     **/
    class DirectMessagerServiceAvailableEvent : public ProxyBase::ServiceAvailableEvent
    {
    //////////////////////////////////////////////////////////////////////////
    // Runtime internals
    //////////////////////////////////////////////////////////////////////////
        DECLARE_RUNTIME_EVENT(DirectMessagerProxy::DirectMessagerServiceAvailableEvent)

    //////////////////////////////////////////////////////////////////////////
    // Constructor/ Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Sets event consumer object to deliver notification.
         **/
        explicit DirectMessagerServiceAvailableEvent( IENotificationEventConsumer & consumer );
        /**
         * \brief   Destructor.
         **/
        virtual ~DirectMessagerServiceAvailableEvent( void ) = default;
    //////////////////////////////////////////////////////////////////////////
    // Forbidden calls
    //////////////////////////////////////////////////////////////////////////
    private:
        DirectMessagerServiceAvailableEvent( void ) = delete;
        DECLARE_NOCOPY_NOMOVE( DirectMessagerServiceAvailableEvent );
    };
//////////////////////////////////////////////////////////////////////////
// DirectMessagerProxy predefined constants and static members.
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
    static DirectMessagerProxy * createProxy( const String & roleName, IEProxyListener & connectListener, DispatcherThread & ownerThread );

    /**
     * \brief   Returns existing or creates new proxy object if it is not existing
     *          in the current thread scope.
     * \param   roleName        The role name of stub component object.
     * \param   connectListener The reference to connection listener
     * \param   ownerThread     The name of thread instance to dispatch messages.
     *                          If nullptr, current dispatching thread is used to dispatch messages.
     * \return  Returns pointer to instantiated proxy object.
     **/
    static DirectMessagerProxy * createProxy( const String & roleName, IEProxyListener & connectListener, const String & ownerThread = String::getEmptyString() );

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
    explicit DirectMessagerProxy( const String & roleName, DispatcherThread * ownerThread = nullptr );
    /**
     * \brief   Destructor
     **/
    virtual ~DirectMessagerProxy( void );

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
    inline bool isChatParticipantsValid( void ) const;
    /**
     * \brief   Returns the value of ChatParticipants attribute.
     *          To get valid value, the Update Notification should be enabled. 
     *          Attribute ChatParticipants description:
     *          The list of char-room participants
     * \param   state   On returns, contains the validation flag of attribute data.
     *                  Check validation flag before use attribute value.
     **/
    inline const NEDirectMessager::ListParticipants & getChatParticipants( NEService::eDataStateType & state ) const;

/************************************************************************
 * Parameters
 ************************************************************************/
    /**
     * \brief   Returns value of succeed of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter succeed description:
     *          Flag, indicating whether connection is accepted or not.
     **/
    inline bool getParamsucceed( void ) const;

    /**
     * \brief   Returns value of listParticipant of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter listParticipant description:
     *          The list of participants.
     **/
    inline const NEDirectMessager::ListParticipants & getParamlistParticipant( void ) const;

    /**
     * \brief   Returns value of timeConnect of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter timeConnect description:
     *          Time-stamp when it was requested to join chat
     **/
    inline const DateTime & getParamtimeConnect( void ) const;

    /**
     * \brief   Returns value of timeConnected of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter timeConnected description:
     *          Time-stamp when the request to join was accepted and new participants was registered.
     **/
    inline const DateTime & getParamtimeConnected( void ) const;

    /**
     * \brief   Returns value of sender of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter sender description:
     *          The structure of participant, who sent the message.
     **/
    inline const NEDirectMessager::sParticipant & getParamsender( void ) const;

    /**
     * \brief   Returns value of msgText of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter msgText description:
     *          The message, which was sent.
     **/
    inline const String & getParammsgText( void ) const;

    /**
     * \brief   Returns value of timeSent of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter timeSent description:
     *          The time-stamp when the message was sent.
     **/
    inline const DateTime & getParamtimeSent( void ) const;

    /**
     * \brief   Returns value of participant of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter participant description:
     *          The structure of participant, who initiated message during typing.
     **/
    inline const NEDirectMessager::sParticipant & getParamparticipant( void ) const;

    /**
     * \brief   Returns value of timeJoined of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter timeJoined description:
     *          Time-stamp when participant joined the chat-room
     **/
    inline const DateTime & getParamtimeJoined( void ) const;

    /**
     * \brief   Returns value of timeLeft of response call function.
     *          The parameter is validated and set when Proxy is getting response call.
     *          Parameter timeLeft description:
     *          The time-stamp when the participant left chat-room.
     **/
    inline const DateTime & getParamtimeLeft( void ) const;

/************************************************************************
 * Requests
 ************************************************************************/
    /**
     * \brief   Request call.
     *          Request to join chat. The participant should be in the list of connections
     * \param   caller      The reference of caller object to get response.
     * \param   participant The participant to join chat. The participant should be in the connection list.
     * \param   timeConnect The time-stamp when the request was sent.
     * \return  The sequence count number of call
     * \see     responseChatJoin
     **/
    unsigned int requestChatJoin( IENotificationEventConsumer & caller, const NEDirectMessager::sParticipant & participant, const DateTime & timeConnect );

    /**
     * \brief   Request call.
     *          Request to send a message, has no response
     * \param   sender      The participant structure of message sender
     * \param   msgText     The text message to send
     * \param   timeSent    The time-stamp when the message is requested to send.
     * \see     Has no Response.
     **/
    void requestMessageSend( const NEDirectMessager::sParticipant & sender, const String & msgText, const DateTime & timeSent );

    /**
     * \brief   Request call.
     *          The request to send message while typing a text. Has no response.
     * \param   participant The structure of participant while sending message during typing.
     * \param   msgText     The text message while typing.
     * \see     Has no Response.
     **/
    void requestMessageType( const NEDirectMessager::sParticipant & participant, const String & msgText );

    /**
     * \brief   Request call.
     *          The request to leave chat-room. Has no response.
     * \param   participant The structure of chat-room participant.
     * \param   timeLeave   Time-stamp when it was requested to leave chat-room.
     * \see     Has no Response.
     **/
    void requestChatLeave( const NEDirectMessager::sParticipant & participant, const DateTime & timeLeave );

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
    inline void setNotification( NEDirectMessager::eMessageIDs notifyId, IENotificationEventConsumer & listener, bool notifyAlways = false );
    /**
     * \brief   Clears listener entries of specified Notification Event consumer
     * \param   msgId       The Notification Message ID
     * \param   consumer    The pointer of Notification Event Consumer.
     **/
    inline void clearNotification( NEDirectMessager::eMessageIDs notifyId, IENotificationEventConsumer & listener );
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
     *          Time-stamp when it was requested to join chat
     **/
    DateTime                            mParamtimeConnect;

    /**
     * \brief   timeConnected parameter value.
     *          Time-stamp when the request to join was accepted and new participants was registered.
     **/
    DateTime                            mParamtimeConnected;

    /**
     * \brief   sender parameter value.
     *          The structure of participant, who sent the message.
     **/
    NEDirectMessager::sParticipant      mParamsender;

    /**
     * \brief   msgText parameter value.
     *          The message, which was sent.
     **/
    String                              mParammsgText;

    /**
     * \brief   timeSent parameter value.
     *          The time-stamp when the message was sent.
     **/
    DateTime                            mParamtimeSent;

    /**
     * \brief   participant parameter value.
     *          The structure of participant, who initiated message during typing.
     **/
    NEDirectMessager::sParticipant      mParamparticipant;

    /**
     * \brief   timeJoined parameter value.
     *          Time-stamp when participant joined the chat-room
     **/
    DateTime                            mParamtimeJoined;

    /**
     * \brief   timeLeft parameter value.
     *          The time-stamp when the participant left chat-room.
     **/
    DateTime                            mParamtimeLeft;

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
    void updateData( DirectMessagerResponseEvent & eventElem, NEDirectMessager::eMessageIDs respId );
    /**
     * \brief   Starts processing response message. The received event contains
     *          ID of appropriate message to update and contains result flag.
     *          If request calls failed (busy), the ID in event is a request call ID.
     *          In all other cases, the ID is either response call, info call,
     *          or attribute update notification call.
     *          It will also update parameters and states.
     * \param   evenElem    The response object containing message ID, result and data.
     **/
    void processResponse( DirectMessagerResponseEvent & evenElem );

    /**
     * \brief   returns reference DirectMessagerProxy object
     **/
    inline DirectMessagerProxy & self( void );

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
    DirectMessagerProxy( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( DirectMessagerProxy );
};

//////////////////////////////////////////////////////////////////////////
// DirectMessagerProxy class declaration End
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// DirectMessagerProxy class inline functions implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * Inline operations
 ************************************************************************/

inline void DirectMessagerProxy::setNotification( NEDirectMessager::eMessageIDs notifyId, IENotificationEventConsumer & listener, bool notifyAlways /* = false */ )
{
    ProxyBase::setNotification(static_cast<unsigned int>(notifyId), &listener, notifyAlways);
}

inline void DirectMessagerProxy::clearNotification( NEDirectMessager::eMessageIDs notifyId, IENotificationEventConsumer & listener )
{
    ProxyBase::clearNotification(static_cast<unsigned int>(notifyId), &listener);
}

inline void DirectMessagerProxy::clearAllNotifications( IENotificationEventConsumer & listener )
{
    ProxyBase::unregisterListener( &listener );
}

inline DirectMessagerProxy & DirectMessagerProxy::self( void )
{
    return (*this);
}

/************************************************************************
 * Inline attributes
 ************************************************************************/

inline bool DirectMessagerProxy::isChatParticipantsValid( void ) const
{
    return (getProxyData().getAttributeState( static_cast<msg_id>(NEDirectMessager::eMessageIDs::MsgId_ChatParticipants) ) == NEService::eDataStateType::DataIsOK);
}

inline const NEDirectMessager::ListParticipants & DirectMessagerProxy::getChatParticipants( NEService::eDataStateType & state ) const
{
    state = getProxyData().getAttributeState( static_cast<msg_id>(NEDirectMessager::eMessageIDs::MsgId_ChatParticipants) );
    return mChatParticipants;
}

/************************************************************************
 * Inline parameters
 ************************************************************************/

inline bool DirectMessagerProxy::getParamsucceed( void ) const
{
    return mParamsucceed;
}

inline const NEDirectMessager::ListParticipants & DirectMessagerProxy::getParamlistParticipant( void ) const
{
    return mParamlistParticipant;
}

inline const DateTime & DirectMessagerProxy::getParamtimeConnect( void ) const
{
    return mParamtimeConnect;
}

inline const DateTime & DirectMessagerProxy::getParamtimeConnected( void ) const
{
    return mParamtimeConnected;
}

inline const NEDirectMessager::sParticipant & DirectMessagerProxy::getParamsender( void ) const
{
    return mParamsender;
}

inline const String & DirectMessagerProxy::getParammsgText( void ) const
{
    return mParammsgText;
}

inline const DateTime & DirectMessagerProxy::getParamtimeSent( void ) const
{
    return mParamtimeSent;
}

inline const NEDirectMessager::sParticipant & DirectMessagerProxy::getParamparticipant( void ) const
{
    return mParamparticipant;
}

inline const DateTime & DirectMessagerProxy::getParamtimeJoined( void ) const
{
    return mParamtimeJoined;
}

inline const DateTime & DirectMessagerProxy::getParamtimeLeft( void ) const
{
    return mParamtimeLeft;
}

#endif   // GENERATED_PRIVATE_DIRECTMESSAGERPROXY_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/private/DirectMessagerProxy.hpp file
//////////////////////////////////////////////////////////////////////////
