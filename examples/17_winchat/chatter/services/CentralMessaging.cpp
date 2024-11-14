/************************************************************************
 * \file            chatter/CentralMessaging.hpp
 * \brief           The connection manager client component implementation
 ************************************************************************/

#include "chatter/res/stdafx.h"
#include "chatter/services/CentralMessaging.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentThread.hpp"
#include "common/NECommon.hpp"
#include "examples/17_winchat/services/NEConnectionManager.hpp"
#include "chatter/NEDistributedApp.hpp"
#include "chatter/services/ConnectionHandler.hpp"
#include "chatter/ui/DistributedDialog.hpp"

DEF_LOG_SCOPE( chatter_CentralMessaging_ServiceConnected );
DEF_LOG_SCOPE( chatter_CentralMessaging_broadcastSendMessage );
DEF_LOG_SCOPE( chatter_CentralMessaging_broadcastKeyTyping );
DEF_LOG_SCOPE( chatter_CentralMessaging_broadcastBroadcastMessage );

CentralMessaging::CentralMessaging( const char * roleName, DispatcherThread & ownerThread, ConnectionHandler & handlerConnection )
    : CentralMessagerClientBase   ( roleName, ownerThread )

    , mConnectionHandler( handlerConnection )
    , mReceiveMessages  ( false )
    , mReceiveTyping    ( false )
    , mReceiveBroadcast ( false )
{
}

bool CentralMessaging::serviceConnected( NEService::eServiceConnection status, ProxyBase & proxy )
{
    LOG_SCOPE( chatter_CentralMessaging_ServiceConnected );
    bool result = CentralMessagerClientBase::serviceConnected( status, proxy );
    if ( isConnected( ) )
    {
        notifyOnBroadcastSendMessage( mReceiveMessages );
        notifyOnBroadcastKeyTyping( mReceiveTyping );
        notifyOnBroadcastBroadcastMessage( mReceiveBroadcast );
    }
    else
    {
        notifyOnBroadcastSendMessage( false );
        notifyOnBroadcastKeyTyping( false );
        notifyOnBroadcastBroadcastMessage( false );
    }

    return result;
}

void CentralMessaging::broadcastSendMessage( const String & nickName, unsigned int cookie, const String & newMessage, const DateTime & dateTime )
{
    LOG_SCOPE( chatter_CentralMessaging_broadcastSendMessage );
    if ( cookie != mConnectionHandler.GetCookie() )
    {
        ASSERT(nickName != mConnectionHandler.GetNickName());

        NECommon::sMessageData * data = NECommon::newData();
        if ( data != nullptr )
        {
            NEString::copyString<TCHAR, char>( data->nickName, NECommon::MAXLEN_NICKNAME, nickName.getString() );
            NEString::copyString<TCHAR, char>( data->message, NECommon::MAXLEN_MESSAGE, newMessage.getString( ) );
            data->dataSave      = cookie;
            data->timeReceived  = DateTime::getNow();
            data->timeSend      = dateTime;

            DistributedDialog::PostServiceMessage(NEDistributedApp::eWndCommands::CmdSendMessage, mConnectionHandler.GetCookie(), reinterpret_cast<LPARAM>(data));
        }
    }
}

void CentralMessaging::broadcastKeyTyping( const String & nickName, unsigned int cookie, const String & newMessage )
{
    LOG_SCOPE( chatter_CentralMessaging_broadcastKeyTyping );
    if ( cookie != mConnectionHandler.GetCookie( ) )
    {
        ASSERT( nickName != mConnectionHandler.GetNickName( ) );

        NECommon::sMessageData * data = NECommon::newData( );
        if ( data != nullptr )
        {
            NEString::copyString<TCHAR, char>( data->nickName, NECommon::MAXLEN_NICKNAME, nickName.getString( ) );
            NEString::copyString<TCHAR, char>( data->message, NECommon::MAXLEN_MESSAGE, newMessage.getString( ) );
            data->dataSave      = cookie;
            data->timeReceived  = 0;
            data->timeSend      = 0;

            DistributedDialog::PostServiceMessage( NEDistributedApp::eWndCommands::CmdTypeMessage, mConnectionHandler.GetCookie( ), reinterpret_cast<LPARAM>(data) );
        }
    }
}

void CentralMessaging::broadcastBroadcastMessage( const String & serverMessage, const DateTime & dateTime )
{
    LOG_SCOPE( chatter_CentralMessaging_broadcastBroadcastMessage );

    NECommon::sMessageData * data = NECommon::newData( );
    if ( data != nullptr )
    {
        NEString::copyString<TCHAR, TCHAR>( data->nickName, NECommon::MAXLEN_NICKNAME, NECommon::SERVER_NAME );
        NEString::copyString<TCHAR, char>( data->message, NECommon::MAXLEN_MESSAGE, serverMessage.getString( ) );

        data->dataSave      = static_cast<uint64_t>(-1);
        data->timeReceived  = DateTime::getNow();
        data->timeSend      = dateTime;

        DistributedDialog::PostServiceMessage( NEDistributedApp::eWndCommands::CmdSendMessage, mConnectionHandler.GetCookie( ), reinterpret_cast<LPARAM>(data) );
    }
}

void CentralMessaging::ReceiveMessages( bool doReceive )
{
    mReceiveMessages = doReceive;
    notifyOnBroadcastSendMessage(doReceive);
}

void CentralMessaging::ReceiveKeytype( bool doReceive )
{
    mReceiveTyping = doReceive;
    notifyOnBroadcastKeyTyping(doReceive);
}

void CentralMessaging::ReceiveBroadcasting( bool doReceive )
{
    mReceiveBroadcast = doReceive;
    notifyOnBroadcastBroadcastMessage(doReceive);
}
