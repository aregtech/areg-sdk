//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/CEDirectMessagerClientBase.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  SHARED_GENERATED_CEDIRECTMESSAGERCLIENTBASE_HPP
#define  SHARED_GENERATED_CEDIRECTMESSAGERCLIENTBASE_HPP
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source DirectMessager.
 * Generated at     25.04.2021  23:50:46 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shared/generated/CEDirectMessagerClientBase.hpp
 * \ingroup         DirectMessager Service Interface
 * \brief           This is an automatic generated code of DirectMessager Service Interface Client base class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "shared/generated/NEDirectMessager.hpp"
#include "areg/component/CEClientBase.hpp"
#include "areg/component/IEProxyListener.hpp"
#include "areg/component/CENotificationEvent.hpp"

#include "shared/generated/private/CEDirectMessagerProxy.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class CEComponent;
class CEDispatcherThread;

//////////////////////////////////////////////////////////////////////////
// CEDirectMessagerClientBase class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       The interface of DirectMessager Service Interface
 *              Client base object. This object should be inherited
 *              and overrides should be implemented.
 *
 *              The service interface of direct connection and messaging used in chat-room.
 **/
class CEDirectMessagerClientBase  : public    IEProxyListener, private CEClientBase
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Constructs and initialize DirectMessager Service Interface client object
     * \param   roleName    The role name assigned to DirectMessager servicing component object.
     * \param   ownerThread The name of component owner thread to dispatch messages.
     *                      If NULL, all messages are dispatched in current component thread.
     **/
    CEDirectMessagerClientBase( const char* roleName, const char * ownerThread = static_cast<const char *>(NULL) );

    /**
     * \brief   Constructs and initialize DirectMessager Service Interface client object.
     * \param   roleName    The role name assigned to DirectMessager servicing component object.
     * \param   ownerThread The instance of component owner thread to dispatch messages.
     **/
    CEDirectMessagerClientBase( const char* roleName, CEDispatcherThread & ownerThread );

    /**
     * \brief   Constructs and initialize DirectMessager Service Interface client object.
     * \param   roleName    The role name assigned to DirectMessager servicing component object.
     * \param   owner       The instance of client owner component. The component object should be already initialized.
     * \note    When this constructor is used, it is important that the CEComponent object is already initialized.
     *          and the component thread is set.
     **/
    CEDirectMessagerClientBase( const char* roleName, CEComponent & owner );

    /**
     * \brief   Destructor.
     **/
    virtual ~CEDirectMessagerClientBase( void );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Clears all notifications, stops receiving notifications from server
     **/
    void ClearAllNotifications( void );

    /**
     * \brief   Returns true if the specified certain notification is already assigned.
     *          Otherwise returns false.
     * \param   msgId   The ID of message to check.
     **/
    bool IsNotificationAssigned( NEDirectMessager::eMessageIDs msgId ) const;

    /**
     * \brief   Returns true if client object has got connection with servicing component
     **/
    bool IsConnected( void ) const;

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
     * \brief   Returns true if value of ChatParticipants attribute is valid.
     *          If Update Notification is disabled, this method will return false.
     **/
    bool IsChatParticipantsValid( void ) const;
    /**
     * \brief   Returns the value of ChatParticipants attribute.
     *          To get valid value, the Update Notification should be enabled. 
     *          Attribute ChatParticipants description: 
     *          The list of char-room participants
     * \param   state   On returns, contains the validation flag of attribute data. 
     *                  Check validation flag before use attribute value.
     * \see     IsChatParticipantsValid, NotifyChatParticipantsUpdate, OnChatParticipantsUpdate
     **/
    const NEDirectMessager::ListParticipants & GetChatParticipants( NEService::eDataStateType & state ) const;
    /**
     * \brief   Call to enable or disable receiving notifications on ChatParticipants attribute update.
     *          Once notification is enabled and the data is updated, 
     *          the GetChatParticipants method will return valid data 
     *          Attribute ChatParticipants description:
     *          The list of char-room participants
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     * \see     IsChatParticipantsValid, GetChatParticipants, OnChatParticipantsUpdate
     **/
    void NotifyChatParticipantsUpdate( bool notify = true );
    /**
     * \brief   Triggered, when ChatParticipants attribute is updated. The function contains
     *          attribute value and validation flag. When notification is enabled,
     *          the method should be overwritten in derived class.
     *          Attributes ChatParticipants description: 
     *          The list of char-room participants
     * \param   ChatParticipants    The value of ChatParticipants attribute.
     * \param   state               The data validation flag.
     **/
    virtual void OnChatParticipantsUpdate( const NEDirectMessager::ListParticipants & ChatParticipants, NEService::eDataStateType state );


//////////////////////////////////////////////////////////////////////////
// DirectMessager Interface Requests / Responses / Broadcasts
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************
 * Request ChatJoin
 ************************************************************************/
    /**
     * \brief   Request call.
     *          Request to join chat. The participant should be in the list of connections
     * \param   participant The participant to join chat. The participant should be in the connection list.
     * \param   timeConnect The timestamp when the request was sent.
     * \return  The sequence count number of call
     * \see     ResponseChatJoin
     **/
    unsigned int RequestChatJoin( const NEDirectMessager::sParticipant & participant, const CEDateTime & timeConnect );
    /**
     * \brief   Overwrite to handle error of ChatJoin request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void RequestChatJoinFailed( NEService::eResultType FailureReason );
    
/************************************************************************
 * Request MessageSend
 ************************************************************************/
    /**
     * \brief   Request call.
     *          Request to send a message, has no response
     * \param   sender      The participant structure of message sender
     * \param   msgText     The text message to send
     * \param   timeSent    The timestamp when the message is requested to send.
     * \see     Has no response
     **/
    inline void RequestMessageSend( const NEDirectMessager::sParticipant & sender, const CEString & msgText, const CEDateTime & timeSent );
    /**
     * \brief   Overwrite to handle error of MessageSend request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void RequestMessageSendFailed( NEService::eResultType FailureReason );
    
/************************************************************************
 * Request MessageType
 ************************************************************************/
    /**
     * \brief   Request call.
     *          The request to send message while typing a text. Has no response.
     * \param   participant The structure of participant while sending message during typing.
     * \param   msgText     The text message while typing.
     * \see     Has no response
     **/
    inline void RequestMessageType( const NEDirectMessager::sParticipant & participant, const CEString & msgText );
    /**
     * \brief   Overwrite to handle error of MessageType request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void RequestMessageTypeFailed( NEService::eResultType FailureReason );
    
/************************************************************************
 * Request ChatLeave
 ************************************************************************/
    /**
     * \brief   Request call.
     *          The request to leave chat-room. Has no response.
     * \param   participant The structure of chat-room participant.
     * \param   timeLeave   Timestamp when it was requested to leave chat-room.
     * \see     Has no response
     **/
    inline void RequestChatLeave( const NEDirectMessager::sParticipant & participant, const CEDateTime & timeLeave );
    /**
     * \brief   Overwrite to handle error of ChatLeave request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void RequestChatLeaveFailed( NEService::eResultType FailureReason );
    
/************************************************************************
 * Response ChatJoin
 ************************************************************************/
    /**
     * \brief   Response callback.
     *          Response to join chat
     *          Overwrite, if need to handle Response call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   succeed         Flag, indicating whether connection is accepted or not.
     * \param   listParticipant The list of participants.
     * \param   timeConnect     Timestamp when it was requested to join chat
     * \param   timeConnected   Timestamp when the request to join was accepted and new participants was registered.
     * \see     RequestChatJoin
     **/
    virtual void ResponseChatJoin( const bool & succeed, const NEDirectMessager::ListParticipants & listParticipant, const CEDateTime & timeConnect, const CEDateTime & timeConnected );
    /**
     * \brief   Call to enable or disable receiving notifications on ChatJoin response call.
     *          This function is triggered, when client object is interested only on response result
     *          without triggering request call.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     **/
    inline void NotifyOnResponseChatJoin( bool notify = true );

/************************************************************************
 * Broadcast MessageSent
 ************************************************************************/
    /**
     * \brief   Server broadcast.
     *          Informs all connected servicing clients that the message is sent.
     *          Overwrite, if need to handle Broadcast call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   sender      The structure of participant, who sent the message.
     * \param   msgText     The message, which was sent.
     * \param   timeSent    The timestamp when the message was sent.
     **/
    virtual void BroadcastMessageSent( const NEDirectMessager::sParticipant & sender, const CEString & msgText, const CEDateTime & timeSent );
    /**
     * \brief   Call to enable or disable receiving notifications on MessageSent broadcast call.
     *          This function is triggered, when client object is interested only on response result
     *          without triggering request call.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     **/
    inline void NotifyOnBroadcastMessageSent( bool notify = true );

/************************************************************************
 * Broadcast MessageTyped
 ************************************************************************/
    /**
     * \brief   Server broadcast.
     *          Informs all connected servicing client that the text was typed.
     *          Overwrite, if need to handle Broadcast call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   participant The structure of participant, who initiated message during typing.
     * \param   msgText     The text message while typing.
     **/
    virtual void BroadcastMessageTyped( const NEDirectMessager::sParticipant & participant, const CEString & msgText );
    /**
     * \brief   Call to enable or disable receiving notifications on MessageTyped broadcast call.
     *          This function is triggered, when client object is interested only on response result
     *          without triggering request call.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     **/
    inline void NotifyOnBroadcastMessageTyped( bool notify = true );

/************************************************************************
 * Broadcast ParticipantJoined
 ************************************************************************/
    /**
     * \brief   Server broadcast.
     *          Informs new participant joined chat-room
     *          Overwrite, if need to handle Broadcast call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   participant The structure of participant, joined chat-room.
     * \param   timeJoined  Timestamp when participant joined the chat-room
     **/
    virtual void BroadcastParticipantJoined( const NEDirectMessager::sParticipant & participant, const CEDateTime & timeJoined );
    /**
     * \brief   Call to enable or disable receiving notifications on ParticipantJoined broadcast call.
     *          This function is triggered, when client object is interested only on response result
     *          without triggering request call.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     **/
    inline void NotifyOnBroadcastParticipantJoined( bool notify = true );

/************************************************************************
 * Broadcast ParticipantLeft
 ************************************************************************/
    /**
     * \brief   Server broadcast.
     *          Informes that a participant left chat-room.
     *          Overwrite, if need to handle Broadcast call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   participant The structure of participant, who left the chat-room.
     * \param   timeLeft    The timestamp when the participant left chat-room.
     **/
    virtual void BroadcastParticipantLeft( const NEDirectMessager::sParticipant & participant, const CEDateTime & timeLeft );
    /**
     * \brief   Call to enable or disable receiving notifications on ParticipantLeft broadcast call.
     *          This function is triggered, when client object is interested only on response result
     *          without triggering request call.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     **/
    inline void NotifyOnBroadcastParticipantLeft( bool notify = true );

/************************************************************************
 * Broadcast ChatClosed
 ************************************************************************/
    /**
     * \brief   Server broadcast.
     *          Informes all service connected clients that the chat-room is closed. Message will be impossible.
     *          Overwrite, if need to handle Broadcast call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     **/
    virtual void BroadcastChatClosed( void );
    /**
     * \brief   Call to enable or disable receiving notifications on ChatClosed broadcast call.
     *          This function is triggered, when client object is interested only on response result
     *          without triggering request call.
     * \param   notify  If true, notification will be enable. If false, notification is disabled
     **/
    inline void NotifyOnBroadcastChatClosed( bool notify = true );

//////////////////////////////////////////////////////////////////////////
// End Service Interface operations / attributes and overrides declaration
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// IEProxyListener Overrides
/************************************************************************/
    /**
     * \brief   Triggered by Proxy, when gets service connected event.
     *          Make client initializations in this function. No request
     *          will be processed until this function is not called for
     *          client object. Also set listeners here if client is interested
     *          to receive update notifications.
     * \param   isConnected     The flag, indicating whether service is connected
     *                          or disconnected. Make cleanups and stop sending
     *                          requests or assigning for notification if
     *                          this flag is false. No event to Stub target will
     *                          be sent, until no service connected event is
     *                          received.
     * \param   proxy           The Service Interface Proxy object, which is
     *                          notifying service connection.
     * \return  Return true if this service connect notification was relevant to client object,
     *          i.e. if passed Proxy address is equal to the Proxy object that client has.
     *          If Proxy objects are not equal, it should return false;
     **/
    virtual bool ServiceConnected( bool isConnected, CEProxyBase & proxy );

protected:
/************************************************************************/
// CEDirectMessagerClientBase Error Handling overrides
/************************************************************************/

    /**
     * \brief   Overwrite this method if need to make error handling on invalid response
     * \param   InvalidRespId   The ID of invalid response
     **/
    virtual void InvalidResponse( NEDirectMessager::eMessageIDs InvalidRespId );

    /**
     * \brief   Overwrite this method if need to make error handling on invalid request
     * \param   InvalidReqId    The ID of invalid request
     **/
    virtual void InvalidRequest( NEDirectMessager::eMessageIDs InvalidReqId );
    
    /**
     * \brief   By default, the function calls appropriate request failure function.
     *          Overwrite this method if need to make error handling on request failed.
     * \param   msgId           The ID of either response of request message, which failed. Normally ID of request.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void RequestFailed( NEDirectMessager::eMessageIDs FailureMsgId, NEService::eResultType FailureReason );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Returns the current sequence number
     **/
    unsigned int GetCurrentSequenceNr( void ) const;

    /**
     * \brief   Call to recreate Proxy for the client. This call will remove and unregister all existing notifications
     *          and the client will not receive pending update or response notifications. 
     *          The client first will receive disconnect message, then again connect.
     *          Reset update notifications manually in connect if need.          
     *          This function call can be used when the client instance should be registered and run in new thread.
     *
     * \return  Returns true if Proxy was created with success.
     **/
     bool RecreateProxy( void );

     /**
      * \brief  Returns pointer to client dispatcher thread where the messages are dispatched.
      *         The function can return NULL if Proxy was not instantiated yet.
      **/
     CEDispatcherThread * GetDispatcherThread( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Connection flag. If true, the client object is connected to server
     **/
    bool                mIsConnected;
    /**
     * \brief   The counter of sequence number
     **/
    unsigned int        mCurrSequenceNr;
    /**
     * \brief   Pointer of Proxy object providing communication
     **/
    CEDirectMessagerProxy *   mProxy;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************/
// IENotificationEventConsumer overrides
/************************************************************************/

    /**
     * \brief   Is processing notification event calls.
     * \param   eventElem   Notification Event object to process
     **/
    virtual void ProcessNotificationEvent( CENotificationEvent & eventElem );
    
private:
    /**
     * \brief   Enables / Disables notification flags on appropriate message call.
     * \param   msgId   The ID of message to enable / disable notification
     * \param   notify  If true, the notification is enabled. 
     *                  If false, the notification is disabled.
     * \param   always  Flag, indicating whether should notify always or not.
     *                  if 'notify' parameter is true and the notification is already
     *                  assigned, then if parameter 'always' is true, it will trigger
     *                  notification immediately after call. 
     **/
    void NotifyOn( NEDirectMessager::eMessageIDs msgId, bool notify, bool always = false );
    /**
     * \brief   Returns reference of CEDirectMessagerClientBase object
     **/
    CEDirectMessagerClientBase & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CEDirectMessagerClientBase( void );
    CEDirectMessagerClientBase( const CEDirectMessagerClientBase & /* src */ );
    const CEDirectMessagerClientBase & operator = ( const CEDirectMessagerClientBase & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// CEDirectMessagerClientBase class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline CEDirectMessagerClientBase & CEDirectMessagerClientBase::self( void )
{   return (*this);         }

inline unsigned int CEDirectMessagerClientBase::GetCurrentSequenceNr( void ) const
{   return mCurrSequenceNr; }

inline bool CEDirectMessagerClientBase::IsConnected( void ) const
{   return mIsConnected;    }

inline bool CEDirectMessagerClientBase::IsNotificationAssigned( NEDirectMessager::eMessageIDs msgId ) const
{   return mProxy->HasNotificationListener(static_cast<unsigned int>(msgId));   }

/************************************************************************
 * Attribute inline functions
 ************************************************************************/

inline bool CEDirectMessagerClientBase::IsChatParticipantsValid( void ) const
{   return mProxy->IsChatParticipantsValid( );  }

inline const NEDirectMessager::ListParticipants & CEDirectMessagerClientBase::GetChatParticipants( NEService::eDataStateType & state ) const
{   return mProxy->GetChatParticipants( state ); }

inline void CEDirectMessagerClientBase::NotifyChatParticipantsUpdate( bool notify /* = true */ )
{   NotifyOn( NEDirectMessager::MSG_ID_ChatParticipants, notify, false );  }

/************************************************************************
 * Request calls
 ************************************************************************/

inline unsigned int CEDirectMessagerClientBase::RequestChatJoin( const NEDirectMessager::sParticipant & participant, const CEDateTime & timeConnect )
{   return mProxy->RequestChatJoin( static_cast<IENotificationEventConsumer &>(self()), participant, timeConnect );  }

inline void CEDirectMessagerClientBase::RequestMessageSend( const NEDirectMessager::sParticipant & sender, const CEString & msgText, const CEDateTime & timeSent )
{   mProxy->RequestMessageSend( sender, msgText, timeSent );   }

inline void CEDirectMessagerClientBase::RequestMessageType( const NEDirectMessager::sParticipant & participant, const CEString & msgText )
{   mProxy->RequestMessageType( participant, msgText );   }

inline void CEDirectMessagerClientBase::RequestChatLeave( const NEDirectMessager::sParticipant & participant, const CEDateTime & timeLeave )
{   mProxy->RequestChatLeave( participant, timeLeave );   }

/************************************************************************
 * Response notifications
 ************************************************************************/

inline void CEDirectMessagerClientBase::NotifyOnResponseChatJoin( bool notify /* = true */ )
{   NotifyOn(NEDirectMessager::MSG_ID_ResponseChatJoin, notify, false);    }

/************************************************************************
 * Broadcast notifications
 ************************************************************************/

inline void CEDirectMessagerClientBase::NotifyOnBroadcastMessageSent( bool notify /* = true */ )
{   NotifyOn(NEDirectMessager::MSG_ID_BroadcastMessageSent, notify, false);    }

inline void CEDirectMessagerClientBase::NotifyOnBroadcastMessageTyped( bool notify /* = true */ )
{   NotifyOn(NEDirectMessager::MSG_ID_BroadcastMessageTyped, notify, false);    }

inline void CEDirectMessagerClientBase::NotifyOnBroadcastParticipantJoined( bool notify /* = true */ )
{   NotifyOn(NEDirectMessager::MSG_ID_BroadcastParticipantJoined, notify, false);    }

inline void CEDirectMessagerClientBase::NotifyOnBroadcastParticipantLeft( bool notify /* = true */ )
{   NotifyOn(NEDirectMessager::MSG_ID_BroadcastParticipantLeft, notify, false);    }

inline void CEDirectMessagerClientBase::NotifyOnBroadcastChatClosed( bool notify /* = true */ )
{   NotifyOn(NEDirectMessager::MSG_ID_BroadcastChatClosed, notify, false);    }

#endif   // SHARED_GENERATED_CEDIRECTMESSAGERCLIENTBASE_HPP

//////////////////////////////////////////////////////////////////////////
// End generate shared/generated/CEDirectMessagerClientBase.hpp file
//////////////////////////////////////////////////////////////////////////
