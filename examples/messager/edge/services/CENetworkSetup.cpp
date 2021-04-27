
#include "edge/res/stdafx.h"
#include "edge/services/CENetworkSetup.hpp"
#include "edge/services/CEConnectionHandler.hpp"
#include "areg/component/CEComponent.hpp"
#include "areg/component/CEComponentThread.hpp"
#include "edge/NEDistributedApp.hpp"
#include "edge/ui/CEDistributedDialog.hpp"

#define MAKE_HWND(wnd)      reinterpret_cast<HWND>(wnd)

CENetworkSetup::CENetworkSetup( const char * roleName, CEComponent & owner, CEConnectionHandler & handlerConnection )
    : CEConnectionManagerClientBase (roleName, owner.GetMasterThread() )

    , mConnectionHandler( handlerConnection )
{
}


CENetworkSetup::~CENetworkSetup( )
{
}

void CENetworkSetup::ResponseConnect( const CEString & nickName, const uint32_t & cookie, const CEDateTime & dateTime, const NEConnectionManager::eConnectionResult & result )
{
    CEDateTime timeConnected = CEDateTime::GetNow();
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
        mConnectionHandler.SetTimeConnect(CEDateTime());
        mConnectionHandler.SetTimeConnected(CEDateTime());
    }
    mConnectionHandler.SetRegistered( false );
    bool isConnected = result == NEConnectionManager::ConnectionAccepted;
    CEDispatcherThread *dispThread = GetDispatcherThread();
    CEDistributedDialog::PostServiceMessage( NEDistributedApp::CmdClientConnection, isConnected ? 1 : 0, reinterpret_cast<LPARAM>(dispThread) );
}

bool CENetworkSetup::ServiceConnected( bool isConnected, CEProxyBase & proxy )
{
    bool result = false;
    if ( CEConnectionManagerClientBase::ServiceConnected( true, proxy ) )
    {
        result = true;
        if ( isConnected )
            CEDistributedDialog::PostServiceMessage( NEDistributedApp::CmdServiceNetwork, 1, reinterpret_cast<LPARAM>(GetDispatcherThread()) );
        else
            CEDistributedDialog::PostServiceMessage( NEDistributedApp::CmdServiceNetwork, 0, 0 );
    }
    return result;
}

void CENetworkSetup::DisconnectServicing(void)
{
    if (mConnectionHandler.GetRegistered())
    {
        mConnectionHandler.SetRegistered(false);
        RequestDiconnect( mConnectionHandler.GetNickName(), mConnectionHandler.GetCookie(), mConnectionHandler.GetTimeConnect() );
    }
}
