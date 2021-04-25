/************************************************************************
 * \file            edge/services/CEDirectMessagingClient.cpp
 * \brief           The messaging service client object
 ************************************************************************/

#include "edge/services/CEDirectMessagingClient.hpp"
#include "edge/services/CEChatPrticipantHandler.hpp"
#include "shared/NECommonSettings.hpp"
#include "areg/trace/GETrace.h"

DEF_TRACE_SCOPE( distrbutedapp_CEDirectMessagingClient_ServiceConnected );
DEF_TRACE_SCOPE( distrbutedapp_CEDirectMessagingClient_ResponseChatJoin );
DEF_TRACE_SCOPE( distrbutedapp_CEDirectMessagingClient_BroadcastMessageSent );
DEF_TRACE_SCOPE( distrbutedapp_CEDirectMessagingClient_BroadcastMessageTyped );
DEF_TRACE_SCOPE( distrbutedapp_CEDirectMessagingClient_BroadcastParticipantJoined );
DEF_TRACE_SCOPE( distrbutedapp_CEDirectMessagingClient_BroadcastParticipantLeft );
DEF_TRACE_SCOPE( distrbutedapp_CEDirectMessagingClient_BroadcastChatClosed );

CEDirectMessagingClient::CEDirectMessagingClient( CEComponent & owner, const char * roleName, CEChatPrticipantHandler & handlerParticipants )
    : CEDirectMessagerClientBase    ( roleName, owner )
    , mParticipantsHandler          ( handlerParticipants )
{
}

CEDirectMessagingClient::~CEDirectMessagingClient( )
{
}

bool CEDirectMessagingClient::ServiceConnected( const bool isConnected, CEProxyBase & proxy )
{
    TRACE_SCOPE( distrbutedapp_CEDirectMessagingClient_ServiceConnected );

    bool result = false;
    if ( isConnected )
    {
        if ( CEDirectMessagerClientBase::ServiceConnected( isConnected, proxy ) )
        {
            result = true;
            mParticipantsHandler.SetChatClient( this );
            
            NotifyOnBroadcastChatClosed(true);
            NotifyOnBroadcastParticipantJoined(true);
            NotifyOnBroadcastParticipantLeft(true);

            RequestChatJoin( mParticipantsHandler.GetConnectionOwner(), CEDateTime::GetNow(false) );
        }
    }
    else
    {
        RequestChatLeave( mParticipantsHandler.GetConnectionOwner(), CEDateTime::GetNow(false) );
        result = CEDirectMessagerClientBase::ServiceConnected( isConnected, proxy );
    }
    return result;
}

void CEDirectMessagingClient::ResponseChatJoin( const bool & succeed, const NEDirectMessager::ListParticipants & listParticipant, const CEDateTime & timeConnect, const CEDateTime & timeConnected )
{
    TRACE_SCOPE( distrbutedapp_CEDirectMessagingClient_ResponseChatJoin );

    ::PostMessage(mParticipantsHandler.GetChatWindow(), NEDistributedApp::CmdChatJoined, succeed ? 1 : 0, 0);
    updateChatOutput( NEDistributedApp::CmdChatMessage, mParticipantsHandler.GetConnectionOwner( ), succeed ? "Succeeded join chat..." : "Failed join chat...", timeConnect, timeConnected );

    if ( succeed )
    {
        for ( int i = 0; i < listParticipant.GetSize( ); ++ i )
            updateChatOutput( NEDistributedApp::CmdChatMessage, listParticipant[i], "Is in chat room", CEDateTime( ), CEDateTime( ) );
    }
}

void CEDirectMessagingClient::BroadcastMessageSent( const NEDirectMessager::sParticipant & sender, const CEString & msgText, const CEDateTime & timeSent )
{
    TRACE_SCOPE( distrbutedapp_CEDirectMessagingClient_BroadcastMessageSent );
    updateChatOutput( NEDistributedApp::CmdChatMessage, sender, msgText, timeSent, CEDateTime::GetNow( false ) );
}

void CEDirectMessagingClient::BroadcastMessageTyped( const NEDirectMessager::sParticipant & participant, const CEString & msgText )
{
    TRACE_SCOPE( distrbutedapp_CEDirectMessagingClient_BroadcastMessageTyped );
    updateChatOutput( NEDistributedApp::CmdChatTyping, participant, msgText, CEDateTime( ), CEDateTime( ) );
}

void CEDirectMessagingClient::BroadcastParticipantJoined( const NEDirectMessager::sParticipant & participant, const CEDateTime & timeJoined )
{
    TRACE_SCOPE( distrbutedapp_CEDirectMessagingClient_BroadcastParticipantJoined );
    if ( participant != mParticipantsHandler.GetConnectionOwner() )
        updateChatOutput( NEDistributedApp::CmdChatMessage, participant, CEString( "Joined chat" ), timeJoined, CEDateTime::GetNow( false ) );
}

void CEDirectMessagingClient::BroadcastParticipantLeft( const NEDirectMessager::sParticipant & participant, const CEDateTime & timeLeft )
{
    TRACE_SCOPE( distrbutedapp_CEDirectMessagingClient_BroadcastParticipantLeft );
    updateChatOutput( NEDistributedApp::CmdChatMessage, participant, CEString( "Left chat" ), timeLeft, CEDateTime::GetNow(false) );
}

void CEDirectMessagingClient::BroadcastChatClosed( void )
{
    TRACE_SCOPE( distrbutedapp_CEDirectMessagingClient_BroadcastChatClosed );
    updateChatOutput( NEDistributedApp::CmdChatClosed, NEDirectMessager::sParticipant(), CEString( "Chat Closed" ), CEDateTime(), CEDateTime() );

    NotifyOnBroadcastMessageSent( false );
    NotifyOnBroadcastMessageTyped( false );

    NotifyOnBroadcastChatClosed(false);
    NotifyOnBroadcastParticipantJoined(false);
    NotifyOnBroadcastParticipantLeft(false);
}

void CEDirectMessagingClient::updateChatOutput( const NEDistributedApp::eWndCommands cmdSend, const NEDirectMessager::sParticipant & participant, const CEString & msgText, const CEDateTime & dateStart, const CEDateTime & dateEnd )
{
    NECommonSettings::sMessageData * data = new NECommonSettings::sMessageData;
    if ( data != NULL )
    {
        CEString nickName;
        if ( mParticipantsHandler.GetConnectionOwner() == participant )
            nickName = "[ " + participant.nickName + " ]";
        else
            nickName = participant.nickName;
        NEString::copyString<TCHAR, char>( data->nickName, NECommonSettings::MAXLEN_NICKNAME, nickName.String( ) );
        NEString::copyString<TCHAR, char>( data->message , NECommonSettings::MAXLEN_MESSAGE , msgText.String( )  );
        data->timeSend      = dateStart;
        data->timeReceived  = dateEnd;
        data->dataSave      = participant.cookie;

        HWND hWnd = mParticipantsHandler.GetChatWindow( );
        ::PostMessage( hWnd, static_cast<UINT>(cmdSend), static_cast<WPARAM>(participant.sessionId), reinterpret_cast<LPARAM>(data) );
    }
}
