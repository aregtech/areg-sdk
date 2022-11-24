/************************************************************************
 * \file            chatter/services/DirectMessagingClient.cpp
 * \brief           The messaging service client object
 ************************************************************************/

#include "chatter/services/DirectMessagingClient.hpp"
#include "chatter/services/ChatPrticipantHandler.hpp"
#include "generated/NECommon.hpp"
#include "areg/trace/GETrace.h"

#include <Windows.h>

#define FIRST_MESSAGE       (WM_USER + 10 + static_cast<unsigned int>(NEDistributedApp::eWndCommands::CmdFirst))
#define MAKE_MESSAGE(elem)  (static_cast<unsigned int>(elem) + FIRST_MESSAGE)

DEF_TRACE_SCOPE( distrbutedapp_DirectMessagingClient_ServiceConnected );
DEF_TRACE_SCOPE( distrbutedapp_DirectMessagingClient_responseChatJoin );
DEF_TRACE_SCOPE( distrbutedapp_DirectMessagingClient_broadcastMessageSent );
DEF_TRACE_SCOPE( distrbutedapp_DirectMessagingClient_broadcastMessageTyped );
DEF_TRACE_SCOPE( distrbutedapp_DirectMessagingClient_broadcastParticipantJoined );
DEF_TRACE_SCOPE( distrbutedapp_DirectMessagingClient_broadcastParticipantLeft );
DEF_TRACE_SCOPE( distrbutedapp_DirectMessagingClient_broadcastChatClosed );

DirectMessagingClient::DirectMessagingClient( Component & owner, const char * roleName, ChatPrticipantHandler & handlerParticipants )
    : DirectMessagerClientBase    ( roleName, owner )
    , mParticipantsHandler          ( handlerParticipants )
{
}

DirectMessagingClient::~DirectMessagingClient( )
{
}

bool DirectMessagingClient::serviceConnected( bool isConnected, ProxyBase & proxy )
{
    TRACE_SCOPE( distrbutedapp_DirectMessagingClient_ServiceConnected );

    bool result = false;
    if ( isConnected )
    {
        if ( DirectMessagerClientBase::serviceConnected( isConnected, proxy ) )
        {
            result = true;
            mParticipantsHandler.SetChatClient( this );
            
            notifyOnBroadcastChatClosed(true);
            notifyOnBroadcastParticipantJoined(true);
            notifyOnBroadcastParticipantLeft(true);

            requestChatJoin( mParticipantsHandler.GetConnectionOwner(), DateTime::getNow() );
        }
    }
    else
    {
        requestChatLeave( mParticipantsHandler.GetConnectionOwner(), DateTime::getNow() );
        result = DirectMessagerClientBase::serviceConnected( isConnected, proxy );
    }

    return result;
}

void DirectMessagingClient::responseChatJoin( bool succeed, const NEDirectMessager::ListParticipants & listParticipant, const DateTime & timeConnect, const DateTime & timeConnected )
{
    TRACE_SCOPE( distrbutedapp_DirectMessagingClient_responseChatJoin );

    postMessage(NEDistributedApp::eWndCommands::CmdChatJoined, succeed ? 1 : 0, 0);
    updateChatOutput( NEDistributedApp::eWndCommands::CmdChatMessage, mParticipantsHandler.GetConnectionOwner( ), succeed ? "Succeeded join chat..." : "Failed join chat...", timeConnect, timeConnected );

    if ( succeed )
    {
        for (uint32_t i = 0; i < listParticipant.getSize(); ++i)
        {
            updateChatOutput( NEDistributedApp::eWndCommands::CmdChatMessage, listParticipant[i], "Is in chat room", DateTime( ), DateTime( ) );
        }
    }
}

void DirectMessagingClient::broadcastMessageSent( const NEDirectMessager::sParticipant & sender, const String & msgText, const DateTime & timeSent )
{
    TRACE_SCOPE( distrbutedapp_DirectMessagingClient_broadcastMessageSent );
    updateChatOutput( NEDistributedApp::eWndCommands::CmdChatMessage, sender, msgText, timeSent, DateTime::getNow() );
}

void DirectMessagingClient::broadcastMessageTyped( const NEDirectMessager::sParticipant & participant, const String & msgText )
{
    TRACE_SCOPE( distrbutedapp_DirectMessagingClient_broadcastMessageTyped );
    updateChatOutput( NEDistributedApp::eWndCommands::CmdChatTyping, participant, msgText, DateTime( ), DateTime( ) );
}

void DirectMessagingClient::broadcastParticipantJoined( const NEDirectMessager::sParticipant & participant, const DateTime & timeJoined )
{
    TRACE_SCOPE( distrbutedapp_DirectMessagingClient_broadcastParticipantJoined );
    if ( participant != mParticipantsHandler.GetConnectionOwner() )
        updateChatOutput( NEDistributedApp::eWndCommands::CmdChatMessage, participant, String( "Joined chat" ), timeJoined, DateTime::getNow() );
}

void DirectMessagingClient::broadcastParticipantLeft( const NEDirectMessager::sParticipant & participant, const DateTime & timeLeft )
{
    TRACE_SCOPE( distrbutedapp_DirectMessagingClient_broadcastParticipantLeft );
    updateChatOutput( NEDistributedApp::eWndCommands::CmdChatMessage, participant, String( "Left chat" ), timeLeft, DateTime::getNow() );
}

void DirectMessagingClient::broadcastChatClosed( void )
{
    TRACE_SCOPE( distrbutedapp_DirectMessagingClient_broadcastChatClosed );
    updateChatOutput( NEDistributedApp::eWndCommands::CmdChatClosed, NEDirectMessager::sParticipant(), String( "Chat Closed" ), DateTime(), DateTime() );

    notifyOnBroadcastMessageSent( false );
    notifyOnBroadcastMessageTyped( false );

    notifyOnBroadcastChatClosed(false);
    notifyOnBroadcastParticipantJoined(false);
    notifyOnBroadcastParticipantLeft(false);
}

void DirectMessagingClient::updateChatOutput( const NEDistributedApp::eWndCommands cmdSend, const NEDirectMessager::sParticipant & participant, const String & msgText, const DateTime & dateStart, const DateTime & dateEnd )
{
    NECommon::sMessageData * data = NECommon::newData( );
    if ( data != nullptr )
    {
        String nickName;
        if ( mParticipantsHandler.GetConnectionOwner() == participant )
            nickName = "[ " + participant.nickName + " ]";
        else
            nickName = participant.nickName;
        NEString::copyString<TCHAR, char>( data->nickName, NECommon::MAXLEN_NICKNAME, nickName.getString( ) );
        NEString::copyString<TCHAR, char>( data->message , NECommon::MAXLEN_MESSAGE , msgText.getString( )  );
        data->timeSend      = dateStart;
        data->timeReceived  = dateEnd;
        data->dataSave      = participant.cookie;

        postMessage(cmdSend, static_cast<ptr_type>(participant.sessionId), reinterpret_cast<ptr_type>(data));
    }
}

inline void DirectMessagingClient::postMessage(NEDistributedApp::eWndCommands cmdSend, ptr_type wParam, ptr_type lParam)
{
    HWND hWnd = reinterpret_cast<HWND>(mParticipantsHandler.GetChatWindow());
    ASSERT(hWnd != nullptr);

    ::PostMessage(hWnd, MAKE_MESSAGE(cmdSend), static_cast<WPARAM>(wParam), static_cast<LPARAM>(lParam));
}
