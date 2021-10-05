#pragma once


// PageConnections dialog

#include "register/res/resource.h"
#include "register/NECentralApp.hpp"
#include "generated/NECommon.hpp"
#include "areg/base/TEArrayList.hpp"

class PageConnections : public CPropertyPage
{

    static LPCTSTR HEADER_TITILES[];

    typedef enum E_Header
    {
          NickName  = 0
        , Message
        , SentAt
        , ReceivedAt
    } eHeader;

    typedef TEArrayList<NECommon::sMessageData *>   TypingList;

DECLARE_DYNAMIC(PageConnections)

// Dialog Data
	enum { IDD = IDD_PAGE_CONNECTIONS };

public:
	PageConnections();
	virtual ~PageConnections();

public:
    void ServiceConnected( bool isConnected );

    void OutputMessage( CString nickName, CString message, CString dateStart, CString dateEnd, LPARAM data );

    void OnDefaultClicked( void );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog( );

	DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnClickedButtonBroadcast();
    afx_msg void OnKickIdle( void );
    afx_msg void OnBtnBroadcastUpdate( CCmdUI* pCmdUI );
    afx_msg void OnEditBroadcastUpdate( CCmdUI* pCmdUI );
    afx_msg LRESULT OnCmdRegistered( WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnCmdUnregistered( WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnCmdSendMessage( WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnCmdTypeMessage( WPARAM wParam, LPARAM lParam );

private:
    void setHeaders( void );

    // int findInTyping( LPCTSTR nickName );

    int findInTyping( unsigned int cookie );

private:
    CListCtrl   mCtrlList;
    INT         mLastItem;
    TypingList  mTypingList;
    // The message to broadcast
    CString     mTextBroadcast;
    UINT        mRegistered;
    BOOL        mEditEnabled;
};
