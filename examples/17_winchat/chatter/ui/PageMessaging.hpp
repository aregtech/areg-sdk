#pragma once

#include "areg/base/String.hpp"
#include "areg/base/TEArrayList.hpp"
#include "generated/NECommon.hpp"
#include "chatter/res/resource.h"
#include "generated/NEConnectionManager.hpp"

class Component;
class DispatcherThread;
class CentralMessaging;
class ConnectionHandler;
class ConnectionList;

// PageMessaging dialog

class PageMessaging : public CPropertyPage
{
    static LPCTSTR HEADER_TITILES[];

	DECLARE_DYNAMIC(PageMessaging)

public:
	PageMessaging( ConnectionHandler & handlerConnection );
	virtual ~PageMessaging();

// Dialog Data
	enum { IDD = IDD_PAGE_MESSAGES };

public:

    void OnServiceStartup( bool isStarted, Component * owner );
    void OnServiceNetwork( bool isConnected, DispatcherThread * ownerThread );
    void OnServiceConnection( bool isConnected, DispatcherThread * ownerThread );
    void OnClientConnection( bool isConnected, DispatcherThread *dispThread );
    void OnClientRegistration( bool isRegistered, DispatcherThread * dispThread );
    void OnAddConnection( NEConnectionManager::sConnection & data );
    void OnRemoveConnection( NEConnectionManager::sConnection & data );
    void OnUpdateConnection( void );
    void OnDisconnectTriggered( void );
    void OnTypeMessage( uint32_t cookie, NECommon::sMessageData & data );
    void OnSendMessage( uint32_t cookie, NECommon::sMessageData & data );

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

    void outputMessage( CString nickName, CString message, CString dateStart, CString dateEnd, uint32_t cookie );
    void outputMessage( const String & nickname, const String & message, const uint64_t begin, const uint64_t end, uint32_t cookie );
    void outputMessage( const String & nickname, const String & message, const DateTime & begin, const DateTime & end, uint32_t cookie);
    void outputTyping( CString nickName, CString message, uint32_t cookie );
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
    CentralMessaging *  mCentralMessage;
    // Last inserted message
    INT                 mLastItem;
    // typed string
    CString             mTextMsg;
    // instance of connection handler object
    ConnectionHandler & mConnectionHandler;
    BOOL                mSendEnabled;
};
