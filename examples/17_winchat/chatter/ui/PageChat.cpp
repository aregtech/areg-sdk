// distrbutedapp\ui\PageChat.cpp : implementation file
//

#include "chatter/res/stdafx.h"
#include "chatter/ui/PageChat.hpp"
#include "chatter/ui/DistributedDialog.hpp"
#include "chatter/services/DirectChatService.hpp"
#include "chatter/services/ChatParticipantService.hpp"
#include "chatter/NEDistributedApp.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/base/DateTime.hpp"
#include "chatter/services/DirectMessagingClient.hpp"

#define FIRST_MESSAGE       (WM_USER + 10 + static_cast<unsigned int>(NEDistributedApp::eWndCommands::CmdFirst))
#define MAKE_MESSAGE(elem)  (static_cast<unsigned int>(elem) + FIRST_MESSAGE)

LPCTSTR PageChat::HEADER_TITILES[] =
{
      _T( "Nick Name:" )
    , _T( "Message:" )
    , _T( "Sent at:" )
    , _T( "Received at:" )
};

// PageChat dialog

IMPLEMENT_DYNAMIC(PageChat, CPropertyPage)

PageChat::PageChat( const String & serviceName
                      , const NEDirectConnection::sInitiator & initiator
                      , const NEDirectConnection::ListParticipants & listParties
                      , const NEDirectConnection::sParticipant & ownerConnection
                      , bool isInitiator )
	: CPropertyPage             (PageChat::IDD)
    , ChatPrticipantHandler   ( serviceName, initiator, listParties, ownerConnection )

    , mCtrlChat                 ( )
    , mChatMsg                  ( _T( "" ) )
    , mCtrlList                 ( )
    , mIsChatMessage            ( TRUE )
    , mIsChatTyping             ( TRUE )
    , mIsChatInitiator          ( isInitiator )
    , mLastItem                 ( 0 )
    , mTitle                    ( _T("") )
    , mEditEnabled              ( FALSE )
    , mDoAutotype(FALSE)
    , mTimerValue               (TIMER_MIN_VALUE)
    , mCtrlTimerValue           ( )
    , mCtrlTimerSpin            ( )
    , mTimerId                  ( 0 )
{

}

PageChat::~PageChat()
{
}

void PageChat::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_CHAT, mCtrlChat);
    DDX_Text(pDX, IDC_EDIT_CHAT, mChatMsg);
    DDV_MaxChars(pDX, mChatMsg, 511);
    DDX_Control(pDX, IDC_LIST_CHAT, mCtrlList);
    DDX_Check(pDX, IDC_CHECK_CHAT_MESSAGES, mIsChatMessage);
    DDX_Check(pDX, IDC_CHECK_CHAT_TYPING, mIsChatTyping);
    DDX_Text(pDX, IDC_CHAT_TIMER, mTimerValue);
    DDV_MinMaxUInt(pDX, mTimerValue, TIMER_MIN_VALUE, TIMER_MAX_VALUE);
    DDX_Check(pDX, IDC_CHECK_AUTO, mDoAutotype);
    DDX_Control(pDX, IDC_CHAT_TIMER, mCtrlTimerValue);
    DDX_Control(pDX, IDC_CHAT_TIMER_SPIN, mCtrlTimerSpin);
}


BEGIN_MESSAGE_MAP(PageChat, CPropertyPage)
    ON_WM_DESTROY( )
    ON_WM_TIMER()
    ON_MESSAGE_VOID( WM_KICKIDLE, OnKickIdle )
    ON_EN_UPDATE(IDC_EDIT_CHAT, &PageChat::OnUpdateEditChat)
    ON_EN_CHANGE(IDC_CHAT_TIMER, &PageChat::OnEnChangeChatTimer)
    ON_BN_CLICKED( IDC_CHECK_CHAT_MESSAGES, &PageChat::OnClickedCheckChatMessages )
    ON_BN_CLICKED( IDC_CHECK_CHAT_TYPING, &PageChat::OnClickedCheckChatTyping )
    ON_BN_CLICKED( IDC_BUTTON_CHAT_SEND, &PageChat::OnClickedButtonChatSend )
    ON_BN_CLICKED( IDC_BUTTON_CLOSE_CHAT, &PageChat::OnClickedButtonCloseChat )
    ON_BN_CLICKED( IDC_CHECK_AUTO, &PageChat::OnBnClickedCheckAuto)
    ON_UPDATE_COMMAND_UI( IDC_BUTTON_CHAT_SEND, &PageChat::OnButtonUpdateChatSend )
    ON_UPDATE_COMMAND_UI( IDC_EDIT_CHAT, &PageChat::OnEditUpdateChat )
    ON_NOTIFY(UDN_DELTAPOS, IDC_CHAT_TIMER_SPIN, &PageChat::OnDeltaposChatTimerSpin)
    ON_MESSAGE( MAKE_MESSAGE(NEDistributedApp::eWndCommands::CmdChatJoined  ), &PageChat::OnCmdChatJoined)
    ON_MESSAGE( MAKE_MESSAGE(NEDistributedApp::eWndCommands::CmdChatMessage ), &PageChat::OnCmdChatMessage)
    ON_MESSAGE( MAKE_MESSAGE(NEDistributedApp::eWndCommands::CmdChatTyping  ), &PageChat::OnCmdChatTyping)
END_MESSAGE_MAP()


// PageChat message handlers


BOOL PageChat::OnInitDialog( )
{
    CPropertyPage::OnInitDialog( );

    mCtrlTimerSpin.SetBase(10);
    mCtrlTimerSpin.SetPos(mTimerValue);
    mCtrlTimerSpin.SetBuddy(&mCtrlTimerValue);
    mCtrlTimerSpin.SetRange(TIMER_MIN_VALUE, TIMER_MAX_VALUE);

    // mCtrlTimerValue.EnableWindow(mDoAutotype);
    // mCtrlTimerSpin.EnableWindow(mDoAutotype);

    SetChatWindow( reinterpret_cast<ptr_type>(GetSafeHwnd()) );
    setHeaders( );
    srand(static_cast<unsigned int>(time(nullptr)));

    const NEDirectConnection::sInitiator & initiator    = GetInitiator();
    const NEDirectConnection::ListParticipants & parties= GetParticipantList();
    const NEDirectConnection::sParticipant & owner      = GetConnectionOwner( );
    String message("");
    String comma("");

    for ( uint32_t i = 0; i < parties.getSize(); ++ i )
    {
        const NEDirectConnection::sParticipant & participant = parties[i];
        if ( owner != participant )
        {
            message += comma;
            message += participant.nickName;
            comma = ", ";
        }
    }

    String caption = "[ " + owner.nickName + " ]: " + message;
    mTitle = CString( caption.getString() );
    setTabTitle(caption);
    message = "Parties: " + message;

    NEMemory::uAlign data;
    data.alignClsPtr.mElement = reinterpret_cast<NEMemory::_EmptyClass *>(this);

    NERegistry::Model model = mIsChatInitiator ? DirectChatService::GetModel( initiator, parties, data ) : ChatParticipantService::GetModel(initiator, parties, data);
    if ( ComponentLoader::isModelLoaded(model.getModelName()) == false )
    {
        ComponentLoader::addModelUnique( model );
        ComponentLoader::loadComponentModel( model.getModelName( ) );    
    }
    else
    {
        String nickName = mIsChatInitiator ? "[ " + initiator.nickName + " ]" : initiator.nickName;
        outputMessage( CString( nickName.getString( ) ), CString( "Is already registered..." ), CString( DateTime::getNow().formatTime( ).getString( ) ), CString( ), 0 );
    }

    return TRUE;
}


void PageChat::OnClickedCheckChatMessages( )
{
    DirectMessagingClient * client = this->GetChatClient( );
    if ( client != nullptr )
    {
        UpdateData( TRUE );
        client->notifyOnBroadcastMessageSent( mIsChatMessage ? true : false );
    }
}


void PageChat::OnClickedCheckChatTyping( )
{
    DirectMessagingClient * client = this->GetChatClient( );
    if ( client != nullptr )
    {
        UpdateData( TRUE );
        client->notifyOnBroadcastMessageTyped( mIsChatTyping  ? true : false );
    }
    if ( mIsChatTyping == FALSE )
    {
        for ( int i = mLastItem; i < mCtrlList.GetItemCount( ); )
            mCtrlList.DeleteItem( i );
    }
}


void PageChat::OnUpdateEditChat( )
{
    CString msgOld = mChatMsg;
    UpdateData(TRUE);
    if (msgOld != mChatMsg)
    {
        sendType();
    }
}


void PageChat::OnClickedButtonChatSend( )
{
    DirectMessagingClient * client = this->GetChatClient( );
    if ( client != nullptr )
    {
        UpdateData( TRUE );
        client->requestMessageSend( GetConnectionOwner( ), String( mChatMsg.GetString( ) ), DateTime::getNow() );
        mChatMsg= _T( "" );
        UpdateData( FALSE );
        GetDlgItem( IDC_EDIT_CHAT )->SetFocus( );
    }
}


void PageChat::OnDestroy( )
{
    DirectMessagingClient * client = this->GetChatClient( );
    if ( client != nullptr )
    {
        client->clearAllNotifications();
        client->requestChatLeave( GetConnectionOwner( ), DateTime::getNow() );
    }


    SetChatWindow( 0 );
    SetChatClient( nullptr );

    const NEDirectConnection::sInitiator & initiator = GetInitiator();
    String modelName = NEDistributedApp::PREFIX_MODEL + GetServiceName();
    ComponentLoader::unloadComponentModel( modelName );
    
    ChatPrticipantHandler::Invalidate();
    
    CPropertyPage::OnDestroy( );
}


void PageChat::OnClickedButtonCloseChat( )
{
    DirectMessagingClient * client = this->GetChatClient( );
    if ( client != nullptr )
    {
        client->clearAllNotifications();
        client->requestChatLeave( GetConnectionOwner( ), DateTime::getNow() );
    }

    SetChatWindow( 0 );
    SetChatClient( nullptr );

    const NEDirectConnection::sInitiator & initiator = GetInitiator();
    String modelName = NEDistributedApp::PREFIX_MODEL + GetServiceName();
    ComponentLoader::unloadComponentModel( modelName );

    ChatPrticipantHandler::Invalidate();

    ::PostMessage( DistributedDialog::GetDialog()->GetSafeHwnd(), MAKE_MESSAGE(NEDistributedApp::eWndCommands::CmdChatClosed), 0, reinterpret_cast<LPARAM>(this));
}

void PageChat::OnKickIdle()
{
    UpdateDialogControls( this, FALSE );
}

void PageChat::OnButtonUpdateChatSend(CCmdUI * pCmdUI)
{
    if ( IsEmpty() == false )
    {
        UpdateData( TRUE );
        if ( mChatMsg.IsEmpty() == FALSE )
        {
            if ( mEditEnabled == FALSE )
            {
                CEdit * edit = reinterpret_cast<CEdit *>(GetDlgItem( IDC_EDIT_CHAT ));
                edit->SetFocus( );
                edit->SetSel( 0, mChatMsg.GetLength( ), TRUE );
            }
            pCmdUI->Enable( TRUE );
        }
        else
        {
            pCmdUI->Enable( FALSE );
        }
        mEditEnabled = TRUE;
    }
    else
    {
        pCmdUI->Enable( FALSE );
        mEditEnabled = FALSE;
        mChatMsg = _T( "" );
        UpdateData( FALSE );
    }
}

void PageChat::OnEditUpdateChat(CCmdUI * pCmdUI)
{
    pCmdUI->Enable( IsEmpty() == false);
}

void PageChat::setHeaders( void )
{
    int count = MACRO_ARRAYLEN( PageChat::HEADER_TITILES );
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

void PageChat::outputMessage( CString nickName
                            , CString message
                            , CString dateStart
                            , CString dateEnd
                            , uint32_t cookie
                            , bool removeType /* = true */ )
{
    if (message.IsEmpty())
        return;

    if (removeType)
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

    mCtrlList.SetItemText( mLastItem, 1, message.IsEmpty( )     == false ? message.GetString( )     : _T( "..." ) );
    mCtrlList.SetItemText( mLastItem, 2, dateStart.IsEmpty( )   == false ? dateStart.GetString( )   : _T( "..." ) );
    mCtrlList.SetItemText( mLastItem, 3, dateEnd.IsEmpty( )     == false ? dateEnd.GetString( )     : _T( "..." ) );

    mCtrlList.EnsureVisible( mLastItem, FALSE );
    ++ mLastItem;
}

void PageChat::outputTyping(CString nickName, CString message, uint32_t cookie )
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
            pos = mCtrlList.InsertItem( &lv );
        }

        mCtrlList.SetItemText( pos, 1, message.IsEmpty( ) == false ? message.GetBuffer( ) : _T( "..." ) );
        mCtrlList.SetItemText( pos, 2, _T( "typing ..." ) );
        mCtrlList.SetItemText( pos, 3, _T( "..." ) );

        mCtrlList.EnsureVisible( mCtrlList.GetItemCount() - 1, FALSE );
    }
    else
    {
        removeTyping(nickName, cookie);
    }
}


BOOL PageChat::OnKillActive( )
{
    DistributedDialog::DefaultCaption();
    return CPropertyPage::OnKillActive( );
}


BOOL PageChat::OnSetActive( )
{
    DistributedDialog::ChangeCaption(mTitle);
    setTabTitle( String(mTitle.GetString()) );
    return CPropertyPage::OnSetActive( );
}

void PageChat::setTabTitle( const String & title )
{
    CString tabTitle( title.getString() );

    DistributedDialog * dlg = DistributedDialog::GetDialog( );
    CTabCtrl * tab = dlg->GetTabControl( );
    dlg->GetPageIndex( this );
    ASSERT( tab != nullptr );
    TC_ITEM ti;
    ti.mask = TCIF_TEXT;
    ti.pszText = tabTitle.GetBuffer( );
    VERIFY( tab->SetItem( dlg->GetPageIndex( this ), &ti ) );
}

LRESULT PageChat::OnCmdChatMessage( WPARAM wParam, LPARAM lParam )
{
    NECommon::sMessageData * data = reinterpret_cast<NECommon::sMessageData *>(lParam);
    if ( data != nullptr )
    {
        outputMessage( CString( data->nickName )
                     , CString( data->message )
                     , CString( data->timeSend      != 0 ? DateTime(data->timeSend).formatTime().getString()     : "" )
                     , CString( data->timeReceived  != 0 ? DateTime(data->timeReceived).formatTime().getString() : "" )
                     , static_cast<uint32_t>(data->dataSave));
        delete data;

        if ( isActivePage() == false )
            setTabTitle( "(*) " + String(mTitle.GetString()) );
    }
    return 0;
}

LRESULT PageChat::OnCmdChatTyping( WPARAM wParam, LPARAM lParam )
{
    NECommon::sMessageData * data = reinterpret_cast<NECommon::sMessageData *>(lParam);
    if ( data != nullptr )
    {
        outputTyping(CString( data->nickName ), CString( data->message ), static_cast<uint32_t>(data->dataSave));
        delete data;
    }
    return 0;
}

LRESULT PageChat::OnCmdChatJoined( WPARAM wParam, LPARAM lParam )
{
    DirectMessagingClient * client = GetChatClient( );
    if ( (wParam == 1) && (client != nullptr) )
    {
        UpdateData(TRUE);
        client->notifyOnBroadcastMessageSent( mIsChatMessage ? true : false );
        client->notifyOnBroadcastMessageTyped( mIsChatTyping ? true : false );
    }
    return 0;
}

void PageChat::removeTyping( CString nickName, uint32_t cookie )
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

bool PageChat::isActivePage( void )
{
    CPropertySheet * sheet = GetParentSheet();
    return ( sheet != nullptr ? sheet->GetActivePage() == static_cast<const CPropertyPage *>(this) : false);
}

void PageChat::OnDefaultClicked( void )
{
    UpdateData(TRUE);
    sendMessage();
}

void PageChat::sendType(void)
{
    DirectMessagingClient* client = this->GetChatClient();
    if (client != nullptr)
    {
        client->requestMessageType(GetConnectionOwner(), String(mChatMsg.GetString()));
    }
}

void PageChat::sendMessage(void)
{
    if ((mEditEnabled == TRUE) && (mChatMsg.IsEmpty() == FALSE))
    {
        CButton* btnSend = reinterpret_cast<CButton*>(GetDlgItem(IDC_BUTTON_CHAT_SEND));
        if (btnSend != nullptr)
        {
            SendMessage(WM_COMMAND, MAKEWPARAM(IDC_BUTTON_CHAT_SEND, BN_CLICKED), reinterpret_cast<LPARAM>(btnSend->GetSafeHwnd()));
        }
    }
}

void PageChat::startTimer(void)
{
    if (mTimerId != 0)
        KillTimer(mTimerId);

    mDoAutotype = TRUE;
    mTimerId = static_cast<UINT_PTR>(time(nullptr));
    SetTimer(mTimerId, mTimerValue, nullptr);
}

void PageChat::stopTimer(void)
{
    if (mTimerId != 0)
    {
        KillTimer(mTimerId);
        mChatMsg        = _T("");
        mEditEnabled    = FALSE;
        mDoAutotype     = FALSE;
        mTimerId        = 0;
    }
}

void PageChat::OnBnClickedCheckAuto()
{
    UpdateData(TRUE);

    if (mDoAutotype)
    {
        startTimer();
    }
    else
    {
        stopTimer();
        UpdateWindow();
    }
}


void PageChat::OnTimer(UINT_PTR nIDEvent)
{
    
    char ch = rand() % 126;
    while (NEString::isAlphanumeric<char>(ch) == false)
    {
        if (++ch > 126)
            ch = 'a';
    }
    mChatMsg += ch;
    // mEditEnabled = mChatMsg.IsEmpty() == FALSE;
    UpdateData(FALSE);
    if (mChatMsg.GetLength() >= AUTOMESSAGE_MAX_LEN)
    {
        sendMessage();
    }
    else if (mChatMsg.IsEmpty() == FALSE)
    {
        sendType();
    }

    CPropertyPage::OnTimer(nIDEvent);
}


void PageChat::OnEnChangeChatTimer()
{
    int oldTimer = mTimerValue;
    UpdateData(TRUE);
    if (mDoAutotype && oldTimer != mTimerValue)
    {
        KillTimer(mTimerId);
        SetTimer(mTimerId, mTimerValue, nullptr);
    }
}


void PageChat::OnDeltaposChatTimerSpin(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
    int oldTimer = mTimerValue;

    mTimerValue += pNMUpDown->iDelta;
    if (mTimerValue < TIMER_MIN_VALUE)
        mTimerValue = TIMER_MIN_VALUE;
    if (mTimerValue > TIMER_MAX_VALUE)
        mTimerValue = TIMER_MAX_VALUE;
    UpdateData(FALSE);

    if (mDoAutotype && oldTimer != mTimerValue)
    {
        KillTimer(mTimerId);
        SetTimer(mTimerId, mTimerValue, nullptr);
    }

    *pResult = 0;
}
