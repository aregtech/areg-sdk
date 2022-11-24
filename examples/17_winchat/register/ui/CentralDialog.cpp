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

// CentralDialogg.cpp : implementation of the CentralDialog class
//

#include "register/res/stdafx.h"
#include "register/ui/CentralDialog.hpp"
#include "register/CentralApp.hpp"
#include "generated/NECommon.hpp"
#include "areg/base/DateTime.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define FIRST_MESSAGE       (WM_USER + 10 + static_cast<unsigned int>(NECentralApp::eWndCommands::CmdFirst))
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


// CentralDialog dialog



CentralDialog::CentralDialog( )
    : CPropertySheet    (CentralDialog::TITLE, nullptr)

    , mPageSetupNetwork ( )
    , mPageConnections  ( )
{
    m_hIcon = AfxGetApp( )->LoadIcon( IDR_MAINFRAME );
    m_psh.dwFlags  |= PSH_NOAPPLYNOW | PSH_USEHICON;
    m_psh.hIcon     = m_hIcon;

    AddPage( &mPageSetupNetwork );
    AddPage( &mPageConnections );
}

bool CentralDialog::StartConnection( const String & ipAddress, unsigned short portNr )
{
    bool result = false;
    CentralDialog * dlg = static_cast<CentralDialog *>(theApp.GetMainWnd());

    if ( dlg != nullptr )
    {
        if ( Application::startMessageRouting(ipAddress, portNr) )
        {
            CString nickName    = NECommon::SERVER_NAME;
            CString dateStart( DateTime::getNow().formatTime().getString() );
            CString message;

            NEMemory::uAlign compData;
            compData.alignInt64.mElement    = reinterpret_cast<uint64_t>(dlg->mPageConnections.GetSafeHwnd());
            ComponentLoader::setComponentData( NECommon::COMP_NAME_CENTRAL_SERVER, compData );
            if ( Application::loadModel( NECommon::MODEL_NAME_CENTRAL_SERVER ) )
            {
                message     = _T("Successfully started servicing ...");
                result = true;
                ::PostMessage(dlg->GetSafeHwnd(), MAKE_MESSAGE(NECentralApp::eWndCommands::CmdServiceConnection), static_cast<WPARAM>(true), 0 );
            }
            else
            {
                message     = _T("Failed to start servicing ...");
            }

            dlg->mPageConnections.OutputMessage(nickName, message, dateStart, CentralApp::EmptyString, 0 );
        }
    }


    return result;
}

void CentralDialog::DoDataExchange(CDataExchange* pDX)
{
    CPropertySheet::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CentralDialog, CPropertySheet)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_COMMAND(IDOK, OnRedirectOK)

    ON_MESSAGE( MAKE_MESSAGE(NECentralApp::eWndCommands::CmdServiceConnection   ), &CentralDialog::OnCmdServiceConnection )
END_MESSAGE_MAP()


// CentralDialog message handlers

BOOL CentralDialog::OnInitDialog()
{
    CPropertySheet::OnInitDialog();

    // Add "About..." menu item to system menu.

    // IDM_ABOUTBOX must be in the system command range.
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != nullptr)
    {
        pSysMenu->AppendMenu( MF_STRING, SC_MINIMIZE, _T( "Minimize" ) );
        pSysMenu->AppendMenu( MF_STRING, SC_RESTORE, _T( "Restore" ) );

        BOOL bNameValid;
        CString strAboutMenu;
        bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
        ASSERT(bNameValid);
        if (!strAboutMenu.IsEmpty())
        {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
        ModifyStyle( 0, WS_MINIMIZEBOX );
    }

    // Set the icon for this dialog.  The areg does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);			// Set big icon
    SetIcon(m_hIcon, FALSE);		// Set small icon

    // Initialize application, enable logging, servicing, timer and watchdog.
    Application::initApplication( true, true, false, true, true );

    SetActivePage(&mPageConnections);
    SetActivePage(&mPageSetupNetwork);

    CButton * btnOk     = reinterpret_cast<CButton *>(GetDlgItem( IDOK ));
    CButton * btnCancel = reinterpret_cast<CButton *>(GetDlgItem( IDCANCEL ));
    if ( (btnOk != nullptr) && (btnCancel != nullptr) )
    {
        btnOk->ShowWindow( TRUE);
        UINT style = btnOk->GetButtonStyle( );
        style |= BS_DEFPUSHBUTTON;
        btnOk->SetButtonStyle( style );
        btnOk->ShowWindow( FALSE );

        btnCancel->SetWindowText( _T( "&Close" ) );
    }

    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CentralDialog::OnSysCommand(UINT nID, LPARAM lParam)
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

void CentralDialog::OnPaint()
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
HCURSOR CentralDialog::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CentralDialog::OnCmdServiceConnection( WPARAM wParam, LPARAM lParam )
{
    bool isConnected = wParam != 0;
    mPageConnections.ServiceConnected(isConnected);
    mPageSetupNetwork.ServiceConnected(isConnected);
    this->SetActivePage(&mPageConnections);
    return 0;
}

void CentralDialog::OnRedirectOK( void )
{
    CPropertyPage * active = GetActivePage();
    if ( active == &mPageSetupNetwork )
    {
        mPageSetupNetwork.OnDefaultClicked( );
    }
    else if ( active == &mPageConnections )
    {
        mPageConnections.OnDefaultClicked();
    }

    CButton * btnOk = reinterpret_cast<CButton *>(GetDlgItem( IDOK ));
    if ( btnOk != nullptr )
    {
        btnOk->ShowWindow( TRUE );
        btnOk->SetFocus();
        UINT style = btnOk->GetButtonStyle( );
        style |= BS_DEFPUSHBUTTON;
        btnOk->SetButtonStyle( style );
        btnOk->ShowWindow( FALSE );
    }
}
