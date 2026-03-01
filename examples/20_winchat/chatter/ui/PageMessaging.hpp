#pragma once

#include "areg/base/String.hpp"
#include "areg/base/ArrayList.hpp"
#include "common/ChatDefs.hpp"
#include "chatter/res/resource.h"
#include "examples/20_winchat/services/ConnectionManager.hpp"

namespace areg { class Component; }
namespace areg { class DispatcherThread; }
class CentralMessaging;
namespace aregext { class ConnectionHandler; }
class ConnectionList;

// PageMessaging dialog

class PageMessaging : public CPropertyPage
{
    static LPCTSTR HEADER_TITILES[];

	DECLARE_DYNAMIC(PageMessaging)

public:
	PageMessaging( aregext::ConnectionHandler & handlerConnection );
	virtual ~PageMessaging();

// Dialog Data
	enum { IDD = IDD_PAGE_MESSAGES };

public:

    void OnServiceStartup( bool isStarted, areg::Component * owner );
    void OnServiceNetwork( bool isConnected, areg::DispatcherThread * ownerThread );
    void OnServiceConnection( bool isConnected, areg::DispatcherThread * ownerThread );
    void OnClientConnection( bool isConnected, areg::DispatcherThread *dispThread );
    void OnClientRegistration( bool isRegistered, areg::DispatcherThread * dispThread );
    void OnAddConnection( ConnectionManager::ConnectionRecord & data );
    void OnRemoveConnection( ConnectionManager::ConnectionRecord & data );
    void OnUpdateConnection();
    void OnDisconnectTriggered();
    void OnTypeMessage( uint32_t cookie, chat:: MessageData & data );
    void OnSendMessage( uint32_t cookie, chat:: MessageData & data );

    void OnDefaultClicked();

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
    BOOL OnInitDialog( ) override;

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

    void setHeaders();

    void cleanService();

    bool is_service_connected() const;

    void outputMessage( CString nickName, CString message, CString dateStart, CString dateEnd, uint32_t cookie );
    void outputMessage( const areg::String & nickname, const areg::String & message, const uint64_t begin, const uint64_t end, uint32_t cookie );
    void outputMessage( const areg::String & nickname, const areg::String & message, const areg::DateTime & begin, const areg::DateTime & end, uint32_t cookie);
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
    aregext::ConnectionHandler & mConnectionHandler;
    BOOL                mSendEnabled;
};
