//////////////////////////////////////////////////////////////////////////
// Begin generate generated/DirectMessagerStub.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_DIRECTMESSAGERSTUB_HPP
#define  GENERATED_DIRECTMESSAGERSTUB_HPP

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:10:05 GMT+01:00
 *                  Create by AREG SDK code generator tool from source DirectMessager.
 *
 * \file            generated/DirectMessagerStub.hpp
 * \ingroup         DirectMessager Service Interface
 * \brief           This is an automatic generated code of DirectMessager
 *                  Service Interface Stub class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "generated/NEDirectMessager.hpp"
#include "areg/component/StubBase.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// DirectMessagerStub class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       DirectMessager Service Interface Stub object.
 *              Instantiated per every server component, which differ by
 *              role name of component.
 *
 *              The service interface of direct connection and messaging used in chat-room.
 **/
class DirectMessagerStub   : protected  StubBase
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initializes Stub by given component object, which should be already instantiated.
     * \param   masterComp  The master component object, which is initializing service Stub.
     * \note    Before constructor is called, the instance of Component must be already initialized.
     **/
    explicit DirectMessagerStub( Component & masterComp );
    /**
     * \brief   Destructor.
     **/
    virtual ~DirectMessagerStub( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Unlocks and cancels all requests, excepts those which were manually unlocked.
     *          This call will send cancel error message (NEService::eResultType::RequestCanceled)
     *          to all clients waiting for response.
     **/
    void unlockAllRequests( void );

//////////////////////////////////////////////////////////////////////////
// Start Service Interface operations / attributes and overrides declaration
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// DirectMessager Interface Attributes
//////////////////////////////////////////////////////////////////////////
 public:
/************************************************************************
 * Attribute ChatParticipants functions
 ************************************************************************/

    /**
     * \brief   Returns true if ChatParticipants attribute is valid
     **/
    inline bool isChatParticipantsValid( void ) const;
    /**
     * \brief   Invalidates ChatParticipants attribute and
     *          sends error update notification message to clients.
     **/
    inline void invalidateChatParticipants( void );
    /**
     * \brief   Force to send ChatParticipants attribute notification message to all clients.
     *          The method can be called when the attribute is updated not via set-method.
     **/
     inline void notifyChatParticipantsUpdated( void );
    /**
     * \brief   Set ChatParticipants attribute value and send notification event to all clients.
     *          The notification is sent only when value is updated or validated.
     * \param   newValue    New value of ChatParticipants attribute to set.
     *                      The ChatParticipants attribute description:
     *                      The list of char-room participants
     **/
    virtual void setChatParticipants( const NEDirectMessager::ListParticipants & newValue );
    /**
     * \brief   Returns the value of ChatParticipants attribute.
     *          The ChatParticipants attribute description:
     *          The list of char-room participants
     **/
    inline const NEDirectMessager::ListParticipants & getChatParticipants( void ) const;
    inline NEDirectMessager::ListParticipants & getChatParticipants( void );

//////////////////////////////////////////////////////////////////////////
// DirectMessager Interface Requests
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Request call.
     *          Request to join chat. The participant should be in the list of connections
     * \param   participant The participant to join chat. The participant should be in the connection list.
     * \param   timeConnect The time-stamp when the request was sent.
     * \see     responseChatJoin
     **/
    virtual void requestChatJoin( const NEDirectMessager::sParticipant & participant, const DateTime & timeConnect ) = 0;

    /**
     * \brief   Request call.
     *          Request to send a message, has no response
     * \param   sender      The participant structure of message sender
     * \param   msgText     The text message to send
     * \param   timeSent    The time-stamp when the message is requested to send.
     * \note    Has no response
     **/
    virtual void requestMessageSend( const NEDirectMessager::sParticipant & sender, const String & msgText, const DateTime & timeSent ) = 0;

    /**
     * \brief   Request call.
     *          The request to send message while typing a text. Has no response.
     * \param   participant The structure of participant while sending message during typing.
     * \param   msgText     The text message while typing.
     * \note    Has no response
     **/
    virtual void requestMessageType( const NEDirectMessager::sParticipant & participant, const String & msgText ) = 0;

    /**
     * \brief   Request call.
     *          The request to leave chat-room. Has no response.
     * \param   participant The structure of chat-room participant.
     * \param   timeLeave   Time-stamp when it was requested to leave chat-room.
     * \note    Has no response
     **/
    virtual void requestChatLeave( const NEDirectMessager::sParticipant & participant, const DateTime & timeLeave ) = 0;

//////////////////////////////////////////////////////////////////////////
// DirectMessager Interface Responses
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Response call.
     *          Response to join chat
     * \param   succeed         Flag, indicating whether connection is accepted or not.
     * \param   listParticipant The list of participants.
     * \param   timeConnect     Time-stamp when it was requested to join chat
     * \param   timeConnected   Time-stamp when the request to join was accepted and new participants was registered.
     * \see     requestChatJoin
     **/
    virtual void responseChatJoin( bool succeed, const NEDirectMessager::ListParticipants & listParticipant, const DateTime & timeConnect, const DateTime & timeConnected );

//////////////////////////////////////////////////////////////////////////
// DirectMessager Interface Broadcasts
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Broadcast call.
     *          Informs all connected servicing clients that the message is sent.
     * \param   sender      The structure of participant, who sent the message.
     * \param   msgText     The message, which was sent.
     * \param   timeSent    The time-stamp when the message was sent.
     **/
    virtual void broadcastMessageSent( const NEDirectMessager::sParticipant & sender, const String & msgText, const DateTime & timeSent );

    /**
     * \brief   Broadcast call.
     *          Informs all connected servicing client that the text was typed.
     * \param   participant The structure of participant, who initiated message during typing.
     * \param   msgText     The text message while typing.
     **/
    virtual void broadcastMessageTyped( const NEDirectMessager::sParticipant & participant, const String & msgText );

    /**
     * \brief   Broadcast call.
     *          Informs new participant joined chat-room
     * \param   participant The structure of participant, joined chat-room.
     * \param   timeJoined  Time-stamp when participant joined the chat-room
     **/
    virtual void broadcastParticipantJoined( const NEDirectMessager::sParticipant & participant, const DateTime & timeJoined );

    /**
     * \brief   Broadcast call.
     *          Informs that a participant left chat-room.
     * \param   participant The structure of participant, who left the chat-room.
     * \param   timeLeft    The time-stamp when the participant left chat-room.
     **/
    virtual void broadcastParticipantLeft( const NEDirectMessager::sParticipant & participant, const DateTime & timeLeft );

    /**
     * \brief   Broadcast call.
     *          Informs all service connected clients that the chat-room is closed. Message will be impossible.
     **/
    virtual void broadcastChatClosed( void );

//////////////////////////////////////////////////////////////////////////
// End Service Interface operations / attributes and overrides declaration
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//  Operations.
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// StubBase overrides. Public methods.
/************************************************************************/

    /**
     * \brief   Sends update notification message to all clients.
     *          This method can be called manually to send update
     *          notification message after updating attribute value.
     *
     * \param   msgId   The attribute message ID to notify clients.
     **/
    virtual void sendNotification( unsigned int msgId ) override;

    /**
     * \brief   Sends error message to clients.
     *          If message ID is a request, it should send result NEService::eResultType::RequestError
     *          or NEService::eResultType::RequestCanceled, depending on msgCancel flag.
     *          If message ID is a response, it should send result NEService::eResultType::RequestInvalid.
     *          If message ID is an attribute, it should send result NEService::eResultType::DataInvalid
     *          and invalidate attribute data value.
     *
     * \param   msgId       The message ID to send error message
     * \param   msgCancel   Indicates whether the request is canceled or should respond with error.
     *                      This parameter has sense only for request IDs.
     *                      It is ignored for response and attributes IDs.
     **/
    virtual void errorRequest( unsigned int msgId, bool msgCancel ) override;

protected:
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
    virtual void startupServiceInterface( Component & holder ) override;

    /**
     * \brief   This function is triggered by Component when shuts down.
     *          Overwrite this method to remove listeners and stub cleanup
     * \param   holder  The holder component of service interface of Stub,
     *                  which shuts down.
     **/
    virtual void shutdownServiceIntrface ( Component & holder ) override;

/************************************************************************/
// StubBase overrides. Protected methods
/************************************************************************/

    /**
     * \brief   Creates Response event object to pass of client.
     * \param   proxy   The address of proxy object to send response event
     * \param   msgId   The message ID to send to client
     * \param   result  The result of message
     * \param   data    The buffer of data to send to client. Can be Invalid buffer.
     * \return  Returns valid pointer to Response event object
     **/
    virtual ResponseEvent * createResponseEvent( const ProxyAddress & proxy, unsigned int msgId, NEService::eResultType result, const EventDataStream & data ) const override;

    /**
     * \brief   Overwrite method to create remote service request event from streaming object for
     *          further dispatching by stub.
     * \param   stream  Streaming object, which contains event data.
     * \return  If operation succeeds, returns valid pointer to Service Request event object.
     *          Otherwise, it returns nullptr.
     **/
    virtual RemoteRequestEvent * createRemoteRequestEvent( const IEInStream & stream ) const override;

    /**
     * \brief   Overwrite method to create remote notify request event from streaming object for
     *          further dispatching by stub.
     * \param   stream  Streaming object, which contains event data.
     * \return  If operation succeeds, returns valid pointer to Service Request event object.
     *          Otherwise, it returns nullptr.
     **/
    virtual RemoteNotifyRequestEvent * createRemoteNotifyRequestEvent( const IEInStream & stream ) const override;

private:
/************************************************************************/
// IEComponentConsumer interface overrides.
/************************************************************************/

    /**
     * \brief   Triggered to process service request event.
     *          Overwrite method to process every service request event.
     * \param   eventElem   Service Request Event object, contains request
     *                      call ID and parameters.
     **/
    virtual void processRequestEvent( ServiceRequestEvent & eventElem ) override;

    /**
     * \brief   Triggered to process attribute update notification event.
     *          Override method to process request to get attribute value and
     *          process notification request of attribute update.
     * \param   eventElem   Service Request Event object, contains attribute ID.
     **/
    virtual void processAttributeEvent( ServiceRequestEvent & eventElem ) override;

//////////////////////////////////////////////////////////////////////////
// Hidden function calls
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns reference to DirectMessagerStub object
     **/
    inline DirectMessagerStub & self( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************
 * Attributes
 ************************************************************************/

    /**
     * \brief   Attribute ChatParticipants
     *          The list of char-room participants
     **/
    NEDirectMessager::ListParticipants  mChatParticipants;
    /**
     * \brief   Attribute ChatParticipants data validation state.
     **/
    NEService::eDataStateType           mChatParticipantsState;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DirectMessagerStub( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( DirectMessagerStub );
};

//////////////////////////////////////////////////////////////////////////
// DirectMessagerStub class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline DirectMessagerStub & DirectMessagerStub::self( void )
{
    return (*this);
}

/************************************************************************
 * \brief   ChatParticipants attribute functions
 ************************************************************************/
 
inline bool DirectMessagerStub::isChatParticipantsValid( void ) const
{
    return (mChatParticipantsState  == NEService::eDataStateType::DataIsOK);
}

inline void DirectMessagerStub::invalidateChatParticipants( void )
{
    errorRequest( static_cast<msg_id>(NEDirectMessager::eMessageIDs::MsgId_ChatParticipants), false );
}

inline void DirectMessagerStub::notifyChatParticipantsUpdated( void )
{
    sendNotification( static_cast<msg_id>(NEDirectMessager::eMessageIDs::MsgId_ChatParticipants) );
}

inline const NEDirectMessager::ListParticipants & DirectMessagerStub::getChatParticipants( void ) const
{
    return mChatParticipants;
}

inline NEDirectMessager::ListParticipants & DirectMessagerStub::getChatParticipants( void )
{
    return mChatParticipants;
}

//////////////////////////////////////////////////////////////////////////
// DirectMessagerStub class declaration End
//////////////////////////////////////////////////////////////////////////

#endif   // GENERATED_DIRECTMESSAGERSTUB_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/DirectMessagerStub.hpp file
//////////////////////////////////////////////////////////////////////////
