/************************************************************************
 * \file            register/services/ConnectionController.cpp
 * \brief           The connection manager server component implementation
 ************************************************************************/

#include "register/res/stdafx.h"
#include "register/services/ConnectionController.hpp"
#include "register/ui/CentralDialog.hpp"
#include "register/CentralApp.hpp"
#include "register/CentralAppDefs.hpp"
#include "common/ChatDefs.hpp"
#include "areg/component/Model.hpp"
#include "areg/component/ComponentLoader.hpp"

#define FIRST_MESSAGE       (WM_USER + 10 + static_cast<uint32_t>(NECentralApp::WindowCommand::CmdFirst))
#define MAKE_MESSAGE(elem)  (static_cast<uint32_t>(elem) + FIRST_MESSAGE)

#define MAKE_HWND(wnd)      reinterpret_cast<HWND>(wnd)

DEF_LOG_SCOPE( centralapp_ConnectionController_startupServiceInterface );
DEF_LOG_SCOPE( centralapp_ConnectionController_request_connect );
DEF_LOG_SCOPE( centralapp_ConnectionController_request_register_connection );
DEF_LOG_SCOPE( centralapp_ConnectionController_request_disconnect );
DEF_LOG_SCOPE( centralapp_ConnectionController_request_send_message );
DEF_LOG_SCOPE( centralapp_ConnectionController_request_key_typing );

BEGIN_MODEL(chat::MODEL_NAME_CENTRAL_SERVER)

    BEGIN_REGISTER_THREAD( NECentralApp::THREAD_CENTRAL )
        BEGIN_REGISTER_COMPONENT( chat::COMP_NAME_CENTRAL_SERVER, ConnectionController )
            REGISTER_IMPLEMENT_SERVICE( ConnectionManager::ServiceName, ConnectionManager::InterfaceVersion )
            REGISTER_IMPLEMENT_SERVICE( CentralMessager::ServiceName, CentralMessager::InterfaceVersion )
        END_REGISTER_COMPONENT( chat::COMP_NAME_CENTRAL_SERVER )
    END_REGISTER_THREAD( NECentralApp::THREAD_CENTRAL )

END_MODEL(chat::MODEL_NAME_CENTRAL_SERVER)

namespace
{
    ConnectionController* _thisService{ nullptr };
}

ConnectionController::ConnectionController( const areg::ComponentEntry & entry, areg::ComponentThread & ownerThread )
    : areg::Component             ( entry, ownerThread )
    , ConnectionManagerProviderBase ( static_cast<areg::Component &>(self()) )
    , CentralMessagerProviderBase   ( static_cast<areg::Component &>(self()) )

    , mWnd                  ( std::any_cast<HWND>(entry.data()) )
    , mCookies              ( ConnectionManager::InvalidCookie )
{
    _thisService = this;
}

ConnectionController::~ConnectionController()
{
    _thisService = nullptr;
    mWnd    =   0;
}

void ConnectionController::startup_service_interface( areg::Component & holder )
{
    LOG_SCOPE( centralapp_ConnectionController_startupServiceInterface );
    ConnectionManagerProviderBase::startup_service_interface( holder );
    CentralMessagerProviderBase::startup_service_interface( holder );

    mCookies = 1;

    set_connection_list(ConnectionManager::MapConnections( ) );
}

void ConnectionController::request_connect( const areg::String & nickName, const areg::DateTime & dateTime )
{
    LOG_SCOPE( centralapp_ConnectionController_request_connect );
    LOG_DBG("Received connection request from client [ %s ] sent at time [ %s ]", static_cast<const char *>(nickName), static_cast<const char *>(dateTime.format_time()));

    ConnectionManager::ConnectionRecord connection;
    connection.nickName     = nickName;
    connection.connectTime  = dateTime;
    connection.cookie       = ConnectionManager::InvalidCookie;

    if ( (nickName.is_empty() == false) && (nickName.is_valid_name() == true) )
    {
        if ( IsReservedNickname(nickName) == false )
        {
            if ( FindConnection(nickName, connection) == false )
            {
                uint32_t cookie = getNextCookie();
                LOG_DBG( "The connection [ %s ] at time [ %s / %u] is accepted and can be registered with recommended cookie [ %u ]"
                        , nickName.as_string()
                        , connection.connectTime.format_time( ).as_string()
                        , static_cast<uint32_t>(connection.connectTime.time())
                        , cookie);
                response_connect(nickName, cookie, dateTime, ConnectionManager::ConnectionResult::Accepted);
            }
            else
            {
                LOG_WARN( "There is already connected client [ %s ], which was accepted at [ %s ]", static_cast<const char *>(nickName), static_cast<const char *>(connection.connectedTime.format_time( )) );
                response_connect( nickName, ConnectionManager::InvalidCookie, dateTime, ConnectionManager::ConnectionResult::ClientExist );
            }
        }
        else
        {
            LOG_WARN( "The name [ %s ] is reserved by system and cannot be registered", static_cast<const char *>(nickName) );
            response_connect( nickName, ConnectionManager::InvalidCookie, dateTime, ConnectionManager::ConnectionResult::NameReserved );
        }
    }
    else
    {
        LOG_ERR("The requested to connect client name [ %s ] cannot be empty or invalid characters, it should be valid name.", static_cast<const char *>(nickName));
        response_connect( nickName, ConnectionManager::InvalidCookie, dateTime, ConnectionManager::ConnectionResult::InvalidClient );
    }
}

void ConnectionController::request_register_connection( const areg::String & nickName, uint32_t cookie, uint32_t connectCookie, const areg::DateTime & dateRegister )
{
    LOG_SCOPE( centralapp_ConnectionController_request_register_connection );
    LOG_DBG( "Received registration request from client [ %s ] with cookie [ %u ] sent at time [ %s ]", static_cast<const char *>(nickName), cookie, static_cast<const char *>(dateRegister.format_time( )) );

    ConnectionManager::ConnectionRecord connection;
    connection.nickName     = nickName;
    connection.connectTime  = dateRegister;
    connection.cookie       = ConnectionManager::InvalidCookie;
    ConnectionManager::ListConnections listConnections;

    if ( (nickName.is_empty( ) == false) && (nickName.is_valid_name( ) == true) )
    {
        if ( IsReservedNickname( nickName ) == false )
        {
            if ( FindConnection( nickName, connection ) == false )
            {
                ConnectionManager::MapConnections & mapConnections = connection_list( );
                listConnections.resize(mapConnections.size( ));
                uint32_t count = 0;
                const auto& map = mapConnections.data();
                for (const auto& entry : map)
                {
                    ASSERT(entry.second != connection);
                    listConnections.set_value_at(count++, entry.second);
                }

                uint32_t whichCookie = connection.cookie != ConnectionManager::InvalidCookie ? connection.cookie : connectCookie;
                connection.cookie       = whichCookie != ConnectionManager::InvalidCookie ? whichCookie : getNextCookie();
                connection.connectedTime= areg::DateTime::now( );
                mapConnections.set_value_at( connection.cookie, connection );

                LOG_DBG( "Accepted new connection registration [ %s ] at time [ %s ]", static_cast<const char *>(nickName), static_cast<const char *>(connection.connectedTime.format_time( )) );

                response_register_connection( connection, listConnections, true );
                broadcast_client_connected( connection );
                broadcast_connection_updated( mapConnections );
                notify_connection_list_updated( );

                HWND hWnd = mWnd;
                if ( ::IsWindow( hWnd ) )
                {
                    chat:: MessageData * data = ::IsWindow( hWnd ) ? chat::newData( ) : nullptr;
                    if ( data != nullptr )
                    {
                        areg::copy_string<TCHAR, char>( data->nickName, ConnectionManager::NicknameMaxLen, connection.nickName.as_string( ) );
                        data->dataSave      = connection.cookie;
                        data->timeSend      = connection.connectTime;
                        data->timeReceived  = connection.connectedTime;
                        data->message[0]    = static_cast<TCHAR>(areg::EndOfString);

                        ::PostMessage( hWnd, MAKE_MESSAGE(NECentralApp::WindowCommand::CmdRegistered), 0, reinterpret_cast<LPARAM>(data) );
                    }
                }
                else
                {
                    LOG_WARN( "The HWN [ 0x%p ] is not window, escape sending message of connected client", hWnd );
                }
            }
            else
            {
                LOG_WARN( "There is already connected client [ %s ], which was accepted at [ %s ]", static_cast<const char *>(nickName), static_cast<const char *>(connection.connectedTime.format_time( )) );
                response_register_connection( connection, listConnections, false );
            }
        }
        else
        {
            LOG_WARN( "The name [ %s ] is reserved by system and cannot be registered", static_cast<const char *>(nickName) );
            response_register_connection( connection, listConnections, false );
        }
    }
    else
    {
        LOG_ERR( "The requested to connect client name [ %s ] cannot be empty or invalid characters, it should be valid name.", static_cast<const char *>(nickName) );
        response_register_connection( connection, listConnections, false );
    }
}

void ConnectionController::request_disconnect( const areg::String & nickName, uint32_t cookie, const areg::DateTime & dateTime )
{
    LOG_SCOPE( centralapp_ConnectionController_request_disconnect );
    ConnectionManager::ConnectionRecord connection;
    ConnectionManager::MapConnections & mapConnections = connection_list( );
    bool found = cookie != ConnectionManager::InvalidCookie ? mapConnections.find(cookie, connection) : FindConnection(nickName, connection);

    if ( found )
    {
        if ( connection.connectTime == dateTime )
        {
            LOG_DBG( "Received request to disconnection client [ %s ] at time [ %s ], disconnecting client", static_cast<const char *>(nickName), static_cast<const char *>(dateTime.format_time( )) );

            VERIFY(mapConnections.remove_at(connection.cookie));
            broadcast_client_disconnected( connection );
            broadcast_connection_updated( mapConnections );
            notify_connection_list_updated( );

            HWND hWnd = mWnd;
            chat:: MessageData * data = ::IsWindow( hWnd ) ? chat::newData( ) : nullptr;
            if ( data != nullptr )
            {
                areg::copy_string<TCHAR, char>( data->nickName, ConnectionManager::NicknameMaxLen, connection.nickName.as_string( ) );
                data->dataSave      = connection.cookie;
                data->timeSend      = connection.connectTime;
                data->timeReceived  = connection.connectedTime;
                data->message[0]    = static_cast<TCHAR>(areg::EndOfString);

                ::PostMessage( hWnd, MAKE_MESSAGE(NECentralApp::WindowCommand::CmdUnregistered), 0, reinterpret_cast<LPARAM>(data) );
            }
        }
        else
        {
            LOG_WARN("Received request to disconnect client [ %s ], but the connection date-time is wrong. It is specified [ %s ], but the registered is [ %s ]. Ignoring request."
                                , static_cast<const char *>(nickName)
                                , static_cast<const char *>(dateTime.format_time())
                                , static_cast<const char *>(connection.connectTime.format_time()) );
        }
    }
    else
    {
        LOG_DBG( "There is not connected client [ %s ] at time [ %s ]. Ignoring request to connect", static_cast<const char *>(nickName), static_cast<const char *>(dateTime.format_time( )) );
    }
}

void ConnectionController::request_send_message( const areg::String & nickName, uint32_t cookie, const areg::String & newMessage, const areg::DateTime & dateTime )
{
    LOG_SCOPE( centralapp_ConnectionController_request_send_message );

    ConnectionManager::ConnectionRecord connection;
    ConnectionManager::MapConnections & mapConnections = connection_list( );
    bool found = cookie != ConnectionManager::InvalidCookie ? mapConnections.find( cookie, connection ) : FindConnection( nickName, connection );
    if ( found )
    {
        LOG_DBG("Found registered client [ %s ], broadcasting to client new message [ %s ] sent at [ %s ]"
                    , static_cast<const char *>(nickName)
                    , static_cast<const char *>(newMessage)
                    , static_cast<const char *>(dateTime.format_time()) );
        broadcast_send_message(connection.nickName, cookie, newMessage, dateTime);
        broadcast_key_typing( connection.nickName, cookie, areg::String::empty_string() );

        HWND hWnd = mWnd;
        chat:: MessageData * data = ::IsWindow( hWnd ) ? chat::newData( ) : nullptr;
        if ( data != nullptr )
        {
            areg::copy_string<TCHAR, char>( data->nickName, ConnectionManager::NicknameMaxLen, connection.nickName.as_string( ) );
            data->dataSave      = connection.cookie;
            data->timeSend      = dateTime;
            data->timeReceived  = areg::DateTime::now();
            data->message[0]    = static_cast<TCHAR>(areg::EndOfString);
            areg::copy_string<TCHAR, char>( data->message, CentralMessager::MessageMaxLen, newMessage.as_string() );

            ::PostMessage( hWnd, MAKE_MESSAGE(NECentralApp::WindowCommand::CmdSendMessage), 0, reinterpret_cast<LPARAM>(data) );
        }
    }
    else
    {
        LOG_WARN("Failed to find client [ %s ] registered with cookie [ %u ], ignoring sending message", static_cast<const char *>(nickName), cookie);
    }
}

void ConnectionController::request_key_typing( const areg::String & nickName, uint32_t cookie, const areg::String & newMessage )
{
    LOG_SCOPE( centralapp_ConnectionController_request_key_typing );

    ConnectionManager::ConnectionRecord connection;
    ConnectionManager::MapConnections & mapConnections = connection_list( );
    bool found = cookie != ConnectionManager::InvalidCookie ? mapConnections.find( cookie, connection ) : FindConnection( nickName, connection );
    if ( found )
    {
        LOG_DBG( "Found registered client [ %s ], broadcasting to client typing message [ %s ]", static_cast<const char *>(nickName), static_cast<const char *>(newMessage));
        broadcast_key_typing( connection.nickName, cookie, newMessage );

        HWND hWnd = mWnd;
        chat:: MessageData * data = ::IsWindow( hWnd ) ? chat::newData( ) : nullptr;
        if ( data != nullptr )
        {
            areg::copy_string<TCHAR, char>( data->nickName, ConnectionManager::NicknameMaxLen, connection.nickName.as_string( ) );
            data->dataSave      = connection.cookie;
            data->timeSend      = connection.connectTime;
            data->timeReceived  = connection.connectedTime;
            data->message[0]    = static_cast<TCHAR>(areg::EndOfString);
            areg::copy_string<TCHAR, char>( data->message, CentralMessager::MessageMaxLen, newMessage.as_string() );

            ::PostMessage( hWnd, MAKE_MESSAGE(NECentralApp::WindowCommand::CmdTypeMessage), 0, reinterpret_cast<LPARAM>(data) );
        }
    }
    else
    {
        LOG_WARN( "Failed to find client [ %s ] registered with cookie [ %u ], ignoring typing message", static_cast<const char *>(nickName), cookie );
    }
}

bool ConnectionController::FindConnection( const areg::String & nickName, ConnectionManager::ConnectionRecord & connection )
{
    bool result = false;
    const ConnectionManager::MapConnections & mapClients = connection_list();
    const auto& map = mapClients.data();
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

bool ConnectionController::IsReservedNickname( const areg::String & nickName ) const
{
    if ( nickName == chat::SERVER_NAME )
        return true;
    else if ( nickName == chat::COMP_NAME_CENTRAL_SERVER )
        return true;
    else if ( nickName == chat::COMP_NAME_DISTRIBUTED_CLIENT )
        return true;
    else
        return false;
}

ConnectionController* ConnectionController::getConnectionService()
{
    return _thisService;
}

inline bool ConnectionController::connectionExist( uint32_t cookie ) const
{
    const ConnectionManager::MapConnections & mapConnections = connection_list( );
    return mapConnections.contains(cookie);
}

inline uint32_t ConnectionController::getNextCookie()
{
    ++ mCookies;
    while ( (mCookies == 0) || (connectionExist(mCookies) == true) )
        ++ mCookies;
    return mCookies;
}
