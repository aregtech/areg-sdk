#pragma once
/************************************************************************
 * \file            chatter/services/DirectMessagingClient.hpp
 * \brief           The messaging service client object
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "areg/component/Component.hpp"
#include "generated/DirectMessagerClientBase.hpp"
#include "chatter/NEDistributedApp.hpp"

class ChatPrticipantHandler;

class DirectMessagingClient   : public DirectMessagerClientBase
{
public:
    DirectMessagingClient( Component & owner, const char * roleName, ChatPrticipantHandler & handlerParticipants );
    virtual ~DirectMessagingClient( void );

public:
    /**
     * \brief   Response callback.
     *          Response to join chat
     *          Overwrite, if need to handle Response call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   succeed         Flag, indicating whether connection is accepted or not.
     * \param   listParticipant The list of participants.
     * \param   timeConnect     Time-stamp when it was requested to join chat
     * \param   timeConnected   Time-stamp when the request to join was accepted and new participants was registered.
     * \see     requestChatJoin
     **/
    virtual void responseChatJoin( bool succeed, const NEDirectMessager::ListParticipants & listParticipant, const DateTime & timeConnect, const DateTime & timeConnected );

    /**
     * \brief   Server broadcast.
     *          Informs all connected servicing clients that the message is sent.
     *          Overwrite, if need to handle Broadcast call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   sender      The structure of participant, who sent the message.
     * \param   msgText     The message, which was sent.
     * \param   timeSent    The time-stamp when the message was sent.
     **/
    virtual void broadcastMessageSent( const NEDirectMessager::sParticipant & sender, const String & msgText, const DateTime & timeSent );

    /**
     * \brief   Server broadcast.
     *          Informs all connected servicing client that the text was typed.
     *          Overwrite, if need to handle Broadcast call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   participant The structure of participant, who initiated message during typing.
     * \param   msgText     The text message while typing.
     **/
    virtual void broadcastMessageTyped( const NEDirectMessager::sParticipant & participant, const String & msgText );

    /**
     * \brief   Server broadcast.
     *          Informs new participant joined chat-room
     *          Overwrite, if need to handle Broadcast call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   participant The structure of participant, joined chat-room.
     * \param   timeJoined  Time-stamp when participant joined the chat-room
     **/
    virtual void broadcastParticipantJoined( const NEDirectMessager::sParticipant & participant, const DateTime & timeJoined );

    /**
     * \brief   Server broadcast.
     *          Informs that a participant left chat-room.
     *          Overwrite, if need to handle Broadcast call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   participant The structure of participant, who left the chat-room.
     * \param   timeLeft    The time-stamp when the participant left chat-room.
     **/
    virtual void broadcastParticipantLeft( const NEDirectMessager::sParticipant & participant, const DateTime & timeLeft );

    /**
     * \brief   Server broadcast.
     *          Informs all service connected clients that the chat-room is closed. Message will be impossible.
     *          Overwrite, if need to handle Broadcast call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     **/
    virtual void broadcastChatClosed( void );

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
    virtual bool serviceConnected( bool isConnected, ProxyBase & proxy );

private:
    inline DirectMessagingClient & self( void );

    inline void updateChatOutput( NEDistributedApp::eWndCommands cmdSend, const NEDirectMessager::sParticipant & participant, const String & msgText, const DateTime & dateStart, const DateTime & dateEnd );

    inline void postMessage(NEDistributedApp::eWndCommands cmdSend, ptr_type wParam, ptr_type lParam);

private:
    ChatPrticipantHandler &   mParticipantsHandler;
};

inline DirectMessagingClient & DirectMessagingClient::self( void )
{   return (*this);         }
