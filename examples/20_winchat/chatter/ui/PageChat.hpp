#pragma once


// PageChat dialog

#pragma once

#include "chatter/res/stdafx.h"
#include "chatter/res/resource.h"
#include "common/ChatDefs.hpp"
#include "areg/base/GEGlobal.h"
#include "areg/base/String.hpp"
#include "areg/base/ArrayList.hpp"
#include "areg/base/HashMap.hpp"
#include "examples/20_winchat/services/DirectConnection.hpp"
#include "chatter/services/ChatPrticipantHandler.hpp"

class PageChat  : public CPropertyPage
                , public ChatPrticipantHandler
{

private:
    static LPCTSTR HEADER_TITILES[];

    static const USHORT TIMER_MIN_VALUE     = 15;
    static const USHORT TIMER_MAX_VALUE     = 1000;
    static const USHORT AUTOMESSAGE_MAX_LEN = 20;

	DECLARE_DYNAMIC(PageChat)

public:
	PageChat( const String & serviceName
              , const DirectConnection::sInitiator & initiator
              , const DirectConnection::ListParticipants & listParties
              , const DirectConnection::sParticipant & ownerConnection
              , bool isInitiator );
	virtual ~PageChat() = default;

// Dialog Data
	enum { IDD = IDD_PAGE_CHAT };

public:
    void OnDefaultClicked();

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    BOOL OnInitDialog( ) override;
    BOOL OnKillActive( ) override;
    BOOL OnSetActive( ) override;

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

    afx_msg void OnBnClickedCheckAuto();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnEnChangeChatTimer();
    afx_msg void OnDeltaposChatTimerSpin(NMHDR* pNMHDR, LRESULT* pResult);

private:
    void setHeaders();
    void setTabTitle( const String & title );
    void outputMessage(CString nickName, CString message, CString dateStart, CString dateEnd, uint32_t cookie, bool removeType = true);
    void outputTyping(CString nickName, CString message, uint32_t cookie );
    void removeTyping(const CString & nickName, uint32_t cookie);
    bool isActivePage();
    void sendMessage();
    void sendType();
    void startTimer();
    void stopTimer();

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
    // If enabled, makes auto-type
    BOOL                mDoAutotype;
    // The timer in milliseconds to generate letter.
    INT                mTimerValue;
    // The timer value control
    CEdit               mCtrlTimerValue;
    // The timer spin
    CSpinButtonCtrl     mCtrlTimerSpin;
    // The timer ID
    UINT_PTR            mTimerId;
    // The name of dynamically created model.
    String              mModelName;

private:
    AREG_NOCOPY_NOMOVE(PageChat);
};
