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

#include "chatter/res/stdafx.h"
#include "chatter/DistrbutedApp.hpp"
#include "chatter/ui/DistributedDialog.hpp"
#include "chatter/ui/PageChat.hpp"
#include "chatter/NEDistributedApp.hpp"
#include "chatter/services/NetworkSetup.hpp"
#include "chatter/services/ConnectionService.hpp"
#include "generated/NECommon.hpp"
#include "generated/NEConnectionManager.hpp"
#include "areg/appbase/Application.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define FIRST_MESSAGE       (WM_USER + 10 + static_cast<unsigned int>(NEDistributedApp::eWndCommands::CmdFirst))
#define MAKE_MESSAGE(elem)  (static_cast<unsigned int>(elem) + FIRST_MESSAGE)


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
    if ( (dlg != nullptr) && (::IsWindow(dlg->GetSafeHwnd()) == TRUE) )
        result = (::PostMessage( dlg->GetSafeHwnd(), MAKE_MESSAGE(cmd), wParam, lParam) == TRUE);
    return result;
}

DistributedDialog::DistributedDialog( )
    : CPropertySheet    (DistributedDialog::TITLE, nullptr )
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

    ON_MESSAGE( MAKE_MESSAGE(NEDistributedApp::eWndCommands::CmdServiceStartup      ), &DistributedDialog::OnCmdServiceStartup )
    ON_MESSAGE( MAKE_MESSAGE(NEDistributedApp::eWndCommands::CmdServiceNetwork      ), &DistributedDialog::OnCmdServiceNetwork )
    ON_MESSAGE( MAKE_MESSAGE(NEDistributedApp::eWndCommands::CmdServiceConnection   ), &DistributedDialog::OnCmdServiceConnection )
    ON_MESSAGE( MAKE_MESSAGE(NEDistributedApp::eWndCommands::CmdClientConnection    ), &DistributedDialog::OnCmdClientConnection )
    ON_MESSAGE( MAKE_MESSAGE(NEDistributedApp::eWndCommands::CmdClientRegistration  ), &DistributedDialog::OnCmdClientRegistration )
    ON_MESSAGE( MAKE_MESSAGE(NEDistributedApp::eWndCommands::CmdAddConnection       ), &DistributedDialog::OnCmdAddConnection )
    ON_MESSAGE( MAKE_MESSAGE(NEDistributedApp::eWndCommands::CmdRemoveConnection    ), &DistributedDialog::OnCmdRemoveConnection )
    ON_MESSAGE( MAKE_MESSAGE(NEDistributedApp::eWndCommands::CmdUpdateConnection    ), &DistributedDialog::OnCmdUpdateConnection )
    ON_MESSAGE( MAKE_MESSAGE(NEDistributedApp::eWndCommands::CmdDisconnectTriggered ), &DistributedDialog::OnCmdDisconnectTriggered)
    ON_MESSAGE( MAKE_MESSAGE(NEDistributedApp::eWndCommands::CmdSendMessage         ), &DistributedDialog::OnCmdSendMessage )
    ON_MESSAGE( MAKE_MESSAGE(NEDistributedApp::eWndCommands::CmdTypeMessage         ), &DistributedDialog::OnCmdTypeMessage )
    ON_MESSAGE( MAKE_MESSAGE(NEDistributedApp::eWndCommands::CmdSetDirectConnection ), &DistributedDialog::OnCmdSetDirectConnection )
    ON_MESSAGE( MAKE_MESSAGE(NEDistributedApp::eWndCommands::CmdChatClosed          ), &DistributedDialog::OnCmdChatClosed )

END_MESSAGE_MAP( )


// DistributedDialog message handlers

BOOL DistributedDialog::OnInitDialog()
{
    CPropertySheet::OnInitDialog();

    // IDM_ABOUTBOX must be in the system command range.
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != nullptr )
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

    // Initialize application, enable logging, servicing, timer and watchdog.
    Application::initApplication( true, true, false, true, true );

    SetActivePage(&mPageConnections);
    SetActivePage(&mPageMessaging);
    SetActivePage(&mPageSetup);

    CButton * btnOk = reinterpret_cast<CButton *>(GetDlgItem( IDOK ));
    CButton * btnCancel = reinterpret_cast<CButton *>(GetDlgItem( IDCANCEL ));
    if ( (btnOk != nullptr) && (btnCancel != nullptr) )
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
        mPageMessaging.OnServiceStartup( false, nullptr );
        mPageSetup.OnServiceStartup( false, nullptr );
        mPageConnections.OnServiceStartup( false, nullptr );
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
        mPageMessaging.OnServiceNetwork( false, nullptr );
        mPageSetup.OnServiceNetwork( false, nullptr );
        mPageConnections.OnServiceNetwork( false, nullptr );
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
    if ( data != nullptr )
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
    if ( data != nullptr )
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
    if ( pageChat != nullptr )
    {
        for (MapChatPages::MAPPOS pos = mMapChatPages.firstPosition( ); mMapChatPages.isValidPosition(pos); pos = mMapChatPages.nextPosition( pos ) )
        {
            PageChat * page = mMapChatPages.valueAtPosition( pos );
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
    NECommon::sMessageData * data = reinterpret_cast<NECommon::sMessageData *>(lParam);
    if ( data != nullptr )
    {
        if ( wParam != static_cast<WPARAM>(NECommon::InvalidCookie) )
            mPageMessaging.OnSendMessage( static_cast<uint32_t>(wParam), *data );
        delete data;
    }
    return 0;
}

LRESULT DistributedDialog::OnCmdTypeMessage( WPARAM wParam, LPARAM lParam )
{
    NECommon::sMessageData * data = reinterpret_cast<NECommon::sMessageData *>(lParam);
    if ( data != nullptr )
    {
        if ( wParam != static_cast<WPARAM>(NECommon::InvalidCookie) )
            mPageMessaging.OnTypeMessage( static_cast<uint32_t>(wParam), *data );
        delete data;
    }
    return 0;
}

bool DistributedDialog::OutputMessage( NEDistributedApp::eWndCommands cmd, void * sender, NECommon::sMessageData * data )
{
    bool result = false;
    if ( data != nullptr )
    {
        CWnd * mainFrame = theApp.GetMainWnd( );
        if ( mainFrame != nullptr )
        {
            HWND hWnd = static_cast<DistributedDialog *>(mainFrame)->mPageMessaging.GetSafeHwnd();
            if ( ::IsWindow(hWnd) )
                result = ::PostMessage(hWnd, MAKE_MESSAGE(cmd), reinterpret_cast<WPARAM>(sender), reinterpret_cast<LPARAM>(data)) == TRUE;
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
    if ( chatPage != nullptr )
    {
        ASSERT(mMapChatPages.contains(serviceName) == false);
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
    if ( dlg != nullptr )
        dlg->SetTitle(dlg->mCaption);
}

void DistributedDialog::ChangeCaption( LPCTSTR newCaption )
{
    DistributedDialog * dlg = DistributedDialog::GetDialog( );
    if ( (newCaption != nullptr) && (dlg != nullptr) )
        dlg->SetTitle( newCaption );
}

bool DistributedDialog::RemoveChatPage( const String & connectName )
{
    bool result = false;
    MapChatPages::MAPPOS pos = mMapChatPages.find(connectName);
    if (mMapChatPages.isValidPosition(pos) )
    {
        result = true;
        PageChat * chatPage = mMapChatPages.valueAtPosition(pos);
        if ( chatPage != nullptr )
        {
            RemovePage(chatPage);
            delete chatPage;
        }

        mMapChatPages.removePosition(pos);
    }

    return result;
}

void DistributedDialog::RemoveAllChatPages( void )
{
    for (MapChatPages::MAPPOS pos = mMapChatPages.firstPosition(); mMapChatPages.isValidPosition(pos); pos = mMapChatPages.nextPosition(pos) )
    {
        PageChat * chatPage = mMapChatPages.valueAtPosition( pos );
        if ( chatPage != nullptr )
        {
            RemovePage( chatPage );
            delete chatPage;
        }
    }
    mMapChatPages.clear();
}

LRESULT DistributedDialog::OnCmdSetDirectConnection( WPARAM wParam, LPARAM lParam )
{
    NEDirectConnection::sInitiator      * initiator     = reinterpret_cast<NEDirectConnection::sInitiator *>( wParam );
    NEDirectConnection::ListParticipants* listParties   = reinterpret_cast<NEDirectConnection::ListParticipants *>( lParam );

    if ( (initiator != nullptr) && (listParties != nullptr) )
        AddChatPage(*initiator, *listParties, false);
    if ( initiator != nullptr)
        delete initiator;
    if ( listParties != nullptr)
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
    if ( btnOk != nullptr )
    {
        btnOk->ShowWindow( TRUE );
        btnOk->SetFocus( );
        UINT style = btnOk->GetButtonStyle( );
        style |= BS_DEFPUSHBUTTON;
        btnOk->SetButtonStyle( style );
        btnOk->ShowWindow( FALSE );
    }
}
