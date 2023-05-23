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

#define FIRST_MESSAGE       (WM_USER + 10 + static_cast<unsigned int>(NECentralApp::eWndCommands::CmdFirst))
#define MAKE_MESSAGE(elem)  (static_cast<unsigned int>(elem) + FIRST_MESSAGE)

#define MAKE_HWND(wnd)      reinterpret_cast<HWND>(wnd)

DEF_TRACE_SCOPE( centralapp_ConnectionManager_CreateComponent );
DEF_TRACE_SCOPE( centralapp_ConnectionManager_DeleteComponent );
DEF_TRACE_SCOPE( centralapp_ConnectionManager_startupServiceInterface );
DEF_TRACE_SCOPE( centralapp_ConnectionManager_requestConnect );
DEF_TRACE_SCOPE( centralapp_ConnectionManager_requestRegisterConnection );
DEF_TRACE_SCOPE( centralapp_ConnectionManager_requestDisconnect );
DEF_TRACE_SCOPE( centralapp_ConnectionManager_requestSendMessage );
DEF_TRACE_SCOPE( centralapp_ConnectionManager_requestKeyTyping );

BEGIN_MODEL(NECommon::MODEL_NAME_CENTRAL_SERVER)

    BEGIN_REGISTER_THREAD( NECentralApp::THREAD_CENTRAL, NECommon::WATCHDOG_IGNORE)
        BEGIN_REGISTER_COMPONENT( NECommon::COMP_NAME_CENTRAL_SERVER, ConnectionManager )
            REGISTER_IMPLEMENT_SERVICE( NEConnectionManager::ServiceName, NEConnectionManager::InterfaceVersion )
            REGISTER_IMPLEMENT_SERVICE( NECentralMessager::ServiceName, NECentralMessager::InterfaceVersion )
        END_REGISTER_COMPONENT( NECommon::COMP_NAME_CENTRAL_SERVER )
    END_REGISTER_THREAD( NECentralApp::THREAD_CENTRAL )

END_MODEL(NECommon::MODEL_NAME_CENTRAL_SERVER)

ConnectionManager *   ConnectionManager::sService   = nullptr;

Component * ConnectionManager::CreateComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner )
{
    TRACE_SCOPE( centralapp_ConnectionManager_CreateComponent );
    return new ConnectionManager( entry, owner, entry.getComponentData() );
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

ConnectionManager::ConnectionManager( const NERegistry::ComponentEntry & entry, ComponentThread & ownerThread, NEMemory::uAlign data )
    : Component             ( entry, ownerThread )
    , ConnectionManagerStub ( static_cast<Component &>(self()) )
    , CentralMessagerStub   ( static_cast<Component &>(self()) )

    , mWnd                  ( data.alignInt64.mElement )
    , mCookies              ( NEConnectionManager::InvalidCookie )
{
    ConnectionManager::sService   = this;
}

ConnectionManager::~ConnectionManager( void )
{
    ConnectionManager::sService = nullptr;
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

void ConnectionManager::requestConnect( const String & nickName, const DateTime & dateTime )
{
    TRACE_SCOPE( centralapp_ConnectionManager_requestConnect );
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
                responseConnect(nickName, getNextCookie(), dateTime, NEConnectionManager::eConnectionResult::ConnectionAccepted);
            }
            else
            {
                TRACE_WARN( "There is already connected client [ %s ], which was accepted at [ %s ]", static_cast<const char *>(nickName), static_cast<const char *>(connection.connectedTime.formatTime( )) );
                responseConnect( nickName, NEConnectionManager::InvalidCookie, dateTime, NEConnectionManager::eConnectionResult::ConnectionClientExist );
            }
        }
        else
        {
            TRACE_WARN( "The name [ %s ] is reserved by system and cannot be registered", static_cast<const char *>(nickName) );
            responseConnect( nickName, NEConnectionManager::InvalidCookie, dateTime, NEConnectionManager::eConnectionResult::ConnectionNameReserved );
        }
    }
    else
    {
        TRACE_ERR("The requested to connect client name [ %s ] cannot be empty or invalid characters, it should be valid name.", static_cast<const char *>(nickName));
        responseConnect( nickName, NEConnectionManager::InvalidCookie, dateTime, NEConnectionManager::eConnectionResult::InvalidClient );
    }
}

void ConnectionManager::requestRegisterConnection( const String & nickName, unsigned int cookie, unsigned int connectCookie, const DateTime & dateRegister )
{
    using MAPPOS = NEConnectionManager::MapConnection::MAPPOS;

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
                for ( MAPPOS pos = mapConnections.firstPosition(); mapConnections.isValidPosition(pos); pos = mapConnections.nextPosition( pos ) )
                {
                    const NEConnectionManager::sConnection & entry = mapConnections.valueAtPosition( pos );
                    ASSERT(connection != entry);
                    listConnections.setAt(count ++, entry);
                }

                uint32_t cookie = connection.cookie != NEConnectionManager::InvalidCookie ? connection.cookie : connectCookie;
                connection.cookie       = cookie != NEConnectionManager::InvalidCookie ? cookie : getNextCookie();
                connection.connectedTime= DateTime::getNow( );
                mapConnections.setAt( connection.cookie, connection );

                TRACE_DBG( "Accepted new connection registration [ %s ] at time [ %s ]", static_cast<const char *>(nickName), static_cast<const char *>(connection.connectedTime.formatTime( )) );

                responseRegisterConnection( connection, listConnections, true );
                broadcastClientConnected( connection );
                broadcastConnectionUpdated( mapConnections );
                notifyConnectionListUpdated( );

                HWND hWnd = MAKE_HWND( mWnd );
                if ( ::IsWindow( hWnd ) )
                {
                    NECommon::sMessageData * data = ::IsWindow( hWnd ) ? NECommon::newData( ) : nullptr;
                    if ( data != nullptr )
                    {
                        NEString::copyString<TCHAR, char>( data->nickName, NEConnectionManager::NicknameMaxLen, connection.nickName.getString( ) );
                        data->dataSave      = connection.cookie;
                        data->timeSend      = connection.connectTime;
                        data->timeReceived  = connection.connectedTime;
                        data->message[0]    = static_cast<TCHAR>(NEString::EndOfString);

                        ::PostMessage( hWnd, MAKE_MESSAGE(NECentralApp::eWndCommands::CmdRegistered), 0, reinterpret_cast<LPARAM>(data) );
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

void ConnectionManager::requestDisconnect( const String & nickName, unsigned int cookie, const DateTime & dateTime )
{
    TRACE_SCOPE( centralapp_ConnectionManager_requestDisconnect );
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
            NECommon::sMessageData * data = ::IsWindow( hWnd ) ? NECommon::newData( ) : nullptr;
            if ( data != nullptr )
            {
                NEString::copyString<TCHAR, char>( data->nickName, NEConnectionManager::NicknameMaxLen, connection.nickName.getString( ) );
                data->dataSave      = connection.cookie;
                data->timeSend      = connection.connectTime;
                data->timeReceived  = connection.connectedTime;
                data->message[0]    = static_cast<TCHAR>(NEString::EndOfString);

                ::PostMessage( hWnd, MAKE_MESSAGE(NECentralApp::eWndCommands::CmdUnregistered), 0, reinterpret_cast<LPARAM>(data) );
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
        TRACE_DBG( "There is not connected client [ %s ] at time [ %s ]. Ignoring request to connect", static_cast<const char *>(nickName), static_cast<const char *>(dateTime.formatTime( )) );
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
        broadcastKeyTyping( connection.nickName, cookie, String::getEmptyString() );

        HWND hWnd = MAKE_HWND( mWnd );
        NECommon::sMessageData * data = ::IsWindow( hWnd ) ? NECommon::newData( ) : nullptr;
        if ( data != nullptr )
        {
            NEString::copyString<TCHAR, char>( data->nickName, NEConnectionManager::NicknameMaxLen, connection.nickName.getString( ) );
            data->dataSave      = connection.cookie;
            data->timeSend      = dateTime;
            data->timeReceived  = DateTime::getNow();
            data->message[0]    = static_cast<TCHAR>(NEString::EndOfString);
            NEString::copyString<TCHAR, char>( data->message, NECentralMessager::MessageMaxLen, newMessage.getString() );

            ::PostMessage( hWnd, MAKE_MESSAGE(NECentralApp::eWndCommands::CmdSendMessage), 0, reinterpret_cast<LPARAM>(data) );
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
        NECommon::sMessageData * data = ::IsWindow( hWnd ) ? NECommon::newData( ) : nullptr;
        if ( data != nullptr )
        {
            NEString::copyString<TCHAR, char>( data->nickName, NEConnectionManager::NicknameMaxLen, connection.nickName.getString( ) );
            data->dataSave      = connection.cookie;
            data->timeSend      = connection.connectTime;
            data->timeReceived  = connection.connectedTime;
            data->message[0]    = static_cast<TCHAR>(NEString::EndOfString);
            NEString::copyString<TCHAR, char>( data->message, NECentralMessager::MessageMaxLen, newMessage.getString() );

            ::PostMessage( hWnd, MAKE_MESSAGE(NECentralApp::eWndCommands::CmdTypeMessage), 0, reinterpret_cast<LPARAM>(data) );
        }
    }
    else
    {
        TRACE_WARN( "Failed to find client [ %s ] registered with cookie [ %u ], ignoring typing message", static_cast<const char *>(nickName), cookie );
    }
}

bool ConnectionManager::FindConnection( const String & nickName, NEConnectionManager::sConnection & connection )
{
    using MAPPOS = NEConnectionManager::MapConnection::MAPPOS;

    bool result = false;
    const NEConnectionManager::MapConnection & mapClients = getConnectionList();
    for ( MAPPOS pos = mapClients.firstPosition(); (result == false) && mapClients.isValidPosition(pos); pos = mapClients.nextPosition(pos) )
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
    return mapConnections.contains(cookie);
}

inline uint32_t ConnectionManager::getNextCookie( void )
{
    ++ mCookies;
    while ( (mCookies == 0) || (connectionExist(mCookies) == true) )
        ++ mCookies;
    return mCookies;
}
