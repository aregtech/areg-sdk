/************************************************************************
 * \file            chatter/services/DirectMessagingClient.cpp
 * \brief           The messaging service client object
 ************************************************************************/

#include "chatter/services/DirectMessagingClient.hpp"
#include "chatter/services/ChatPrticipantHandler.hpp"
#include "common/ChatDefs.hpp"
#include "areg/logging/GELog.h"

#ifndef NOMINMAX
    #define NOMINMAX
#endif // !NOMINMAX
#include <Windows.h>

#define FIRST_MESSAGE       (WM_USER + 10 + static_cast<unsigned int>(NEDistributedApp::WindowCommand::CmdFirst))
#define MAKE_MESSAGE(elem)  (static_cast<unsigned int>(elem) + FIRST_MESSAGE)

DEF_LOG_SCOPE( chatter_DirectMessagingClient_ServiceConnected );
DEF_LOG_SCOPE( chatter_DirectMessagingClient_responseChatJoin );
DEF_LOG_SCOPE( chatter_DirectMessagingClient_broadcastMessageSent );
DEF_LOG_SCOPE( chatter_DirectMessagingClient_broadcastMessageTyped );
DEF_LOG_SCOPE( chatter_DirectMessagingClient_broadcastParticipantJoined );
DEF_LOG_SCOPE( chatter_DirectMessagingClient_broadcastParticipantLeft );
DEF_LOG_SCOPE( chatter_DirectMessagingClient_broadcastChatClosed );

DirectMessagingClient::DirectMessagingClient( Component & owner, const char * roleName, ChatPrticipantHandler* handlerParticipants )
    : DirectMessagerClientBase  ( roleName, owner )
    , mParticipantsHandler      ( handlerParticipants )
    , mJoinedChat               (false)
{
    ASSERT(mParticipantsHandler != nullptr);
}

void DirectMessagingClient::shutdownChat()
{
    clearAllNotifications();
    if (mJoinedChat)
    {
        mJoinedChat = false;
        requestChatLeave(mParticipantsHandler->GetConnectionOwner(), DateTime::getNow());
    }
}

bool DirectMessagingClient::serviceConnected( NEService::ServiceConnectionState status, ProxyBase & proxy )
{
    LOG_SCOPE( chatter_DirectMessagingClient_ServiceConnected );

    bool result = DirectMessagerClientBase::serviceConnected( status, proxy );
    notifyOnBroadcastChatClosed( isConnected( ) );
    notifyOnBroadcastParticipantJoined( isConnected( ) );
    notifyOnBroadcastParticipantLeft( isConnected( ) );

    if ( isConnected( ) )
    {
        mJoinedChat = true;
        mParticipantsHandler->SetChatClient( this );
        requestChatJoin( mParticipantsHandler->GetConnectionOwner( ), DateTime::getNow( ) );
    }
    else
    {
        shutdownChat();
    }

    return result;
}

void DirectMessagingClient::responseChatJoin( bool succeed, const DirectMessager::ListParticipants & listParticipant, const DateTime & timeConnect, const DateTime & timeConnected )
{
    LOG_SCOPE( chatter_DirectMessagingClient_responseChatJoin );

    postMessage(NEDistributedApp::WindowCommand::CmdChatJoined, succeed ? 1 : 0, 0);
    updateChatOutput( NEDistributedApp::WindowCommand::CmdChatMessage, mParticipantsHandler->GetConnectionOwner( ), succeed ? "Succeeded join chat..." : "Failed join chat...", timeConnect, timeConnected );

    if ( succeed )
    {
        mJoinedChat = true;
        for (uint32_t i = 0; i < listParticipant.getSize(); ++i)
        {
            updateChatOutput( NEDistributedApp::WindowCommand::CmdChatMessage, listParticipant[i], "Is in chat room", DateTime( ), DateTime( ) );
        }
    }
}

void DirectMessagingClient::broadcastMessageSent( const DirectMessager::sParticipant & sender, const String & msgText, const DateTime & timeSent )
{
    LOG_SCOPE( chatter_DirectMessagingClient_broadcastMessageSent );
    updateChatOutput( NEDistributedApp::WindowCommand::CmdChatMessage, sender, msgText, timeSent, DateTime::getNow() );
}

void DirectMessagingClient::broadcastMessageTyped( const DirectMessager::sParticipant & participant, const String & msgText )
{
    LOG_SCOPE( chatter_DirectMessagingClient_broadcastMessageTyped );
    updateChatOutput( NEDistributedApp::WindowCommand::CmdChatTyping, participant, msgText, DateTime( ), DateTime( ) );
}

void DirectMessagingClient::broadcastParticipantJoined( const DirectMessager::sParticipant & participant, const DateTime & timeJoined )
{
    LOG_SCOPE( chatter_DirectMessagingClient_broadcastParticipantJoined );
    if ( participant != mParticipantsHandler->GetConnectionOwner() )
        updateChatOutput( NEDistributedApp::WindowCommand::CmdChatMessage, participant, String( "Joined chat" ), timeJoined, DateTime::getNow() );
}

void DirectMessagingClient::broadcastParticipantLeft( const DirectMessager::sParticipant & participant, const DateTime & timeLeft )
{
    LOG_SCOPE( chatter_DirectMessagingClient_broadcastParticipantLeft );
    updateChatOutput( NEDistributedApp::WindowCommand::CmdChatMessage, participant, String( "Left chat" ), timeLeft, DateTime::getNow() );
}

void DirectMessagingClient::broadcastChatClosed()
{
    LOG_SCOPE( chatter_DirectMessagingClient_broadcastChatClosed );
    updateChatOutput( NEDistributedApp::WindowCommand::CmdChatClosed, DirectMessager::sParticipant(), String( "Chat Closed" ), DateTime(), DateTime() );

    notifyOnBroadcastMessageSent( false );
    notifyOnBroadcastMessageTyped( false );

    notifyOnBroadcastChatClosed(false);
    notifyOnBroadcastParticipantJoined(false);
    notifyOnBroadcastParticipantLeft(false);
}

void DirectMessagingClient::updateChatOutput( const NEDistributedApp::WindowCommand cmdSend, const DirectMessager::sParticipant & participant, const String & msgText, const DateTime & dateStart, const DateTime & dateEnd )
{
    chat::sMessageData * data = chat::newData( );
    if ( data != nullptr )
    {
        String nickName;
        if ( mParticipantsHandler->GetConnectionOwner() == participant )
            nickName = "[ " + participant.nickName + " ]";
        else
            nickName = participant.nickName;
        NEString::copyString<TCHAR, char>( data->nickName, chat::MAXLEN_NICKNAME, nickName.getString( ) );
        NEString::copyString<TCHAR, char>( data->message , chat::MAXLEN_MESSAGE , msgText.getString( )  );
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
