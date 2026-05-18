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

DEF_LOG_SCOPE(chatter_CentralMessaging, ServiceConnected);
DEF_LOG_SCOPE(chatter_CentralMessaging, broadcast_send_message);
DEF_LOG_SCOPE(chatter_CentralMessaging, broadcast_key_typing);
DEF_LOG_SCOPE(chatter_CentralMessaging, broadcast_broadcast_message);

CentralMessaging::CentralMessaging( const char * roleName, areg::DispatcherThread & ownerThread, ConnectionHandler & handlerConnection )
    : CentralMessagerConsumerBase   ( roleName, ownerThread )

    , mConnectionHandler( handlerConnection )
    , mReceiveMessages  ( false )
    , mReceiveTyping    ( false )
    , mReceiveBroadcast ( false )
{
}

bool CentralMessaging::service_connected( areg::ServiceConnectionState status, areg::ProxyBase & proxy )
{
    LOG_SCOPE( chatter_CentralMessaging, ServiceConnected );
    bool result = CentralMessagerConsumerBase::service_connected( status, proxy );
    if ( is_connected( ) )
    {
        notify_on_broadcast_send_message( mReceiveMessages );
        notify_on_broadcast_key_typing( mReceiveTyping );
        notify_on_broadcast_broadcast_message( mReceiveBroadcast );
    }
    else
    {
        notify_on_broadcast_send_message( false );
        notify_on_broadcast_key_typing( false );
        notify_on_broadcast_broadcast_message( false );
    }

    return result;
}

void CentralMessaging::broadcast_send_message( const areg::String & nickName, uint32_t cookie, const areg::String & newMessage, const areg::DateTime & dateTime )
{
    LOG_SCOPE( chatter_CentralMessaging, broadcast_send_message );
    if ( cookie != mConnectionHandler.GetCookie() )
    {
        ASSERT(nickName != mConnectionHandler.GetNickName());

        chat:: MessageData * data = chat::newData();
        if ( data != nullptr )
        {
            areg::copy_string<TCHAR, char>( data->nickName, chat::MAXLEN_NICKNAME, nickName.as_string() );
            areg::copy_string<TCHAR, char>( data->message, chat::MAXLEN_MESSAGE, newMessage.as_string( ) );
            data->dataSave      = cookie;
            data->timeReceived  = areg::DateTime::now();
            data->timeSend      = dateTime;

            DistributedDialog::PostServiceMessage(NEDistributedApp::WindowCommand::CmdSendMessage, mConnectionHandler.GetCookie(), reinterpret_cast<LPARAM>(data));
        }
    }
}

void CentralMessaging::broadcast_key_typing( const areg::String & nickName, uint32_t cookie, const areg::String & newMessage )
{
    LOG_SCOPE( chatter_CentralMessaging, broadcast_key_typing );
    if ( cookie != mConnectionHandler.GetCookie( ) )
    {
        ASSERT( nickName != mConnectionHandler.GetNickName( ) );

        chat:: MessageData * data = chat::newData( );
        if ( data != nullptr )
        {
            areg::copy_string<TCHAR, char>( data->nickName, chat::MAXLEN_NICKNAME, nickName.as_string( ) );
            areg::copy_string<TCHAR, char>( data->message, chat::MAXLEN_MESSAGE, newMessage.as_string( ) );
            data->dataSave      = cookie;
            data->timeReceived  = 0;
            data->timeSend      = 0;

            DistributedDialog::PostServiceMessage( NEDistributedApp::WindowCommand::CmdTypeMessage, mConnectionHandler.GetCookie( ), reinterpret_cast<LPARAM>(data) );
        }
    }
}

void CentralMessaging::broadcast_broadcast_message( const areg::String & serverMessage, const areg::DateTime & dateTime )
{
    LOG_SCOPE( chatter_CentralMessaging, broadcast_broadcast_message );

    chat:: MessageData * data = chat::newData( );
    if ( data != nullptr )
    {
        areg::copy_string<TCHAR, TCHAR>( data->nickName, chat::MAXLEN_NICKNAME, chat::SERVER_NAME );
        areg::copy_string<TCHAR, char>( data->message, chat::MAXLEN_MESSAGE, serverMessage.as_string( ) );
        data->dataSave      = static_cast<uint64_t>(-1);
        data->timeReceived  = areg::DateTime::now();
        data->timeSend      = dateTime;

        DistributedDialog::PostServiceMessage( NEDistributedApp::WindowCommand::CmdSendMessage, mConnectionHandler.GetCookie( ), reinterpret_cast<LPARAM>(data) );
    }
}

void CentralMessaging::ReceiveMessages( bool doReceive )
{
    mReceiveMessages = doReceive;
    notify_on_broadcast_send_message(doReceive);
}

void CentralMessaging::ReceiveKeytype( bool doReceive )
{
    mReceiveTyping = doReceive;
    notify_on_broadcast_key_typing(doReceive);
}

void CentralMessaging::ReceiveBroadcasting( bool doReceive )
{
    mReceiveBroadcast = doReceive;
    notify_on_broadcast_broadcast_message(doReceive);
}
