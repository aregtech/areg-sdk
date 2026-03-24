/************************************************************************
 * \file            chatter/services/DirectMessagingClient.cpp
 * \brief           The messaging service client object
 ************************************************************************/

#include "chatter/services/DirectMessagingClient.hpp"
#include "chatter/services/ChatPrticipantHandler.hpp"
#include "common/ChatDefs.hpp"
#include "areg/logging/areg_log.h"

#ifndef NOMINMAX
    #define NOMINMAX
#endif // !NOMINMAX
#include <Windows.h>

#define FIRST_MESSAGE       (WM_USER + 10 + static_cast<uint32_t>(NEDistributedApp::WindowCommand::CmdFirst))
#define MAKE_MESSAGE(elem)  (static_cast<uint32_t>(elem) + FIRST_MESSAGE)

DEF_LOG_SCOPE(chatter_DirectMessagingClient, service_connected);
DEF_LOG_SCOPE(chatter_DirectMessagingClient, response_chat_join);
DEF_LOG_SCOPE(chatter_DirectMessagingClient, broadcast_message_sent);
DEF_LOG_SCOPE(chatter_DirectMessagingClient, broadcast_message_typed);
DEF_LOG_SCOPE(chatter_DirectMessagingClient, broadcast_participant_joined);
DEF_LOG_SCOPE(chatter_DirectMessagingClient, broadcast_participant_left);
DEF_LOG_SCOPE(chatter_DirectMessagingClient, broadcast_chat_closed);

DirectMessagingClient::DirectMessagingClient( areg::Component & owner, const char * roleName, ChatPrticipantHandler* handlerParticipants )
    : DirectMessagerConsumerBase  ( roleName, owner )
    , mParticipantsHandler      ( handlerParticipants )
    , mJoinedChat               (false)
{
    ASSERT(mParticipantsHandler != nullptr);
}

void DirectMessagingClient::shutdownChat()
{
    clear_all_notifications();
    if (mJoinedChat)
    {
        mJoinedChat = false;
        request_chat_leave(mParticipantsHandler->GetConnectionOwner(), areg::DateTime::now());
    }
}

bool DirectMessagingClient::service_connected( areg::ServiceConnectionState status, areg::ProxyBase & proxy )
{
    LOG_SCOPE( chatter_DirectMessagingClient, service_connected );

    bool result = DirectMessagerConsumerBase::service_connected( status, proxy );
    notify_on_broadcast_chat_closed( is_connected( ) );
    notify_on_broadcast_participant_joined( is_connected( ) );
    notify_on_broadcast_participant_left( is_connected( ) );

    if ( is_connected( ) )
    {
        mJoinedChat = true;
        mParticipantsHandler->SetChatClient( this );
        request_chat_join( mParticipantsHandler->GetConnectionOwner( ), areg::DateTime::now( ) );
    }
    else
    {
        shutdownChat();
    }

    return result;
}

void DirectMessagingClient::response_chat_join( bool succeed, const DirectMessager::ListParticipants & listParticipant, const areg::DateTime & timeConnect, const areg::DateTime & timeConnected )
{
    LOG_SCOPE( chatter_DirectMessagingClient, response_chat_join );

    postMessage(NEDistributedApp::WindowCommand::CmdChatJoined, succeed ? 1 : 0, 0);
    updateChatOutput( NEDistributedApp::WindowCommand::CmdChatMessage, mParticipantsHandler->GetConnectionOwner( ), succeed ? "Succeeded join chat..." : "Failed join chat...", timeConnect, timeConnected );

    if ( succeed )
    {
        mJoinedChat = true;
        for (uint32_t i = 0; i < listParticipant.size(); ++i)
        {
            updateChatOutput(NEDistributedApp::WindowCommand::CmdChatMessage, listParticipant[i], "Is in chat room", areg::DateTime( ), areg::DateTime( ) );
        }
    }
}

void DirectMessagingClient::broadcast_message_sent( const DirectMessager::Participant & sender, const areg::String & msgText, const areg::DateTime & timeSent )
{
    LOG_SCOPE( chatter_DirectMessagingClient, broadcast_message_sent );
    updateChatOutput(NEDistributedApp::WindowCommand::CmdChatMessage, sender, msgText, timeSent, areg::DateTime::now() );
}

void DirectMessagingClient::broadcast_message_typed( const DirectMessager::Participant & participant, const areg::String & msgText )
{
    LOG_SCOPE( chatter_DirectMessagingClient, broadcast_message_typed );
    updateChatOutput(NEDistributedApp::WindowCommand::CmdChatTyping, participant, msgText, areg::DateTime( ), areg::DateTime( ) );
}

void DirectMessagingClient::broadcast_participant_joined( const DirectMessager::Participant & participant, const areg::DateTime & timeJoined )
{
    LOG_SCOPE( chatter_DirectMessagingClient, broadcast_participant_joined );
    if ( participant != mParticipantsHandler->GetConnectionOwner() )
        updateChatOutput(NEDistributedApp::WindowCommand::CmdChatMessage, participant, areg::String( "Joined chat" ), timeJoined, areg::DateTime::now() );
}

void DirectMessagingClient::broadcast_participant_left( const DirectMessager::Participant & participant, const areg::DateTime & timeLeft )
{
    LOG_SCOPE( chatter_DirectMessagingClient, broadcast_participant_left );
    updateChatOutput(NEDistributedApp::WindowCommand::CmdChatMessage, participant, areg::String( "Left chat" ), timeLeft, areg::DateTime::now() );
}

void DirectMessagingClient::broadcast_chat_closed()
{
    LOG_SCOPE( chatter_DirectMessagingClient, broadcast_chat_closed );
    updateChatOutput(NEDistributedApp::WindowCommand::CmdChatClosed, DirectMessager::Participant(), areg::String( "Chat Closed" ), areg::DateTime(), areg::DateTime() );

    notify_on_broadcast_message_sent( false );
    notify_on_broadcast_message_typed( false );

    notify_on_broadcast_chat_closed(false);
    notify_on_broadcast_participant_joined(false);
    notify_on_broadcast_participant_left(false);
}

void DirectMessagingClient::updateChatOutput( const NEDistributedApp::WindowCommand cmdSend, const DirectMessager::Participant & participant, const areg::String & msgText, const areg::DateTime & dateStart, const areg::DateTime & dateEnd )
{
    chat:: MessageData * data = chat::newData( );
    if ( data != nullptr )
    {
        areg::String nickName;
        if ( mParticipantsHandler->GetConnectionOwner() == participant )
            nickName = "[ " + participant.nickName + " ]";
        else
            nickName = participant.nickName;
        areg::copy_string<TCHAR, char>( data->nickName, chat::MAXLEN_NICKNAME, nickName.as_string( ) );
        areg::copy_string<TCHAR, char>( data->message , chat::MAXLEN_MESSAGE , msgText.as_string( )  );
        data->timeSend      = dateStart;
        data->timeReceived  = dateEnd;
        data->dataSave      = participant.cookie;

        postMessage(cmdSend, static_cast<ptr_type>(participant.sessionId), reinterpret_cast<ptr_type>(data));
    }
}

inline void DirectMessagingClient::postMessage(NEDistributedApp::WindowCommand cmdSend, ptr_type wParam, ptr_type lParam)
{
    HWND hWnd = reinterpret_cast<HWND>(mParticipantsHandler->GetChatWindow());
    ASSERT(hWnd != nullptr);

    ::PostMessage(hWnd, MAKE_MESSAGE(cmdSend), static_cast<WPARAM>(wParam), static_cast<LPARAM>(lParam));
}
