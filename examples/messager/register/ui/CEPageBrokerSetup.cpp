// CEPageBrokerSetup.cpp : implementation file
//

#include "register/res/stdafx.h"
#include "register/CECentralApp.hpp"
#include "register/ui/CEPageBrokerSetup.hpp"
#include "register/ui/CECentralDialog.hpp"
#include "register/NECentralApp.hpp"
#include "shared/NECommonSettings.hpp"

#include "areg/base/CEString.hpp"
#include "areg/base/NESocket.hpp"
#include "areg/appbase/CEApplication.hpp"
#include "areg/ipc/CEConnectionConfiguration.hpp"
#include "areg/component/CEComponentLoader.hpp"

// CEPageBrokerSetup dialog

IMPLEMENT_DYNAMIC(CEPageBrokerSetup, CPropertyPage)

CEPageBrokerSetup::CEPageBrokerSetup()
	: CPropertyPage     (CEPageBrokerSetup::IDD)

    , mCtrlAddress      ( )
    , mCtrlPort         ( )
    , mBrokerPort       ( 0xFFFFu )
    , mIsConnected      ( false )
    , mConnectEnable    ( FALSE )
    , mDisconnectEnabled( FALSE )
{
}

CEPageBrokerSetup::~CEPageBrokerSetup()
{
}

void CEPageBrokerSetup::ServiceConnected(bool isConnected)
{
}

void CEPageBrokerSetup::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange( pDX );
    DDX_Control( pDX, IDC_BROKER_IPADDRESS, mCtrlAddress );
    DDX_Control( pDX, IDC_BROKER_PORT, mCtrlPort );
}

BEGIN_MESSAGE_MAP(CEPageBrokerSetup, CPropertyPage)
    ON_BN_CLICKED( IDC_BROKER_CONNECT, &CEPageBrokerSetup::OnBnClickedBrokerConnect )
    ON_BN_CLICKED( IDC_BROKER_DISCONNECT, &CEPageBrokerSetup::OnBnClickedBrokerDisconnect )
    ON_MESSAGE_VOID(WM_KICKIDLE, OnKickIdle)
    ON_UPDATE_COMMAND_UI(IDC_BROKER_CONNECT, &CEPageBrokerSetup::OnBnUpdateBrokerConnect)
    ON_UPDATE_COMMAND_UI( IDC_BROKER_DISCONNECT, &CEPageBrokerSetup::OnBnUdateBrokerDisconnect )
    ON_UPDATE_COMMAND_UI( IDC_BROKER_IPADDRESS, &CEPageBrokerSetup::OnUpdateRemoteData )
    ON_UPDATE_COMMAND_UI( IDC_BROKER_PORT, &CEPageBrokerSetup::OnUpdateRemoteData )
END_MESSAGE_MAP( )

// CEPageBrokerSetup message handlers

void CEPageBrokerSetup::OnBnClickedBrokerConnect( )
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
            ipAddress.Format("%u.%u.%u.%u", ip1, ip2, ip3, ip4);
            mIsConnected = CECentralDialog::StartConnection(ipAddress, static_cast<unsigned short>(mBrokerPort) );
        }
    }
}

void CEPageBrokerSetup::OnBnClickedBrokerDisconnect( )
{
    if ( mIsConnected )
    {
        CPropertySheet * sheet = GetParentSheet();
        if ( sheet != NULL )
        {
            ::PostMessage(sheet->m_hWnd, NECentralApp::CmdServiceConnection, static_cast<WPARAM>(false), 0 );
        }

        CEApplication::StopModel( NECommonSettings::MODEL_NAME_CENTRAL_SERVER );
        CEApplication::StopBrokerClient();
        mIsConnected = false;
    }
}

void CEPageBrokerSetup::OnUpdateRemoteData( CCmdUI* pCmdUI )
{
    pCmdUI->Enable(mIsConnected ? FALSE : TRUE);
}

void CEPageBrokerSetup::OnBnUpdateBrokerConnect( CCmdUI* pCmdUI )
{
    CString port;
    BYTE ip1, ip2, ip3, ip4;
    mCtrlPort.GetWindowText( port );

    CEString check( port.GetBuffer( ) );
    if ( (check.IsInteger( false ) == true) && (mCtrlAddress.GetAddress( ip1, ip2, ip3, ip4 ) == 4) )
    {
        uint32_t temp = check.ToUInt32();
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

void CEPageBrokerSetup::OnBnUdateBrokerDisconnect( CCmdUI* pCmdUI )
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

void CEPageBrokerSetup::OnKickIdle( )
{
    UpdateData( TRUE );
    UpdateDialogControls(this, FALSE);
}

BOOL CEPageBrokerSetup::OnInitDialog( )
{    
    CPropertyPage::OnInitDialog( );

    CButton * btnConnect = reinterpret_cast<CButton *>(GetDlgItem( IDC_BROKER_CONNECT ));
    if (btnConnect != NULL )
        btnConnect->SetFocus();
  
    mCtrlAddress.SetAddress( 127, 0, 0, 1 );
    mCtrlPort.SetWindowText( _T( "8181" ) );

    CEConnectionConfiguration config;
    if ( config.LoadConfiguration(NEApplication::DEFAULT_BROKER_CONFIG_FILE) )
    {
        unsigned char field0, field1, field2, field3;
        if ( config.GetConnectionHostIpAddress(field0, field1, field2, field3, NERemoteService::ConnectionTcpip) )
        {
            mBrokerPort = static_cast<USHORT>( config.GetConnectionPort(NERemoteService::ConnectionTcpip) );
            CString port ( CEString::UInt32ToString(mBrokerPort).String() );
            mCtrlAddress.SetAddress(field0, field1, field2, field3);
            mCtrlPort.SetWindowText( port );
        }
    }
    UpdateDialogControls( this, FALSE );

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CEPageBrokerSetup::OnDefaultClicked( void )
{
    CButton * btnConnect = reinterpret_cast<CButton *>(GetDlgItem( IDC_BROKER_CONNECT ));
    CButton * btnDisconnect = reinterpret_cast<CButton *>(GetDlgItem( IDC_BROKER_DISCONNECT ));
    if ( (btnConnect != NULL) && (btnDisconnect != NULL) )
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

