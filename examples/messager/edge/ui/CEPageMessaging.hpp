#pragma once

#include "areg/base/CEString.hpp"
#include "areg/base/TEArrayList.hpp"
#include "shared/NECommonSettings.hpp"
#include "edge/res/resource.h"
#include "shared/generated/NEConnectionManager.hpp"

class CEComponent;
class CEDispatcherThread;
class CECentralMessaging;
class CEConnectionHandler;
class CEConnectionList;

// CEPageMessaging dialog

class CEPageMessaging : public CPropertyPage
{
    static LPCTSTR HEADER_TITILES[];

	DECLARE_DYNAMIC(CEPageMessaging)

public:
	CEPageMessaging( CEConnectionHandler & handlerConnection );
	virtual ~CEPageMessaging();

// Dialog Data
	enum { IDD = IDD_PAGE_MESSAGES };

public:

    void OnServiceStartup( bool isStarted, CEComponent * owner );
    void OnServiceNetwork( bool isConnected, CEDispatcherThread * ownerThread );
    void OnServiceConnection( bool isConnected, CEDispatcherThread * ownerThread );
    void OnClientConnection( bool isConnected, CEDispatcherThread *dispThread );
    void OnClientRegistration( bool isRegistered, CEDispatcherThread * dispThread );
    void OnAddConnection( NEConnectionManager::sConnection & data );
    void OnRemoveConnection( NEConnectionManager::sConnection & data );
    void OnUpdateConnection( void );
    void OnDisconnectTriggered( void );
    void OnTypeMessage( uint32_t cookie, NECommonSettings::sMessageData & data );
    void OnSendMessage( uint32_t cookie, NECommonSettings::sMessageData & data );

    void OnDefaultClicked( void );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog( );

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnDestroy( );
    afx_msg void OnClickedButtonSend( );
    afx_msg void OnClickedCheckMessages( );
    afx_msg void OnClickedCheckKeytype( );
    afx_msg void OnClickedCheckBroadcast( );
    afx_msg void OnChangeEditMessageAll( );
    afx_msg void OnKickIdle( );
    afx_msg void OnButtonUpdateSend( CCmdUI* pCmdUI );
    afx_msg void OnCheckUpdate( CCmdUI* pCmdUI );
    afx_msg LRESULT OnOutputMessage( WPARAM wParam, LPARAM lParam );

private:

    void setHeaders( void );

    void cleanService( void );

    bool isServiceConnected( void ) const;

    void outputMessage( CString & nickName, CString & message, CString & dateStart, CString & dateEnd, uint32_t cookie );
    void outputMessage( const CEString & nickname, const CEString & message, const uint64_t begin, const uint64_t end, uint32_t cookie );
    void outputMessage( const CEString & nickname, const CEString & message, const CEDateTime & begin, const CEDateTime & end, uint32_t cookie);
    void outputTyping( CString & nickName, CString & message, uint32_t cookie );
    void removeTyping( const CString & nickName, uint32_t cookie );

private:
     // When selected, receives broadcasting message
    BOOL                mIsBroadcast;
   // When selected, receives messages when typing key
    BOOL                mIsKeytype;
    // When selected, receives messages.
    BOOL                mIsMessages;
    // The list of messages
    CListCtrl           mCtrlList;
    // Central messaging service
    CECentralMessaging *mCentralMessage;
    // Last inserted message
    uint32_t            mLastItem;
    // typed string
    CString             mTextMsg;
    // instance of connection handler object
    CEConnectionHandler & mConnectionHandler;
    BOOL                mSendEnabled;
};
