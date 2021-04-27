// CEPageConnections.cpp : implementation file
//

#include "register/res/stdafx.h"
#include "register/CECentralApp.hpp"
#include "register/ui/CEPageConnections.hpp"
#include "register/services/CEConnectionManager.hpp"
#include "register/NECentralApp.hpp"

#include "areg/base/GEGlobal.h"
#include "areg/base/GEMacros.h"
#include "areg/base/NEMemory.hpp"


// CEPageConnections

LPCTSTR CEPageConnections::HEADER_TITILES[] =
{
      _T( "Nick Name:" )
    , _T( "Message:" )
    , _T( "Sent at:" )
    , _T( "Received at:" )
};

IMPLEMENT_DYNAMIC(CEPageConnections, CPropertyPage)

CEPageConnections::CEPageConnections()
    : CPropertyPage (CEPageConnections::IDD)

    , mCtrlList     ( )
    , mLastItem     ( 0 )
    , mTypingList   ( )
    , mTextBroadcast( _T( "" ) )
    , mRegistered   ( 0 )
    , mEditEnabled  ( FALSE )
{

}

CEPageConnections::~CEPageConnections()
{
    for ( int i = 0; i < mTypingList.GetSize(); ++ i )
    {
        NECommonSettings::sMessageData * data = mTypingList.GetAt(i);
        delete data;
    }
    mTypingList.RemoveAll();
}

void CEPageConnections::ServiceConnected( bool isConnected )
{
}

void CEPageConnections::OutputMessage( CString & nickName, CString & message, CString & dateStart, CString & dateEnd, LPARAM data )
{
    LVITEM lv;
    NEMemory::ZeroData<LVITEM>(lv);

    // Column nickname
    lv.mask         = LVIF_TEXT | LVIF_PARAM;
    lv.iItem        = mLastItem;
    lv.iSubItem     = 0;
    lv.pszText      = nickName.GetBuffer();
    lv.lParam       = data;
    lv.cchTextMax   = NEConnectionManager::NicknameMaxLen;
    mCtrlList.InsertItem(&lv);

    if ( dateStart.GetLength() > NECommonSettings::DAY_FORMAT_LEN )
        dateStart = dateStart.Mid( NECommonSettings::DAY_FORMAT_LEN );
    if ( dateEnd.GetLength() > NECommonSettings::DAY_FORMAT_LEN )
        dateEnd = dateEnd.Mid( NECommonSettings::DAY_FORMAT_LEN );

    mCtrlList.SetItemText(mLastItem, 1, message.IsEmpty() == false  ? message.GetBuffer()   : _T("..."));
    mCtrlList.SetItemText(mLastItem, 2, dateStart.IsEmpty() == false? dateStart.GetBuffer() : _T("..."));
    mCtrlList.SetItemText(mLastItem, 3, dateEnd.IsEmpty() == false  ? dateEnd.GetBuffer()   : _T("..."));

    mCtrlList.EnsureVisible( mLastItem, FALSE );
    ++ mLastItem;
}

void CEPageConnections::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange( pDX );
    DDX_Control( pDX, IDC_LIST_CONNECTIONS, mCtrlList );
    DDX_Text( pDX, IDC_EDIT_BROADCAST, mTextBroadcast );
	DDV_MaxChars(pDX, mTextBroadcast, 511);
}


BEGIN_MESSAGE_MAP(CEPageConnections, CPropertyPage)
    ON_BN_CLICKED(IDC_BUTTON_BROADCAST, &CEPageConnections::OnClickedButtonBroadcast)
    ON_MESSAGE_VOID( WM_KICKIDLE, OnKickIdle )
    ON_UPDATE_COMMAND_UI( IDC_BUTTON_BROADCAST, &CEPageConnections::OnBtnBroadcastUpdate )
    ON_UPDATE_COMMAND_UI( IDC_EDIT_BROADCAST, &CEPageConnections::OnEditBroadcastUpdate )
    ON_WM_CREATE( )
    ON_MESSAGE( NECentralApp::CmdRegistered    , &CEPageConnections::OnCmdRegistered)
    ON_MESSAGE( NECentralApp::CmdUnregistered  , &CEPageConnections::OnCmdUnregistered )
    ON_MESSAGE( NECentralApp::CmdSendMessage   , &CEPageConnections::OnCmdSendMessage )
    ON_MESSAGE( NECentralApp::CmdTypeMessage   , &CEPageConnections::OnCmdTypeMessage )
END_MESSAGE_MAP( )

// CEPageConnections message handlers

void CEPageConnections::OnClickedButtonBroadcast()
{
    UpdateData(TRUE);
    CEConnectionManager * service = !mTextBroadcast.IsEmpty() ? CEConnectionManager::GetService( ) : static_cast<CEConnectionManager *>(NULL);
    if ( service != static_cast<CEConnectionManager *>(NULL) )
    {
        CEDateTime timestamp = CEDateTime::GetNow();
        CEString msg( mTextBroadcast.GetString() );
        service->BroadcastBroadcastMessage(msg, timestamp);

        OutputMessage(   CString(NECommonSettings::SERVER_NAME)
                       , mTextBroadcast
                       ,  CString( timestamp.FormatTime( ).GetBuffer( ) )
                       , CString()
                       , NEConnectionManager::InvalidCookie );

        mTextBroadcast = _T("");
        UpdateData(FALSE);
    }
}

void CEPageConnections::OnBtnBroadcastUpdate( CCmdUI* pCmdUI )
{
    UpdateData(TRUE);
    pCmdUI->Enable( (mRegistered != 0) && (mTextBroadcast.IsEmpty() == FALSE));
}

void CEPageConnections::OnEditBroadcastUpdate( CCmdUI* pCmdUI )
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

void CEPageConnections::setHeaders( void )
{
    int count = MACRO_ARRAYLEN( HEADER_TITILES );
    CRect rc( 0, 0, 0, 0 );
    mCtrlList.GetClientRect( &rc );
    int width1, width2;
    NECommonSettings::getWidths( rc.Width(), count, width1, width2 );

    for ( int i = 0; i < count; ++ i )
    {
        CString str( HEADER_TITILES[i] );
        LVCOLUMN lv;
        NEMemory::ZeroData<LVCOLUMN>(lv);
        lv.mask         = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
        lv.fmt          = LVCFMT_LEFT;
        lv.cx           = i == 0 ? width1 : width2;
        lv.pszText      = str.GetBuffer();
        lv.cchTextMax   = str.GetLength();

        mCtrlList.InsertColumn(i, &lv);
    }
}

void CEPageConnections::OnKickIdle( void )
{
    UpdateDialogControls( this, FALSE );
}


BOOL CEPageConnections::OnInitDialog( )
{
    CPropertyPage::OnInitDialog( );

    CButton * btnSend = reinterpret_cast<CButton *>(GetDlgItem( IDC_BUTTON_BROADCAST ));
    if (btnSend != NULL )
        btnSend->SetFocus();

    setHeaders();
    UpdateDialogControls( this, FALSE );


    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CEPageConnections::OnCmdRegistered( WPARAM wParam, LPARAM lParam )
{
    NECommonSettings::sMessageData * data = reinterpret_cast<NECommonSettings::sMessageData *>(lParam);
    if ( data != NULL )
    {
        ++ mRegistered;

        OutputMessage(   CString(data->nickName)
                       , CString(_T("New registration ..."))
                       , CString( CEDateTime(data->timeSend).FormatTime().GetBuffer() )
                       , CString( CEDateTime(data->timeReceived).FormatTime().GetBuffer() )
                       , static_cast<LPARAM>(data->dataSave) );

        delete data;
    }
    return 0L;
}

int CEPageConnections::findInTyping( unsigned int cookie )
{
    int result = NECommon::InvalidIndex;
    for ( int i = 0; (result == NECommon::InvalidIndex) && (i < mTypingList.GetSize( )); ++ i )
    {
        if ( cookie == mTypingList[i]->dataSave )
            result = i;
    }
    return result;
}

LRESULT CEPageConnections::OnCmdUnregistered( WPARAM wParam, LPARAM lParam )
{
    NECommonSettings::sMessageData * data = reinterpret_cast<NECommonSettings::sMessageData *>(lParam);
    if ( data != NULL )
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
                       , CString( CEDateTime(data->timeSend).FormatTime().GetBuffer() )
                       , CString( CEDateTime(data->timeReceived).FormatTime().GetBuffer() )
                       , static_cast<LPARAM>(NECommonSettings::InvalidCookie) );

        delete data;
    }
    return 0L;
}

LRESULT CEPageConnections::OnCmdSendMessage( WPARAM wParam, LPARAM lParam )
{
    NECommonSettings::sMessageData * data = reinterpret_cast<NECommonSettings::sMessageData *>(lParam);
    if ( data != NULL )
    {
        int rmIndex = findInTyping( static_cast<uint32_t>(data->dataSave) );
        if ( rmIndex != NECommon::InvalidIndex )
        {
            NECommonSettings::sMessageData *temp = mTypingList.GetAt(rmIndex);
            mTypingList.RemoveAt( rmIndex, 1 );
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
                       , CString( CEDateTime(data->timeSend).FormatTime().GetBuffer() )
                       , CString( CEDateTime(data->timeReceived).FormatTime().GetBuffer() )
                       , static_cast<LPARAM>(NECommonSettings::InvalidCookie) );

        delete data;
    }
    return 0L;
}

LRESULT CEPageConnections::OnCmdTypeMessage( WPARAM wParam, LPARAM lParam )
{
    NECommonSettings::sMessageData * data = reinterpret_cast<NECommonSettings::sMessageData *>(lParam);
    bool isEmpty = data != NULL ? NEString::isEmpty<TCHAR>( data->message ) : true;
    if ( data != NULL )
    {
        int rmIndex = findInTyping( static_cast<uint32_t>(data->dataSave) );
        if ( rmIndex != NECommon::InvalidIndex )
        {
            NECommonSettings::sMessageData *temp = mTypingList.GetAt( rmIndex );
            if ( isEmpty )
                mTypingList.RemoveAt(rmIndex);
            else
                mTypingList.SetAt(rmIndex, data);
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
            mTypingList.Add(data);

            LVITEM lv;
            NEMemory::ZeroData<LVITEM>( lv );

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

void CEPageConnections::OnDefaultClicked( void )
{
    CButton * btnSend   = reinterpret_cast<CButton *>( GetDlgItem( IDC_BUTTON_BROADCAST ) );
    if ( btnSend != NULL )
    {
        PostMessage( WM_COMMAND, MAKEWPARAM( IDC_BUTTON_BROADCAST, BN_CLICKED ), reinterpret_cast<LPARAM>(btnSend->GetSafeHwnd( )) );
    }
}
