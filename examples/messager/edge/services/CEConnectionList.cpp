
#include "edge/res/stdafx.h"
#include "edge/services/CEConnectionList.hpp"
#include "areg/component/CEComponent.hpp"
#include "areg/component/CEComponentThread.hpp"
#include "edge/NEDistributedApp.hpp"
#include "edge/services/CEConnectionHandler.hpp"
#include "edge/ui/CEDistributedDialog.hpp"

CEConnectionList::CEConnectionList( const char * roleName, CEComponent & owner, CEConnectionHandler & handlerConnection )
    : CEConnectionManagerClientBase ( roleName, owner.GetMasterThread() )
    , mConnectionHandler            ( handlerConnection )
{

}

CEConnectionList::CEConnectionList( const char * roleName, CEDispatcherThread & dispThread, CEConnectionHandler & handlerConnection )
    : CEConnectionManagerClientBase ( roleName, dispThread )
    , mConnectionHandler            ( handlerConnection )
{

}

CEConnectionList::~CEConnectionList( )
{
}

bool CEConnectionList::ServiceConnected( bool isConnected, CEProxyBase & proxy )
{
    bool result = false;
    if ( CEConnectionManagerClientBase::ServiceConnected( isConnected, proxy ) )
    {
        result = true;
        if ( isConnected )
            CEDistributedDialog::PostServiceMessage( NEDistributedApp::CmdServiceConnection, 1, reinterpret_cast<LPARAM>(GetDispatcherThread( )) );
        else
            CEDistributedDialog::PostServiceMessage( NEDistributedApp::CmdServiceConnection, 0, 0 );
    }
    return result;
}

void CEConnectionList::BroadcastClientDisconnected( const NEConnectionManager::sConnection & clientData )
{
    if (mConnectionHandler.RemoveConnection(clientData))
    {
        NEConnectionManager::sConnection * data = new NEConnectionManager::sConnection( clientData );
        CEDistributedDialog::PostServiceMessage( NEDistributedApp::CmdRemoveConnection, 1, reinterpret_cast<LPARAM>(data) );
    }
}

void CEConnectionList::BroadcastClientConnected( const NEConnectionManager::sConnection & newClient )
{
    if (mConnectionHandler.AddConnection(newClient))
    {
        NEConnectionManager::sConnection * data = new NEConnectionManager::sConnection(newClient);
        CEDistributedDialog::PostServiceMessage(NEDistributedApp::CmdAddConnection, 1, reinterpret_cast<LPARAM>(data));
    }
}

void CEConnectionList::ResponseRegisterConnection( const NEConnectionManager::sConnection & connection, const NEConnectionManager::ListConnection & connectionList, const bool & success )
{
    if ( success )
    {
        mConnectionHandler.SetNickName( connection.nickName );
        mConnectionHandler.SetCookie( connection.cookie );
        mConnectionHandler.SetTimeConnect( connection.connectTime );
        mConnectionHandler.SetTimeConnected( connection.connectedTime );
        mConnectionHandler.AddConnections(connectionList);
        mConnectionHandler.SetRegistered( true );
        CEDistributedDialog::PostServiceMessage(NEDistributedApp::CmdClientRegistration, 1, reinterpret_cast<LPARAM>(GetDispatcherThread()));
    }
    else
    {
        mConnectionHandler.SetRegistered( false );
        mConnectionHandler.SetCookie( NEConnectionManager::InvalidCookie );
        mConnectionHandler.SetTimeConnect( CEDateTime( ) );
        mConnectionHandler.SetTimeConnected( CEDateTime( ) );
        mConnectionHandler.RemoveConnections();
        CEDistributedDialog::PostServiceMessage(NEDistributedApp::CmdClientRegistration, 0, 0);
    }
}
