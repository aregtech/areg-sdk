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

// CEDistributedDialog.hpp : interface of the CEDistributedDialog class
//

#pragma once

#include "edge/res/resource.h"
#include "edge/ui/CEPageNetworkSetup.hpp"
#include "edge/ui/CEPageMessaging.hpp"
#include "edge/ui/CEPageConnections.hpp"
#include "edge/NEDistributedApp.hpp"
#include "edge/services/CEConnectionHandler.hpp"
#include "areg/src/base/TEHashMap.hpp"

class CEPageChat;
typedef TEHashMap<CEString, CEPageChat *, const CEString &, CEPageChat *>   MapChatPages;

class CEDistributedDialog   : public CPropertySheet
                            , public CEConnectionHandler
{
public:
    static bool OutputMessage( NEDistributedApp::eWndCommands cmd, void * sender, NECommonSettings::sMessageData * data );

    static CEDistributedDialog * GetDialog( void );

    static void DefaultCaption( void );

    static void ChangeCaption( LPCTSTR newCaption );

    static bool PostServiceMessage( NEDistributedApp::eWndCommands cmd, WPARAM wParam, LPARAM lParam );

    // Construction
public:
    CEDistributedDialog( void );	// standard constructor

    virtual ~CEDistributedDialog( void );

                                            // Dialog Data
    enum { TITLE = IDS_APPLICATION_TITLE };

public:

    CEPageChat * AddChatPage( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParties, bool isInitiator );

    bool RemoveChatPage( const CEString & connectName );

    void RemoveAllChatPages( void );

protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

                                                        // Implementation
protected:
    HICON m_hIcon;

    // Generated message map functions
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg void OnDestroy();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnRedirectOK( void );

    afx_msg LRESULT OnCmdServiceStartup( WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnCmdServiceNetwork( WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnCmdServiceConnection( WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnCmdClientConnection( WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnCmdClientRegistration( WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnCmdAddConnection( WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnCmdRemoveConnection( WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnCmdUpdateConnection( WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnCmdDisconnectTriggered( WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnCmdSetDirectConnection( WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnCmdChatClosed( WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnCmdSendMessage( WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnCmdTypeMessage( WPARAM wParam, LPARAM lParam );
    DECLARE_MESSAGE_MAP()

private:
    inline CEDistributedDialog & self( void  );

private:
    CEPageNetworkSetup  mPageSetup;
    CEPageMessaging     mPageMessaging;
    CEPageConnections   mPageConnections;

    MapChatPages        mMapChatPages;

    CString             mCaption;

    CString             mCaptionInit;
};

inline CEDistributedDialog & CEDistributedDialog::self( void  )
{   return (*this);     }
