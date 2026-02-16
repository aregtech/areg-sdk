
#include "chatter/res/stdafx.h"
#include "chatter/services/ConnectionList.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentThread.hpp"
#include "chatter/DistributedAppDefs.hpp"
#include "chatter/services/ConnectionHandler.hpp"
#include "chatter/ui/DistributedDialog.hpp"

DEF_LOG_SCOPE(chatter_ConnectionList_serviceConnected);
DEF_LOG_SCOPE(chatter_ConnectionList_responseRegisterConnection);

ConnectionList::ConnectionList( const char * roleName, Component & owner, ConnectionHandler & handlerConnection )
    : ConnectionManagerClientBase ( roleName, owner.getMasterThread() )
    , mConnectionHandler            ( handlerConnection )
{

}

ConnectionList::ConnectionList( const char * roleName, DispatcherThread & dispThread, ConnectionHandler & handlerConnection )
    : ConnectionManagerClientBase ( roleName, dispThread )
    , mConnectionHandler            ( handlerConnection )
{

}

bool ConnectionList::serviceConnected( NEService::eServiceConnection status, ProxyBase & proxy )
{
    LOG_SCOPE(chatter_ConnectionList_serviceConnected);
    bool result = ConnectionManagerClientBase::serviceConnected( status, proxy );
    if ( isConnected( ) )
    {
        LOG_DBG("The service is connected, posting DistributedApp::eWndCommands::CmdServiceConnection message");
        DistributedDialog::PostServiceMessage( NEDistributedApp::eWndCommands::CmdServiceConnection, 1, reinterpret_cast<LPARAM>(getDispatcherThread( )) );
    }
    else
    {
        LOG_DBG("The service is disconnected, posting DistributedApp::eWndCommands::CmdServiceConnection message");
        DistributedDialog::PostServiceMessage( NEDistributedApp::eWndCommands::CmdServiceConnection, 0, 0 );
    }

    return result;
}

void ConnectionList::broadcastClientDisconnected( const ConnectionManager::sConnection & clientData )
{
    if (mConnectionHandler.RemoveConnection(clientData))
    {
        ConnectionManager::sConnection * data = new ConnectionManager::sConnection( clientData );
        DistributedDialog::PostServiceMessage( NEDistributedApp::eWndCommands::CmdRemoveConnection, 1, reinterpret_cast<LPARAM>(data) );
    }
}

void ConnectionList::broadcastClientConnected( const ConnectionManager::sConnection & newClient )
{
    if (mConnectionHandler.AddConnection(newClient))
    {
        ConnectionManager::sConnection * data = new ConnectionManager::sConnection(newClient);
        DistributedDialog::PostServiceMessage(NEDistributedApp::eWndCommands::CmdAddConnection, 1, reinterpret_cast<LPARAM>(data));
    }
}

void ConnectionList::responseRegisterConnection( const ConnectionManager::sConnection & connection, const ConnectionManager::ListConnections & connectionList, bool success )
{
    LOG_SCOPE(chatter_ConnectionList_responseRegisterConnection);
    LOG_DBG("[ %s ] to register connection [ %s ]", success ? "SUCCEEDED" : "FAILED", connection.nickName.getString());

    if ( success )
    {
        mConnectionHandler.SetNickName( connection.nickName );
        mConnectionHandler.SetCookie( connection.cookie );
        mConnectionHandler.SetTimeConnect( connection.connectTime );
        mConnectionHandler.SetTimeConnected( connection.connectedTime );
        mConnectionHandler.AddConnections(connectionList);
        mConnectionHandler.SetRegistered( true );
        DistributedDialog::PostServiceMessage(NEDistributedApp::eWndCommands::CmdClientRegistration, 1, reinterpret_cast<LPARAM>(getDispatcherThread()));
    }
    else
    {
        mConnectionHandler.SetRegistered( false );
        mConnectionHandler.SetCookie( ConnectionManager::InvalidCookie );
        mConnectionHandler.SetTimeConnect( DateTime( ) );
        mConnectionHandler.SetTimeConnected( DateTime( ) );
        mConnectionHandler.RemoveConnections();
        DistributedDialog::PostServiceMessage(NEDistributedApp::eWndCommands::CmdClientRegistration, 0, 0);
    }
}
