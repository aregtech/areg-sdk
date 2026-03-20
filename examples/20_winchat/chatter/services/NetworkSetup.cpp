
#include "chatter/res/stdafx.h"
#include "chatter/services/NetworkSetup.hpp"
#include "chatter/services/ConnectionHandler.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentThread.hpp"
#include "chatter/DistributedAppDefs.hpp"
#include "chatter/ui/DistributedDialog.hpp"

DEF_LOG_SCOPE(chatter_NetworkSetup_serviceConnected);
DEF_LOG_SCOPE(chatter_NetworkSetup_response_connect);

NetworkSetup::NetworkSetup( const char * roleName, areg::Component & owner, ConnectionHandler & handlerConnection )
    : ConnectionManagerConsumerBase (roleName, owner.master_thread() )

    , mConnectionHandler( handlerConnection )
{
}

void NetworkSetup::response_connect( const areg::String & nickName, uint32_t cookie, const areg::DateTime & dateTime, ConnectionManager::ConnectionResult result )
{
    LOG_SCOPE(chatter_NetworkSetup_response_connect);
    LOG_DBG("Got connection [ %s ], cookie [ %u ], connection result [ %s ]", nickName.as_string(), cookie, ConnectionManager::as_string(result));
    areg::DateTime timeConnected = areg::DateTime::now();

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
        mConnectionHandler.SetTimeConnect(areg::DateTime());
        mConnectionHandler.SetTimeConnected(areg::DateTime());
    }

    mConnectionHandler.SetRegistered( false );
    bool isConnected = result == ConnectionManager::ConnectionResult::Accepted;
    areg::DispatcherThread *dispThread = dispatcher_thread();
    DistributedDialog::PostServiceMessage( NEDistributedApp::WindowCommand::CmdClientConnection, isConnected ? 1 : 0, reinterpret_cast<LPARAM>(dispThread) );
}

bool NetworkSetup::service_connected( areg::ServiceConnectionState status, areg::ProxyBase & proxy )
{
    LOG_SCOPE(chatter_NetworkSetup_serviceConnected);

    bool result = ConnectionManagerConsumerBase::service_connected( status, proxy );
    if ( is_connected( ) )
    {
        LOG_DBG("The service is connected, network setup can start. posting NEDistributedApp::WindowCommand::CmdServiceNetwork message");
        DistributedDialog::PostServiceMessage( NEDistributedApp::WindowCommand::CmdServiceNetwork, 1, reinterpret_cast<LPARAM>(dispatcher_thread( )) );
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
        request_disconnect( mConnectionHandler.GetNickName(), mConnectionHandler.GetCookie(), mConnectionHandler.GetTimeConnect() );
    }
}
