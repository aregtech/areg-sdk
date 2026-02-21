#pragma once


// PageConnections dialog

#include "register/res/resource.h"
#include "register/CentralApp.hpp"
#include "common/ChatDefs.hpp"
#include "areg/base/ArrayList.hpp"

class PageConnections : public CPropertyPage
{

    static LPCTSTR HEADER_TITILES[];

    using TypingList = ArrayList<chat::sMessageData *>;

DECLARE_DYNAMIC(PageConnections)

// Dialog Data
enum { IDD = IDD_PAGE_CONNECTIONS };

public:
	PageConnections();
	virtual ~PageConnections();

public:
    void Connected( bool isConnected );

    void OutputMessage( CString nickName, CString message, CString dateStart, CString dateEnd, LPARAM data );

    void OnDefaultClicked();

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
    BOOL OnInitDialog( ) override;

	DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnClickedButtonBroadcast();
    afx_msg void OnKickIdle();
    afx_msg void OnBtnBroadcastUpdate( CCmdUI* pCmdUI );
    afx_msg void OnEditBroadcastUpdate( CCmdUI* pCmdUI );
    afx_msg LRESULT OnCmdRegistered( WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnCmdUnregistered( WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnCmdSendMessage( WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnCmdTypeMessage( WPARAM wParam, LPARAM lParam );

private:
    void setHeaders();

    // int32_t findInTyping( LPCTSTR nickName );

    int32_t findInTyping( uint32_t cookie );

private:
    CListCtrl   mCtrlList;
    INT         mLastItem;
    TypingList  mTypingList;
    // The message to broadcast
    CString     mTextBroadcast;
    UINT        mRegistered;
    BOOL        mEditEnabled;
};
