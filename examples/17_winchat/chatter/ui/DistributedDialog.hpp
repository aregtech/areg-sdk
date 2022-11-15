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

// DistributedDialog.hpp : interface of the DistributedDialog class
//

#pragma once

#include "chatter/res/resource.h"
#include "chatter/ui/PageNetworkSetup.hpp"
#include "chatter/ui/PageMessaging.hpp"
#include "chatter/ui/PageConnections.hpp"
#include "chatter/NEDistributedApp.hpp"
#include "chatter/services/ConnectionHandler.hpp"
#include "areg/base/Containers.hpp"

class PageChat;
typedef TEStringHashMap<PageChat *>   MapChatPages;

class DistributedDialog : public CPropertySheet
                        , public ConnectionHandler
{
public:
    static bool OutputMessage( NEDistributedApp::eWndCommands cmd, void * sender, NECommon::sMessageData * data );

    static DistributedDialog * GetDialog( void );

    static void DefaultCaption( void );

    static void ChangeCaption( LPCTSTR newCaption );

    static bool PostServiceMessage( NEDistributedApp::eWndCommands cmd, WPARAM wParam, LPARAM lParam );

    // Construction
public:
    DistributedDialog( void );	// standard constructor

    virtual ~DistributedDialog( void );

                                            // Dialog Data
    enum { TITLE = IDS_APPLICATION_TITLE };

public:

    PageChat * AddChatPage( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParties, bool isInitiator );

    bool RemoveChatPage( const String & connectName );

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
    inline DistributedDialog & self( void  );

private:
    PageNetworkSetup  mPageSetup;
    PageMessaging     mPageMessaging;
    PageConnections   mPageConnections;

    MapChatPages        mMapChatPages;

    CString             mCaption;

    CString             mCaptionInit;
};

inline DistributedDialog & DistributedDialog::self( void  )
{   return (*this);     }
