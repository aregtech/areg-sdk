// chatter/PageMessaging.cpp : implementation file
//

#include "chatter/res/stdafx.h"
#include "chatter/ui/PageMessaging.hpp"
#include "chatter/services/CentralMessaging.hpp"
#include "chatter/DistrbutedApp.hpp"
#include "chatter/NEDistributedApp.hpp"
#include "generated/NEConnectionManager.hpp"
#include "areg/base/GEGlobal.h"
#include "chatter/services/ConnectionHandler.hpp"

// PageMessaging dialog

IMPLEMENT_DYNAMIC(PageMessaging, CPropertyPage)
LPCTSTR PageMessaging::HEADER_TITILES[] =
{
      _T( "Nick Name:" )
    , _T( "Message:" )
    , _T( "Sent at:" )
    , _T( "Received at:" )
};


PageMessaging::PageMessaging( ConnectionHandler & handlerConnection )
	: CPropertyPage     (PageMessaging::IDD)
    , mIsBroadcast      ( TRUE )
    , mIsKeytype        ( TRUE )
    , mIsMessages       ( TRUE )
    , mCtrlList         ( )
    , mCentralMessage   ( nullptr )
    , mLastItem         ( 0 )
    , mTextMsg          ( _T( "" ) )
    , mConnectionHandler( handlerConnection )
    , mSendEnabled      ( FALSE )
{

}

PageMessaging::~PageMessaging()
{
    cleanService( );
}

void PageMessaging::OnServiceStartup( bool isStarted, Component * owner )
{
    // do nothing
}

void PageMessaging::OnServiceNetwork( bool isConnected, DispatcherThread * ownerThread )
{
    // do nothing
}

void PageMessaging::OnServiceConnection( bool isConnected, DispatcherThread * ownerThread )
{
    // do nothing
}

void PageMessaging::OnClientConnection( bool isConnected, DispatcherThread *dispThread )
{
    // do nothing
}

void PageMessaging::OnClientRegistration( bool isRegistered, DispatcherThread * dispThread )
{
    if ( isRegistered )
    {
        if ( (mCentralMessage == nullptr) && (dispThread != nullptr) )
        {
            mCentralMessage = DEBUG_NEW CentralMessaging( NECommon::COMP_NAME_CENTRAL_SERVER, *dispThread, mConnectionHandler);
            if ( mCentralMessage != nullptr )
            {
                UpdateData( TRUE );
                mCentralMessage->ReceiveBroadcasting(mIsBroadcast ? true : false);
                mCentralMessage->ReceiveKeytype(mIsKeytype ? true : false);
                mCentralMessage->ReceiveMessages(mIsMessages ? true : false);
            }
        }
        outputMessage( mConnectionHandler.GetNickName(), "Is registered", mConnectionHandler.GetTimeConnect(), mConnectionHandler.GetTimeConnected(), mConnectionHandler.GetCookie());

        uint32_t cookie = mConnectionHandler.GetCookie();
        const String & nickName = mConnectionHandler.GetNickName();
        const NECommon::ListConnections & listConnections = mConnectionHandler.GetConnectionList();
        if ( listConnections.getSize() > 0 )
        {
            outputMessage( "<Info>", String::toString(listConnections.getSize()) + " participants...", 0, 0, 0 );
        }

        for (uint32_t i = 0; i < listConnections.getSize(); ++ i )
        {
            const NECommon::sConnection & connection = listConnections.getAt(i);
            if ( (connection.cookie != cookie) && (connection.nickName != nickName) )
            {
                ASSERT(connection.nickName.isEmpty() == false);
                outputMessage( connection.nickName, "Is registered in system", connection.connectTime, connection.connectedTime, connection.cookie );
            }
        }
    }
    else
    {
        outputMessage( mConnectionHandler.GetNickName( ), "Is unregistered", mConnectionHandler.GetTimeConnect( ), mConnectionHandler.GetTimeConnected( ), mConnectionHandler.GetCookie( ) );
    }
}

void PageMessaging::OnAddConnection( NEConnectionManager::sConnection & data )
{
    if ( (mConnectionHandler.GetCookie() != data.cookie) && (mConnectionHandler.GetNickName() != data.nickName) )
        outputMessage( data.nickName, "Is connected and registered", data.connectTime, data.connectedTime, data.cookie );
}

void PageMessaging::OnRemoveConnection( NEConnectionManager::sConnection & data )
{
    outputMessage( data.nickName, "Is unregistered and disconnected", data.connectTime, data.connectedTime, data.cookie );
}

void PageMessaging::OnUpdateConnection( void )
{
    // do nothing
}

void PageMessaging::OnDisconnectTriggered( void )
{
    outputMessage( mConnectionHandler.GetNickName( ), "Disconnects system", mConnectionHandler.GetTimeConnect( ), mConnectionHandler.GetTimeConnected( ), mConnectionHandler.GetCookie( ) );
}

void PageMessaging::setHeaders( void )
{
    int count = MACRO_ARRAYLEN( PageMessaging::HEADER_TITILES );
    CRect rc( 0, 0, 0, 0 );
    mCtrlList.GetClientRect( &rc );
    int width1, width2;
    NECommon::getWidths( rc.Width(), count, width1, width2 );

    for ( int i = 0; i < count; ++ i )
    {
        CString str( HEADER_TITILES[i] );
        LVCOLUMN lv;
        NEMemory::zeroElement<LVCOLUMN>( lv );
        lv.mask         = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
        lv.fmt          = LVCFMT_LEFT;
        lv.cx           = i == 0 ? width1 : width2;
        lv.pszText      = str.GetBuffer( );
        lv.cchTextMax   = str.GetLength( );

        mCtrlList.InsertColumn( i, &lv );
    }
}

bool PageMessaging::isServiceConnected(void) const
{
    return ( (mCentralMessage                           != nullptr ) && 
             (mCentralMessage->isConnected()            == true ) );
}

void PageMessaging::cleanService(void)
{
    if (mCentralMessage != nullptr)
    {
        mCentralMessage->clearAllNotifications();
        delete mCentralMessage;
        mCentralMessage = nullptr;
    }
}

void PageMessaging::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange( pDX );
    DDX_Check( pDX, IDC_CHECK_BROADCAST, mIsBroadcast );
    DDX_Check( pDX, IDC_CHECK_KEYTYPE, mIsKeytype );
    DDX_Check( pDX, IDC_CHECK_MESSAGES, mIsMessages );
    DDX_Control( pDX, IDC_LIST_MESSAGES, mCtrlList );
    DDX_Text( pDX, IDC_EDIT_MESSAGE_ALL, mTextMsg );
	DDV_MaxChars(pDX, mTextMsg, 511);
}


BEGIN_MESSAGE_MAP(PageMessaging, CPropertyPage)
    ON_WM_DESTROY( )
    ON_BN_CLICKED( IDC_BUTTON_SEND, &PageMessaging::OnClickedButtonSend )
    ON_BN_CLICKED( IDC_CHECK_MESSAGES, &PageMessaging::OnClickedCheckMessages )
    ON_BN_CLICKED( IDC_CHECK_KEYTYPE, &PageMessaging::OnClickedCheckKeytype )
    ON_BN_CLICKED( IDC_CHECK_BROADCAST, &PageMessaging::OnClickedCheckBroadcast )
    ON_EN_CHANGE( IDC_EDIT_MESSAGE_ALL, &PageMessaging::OnChangeEditMessageAll )
    ON_MESSAGE_VOID( WM_KICKIDLE, OnKickIdle )
    ON_UPDATE_COMMAND_UI( IDC_BUTTON_SEND       , &PageMessaging::OnButtonUpdateSend )
    ON_UPDATE_COMMAND_UI( IDC_CHECK_MESSAGES    , &PageMessaging::OnCheckUpdate )
    ON_UPDATE_COMMAND_UI( IDC_CHECK_KEYTYPE     , &PageMessaging::OnCheckUpdate )
    ON_UPDATE_COMMAND_UI( IDC_CHECK_BROADCAST   , &PageMessaging::OnCheckUpdate)
END_MESSAGE_MAP( )


// PageMessaging message handlers

BOOL PageMessaging::OnInitDialog( )
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
                  // EXPTION: OCX Property Pages should return FALSE
}

void PageMessaging::OnClickedButtonSend( )
{
    UpdateData( TRUE );
    if ( mCentralMessage != nullptr)
    {
        DateTime dateTime = DateTime::getNow();
        outputMessage( CString( mConnectionHandler.GetNickName().getString() )
                     , mTextMsg
                     , CString( dateTime.formatTime().getString())
                     , CString( "..." )
                     , mConnectionHandler.GetCookie());

        mCentralMessage->requestSendMessage( mConnectionHandler.GetNickName(), mConnectionHandler.GetCookie(), String( mTextMsg.GetString( ) ), dateTime );
        mTextMsg = _T("");
        UpdateData(FALSE);
        GetDlgItem( IDC_EDIT_MESSAGE_ALL )->SetFocus( );
    }
}

void PageMessaging::OnClickedCheckMessages( )
{
    UpdateData(TRUE);
    if ( mCentralMessage )
        mCentralMessage->ReceiveMessages( mIsMessages ? true : false );
}

void PageMessaging::OnClickedCheckKeytype( )
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

void PageMessaging::OnClickedCheckBroadcast( )
{
    UpdateData( TRUE );
    if ( mCentralMessage )
        mCentralMessage->ReceiveBroadcasting( mIsBroadcast ? true : false );
}

void PageMessaging::OnKickIdle( )
{
    UpdateDialogControls( this, FALSE );
}

void PageMessaging::OnButtonUpdateSend( CCmdUI* pCmdUI )
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

void PageMessaging::OnCheckUpdate( CCmdUI* pCmdUI )
{
    pCmdUI->Enable( isServiceConnected() ? TRUE : FALSE );
}

void PageMessaging::OnChangeEditMessageAll( )
{
    if ( mCentralMessage )
    {
        CString oldTxt = mTextMsg;
        UpdateData( TRUE );
        if ( oldTxt != mTextMsg )
        {
            mCentralMessage->requestKeyTyping( mConnectionHandler.GetNickName(), mConnectionHandler.GetCookie( ), String( mTextMsg.GetString( ) ) );
        }
    }
}

void PageMessaging::OnTypeMessage( uint32_t cookie, NECommon::sMessageData & data )
{
    outputTyping( CString( data.nickName ), CString( data.message ), static_cast<uint32_t>(data.dataSave) );
}

void PageMessaging::OnSendMessage( uint32_t cookie, NECommon::sMessageData & data )
{
        outputMessage( data.nickName, data.message, data.timeSend, data.timeReceived, static_cast<uint32_t>(data.dataSave) );
}

LRESULT PageMessaging::OnOutputMessage( WPARAM wParam, LPARAM lParam )
{
    NECommon::sMessageData * data = reinterpret_cast<NECommon::sMessageData *>(lParam);
    if ( data != nullptr)
    {
        outputMessage( data->nickName, data->message, data->timeSend, data->timeReceived, static_cast<uint32_t>(data->dataSave) );
        delete data;
    }
    return 0;
}

void PageMessaging::outputMessage( CString nickName, CString message, CString dateStart, CString dateEnd, uint32_t cookie )
{
    removeTyping(nickName, cookie);

    LVITEM lv;
    NEMemory::zeroElement<LVITEM>( lv );

    // Column nickname
    lv.mask     = LVIF_TEXT | LVIF_PARAM;
    lv.iItem    = mLastItem;
    lv.iSubItem = 0;
    lv.pszText  = nickName.GetBuffer( );
    lv.lParam   = cookie;
    lv.cchTextMax = NECommon::MAXLEN_NICKNAME;
    mCtrlList.InsertItem( &lv );

    if ( dateStart.GetLength( ) > NECommon::DAY_FORMAT_LEN )
        dateStart = dateStart.Mid( NECommon::DAY_FORMAT_LEN );
    if ( dateEnd.GetLength( ) > NECommon::DAY_FORMAT_LEN )
        dateEnd = dateEnd.Mid( NECommon::DAY_FORMAT_LEN );

    mCtrlList.SetItemText( mLastItem, 1, message.IsEmpty( )     == false ? message.GetBuffer( )     : _T( "..." ) );
    mCtrlList.SetItemText( mLastItem, 2, dateStart.IsEmpty( )   == false ? dateStart.GetBuffer( )   : _T( "..." ) );
    mCtrlList.SetItemText( mLastItem, 3, dateEnd.IsEmpty( )     == false ? dateEnd.GetBuffer( )     : _T( "..." ) );

    mCtrlList.EnsureVisible( mLastItem, FALSE );

    ++ mLastItem;
}

void PageMessaging::outputMessage( const String & nickname, const String & message, const uint64_t begin, const uint64_t end, uint32_t cookie )
{
    outputMessage(nickname, message, begin != 0 ? DateTime(begin) : DateTime(), end != 0 ? DateTime(end) : DateTime(), cookie );
}

void PageMessaging::outputMessage( const String & nickname, const String & message, const DateTime & begin, const DateTime & end, uint32_t cookie )
{
    outputMessage( CString(nickname.getString())
                 , CString(message.getString())
                 , CString( begin.isValid() ? begin.formatTime().getString() : String::getEmptyString().getString() )
                 , CString( end.isValid()   ? end.formatTime().getString()   : String::getEmptyString().getString() )
                 , cookie );
}

void PageMessaging::outputTyping( CString nickName, CString message, uint32_t cookie )
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
            NEMemory::zeroElement<LVITEM>( lv );

            // Column nickname
            lv.mask     = LVIF_TEXT | LVIF_PARAM;
            lv.iItem    = pos;
            lv.iSubItem = 0;
            lv.pszText  = nickName.GetBuffer( );
            lv.lParam   = cookie;
            lv.cchTextMax = NECommon::MAXLEN_NICKNAME;
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

void PageMessaging::removeTyping( const CString & nickName, uint32_t cookie )
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

void PageMessaging::OnDestroy( )
{
    cleanService( );
    CPropertyPage::OnDestroy( );
}

void PageMessaging::OnDefaultClicked( void )
{
    UpdateData(TRUE);
    if ( (mSendEnabled == TRUE) && (mTextMsg.IsEmpty() == FALSE) )
    {
        CButton * btnSend = reinterpret_cast<CButton *>(GetDlgItem( IDC_BUTTON_SEND ));
        if ( btnSend != nullptr )
        {
            PostMessage( WM_COMMAND, MAKEWPARAM( IDC_BUTTON_SEND, BN_CLICKED ), reinterpret_cast<LPARAM>(btnSend->GetSafeHwnd( )) );
        }
    }
}
