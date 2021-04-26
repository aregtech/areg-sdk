#pragma once

#include "areg/src/base/GEGlobal.h"
#include "edge/res/resource.h"
#include "areg/src/base/CEString.hpp"
#include "shared/generated/NEDirectConnection.hpp"
#include "shared/generated/NEConnectionManager.hpp"

class CEString;
class CEComponent;
class CEConnectionList;
class CEDispatcherThread;
class CEConnectionHandler;

// CEPageConnections dialog

class CEPageConnections : public CPropertyPage
{
    static LPCTSTR HEADER_TITILES[];
	DECLARE_DYNAMIC(CEPageConnections)

public:
	CEPageConnections(CEConnectionHandler & handlerConnection);
	virtual ~CEPageConnections();

// Dialog Data
	enum { IDD = IDD_PAGE_CONNECTIONS };

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

    void OnDefaultClicked( void );

public:

    const CEString & GetRegisteredName( void ) const;

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

    inline bool loadModel( const CEString & nickName, const uint32_t cookie );

private:
    // List of connections
    CListCtrl               mCtrlConnections;
    // Connection list service client
    CEConnectionList *      mClientConnections;
    // The name of direct connection model, which contains service
    CEString                mDirectConnectModel;
    // The name of generated direct connection service
    CEString                mDirectConnectService;
    // The instance of connection handler object
    CEConnectionHandler &   mConnectionHandler;
    BOOL                    mChatEnable;
};
