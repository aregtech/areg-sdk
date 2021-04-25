//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/CEDirectMessagerStub.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  SHARED_GENERATED_CEDIRECTMESSAGERSTUB_HPP
#define  SHARED_GENERATED_CEDIRECTMESSAGERSTUB_HPP
/************************************************************************
 * (c) copyright    2019
 *                  Create by AREGtech code generator tool from source DirectMessager.
 * Generated at     03.09.2019  02:48:11 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shared/generated/CEDirectMessagerStub.hpp
 * \ingroup         DirectMessager Service Interface
 * \brief           This is an automatic generated code of DirectMessager Service Interface Stub class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "shared/generated/NEDirectMessager.hpp"
#include "areg/component/CEStubBase.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// CEDirectMessagerStub class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       DirectMessager Service Interface Stub object.
 *              Instantiated per every server component, which differ by
 *              role name of component.
 *
 *              The service interface of direct connection and messaging used in chat-room.
 **/
class CEDirectMessagerStub   : protected  CEStubBase
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initializes Stub by given component object, which should be already instantiated.
     * \param   masterComp  The master component object, which is initializing service Stub.
     * \note    Before constructor is called, the instance of CEComponent must be already initialized.
     **/
    CEDirectMessagerStub( CEComponent & masterComp );
    /**
     * \brief   Destructor.
     **/
    virtual ~CEDirectMessagerStub( void );
    
//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Unlocks and cancels all requests, excepts those which were manually unlocked.
     *          This call will send cancel error message (NEService::RESULT_REQUEST_CANCELED) to all clients waiting for response.
     **/
    void UnlockAllRequests( void );

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
    inline bool IsChatParticipantsValid( void ) const;
    /**
     * \brief   Invalidates ChatParticipants attribute and
     *          sends error update notification message to clients.
     **/
    inline void InvalidateChatParticipants( void );
    /**
     * \brief   Force to send ChatParticipants attribute notification message to all clients.
     *          The method can be called when the attribute is updated not via set-method.
     **/
     inline void NotifyChatParticipantsUpdated( void );
    /**
     * \brief   Set ChatParticipants attribute value and send notification event to all clients.
     *          The notification is sent only when value is updated or validated.
     * \param   newValue    New value of ChatParticipants attribute to set.
     *                      The ChatParticipants attribute description:
     *                      The list of char-room participants
     **/
    virtual void SetChatParticipants( const NEDirectMessager::ListParticipants & newValue );
    /**
     * \brief   Returns the value of ChatParticipants attribute.
     *          The ChatParticipants attribute description:
     *          The list of char-room participants
     **/
    inline const NEDirectMessager::ListParticipants & GetChatParticipants( void ) const;
    inline NEDirectMessager::ListParticipants & GetChatParticipants( void );

//////////////////////////////////////////////////////////////////////////
// DirectMessager Interface Requests
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Request call.
     *          Request to join chat. The participant should be in the list of connections
     * \param   participant The participant to join chat. The participant should be in the connection list.
     * \param   timeConnect The timestamp when the request was sent.
     * \see     ResponseChatJoin
     **/
    virtual void RequestChatJoin( const NEDirectMessager::sParticipant & participant, const CEDateTime & timeConnect ) = 0;

    /**
     * \brief   Request call.
     *          Request to send a message, has no response
     * \param   sender      The participant structure of message sender
     * \param   msgText     The text message to send
     * \param   timeSent    The timestamp when the message is requested to send.
     * \note    Has no response
     **/
    virtual void RequestMessageSend( const NEDirectMessager::sParticipant & sender, const CEString & msgText, const CEDateTime & timeSent ) = 0;

    /**
     * \brief   Request call.
     *          The request to send message while typing a text. Has no response.
     * \param   participant The structure of participant while sending message during typing.
     * \param   msgText     The text message while typing.
     * \note    Has no response
     **/
    virtual void RequestMessageType( const NEDirectMessager::sParticipant & participant, const CEString & msgText ) = 0;

    /**
     * \brief   Request call.
     *          The request to leave chat-room. Has no response.
     * \param   participant The structure of chat-room participant.
     * \param   timeLeave   Timestamp when it was requested to leave chat-room.
     * \note    Has no response
     **/
    virtual void RequestChatLeave( const NEDirectMessager::sParticipant & participant, const CEDateTime & timeLeave ) = 0;

//////////////////////////////////////////////////////////////////////////
// DirectMessager Interface Responses
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Response call.
     *          Response to join chat
     * \param   succeed         Flag, indicating whether connection is accepted or not.
     * \param   listParticipant The list of participants.
     * \param   timeConnect     Timestamp when it was requested to join chat
     * \param   timeConnected   Timestamp when the request to join was accepted and new participants was registered.
     * \see     RequestChatJoin
     **/
    virtual void ResponseChatJoin( const bool & succeed, const NEDirectMessager::ListParticipants & listParticipant, const CEDateTime & timeConnect, const CEDateTime & timeConnected );

//////////////////////////////////////////////////////////////////////////
// DirectMessager Interface Broadcasts
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Broadcast call.
     *          Informs all connected servicing clients that the message is sent.
     * \param   sender      The structure of participant, who sent the message.
     * \param   msgText     The message, which was sent.
     * \param   timeSent    The timestamp when the message was sent.
     **/
    virtual void BroadcastMessageSent( const NEDirectMessager::sParticipant & sender, const CEString & msgText, const CEDateTime & timeSent );

    /**
     * \brief   Broadcast call.
     *          Informs all connected servicing client that the text was typed.
     * \param   participant The structure of participant, who initiated message during typing.
     * \param   msgText     The text message while typing.
     **/
    virtual void BroadcastMessageTyped( const NEDirectMessager::sParticipant & participant, const CEString & msgText );

    /**
     * \brief   Broadcast call.
     *          Informs new participant joined chat-room
     * \param   participant The structure of participant, joined chat-room.
     * \param   timeJoined  Timestamp when participant joined the chat-room
     **/
    virtual void BroadcastParticipantJoined( const NEDirectMessager::sParticipant & participant, const CEDateTime & timeJoined );

    /**
     * \brief   Broadcast call.
     *          Informes that a participant left chat-room.
     * \param   participant The structure of participant, who left the chat-room.
     * \param   timeLeft    The timestamp when the participant left chat-room.
     **/
    virtual void BroadcastParticipantLeft( const NEDirectMessager::sParticipant & participant, const CEDateTime & timeLeft );

    /**
     * \brief   Broadcast call.
     *          Informes all service connected clients that the chat-room is closed. Message will be impossible.
     **/
    virtual void BroadcastChatClosed( void );

//////////////////////////////////////////////////////////////////////////
// End Service Interface operations / attributes and overrides declaration
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//  Operations.
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// CEStubBase overrides. Public methods.
/************************************************************************/

    /**
     * \brief   Returns implemented version of service interface.
     **/
    virtual const CEVersion & GetImplVersion( void ) const;

    /**
     * \brief   Sends update notification message to all clients. 
     *          This method can be called manually to send update 
     *          notification message after updating attribute value.
     *
     * \param   msgId   The attribute message ID to notify clients.
     **/
    virtual void SendNotification( const unsigned int msgId );

    /**
     * \brief   Sends error message to clients.
     *          If message ID is a request, it should send result NEService::RESULT_REQUEST_ERROR or NEService::RESULT_REQUEST_CANCELED, depending on msgCancel flag.
     *          If message ID is a response, it should send result NEService::RESULT_INVALID.
     *          If message ID is an attribute, it should send result NEService::RESULT_DATA_INVALID
     *          and invalidate attribute data value.
     *
     * \param   msgId       The message ID to send error message
     * \param   msgCancel   Indicates whether the request is canceled or should respond with error.
     *                      This parameter has sense only for request IDs.
     *                      It is ignored for response and attributes IDs.
     **/
    virtual void ErrorRequest( const unsigned int msgId, const bool msgCancel );

protected:
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
    virtual void StartupServiceInterface( CEComponent & holder );

    /**
     * \brief   This function is triggered by Component when shuts down.
     *          Overwrite this method to remove listeners and stub cleanup
     * \param   holder  The holder component of service interface of Stub,
     *                  which shuts down.
     **/
    virtual void ShutdownServiceIntrface ( CEComponent & holder );

/************************************************************************/
// CEStubBase overrides. Protected methods 
/************************************************************************/

    /**
     * \brief   Returns number of requests of Service Interface
     **/
    virtual const unsigned int GetNumberOfRequests(void) const;

    /**
     * \brief   Returns number of responses of Service Interface
     **/
    virtual const unsigned int GetNumberOfResponses( void ) const;

    /**
     * \brief   Returns number of attributes of Service Interface
     **/
    virtual const unsigned int GetNumberOfAttributes( void ) const;

    /**
     * \brief   Returns pointer of array of requests IDs of Service Interface
     **/
    virtual const unsigned int * GetRequestIds(void) const;

    /**
     * \brief   Returns pointer of array of response IDs of Service Interface
     **/
    virtual const unsigned int * GetResponseIds(void) const;

    /**
     * \brief   Returns pointer of array of attribute IDs of Service Interface
     **/
    virtual const unsigned int * GetAttributeIds(void) const;

    /**
     * \brief   Creates Response event object to pass of client.
     * \param   proxy   The address of proxy object to send response event
     * \param   msgId   The message ID to send to client
     * \param   result  The result of message
     * \param   data    The buffer of data to send to client. Can be Invalid buffer.
     * \return  Returns valid pointer to Response event object
     **/
    virtual CEResponseEvent * CreateResponseEvent( const CEProxyAddress & proxy, const unsigned int msgId, const NEService::eResultType result, const CEEventDataStream & data ) const;

    /**
     * \brief   Overwrite method to create remote service request event from streaming object for 
     *          further dispatching by stub.
     * \param   stream  Streaming object, which contains event data.
     * \return  If operation succeeds, returns valid pointer to Service Request event object.
     *          Otherwise, it returns NULL.
     **/
    virtual CERemoteRequestEvent * CreateRemoteServiceRequestEvent( const IEInStream & stream ) const;

    /**
     * \brief   Overwrite method to create remote notify request event from streaming object for 
     *          further dispatching by stub.
     * \param   stream  Streaming object, which contains event data.
     * \return  If operation succeeds, returns valid pointer to Service Request event object.
     *          Otherwise, it returns NULL.
     **/
    virtual CERemoteNotifyRequestEvent * CreateRemoteNotifyRequestEvent( const IEInStream & stream ) const;

private:
/************************************************************************/
// IEComponentConsumer interface overrides.
/************************************************************************/
    /**
     * \brief   Triggered to process generic stub event.
     *          Usually should not be triggered.
     **/
    virtual void ProcessStubEvent( CEStubEvent & eventElem );

    /**
     * \brief   Triggered to process generic event.
     *          Usually should not be triggered.
     **/
    virtual void ProcessGenericEvent( CEEvent & eventElem );

    /**
     * \brief   Triggered to process service request event.
     *          Overwrite method to process every service request event.
     * \param   eventElem   Service Request Event object, contains request
     *                      call ID and parameters.
     **/
    virtual void ProcessRequestEvent( CEServiceRequestEvent & eventElem );

    /**
     * \brief   Triggered to process attribute update notification event.
     *          Override method to process request to get attribute value and
     *          process notification request of attribute update.
     * \param   eventElem   Service Request Event object, contains attribute ID.
     **/
    virtual void ProcessAttributeEvent( CEServiceRequestEvent & eventElem );

//////////////////////////////////////////////////////////////////////////
// Hidden function calls
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns reference to CEDirectMessagerStub object
     **/
    inline CEDirectMessagerStub & self( void );

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
    CEDirectMessagerStub( void );
    CEDirectMessagerStub( const CEDirectMessagerStub & /* src */ );
    const CEDirectMessagerStub & operator = ( const CEDirectMessagerStub & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// CEDirectMessagerStub class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline CEDirectMessagerStub & CEDirectMessagerStub::self( void )
{   return (*this); }

/************************************************************************
 * \brief   ChatParticipants attribute functions
 ************************************************************************/
 
inline bool CEDirectMessagerStub::IsChatParticipantsValid( void ) const
{   return (mChatParticipantsState  == NEService::DATA_OK); }

inline void CEDirectMessagerStub::InvalidateChatParticipants( void )
{   ErrorRequest( static_cast<const unsigned int &>(NEDirectMessager::MSG_ID_ChatParticipants), false ); }

inline void CEDirectMessagerStub::NotifyChatParticipantsUpdated( void )
{   SendNotification( NEDirectMessager::MSG_ID_ChatParticipants );   }

inline const NEDirectMessager::ListParticipants & CEDirectMessagerStub::GetChatParticipants( void ) const
{   return mChatParticipants; }

inline NEDirectMessager::ListParticipants & CEDirectMessagerStub::GetChatParticipants( void )
{   return mChatParticipants; }

//////////////////////////////////////////////////////////////////////////
// CEDirectMessagerStub class declaration End
//////////////////////////////////////////////////////////////////////////

#endif   // SHARED_GENERATED_CEDIRECTMESSAGERSTUB_HPP

//////////////////////////////////////////////////////////////////////////
// End generate shared/generated/CEDirectMessagerStub.hpp file
//////////////////////////////////////////////////////////////////////////
