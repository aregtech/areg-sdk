// edge/CEPageMessaging.cpp : implementation file
//

#include "edge/res/stdafx.h"
#include "edge/ui/CEPageMessaging.hpp"
#include "edge/services/CECentralMessaging.hpp"
#include "edge/CEDistrbutedApp.hpp"
#include "edge/NEDistributedApp.hpp"
#include "shared/generated/NEConnectionManager.hpp"
#include "areg/base/GEGlobal.h"
#include "edge/services/CEConnectionHandler.hpp"

// CEPageMessaging dialog

IMPLEMENT_DYNAMIC(CEPageMessaging, CPropertyPage)
LPCTSTR CEPageMessaging::HEADER_TITILES[] =
{
      _T( "Nick Name:" )
    , _T( "Message:" )
    , _T( "Sent at:" )
    , _T( "Received at:" )
};


CEPageMessaging::CEPageMessaging( CEConnectionHandler & handlerConnection )
	: CPropertyPage     (CEPageMessaging::IDD)
    , mIsBroadcast      ( TRUE )
    , mIsKeytype        ( TRUE )
    , mIsMessages       ( TRUE )
    , mCtrlList         ( )
    , mCentralMessage   ( static_cast<CECentralMessaging *>(NULL) )
    , mLastItem         ( 0 )
    , mTextMsg          ( _T( "" ) )
    , mConnectionHandler( handlerConnection )
    , mSendEnabled      ( FALSE )
{

}

CEPageMessaging::~CEPageMessaging()
{
    cleanService( );
}

void CEPageMessaging::OnServiceStartup( bool isStarted, CEComponent * owner )
{
    // do nothing
}

void CEPageMessaging::OnServiceNetwork( bool isConnected, CEDispatcherThread * ownerThread )
{
    // do nothing
}

void CEPageMessaging::OnServiceConnection( bool isConnected, CEDispatcherThread * ownerThread )
{
    // do nothing
}

void CEPageMessaging::OnClientConnection( bool isConnected, CEDispatcherThread *dispThread )
{
    // do nothing
}

void CEPageMessaging::OnClientRegistration( bool isRegistered, CEDispatcherThread * dispThread )
{
    if ( isRegistered )
    {
        if ( (mCentralMessage == NULL) && (dispThread != NULL) )
        {
            mCentralMessage = DEBUG_NEW CECentralMessaging( NECommonSettings::COMP_NAME_CENTRAL_SERVER, *dispThread, mConnectionHandler);
            if ( mCentralMessage != NULL )
            {
                UpdateData( TRUE );
                mCentralMessage->ReceiveBroadcasting(mIsBroadcast ? true : false);
                mCentralMessage->ReceiveKeytype(mIsKeytype ? true : false);
                mCentralMessage->ReceiveMessages(mIsMessages ? true : false);
            }
        }
        outputMessage( mConnectionHandler.GetNickName(), "Is registered", mConnectionHandler.GetTimeConnect(), mConnectionHandler.GetTimeConnected(), mConnectionHandler.GetCookie());

        uint32_t cookie = mConnectionHandler.GetCookie();
        const CEString & nickName = mConnectionHandler.GetNickName();
        const NECommonSettings::ListConnections & listConnections = mConnectionHandler.GetConnectionList();
        if ( listConnections.GetSize() > 0 )
        {
            outputMessage( "<Info>", CEString::Int32ToString(listConnections.GetSize()) + " participants...", 0, 0, 0 );
        }
        for ( int i = 0; i < listConnections.GetSize(); ++ i )
        {
            const NECommonSettings::sConnection & connection = listConnections.GetAt(i);
            if ( (connection.cookie != cookie) && (connection.nickName != nickName) )
            {
                ASSERT(connection.nickName.IsEmpty() == false);
                outputMessage( connection.nickName, "Is registered in system", connection.connectTime, connection.connectedTime, connection.cookie );
            }
        }
    }
    else
    {
        outputMessage( mConnectionHandler.GetNickName( ), "Is unregistered", mConnectionHandler.GetTimeConnect( ), mConnectionHandler.GetTimeConnected( ), mConnectionHandler.GetCookie( ) );
    }
}

void CEPageMessaging::OnAddConnection( NEConnectionManager::sConnection & data )
{
    if ( (mConnectionHandler.GetCookie() != data.cookie) && (mConnectionHandler.GetNickName() != data.nickName) )
        outputMessage( data.nickName, "Is connected and registered", data.connectTime, data.connectedTime, data.cookie );
}

void CEPageMessaging::OnRemoveConnection( NEConnectionManager::sConnection & data )
{
    outputMessage( data.nickName, "Is unregistered and disconnected", data.connectTime, data.connectedTime, data.cookie );
}

void CEPageMessaging::OnUpdateConnection( void )
{
    // do nothing
}

void CEPageMessaging::OnDisconnectTriggered( void )
{
    outputMessage( mConnectionHandler.GetNickName( ), "Disconnects system", mConnectionHandler.GetTimeConnect( ), mConnectionHandler.GetTimeConnected( ), mConnectionHandler.GetCookie( ) );
}

void CEPageMessaging::setHeaders( void )
{
    int count = MACRO_ARRAYLEN( CEPageMessaging::HEADER_TITILES );
    CRect rc( 0, 0, 0, 0 );
    mCtrlList.GetClientRect( &rc );
    int width1, width2;
    NECommonSettings::getWidths( rc.Width(), count, width1, width2 );

    for ( int i = 0; i < count; ++ i )
    {
        CString str( HEADER_TITILES[i] );
        LVCOLUMN lv;
        NEMemory::ZeroData<LVCOLUMN>( lv );
        lv.mask         = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
        lv.fmt          = LVCFMT_LEFT;
        lv.cx           = i == 0 ? width1 : width2;
        lv.pszText      = str.GetBuffer( );
        lv.cchTextMax   = str.GetLength( );

        mCtrlList.InsertColumn( i, &lv );
    }
}

bool CEPageMessaging::isServiceConnected(void) const
{
    return ( (mCentralMessage                           != NULL ) && 
             (mCentralMessage->IsConnected()            == true ) );
}

void CEPageMessaging::cleanService(void)
{
    if (mCentralMessage != NULL)
    {
        mCentralMessage->ClearAllNotifications();
        delete mCentralMessage;
        mCentralMessage = NULL;
    }
}

void CEPageMessaging::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange( pDX );
    DDX_Check( pDX, IDC_CHECK_BROADCAST, mIsBroadcast );
    DDX_Check( pDX, IDC_CHECK_KEYTYPE, mIsKeytype );
    DDX_Check( pDX, IDC_CHECK_MESSAGES, mIsMessages );
    DDX_Control( pDX, IDC_LIST_MESSAGES, mCtrlList );
    DDX_Text( pDX, IDC_EDIT_MESSAGE_ALL, mTextMsg );
	DDV_MaxChars(pDX, mTextMsg, 511);
}


BEGIN_MESSAGE_MAP(CEPageMessaging, CPropertyPage)
    ON_WM_DESTROY( )
    ON_BN_CLICKED( IDC_BUTTON_SEND, &CEPageMessaging::OnClickedButtonSend )
    ON_BN_CLICKED( IDC_CHECK_MESSAGES, &CEPageMessaging::OnClickedCheckMessages )
    ON_BN_CLICKED( IDC_CHECK_KEYTYPE, &CEPageMessaging::OnClickedCheckKeytype )
    ON_BN_CLICKED( IDC_CHECK_BROADCAST, &CEPageMessaging::OnClickedCheckBroadcast )
    ON_EN_CHANGE( IDC_EDIT_MESSAGE_ALL, &CEPageMessaging::OnChangeEditMessageAll )
    ON_MESSAGE_VOID( WM_KICKIDLE, OnKickIdle )
    ON_UPDATE_COMMAND_UI( IDC_BUTTON_SEND       , &CEPageMessaging::OnButtonUpdateSend )
    ON_UPDATE_COMMAND_UI( IDC_CHECK_MESSAGES    , &CEPageMessaging::OnCheckUpdate )
    ON_UPDATE_COMMAND_UI( IDC_CHECK_KEYTYPE     , &CEPageMessaging::OnCheckUpdate )
    ON_UPDATE_COMMAND_UI( IDC_CHECK_BROADCAST   , &CEPageMessaging::OnCheckUpdate)
END_MESSAGE_MAP( )


// CEPageMessaging message handlers

BOOL CEPageMessaging::OnInitDialog( )
{
    CPropertyPage::OnInitDialog( );

    setHeaders( );

    if ( isServiceConnected() )
    {
        mCentralMessage->ReceiveMessages( mIsMessages ? true : false );
        mCentralMessage->ReceiveKeytype( mIsKeytype ? true : false );
        mCentralMessage->ReceiveBroadcasting( mIsBroadcast ? true : false );
    }

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

void CEPageMessaging::OnClickedButtonSend( )
{
    UpdateData( TRUE );
    if ( mCentralMessage != NULL)
    {
        CEDateTime dateTime = CEDateTime::GetNow();
        outputMessage( CString( mConnectionHandler.GetNickName().GetBuffer() )
                     , mTextMsg
                     , CString( dateTime.FormatTime().String())
                     , CString( "..." )
                     , mConnectionHandler.GetCookie());

        mCentralMessage->RequestSendMessage( mConnectionHandler.GetNickName(), mConnectionHandler.GetCookie(), CEString( mTextMsg.GetString( ) ), dateTime );
        mTextMsg = _T("");
        UpdateData(FALSE);
        GetDlgItem( IDC_EDIT_MESSAGE_ALL )->SetFocus( );
    }
}

void CEPageMessaging::OnClickedCheckMessages( )
{
    UpdateData(TRUE);
    if ( mCentralMessage )
        mCentralMessage->ReceiveMessages( mIsMessages ? true : false );
}

void CEPageMessaging::OnClickedCheckKeytype( )
{
    UpdateData( TRUE );
    if ( mCentralMessage )
    {
        mCentralMessage->ReceiveKeytype( mIsKeytype ? true : false );
    }
    if ( mIsKeytype == FALSE )
    {
        for ( int i = mLastItem; i < mCtrlList.GetItemCount( ); )
            mCtrlList.DeleteItem( i );
    }
}

void CEPageMessaging::OnClickedCheckBroadcast( )
{
    UpdateData( TRUE );
    if ( mCentralMessage )
        mCentralMessage->ReceiveBroadcasting( mIsBroadcast ? true : false );
}

void CEPageMessaging::OnKickIdle( )
{
    UpdateDialogControls( this, FALSE );
}

void CEPageMessaging::OnButtonUpdateSend( CCmdUI* pCmdUI )
{
    if ( isServiceConnected() )
    {
        CString oldTxt = mTextMsg;
        UpdateData( TRUE );
        if ( mTextMsg.IsEmpty() == FALSE )
        {
            pCmdUI->Enable( TRUE );
            if ( mSendEnabled == FALSE )
            {
                CEdit * edit = reinterpret_cast<CEdit *>(GetDlgItem( IDC_EDIT_MESSAGE_ALL ));
                edit->SetFocus( );
                edit->SetSel( 0, mTextMsg.GetLength( ), FALSE );
            }
        }
        else
        {
            pCmdUI->Enable( FALSE );
        }
        mTextMsg = oldTxt;
        mSendEnabled = TRUE;
    }
    else
    {
        pCmdUI->Enable(FALSE);
        mSendEnabled = FALSE;
    }
}

void CEPageMessaging::OnCheckUpdate( CCmdUI* pCmdUI )
{
    pCmdUI->Enable( isServiceConnected() ? TRUE : FALSE );
}

void CEPageMessaging::OnChangeEditMessageAll( )
{
    if ( mCentralMessage )
    {
        CString oldTxt = mTextMsg;
        UpdateData( TRUE );
        if ( oldTxt != mTextMsg )
        {
            mCentralMessage->RequestKeyTyping( mConnectionHandler.GetNickName(), mConnectionHandler.GetCookie( ), CEString( mTextMsg.GetString( ) ) );
        }
    }
}

void CEPageMessaging::OnTypeMessage( uint32_t cookie, NECommonSettings::sMessageData & data )
{
    outputTyping( CString( data.nickName ), CString( data.message ), static_cast<uint32_t>(data.dataSave) );
}

void CEPageMessaging::OnSendMessage( uint32_t cookie, NECommonSettings::sMessageData & data )
{
        outputMessage( data.nickName, data.message, data.timeSend, data.timeReceived, static_cast<uint32_t>(data.dataSave) );
}

LRESULT CEPageMessaging::OnOutputMessage( WPARAM wParam, LPARAM lParam )
{
    NECommonSettings::sMessageData * data = reinterpret_cast<NECommonSettings::sMessageData *>(lParam);
    if ( data != NULL)
    {
        outputMessage( data->nickName, data->message, data->timeSend, data->timeReceived, static_cast<uint32_t>(data->dataSave) );
        delete data;
    }
    return 0;
}

void CEPageMessaging::outputMessage( CString & nickName, CString & message, CString & dateStart, CString & dateEnd, uint32_t cookie )
{
    removeTyping(nickName, cookie);

    LVITEM lv;
    NEMemory::ZeroData<LVITEM>( lv );

    // Column nickname
    lv.mask     = LVIF_TEXT | LVIF_PARAM;
    lv.iItem    = mLastItem;
    lv.iSubItem = 0;
    lv.pszText  = nickName.GetBuffer( );
    lv.lParam   = cookie;
    lv.cchTextMax = NECommonSettings::MAXLEN_NICKNAME;
    mCtrlList.InsertItem( &lv );

    if ( dateStart.GetLength( ) > NECommonSettings::DAY_FORMAT_LEN )
        dateStart = dateStart.Mid( NECommonSettings::DAY_FORMAT_LEN );
    if ( dateEnd.GetLength( ) > NECommonSettings::DAY_FORMAT_LEN )
        dateEnd = dateEnd.Mid( NECommonSettings::DAY_FORMAT_LEN );

    mCtrlList.SetItemText( mLastItem, 1, message.IsEmpty( )     == false ? message.GetBuffer( )     : _T( "..." ) );
    mCtrlList.SetItemText( mLastItem, 2, dateStart.IsEmpty( )   == false ? dateStart.GetBuffer( )   : _T( "..." ) );
    mCtrlList.SetItemText( mLastItem, 3, dateEnd.IsEmpty( )     == false ? dateEnd.GetBuffer( )     : _T( "..." ) );

    mCtrlList.EnsureVisible( mLastItem, FALSE );

    ++ mLastItem;
}

void CEPageMessaging::outputMessage( const CEString & nickname, const CEString & message, const uint64_t begin, const uint64_t end, uint32_t cookie )
{
    outputMessage(nickname, message, begin != 0 ? CEDateTime(begin) : CEDateTime(), end != 0 ? CEDateTime(end) : CEDateTime(), cookie );
}

void CEPageMessaging::outputMessage( const CEString & nickname, const CEString & message, const CEDateTime & begin, const CEDateTime & end, uint32_t cookie )
{
    outputMessage( CString(nickname.String())
                 , CString(message.String())
                 , CString( begin.IsValid() ? begin.FormatTime().String() : CEString::EmptyString )
                 , CString( end.IsValid()   ? end.FormatTime().String()   : CEString::EmptyString )
                 , cookie );
}

void CEPageMessaging::outputTyping( CString & nickName, CString & message, uint32_t cookie )
{
    if ( message.IsEmpty() == false )
    {
        int pos = mLastItem;
        for ( pos; pos < mCtrlList.GetItemCount(); ++ pos )
        {
            if ( cookie == static_cast<uint32_t>(mCtrlList.GetItemData(pos)) )
                break;
        }
        if ( pos == mCtrlList.GetItemCount() )
        {
            LVITEM lv;
            NEMemory::ZeroData<LVITEM>( lv );

            // Column nickname
            lv.mask     = LVIF_TEXT | LVIF_PARAM;
            lv.iItem    = pos;
            lv.iSubItem = 0;
            lv.pszText  = nickName.GetBuffer( );
            lv.lParam   = cookie;
            lv.cchTextMax = NECommonSettings::MAXLEN_NICKNAME;
            mCtrlList.InsertItem( &lv );
        }

        mCtrlList.SetItemText( mLastItem, 1, message.IsEmpty( ) == false ? message.GetBuffer( ) : _T( "..." ) );
        mCtrlList.SetItemText( mLastItem, 2, _T( "typing..." ) );
        mCtrlList.SetItemText( mLastItem, 3, _T( "..." ) );

        mCtrlList.EnsureVisible( mCtrlList.GetItemCount() - 1, FALSE );
    }
    else
    {
        removeTyping(nickName, cookie);
    }
}

void CEPageMessaging::removeTyping( const CString & nickName, uint32_t cookie )
{
    for ( int i = mLastItem; i < mCtrlList.GetItemCount(); ++ i)
    {
        if ( cookie == static_cast<uint32_t>(mCtrlList.GetItemData(i)) )
        {
            ASSERT( nickName == mCtrlList.GetItemText(i, 0) );
            mCtrlList.DeleteItem(i);
            mCtrlList.EnsureVisible( mCtrlList.GetItemCount( ) - 1, FALSE );
            break;
        }
    }
}

void CEPageMessaging::OnDestroy( )
{
    cleanService( );
    CPropertyPage::OnDestroy( );
}

void CEPageMessaging::OnDefaultClicked( void )
{
    UpdateData(TRUE);
    if ( (mSendEnabled == TRUE) && (mTextMsg.IsEmpty() == FALSE) )
    {
        CButton * btnSend = reinterpret_cast<CButton *>(GetDlgItem( IDC_BUTTON_SEND ));
        if ( btnSend != NULL )
        {
            PostMessage( WM_COMMAND, MAKEWPARAM( IDC_BUTTON_SEND, BN_CLICKED ), reinterpret_cast<LPARAM>(btnSend->GetSafeHwnd( )) );
        }
    }
}
