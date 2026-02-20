#pragma once

#include "areg/base/GEGlobal.h"
#include "chatter/res/resource.h"
#include "areg/base/String.hpp"
#include "examples/20_winchat/services/DirectConnection.hpp"
#include "examples/20_winchat/services/ConnectionManager.hpp"

class String;
class Component;
class ConnectionList;
class DispatcherThread;
class ConnectionHandler;

// PageConnections dialog

class PageConnections : public CPropertyPage
{
    static LPCTSTR HEADER_TITILES[];
	DECLARE_DYNAMIC(PageConnections)

public:
	PageConnections(ConnectionHandler & handlerConnection);
	virtual ~PageConnections();

// Dialog Data
	enum { IDD = IDD_PAGE_CONNECTIONS };

public:

    void OnServiceStartup( bool isStarted, Component * owner );
    void OnServiceNetwork( bool isConnected, DispatcherThread * ownerThread );
    void OnServiceConnection( bool isConnected, DispatcherThread * ownerThread );
    void OnClientConnection( bool isConnected, DispatcherThread *dispThread );
    void OnClientRegistration( bool isRegistered, DispatcherThread * dispThread );
    void OnAddConnection( ConnectionManager::sConnection & data );
    void OnRemoveConnection( ConnectionManager::sConnection & data );
    void OnUpdateConnection();
    void OnDisconnectTriggered();

    void OnDefaultClicked();

public:

    const String & GetRegisteredName() const;

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

    inline int getSelectedConnections( DirectConnection::sInitiator & outParticipant, DirectConnection::ListParticipants & outListParticipants );

    inline void addConnection( const ConnectionManager::sConnection & connection );

    inline int findConnection( const ConnectionManager::sConnection & connection ) const;

    inline void removeConnection( const ConnectionManager::sConnection & connection );

    inline void unloadModel();

    inline bool loadModel( const String & nickName, const uint32_t cookie );

private:
    // List of connections
    CListCtrl           mCtrlConnections;
    // Connection list service client
    ConnectionList *    mClientConnections;
    // The name of direct connection model, which contains service
    String              mDirectConnectModel;
    // The name of generated direct connection service
    String              mDirectConnectService;
    // The instance of connection handler object
    ConnectionHandler & mConnectionHandler;
    BOOL                mChatEnable;
};
