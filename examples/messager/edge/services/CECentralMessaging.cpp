/************************************************************************
 * \file            edge/CECentralMessaging.hpp
 * \brief           The connection manager client component implementation
 ************************************************************************/

#include "edge/res/stdafx.h"
#include "edge/services/CECentralMessaging.hpp"
#include "areg/src/component/CEComponent.hpp"
#include "areg/src/component/CEComponentThread.hpp"
#include "shared/NECommonSettings.hpp"
#include "shared/generated/NEConnectionManager.hpp"
#include "edge/NEDistributedApp.hpp"
#include "edge/services/CEConnectionHandler.hpp"
#include "edge/ui/CEDistributedDialog.hpp"

DEF_TRACE_SCOPE( distrbutedapp_CECentralMessaging_ServiceConnected );
DEF_TRACE_SCOPE( distrbutedapp_CECentralMessaging_BroadcastSendMessage );
DEF_TRACE_SCOPE( distrbutedapp_CECentralMessaging_BroadcastKeyTyping );
DEF_TRACE_SCOPE( distrbutedapp_CECentralMessaging_BroadcastBroadcastMessage );

CECentralMessaging::CECentralMessaging( const char * roleName, CEDispatcherThread & ownerThread, CEConnectionHandler & handlerConnection )
    : CECentralMessagerClientBase   ( roleName, ownerThread )

    , mConnectionHandler            ( handlerConnection )
    , mReceiveMessages              ( false )
    , mReceiveTyping                ( false )
    , mReceiveBroadcast             ( false )
{
}

CECentralMessaging::~CECentralMessaging( void )
{
}

bool CECentralMessaging::ServiceConnected( bool isConnected, CEProxyBase & proxy )
{
    TRACE_SCOPE( distrbutedapp_CECentralMessaging_ServiceConnected );
    bool result = false;
    if ( isConnected == false )
    {
        NotifyOnBroadcastSendMessage(false);
        NotifyOnBroadcastKeyTyping(false);
        NotifyOnBroadcastBroadcastMessage(false);
        result = CECentralMessagerClientBase::ServiceConnected( false, proxy );
    }
    else
    {
        result = CECentralMessagerClientBase::ServiceConnected( true, proxy );
        NotifyOnBroadcastSendMessage( mReceiveMessages );
        NotifyOnBroadcastKeyTyping( mReceiveTyping );
        NotifyOnBroadcastBroadcastMessage( mReceiveBroadcast );
    }
    return result;
}

void CECentralMessaging::BroadcastSendMessage( const CEString & nickName, const uint32_t & cookie, const CEString & newMessage, const CEDateTime & dateTime )
{
    TRACE_SCOPE( distrbutedapp_CECentralMessaging_BroadcastSendMessage );
    if ( cookie != mConnectionHandler.GetCookie() )
    {
        ASSERT(nickName != mConnectionHandler.GetNickName());

        NECommonSettings::sMessageData * data = DEBUG_NEW NECommonSettings::sMessageData;
        if ( data != NULL )
        {
            NEString::copyString<TCHAR, char>( data->nickName, NECommonSettings::MAXLEN_NICKNAME, nickName.String() );
            NEString::copyString<TCHAR, char>( data->message, NECommonSettings::MAXLEN_MESSAGE, newMessage.String( ) );
            data->dataSave      = cookie;
            data->timeReceived  = CEDateTime::GetNow();
            data->timeSend      = dateTime;

            CEDistributedDialog::PostServiceMessage(NEDistributedApp::CmdSendMessage, mConnectionHandler.GetCookie(), reinterpret_cast<LPARAM>(data));
        }
    }
}

void CECentralMessaging::BroadcastKeyTyping( const CEString & nickName, const uint32_t & cookie, const CEString & newMessage )
{
    TRACE_SCOPE( distrbutedapp_CECentralMessaging_BroadcastKeyTyping );
    if ( cookie != mConnectionHandler.GetCookie( ) )
    {
        ASSERT( nickName != mConnectionHandler.GetNickName( ) );

        NECommonSettings::sMessageData * data = DEBUG_NEW NECommonSettings::sMessageData;
        if ( data != NULL )
        {
            NEString::copyString<TCHAR, char>( data->nickName, NECommonSettings::MAXLEN_NICKNAME, nickName.String( ) );
            NEString::copyString<TCHAR, char>( data->message, NECommonSettings::MAXLEN_MESSAGE, newMessage.String( ) );
            data->dataSave      = cookie;
            data->timeReceived  = 0;
            data->timeSend      = 0;

            CEDistributedDialog::PostServiceMessage( NEDistributedApp::CmdTypeMessage, mConnectionHandler.GetCookie( ), reinterpret_cast<LPARAM>(data) );
        }
    }
}

void CECentralMessaging::BroadcastBroadcastMessage( const CEString & serverMessage, const CEDateTime & dateTime )
{
    TRACE_SCOPE( distrbutedapp_CECentralMessaging_BroadcastBroadcastMessage );

    NECommonSettings::sMessageData * data = DEBUG_NEW NECommonSettings::sMessageData;
    if ( data != NULL )
    {
        NEString::copyString<TCHAR, TCHAR>( data->nickName, NECommonSettings::MAXLEN_NICKNAME, NECommonSettings::SERVER_NAME );
        NEString::copyString<TCHAR, char>( data->message, NECommonSettings::MAXLEN_MESSAGE, serverMessage.String( ) );

        data->dataSave      = -1;
        data->timeReceived  = CEDateTime::GetNow();
        data->timeSend      = dateTime;

        CEDistributedDialog::PostServiceMessage( NEDistributedApp::CmdSendMessage, mConnectionHandler.GetCookie( ), reinterpret_cast<LPARAM>(data) );
    }
}

void CECentralMessaging::ReceiveMessages( bool doReceive )
{
    mReceiveMessages = doReceive;
    NotifyOnBroadcastSendMessage(doReceive);
}

void CECentralMessaging::ReceiveKeytype( bool doReceive )
{
    mReceiveTyping = doReceive;
    NotifyOnBroadcastKeyTyping(doReceive);
}

void CECentralMessaging::ReceiveBroadcasting( bool doReceive )
{
    mReceiveBroadcast = doReceive;
    NotifyOnBroadcastBroadcastMessage(doReceive);
}
