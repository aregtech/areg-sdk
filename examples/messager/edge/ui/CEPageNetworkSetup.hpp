#pragma once

#include "areg/src/base/CEString.hpp"
#include "edge/res/resource.h"
#include "shared/generated/NEConnectionManager.hpp"

class CEComponent;
class CENetworkSetup;
class CEDispatcherThread;
class CEConnectionHandler;

// CEPageNetworkSetup dialog

class CEPageNetworkSetup : public CPropertyPage
{
	DECLARE_DYNAMIC(CEPageNetworkSetup)

public:
	CEPageNetworkSetup( CEConnectionHandler & handlerConnection);
	virtual ~CEPageNetworkSetup();

// Dialog Data
	enum { IDD = IDD_PAGE_SETUP_NETWORK };

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

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog( );

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnDestroy( );
    afx_msg void OnClickedBrokerConnect();
    afx_msg void OnClickedBrokerDisconnect();
    afx_msg void OnClickedButtonRegister( );
    afx_msg void OnUpdateEditNickname();
    afx_msg void OnKickIdle( );
    afx_msg void OnBnUpdateBrokerConnect( CCmdUI* pCmdUI);
    afx_msg void OnBnUdateBrokerDisconnect( CCmdUI* pCmdUI);
    afx_msg void OnUpdateRemoteData( CCmdUI* pCmdUI );
    afx_msg void OnUpdateNickname( CCmdUI* pCmdUI );
    afx_msg void OnUpdateButtonRegister( CCmdUI* pCmdUI );

private:
    void cleanService( void );

    bool isServiceConnected( void ) const;

    bool canRegistered( void ) const;

    void setFocusNickname( void ) const;

private:
    // IP Address of service broker
    CIPAddressCtrl          mCtrlAddress;
    // Port number of service broker
    CEdit                   mCtrlPort;
    // The nick name of connected client
    CString                 mNickName;
    // // The nick-name field control
    CEdit                   mCtrlNickName;
    // The port number of service broker
    USHORT                  mBrokerPort;
    // Network setup service client
    CENetworkSetup *        mNetworkSetup;
    // Flag, indicating whether the network connection is pending or not
    CEConnectionHandler &   mConnectionHandler;
    bool                    mConnectPending;
    bool                    mRegisterPending;
    BOOL                    mConnectEnable;
    BOOL                    mDisconnectEnabled;
    BOOL                    mRegisterEnabled;
};
