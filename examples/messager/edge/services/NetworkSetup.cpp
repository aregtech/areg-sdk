
#include "edge/res/stdafx.h"
#include "edge/services/NetworkSetup.hpp"
#include "edge/services/ConnectionHandler.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentThread.hpp"
#include "edge/NEDistributedApp.hpp"
#include "edge/ui/DistributedDialog.hpp"

#define MAKE_HWND(wnd)      reinterpret_cast<HWND>(wnd)

NetworkSetup::NetworkSetup( const char * roleName, Component & owner, ConnectionHandler & handlerConnection )
    : ConnectionManagerClientBase (roleName, owner.getMasterThread() )

    , mConnectionHandler( handlerConnection )
{
}


NetworkSetup::~NetworkSetup( )
{
}

void NetworkSetup::responseConnect( const String & nickName, unsigned int cookie, const DateTime & dateTime, NEConnectionManager::eConnectionResult result )
{
    DateTime timeConnected = DateTime::getNow();
    if (result == NEConnectionManager::ConnectionAccepted)
    {
        mConnectionHandler.SetNickName(nickName);
        mConnectionHandler.SetConnectCookie(cookie);
        mConnectionHandler.SetTimeConnect(dateTime);
        mConnectionHandler.SetTimeConnected(timeConnected);
    }
    else
    {
        mConnectionHandler.SetNickName(nickName);
        mConnectionHandler.SetCookie(NEConnectionManager::InvalidCookie);
        mConnectionHandler.SetConnectCookie(NEConnectionManager::InvalidCookie);
        mConnectionHandler.SetTimeConnect(DateTime());
        mConnectionHandler.SetTimeConnected(DateTime());
    }
    mConnectionHandler.SetRegistered( false );
    bool isConnected = result == NEConnectionManager::ConnectionAccepted;
    DispatcherThread *dispThread = getDispatcherThread();
    DistributedDialog::PostServiceMessage( NEDistributedApp::CmdClientConnection, isConnected ? 1 : 0, reinterpret_cast<LPARAM>(dispThread) );
}

bool NetworkSetup::serviceConnected( bool isConnected, ProxyBase & proxy )
{
    bool result = false;
    if ( ConnectionManagerClientBase::serviceConnected( true, proxy ) )
    {
        result = true;
        if ( isConnected )
            DistributedDialog::PostServiceMessage( NEDistributedApp::CmdServiceNetwork, 1, reinterpret_cast<LPARAM>(getDispatcherThread()) );
        else
            DistributedDialog::PostServiceMessage( NEDistributedApp::CmdServiceNetwork, 0, 0 );
    }
    return result;
}

void NetworkSetup::DisconnectServicing(void)
{
    if (mConnectionHandler.GetRegistered())
    {
        mConnectionHandler.SetRegistered(false);
        requestDiconnect( mConnectionHandler.GetNickName(), mConnectionHandler.GetCookie(), mConnectionHandler.GetTimeConnect() );
    }
}
