// edge/CEPageNetworkSetup.cpp : implementation file
//

#include "edge/res/stdafx.h"
#include "edge/CEDistrbutedApp.hpp"
#include "edge/NEDistributedApp.hpp"
#include "edge/services/CENetworkSetup.hpp"
#include "edge/ui/CEPageNetworkSetup.hpp"
#include "edge/ui/CEDistributedDialog.hpp"
#include "shared/NECommonSettings.hpp"
#include "areg/src/base/CEString.hpp"
#include "areg/src/base/NESocket.hpp"
#include "areg/src/ipc/CEConnectionConfiguration.hpp"
#include "areg/src/appbase/CEApplication.hpp"
#include "edge/services/CEChatPrticipantHandler.hpp"

// CEPageNetworkSetup dialog

IMPLEMENT_DYNAMIC(CEPageNetworkSetup, CPropertyPage)

CEPageNetworkSetup::CEPageNetworkSetup( CEConnectionHandler & handlerConnection)
	: CPropertyPage (CEPageNetworkSetup::IDD)

    , mCtrlAddress      ( )
    , mCtrlPort         ( )
    , mNickName         ( )
    , mCtrlNickName     ( )
    , mBrokerPort       ( NESocket::InvalidPort )
    , mNetworkSetup     ( NULL )
    , mConnectionHandler( handlerConnection )
    , mConnectPending   ( false )
    , mRegisterPending  ( false )
    , mConnectEnable    ( FALSE )
    , mDisconnectEnabled( FALSE )
    , mRegisterEnabled  ( FALSE )
{

}

CEPageNetworkSetup::~CEPageNetworkSetup()
{
    cleanService();
}

void CEPageNetworkSetup::cleanService(void)
{
    mConnectionHandler.SetConnected(false);
    mConnectionHandler.SetRegistered(false);

    if (mNetworkSetup != NULL)
    {
        mNetworkSetup->DisconnectServicing( );

        delete mNetworkSetup;
        mNetworkSetup = NULL;
    }
}

bool CEPageNetworkSetup::isServiceConnected(void) const
{
    return (mNetworkSetup != NULL ? mNetworkSetup->IsConnected() : false);
}


void CEPageNetworkSetup::OnServiceStartup( bool isStarted, CEComponent * owner )
{
    mConnectionHandler.SetConnected( false);
    if ( isStarted )
    {
        if ( (mNetworkSetup == NULL) && (owner != NULL) )
            mNetworkSetup = DEBUG_NEW CENetworkSetup( NECommonSettings::COMP_NAME_CENTRAL_SERVER, *owner, mConnectionHandler );
    }
    else
    {
        if ( mNetworkSetup != NULL )
            delete mNetworkSetup;
        mNetworkSetup = NULL;
    }
}

void CEPageNetworkSetup::OnServiceNetwork( bool isConnected, CEDispatcherThread * ownerThread )
{
    mConnectionHandler.SetConnected( isConnected );
    if ( mRegisterPending )
    {
        if ( (mConnectionHandler.GetNickName().IsEmpty() == false) && (mConnectionHandler.GetCookie() == NECommonSettings::InvalidCookie) )
        {
            ASSERT(mNetworkSetup != NULL);
            mNetworkSetup->RequestConnet(mConnectionHandler.GetNickName(), CEDateTime::GetNow() );
        }
    }
    mConnectPending = isConnected ? false : mConnectPending;
}

void CEPageNetworkSetup::OnServiceConnection( bool isConnected, CEDispatcherThread * ownerThread )
{
    // do nothing
}

void CEPageNetworkSetup::OnClientConnection( bool isConnected, CEDispatcherThread *dispThread )
{
    mConnectPending = isConnected ? false : mConnectPending;
    mConnectionHandler.SetConnected(isConnected);
}

void CEPageNetworkSetup::OnClientRegistration( bool isRegistered, CEDispatcherThread * dispThread )
{
    mRegisterPending = isRegistered ? false : mRegisterPending;
    if ( isRegistered )
    {
        mNickName = mConnectionHandler.GetNickName().String();
        UpdateData(FALSE);
    }
    else if ( (isRegistered == false) && (mNetworkSetup != NULL) )
    {
        mNetworkSetup->RequestDiconnect( mConnectionHandler.GetNickName(), mConnectionHandler.GetCookie(), CEDateTime::GetNow());
        mConnectionHandler.ResetConnectionList( );
    }
}

void CEPageNetworkSetup::OnAddConnection( NEConnectionManager::sConnection & data )
{
    // do nothing
}

void CEPageNetworkSetup::OnRemoveConnection( NEConnectionManager::sConnection & data )
{
    // do nothing
}

void CEPageNetworkSetup::OnUpdateConnection( void )
{
    // do nothing
}

void CEPageNetworkSetup::OnDisconnectTriggered( void )
{
    if ( mNetworkSetup != NULL )
    {
        mNetworkSetup->RequestDiconnect(mConnectionHandler.GetNickName(), mConnectionHandler.GetCookie(), mConnectionHandler.GetTimeConnect());
        delete mNetworkSetup;
        mNetworkSetup = NULL;
    }
}

void CEPageNetworkSetup::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BROKER_IPADDRESS, mCtrlAddress);
    DDX_Control(pDX, IDC_BROKER_PORT, mCtrlPort);
    DDX_Text(pDX, IDC_EDIT_NICKNAME, mNickName);
    DDV_MaxChars(pDX, mNickName, 63);
    DDX_Control(pDX, IDC_EDIT_NICKNAME, mCtrlNickName);
}


BEGIN_MESSAGE_MAP(CEPageNetworkSetup, CPropertyPage)
    ON_WM_DESTROY( )
    ON_BN_CLICKED(IDC_BROKER_CONNECT, &CEPageNetworkSetup::OnClickedBrokerConnect)
    ON_BN_CLICKED(IDC_BROKER_DISCONNECT, &CEPageNetworkSetup::OnClickedBrokerDisconnect)
    ON_BN_CLICKED( IDC_BUTTON_REGISTER, &CEPageNetworkSetup::OnClickedButtonRegister )
    ON_EN_UPDATE(IDC_EDIT_NICKNAME, &CEPageNetworkSetup::OnUpdateEditNickname)
    ON_MESSAGE_VOID( WM_KICKIDLE, OnKickIdle )
    ON_UPDATE_COMMAND_UI( IDC_BROKER_CONNECT, &CEPageNetworkSetup::OnBnUpdateBrokerConnect )
    ON_UPDATE_COMMAND_UI( IDC_BROKER_DISCONNECT, &CEPageNetworkSetup::OnBnUdateBrokerDisconnect )
    ON_UPDATE_COMMAND_UI( IDC_BROKER_IPADDRESS, &CEPageNetworkSetup::OnUpdateRemoteData )
    ON_UPDATE_COMMAND_UI( IDC_BROKER_PORT, &CEPageNetworkSetup::OnUpdateRemoteData )
    ON_UPDATE_COMMAND_UI( IDC_EDIT_NICKNAME, &CEPageNetworkSetup::OnUpdateNickname )
    ON_UPDATE_COMMAND_UI( IDC_BUTTON_REGISTER, &CEPageNetworkSetup::OnUpdateButtonRegister)
END_MESSAGE_MAP()


// CEPageNetworkSetup message handlers

void CEPageNetworkSetup::OnClickedBrokerConnect()
{
    CString port;
    BYTE ip1, ip2, ip3, ip4;
    mCtrlPort.GetWindowText( port );

    CEString check( port.GetBuffer( ) );
    if ( (check.IsInteger( false ) == true) && (mCtrlAddress.GetAddress( ip1, ip2, ip3, ip4 ) == 4) )
    {
        uint32_t temp = check.ToUInt32( );
        if ( (temp != NESocket::InvalidPort) && (temp < 0xFFFFu) )
        {
            mBrokerPort = temp;
            CEString ipAddress;
            ipAddress.Format( "%u.%u.%u.%u", ip1, ip2, ip3, ip4 );
            if ( CEApplication::StartMessageRouterClient( ipAddress, mBrokerPort ) )
            {
                CEApplication::StartModel( NECommonSettings::MODEL_NAME_DISTRIBUTED_CLIENT );
                CWnd *wnd = GetDlgItem(IDC_EDIT_NICKNAME);
                wnd->EnableWindow(TRUE);
                wnd->SetFocus();
                mConnectPending = true;
            }
        }
    }
}

void CEPageNetworkSetup::OnClickedBrokerDisconnect()
{
    if ( mNetworkSetup )
    {
        CWnd * wnd = GetParentSheet();
        ::SendMessage( wnd->GetSafeHwnd(), NEDistributedApp::CmdDisconnectTriggered, 0, 0);
        mConnectionHandler.ResetConnectionList();

        CEApplication::StopModel(NECommonSettings::MODEL_NAME_DISTRIBUTED_CLIENT);
        CEApplication::StopMessageRouterClient();
        mConnectPending = false;
        mRegisterPending= false;
    }
}

void CEPageNetworkSetup::OnClickedButtonRegister( )
{
    if ( (mNetworkSetup != NULL) && (mConnectionHandler.GetRegistered() == false) )
    {
        UpdateData(TRUE);
        if ( mNickName.IsEmpty( ) == false )
        {
            CEString nickName(mNickName.GetString());
            mRegisterPending = true;
            mConnectionHandler.SetRegistered(false);
            mConnectionHandler.SetNickName(nickName);
            mNetworkSetup->RequestConnet(nickName, CEDateTime::GetNow() );
        }
        else
        {
            setFocusNickname( );
        }
    }
}

void CEPageNetworkSetup::OnUpdateEditNickname()
{
    UpdateData( TRUE );
    CEString nickName(mNickName.GetString());
    if (mNickName.GetLength() != static_cast<int>(nickName.MakeAlphanumeric()))
    {
        mNickName = nickName.GetBuffer();
        UpdateData(FALSE);
        mCtrlNickName.SetSel(mNickName.GetLength(), mNickName.GetLength(), FALSE);
    }
}

void CEPageNetworkSetup::OnKickIdle( )
{
    UpdateDialogControls( this, FALSE );
}

void CEPageNetworkSetup::OnBnUpdateBrokerConnect( CCmdUI* pCmdUI )
{
    CString port;
    BYTE ip1, ip2, ip3, ip4;
    mCtrlPort.GetWindowText( port );

    CEString check( port.GetBuffer( ) );
    if ( (check.IsInteger( false ) == true) && (mCtrlAddress.GetAddress( ip1, ip2, ip3, ip4 ) == 4) )
    {
        uint32_t temp = check.ToUInt32( );
        mBrokerPort = temp > 0xFFFFu ? 0xFFFFu : temp;
    }
    else
    {
        mBrokerPort = 0xFFFFu;
    }
    
    if ( (CEApplication::IsRouterConnected( ) == false) && (mBrokerPort < 0xFFFFu) && (mCtrlAddress.IsBlank( ) == FALSE) )
    {
        pCmdUI->Enable( TRUE );
        if ( mConnectEnable == FALSE )
        {
            CButton * btn = reinterpret_cast<CButton *>(GetDlgItem( IDC_BROKER_CONNECT ));
            btn->SetFocus( );
        }
        mConnectEnable = TRUE;
    }
    else
    {
        pCmdUI->Enable( FALSE );
        mConnectEnable = FALSE;
    }
}

void CEPageNetworkSetup::OnBnUdateBrokerDisconnect( CCmdUI* pCmdUI )
{
    if ( CEApplication::IsRouterConnected( ) )
    {
        pCmdUI->Enable( TRUE );
        if ( mDisconnectEnabled == FALSE )
        {
            CButton * btn = reinterpret_cast<CButton *>(GetDlgItem( IDC_BROKER_DISCONNECT ));
            btn->SetFocus( );
        }
        mDisconnectEnabled = TRUE;
    }
    else
    {
        pCmdUI->Enable( FALSE );
        mDisconnectEnabled = FALSE;
    }
}

void CEPageNetworkSetup::OnUpdateRemoteData( CCmdUI* pCmdUI )
{
    pCmdUI->Enable( CEApplication::IsRouterConnected( ) ? FALSE : TRUE );
}

void CEPageNetworkSetup::OnUpdateNickname( CCmdUI* pCmdUI )
{
    if ( mRegisterPending )
    {
        pCmdUI->Enable(FALSE);
    }
    else
    {
        if ( canRegistered() )
        {
            pCmdUI->Enable( TRUE );
            if ( (mNickName.IsEmpty( ) == TRUE) && (mRegisterEnabled == FALSE) )
            {
                setFocusNickname( );
            }
            mRegisterEnabled = TRUE;
        }
        else
        {
            pCmdUI->Enable( FALSE );
            mRegisterEnabled = FALSE;
        }
    }
}

void CEPageNetworkSetup::OnUpdateButtonRegister( CCmdUI* pCmdUI )
{
    UpdateData( TRUE );
    if ( mRegisterPending )
    {
        pCmdUI->Enable( mNickName.IsEmpty() == FALSE );
    }
    else
    {
        if ( canRegistered( ) )
        {
            pCmdUI->Enable( mNickName.IsEmpty() == FALSE );
        }
        else
        {
            pCmdUI->Enable(FALSE);
        }
    }
}

bool CEPageNetworkSetup::canRegistered( void ) const
{
    return (CEApplication::IsRouterConnected( ) ? mConnectionHandler.GetRegistered() == false : false);
}

void CEPageNetworkSetup::setFocusNickname( void ) const
{
    CEdit * nick = reinterpret_cast<CEdit *>(GetDlgItem( IDC_EDIT_NICKNAME ));
    if ( nick != NULL )
    {
        nick->SetFocus( );
        nick->SetSel( 0, mNickName.GetLength( ), TRUE );
    }
}

BOOL CEPageNetworkSetup::OnInitDialog( )
{
    CPropertyPage::OnInitDialog( );

    mCtrlAddress.SetAddress( 127, 0, 0, 1 );
    mCtrlPort.SetWindowText( _T( "8181" ) );

    CEConnectionConfiguration config;
    if ( config.LoadConfiguration( NEApplication::DEFAULT_ROUTER_CONFIG_FILE ) )
    {
        unsigned char field0, field1, field2, field3;
        if ( config.GetConnectionHostIpAddress( field0, field1, field2, field3, NERemoteService::ConnectionTcpip ) )
        {
            mBrokerPort = static_cast<USHORT>(config.GetConnectionPort( NERemoteService::ConnectionTcpip ));
            CString port( CEString::UInt32ToString( mBrokerPort ).String( ) );
            mCtrlAddress.SetAddress( field0, field1, field2, field3 );
            mCtrlPort.SetWindowText( port );
        }
    }

    GetDlgItem(IDC_BROKER_CONNECT)->SetFocus();
    UpdateDialogControls( this, FALSE );

    return TRUE;  // return TRUE unless you set the focus to a control
}

void CEPageNetworkSetup::OnDestroy( )
{
    CPropertyPage::OnDestroy();

    if ( mNetworkSetup != NULL )
    {
        mNetworkSetup->RequestDiconnect( mConnectionHandler.GetNickName( ), mConnectionHandler.GetCookie( ), mConnectionHandler.GetTimeConnect( ) );
        delete mNetworkSetup;
        mNetworkSetup = NULL;
    }
}

void CEPageNetworkSetup::OnDefaultClicked( void )
{
    CButton * btnConnect    = reinterpret_cast<CButton *>(GetDlgItem( IDC_BROKER_CONNECT ));
    CButton * btnDisconnect = reinterpret_cast<CButton *>(GetDlgItem( IDC_BROKER_DISCONNECT ));
    CButton * btnRegister   = reinterpret_cast<CButton *>(GetDlgItem( IDC_BUTTON_REGISTER ));
    if ( (btnConnect != NULL) && (btnDisconnect != NULL) && (btnRegister != NULL) )
    {
        if ( mRegisterEnabled )
        {
            PostMessage( WM_COMMAND, MAKEWPARAM( IDC_BUTTON_REGISTER, BN_CLICKED ), reinterpret_cast<LPARAM>(btnRegister->GetSafeHwnd( )) );
        }
        else if ( mDisconnectEnabled )
        {
            PostMessage( WM_COMMAND, MAKEWPARAM( IDC_BROKER_DISCONNECT, BN_CLICKED ), reinterpret_cast<LPARAM>(btnConnect->GetSafeHwnd( )) );
        }
        else if (mConnectEnable)
        {
            PostMessage( WM_COMMAND, MAKEWPARAM( IDC_BROKER_CONNECT, BN_CLICKED ), reinterpret_cast<LPARAM>(btnDisconnect->GetSafeHwnd( )) );
        }
    }
}
