/************************************************************************
 * \file            register/services/ConnectionManager.cpp
 * \brief           The connection manager server component implementation
 ************************************************************************/

#include "register/res/stdafx.h"
#include "register/services/ConnectionManager.hpp"
#include "register/ui/CentralDialog.hpp"
#include "register/NECentralApp.hpp"
#include "register/CentralApp.hpp"
#include "common/NECommon.hpp"
#include "areg/component/NERegistry.hpp"
#include "areg/component/ComponentLoader.hpp"

#define FIRST_MESSAGE       (WM_USER + 10 + static_cast<unsigned int>(NECentralApp::eWndCommands::CmdFirst))
#define MAKE_MESSAGE(elem)  (static_cast<unsigned int>(elem) + FIRST_MESSAGE)

#define MAKE_HWND(wnd)      reinterpret_cast<HWND>(wnd)

DEF_LOG_SCOPE( centralapp_ConnectionManager_startupServiceInterface );
DEF_LOG_SCOPE( centralapp_ConnectionManager_requestConnect );
DEF_LOG_SCOPE( centralapp_ConnectionManager_requestRegisterConnection );
DEF_LOG_SCOPE( centralapp_ConnectionManager_requestDisconnect );
DEF_LOG_SCOPE( centralapp_ConnectionManager_requestSendMessage );
DEF_LOG_SCOPE( centralapp_ConnectionManager_requestKeyTyping );

BEGIN_MODEL(NECommon::MODEL_NAME_CENTRAL_SERVER)

    BEGIN_REGISTER_THREAD( NECentralApp::THREAD_CENTRAL )
        BEGIN_REGISTER_COMPONENT( NECommon::COMP_NAME_CENTRAL_SERVER, ConnectionManager )
            REGISTER_IMPLEMENT_SERVICE( NEConnectionManager::ServiceName, NEConnectionManager::InterfaceVersion )
            REGISTER_IMPLEMENT_SERVICE( NECentralMessager::ServiceName, NECentralMessager::InterfaceVersion )
        END_REGISTER_COMPONENT( NECommon::COMP_NAME_CENTRAL_SERVER )
    END_REGISTER_THREAD( NECentralApp::THREAD_CENTRAL )

END_MODEL(NECommon::MODEL_NAME_CENTRAL_SERVER)

ConnectionManager *   ConnectionManager::sService   = nullptr;

ConnectionManager * ConnectionManager::getService( void )
{
    return ConnectionManager::sService;
}

ConnectionManager::ConnectionManager( const NERegistry::ComponentEntry & entry, ComponentThread & ownerThread )
    : Component             ( entry, ownerThread )
    , ConnectionManagerStub ( static_cast<Component &>(self()) )
    , CentralMessagerStub   ( static_cast<Component &>(self()) )

    , mWnd                  ( std::any_cast<HWND>(entry.getComponentData()) )
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
    LOG_SCOPE( centralapp_ConnectionManager_startupServiceInterface );
    ConnectionManagerStub::startupServiceInterface( holder );
    CentralMessagerStub::startupServiceInterface( holder );

    mCookies = 1;

    setConnectionList( NEConnectionManager::MapConnections( ) );
}

void ConnectionManager::requestConnect( const String & nickName, const DateTime & dateTime )
{
    LOG_SCOPE( centralapp_ConnectionManager_requestConnect );
    LOG_DBG("Received connection request from client [ %s ] sent at time [ %s ]", static_cast<const char *>(nickName), static_cast<const char *>(dateTime.formatTime()));

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
                uint32_t cookie = getNextCookie();
                LOG_DBG( "The connection [ %s ] at time [ %s / %u] is accepted and can be registered with recommended cookie [ %u ]"
                        , nickName.getString()
                        , connection.connectTime.formatTime( ).getString()
                        , static_cast<uint32_t>(connection.connectTime.getTime())
                        , cookie);
                responseConnect(nickName, cookie, dateTime, NEConnectionManager::eConnectionResult::ConnectionAccepted);
            }
            else
            {
                LOG_WARN( "There is already connected client [ %s ], which was accepted at [ %s ]", static_cast<const char *>(nickName), static_cast<const char *>(connection.connectedTime.formatTime( )) );
                responseConnect( nickName, NEConnectionManager::InvalidCookie, dateTime, NEConnectionManager::eConnectionResult::ConnectionClientExist );
            }
        }
        else
        {
            LOG_WARN( "The name [ %s ] is reserved by system and cannot be registered", static_cast<const char *>(nickName) );
            responseConnect( nickName, NEConnectionManager::InvalidCookie, dateTime, NEConnectionManager::eConnectionResult::ConnectionNameReserved );
        }
    }
    else
    {
        LOG_ERR("The requested to connect client name [ %s ] cannot be empty or invalid characters, it should be valid name.", static_cast<const char *>(nickName));
        responseConnect( nickName, NEConnectionManager::InvalidCookie, dateTime, NEConnectionManager::eConnectionResult::InvalidClient );
    }
}

void ConnectionManager::requestRegisterConnection( const String & nickName, unsigned int cookie, unsigned int connectCookie, const DateTime & dateRegister )
{
    LOG_SCOPE( centralapp_ConnectionManager_requestRegisterConnection );
    LOG_DBG( "Received registration request from client [ %s ] with cookie [ %u ] sent at time [ %s ]", static_cast<const char *>(nickName), cookie, static_cast<const char *>(dateRegister.formatTime( )) );

    NEConnectionManager::sConnection connection;
    connection.nickName     = nickName;
    connection.connectTime  = dateRegister;
    connection.cookie       = NEConnectionManager::InvalidCookie;
    NEConnectionManager::ListConnections listConnections;

    if ( (nickName.isEmpty( ) == false) && (nickName.isValidName( ) == true) )
    {
        if ( IsReservedNickname( nickName ) == false )
        {
            if ( FindConnection( nickName, connection ) == false )
            {
                NEConnectionManager::MapConnections & mapConnections = getConnectionList( );
                listConnections.resize(mapConnections.getSize( ));
                uint32_t count = 0;
                const auto& map = mapConnections.getData();
                for (const auto& entry : map)
                {
                    ASSERT(entry.second != connection);
                    listConnections.setAt(count++, entry.second);
                }

                uint32_t whichCookie = connection.cookie != NEConnectionManager::InvalidCookie ? connection.cookie : connectCookie;
                connection.cookie       = whichCookie != NEConnectionManager::InvalidCookie ? whichCookie : getNextCookie();
                connection.connectedTime= DateTime::getNow( );
                mapConnections.setAt( connection.cookie, connection );

                LOG_DBG( "Accepted new connection registration [ %s ] at time [ %s ]", static_cast<const char *>(nickName), static_cast<const char *>(connection.connectedTime.formatTime( )) );

                responseRegisterConnection( connection, listConnections, true );
                broadcastClientConnected( connection );
                broadcastConnectionUpdated( mapConnections );
                notifyConnectionListUpdated( );

                HWND hWnd = mWnd;
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
                    LOG_WARN( "The HWN [ 0x%p ] is not window, escape sending message of connected client", hWnd );
                }
            }
            else
            {
                LOG_WARN( "There is already connected client [ %s ], which was accepted at [ %s ]", static_cast<const char *>(nickName), static_cast<const char *>(connection.connectedTime.formatTime( )) );
                responseRegisterConnection( connection, listConnections, false );
            }
        }
        else
        {
            LOG_WARN( "The name [ %s ] is reserved by system and cannot be registered", static_cast<const char *>(nickName) );
            responseRegisterConnection( connection, listConnections, false );
        }
    }
    else
    {
        LOG_ERR( "The requested to connect client name [ %s ] cannot be empty or invalid characters, it should be valid name.", static_cast<const char *>(nickName) );
        responseRegisterConnection( connection, listConnections, false );
    }
}

void ConnectionManager::requestDisconnect( const String & nickName, unsigned int cookie, const DateTime & dateTime )
{
    LOG_SCOPE( centralapp_ConnectionManager_requestDisconnect );
    NEConnectionManager::sConnection connection;
    NEConnectionManager::MapConnections & mapConnections = getConnectionList( );
    bool found = cookie != NEConnectionManager::InvalidCookie ? mapConnections.find(cookie, connection) : FindConnection(nickName, connection);

    if ( found )
    {
        if ( connection.connectTime == dateTime )
        {
            LOG_DBG( "Received request to disconnection client [ %s ] at time [ %s ], disconnecting client", static_cast<const char *>(nickName), static_cast<const char *>(dateTime.formatTime( )) );

            VERIFY(mapConnections.removeAt(connection.cookie));
            broadcastClientDisconnected( connection );
            broadcastConnectionUpdated( mapConnections );
            notifyConnectionListUpdated( );

            HWND hWnd = mWnd;
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
            LOG_WARN("Received request to disconnect client [ %s ], but the connection date-time is wrong. It is specified [ %s ], but the registered is [ %s ]. Ignoring request."
                                , static_cast<const char *>(nickName)
                                , static_cast<const char *>(dateTime.formatTime())
                                , static_cast<const char *>(connection.connectTime.formatTime()) );
        }
    }
    else
    {
        LOG_DBG( "There is not connected client [ %s ] at time [ %s ]. Ignoring request to connect", static_cast<const char *>(nickName), static_cast<const char *>(dateTime.formatTime( )) );
    }
}

void ConnectionManager::requestSendMessage( const String & nickName, unsigned int cookie, const String & newMessage, const DateTime & dateTime )
{
    LOG_SCOPE( centralapp_ConnectionManager_requestSendMessage );

    NEConnectionManager::sConnection connection;
    NEConnectionManager::MapConnections & mapConnections = getConnectionList( );
    bool found = cookie != NEConnectionManager::InvalidCookie ? mapConnections.find( cookie, connection ) : FindConnection( nickName, connection );
    if ( found )
    {
        LOG_DBG("Found registered client [ %s ], broadcasting to client new message [ %s ] sent at [ %s ]"
                    , static_cast<const char *>(nickName)
                    , static_cast<const char *>(newMessage)
                    , static_cast<const char *>(dateTime.formatTime()) );
        broadcastSendMessage(connection.nickName, cookie, newMessage, dateTime);
        broadcastKeyTyping( connection.nickName, cookie, String::getEmptyString() );

        HWND hWnd = mWnd;
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
        LOG_WARN("Failed to find client [ %s ] registered with cookie [ %u ], ignoring sending message", static_cast<const char *>(nickName), cookie);
    }
}

void ConnectionManager::requestKeyTyping( const String & nickName, unsigned int cookie, const String & newMessage )
{
    LOG_SCOPE( centralapp_ConnectionManager_requestKeyTyping );

    NEConnectionManager::sConnection connection;
    NEConnectionManager::MapConnections & mapConnections = getConnectionList( );
    bool found = cookie != NEConnectionManager::InvalidCookie ? mapConnections.find( cookie, connection ) : FindConnection( nickName, connection );
    if ( found )
    {
        LOG_DBG( "Found registered client [ %s ], broadcasting to client typing message [ %s ]", static_cast<const char *>(nickName), static_cast<const char *>(newMessage));
        broadcastKeyTyping( connection.nickName, cookie, newMessage );

        HWND hWnd = mWnd;
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
        LOG_WARN( "Failed to find client [ %s ] registered with cookie [ %u ], ignoring typing message", static_cast<const char *>(nickName), cookie );
    }
}

bool ConnectionManager::FindConnection( const String & nickName, NEConnectionManager::sConnection & connection )
{
    bool result = false;
    const NEConnectionManager::MapConnections & mapClients = getConnectionList();
    const auto& map = mapClients.getData();
    for (const auto& entry : map)
    {
        if (nickName == entry.second.nickName)
        {
            result = true;
            connection = entry.second;
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
    const NEConnectionManager::MapConnections & mapConnections = getConnectionList( );
    return mapConnections.contains(cookie);
}

inline uint32_t ConnectionManager::getNextCookie( void )
{
    ++ mCookies;
    while ( (mCookies == 0) || (connectionExist(mCookies) == true) )
        ++ mCookies;
    return mCookies;
}
