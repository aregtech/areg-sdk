// chatter/PageNetworkSetup.cpp : implementation file
//

#include "chatter/res/stdafx.h"
#include "chatter/DistrbutedApp.hpp"
#include "chatter/NEDistributedApp.hpp"
#include "chatter/services/NetworkSetup.hpp"
#include "chatter/ui/PageNetworkSetup.hpp"
#include "chatter/ui/DistributedDialog.hpp"
#include "generated/NECommon.hpp"
#include "areg/base/String.hpp"
#include "areg/base/NESocket.hpp"
#include "areg/ipc/ConnectionConfiguration.hpp"
#include "areg/appbase/Application.hpp"
#include "chatter/services/ChatPrticipantHandler.hpp"

#define FIRST_MESSAGE       (WM_USER + 10 + static_cast<unsigned int>(NEDistributedApp::eWndCommands::CmdFirst))
#define MAKE_MESSAGE(elem)  (static_cast<unsigned int>(elem) + FIRST_MESSAGE)

// PageNetworkSetup dialog

IMPLEMENT_DYNAMIC(PageNetworkSetup, CPropertyPage)

PageNetworkSetup::PageNetworkSetup( ConnectionHandler & handlerConnection)
	: CPropertyPage (PageNetworkSetup::IDD)

    , mCtrlAddress      ( )
    , mCtrlPort         ( )
    , mNickName         ( )
    , mCtrlNickName     ( )
    , mBrokerPort       ( NESocket::InvalidPort )
    , mNetworkSetup     ( nullptr )
    , mConnectionHandler( handlerConnection )
    , mConnectPending   ( false )
    , mRegisterPending  ( false )
    , mConnectEnable    ( FALSE )
    , mDisconnectEnabled( FALSE )
    , mRegisterEnabled  ( FALSE )
{

}

PageNetworkSetup::~PageNetworkSetup()
{
    cleanService();
}

void PageNetworkSetup::cleanService(void)
{
    mConnectionHandler.SetConnected(false);
    mConnectionHandler.SetRegistered(false);

    if (mNetworkSetup != nullptr)
    {
        mNetworkSetup->DisconnectServicing( );

        delete mNetworkSetup;
        mNetworkSetup = nullptr;
    }
}

bool PageNetworkSetup::isServiceConnected(void) const
{
    return (mNetworkSetup != nullptr ? mNetworkSetup->isConnected() : false);
}


void PageNetworkSetup::OnServiceStartup( bool isStarted, Component * owner )
{
    mConnectionHandler.SetConnected( false);
    if ( isStarted )
    {
        if ( (mNetworkSetup == nullptr) && (owner != nullptr) )
            mNetworkSetup = DEBUG_NEW NetworkSetup( NECommon::COMP_NAME_CENTRAL_SERVER, *owner, mConnectionHandler );
    }
    else
    {
        if ( mNetworkSetup != nullptr )
            delete mNetworkSetup;
        mNetworkSetup = nullptr;
    }
}

void PageNetworkSetup::OnServiceNetwork( bool isConnected, DispatcherThread * ownerThread )
{
    mConnectionHandler.SetConnected( isConnected );
    if ( mRegisterPending )
    {
        if ( (mConnectionHandler.GetNickName().isEmpty() == false) && (mConnectionHandler.GetCookie() == NECommon::InvalidCookie) )
        {
            ASSERT(mNetworkSetup != nullptr);
            mNetworkSetup->requestConnect(mConnectionHandler.GetNickName(), DateTime::getNow() );
        }
    }
    mConnectPending = isConnected ? false : mConnectPending;
}

void PageNetworkSetup::OnServiceConnection( bool isConnected, DispatcherThread * ownerThread )
{
    // do nothing
}

void PageNetworkSetup::OnClientConnection( bool isConnected, DispatcherThread *dispThread )
{
    mConnectPending = isConnected ? false : mConnectPending;
    mConnectionHandler.SetConnected(isConnected);
}

void PageNetworkSetup::OnClientRegistration( bool isRegistered, DispatcherThread * dispThread )
{
    mRegisterPending = isRegistered ? false : mRegisterPending;
    if ( isRegistered )
    {
        mNickName = mConnectionHandler.GetNickName().getString();
        UpdateData(FALSE);
    }
    else if ( (isRegistered == false) && (mNetworkSetup != nullptr) )
    {
        mNetworkSetup->requestDisconnect( mConnectionHandler.GetNickName(), mConnectionHandler.GetCookie(), DateTime::getNow());
        mConnectionHandler.ResetConnectionList( );
    }
}

void PageNetworkSetup::OnAddConnection( NEConnectionManager::sConnection & data )
{
    // do nothing
}

void PageNetworkSetup::OnRemoveConnection( NEConnectionManager::sConnection & data )
{
    // do nothing
}

void PageNetworkSetup::OnUpdateConnection( void )
{
    // do nothing
}

void PageNetworkSetup::OnDisconnectTriggered( void )
{
    if ( mNetworkSetup != nullptr )
    {
        mNetworkSetup->requestDisconnect(mConnectionHandler.GetNickName(), mConnectionHandler.GetCookie(), mConnectionHandler.GetTimeConnect());
        delete mNetworkSetup;
        mNetworkSetup = nullptr;
    }
}

void PageNetworkSetup::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BROKER_IPADDRESS, mCtrlAddress);
    DDX_Control(pDX, IDC_BROKER_PORT, mCtrlPort);
    DDX_Text(pDX, IDC_EDIT_NICKNAME, mNickName);
    DDV_MaxChars(pDX, mNickName, 63);
    DDX_Control(pDX, IDC_EDIT_NICKNAME, mCtrlNickName);
}


BEGIN_MESSAGE_MAP(PageNetworkSetup, CPropertyPage)
    ON_WM_DESTROY( )
    ON_BN_CLICKED(IDC_BROKER_CONNECT, &PageNetworkSetup::OnClickedBrokerConnect)
    ON_BN_CLICKED(IDC_BROKER_DISCONNECT, &PageNetworkSetup::OnClickedBrokerDisconnect)
    ON_BN_CLICKED( IDC_BUTTON_REGISTER, &PageNetworkSetup::OnClickedButtonRegister )
    ON_EN_UPDATE(IDC_EDIT_NICKNAME, &PageNetworkSetup::OnUpdateEditNickname)
    ON_MESSAGE_VOID( WM_KICKIDLE, OnKickIdle )
    ON_UPDATE_COMMAND_UI( IDC_BROKER_CONNECT, &PageNetworkSetup::OnBnUpdateBrokerConnect )
    ON_UPDATE_COMMAND_UI( IDC_BROKER_DISCONNECT, &PageNetworkSetup::OnBnUdateBrokerDisconnect )
    ON_UPDATE_COMMAND_UI( IDC_BROKER_IPADDRESS, &PageNetworkSetup::OnUpdateRemoteData )
    ON_UPDATE_COMMAND_UI( IDC_BROKER_PORT, &PageNetworkSetup::OnUpdateRemoteData )
    ON_UPDATE_COMMAND_UI( IDC_EDIT_NICKNAME, &PageNetworkSetup::OnUpdateNickname )
    ON_UPDATE_COMMAND_UI( IDC_BUTTON_REGISTER, &PageNetworkSetup::OnUpdateButtonRegister)
END_MESSAGE_MAP()


// PageNetworkSetup message handlers

void PageNetworkSetup::OnClickedBrokerConnect()
{
    CString port;
    BYTE ip1, ip2, ip3, ip4;
    mCtrlPort.GetWindowText( port );

    String check( port.GetBuffer( ) );
    if ( (check.isNumeric( false ) == true) && (mCtrlAddress.GetAddress( ip1, ip2, ip3, ip4 ) == 4) )
    {
        uint32_t temp = check.toUInt32( );
        if ( (temp != NESocket::InvalidPort) && (temp < 0xFFFFu) )
        {
            mBrokerPort = temp;
            String ipAddress;
            ipAddress.format( "%u.%u.%u.%u", ip1, ip2, ip3, ip4 );
            if ( Application::startMessageRouting( ipAddress, mBrokerPort ) )
            {
                Application::loadModel( NECommon::MODEL_NAME_DISTRIBUTED_CLIENT );
                CWnd *wnd = GetDlgItem(IDC_EDIT_NICKNAME);
                wnd->EnableWindow(TRUE);
                wnd->SetFocus();
                mConnectPending = true;
            }
        }
    }
}

void PageNetworkSetup::OnClickedBrokerDisconnect()
{
    if ( mNetworkSetup )
    {
        CWnd * wnd = GetParentSheet();
        ::SendMessage( wnd->GetSafeHwnd(), MAKE_MESSAGE(NEDistributedApp::eWndCommands::CmdDisconnectTriggered), 0, 0);
        mConnectionHandler.ResetConnectionList();

        Application::unloadModel(NECommon::MODEL_NAME_DISTRIBUTED_CLIENT);
        Application::stopMessageRouting();
        mConnectPending = false;
        mRegisterPending= false;
    }
}

void PageNetworkSetup::OnClickedButtonRegister( )
{
    if ( (mNetworkSetup != nullptr) && (mConnectionHandler.GetRegistered() == false) )
    {
        UpdateData(TRUE);
        if ( mNickName.IsEmpty( ) == false )
        {
            String nickName(mNickName.GetString());
            mRegisterPending = true;
            mConnectionHandler.SetRegistered(false);
            mConnectionHandler.SetNickName(nickName);
            mNetworkSetup->requestConnect(nickName, DateTime::getNow() );
        }
        else
        {
            setFocusNickname( );
        }
    }
}

void PageNetworkSetup::OnUpdateEditNickname()
{
    UpdateData( TRUE );
    String nickName(mNickName.GetString());
    if (mNickName.GetLength() != nickName.makeAlphanumeric().getLength())
    {
        mNickName = nickName.getBuffer();
        UpdateData(FALSE);
        mCtrlNickName.SetSel(mNickName.GetLength(), mNickName.GetLength(), FALSE);
    }
}

void PageNetworkSetup::OnKickIdle( )
{
    UpdateDialogControls( this, FALSE );
}

void PageNetworkSetup::OnBnUpdateBrokerConnect( CCmdUI* pCmdUI )
{
    CString port;
    BYTE ip1, ip2, ip3, ip4;
    mCtrlPort.GetWindowText( port );

    String check( port.GetBuffer( ) );
    if ( (check.isNumeric( false ) == true) && (mCtrlAddress.GetAddress( ip1, ip2, ip3, ip4 ) == 4) )
    {
        uint32_t temp = check.toUInt32( );
        mBrokerPort = temp > 0xFFFFu ? 0xFFFFu : temp;
    }
    else
    {
        mBrokerPort = 0xFFFFu;
    }
    
    if ( (Application::isRouterConnected( ) == false) && (mBrokerPort < 0xFFFFu) && (mCtrlAddress.IsBlank( ) == FALSE) )
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

void PageNetworkSetup::OnBnUdateBrokerDisconnect( CCmdUI* pCmdUI )
{
    if ( Application::isRouterConnected( ) )
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

void PageNetworkSetup::OnUpdateRemoteData( CCmdUI* pCmdUI )
{
    pCmdUI->Enable( Application::isRouterConnected( ) ? FALSE : TRUE );
}

void PageNetworkSetup::OnUpdateNickname( CCmdUI* pCmdUI )
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

void PageNetworkSetup::OnUpdateButtonRegister( CCmdUI* pCmdUI )
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

bool PageNetworkSetup::canRegistered( void ) const
{
    return (Application::isRouterConnected( ) ? mConnectionHandler.GetRegistered() == false : false);
}

void PageNetworkSetup::setFocusNickname( void ) const
{
    CEdit * nick = reinterpret_cast<CEdit *>(GetDlgItem( IDC_EDIT_NICKNAME ));
    if ( nick != nullptr )
    {
        nick->SetFocus( );
        nick->SetSel( 0, mNickName.GetLength( ), TRUE );
    }
}

BOOL PageNetworkSetup::OnInitDialog( )
{
    CPropertyPage::OnInitDialog( );

    mCtrlAddress.SetAddress( 127, 0, 0, 1 );
    mCtrlPort.SetWindowText( _T( "8181" ) );

    ConnectionConfiguration config;
    if ( config.loadConfiguration( NEApplication::DEFAULT_ROUTER_CONFIG_FILE.data() ) )
    {
        unsigned char field0, field1, field2, field3;
        if ( config.getConnectionHostIpAddress( field0, field1, field2, field3, NERemoteService::eServiceConnection::ConnectionTcpip ) )
        {
            mBrokerPort = static_cast<USHORT>(config.getConnectionPort( NERemoteService::eServiceConnection::ConnectionTcpip ));
            CString port( String::toString( mBrokerPort ).getString( ) );
            mCtrlAddress.SetAddress( field0, field1, field2, field3 );
            mCtrlPort.SetWindowText( port );
        }
    }

    GetDlgItem(IDC_BROKER_CONNECT)->SetFocus();
    UpdateDialogControls( this, FALSE );

    return TRUE;  // return TRUE unless you set the focus to a control
}

void PageNetworkSetup::OnDestroy( )
{
    CPropertyPage::OnDestroy();

    if ( mNetworkSetup != nullptr )
    {
        mNetworkSetup->requestDisconnect( mConnectionHandler.GetNickName( ), mConnectionHandler.GetCookie( ), mConnectionHandler.GetTimeConnect( ) );
        delete mNetworkSetup;
        mNetworkSetup = nullptr;
    }
}

void PageNetworkSetup::OnDefaultClicked( void )
{
    CButton * btnConnect    = reinterpret_cast<CButton *>(GetDlgItem( IDC_BROKER_CONNECT ));
    CButton * btnDisconnect = reinterpret_cast<CButton *>(GetDlgItem( IDC_BROKER_DISCONNECT ));
    CButton * btnRegister   = reinterpret_cast<CButton *>(GetDlgItem( IDC_BUTTON_REGISTER ));
    if ( (btnConnect != nullptr) && (btnDisconnect != nullptr) && (btnRegister != nullptr) )
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
