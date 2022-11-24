
#include "chatter/res/stdafx.h"
#include "chatter/services/ConnectionList.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentThread.hpp"
#include "chatter/NEDistributedApp.hpp"
#include "chatter/services/ConnectionHandler.hpp"
#include "chatter/ui/DistributedDialog.hpp"

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

ConnectionList::~ConnectionList( )
{
}

bool ConnectionList::serviceConnected( bool isConnected, ProxyBase & proxy )
{
    bool result = false;
    if ( ConnectionManagerClientBase::serviceConnected( isConnected, proxy ) )
    {
        result = true;
        if ( isConnected )
            DistributedDialog::PostServiceMessage( NEDistributedApp::eWndCommands::CmdServiceConnection, 1, reinterpret_cast<LPARAM>(getDispatcherThread( )) );
        else
            DistributedDialog::PostServiceMessage( NEDistributedApp::eWndCommands::CmdServiceConnection, 0, 0 );
    }
    return result;
}

void ConnectionList::broadcastClientDisconnected( const NEConnectionManager::sConnection & clientData )
{
    if (mConnectionHandler.RemoveConnection(clientData))
    {
        NEConnectionManager::sConnection * data = new NEConnectionManager::sConnection( clientData );
        DistributedDialog::PostServiceMessage( NEDistributedApp::eWndCommands::CmdRemoveConnection, 1, reinterpret_cast<LPARAM>(data) );
    }
}

void ConnectionList::broadcastClientConnected( const NEConnectionManager::sConnection & newClient )
{
    if (mConnectionHandler.AddConnection(newClient))
    {
        NEConnectionManager::sConnection * data = new NEConnectionManager::sConnection(newClient);
        DistributedDialog::PostServiceMessage(NEDistributedApp::eWndCommands::CmdAddConnection, 1, reinterpret_cast<LPARAM>(data));
    }
}

void ConnectionList::responseRegisterConnection( const NEConnectionManager::sConnection & connection, const NEConnectionManager::ListConnection & connectionList, bool success )
{
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
        mConnectionHandler.SetCookie( NEConnectionManager::InvalidCookie );
        mConnectionHandler.SetTimeConnect( DateTime( ) );
        mConnectionHandler.SetTimeConnected( DateTime( ) );
        mConnectionHandler.RemoveConnections();
        DistributedDialog::PostServiceMessage(NEDistributedApp::eWndCommands::CmdClientRegistration, 0, 0);
    }
}
