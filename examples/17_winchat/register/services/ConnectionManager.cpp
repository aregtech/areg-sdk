/************************************************************************
 * \file            register/services/ConnectionManager.cpp
 * \brief           The connection manager server component implementation
 ************************************************************************/

#include "register/res/stdafx.h"
#include "register/services/ConnectionManager.hpp"
#include "register/ui/CentralDialog.hpp"
#include "register/NECentralApp.hpp"
#include "register/CentralApp.hpp"
#include "generated/NECommon.hpp"
#include "areg/component/NERegistry.hpp"
#include "areg/component/ComponentLoader.hpp"

#define MAKE_HWND(wnd)      reinterpret_cast<HWND>(wnd)

DEF_TRACE_SCOPE( centralapp_ConnectionManager_CreateComponent );
DEF_TRACE_SCOPE( centralapp_ConnectionManager_DeleteComponent );
DEF_TRACE_SCOPE( centralapp_ConnectionManager_startupServiceInterface );
DEF_TRACE_SCOPE( centralapp_ConnectionManager_requestConnet );
DEF_TRACE_SCOPE( centralapp_ConnectionManager_requestRegisterConnection );
DEF_TRACE_SCOPE( centralapp_ConnectionManager_requestDiconnect );
DEF_TRACE_SCOPE( centralapp_ConnectionManager_requestSendMessage );
DEF_TRACE_SCOPE( centralapp_ConnectionManager_requestKeyTyping );

BEGIN_MODEL(NECommon::MODEL_NAME_CENTRAL_SERVER)

    BEGIN_REGISTER_THREAD( NECentralApp::THREAD_CENTRAL )
        BEGIN_REGISTER_COMPONENT( NECommon::COMP_NAME_CENTRAL_SERVER, ConnectionManager )
            REGISTER_IMPLEMENT_SERVICE( NEConnectionManager::ServiceName, NEConnectionManager::InterfaceVersion )
            REGISTER_IMPLEMENT_SERVICE( NECentralMessager::ServiceName, NECentralMessager::InterfaceVersion )
        END_REGISTER_COMPONENT( NECommon::COMP_NAME_CENTRAL_SERVER )
    END_REGISTER_THREAD( NECentralApp::THREAD_CENTRAL )

END_MODEL(NECommon::MODEL_NAME_CENTRAL_SERVER)

ConnectionManager *   ConnectionManager::sService   = NULL;

Component * ConnectionManager::CreateComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner )
{
    TRACE_SCOPE( centralapp_ConnectionManager_CreateComponent );
    return new ConnectionManager( owner, entry.mRoleName.getBuffer(), entry.getComponentData() );
}

void ConnectionManager::DeleteComponent( Component & compObject, const NERegistry::ComponentEntry & entry )
{
    TRACE_SCOPE( centralapp_ConnectionManager_DeleteComponent );
    delete (&compObject);
}

ConnectionManager * ConnectionManager::getService( void )
{
    return ConnectionManager::sService;
}

ConnectionManager::ConnectionManager( ComponentThread & masterThread, const char * const roleName, NEMemory::uAlign data )
    : Component               ( masterThread, roleName )
    , ConnectionManagerStub   ( static_cast<Component &>(self()) )
    , CentralMessagerStub     ( static_cast<Component &>(self()) )

    , mWnd                      ( data.alignInt64.mElement )
    , mCookies                  ( NEConnectionManager::InvalidCookie )
{
    ConnectionManager::sService   = this;
}

ConnectionManager::~ConnectionManager( void )
{
    ConnectionManager::sService = NULL;
    mWnd    =   0;
}

void ConnectionManager::startupServiceInterface( Component & holder )
{
    TRACE_SCOPE( centralapp_ConnectionManager_startupServiceInterface );
    ConnectionManagerStub::startupServiceInterface( holder );
    CentralMessagerStub::startupServiceInterface( holder );

    srand( static_cast<unsigned int>( DateTime::getNow().getTime() ) );
    mCookies = rand();

    setConnectionList( NEConnectionManager::MapConnection( ) );
}

void ConnectionManager::requestConnet( const String & nickName, const DateTime & dateTime )
{
    TRACE_SCOPE( centralapp_ConnectionManager_requestConnet );
    TRACE_DBG("Received connection request from client [ %s ] sent at time [ %s ]", static_cast<const char *>(nickName), static_cast<const char *>(dateTime.formatTime()));

    NEConnectionManager::sConnection connection;
    connection.nickName     = nickName;
    connection.connectTime  = dateTime;
    connection.cookie       = NEConnectionManager::InvalidCookie;

    if ( (nickName.isEmpty() == false) && (nickName.isValidName() == true) )
    {
        if ( IsReservedNickname(nickName) == false )
        {
            if ( FindConnection(nickName, connection) == false )
            {
                TRACE_DBG( "The connection [ %s ] at time [ %s ] can be registered with recommended cookie [ %u ]", static_cast<const char *>(nickName), static_cast<const char *>(connection.connectedTime.formatTime( )), mCookies + 1 );
                responseConnect(nickName, getNextCookie(), dateTime, NEConnectionManager::ConnectionAccepted);
            }
            else
            {
                TRACE_WARN( "There is already connected client [ %s ], which was accepted at [ %s ]", static_cast<const char *>(nickName), static_cast<const char *>(connection.connectedTime.formatTime( )) );
                responseConnect( nickName, NEConnectionManager::InvalidCookie, dateTime, NEConnectionManager::ConnectionClientExist );
            }
        }
        else
        {
            TRACE_WARN( "The name [ %s ] is reserved by system and cannot be registered", static_cast<const char *>(nickName) );
            responseConnect( nickName, NEConnectionManager::InvalidCookie, dateTime, NEConnectionManager::ConnectionNameReserved );
        }
    }
    else
    {
        TRACE_ERR("The requested to connect client name [ %s ] cannot be empty or invalid characters, it should be valid name.", static_cast<const char *>(nickName));
        responseConnect( nickName, NEConnectionManager::InvalidCookie, dateTime, NEConnectionManager::InvalidClient );
    }
}

void ConnectionManager::requestRegisterConnection( const String & nickName, unsigned int cookie, unsigned int connectCookie, const DateTime & dateRegister )
{
    TRACE_SCOPE( centralapp_ConnectionManager_requestRegisterConnection );
    TRACE_DBG( "Received registration request from client [ %s ] with cookie [ %u ] sent at time [ %s ]", static_cast<const char *>(nickName), cookie, static_cast<const char *>(dateRegister.formatTime( )) );

    NEConnectionManager::sConnection connection;
    connection.nickName     = nickName;
    connection.connectTime  = dateRegister;
    connection.cookie       = NEConnectionManager::InvalidCookie;
    NEConnectionManager::ListConnection listConnections;

    if ( (nickName.isEmpty( ) == false) && (nickName.isValidName( ) == true) )
    {
        if ( IsReservedNickname( nickName ) == false )
        {
            if ( FindConnection( nickName, connection ) == false )
            {
                NEConnectionManager::MapConnection & mapConnections = getConnectionList( );
                listConnections.resize(mapConnections.getSize( ));
                int count = 0;
                for ( MAPPOS pos = mapConnections.firstPosition(); pos != NULL; pos = mapConnections.nextPosition( pos ) )
                {
                    const NEConnectionManager::sConnection & entry = mapConnections.getPosition( pos );
                    ASSERT(connection != entry);
                    listConnections.setAt(count ++, entry);
                }

                uint32_t cookie = connection.cookie != NEConnectionManager::InvalidCookie ? connection.cookie : connectCookie;
                connection.cookie       = cookie != NEConnectionManager::InvalidCookie ? cookie : getNextCookie();
                connection.connectedTime= DateTime::getNow( );
                mapConnections.setAt( connection.cookie, connection, false );

                TRACE_DBG( "Accepted new connection registration [ %s ] at time [ %s ]", static_cast<const char *>(nickName), static_cast<const char *>(connection.connectedTime.formatTime( )) );

                responseRegisterConnection( connection, listConnections, true );
                broadcastClientConnected( connection );
                broadcastConnectionUpdated( mapConnections );
                notifyConnectionListUpdated( );

                HWND hWnd = MAKE_HWND( mWnd );
                if ( ::IsWindow( hWnd ) )
                {
                    NECommon::sMessageData * data = ::IsWindow( hWnd ) ? DEBUG_NEW NECommon::sMessageData : NULL;
                    if ( data != NULL )
                    {
                        NEString::copyString<TCHAR, char>( data->nickName, NEConnectionManager::NicknameMaxLen, connection.nickName.getBuffer( ) );
                        data->dataSave      = connection.cookie;
                        data->timeSend      = connection.connectTime;
                        data->timeReceived  = connection.connectedTime;
                        data->message[0]    = static_cast<TCHAR>(NEString::EndOfString);

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
                TRACE_WARN( "There is already connected client [ %s ], which was accepted at [ %s ]", static_cast<const char *>(nickName), static_cast<const char *>(connection.connectedTime.formatTime( )) );
                responseRegisterConnection( connection, listConnections, false );
            }
        }
        else
        {
            TRACE_WARN( "The name [ %s ] is reserved by system and cannot be registered", static_cast<const char *>(nickName) );
            responseRegisterConnection( connection, listConnections, false );
        }
    }
    else
    {
        TRACE_ERR( "The requested to connect client name [ %s ] cannot be empty or invalid characters, it should be valid name.", static_cast<const char *>(nickName) );
        responseRegisterConnection( connection, listConnections, false );
    }
}

void ConnectionManager::requestDiconnect( const String & nickName, unsigned int cookie, const DateTime & dateTime )
{
    TRACE_SCOPE( centralapp_ConnectionManager_requestDiconnect );
    NEConnectionManager::sConnection connection;
    NEConnectionManager::MapConnection & mapConnections = getConnectionList( );
    bool found = cookie != NEConnectionManager::InvalidCookie ? mapConnections.find(cookie, connection) : FindConnection(nickName, connection);

    if ( found )
    {
        if ( connection.connectTime == dateTime )
        {
            TRACE_DBG( "Received request to disconnection client [ %s ] at time [ %s ], disconnecting client", static_cast<const char *>(nickName), static_cast<const char *>(dateTime.formatTime( )) );

            VERIFY(mapConnections.removeAt(connection.cookie));
            broadcastClientDisconnected( connection );
            broadcastConnectionUpdated( mapConnections );
            notifyConnectionListUpdated( );

            HWND hWnd = MAKE_HWND( mWnd );
            NECommon::sMessageData * data = ::IsWindow( hWnd ) ? DEBUG_NEW NECommon::sMessageData : NULL;
            if ( data != NULL )
            {
                NEString::copyString<TCHAR, char>( data->nickName, NEConnectionManager::NicknameMaxLen, connection.nickName.getBuffer( ) );
                data->dataSave      = connection.cookie;
                data->timeSend      = connection.connectTime;
                data->timeReceived  = connection.connectedTime;
                data->message[0]    = static_cast<TCHAR>(NEString::EndOfString);

                ::PostMessage( hWnd, NECentralApp::CmdUnregistered, 0, reinterpret_cast<LPARAM>(data) );
            }
        }
        else
        {
            TRACE_WARN("Received request to disconnect client [ %s ], but the connection date-time is wrong. It is specified [ %s ], but the registered is [ %s ]. Ignoring request."
                                , static_cast<const char *>(nickName)
                                , static_cast<const char *>(dateTime.formatTime())
                                , static_cast<const char *>(connection.connectTime.formatTime()) );
        }
    }
    else
    {
        TRACE_DBG( "There is not connected cient [ %s ] at time [ %s ]. Ignoring request to connect", static_cast<const char *>(nickName), static_cast<const char *>(dateTime.formatTime( )) );
    }
}

void ConnectionManager::requestSendMessage( const String & nickName, unsigned int cookie, const String & newMessage, const DateTime & dateTime )
{
    TRACE_SCOPE( centralapp_ConnectionManager_requestSendMessage );

    NEConnectionManager::sConnection connection;
    NEConnectionManager::MapConnection & mapConnections = getConnectionList( );
    bool found = cookie != NEConnectionManager::InvalidCookie ? mapConnections.find( cookie, connection ) : FindConnection( nickName, connection );
    if ( found )
    {
        TRACE_DBG("Found registered client [ %s ], broadcasting to client new message [ %s ] sent at [ %s ]"
                    , static_cast<const char *>(nickName)
                    , static_cast<const char *>(newMessage)
                    , static_cast<const char *>(dateTime.formatTime()) );
        broadcastSendMessage(connection.nickName, cookie, newMessage, dateTime);
        broadcastKeyTyping( connection.nickName, cookie, String::EmptyString );

        HWND hWnd = MAKE_HWND( mWnd );
        NECommon::sMessageData * data = ::IsWindow( hWnd ) ? DEBUG_NEW NECommon::sMessageData : NULL;
        if ( data != NULL )
        {
            NEString::copyString<TCHAR, char>( data->nickName, NEConnectionManager::NicknameMaxLen, connection.nickName.getBuffer( ) );
            data->dataSave      = connection.cookie;
            data->timeSend      = dateTime;
            data->timeReceived  = DateTime::getNow();
            data->message[0]    = static_cast<TCHAR>(NEString::EndOfString);
            NEString::copyString<TCHAR, char>( data->message, NECentralMessager::MessageMaxLen, newMessage.getBuffer() );

            ::PostMessage( hWnd, NECentralApp::CmdSendMessage, 0, reinterpret_cast<LPARAM>(data) );
        }
    }
    else
    {
        TRACE_WARN("Failed to find client [ %s ] registered with cookie [ %u ], ignoring sending message", static_cast<const char *>(nickName), cookie);
    }
}

void ConnectionManager::requestKeyTyping( const String & nickName, unsigned int cookie, const String & newMessage )
{
    TRACE_SCOPE( centralapp_ConnectionManager_requestKeyTyping );

    NEConnectionManager::sConnection connection;
    NEConnectionManager::MapConnection & mapConnections = getConnectionList( );
    bool found = cookie != NEConnectionManager::InvalidCookie ? mapConnections.find( cookie, connection ) : FindConnection( nickName, connection );
    if ( found )
    {
        TRACE_DBG( "Found registered client [ %s ], broadcasting to client typing message [ %s ]", static_cast<const char *>(nickName), static_cast<const char *>(newMessage));
        broadcastKeyTyping( connection.nickName, cookie, newMessage );

        HWND hWnd = MAKE_HWND( mWnd );
        NECommon::sMessageData * data = ::IsWindow( hWnd ) ? DEBUG_NEW NECommon::sMessageData : NULL;
        if ( data != NULL )
        {
            NEString::copyString<TCHAR, char>( data->nickName, NEConnectionManager::NicknameMaxLen, connection.nickName.getBuffer( ) );
            data->dataSave      = connection.cookie;
            data->timeSend      = connection.connectTime;
            data->timeReceived  = connection.connectedTime;
            data->message[0]    = static_cast<TCHAR>(NEString::EndOfString);
            NEString::copyString<TCHAR, char>( data->message, NECentralMessager::MessageMaxLen, newMessage.getBuffer() );

            ::PostMessage( hWnd, NECentralApp::CmdTypeMessage, 0, reinterpret_cast<LPARAM>(data) );
        }
    }
    else
    {
        TRACE_WARN( "Failed to find client [ %s ] registered with cookie [ %u ], ignoring typing message", static_cast<const char *>(nickName), cookie );
    }
}

bool ConnectionManager::FindConnection( const String & nickName, NEConnectionManager::sConnection & connection )
{
    bool result = false;
    const NEConnectionManager::MapConnection & mapClients = getConnectionList();
    for ( MAPPOS pos = mapClients.firstPosition(); (result == false) && (pos != NULL); pos = mapClients.nextPosition(pos) )
    {
        const NEConnectionManager::sConnection & temp = mapClients.valueAtPosition(pos);
        if ( nickName == temp.nickName )
        {
            result      = true;
            connection  = temp;
        }
    }
    return result;
}

bool ConnectionManager::IsReservedNickname( const String & nickName ) const
{
    if ( nickName == NECommon::SERVER_NAME )
        return true;
    else if ( nickName == NECommon::COMP_NAME_CENTRAL_SERVER )
        return true;
    else if ( nickName == NECommon::COMP_NAME_DISTRIBUTED_CLIENT )
        return true;
    else
        return false;
}

inline bool ConnectionManager::connectionExist( uint32_t cookie ) const
{
    const NEConnectionManager::MapConnection & mapConnections = getConnectionList( );
    return (mapConnections.find(cookie) != static_cast<MAPPOS>(NULL));
}

inline uint32_t ConnectionManager::getNextCookie( void )
{
    ++ mCookies;
    while ( (mCookies == 0) || (connectionExist(mCookies) == true) )
        ++ mCookies;
    return mCookies;
}
