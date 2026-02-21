
#include "chatter/res/stdafx.h"
#include "chatter/services/NetworkSetup.hpp"
#include "chatter/services/ConnectionHandler.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentThread.hpp"
#include "chatter/DistributedAppDefs.hpp"
#include "chatter/ui/DistributedDialog.hpp"

DEF_LOG_SCOPE(chatter_NetworkSetup_serviceConnected);
DEF_LOG_SCOPE(chatter_NetworkSetup_responseConnect);

NetworkSetup::NetworkSetup( const char * roleName, Component & owner, ConnectionHandler & handlerConnection )
    : ConnectionManagerClientBase (roleName, owner.getMasterThread() )

    , mConnectionHandler( handlerConnection )
{
}

void NetworkSetup::responseConnect( const String & nickName, uint32_t cookie, const DateTime & dateTime, ConnectionManager::ConnectionResult result )
{
    LOG_SCOPE(chatter_NetworkSetup_responseConnect);
    LOG_DBG("Got connection [ %s ], cookie [ %u ], connection result [ %s ]", nickName.getString(), cookie, ConnectionManager::getString(result));
    DateTime timeConnected = DateTime::getNow();

    if (result == ConnectionManager::ConnectionResult::Accepted)
    {
        mConnectionHandler.SetNickName(nickName);
        mConnectionHandler.SetConnectCookie(cookie);
        mConnectionHandler.SetTimeConnect(dateTime);
        mConnectionHandler.SetTimeConnected(timeConnected);
    }
    else
    {
        mConnectionHandler.SetNickName(nickName);
        mConnectionHandler.SetCookie(ConnectionManager::InvalidCookie);
        mConnectionHandler.SetConnectCookie(ConnectionManager::InvalidCookie);
        mConnectionHandler.SetTimeConnect(DateTime());
        mConnectionHandler.SetTimeConnected(DateTime());
    }

    mConnectionHandler.SetRegistered( false );
    bool isConnected = result == ConnectionManager::ConnectionResult::Accepted;
    DispatcherThread *dispThread = getDispatcherThread();
    DistributedDialog::PostServiceMessage( NEDistributedApp::WindowCommand::CmdClientConnection, isConnected ? 1 : 0, reinterpret_cast<LPARAM>(dispThread) );
}

bool NetworkSetup::serviceConnected( NEService::ServiceConnectionState status, ProxyBase & proxy )
{
    LOG_SCOPE(chatter_NetworkSetup_serviceConnected);

    bool result = ConnectionManagerClientBase::serviceConnected( status, proxy );
    if ( isConnected( ) )
    {
        LOG_DBG("The service is connected, network setup can start. posting NEDistributedApp::WindowCommand::CmdServiceNetwork message");
        DistributedDialog::PostServiceMessage( NEDistributedApp::WindowCommand::CmdServiceNetwork, 1, reinterpret_cast<LPARAM>(getDispatcherThread( )) );
    }
    else
    {
        LOG_DBG("The service is disconnected, network setup stops. posting NEDistributedApp::WindowCommand::CmdServiceNetwork message");
        DistributedDialog::PostServiceMessage( NEDistributedApp::WindowCommand::CmdServiceNetwork, 0, 0 );
    }

    return result;
}

void NetworkSetup::DisconnectServicing()
{
    if (mConnectionHandler.GetRegistered())
    {
        mConnectionHandler.SetRegistered(false);
        requestDisconnect( mConnectionHandler.GetNickName(), mConnectionHandler.GetCookie(), mConnectionHandler.GetTimeConnect() );
    }
}
