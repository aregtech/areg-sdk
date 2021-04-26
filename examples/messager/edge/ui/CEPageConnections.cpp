// edge/CEPageConnections.cpp : implementation file
//

#include "edge/res/stdafx.h"
#include "edge/ui/CEPageConnections.hpp"
#include "edge/ui/CEPageChat.hpp"
#include "edge/ui/CEDistributedDialog.hpp"

#include "edge/services/CEDirectConnectionService.hpp"
#include "edge/services/CEConnectionList.hpp"
#include "areg/src/component/CEComponentLoader.hpp"
#include "areg/src/base/GEGlobal.h"
#include "areg/src/trace/GETrace.h"
#include "shared/NECommonSettings.hpp"

DEF_TRACE_SCOPE( distrbutedapp_ui_CEPageConnections_OnClientRegistration );
DEF_TRACE_SCOPE( distrbutedapp_ui_CEPageConnections_LoadModel );
DEF_TRACE_SCOPE( distrbutedapp_ui_CEPageConnections_AddConnection );

// CEPageConnections dialog

IMPLEMENT_DYNAMIC(CEPageConnections, CPropertyPage)

LPCTSTR CEPageConnections::HEADER_TITILES[] = 
{
      _T("Nick Name:")
    , _T("Connected at...")
};

CEPageConnections::CEPageConnections(CEConnectionHandler & handlerConnection)
	: CPropertyPage(CEPageConnections::IDD)

    , mCtrlConnections      ( )
    , mClientConnections    ( NULL )
    , mDirectConnectModel   ( )
    , mDirectConnectService ( )
    , mConnectionHandler    ( handlerConnection )
    , mChatEnable           ( FALSE )
{

}

CEPageConnections::~CEPageConnections()
{
    cleanService();
}

void CEPageConnections::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange( pDX );
    DDX_Control( pDX, IDC_LIST_CONNECTIONS, mCtrlConnections );
}

BEGIN_MESSAGE_MAP(CEPageConnections, CPropertyPage)
    ON_BN_CLICKED( IDC_BUTTON_INITIATE_CHAT, &CEPageConnections::OnClickedButtonInitiateChat )
    ON_MESSAGE_VOID( WM_KICKIDLE, OnKickIdle )
    ON_UPDATE_COMMAND_UI( IDC_BUTTON_INITIATE_CHAT, &CEPageConnections::OnBnUpdateInitiateChat )
    ON_WM_DESTROY( )
END_MESSAGE_MAP()

void CEPageConnections::OnServiceStartup( bool isStarted, CEComponent * owner )
{
    // do nothing
}

void CEPageConnections::OnServiceNetwork( bool isConnected, CEDispatcherThread * ownerThread )
{
    if ( isConnected && (ownerThread != NULL) )
    {
        if ( mConnectionHandler.IsValid( ) && (mConnectionHandler.GetRegistered( ) == false) && (mClientConnections == NULL) )
        {
            mClientConnections = DEBUG_NEW CEConnectionList( NECommonSettings::COMP_NAME_CENTRAL_SERVER, *ownerThread, mConnectionHandler );
        }
    }
}

void CEPageConnections::OnServiceConnection( bool isConnected, CEDispatcherThread * ownerThread )
{
    if ( isConnected && (ownerThread != NULL) )
    {
        ASSERT( mConnectionHandler.IsValid( ) && (mConnectionHandler.GetRegistered( ) == false) );
        const CEDateTime & dateTime = mConnectionHandler.GetTimeConnect();
        mClientConnections->NotifyOnBroadcastClientConnected( true );
        mClientConnections->NotifyOnBroadcastClientDisconnected( true );
        mClientConnections->RequestRegisterConnection( mConnectionHandler.GetNickName( ), mConnectionHandler.GetCookieDirect( ), mConnectionHandler.GetConnectCookie(), dateTime.IsValid() ? dateTime : CEDateTime::GetNow() );
    }
    else
    {
        if ( mClientConnections != NULL )
            delete mClientConnections;
        mClientConnections = NULL;
    }
}

void CEPageConnections::OnClientConnection( bool isConnected, CEDispatcherThread *dispThread )
{
    if ( isConnected )
    {
        if ( mClientConnections == NULL )
        {
            ASSERT( mConnectionHandler.IsValid() == true );
            ASSERT( mConnectionHandler.GetRegistered() == false );
            mClientConnections = DEBUG_NEW CEConnectionList( NECommonSettings::COMP_NAME_CENTRAL_SERVER, *dispThread, mConnectionHandler );
        }
    }
    else
    {
        if ( mClientConnections != NULL )
        {
            mClientConnections->NotifyOnBroadcastClientConnected( false );
            mClientConnections->NotifyOnBroadcastClientDisconnected( false );
        }
    }
}

void CEPageConnections::OnClientRegistration( bool isRegistered, CEDispatcherThread * dispThread )
{
    TRACE_SCOPE( distrbutedapp_ui_CEPageConnections_OnClientRegistration );
    if ( isRegistered )
    {
        ASSERT(mClientConnections != NULL);
        const CEString & nickname   = mConnectionHandler.GetNickName();
        const uint32_t cookie       = mConnectionHandler.GetCookie();

        TRACE_DBG("The client with nickName [ %s ] and cookie [ %u ] is registered with success, the existing service name is [ %s ]."
                    , nickname.String()
                    , cookie
                    , mDirectConnectService.String() );

        mCtrlConnections.DeleteAllItems();
        const NECommonSettings::ListConnections & listConnections = mConnectionHandler.GetConnectionList( );
        for ( int i = 0; i < listConnections.GetSize(); ++ i )
        {
            const NECommonSettings::sConnection & connection = listConnections.GetAt(i);
            addConnection( connection );
        }
        loadModel(nickname, cookie);
    }
    else
    {
        TRACE_WARN("Client [ %s ] is unregistered", mConnectionHandler.GetNickName().String());
    }
}

void CEPageConnections::OnAddConnection( NEConnectionManager::sConnection & data )
{
    addConnection(data);
}

void CEPageConnections::OnRemoveConnection( NEConnectionManager::sConnection & data )
{
    removeConnection(data);
}

void CEPageConnections::OnUpdateConnection( void )
{
    // do nothing
}

void CEPageConnections::OnDisconnectTriggered( void )
{
    mCtrlConnections.DeleteAllItems( );
    unloadModel();
    cleanService( );
}

// CEPageConnections message handlers

void CEPageConnections::OnClickedButtonInitiateChat( )
{
    NEDirectConnection::sInitiator          initiator;
    NEDirectConnection::ListParticipants    listParticipnats;
    if ( getSelectedConnections(initiator, listParticipnats) > 1 )
        CEDistributedDialog::GetDialog( )->AddChatPage( initiator, listParticipnats, true );
}

BOOL CEPageConnections::OnInitDialog( )
{
    CPropertyPage::OnInitDialog( );

    setHeaders();
    return TRUE;  // return TRUE unless you set the focus to a control
}

void CEPageConnections::OnKickIdle( )
{
    UpdateDialogControls( this, FALSE );
}

void CEPageConnections::OnBnUpdateInitiateChat( CCmdUI* pCmdUI )
{
    if ( mCtrlConnections.GetSelectedCount( ) != 0 )
    {
        pCmdUI->Enable( TRUE );
        mChatEnable = TRUE;
    }
    else
    {
        pCmdUI->Enable( FALSE );
        mChatEnable = FALSE;
    }
}

void CEPageConnections::OnDestroy( )
{
    CPropertyPage::OnDestroy( );
}

const CEString & CEPageConnections::GetRegisteredName( void ) const
{
    return mConnectionHandler.GetNickName( );
}

const uint32_t CEPageConnections::GetRegisteredCookie( void ) const
{
    return mConnectionHandler.GetCookie( );
}

inline void CEPageConnections::addConnection( const NEConnectionManager::sConnection & connection )
{
    TRACE_SCOPE( distrbutedapp_ui_CEPageConnections_AddConnection );
    if ( mConnectionHandler.GetNickName() != connection.nickName )
    {
        TRACE_DBG( "Adding new connection of nickName [ %s ] and cookie [ %u ]", connection.nickName.String( ), connection.cookie );
        int pos = mCtrlConnections.GetItemCount( );
        CString nickName( connection.nickName );
        CString timeConnect( CEDateTime( connection.connectTime ).FormatTime( ).GetBuffer( ) );
        uint32_t cookie = connection.cookie;

        LVITEM lv;
        NEMemory::ZeroData<LVITEM>( lv );

        // Column nickname
        lv.mask = LVIF_TEXT | LVIF_PARAM;
        lv.iItem = pos;
        lv.iSubItem = 0;
        lv.pszText = nickName.GetBuffer( );
        lv.lParam = cookie;
        lv.cchTextMax = NEConnectionManager::NicknameMaxLen;
        mCtrlConnections.InsertItem( &lv );
        mCtrlConnections.SetItemText( pos, 1, timeConnect.GetBuffer( ) );
        mCtrlConnections.EnsureVisible( pos, FALSE );
    }
    else
    {
        TRACE_WARN("Ignoring adding new connection. Existing connection nickName [ %s ] cookie [ %u ], add connection nickaName [ %s ] cookie [ %u ]"
                    , mConnectionHandler.GetNickName().String()
                    , mConnectionHandler.GetCookie()
                    , connection.nickName.String()
                    , connection.cookie);
    }
}

inline int CEPageConnections::getSelectedConnections( NEDirectConnection::sInitiator & outParticipant, NEDirectConnection::ListParticipants & outListParticipants )
{
    outListParticipants.RemoveAll();
    UINT selected = mCtrlConnections.GetSelectedCount();
    int count = 0;
    if (selected != 0)
    {
        outListParticipants.ReserveElements(selected + 1, 1);
        CEDateTime now = CEDateTime::GetNow();

        outParticipant.nickName  = mConnectionHandler.GetNickName();
        outParticipant.cookie    = mConnectionHandler.GetCookie();
        outParticipant.sessionId = now.GetTime();

        outListParticipants.SetAt(count ++, outParticipant);

        POSITION pos = mCtrlConnections.GetFirstSelectedItemPosition();
        do
        {
            int i = mCtrlConnections.GetNextSelectedItem(pos);
            ASSERT( i != -1 );
            CString nickName= mCtrlConnections.GetItemText(i, 0);
            LPARAM cookie   = mCtrlConnections.GetItemData(i);

            NEDirectConnection::sParticipant participant;
            participant.nickName    = static_cast<LPCTSTR>(nickName);
            participant.cookie      = static_cast<uint32_t>(cookie);
            participant.sessionId   = static_cast<uint64_t>(now);

            outListParticipants.SetAt(count ++, participant);

        } while (pos != NULL);
    }
    return count;
}

inline void CEPageConnections::setHeaders( void )
{
    int count = MACRO_ARRAYLEN( CEPageConnections::HEADER_TITILES );
    CRect rc( 0, 0, 0, 0 );
    mCtrlConnections.GetClientRect( &rc );
    int width = rc.Width( ) / count;

    for ( int i = 0; i < count; ++ i )
    {
        CString str( HEADER_TITILES[i] );
        LVCOLUMN lv;
        NEMemory::ZeroData<LVCOLUMN>( lv );
        lv.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
        lv.fmt = LVCFMT_LEFT;
        lv.cx = width;
        lv.pszText = str.GetBuffer( );
        lv.cchTextMax = str.GetLength( );

        mCtrlConnections.InsertColumn( i, &lv );
    }
}

inline bool CEPageConnections::isServiceConnected( void ) const
{
    return (mClientConnections != NULL ? mClientConnections->IsConnected( ) : false);
}

inline void CEPageConnections::cleanService( void )
{
    if ( mClientConnections != NULL )
    {
        delete mClientConnections;
        mClientConnections = NULL;
    }
    if ( mDirectConnectModel.IsEmpty( ) == false )
    {
        CEComponentLoader::RemoveModel( mDirectConnectModel );
        mDirectConnectModel.Clear( );
    }
}

inline int CEPageConnections::findConnection( const NEConnectionManager::sConnection & connection ) const
{
    int result = NECommon::InvalidIndex;
    for ( int i = 0; i < mCtrlConnections.GetItemCount(); ++ i )
    {
        if ( mCtrlConnections.GetItemData(i) == connection.cookie )
        {
#ifdef _DEBUG
            CString nickName = mCtrlConnections.GetItemText( i, 0 );
            ASSERT(connection.nickName == nickName.GetString());
#endif // _DEBUG
            result = i;
            break;
        }
    }
    return result;
}

inline void CEPageConnections::removeConnection( const NEConnectionManager::sConnection & connection )
{
    int pos = findConnection(connection);
    if ( pos != NECommon::InvalidIndex )
    {
        mCtrlConnections.EnsureVisible( pos, FALSE );
        mCtrlConnections.DeleteItem( pos );
    }
}

inline void CEPageConnections::unloadModel( void )
{
    if ( mDirectConnectModel.IsEmpty( ) == false )
    {
        CEComponentLoader::RemoveModel( mDirectConnectModel );
    }
    mDirectConnectModel.Clear( );
    mDirectConnectService.Clear( );
}

inline bool CEPageConnections::loadModel( const CEString & nickName, const uint32_t cookie )
{
    TRACE_SCOPE( distrbutedapp_ui_CEPageConnections_LoadModel );

    bool result = false;

    CEString serviceName = CEDirectConnectionService::GetGeneratedService(nickName, cookie);
    if ( (mDirectConnectService.IsEmpty() == true) || (mDirectConnectService != serviceName) )
    {
        unloadModel( );
        
        NEMemory::uAlign compData;
        compData.alignClsPtr.mElement = reinterpret_cast<NEMemory::_EmptyClass *>(this);
        NERegistry::CEModel model = CEDirectConnectionService::GetModel( nickName, cookie, compData );

        TRACE_DBG("Going to load model [ %s ] with service name [ %s ]", model.GetModelName().String(), serviceName.String() );

        if ( CEComponentLoader::AddComponentModel( model ) )
        {
            mDirectConnectModel     = model.GetModelName( );
            mDirectConnectService   = serviceName;
            result = CEComponentLoader::LoadComponentModel( mDirectConnectModel );
        }
    }
    else
    {
        TRACE_WARN( "The service [ %s ] is already running, ignoring creating new model for nick name [ %s ] with cookie [ %u ]", serviceName.String( ), nickName.String( ), cookie );
        ASSERT( mConnectionHandler.GetNickName( ).IsEmpty( ) == false );
        ASSERT( mConnectionHandler.GetCookie( ) != NEDirectConnection::InvalidCookie );
        result = true;
    }
    return result;
}

void CEPageConnections::OnDefaultClicked( void )
{
    CButton * btnSend = reinterpret_cast<CButton *>(GetDlgItem( IDC_BUTTON_INITIATE_CHAT ));
    if ( btnSend != NULL )
    {
        PostMessage( WM_COMMAND, MAKEWPARAM( IDC_BUTTON_INITIATE_CHAT, BN_CLICKED ), reinterpret_cast<LPARAM>(btnSend->GetSafeHwnd( )) );
    }
}
