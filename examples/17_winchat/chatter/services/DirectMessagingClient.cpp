/************************************************************************
 * \file            chatter/services/DirectMessagingClient.cpp
 * \brief           The messaging service client object
 ************************************************************************/

#include "chatter/services/DirectMessagingClient.hpp"
#include "chatter/services/ChatPrticipantHandler.hpp"
#include "generated/NECommon.hpp"
#include "areg/trace/GETrace.h"

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

    ::PostMessage(mParticipantsHandler.GetChatWindow(), NEDistributedApp::CmdChatJoined, succeed ? 1 : 0, 0);
    updateChatOutput( NEDistributedApp::CmdChatMessage, mParticipantsHandler.GetConnectionOwner( ), succeed ? "Succeeded join chat..." : "Failed join chat...", timeConnect, timeConnected );

    if ( succeed )
    {
        for ( int i = 0; i < listParticipant.getSize( ); ++ i )
            updateChatOutput( NEDistributedApp::CmdChatMessage, listParticipant[i], "Is in chat room", DateTime( ), DateTime( ) );
    }
}

void DirectMessagingClient::broadcastMessageSent( const NEDirectMessager::sParticipant & sender, const String & msgText, const DateTime & timeSent )
{
    TRACE_SCOPE( distrbutedapp_DirectMessagingClient_broadcastMessageSent );
    updateChatOutput( NEDistributedApp::CmdChatMessage, sender, msgText, timeSent, DateTime::getNow() );
}

void DirectMessagingClient::broadcastMessageTyped( const NEDirectMessager::sParticipant & participant, const String & msgText )
{
    TRACE_SCOPE( distrbutedapp_DirectMessagingClient_broadcastMessageTyped );
    updateChatOutput( NEDistributedApp::CmdChatTyping, participant, msgText, DateTime( ), DateTime( ) );
}

void DirectMessagingClient::broadcastParticipantJoined( const NEDirectMessager::sParticipant & participant, const DateTime & timeJoined )
{
    TRACE_SCOPE( distrbutedapp_DirectMessagingClient_broadcastParticipantJoined );
    if ( participant != mParticipantsHandler.GetConnectionOwner() )
        updateChatOutput( NEDistributedApp::CmdChatMessage, participant, String( "Joined chat" ), timeJoined, DateTime::getNow() );
}

void DirectMessagingClient::broadcastParticipantLeft( const NEDirectMessager::sParticipant & participant, const DateTime & timeLeft )
{
    TRACE_SCOPE( distrbutedapp_DirectMessagingClient_broadcastParticipantLeft );
    updateChatOutput( NEDistributedApp::CmdChatMessage, participant, String( "Left chat" ), timeLeft, DateTime::getNow() );
}

void DirectMessagingClient::broadcastChatClosed( void )
{
    TRACE_SCOPE( distrbutedapp_DirectMessagingClient_broadcastChatClosed );
    updateChatOutput( NEDistributedApp::CmdChatClosed, NEDirectMessager::sParticipant(), String( "Chat Closed" ), DateTime(), DateTime() );

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

        HWND hWnd = mParticipantsHandler.GetChatWindow( );
        ::PostMessage( hWnd, static_cast<UINT>(cmdSend), static_cast<WPARAM>(participant.sessionId), reinterpret_cast<LPARAM>(data) );
    }
}
