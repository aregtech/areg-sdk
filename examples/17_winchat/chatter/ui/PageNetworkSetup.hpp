#pragma once

#include "areg/base/String.hpp"
#include "chatter/res/resource.h"
#include "generated/NEConnectionManager.hpp"

class Component;
class NetworkSetup;
class DispatcherThread;
class ConnectionHandler;

// PageNetworkSetup dialog

class PageNetworkSetup : public CPropertyPage
{
	DECLARE_DYNAMIC(PageNetworkSetup)

public:
	PageNetworkSetup( ConnectionHandler & handlerConnection);
	virtual ~PageNetworkSetup();

// Dialog Data
	enum { IDD = IDD_PAGE_SETUP_NETWORK };

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
    NetworkSetup *        mNetworkSetup;
    // Flag, indicating whether the network connection is pending or not
    ConnectionHandler &   mConnectionHandler;
    bool                    mConnectPending;
    bool                    mRegisterPending;
    BOOL                    mConnectEnable;
    BOOL                    mDisconnectEnabled;
    BOOL                    mRegisterEnabled;
};
