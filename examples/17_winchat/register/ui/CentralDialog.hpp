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

// CentralDialog.hpp : interface of the CentralDialog class
//

#pragma once

#include "register/res/resource.h"
#include "register/ui/PageBrokerSetup.hpp"
#include "register/ui/PageConnections.hpp"


class CentralDialog : public CPropertySheet 
{
    // Construction
public:
    CentralDialog( void );	// standard constructor

                                            // Dialog Data
    enum { TITLE = IDS_APPLICATION_TITLE };

public:
    static bool StartConnection( const String & ipAddress, unsigned short portNr );

protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

protected:
    HICON m_hIcon;

    // Generated message map functions
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg LRESULT OnCmdServiceConnection(WPARAM wParam, LPARAM lParam);
    afx_msg void OnRedirectOK( void );
    DECLARE_MESSAGE_MAP()

private:
    inline CentralDialog & self( void  );

private:
    PageBrokerSetup   mPageSetupNetwork;
    PageConnections   mPageConnections;
};

inline CentralDialog & CentralDialog::self( void  )
{   return (*this);     }
