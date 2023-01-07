// PageBrokerSetup.cpp : implementation file
//

#include "register/res/stdafx.h"
#include "register/CentralApp.hpp"
#include "register/ui/PageBrokerSetup.hpp"
#include "register/ui/CentralDialog.hpp"
#include "register/NECentralApp.hpp"
#include "generated/NECommon.hpp"

#include "areg/base/String.hpp"
#include "areg/base/NESocket.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/ipc/ConnectionConfiguration.hpp"
#include "areg/component/ComponentLoader.hpp"

#define FIRST_MESSAGE       (WM_USER + 10 + static_cast<unsigned int>(NECentralApp::eWndCommands::CmdFirst))
#define MAKE_MESSAGE(elem)  (static_cast<unsigned int>(elem) + FIRST_MESSAGE)


// PageBrokerSetup dialog

IMPLEMENT_DYNAMIC(PageBrokerSetup, CPropertyPage)

PageBrokerSetup::PageBrokerSetup()
	: CPropertyPage     (PageBrokerSetup::IDD)

    , mCtrlAddress      ( )
    , mCtrlPort         ( )
    , mBrokerPort       ( 0xFFFFu )
    , mIsConnected      ( false )
    , mConnectEnable    ( FALSE )
    , mDisconnectEnabled( FALSE )
{
}

void PageBrokerSetup::ServiceConnected(bool isConnected)
{
}

void PageBrokerSetup::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange( pDX );
    DDX_Control( pDX, IDC_BROKER_IPADDRESS, mCtrlAddress );
    DDX_Control( pDX, IDC_BROKER_PORT, mCtrlPort );
}

BEGIN_MESSAGE_MAP(PageBrokerSetup, CPropertyPage)
    ON_BN_CLICKED( IDC_BROKER_CONNECT, &PageBrokerSetup::OnBnClickedBrokerConnect )
    ON_BN_CLICKED( IDC_BROKER_DISCONNECT, &PageBrokerSetup::OnBnClickedBrokerDisconnect )
    ON_MESSAGE_VOID(WM_KICKIDLE, OnKickIdle)
    ON_UPDATE_COMMAND_UI(IDC_BROKER_CONNECT, &PageBrokerSetup::OnBnUpdateBrokerConnect)
    ON_UPDATE_COMMAND_UI( IDC_BROKER_DISCONNECT, &PageBrokerSetup::OnBnUdateBrokerDisconnect )
    ON_UPDATE_COMMAND_UI( IDC_BROKER_IPADDRESS, &PageBrokerSetup::OnUpdateRemoteData )
    ON_UPDATE_COMMAND_UI( IDC_BROKER_PORT, &PageBrokerSetup::OnUpdateRemoteData )
END_MESSAGE_MAP( )

// PageBrokerSetup message handlers

void PageBrokerSetup::OnBnClickedBrokerConnect( )
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
            ipAddress.format("%u.%u.%u.%u", ip1, ip2, ip3, ip4);
            mIsConnected = CentralDialog::StartConnection(ipAddress, static_cast<unsigned short>(mBrokerPort) );
        }
    }
}

void PageBrokerSetup::OnBnClickedBrokerDisconnect( )
{
    if ( mIsConnected )
    {
        CPropertySheet * sheet = GetParentSheet();
        if ( sheet != nullptr )
        {
            ::PostMessage(sheet->m_hWnd, MAKE_MESSAGE(NECentralApp::eWndCommands::CmdServiceConnection), static_cast<WPARAM>(false), 0 );
        }

        Application::unloadModel( NECommon::MODEL_NAME_CENTRAL_SERVER );
        Application::stopMessageRouting();
        mIsConnected = false;
    }
}

void PageBrokerSetup::OnUpdateRemoteData( CCmdUI* pCmdUI )
{
    pCmdUI->Enable(mIsConnected ? FALSE : TRUE);
}

void PageBrokerSetup::OnBnUpdateBrokerConnect( CCmdUI* pCmdUI )
{
    CString port;
    BYTE ip1, ip2, ip3, ip4;
    mCtrlPort.GetWindowText( port );

    String check( port.GetBuffer( ) );
    if ( (check.isNumeric( false ) == true) && (mCtrlAddress.GetAddress( ip1, ip2, ip3, ip4 ) == 4) )
    {
        uint32_t temp = check.toUInt32();
        mBrokerPort = temp > 0xFFFFu ? 0xFFFFu : temp;
    }
    else
    {
        mBrokerPort = 0xFFFFu;
    }
    if ( (mIsConnected == false) && (mBrokerPort < 0xFFFFu) && (mCtrlAddress.IsBlank() == FALSE) )
    {
        pCmdUI->Enable( TRUE );
        if ( mConnectEnable == FALSE)
        {
            CButton * btn = reinterpret_cast<CButton *>(GetDlgItem(IDC_BROKER_CONNECT));
            btn->SetFocus();
        }
        mConnectEnable = TRUE;
    }
    else
    {
        pCmdUI->Enable( FALSE );
        mConnectEnable = FALSE;
    }
}

void PageBrokerSetup::OnBnUdateBrokerDisconnect( CCmdUI* pCmdUI )
{
    if ( mIsConnected )
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

void PageBrokerSetup::OnKickIdle( )
{
    UpdateData( TRUE );
    UpdateDialogControls(this, FALSE);
}

BOOL PageBrokerSetup::OnInitDialog( )
{    
    CPropertyPage::OnInitDialog( );

    CButton * btnConnect = reinterpret_cast<CButton *>(GetDlgItem( IDC_BROKER_CONNECT ));
    if (btnConnect != nullptr )
        btnConnect->SetFocus();
  
    mCtrlAddress.SetAddress( 127, 0, 0, 1 );
    mCtrlPort.SetWindowText( _T( "8181" ) );

    ConnectionConfiguration config;
    if ( config.loadConfiguration(NEApplication::DEFAULT_ROUTER_CONFIG_FILE.data()) )
    {
        unsigned char field0, field1, field2, field3;
        if ( config.getConnectionHostIpAddress(field0, field1, field2, field3, NERemoteService::eServiceConnection::ConnectionTcpip) )
        {
            mBrokerPort = static_cast<USHORT>( config.getConnectionPort(NERemoteService::eServiceConnection::ConnectionTcpip) );
            CString port ( String::toString(mBrokerPort).getString() );
            mCtrlAddress.SetAddress(field0, field1, field2, field3);
            mCtrlPort.SetWindowText( port );
        }
    }
    UpdateDialogControls( this, FALSE );

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXPTION: OCX Property Pages should return FALSE
}

void PageBrokerSetup::OnDefaultClicked( void )
{
    CButton * btnConnect = reinterpret_cast<CButton *>(GetDlgItem( IDC_BROKER_CONNECT ));
    CButton * btnDisconnect = reinterpret_cast<CButton *>(GetDlgItem( IDC_BROKER_DISCONNECT ));
    if ( (btnConnect != nullptr) && (btnDisconnect != nullptr) )
    {
        if ( mDisconnectEnabled )
        {
            SendMessage( WM_COMMAND, MAKEWPARAM( IDC_BROKER_DISCONNECT, BN_CLICKED  ), reinterpret_cast<LPARAM>(btnConnect->GetSafeHwnd( )) );
        }
        else
        {
            SendMessage( WM_COMMAND, MAKEWPARAM( IDC_BROKER_CONNECT, BN_CLICKED     ), reinterpret_cast<LPARAM>(btnDisconnect->GetSafeHwnd( )) );
        }
    }
}

