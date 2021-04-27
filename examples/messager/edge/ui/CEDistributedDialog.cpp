// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// CEDistributedDialog.cpp : implementation of the CEDistributedDialog class
//

#include "edge/res/stdafx.h"
#include "edge/CEDistrbutedApp.hpp"
#include "edge/ui/CEDistributedDialog.hpp"
#include "edge/ui/CEPageChat.hpp"
#include "edge/NEDistributedApp.hpp"
#include "edge/services/CENetworkSetup.hpp"
#include "edge/services/CEConnectionServicing.hpp"
#include "shared/NECommonSettings.hpp"
#include "shared/generated/NEConnectionManager.hpp"
#include "areg/appbase/CEApplication.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
    CAboutDlg();

    // Dialog Data
    enum { IDD = IDD_ABOUTBOX };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

                                                        // Implementation
protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CEDistributedDialog dialog

bool CEDistributedDialog::PostServiceMessage( NEDistributedApp::eWndCommands cmd, WPARAM wParam, LPARAM lParam )
{
    bool result = false;
    CEDistributedDialog * dlg = CEDistributedDialog::GetDialog();
    if ( (dlg != NULL) && (::IsWindow(dlg->GetSafeHwnd()) == TRUE) ) 
        result = (::PostMessage( dlg->GetSafeHwnd(), static_cast<UINT>(cmd), wParam, lParam) == TRUE);
    return result;
}

CEDistributedDialog::CEDistributedDialog( )
    : CPropertySheet    (CEDistributedDialog::TITLE, NULL)
    , CEConnectionHandler( )

    , mPageSetup        ( static_cast<CEConnectionHandler &>(self()) )
    , mPageMessaging    ( static_cast<CEConnectionHandler &>(self()) )
    , mPageConnections  ( static_cast<CEConnectionHandler &>(self()) )

    , mMapChatPages     ( )
    , mCaption          ( _T("") )
    , mCaptionInit      ( _T("") )
{
    m_hIcon = AfxGetApp( )->LoadIcon( IDR_MAINFRAME );
    m_psh.dwFlags  |= PSH_NOAPPLYNOW | PSH_USEHICON;
    m_psh.hIcon     = m_hIcon;

    AddPage(&mPageSetup);
    AddPage(&mPageMessaging);
    AddPage(&mPageConnections);
}

CEDistributedDialog::~CEDistributedDialog( void )
{
    RemoveAllChatPages( );
}

void CEDistributedDialog::DoDataExchange(CDataExchange* pDX)
{
    CPropertySheet::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CEDistributedDialog, CPropertySheet)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_DESTROY( )
    ON_WM_QUERYDRAGICON()
    ON_COMMAND(IDOK, OnRedirectOK)

    ON_MESSAGE( NEDistributedApp::CmdServiceStartup     , &CEDistributedDialog::OnCmdServiceStartup )
    ON_MESSAGE( NEDistributedApp::CmdServiceNetwork     , &CEDistributedDialog::OnCmdServiceNetwork )
    ON_MESSAGE( NEDistributedApp::CmdServiceConnection  , &CEDistributedDialog::OnCmdServiceConnection )
    ON_MESSAGE( NEDistributedApp::CmdClientConnection   , &CEDistributedDialog::OnCmdClientConnection )
    ON_MESSAGE( NEDistributedApp::CmdClientRegistration , &CEDistributedDialog::OnCmdClientRegistration )
    ON_MESSAGE( NEDistributedApp::CmdAddConnection      , &CEDistributedDialog::OnCmdAddConnection )
    ON_MESSAGE( NEDistributedApp::CmdRemoveConnection   , &CEDistributedDialog::OnCmdRemoveConnection )
    ON_MESSAGE( NEDistributedApp::CmdUpdateConnection   , &CEDistributedDialog::OnCmdUpdateConnection )
    ON_MESSAGE( NEDistributedApp::CmdDisconnectTriggered, &CEDistributedDialog::OnCmdDisconnectTriggered)
    ON_MESSAGE( NEDistributedApp::CmdSendMessage        , &CEDistributedDialog::OnCmdSendMessage )
    ON_MESSAGE( NEDistributedApp::CmdTypeMessage        , &CEDistributedDialog::OnCmdTypeMessage )


    ON_MESSAGE( NEDistributedApp::CmdSetDirectConnection, &CEDistributedDialog::OnCmdSetDirectConnection )
    ON_MESSAGE( NEDistributedApp::CmdChatClosed         , &CEDistributedDialog::OnCmdChatClosed )
END_MESSAGE_MAP( )


// CEDistributedDialog message handlers

BOOL CEDistributedDialog::OnInitDialog()
{
    CPropertySheet::OnInitDialog();

    // IDM_ABOUTBOX must be in the system command range.
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL)
    {
        BOOL bNameValid;
        CString strAboutMenu;
        bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
        ASSERT(bNameValid);
        if (!strAboutMenu.IsEmpty())
        {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    // Set the icon for this dialog.  The areg does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);			// Set big icon
    SetIcon(m_hIcon, FALSE);		// Set small icon
    ModifyStyle( 0, WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX, SWP_FRAMECHANGED );

    mCaptionInit.LoadString(IDS_APPLICATION_TITLE);
    mCaption = mCaptionInit;
    SetTitle(mCaption);

    CEApplication::Initialize( true, true, true, false );

    SetActivePage(&mPageConnections);
    SetActivePage(&mPageMessaging);
    SetActivePage(&mPageSetup);

    CButton * btnOk = reinterpret_cast<CButton *>(GetDlgItem( IDOK ));
    CButton * btnCancel = reinterpret_cast<CButton *>(GetDlgItem( IDCANCEL ));
    if ( (btnOk != NULL) && (btnCancel != NULL) )
    {
        btnOk->ShowWindow( TRUE );
        UINT style = btnOk->GetButtonStyle( );
        style |= BS_DEFPUSHBUTTON;
        btnOk->SetButtonStyle( style );
        btnOk->ShowWindow( FALSE );

        btnCancel->SetWindowText( _T( "&Close" ) );
    }

    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CEDistributedDialog::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else
    {
        CPropertySheet::OnSysCommand(nID, lParam);
    }
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the areg.

void CEDistributedDialog::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CPropertySheet::OnPaint();
    }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CEDistributedDialog::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CEDistributedDialog::OnDestroy( )
{
    RemoveAllChatPages();
    CPropertySheet::OnDestroy( );
}

LRESULT CEDistributedDialog::OnCmdServiceStartup( WPARAM wParam, LPARAM lParam )
{
    if ( (wParam == 1) && (lParam != 0))
    {
        CEComponent * owner = reinterpret_cast<CEComponent *>(lParam);
        mPageSetup.OnServiceStartup( true, owner );
        mPageConnections.OnServiceStartup( true, owner );
        mPageMessaging.OnServiceStartup( true, owner );
    }
    else if ( (wParam == 0) && (lParam == 0))
    {
        mPageMessaging.OnServiceStartup( false, NULL );
        mPageSetup.OnServiceStartup( false, NULL);
        mPageConnections.OnServiceStartup( false, NULL);
    }

    return 0;
}

LRESULT CEDistributedDialog::OnCmdServiceNetwork( WPARAM wParam, LPARAM lParam )
{
    if ( (wParam == 1) && (lParam != 0) )
    {
        CEDispatcherThread * ownerThread = reinterpret_cast<CEDispatcherThread *>(lParam);
        mPageSetup.OnServiceNetwork( true, ownerThread );
        mPageConnections.OnServiceNetwork( true, ownerThread );
        mPageMessaging.OnServiceNetwork( true, ownerThread );
    }
    else if ( (wParam == 0) && (lParam == 0) )
    {
        mPageMessaging.OnServiceNetwork( false, NULL );
        mPageSetup.OnServiceNetwork( false, NULL );
        mPageConnections.OnServiceNetwork( false, NULL);
    }

    return 0;
}

LRESULT CEDistributedDialog::OnCmdServiceConnection( WPARAM wParam, LPARAM lParam )
{
    bool isConnected = wParam != 0;
    CEDispatcherThread * ownerThread = reinterpret_cast<CEDispatcherThread *>(lParam);
    if ( isConnected )
    {
        mPageSetup.OnServiceConnection( isConnected, ownerThread );
        mPageConnections.OnServiceConnection( isConnected, ownerThread );
        mPageMessaging.OnServiceConnection( isConnected, ownerThread );

        RemoveConnections();
    }
    else
    {
        mPageMessaging.OnServiceConnection( isConnected, ownerThread );
        mPageSetup.OnServiceConnection( isConnected, ownerThread );
        mPageConnections.OnServiceConnection( isConnected, ownerThread );
    }

    return 0;
}

LRESULT CEDistributedDialog::OnCmdClientConnection( WPARAM wParam, LPARAM lParam )
{
    bool isConnected = wParam != 0;
    CEDispatcherThread *dispThread = reinterpret_cast<CEDispatcherThread *>(lParam);
    if ( isConnected )
    {
        mPageSetup.OnClientConnection( isConnected, dispThread );
        mPageConnections.OnClientConnection( isConnected, dispThread );
        mPageMessaging.OnClientConnection( isConnected, dispThread );

        this->SetActivePage(&mPageMessaging);
    }
    else
    {
        mPageMessaging.OnClientConnection( isConnected, dispThread );
        mPageSetup.OnClientConnection( isConnected, dispThread );
        mPageConnections.OnClientConnection( isConnected, dispThread );
    }

    return 0;
}

LRESULT CEDistributedDialog::OnCmdClientRegistration( WPARAM wParam, LPARAM lParam )
{
    bool isRegistered = wParam != 0;
    CEDispatcherThread * dispThread = reinterpret_cast<CEDispatcherThread *>(lParam);
    if ( isRegistered )
    {
        mPageSetup.OnClientRegistration( isRegistered, dispThread );
        mPageConnections.OnClientRegistration( isRegistered, dispThread );
        mPageMessaging.OnClientRegistration( isRegistered, dispThread );
    }
    else
    {
        mPageMessaging.OnClientRegistration( isRegistered, dispThread );
        mPageSetup.OnClientRegistration( isRegistered, dispThread );
        mPageConnections.OnClientRegistration( isRegistered, dispThread );
    }

    CEString nickName = GetNickName();
    if ( nickName.IsEmpty() == false )
    {
        nickName = "[ " + nickName + " ]: ";
        CString temp( nickName.String() );
        mCaption = temp + mCaptionInit;
    }
    else
    {
        mCaption = mCaptionInit;
    }
    if ( GetActiveIndex() < 3)
        SetTitle( mCaption );

    return 0;
}

LRESULT CEDistributedDialog::OnCmdAddConnection( WPARAM wParam, LPARAM lParam )
{
    NEConnectionManager::sConnection * data = reinterpret_cast<NEConnectionManager::sConnection *>(lParam);
    if ( data != static_cast<NEConnectionManager::sConnection *>(NULL) )
    {
        mPageSetup.OnAddConnection( *data );
        mPageConnections.OnAddConnection( *data );
        mPageMessaging.OnAddConnection( *data );

        delete data;
    }

    return 0;
}

LRESULT CEDistributedDialog::OnCmdRemoveConnection( WPARAM wParam, LPARAM lParam )
{
    NEConnectionManager::sConnection * data = reinterpret_cast<NEConnectionManager::sConnection *>(lParam);
    if ( data != static_cast<NEConnectionManager::sConnection *>(NULL) )
    {
        mPageMessaging.OnRemoveConnection( *data );
        mPageSetup.OnRemoveConnection( *data );
        mPageConnections.OnRemoveConnection( *data );

        delete data;
    }

    return 0;
}

LRESULT CEDistributedDialog::OnCmdUpdateConnection( WPARAM wParam, LPARAM lParam )
{
    ASSERT(lParam == 0);
    mPageSetup.OnUpdateConnection( );
    mPageConnections.OnUpdateConnection( );
    mPageMessaging.OnUpdateConnection( );


    return 0;
}

LRESULT CEDistributedDialog::OnCmdDisconnectTriggered( WPARAM wParam, LPARAM lParam )
{
    ASSERT( lParam == 0 );
    RemoveAllChatPages( );

    mPageMessaging.OnDisconnectTriggered( );
    mPageSetup.OnDisconnectTriggered( );
    mPageConnections.OnDisconnectTriggered( );

    return 0;
}

LRESULT CEDistributedDialog::OnCmdChatClosed( WPARAM wParam, LPARAM lParam )
{
    CEPageChat * pageChat = reinterpret_cast<CEPageChat *>(lParam);
    if ( pageChat != NULL )
    {
        for ( MAPPOS pos = mMapChatPages.GetStartPosition( ); pos != NULL; pos = mMapChatPages.GetNextPosition( pos ) )
        {
            CEPageChat * page = mMapChatPages.GetAt( pos );
            if ( page == pageChat )
            {
                mMapChatPages.RemoveAt( pos );
                break;
            }
        }

        RemovePage( pageChat );
        delete pageChat;
    }
    return 0;
}


LRESULT CEDistributedDialog::OnCmdSendMessage( WPARAM wParam, LPARAM lParam )
{
    NECommonSettings::sMessageData * data = reinterpret_cast<NECommonSettings::sMessageData *>(lParam);
    if ( data != NULL )
    {
        if ( wParam != static_cast<WPARAM>(NECommonSettings::InvalidCookie) )
            mPageMessaging.OnSendMessage( static_cast<uint32_t>(wParam), *data );
        delete data;
    }
    return 0;
}

LRESULT CEDistributedDialog::OnCmdTypeMessage( WPARAM wParam, LPARAM lParam )
{
    NECommonSettings::sMessageData * data = reinterpret_cast<NECommonSettings::sMessageData *>(lParam);
    if ( data != NULL )
    {
        if ( wParam != static_cast<WPARAM>(NECommonSettings::InvalidCookie) )
            mPageMessaging.OnTypeMessage( static_cast<uint32_t>(wParam), *data );
        delete data;
    }
    return 0;
}

bool CEDistributedDialog::OutputMessage( NEDistributedApp::eWndCommands cmd, void * sender, NECommonSettings::sMessageData * data )
{
    bool result = false;
    if ( data != NULL )
    {
        CWnd * mainFrame = theApp.GetMainWnd( );
        if ( mainFrame != NULL )
        {
            HWND hWnd = static_cast<CEDistributedDialog *>(mainFrame)->mPageMessaging.GetSafeHwnd();
            if ( ::IsWindow(hWnd) )
                result = ::PostMessage(hWnd, cmd, reinterpret_cast<WPARAM>(sender), reinterpret_cast<LPARAM>(data)) == TRUE;
        }
        if ( result == false )
            delete data;
    }
    return result;
}

CEPageChat * CEDistributedDialog::AddChatPage( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParties, bool isInitiator )
{
    NEDirectConnection::sParticipant owner;
    if ( isInitiator )
    {
        owner.nickName  = initiator.nickName;
        owner.cookie    = initiator.cookie;
        owner.sessionId = initiator.sessionId;

    }
    else
    {
        owner.nickName  = mPageConnections.GetRegisteredName();
        owner.cookie    = mPageConnections.GetRegisteredCookie();
        owner.sessionId = initiator.sessionId;
    }

    CEString serviceName    = NEDistributedApp::getDirectMessagingRole( owner.nickName, owner.cookie, owner.sessionId, isInitiator );
    CEPageChat * chatPage   = new CEPageChat( serviceName, initiator, listParties, owner, isInitiator );
    if ( chatPage != NULL )
    {
        ASSERT(mMapChatPages.Find(serviceName) == static_cast<MAPPOS>(NULL) );
        mMapChatPages.SetKey( serviceName, chatPage );

        AddPage( chatPage );
        SetActivePage( chatPage );
    }

    return chatPage;
}

CEDistributedDialog * CEDistributedDialog::GetDialog( void )
{
    return static_cast<CEDistributedDialog *>(theApp.GetMainWnd());
}



void CEDistributedDialog::DefaultCaption( void )
{
    CEDistributedDialog * dlg = CEDistributedDialog::GetDialog();
    if ( dlg != NULL )
        dlg->SetTitle(dlg->mCaption);
}

void CEDistributedDialog::ChangeCaption( LPCTSTR newCaption )
{
    CEDistributedDialog * dlg = CEDistributedDialog::GetDialog( );
    if ( (newCaption != NULL) && (dlg != NULL) )
        dlg->SetTitle( newCaption );
}

bool CEDistributedDialog::RemoveChatPage( const CEString & connectName )
{
    MAPPOS pos = mMapChatPages.Find(connectName);
    if ( pos != NULL )
    {
        CEPageChat * chatPage = mMapChatPages.GetAt(pos);
        if ( chatPage != NULL )
        {
            RemovePage(chatPage);
            delete chatPage;
        }
        mMapChatPages.RemoveAt(pos);
    }
    return (pos != NULL);
}

void CEDistributedDialog::RemoveAllChatPages( void )
{
    for ( MAPPOS pos = mMapChatPages.GetStartPosition(); pos != NULL; pos = mMapChatPages.GetNextPosition(pos) )
    {
        CEPageChat * chatPage = mMapChatPages.GetAt( pos );
        if ( chatPage != NULL )
        {
            RemovePage( chatPage );
            delete chatPage;
        }
    }
    mMapChatPages.RemoveAll();
}

LRESULT CEDistributedDialog::OnCmdSetDirectConnection( WPARAM wParam, LPARAM lParam )
{
    NEDirectConnection::sInitiator      * initiator     = reinterpret_cast<NEDirectConnection::sInitiator *>( wParam );
    NEDirectConnection::ListParticipants* listParties   = reinterpret_cast<NEDirectConnection::ListParticipants *>( lParam );

    if ( (initiator != NULL) && (listParties != NULL) )
        AddChatPage(*initiator, *listParties, false);
    if ( initiator != NULL)
        delete initiator;
    if ( listParties != NULL)
        delete listParties;

    return 0;
}

void CEDistributedDialog::OnRedirectOK( void )
{
    CPropertyPage * active = GetActivePage( );
    if ( active == &mPageSetup )
    {
        mPageSetup.OnDefaultClicked( );
    }
    else if ( active == &mPageMessaging)
    {
        mPageMessaging.OnDefaultClicked();
    }
    else if ( active == &mPageConnections )
    {
        mPageConnections.OnDefaultClicked( );
    }
    else
    {
        CEPageChat * page = static_cast<CEPageChat *>(active);
        page->OnDefaultClicked();
    }

    CButton * btnOk = reinterpret_cast<CButton *>(GetDlgItem( IDOK ));
    if ( btnOk != NULL )
    {
        btnOk->ShowWindow( TRUE );
        btnOk->SetFocus( );
        UINT style = btnOk->GetButtonStyle( );
        style |= BS_DEFPUSHBUTTON;
        btnOk->SetButtonStyle( style );
        btnOk->ShowWindow( FALSE );
    }
}
