#pragma once
/************************************************************************
 * \file            chatter/services/DirectMessagingClient.hpp
 * \brief           The messaging service client object
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "areg/component/Component.hpp"
#include "generated/src/DirectMessagerClientBase.hpp"
#include "chatter/NEDistributedApp.hpp"

class ChatPrticipantHandler;

class DirectMessagingClient   : public DirectMessagerClientBase
{
public:
    DirectMessagingClient( Component & owner, const char * roleName, ChatPrticipantHandler & handlerParticipants );
    virtual ~DirectMessagingClient( void );

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:
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

/************************************************************************/
// IEProxyListener Overrides
/************************************************************************/
    /**
     * \brief   Triggered when receives service provider connected / disconnected event.
     *          When the service provider is connected, the client objects can set the listeners here.
     *          When the service provider is disconnected, the client object should clean the listeners.
     *          Up from connected status, the clients can subscribe and unsubscribe on updates,
     *          responses and broadcasts, can trigger requests. Before connection, the clients cannot
     *          neither trigger requests, nor receive data update messages.
     * \param   status  The service connection status.
     * \param   proxy   The Service Interface Proxy object, which is notifying service connection.
     * \return  Return true if this service connect notification was relevant to client object.
     **/
    virtual bool serviceConnected( NEService::eServiceConnection status, ProxyBase & proxy ) override;

//////////////////////////////////////////////////////////////////////////
// Hidden members
//////////////////////////////////////////////////////////////////////////
private:
    inline DirectMessagingClient & self( void );

    inline void updateChatOutput( NEDistributedApp::eWndCommands cmdSend, const NEDirectMessager::sParticipant & participant, const String & msgText, const DateTime & dateStart, const DateTime & dateEnd );

    inline void postMessage(NEDistributedApp::eWndCommands cmdSend, ptr_type wParam, ptr_type lParam);

private:
    ChatPrticipantHandler &   mParticipantsHandler;
//////////////////////////////////////////////////////////////////////////
// Hidden members
//////////////////////////////////////////////////////////////////////////
private:
    DirectMessagingClient( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( DirectMessagingClient );
};

inline DirectMessagingClient & DirectMessagingClient::self( void )
{
    return (*this);
}
