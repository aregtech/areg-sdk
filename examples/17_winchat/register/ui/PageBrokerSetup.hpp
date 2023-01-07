#pragma once

#include "register/res/resource.h"

// PageBrokerSetup dialog

class PageBrokerSetup : public CPropertyPage
{
	DECLARE_DYNAMIC(PageBrokerSetup)

// Dialog Data
	enum { IDD = IDD_PAGE_SETUP_NETWORK };

public:
	PageBrokerSetup();
	virtual ~PageBrokerSetup() = default;

public:
    void ServiceConnected( bool isConnected );

    void OnDefaultClicked( void );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog( );

	DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnBnClickedBrokerConnect( );
    afx_msg void OnBnClickedBrokerDisconnect( );
    afx_msg void OnKickIdle( );
    afx_msg void OnBnUpdateBrokerConnect( CCmdUI* pCmdUI);
    afx_msg void OnBnUdateBrokerDisconnect( CCmdUI* pCmdUI);
    afx_msg void OnUpdateRemoteData( CCmdUI* pCmdUI );

private:
    // The IP-Address of service broker
    CIPAddressCtrl  mCtrlAddress;
    // Port number of service broker
    CEdit           mCtrlPort;
    // The port number of service broker
    USHORT          mBrokerPort;
    // Flag, indicating whether it is connected
    bool            mIsConnected;
    BOOL            mConnectEnable;
    BOOL            mDisconnectEnabled;
};
