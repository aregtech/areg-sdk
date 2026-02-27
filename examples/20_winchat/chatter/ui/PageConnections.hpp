#pragma once

#include "areg/base/GEGlobal.h"
#include "chatter/res/resource.h"
#include "areg/base/String.hpp"
#include "examples/20_winchat/services/DirectConnection.hpp"
#include "examples/20_winchat/services/ConnectionManager.hpp"

namespace areg { class Component; }
class ConnectionList;
namespace areg { class DispatcherThread; }
namespace aregext { class ConnectionHandler; }

// PageConnections dialog

class PageConnections : public CPropertyPage
{
    static LPCTSTR HEADER_TITILES[];
	DECLARE_DYNAMIC(PageConnections)

public:
	PageConnections(aregext::ConnectionHandler & handlerConnection);
	virtual ~PageConnections();

// Dialog Data
	enum { IDD = IDD_PAGE_CONNECTIONS };

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

    void OnDefaultClicked();

public:

    const areg::String & GetRegisteredName() const;

    uint32_t GetRegisteredCookie() const;

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
    BOOL OnInitDialog( ) override;

	DECLARE_MESSAGE_MAP()
protected:
    afx_msg void OnClickedButtonInitiateChat( );
    afx_msg void OnKickIdle( );
    afx_msg void OnBnUpdateInitiateChat( CCmdUI* pCmdUI );
    afx_msg void OnDestroy( );

private:
    inline void cleanService();

    inline bool isServiceConnected() const;

    inline void setHeaders();

    inline int32_t getSelectedConnections( DirectConnection::sInitiator & outParticipant, DirectConnection::ListParticipants & outListParticipants );

    inline void addConnection( const ConnectionManager::ConnectionRecord & connection );

    inline int32_t findConnection( const ConnectionManager::ConnectionRecord & connection ) const;

    inline void removeConnection( const ConnectionManager::ConnectionRecord & connection );

    inline void unloadModel();

    inline bool loadModel( const areg::String & nickName, const uint32_t cookie );

private:
    // List of connections
    CListCtrl           mCtrlConnections;
    // Connection list service client
    ConnectionList *    mClientConnections;
    // The name of direct connection model, which contains service
    areg::String              mDirectConnectModel;
    // The name of generated direct connection service
    areg::String              mDirectConnectService;
    // The instance of connection handler object
    aregext::ConnectionHandler & mConnectionHandler;
    BOOL                mChatEnable;
};
