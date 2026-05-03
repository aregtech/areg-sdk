// chatter/PageNetworkSetup.cpp : implementation file
//

#include "chatter/res/stdafx.h"
#include "chatter/DistrbutedApp.hpp"
#include "chatter/DistributedAppDefs.hpp"
#include "chatter/services/ChatPrticipantHandler.hpp"
#include "chatter/services/NetworkSetup.hpp"
#include "chatter/ui/PageNetworkSetup.hpp"
#include "chatter/ui/DistributedDialog.hpp"
#include "common/ChatDefs.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/base/String.hpp"
#include "areg/base/SocketDefs.hpp"
#include "areg/ipc/ConnectionConfiguration.hpp"
#include "areg/ipc/RemoteServiceDefs.hpp"

#define FIRST_MESSAGE       (WM_USER + 10 + static_cast<uint32_t>(NEDistributedApp::WindowCommand::CmdFirst))
#define MAKE_MESSAGE(elem)  (static_cast<uint32_t>(elem) + FIRST_MESSAGE)

// PageNetworkSetup dialog

IMPLEMENT_DYNAMIC(PageNetworkSetup, CPropertyPage)

PageNetworkSetup::PageNetworkSetup( ConnectionHandler & handlerConnection)
	: CPropertyPage (PageNetworkSetup::IDD)

    , mCtrlAddress      ( )
    , mCtrlPort         ( )
    , mNickName         ( )
    , mCtrlNickName     ( )
    , mBrokerPort       ( areg::InvalidPort )
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

void PageNetworkSetup::cleanService()
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

bool PageNetworkSetup::is_service_connected() const
{
    return (mNetworkSetup != nullptr ? mNetworkSetup->is_connected() : false);
}


void PageNetworkSetup::OnServiceStartup( bool isStarted, areg::Component * owner )
{
    mConnectionHandler.SetConnected( false);
    if ( isStarted )
    {
        if ( (mNetworkSetup == nullptr) && (owner != nullptr) )
            mNetworkSetup = DEBUG_NEW NetworkSetup( chat::COMP_NAME_CENTRAL_SERVER, *owner, mConnectionHandler );
    }
    else
    {
        if ( mNetworkSetup != nullptr )
            delete mNetworkSetup;
        mNetworkSetup = nullptr;
    }
}

void PageNetworkSetup::OnServiceNetwork( bool isConnected, areg::DispatcherThread * /*ownerThread*/)
{
    mConnectionHandler.SetConnected( isConnected );
    if ( mRegisterPending )
    {
        if ( (mConnectionHandler.GetNickName().is_empty() == false) && (mConnectionHandler.GetCookie() == chat::InvalidCookie) )
        {
            ASSERT(mNetworkSetup != nullptr);
            mNetworkSetup->request_connect(mConnectionHandler.GetNickName(), areg::DateTime::now() );
        }
    }
    mConnectPending = isConnected ? false : mConnectPending;
}

void PageNetworkSetup::OnServiceConnection( bool /*isConnected*/, areg::DispatcherThread* /*ownerThread*/)
{
    // do nothing
}

void PageNetworkSetup::OnClientConnection( bool isConnected, areg::DispatcherThread * /*dispThread*/)
{
    mConnectPending = isConnected ? false : mConnectPending;
    mConnectionHandler.SetConnected(isConnected);
}

void PageNetworkSetup::OnClientRegistration( bool isRegistered, areg::DispatcherThread * /*dispThread*/)
{
    mRegisterPending = isRegistered ? false : mRegisterPending;
    if ( isRegistered )
    {
        mNickName = mConnectionHandler.GetNickName().as_string();
        UpdateData(FALSE);
    }
    else if ( (isRegistered == false) && (mNetworkSetup != nullptr) )
    {
        mNetworkSetup->request_disconnect( mConnectionHandler.GetNickName(), mConnectionHandler.GetCookie(), areg::DateTime::now());
        mConnectionHandler.ResetConnectionList( );
    }
}

void PageNetworkSetup::OnAddConnection( ConnectionManager::ConnectionRecord & /*data*/)
{
    // do nothing
}

void PageNetworkSetup::OnRemoveConnection( ConnectionManager::ConnectionRecord & /*data*/)
{
    // do nothing
}

void PageNetworkSetup::OnUpdateConnection()
{
    // do nothing
}

void PageNetworkSetup::OnDisconnectTriggered()
{
    if ( mNetworkSetup != nullptr )
    {
        mNetworkSetup->request_disconnect(mConnectionHandler.GetNickName(), mConnectionHandler.GetCookie(), mConnectionHandler.GetTimeConnect());
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
    ON_MESSAGE_VOID( WM_KICKIDLE, PageNetworkSetup::OnKickIdle )
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

    areg::String check( port.GetBuffer( ) );
    if ( (check.is_numeric( false ) == true) && (mCtrlAddress.GetAddress( ip1, ip2, ip3, ip4 ) == 4) )
    {
        uint32_t temp = check.to_uint32( );
        if ( (temp != areg::InvalidPort) && (temp < 0xFFFFu) )
        {
            mBrokerPort = static_cast<USHORT>(temp);
            areg::String ipAddress;
            ipAddress.format( "%u.%u.%u.%u", ip1, ip2, ip3, ip4 );
            if ( areg::Application::start_message_routing( ipAddress, mBrokerPort ) )
            {
                areg::Application::load_model( chat::MODEL_NAME_DISTRIBUTED_CLIENT );
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
        ::SendMessage( wnd->GetSafeHwnd(), MAKE_MESSAGE(NEDistributedApp::WindowCommand::CmdDisconnectTriggered), 0, 0);
        mConnectionHandler.ResetConnectionList();

        areg::Application::unload_model(chat::MODEL_NAME_DISTRIBUTED_CLIENT);
        areg::Application::stop_message_routing();
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
            areg::String nickName(mNickName.GetString());
            mRegisterPending = true;
            mConnectionHandler.SetRegistered(false);
            mConnectionHandler.SetNickName(nickName);
            mNetworkSetup->request_connect(nickName, areg::DateTime::now() );
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
    areg::String nickName(mNickName.GetString());
    if (mNickName.GetLength() != nickName.make_alphanumeric().length())
    {
        mNickName = nickName.as_string();
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

    areg::String check( port.GetBuffer( ) );
    if ( (check.is_numeric( false ) == true) && (mCtrlAddress.GetAddress( ip1, ip2, ip3, ip4 ) == 4) )
    {
        uint32_t temp = check.to_uint32( );
        mBrokerPort = temp > 0xFFFFu ? 0xFFFFu : static_cast<USHORT>(temp);
    }
    else
    {
        mBrokerPort = 0xFFFFu;
    }
    
    if ( (areg::Application::is_router_connected( ) == false) && (mBrokerPort < 0xFFFFu) && (mCtrlAddress.IsBlank( ) == FALSE) )
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
    if ( areg::Application::is_router_connected( ) )
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
    pCmdUI->Enable( areg::Application::is_router_connected( ) ? FALSE : TRUE );
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

bool PageNetworkSetup::canRegistered() const
{
    return (areg::Application::is_router_connected( ) ? mConnectionHandler.GetRegistered() == false : false);
}

void PageNetworkSetup::setFocusNickname() const
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

    areg::ConnectionConfiguration config(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip);
    uint8_t field0, field1, field2, field3;
    if (config.connection_ip_address(field0, field1, field2, field3))
    {
        mBrokerPort = static_cast<USHORT>(config.connection_port());
        CString port(areg::String::make_string(mBrokerPort).as_string());
        mCtrlAddress.SetAddress(field0, field1, field2, field3);
        mCtrlPort.SetWindowText(port);
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
        mNetworkSetup->request_disconnect( mConnectionHandler.GetNickName( ), mConnectionHandler.GetCookie( ), mConnectionHandler.GetTimeConnect( ) );
        delete mNetworkSetup;
        mNetworkSetup = nullptr;
    }
}

void PageNetworkSetup::OnDefaultClicked()
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
