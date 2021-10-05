#pragma once


// PageChat dialog

#pragma once

#include "chatter/res/stdafx.h"
#include "chatter/res/resource.h"
#include "generated/NECommon.hpp"
#include "areg/base/GEGlobal.h"
#include "areg/base/String.hpp"
#include "areg/base/TEArrayList.hpp"
#include "areg/base/TEHashMap.hpp"
#include "generated/NEDirectConnection.hpp"
#include "chatter/services/ChatPrticipantHandler.hpp"

class PageChat  : public CPropertyPage
                , public ChatPrticipantHandler
{

private:
    static LPCTSTR HEADER_TITILES[];

    static const USHORT TIMER_MIN_VALUE     = 1;
    static const USHORT TIMER_MAX_VALUE     = 1000;
    static const USHORT AUTOMESSAGE_MAX_LEN = 20;

	DECLARE_DYNAMIC(PageChat)

public:
	PageChat( const String & serviceName
              , const NEDirectConnection::sInitiator & initiator
              , const NEDirectConnection::ListParticipants & listParties
              , const NEDirectConnection::sParticipant & ownerConnection
              , bool isInitiator );
	virtual ~PageChat();

// Dialog Data
	enum { IDD = IDD_PAGE_CHAT };

public:
    void OnDefaultClicked( void );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog( );
    virtual BOOL OnKillActive( );
    virtual BOOL OnSetActive( );

public:
    afx_msg void OnClickedCheckChatMessages( );
    afx_msg void OnClickedCheckChatTyping( );
    afx_msg void OnUpdateEditChat( );
    afx_msg void OnClickedButtonChatSend( );
    afx_msg void OnDestroy( );
    afx_msg void OnClickedButtonCloseChat( );
    afx_msg void OnKickIdle( );
    afx_msg void OnButtonUpdateChatSend( CCmdUI* pCmdUI);
    afx_msg void OnEditUpdateChat( CCmdUI* pCmdUI);
    afx_msg LRESULT OnCmdChatJoined( WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnCmdChatMessage( WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnCmdChatTyping( WPARAM wParam, LPARAM lParam );

private:
    void setHeaders( void );
    void setTabTitle( const String & title );
    void outputMessage(CString nickName, CString message, CString dateStart, CString dateEnd, uint32_t cookie, bool removeType = true);
    void outputTyping(CString nickName, CString message, uint32_t cookie );
    void removeTyping(CString nickName, uint32_t cookie);
    bool isActivePage( void );
    void sendMessage( void );
    void sendType( void );
    void startTimer( void );
    void stopTimer(void);

private:
    // The chatting edit control
    CEdit               mCtrlChat;
    // Chatting message
    CString             mChatMsg;
    // The list of char participants
    CListCtrl           mCtrlList;
    // If true, can receive chatting messages
    BOOL                mIsChatMessage;
    // If true, receives messages when typing
    BOOL                mIsChatTyping;
    // Flag, indicating whether the chat was initiated by other or current participant
    const bool          mIsChatInitiator;
    // The last item output in the list control
    INT                 mLastItem;
    CString             mTitle;
    BOOL                mEditEnabled;
    // If enabled, makes autotype
    BOOL                mDoAutotype;
    // The timer in milliseconds to generate letter.
    INT                mTimerValue;
    // The timer value control
    CEdit               mCtrlTimerValue;
    // The timer spin
    CSpinButtonCtrl     mCtrlTimerSpin;
    UINT_PTR            mTimerId;

private:
    PageChat( void );
    PageChat( const PageChat & /*src*/ );
    const PageChat & operator = ( const PageChat & /*src*/ );
public:
    afx_msg void OnBnClickedCheckAuto();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnEnChangeChatTimer();
    afx_msg void OnDeltaposChatTimerSpin(NMHDR* pNMHDR, LRESULT* pResult);
};
