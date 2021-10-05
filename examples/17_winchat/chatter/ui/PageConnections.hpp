#pragma once

#include "areg/base/GEGlobal.h"
#include "chatter/res/resource.h"
#include "areg/base/String.hpp"
#include "generated/NEDirectConnection.hpp"
#include "generated/NEConnectionManager.hpp"

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
    void OnAddConnection( NEConnectionManager::sConnection & data );
    void OnRemoveConnection( NEConnectionManager::sConnection & data );
    void OnUpdateConnection( void );
    void OnDisconnectTriggered( void );

    void OnDefaultClicked( void );

public:

    const String & GetRegisteredName( void ) const;

    const uint32_t GetRegisteredCookie( void ) const;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog( );

	DECLARE_MESSAGE_MAP()
protected:
    afx_msg void OnClickedButtonInitiateChat( );
    afx_msg void OnKickIdle( );
    afx_msg void OnBnUpdateInitiateChat( CCmdUI* pCmdUI );
    afx_msg void OnDestroy( );

private:
    inline void cleanService( void );

    inline bool isServiceConnected( void ) const;

    inline void setHeaders( void );

    inline int getSelectedConnections( NEDirectConnection::sInitiator & outParticipant, NEDirectConnection::ListParticipants & outListParticipants );

    inline void addConnection( const NEConnectionManager::sConnection & connection );

    inline int findConnection( const NEConnectionManager::sConnection & connection ) const;

    inline void removeConnection( const NEConnectionManager::sConnection & connection );

    inline void unloadModel( void );

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
