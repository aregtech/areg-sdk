/************************************************************************
 * \file            register/services/CEConnectionManager.cpp
 * \brief           The connection manager server component implementation
 ************************************************************************/

#include "register/res/stdafx.h"
#include "register/services/CEConnectionManager.hpp"
#include "register/ui/CECentralDialog.hpp"
#include "register/NECentralApp.hpp"
#include "register/CECentralApp.hpp"
#include "shared/NECommonSettings.hpp"
#include "areg/component/NERegistry.hpp"
#include "areg/component/CEComponentLoader.hpp"

#define MAKE_HWND(wnd)      reinterpret_cast<HWND>(wnd)

DEF_TRACE_SCOPE( centralapp_CEConnectionManager_CreateComponent );
DEF_TRACE_SCOPE( centralapp_CEConnectionManager_DeleteComponent );
DEF_TRACE_SCOPE( centralapp_CEConnectionManager_StartupServiceInterface );
DEF_TRACE_SCOPE( centralapp_CEConnectionManager_RequestConnet );
DEF_TRACE_SCOPE( centralapp_CEConnectionManager_RequestRegisterConnection );
DEF_TRACE_SCOPE( centralapp_CEConnectionManager_RequestDiconnect );
DEF_TRACE_SCOPE( centralapp_CEConnectionManager_RequestSendMessage );
DEF_TRACE_SCOPE( centralapp_CEConnectionManager_RequestKeyTyping );

BEGIN_MODEL(NECommonSettings::MODEL_NAME_CENTRAL_SERVER)

    BEGIN_REGISTER_THREAD( NECentralApp::THREAD_CENTRAL )
        BEGIN_REGISTER_COMPONENT( NECommonSettings::COMP_NAME_CENTRAL_SERVER, CEConnectionManager )
            REGISTER_IMPLEMENT_SERVICE( NEConnectionManager::ServiceName, 1, 0, 0 )
            REGISTER_IMPLEMENT_SERVICE( NECentralMessager::ServiceName, 1, 0, 0 )
        END_REGISTER_COMPONENT( NECommonSettings::COMP_NAME_CENTRAL_SERVER )
    END_REGISTER_THREAD( NECentralApp::THREAD_CENTRAL )

END_MODEL(NECommonSettings::MODEL_NAME_CENTRAL_SERVER)

CEConnectionManager *   CEConnectionManager::sService   = NULL;

CEComponent * CEConnectionManager::CreateComponent( const NERegistry::CEComponentEntry & entry, CEComponentThread & owner )
{
    TRACE_SCOPE( centralapp_CEConnectionManager_CreateComponent );
    return new CEConnectionManager( owner, entry.mRoleName.GetBuffer(), entry.GetComponentData() );
}

void CEConnectionManager::DeleteComponent( CEComponent & compObject, const NERegistry::CEComponentEntry & entry )
{
    TRACE_SCOPE( centralapp_CEConnectionManager_DeleteComponent );
    delete (&compObject);
}

CEConnectionManager * CEConnectionManager::GetService( void )
{
    return CEConnectionManager::sService;
}

CEConnectionManager::CEConnectionManager( CEComponentThread & masterThread, const char * const roleName, NEMemory::uAlign data )
    : CEComponent               ( masterThread, roleName )
    , CEConnectionManagerStub   ( static_cast<CEComponent &>(self()) )
    , CECentralMessagerStub     ( static_cast<CEComponent &>(self()) )

    , mWnd                      ( data.alignInt64.mElement )
    , mCookies                  ( NEConnectionManager::InvalidCookie )
{
    CEConnectionManager::sService   = this;
}

CEConnectionManager::~CEConnectionManager( void )
{
    CEConnectionManager::sService = NULL;
    mWnd    =   0;
}

void CEConnectionManager::StartupServiceInterface( CEComponent & holder )
{
    TRACE_SCOPE( centralapp_CEConnectionManager_StartupServiceInterface );
    CEConnectionManagerStub::StartupServiceInterface( holder );
    CECentralMessagerStub::StartupServiceInterface( holder );

    srand( static_cast<unsigned int>( CEDateTime::GetNow(false).GetTime() ) );
    mCookies = rand();

    SetConnectionList( NEConnectionManager::MapConnection( ) );
}

void CEConnectionManager::RequestConnet( const CEString & nickName, const CEDateTime & dateTime )
{
    TRACE_SCOPE( centralapp_CEConnectionManager_RequestConnet );
    TRACE_DBG("Received connection request from client [ %s ] sent at time [ %s ]", static_cast<const char *>(nickName), static_cast<const char *>(dateTime.FormatTime()));

    NEConnectionManager::sConnection connection;
    connection.nickName     = nickName;
    connection.connectTime  = dateTime;
    connection.cookie       = NEConnectionManager::InvalidCookie;

    if ( (nickName.IsEmpty() == false) && (nickName.IsValidName() == true) )
    {
        if ( IsReservedNickname(nickName) == false )
        {
            if ( FindConnection(nickName, connection) == false )
            {
                TRACE_DBG( "The connection [ %s ] at time [ %s ] can be registered with recommended cookie [ %u ]", static_cast<const char *>(nickName), static_cast<const char *>(connection.connectedTime.FormatTime( )), mCookies + 1 );
                ResponseConnect(nickName, getNextCookie(), dateTime, NEConnectionManager::ConnectionAccepted);
            }
            else
            {
                TRACE_WARN( "There is already connected client [ %s ], which was accepted at [ %s ]", static_cast<const char *>(nickName), static_cast<const char *>(connection.connectedTime.FormatTime( )) );
                ResponseConnect( nickName, NEConnectionManager::InvalidCookie, dateTime, NEConnectionManager::ConnectionClientExist );
            }
        }
        else
        {
            TRACE_WARN( "The name [ %s ] is reserved by system and cannot be registered", static_cast<const char *>(nickName) );
            ResponseConnect( nickName, NEConnectionManager::InvalidCookie, dateTime, NEConnectionManager::ConnectionNameReserved );
        }
    }
    else
    {
        TRACE_ERR("The requested to connect client name [ %s ] cannot be empty or invalid characters, it should be valid name.", static_cast<const char *>(nickName));
        ResponseConnect( nickName, NEConnectionManager::InvalidCookie, dateTime, NEConnectionManager::InvalidClient );
    }
}

void CEConnectionManager::RequestRegisterConnection( const CEString & nickName, const uint32_t & cookie, const uint32_t & connectCookie, const CEDateTime & dateRegister )
{
    TRACE_SCOPE( centralapp_CEConnectionManager_RequestRegisterConnection );
    TRACE_DBG( "Received registration request from client [ %s ] with cookie [ %u ] sent at time [ %s ]", static_cast<const char *>(nickName), cookie, static_cast<const char *>(dateRegister.FormatTime( )) );

    NEConnectionManager::sConnection connection;
    connection.nickName     = nickName;
    connection.connectTime  = dateRegister;
    connection.cookie       = NEConnectionManager::InvalidCookie;
    NEConnectionManager::ListConnection listConnections;

    if ( (nickName.IsEmpty( ) == false) && (nickName.IsValidName( ) == true) )
    {
        if ( IsReservedNickname( nickName ) == false )
        {
            if ( FindConnection( nickName, connection ) == false )
            {
                NEConnectionManager::MapConnection & mapConnections = GetConnectionList( );
                listConnections.ReserveElements(mapConnections.GetSize( ));
                int count = 0;
                for ( MAPPOS pos = mapConnections.GetStartPosition(); pos != NULL; pos = mapConnections.GetNextPosition( pos ) )
                {
                    const NEConnectionManager::sConnection & entry = mapConnections.ElementAt( pos );
                    ASSERT(connection != entry);
                    listConnections.SetAt(count ++, entry);
                }

                uint32_t cookie = connection.cookie != NEConnectionManager::InvalidCookie ? connection.cookie : connectCookie;
                connection.cookie       = cookie != NEConnectionManager::InvalidCookie ? cookie : getNextCookie();
                connection.connectedTime= CEDateTime::GetNow( false );
                mapConnections.SetKey( connection.cookie, connection, false );

                TRACE_DBG( "Accepted new connection registration [ %s ] at time [ %s ]", static_cast<const char *>(nickName), static_cast<const char *>(connection.connectedTime.FormatTime( )) );

                ResponseRegisterConnection( connection, listConnections, true );
                BroadcastClientConnected( connection );
                BroadcastConnectionUpdated( mapConnections );
                NotifyConnectionListUpdated( );

                HWND hWnd = MAKE_HWND( mWnd );
                if ( ::IsWindow( hWnd ) )
                {
                    NECommonSettings::sMessageData * data = ::IsWindow( hWnd ) ? DEBUG_NEW NECommonSettings::sMessageData : NULL;
                    if ( data != NULL )
                    {
                        NEString::copyString<TCHAR, char>( data->nickName, NEConnectionManager::NicknameMaxLen, connection.nickName.GetBuffer( ) );
                        data->dataSave      = connection.cookie;
                        data->timeSend      = connection.connectTime;
                        data->timeReceived  = connection.connectedTime;
                        data->message[0]    = static_cast<TCHAR>(NEString::EndofString);

                        ::PostMessage( hWnd, NECentralApp::CmdRegistered, 0, reinterpret_cast<LPARAM>(data) );
                    }
                }
                else
                {
                    TRACE_WARN( "The HWN [ 0x%p ] is not window, escape sending message of connected client", hWnd );
                }
            }
            else
            {
                TRACE_WARN( "There is already connected client [ %s ], which was accepted at [ %s ]", static_cast<const char *>(nickName), static_cast<const char *>(connection.connectedTime.FormatTime( )) );
                ResponseRegisterConnection( connection, listConnections, false );
            }
        }
        else
        {
            TRACE_WARN( "The name [ %s ] is reserved by system and cannot be registered", static_cast<const char *>(nickName) );
            ResponseRegisterConnection( connection, listConnections, false );
        }
    }
    else
    {
        TRACE_ERR( "The requested to connect client name [ %s ] cannot be empty or invalid characters, it should be valid name.", static_cast<const char *>(nickName) );
        ResponseRegisterConnection( connection, listConnections, false );
    }
}

void CEConnectionManager::RequestDiconnect( const CEString & nickName, const uint32_t & cookie, const CEDateTime & dateTime )
{
    TRACE_SCOPE( centralapp_CEConnectionManager_RequestDiconnect );
    NEConnectionManager::sConnection connection;
    NEConnectionManager::MapConnection & mapConnections = GetConnectionList( );
    bool found = cookie != NEConnectionManager::InvalidCookie ? mapConnections.Find(cookie, connection) : FindConnection(nickName, connection);

    if ( found )
    {
        if ( connection.connectTime == dateTime )
        {
            TRACE_DBG( "Received request to disconnection client [ %s ] at time [ %s ], disconnecting client", static_cast<const char *>(nickName), static_cast<const char *>(dateTime.FormatTime( )) );

            VERIFY(mapConnections.RemoveKey(connection.cookie));
            BroadcastClientDisconnected( connection );
            BroadcastConnectionUpdated( mapConnections );
            NotifyConnectionListUpdated( );

            HWND hWnd = MAKE_HWND( mWnd );
            NECommonSettings::sMessageData * data = ::IsWindow( hWnd ) ? DEBUG_NEW NECommonSettings::sMessageData : NULL;
            if ( data != NULL )
            {
                NEString::copyString<TCHAR, char>( data->nickName, NEConnectionManager::NicknameMaxLen, connection.nickName.GetBuffer( ) );
                data->dataSave      = connection.cookie;
                data->timeSend      = connection.connectTime;
                data->timeReceived  = connection.connectedTime;
                data->message[0]    = static_cast<TCHAR>(NEString::EndofString);

                ::PostMessage( hWnd, NECentralApp::CmdUnregistered, 0, reinterpret_cast<LPARAM>(data) );
            }
        }
        else
        {
            TRACE_WARN("Received request to disconnect client [ %s ], but the connection date-time is wrong. It is specified [ %s ], but the registered is [ %s ]. Ignoring request."
                                , static_cast<const char *>(nickName)
                                , static_cast<const char *>(dateTime.FormatTime())
                                , static_cast<const char *>(connection.connectTime.FormatTime()) );
        }
    }
    else
    {
        TRACE_DBG( "There is not connected cient [ %s ] at time [ %s ]. Ignoring request to connect", static_cast<const char *>(nickName), static_cast<const char *>(dateTime.FormatTime( )) );
    }
}

void CEConnectionManager::RequestSendMessage( const CEString & nickName, const uint32_t & cookie, const CEString & newMessage, const CEDateTime & dateTime )
{
    TRACE_SCOPE( centralapp_CEConnectionManager_RequestSendMessage );

    NEConnectionManager::sConnection connection;
    NEConnectionManager::MapConnection & mapConnections = GetConnectionList( );
    bool found = cookie != NEConnectionManager::InvalidCookie ? mapConnections.Find( cookie, connection ) : FindConnection( nickName, connection );
    if ( found )
    {
        TRACE_DBG("Found registered client [ %s ], broadcasting to client new message [ %s ] sent at [ %s ]"
                    , static_cast<const char *>(nickName)
                    , static_cast<const char *>(newMessage)
                    , static_cast<const char *>(dateTime.FormatTime()) );
        BroadcastSendMessage(connection.nickName, cookie, newMessage, dateTime);
        BroadcastKeyTyping( connection.nickName, cookie, CEString::EmptyString );

        HWND hWnd = MAKE_HWND( mWnd );
        NECommonSettings::sMessageData * data = ::IsWindow( hWnd ) ? DEBUG_NEW NECommonSettings::sMessageData : NULL;
        if ( data != NULL )
        {
            NEString::copyString<TCHAR, char>( data->nickName, NEConnectionManager::NicknameMaxLen, connection.nickName.GetBuffer( ) );
            data->dataSave      = connection.cookie;
            data->timeSend      = dateTime;
            data->timeReceived  = CEDateTime::GetNow();
            data->message[0]    = static_cast<TCHAR>(NEString::EndofString);
            NEString::copyString<TCHAR, char>( data->message, NECentralMessager::MessageMaxLen, newMessage.GetBuffer() );

            ::PostMessage( hWnd, NECentralApp::CmdSendMessage, 0, reinterpret_cast<LPARAM>(data) );
        }
    }
    else
    {
        TRACE_WARN("Failed to find client [ %s ] registered with cookie [ %u ], ignoring sending message", static_cast<const char *>(nickName), cookie);
    }
}

void CEConnectionManager::RequestKeyTyping( const CEString & nickName, const uint32_t & cookie, const CEString & newMessage )
{
    TRACE_SCOPE( centralapp_CEConnectionManager_RequestKeyTyping );

    NEConnectionManager::sConnection connection;
    NEConnectionManager::MapConnection & mapConnections = GetConnectionList( );
    bool found = cookie != NEConnectionManager::InvalidCookie ? mapConnections.Find( cookie, connection ) : FindConnection( nickName, connection );
    if ( found )
    {
        TRACE_DBG( "Found registered client [ %s ], broadcasting to client typing message [ %s ]", static_cast<const char *>(nickName), static_cast<const char *>(newMessage));
        BroadcastKeyTyping( connection.nickName, cookie, newMessage );

        HWND hWnd = MAKE_HWND( mWnd );
        NECommonSettings::sMessageData * data = ::IsWindow( hWnd ) ? DEBUG_NEW NECommonSettings::sMessageData : NULL;
        if ( data != NULL )
        {
            NEString::copyString<TCHAR, char>( data->nickName, NEConnectionManager::NicknameMaxLen, connection.nickName.GetBuffer( ) );
            data->dataSave      = connection.cookie;
            data->timeSend      = connection.connectTime;
            data->timeReceived  = connection.connectedTime;
            data->message[0]    = static_cast<TCHAR>(NEString::EndofString);
            NEString::copyString<TCHAR, char>( data->message, NECentralMessager::MessageMaxLen, newMessage.GetBuffer() );

            ::PostMessage( hWnd, NECentralApp::CmdTypeMessage, 0, reinterpret_cast<LPARAM>(data) );
        }
    }
    else
    {
        TRACE_WARN( "Failed to find client [ %s ] registered with cookie [ %u ], ignoring typing message", static_cast<const char *>(nickName), cookie );
    }
}

bool CEConnectionManager::FindConnection( const CEString & nickName, NEConnectionManager::sConnection & connection )
{
    bool result = false;
    const NEConnectionManager::MapConnection & mapClients = GetConnectionList();
    for ( MAPPOS pos = mapClients.GetStartPosition(); (result == false) && (pos != NULL); pos = mapClients.GetNextPosition(pos) )
    {
        const NEConnectionManager::sConnection & temp = mapClients.GetValueAt(pos);
        if ( nickName == temp.nickName )
        {
            result      = true;
            connection  = temp;
        }
    }
    return result;
}

bool CEConnectionManager::IsReservedNickname( const CEString & nickName ) const
{
    if ( nickName == NECommonSettings::SERVER_NAME )
        return true;
    else if ( nickName == NECommonSettings::COMP_NAME_CENTRAL_SERVER )
        return true;
    else if ( nickName == NECommonSettings::COMP_NAME_DISTRIBUTED_CLIENT )
        return true;
    else
        return false;
}

inline bool CEConnectionManager::connectionExist( uint32_t cookie ) const
{
    const NEConnectionManager::MapConnection & mapConnections = GetConnectionList( );
    return (mapConnections.Find(cookie) != static_cast<MAPPOS>(NULL));
}

inline uint32_t CEConnectionManager::getNextCookie( void )
{
    ++ mCookies;
    while ( (mCookies == 0) || (connectionExist(mCookies) == true) )
        ++ mCookies;
    return mCookies;
}
