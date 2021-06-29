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

// DistributedDialog.cpp : implementation of the DistributedDialog class
//

#include "edge/res/stdafx.h"
#include "edge/DistrbutedApp.hpp"
#include "edge/ui/DistributedDialog.hpp"
#include "edge/ui/PageChat.hpp"
#include "edge/NEDistributedApp.hpp"
#include "edge/services/NetworkSetup.hpp"
#include "edge/services/ConnectionServicing.hpp"
#include "shared/NECommonSettings.hpp"
#include "shared/generated/NEConnectionManager.hpp"
#include "areg/appbase/Application.hpp"

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


// DistributedDialog dialog

bool DistributedDialog::PostServiceMessage( NEDistributedApp::eWndCommands cmd, WPARAM wParam, LPARAM lParam )
{
    bool result = false;
    DistributedDialog * dlg = DistributedDialog::GetDialog();
    if ( (dlg != NULL) && (::IsWindow(dlg->GetSafeHwnd()) == TRUE) ) 
        result = (::PostMessage( dlg->GetSafeHwnd(), static_cast<UINT>(cmd), wParam, lParam) == TRUE);
    return result;
}

DistributedDialog::DistributedDialog( )
    : CPropertySheet    (DistributedDialog::TITLE, NULL)
    , ConnectionHandler( )

    , mPageSetup        ( static_cast<ConnectionHandler &>(self()) )
    , mPageMessaging    ( static_cast<ConnectionHandler &>(self()) )
    , mPageConnections  ( static_cast<ConnectionHandler &>(self()) )

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

DistributedDialog::~DistributedDialog( void )
{
    RemoveAllChatPages( );
}

void DistributedDialog::DoDataExchange(CDataExchange* pDX)
{
    CPropertySheet::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(DistributedDialog, CPropertySheet)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_DESTROY( )
    ON_WM_QUERYDRAGICON()
    ON_COMMAND(IDOK, OnRedirectOK)

    ON_MESSAGE( NEDistributedApp::CmdServiceStartup     , &DistributedDialog::OnCmdServiceStartup )
    ON_MESSAGE( NEDistributedApp::CmdServiceNetwork     , &DistributedDialog::OnCmdServiceNetwork )
    ON_MESSAGE( NEDistributedApp::CmdServiceConnection  , &DistributedDialog::OnCmdServiceConnection )
    ON_MESSAGE( NEDistributedApp::CmdClientConnection   , &DistributedDialog::OnCmdClientConnection )
    ON_MESSAGE( NEDistributedApp::CmdClientRegistration , &DistributedDialog::OnCmdClientRegistration )
    ON_MESSAGE( NEDistributedApp::CmdAddConnection      , &DistributedDialog::OnCmdAddConnection )
    ON_MESSAGE( NEDistributedApp::CmdRemoveConnection   , &DistributedDialog::OnCmdRemoveConnection )
    ON_MESSAGE( NEDistributedApp::CmdUpdateConnection   , &DistributedDialog::OnCmdUpdateConnection )
    ON_MESSAGE( NEDistributedApp::CmdDisconnectTriggered, &DistributedDialog::OnCmdDisconnectTriggered)
    ON_MESSAGE( NEDistributedApp::CmdSendMessage        , &DistributedDialog::OnCmdSendMessage )
    ON_MESSAGE( NEDistributedApp::CmdTypeMessage        , &DistributedDialog::OnCmdTypeMessage )


    ON_MESSAGE( NEDistributedApp::CmdSetDirectConnection, &DistributedDialog::OnCmdSetDirectConnection )
    ON_MESSAGE( NEDistributedApp::CmdChatClosed         , &DistributedDialog::OnCmdChatClosed )
END_MESSAGE_MAP( )


// DistributedDialog message handlers

BOOL DistributedDialog::OnInitDialog()
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

    Application::initApplication( true, true, true, false );

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

void DistributedDialog::OnSysCommand(UINT nID, LPARAM lParam)
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

void DistributedDialog::OnPaint()
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
HCURSOR DistributedDialog::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void DistributedDialog::OnDestroy( )
{
    RemoveAllChatPages();
    CPropertySheet::OnDestroy( );
}

LRESULT DistributedDialog::OnCmdServiceStartup( WPARAM wParam, LPARAM lParam )
{
    if ( (wParam == 1) && (lParam != 0))
    {
        Component * owner = reinterpret_cast<Component *>(lParam);
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

LRESULT DistributedDialog::OnCmdServiceNetwork( WPARAM wParam, LPARAM lParam )
{
    if ( (wParam == 1) && (lParam != 0) )
    {
        DispatcherThread * ownerThread = reinterpret_cast<DispatcherThread *>(lParam);
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

LRESULT DistributedDialog::OnCmdServiceConnection( WPARAM wParam, LPARAM lParam )
{
    bool isConnected = wParam != 0;
    DispatcherThread * ownerThread = reinterpret_cast<DispatcherThread *>(lParam);
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

LRESULT DistributedDialog::OnCmdClientConnection( WPARAM wParam, LPARAM lParam )
{
    bool isConnected = wParam != 0;
    DispatcherThread *dispThread = reinterpret_cast<DispatcherThread *>(lParam);
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

LRESULT DistributedDialog::OnCmdClientRegistration( WPARAM wParam, LPARAM lParam )
{
    bool isRegistered = wParam != 0;
    DispatcherThread * dispThread = reinterpret_cast<DispatcherThread *>(lParam);
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

    String nickName = GetNickName();
    if ( nickName.isEmpty() == false )
    {
        nickName = "[ " + nickName + " ]: ";
        CString temp( nickName.getString() );
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

LRESULT DistributedDialog::OnCmdAddConnection( WPARAM wParam, LPARAM lParam )
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

LRESULT DistributedDialog::OnCmdRemoveConnection( WPARAM wParam, LPARAM lParam )
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

LRESULT DistributedDialog::OnCmdUpdateConnection( WPARAM wParam, LPARAM lParam )
{
    ASSERT(lParam == 0);
    mPageSetup.OnUpdateConnection( );
    mPageConnections.OnUpdateConnection( );
    mPageMessaging.OnUpdateConnection( );


    return 0;
}

LRESULT DistributedDialog::OnCmdDisconnectTriggered( WPARAM wParam, LPARAM lParam )
{
    ASSERT( lParam == 0 );
    RemoveAllChatPages( );

    mPageMessaging.OnDisconnectTriggered( );
    mPageSetup.OnDisconnectTriggered( );
    mPageConnections.OnDisconnectTriggered( );

    return 0;
}

LRESULT DistributedDialog::OnCmdChatClosed( WPARAM wParam, LPARAM lParam )
{
    PageChat * pageChat = reinterpret_cast<PageChat *>(lParam);
    if ( pageChat != NULL )
    {
        for ( MAPPOS pos = mMapChatPages.firstPosition( ); pos != NULL; pos = mMapChatPages.nextPosition( pos ) )
        {
            PageChat * page = mMapChatPages.getPosition( pos );
            if ( page == pageChat )
            {
                mMapChatPages.removePosition( pos );
                break;
            }
        }

        RemovePage( pageChat );
        delete pageChat;
    }
    return 0;
}


LRESULT DistributedDialog::OnCmdSendMessage( WPARAM wParam, LPARAM lParam )
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

LRESULT DistributedDialog::OnCmdTypeMessage( WPARAM wParam, LPARAM lParam )
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

bool DistributedDialog::OutputMessage( NEDistributedApp::eWndCommands cmd, void * sender, NECommonSettings::sMessageData * data )
{
    bool result = false;
    if ( data != NULL )
    {
        CWnd * mainFrame = theApp.GetMainWnd( );
        if ( mainFrame != NULL )
        {
            HWND hWnd = static_cast<DistributedDialog *>(mainFrame)->mPageMessaging.GetSafeHwnd();
            if ( ::IsWindow(hWnd) )
                result = ::PostMessage(hWnd, cmd, reinterpret_cast<WPARAM>(sender), reinterpret_cast<LPARAM>(data)) == TRUE;
        }
        if ( result == false )
            delete data;
    }
    return result;
}

PageChat * DistributedDialog::AddChatPage( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParties, bool isInitiator )
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

    String serviceName    = NEDistributedApp::getDirectMessagingRole( owner.nickName, owner.cookie, owner.sessionId, isInitiator );
    PageChat * chatPage   = new PageChat( serviceName, initiator, listParties, owner, isInitiator );
    if ( chatPage != NULL )
    {
        ASSERT(mMapChatPages.find(serviceName) == static_cast<MAPPOS>(NULL) );
        mMapChatPages.setAt( serviceName, chatPage );

        AddPage( chatPage );
        SetActivePage( chatPage );
    }

    return chatPage;
}

DistributedDialog * DistributedDialog::GetDialog( void )
{
    return static_cast<DistributedDialog *>(theApp.GetMainWnd());
}



void DistributedDialog::DefaultCaption( void )
{
    DistributedDialog * dlg = DistributedDialog::GetDialog();
    if ( dlg != NULL )
        dlg->SetTitle(dlg->mCaption);
}

void DistributedDialog::ChangeCaption( LPCTSTR newCaption )
{
    DistributedDialog * dlg = DistributedDialog::GetDialog( );
    if ( (newCaption != NULL) && (dlg != NULL) )
        dlg->SetTitle( newCaption );
}

bool DistributedDialog::RemoveChatPage( const String & connectName )
{
    MAPPOS pos = mMapChatPages.find(connectName);
    if ( pos != NULL )
    {
        PageChat * chatPage = mMapChatPages.getPosition(pos);
        if ( chatPage != NULL )
        {
            RemovePage(chatPage);
            delete chatPage;
        }
        mMapChatPages.removePosition(pos);
    }
    return (pos != NULL);
}

void DistributedDialog::RemoveAllChatPages( void )
{
    for ( MAPPOS pos = mMapChatPages.firstPosition(); pos != NULL; pos = mMapChatPages.nextPosition(pos) )
    {
        PageChat * chatPage = mMapChatPages.getPosition( pos );
        if ( chatPage != NULL )
        {
            RemovePage( chatPage );
            delete chatPage;
        }
    }
    mMapChatPages.removeAll();
}

LRESULT DistributedDialog::OnCmdSetDirectConnection( WPARAM wParam, LPARAM lParam )
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

void DistributedDialog::OnRedirectOK( void )
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
        PageChat * page = static_cast<PageChat *>(active);
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
