
#include "chatter/res/stdafx.h"
#include "chatter/services/ConnectionList.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentThread.hpp"
#include "chatter/DistributedAppDefs.hpp"
#include "chatter/services/ConnectionHandler.hpp"
#include "chatter/ui/DistributedDialog.hpp"

DEF_LOG_SCOPE(chatter_ConnectionList, service_connected);
DEF_LOG_SCOPE(chatter_ConnectionList, response_register_connection);

ConnectionList::ConnectionList( const char * roleName, areg::Component & owner, ConnectionHandler & handlerConnection )
    : ConnectionManagerConsumerBase ( roleName, owner.master_thread() )
    , mConnectionHandler            ( handlerConnection )
{

}

ConnectionList::ConnectionList( const char * roleName, areg::DispatcherThread & dispThread, ConnectionHandler & handlerConnection )
    : ConnectionManagerConsumerBase ( roleName, dispThread )
    , mConnectionHandler            ( handlerConnection )
{

}

bool ConnectionList::service_connected( areg::ServiceConnectionState status, areg::ProxyBase & proxy )
{
    LOG_SCOPE( chatter_ConnectionList, service_connected );
    bool result = ConnectionManagerConsumerBase::service_connected( status, proxy );
    if ( is_connected( ) )
    {
        LOG_DBG("The service is connected, posting DistributedApp::WindowCommand::CmdServiceConnection message");
        DistributedDialog::PostServiceMessage( NEDistributedApp::WindowCommand::CmdServiceConnection, 1, reinterpret_cast<LPARAM>(dispatcher_thread( )) );
    }
    else
    {
        LOG_DBG("The service is disconnected, posting DistributedApp::WindowCommand::CmdServiceConnection message");
        DistributedDialog::PostServiceMessage( NEDistributedApp::WindowCommand::CmdServiceConnection, 0, 0 );
    }

    return result;
}

void ConnectionList::broadcast_client_disconnected( const ConnectionManager::ConnectionRecord & clientData )
{
    if (mConnectionHandler.RemoveConnection(clientData))
    {
        ConnectionManager::ConnectionRecord * data = new ConnectionManager::ConnectionRecord( clientData );
        DistributedDialog::PostServiceMessage( NEDistributedApp::WindowCommand::CmdRemoveConnection, 1, reinterpret_cast<LPARAM>(data) );
    }
}

void ConnectionList::broadcast_client_connected( const ConnectionManager::ConnectionRecord & newClient )
{
    if (mConnectionHandler.AddConnection(newClient))
    {
        ConnectionManager::ConnectionRecord * data = new ConnectionManager::ConnectionRecord(newClient);
        DistributedDialog::PostServiceMessage(NEDistributedApp::WindowCommand::CmdAddConnection, 1, reinterpret_cast<LPARAM>(data));
    }
}

void ConnectionList::response_register_connection( const ConnectionManager::ConnectionRecord & connection, const ConnectionManager::ListConnections & connectionList, bool success )
{
    LOG_SCOPE( chatter_ConnectionList, response_register_connection );
    LOG_DBG("[ %s ] to register connection [ %s ]", success ? "SUCCEEDED" : "FAILED", connection.nickName.as_string());

    if ( success )
    {
        mConnectionHandler.SetNickName( connection.nickName );
        mConnectionHandler.SetCookie( connection.cookie );
        mConnectionHandler.SetTimeConnect( connection.connectTime );
        mConnectionHandler.SetTimeConnected( connection.connectedTime );
        mConnectionHandler.AddConnections(connectionList);
        mConnectionHandler.SetRegistered( true );
        DistributedDialog::PostServiceMessage(NEDistributedApp::WindowCommand::CmdClientRegistration, 1, reinterpret_cast<LPARAM>(dispatcher_thread()));
    }
    else
    {
        mConnectionHandler.SetRegistered( false );
        mConnectionHandler.SetCookie( ConnectionManager::InvalidCookie );
        mConnectionHandler.SetTimeConnect( areg::DateTime( ) );
        mConnectionHandler.SetTimeConnected( areg::DateTime( ) );
        mConnectionHandler.RemoveConnections();
        DistributedDialog::PostServiceMessage(NEDistributedApp::WindowCommand::CmdClientRegistration, 0, 0);
    }
}
