// PageConnections.cpp : implementation file
//

#include "register/res/stdafx.h"
#include "register/CentralApp.hpp"
#include "register/ui/PageConnections.hpp"
#include "register/services/ConnectionManager.hpp"
#include "register/NECentralApp.hpp"

#include "areg/base/GEGlobal.h"
#include "areg/base/GEMacros.h"
#include "areg/base/NEMemory.hpp"


// PageConnections

#define FIRST_MESSAGE       (WM_USER + 10 + static_cast<unsigned int>(NECentralApp::eWndCommands::CmdFirst))
#define MAKE_MESSAGE(elem)  (static_cast<unsigned int>(elem) + FIRST_MESSAGE)

LPCTSTR PageConnections::HEADER_TITILES[] =
{
      _T( "Nick Name:" )
    , _T( "Message:" )
    , _T( "Sent at:" )
    , _T( "Received at:" )
};

IMPLEMENT_DYNAMIC(PageConnections, CPropertyPage)

PageConnections::PageConnections()
    : CPropertyPage (PageConnections::IDD)

    , mCtrlList     ( )
    , mLastItem     ( 0 )
    , mTypingList   ( )
    , mTextBroadcast( _T( "" ) )
    , mRegistered   ( 0 )
    , mEditEnabled  ( FALSE )
{

}

PageConnections::~PageConnections()
{
    for (uint32_t i = 0; i < mTypingList.getSize(); ++ i )
    {
        NECommon::sMessageData * data = mTypingList.getAt(i);
        delete data;
    }

    mTypingList.clear();
}

void PageConnections::ServiceConnected( bool isConnected )
{
}

void PageConnections::OutputMessage( CString nickName, CString message, CString dateStart, CString dateEnd, LPARAM data )
{
    LVITEM lv;
    NEMemory::zeroElement<LVITEM>(lv);

    // Column nickname
    lv.mask         = LVIF_TEXT | LVIF_PARAM;
    lv.iItem        = mLastItem;
    lv.iSubItem     = 0;
    lv.pszText      = nickName.GetBuffer();
    lv.lParam       = data;
    lv.cchTextMax   = NEConnectionManager::NicknameMaxLen;
    mCtrlList.InsertItem(&lv);

    if ( dateStart.GetLength() > NECommon::DAY_FORMAT_LEN )
        dateStart = dateStart.Mid( NECommon::DAY_FORMAT_LEN );
    if ( dateEnd.GetLength() > NECommon::DAY_FORMAT_LEN )
        dateEnd = dateEnd.Mid( NECommon::DAY_FORMAT_LEN );

    mCtrlList.SetItemText(mLastItem, 1, message.IsEmpty() == false  ? message.GetBuffer()   : _T("..."));
    mCtrlList.SetItemText(mLastItem, 2, dateStart.IsEmpty() == false? dateStart.GetBuffer() : _T("..."));
    mCtrlList.SetItemText(mLastItem, 3, dateEnd.IsEmpty() == false  ? dateEnd.GetBuffer()   : _T("..."));

    mCtrlList.EnsureVisible( mLastItem, FALSE );
    ++ mLastItem;
}

void PageConnections::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange( pDX );
    DDX_Control( pDX, IDC_LIST_CONNECTIONS, mCtrlList );
    DDX_Text( pDX, IDC_EDIT_BROADCAST, mTextBroadcast );
	DDV_MaxChars(pDX, mTextBroadcast, 511);
}


BEGIN_MESSAGE_MAP(PageConnections, CPropertyPage)
    ON_BN_CLICKED(IDC_BUTTON_BROADCAST, &PageConnections::OnClickedButtonBroadcast)
    ON_MESSAGE_VOID( WM_KICKIDLE, OnKickIdle )
    ON_UPDATE_COMMAND_UI( IDC_BUTTON_BROADCAST, &PageConnections::OnBtnBroadcastUpdate )
    ON_UPDATE_COMMAND_UI( IDC_EDIT_BROADCAST, &PageConnections::OnEditBroadcastUpdate )
    ON_WM_CREATE( )
    ON_MESSAGE( MAKE_MESSAGE(NECentralApp::eWndCommands::CmdRegistered  ), &PageConnections::OnCmdRegistered)
    ON_MESSAGE( MAKE_MESSAGE(NECentralApp::eWndCommands::CmdUnregistered), &PageConnections::OnCmdUnregistered )
    ON_MESSAGE( MAKE_MESSAGE(NECentralApp::eWndCommands::CmdSendMessage ), &PageConnections::OnCmdSendMessage )
    ON_MESSAGE( MAKE_MESSAGE(NECentralApp::eWndCommands::CmdTypeMessage ), &PageConnections::OnCmdTypeMessage )
END_MESSAGE_MAP( )

// PageConnections message handlers

void PageConnections::OnClickedButtonBroadcast()
{
    UpdateData(TRUE);
    ConnectionManager * service = !mTextBroadcast.IsEmpty() ? ConnectionManager::getService( ) : nullptr;
    if ( service != nullptr )
    {
        DateTime timestamp = DateTime::getNow();
        String msg( mTextBroadcast.GetString() );
        service->broadcastBroadcastMessage(msg, timestamp);

        OutputMessage(   CString(NECommon::SERVER_NAME)
                       , mTextBroadcast
                       , CString( timestamp.formatTime( ).getString( ) )
                       , CentralApp::EmptyString
                       , NEConnectionManager::InvalidCookie );

        mTextBroadcast = _T("");
        UpdateData(FALSE);
    }
}

void PageConnections::OnBtnBroadcastUpdate( CCmdUI* pCmdUI )
{
    UpdateData(TRUE);
    pCmdUI->Enable( (mRegistered != 0) && (mTextBroadcast.IsEmpty() == FALSE));
}

void PageConnections::OnEditBroadcastUpdate( CCmdUI* pCmdUI )
{
    if ( mRegistered != 0 )
    {
        pCmdUI->Enable( TRUE );
        if ( mEditEnabled == FALSE )
        {
            CEdit * edit = reinterpret_cast<CEdit *>(GetDlgItem(IDC_EDIT_BROADCAST));
            edit->SetFocus();
            edit->SetSel(0, mTextBroadcast.GetLength(), TRUE );
        }
        mEditEnabled = TRUE;
    }
    else
    {
        pCmdUI->Enable( FALSE);
        mEditEnabled    = FALSE;
        mTextBroadcast  = _T("");
        UpdateData(FALSE);
    }
}

void PageConnections::setHeaders( void )
{
    int count = MACRO_ARRAYLEN( HEADER_TITILES );
    CRect rc( 0, 0, 0, 0 );
    mCtrlList.GetClientRect( &rc );
    int width1, width2;
    NECommon::getWidths( rc.Width(), count, width1, width2 );

    for ( int i = 0; i < count; ++ i )
    {
        CString str( HEADER_TITILES[i] );
        LVCOLUMN lv;
        NEMemory::zeroElement<LVCOLUMN>(lv);
        lv.mask         = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
        lv.fmt          = LVCFMT_LEFT;
        lv.cx           = i == 0 ? width1 : width2;
        lv.pszText      = str.GetBuffer();
        lv.cchTextMax   = str.GetLength();

        mCtrlList.InsertColumn(i, &lv);
    }
}

void PageConnections::OnKickIdle( void )
{
    UpdateDialogControls( this, FALSE );
}


BOOL PageConnections::OnInitDialog( )
{
    CPropertyPage::OnInitDialog( );

    CButton * btnSend = reinterpret_cast<CButton *>(GetDlgItem( IDC_BUTTON_BROADCAST ));
    if (btnSend != nullptr )
        btnSend->SetFocus();

    setHeaders();
    UpdateDialogControls( this, FALSE );


    return TRUE;  // return TRUE unless you set the focus to a control
    // EXPTION: OCX Property Pages should return FALSE
}

LRESULT PageConnections::OnCmdRegistered( WPARAM wParam, LPARAM lParam )
{
    NECommon::sMessageData * data = reinterpret_cast<NECommon::sMessageData *>(lParam);
    if ( data != nullptr )
    {
        ++ mRegistered;

        OutputMessage(   CString(data->nickName)
                       , CString(_T("New registration ..."))
                       , CString( DateTime(data->timeSend).formatTime().getString() )
                       , CString( DateTime(data->timeReceived).formatTime().getString() )
                       , static_cast<LPARAM>(data->dataSave) );

        delete data;
    }
    return 0L;
}

int PageConnections::findInTyping( unsigned int cookie )
{
    int result = NECommon::INVALID_INDEX;
    for (uint32_t i = 0; i < mTypingList.getSize( ); ++i )
    {
        if (cookie == mTypingList[i]->dataSave)
        {
            result = static_cast<int>(i);
            break;
        }
    }

    return result;
}

LRESULT PageConnections::OnCmdUnregistered( WPARAM wParam, LPARAM lParam )
{
    NECommon::sMessageData * data = reinterpret_cast<NECommon::sMessageData *>(lParam);
    if ( data != nullptr )
    {
        if ( mRegistered != 0 )
            -- mRegistered;
        int count = mCtrlList.GetItemCount();

        for ( int i = mLastItem; i < count; ++ i )
        {
            if ( mCtrlList.GetItemData( i ) == data->dataSave )
            {
                mCtrlList.DeleteItem(i);
                break;
            }
        }

        OutputMessage(   CString(data->nickName)
                       , CString(_T("Disconnected ..."))
                       , CString( DateTime(data->timeSend).formatTime().getString() )
                       , CString( DateTime(data->timeReceived).formatTime().getString() )
                       , static_cast<LPARAM>(NECommon::InvalidCookie) );

        delete data;
    }
    return 0L;
}

LRESULT PageConnections::OnCmdSendMessage( WPARAM wParam, LPARAM lParam )
{
    NECommon::sMessageData * data = reinterpret_cast<NECommon::sMessageData *>(lParam);
    if ( data != nullptr )
    {
        int rmIndex = findInTyping( static_cast<uint32_t>(data->dataSave) );
        if ( rmIndex != NECommon::INVALID_INDEX )
        {
            NECommon::sMessageData *temp = mTypingList.getAt(rmIndex);
            mTypingList.removeAt( rmIndex, 1 );
            delete temp;
        }

        for ( int i = mLastItem; i < mCtrlList.GetItemCount(); ++ i )
        {
            LPARAM lParam = mCtrlList.GetItemData(i);
            if ( data->dataSave == static_cast<uint64_t>(lParam) )
            {
                mCtrlList.DeleteItem(i);
                break;
            }
        }

        OutputMessage(   CString(data->nickName)
                       , CString(data->message)
                       , CString( DateTime(data->timeSend).formatTime().getString() )
                       , CString( DateTime(data->timeReceived).formatTime().getString() )
                       , static_cast<LPARAM>(NECommon::InvalidCookie) );

        delete data;
    }
    return 0L;
}

LRESULT PageConnections::OnCmdTypeMessage( WPARAM wParam, LPARAM lParam )
{
    NECommon::sMessageData * data = reinterpret_cast<NECommon::sMessageData *>(lParam);
    bool isEmpty = data != nullptr ? NEString::isEmpty<TCHAR>( data->message ) : true;
    if ( data != nullptr )
    {
        int rmIndex = findInTyping( static_cast<uint32_t>(data->dataSave) );
        if ( rmIndex != NECommon::INVALID_INDEX )
        {
            NECommon::sMessageData *temp = mTypingList.getAt( rmIndex );
            if ( isEmpty )
                mTypingList.removeAt(rmIndex);
            else
                mTypingList.setAt(rmIndex, data);
            delete temp;

            if ( mCtrlList.GetItemCount() != 0 )
            {
                for ( int i = mLastItem; i < mCtrlList.GetItemCount( ); ++ i )
                {
                    LPARAM lParam = mCtrlList.GetItemData( i );
                    if ( data->dataSave == static_cast<uint64_t>(lParam) )
                    {
                        if ( isEmpty )
                            mCtrlList.DeleteItem(i);
                        else
                            mCtrlList.SetItemText(i, 1, data->message);
                        break;
                    }
                }
            }

        }
        else if (isEmpty == false )
        {
            mTypingList.add(data);

            LVITEM lv;
            NEMemory::zeroElement<LVITEM>( lv );

            // Column nickname
            lv.mask         = LVIF_TEXT | LVIF_PARAM;
            lv.iItem        = mLastItem;
            lv.iSubItem     = 0;
            lv.pszText      = data->nickName;
            lv.lParam       = static_cast<LPARAM>(data->dataSave);
            lv.cchTextMax   = NECentralMessager::MessageMaxLen;
            mCtrlList.InsertItem( &lv );

            mCtrlList.SetItemText( mLastItem, 1, data->message );
            mCtrlList.SetItemText( mLastItem, 2, _T("typing...") );
            mCtrlList.SetItemText( mLastItem, 3, _T("...") );
        }
    }
    return 0L;
}

void PageConnections::OnDefaultClicked( void )
{
    CButton * btnSend   = reinterpret_cast<CButton *>( GetDlgItem( IDC_BUTTON_BROADCAST ) );
    if ( btnSend != nullptr )
    {
        PostMessage( WM_COMMAND, MAKEWPARAM( IDC_BUTTON_BROADCAST, BN_CLICKED ), reinterpret_cast<LPARAM>(btnSend->GetSafeHwnd( )) );
    }
}
