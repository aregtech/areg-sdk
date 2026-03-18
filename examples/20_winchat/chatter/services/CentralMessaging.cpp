/************************************************************************
 * \file            chatter/CentralMessaging.hpp
 * \brief           The connection manager client component implementation
 ************************************************************************/

#include "chatter/res/stdafx.h"
#include "chatter/services/CentralMessaging.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentThread.hpp"
#include "common/ChatDefs.hpp"
#include "examples/20_winchat/services/ConnectionManager.hpp"
#include "chatter/DistributedAppDefs.hpp"
#include "chatter/services/ConnectionHandler.hpp"
#include "chatter/ui/DistributedDialog.hpp"

DEF_LOG_SCOPE( chatter_CentralMessaging_ServiceConnected );
DEF_LOG_SCOPE( chatter_CentralMessaging_broadcastSendMessage );
DEF_LOG_SCOPE( chatter_CentralMessaging_broadcastKeyTyping );
DEF_LOG_SCOPE( chatter_CentralMessaging_broadcastBroadcastMessage );

CentralMessaging::CentralMessaging( const char * roleName, areg::DispatcherThread & ownerThread, aregext::ConnectionHandler & handlerConnection )
    : CentralMessagerConsumerBase   ( roleName, ownerThread )

    , mConnectionHandler( handlerConnection )
    , mReceiveMessages  ( false )
    , mReceiveTyping    ( false )
    , mReceiveBroadcast ( false )
{
}

bool CentralMessaging::service_connected( areg::ServiceConnectionState status, areg::ProxyBase & proxy )
{
    LOG_SCOPE( chatter_CentralMessaging_ServiceConnected );
    bool result = CentralMessagerConsumerBase::service_connected( status, proxy );
    if ( is_connected( ) )
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

void CentralMessaging::broadcastSendMessage( const areg::String & nickName, uint32_t cookie, const areg::String & newMessage, const areg::DateTime & dateTime )
{
    LOG_SCOPE( chatter_CentralMessaging_broadcastSendMessage );
    if ( cookie != mConnectionHandler.GetCookie() )
    {
        ASSERT(nickName != mConnectionHandler.GetNickName());

        chat:: MessageData * data = chat::newData();
        if ( data != nullptr )
        {
            areg::copyString<TCHAR, char>( data->nickName, chat::MAXLEN_NICKNAME, nickName.as_string() );
            areg::copyString<TCHAR, char>( data->message, chat::MAXLEN_MESSAGE, newMessage.as_string( ) );
            data->dataSave      = cookie;
            data->timeReceived  = areg::DateTime::now();
            data->timeSend      = dateTime;

            DistributedDialog::PostServiceMessage(NEDistributedApp::WindowCommand::CmdSendMessage, mConnectionHandler.GetCookie(), reinterpret_cast<LPARAM>(data));
        }
    }
}

void CentralMessaging::broadcastKeyTyping( const areg::String & nickName, uint32_t cookie, const areg::String & newMessage )
{
    LOG_SCOPE( chatter_CentralMessaging_broadcastKeyTyping );
    if ( cookie != mConnectionHandler.GetCookie( ) )
    {
        ASSERT( nickName != mConnectionHandler.GetNickName( ) );

        chat:: MessageData * data = chat::newData( );
        if ( data != nullptr )
        {
            areg::copyString<TCHAR, char>( data->nickName, chat::MAXLEN_NICKNAME, nickName.as_string( ) );
            areg::copyString<TCHAR, char>( data->message, chat::MAXLEN_MESSAGE, newMessage.as_string( ) );
            data->dataSave      = cookie;
            data->timeReceived  = 0;
            data->timeSend      = 0;

            DistributedDialog::PostServiceMessage( NEDistributedApp::WindowCommand::CmdTypeMessage, mConnectionHandler.GetCookie( ), reinterpret_cast<LPARAM>(data) );
        }
    }
}

void CentralMessaging::broadcastBroadcastMessage( const areg::String & serverMessage, const areg::DateTime & dateTime )
{
    LOG_SCOPE( chatter_CentralMessaging_broadcastBroadcastMessage );

    chat:: MessageData * data = chat::newData( );
    if ( data != nullptr )
    {
        areg::copyString<TCHAR, TCHAR>( data->nickName, chat::MAXLEN_NICKNAME, chat::SERVER_NAME );
        areg::copyString<TCHAR, char>( data->message, chat::MAXLEN_MESSAGE, serverMessage.as_string( ) );
        data->dataSave      = static_cast<uint64_t>(-1);
        data->timeReceived  = areg::DateTime::now();
        data->timeSend      = dateTime;

        DistributedDialog::PostServiceMessage( NEDistributedApp::WindowCommand::CmdSendMessage, mConnectionHandler.GetCookie( ), reinterpret_cast<LPARAM>(data) );
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
