// chatter/PageConnections.cpp : implementation file
//

#include "chatter/res/stdafx.h"
#include "chatter/ui/PageConnections.hpp"
#include "chatter/ui/PageChat.hpp"
#include "chatter/ui/DistributedDialog.hpp"

#include "chatter/services/DirectConnectionService.hpp"
#include "chatter/services/ConnectionList.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/base/GEGlobal.h"
#include "areg/trace/GETrace.h"
#include "generated/NECommon.hpp"

DEF_TRACE_SCOPE( distrbutedapp_ui_PageConnections_OnClientRegistration );
DEF_TRACE_SCOPE( distrbutedapp_ui_PageConnections_LoadModel );
DEF_TRACE_SCOPE( distrbutedapp_ui_PageConnections_AddConnection );

// PageConnections dialog

IMPLEMENT_DYNAMIC(PageConnections, CPropertyPage)

LPCTSTR PageConnections::HEADER_TITILES[] = 
{
      _T("Nick Name:")
    , _T("Connected at...")
};

PageConnections::PageConnections(ConnectionHandler & handlerConnection)
	: CPropertyPage(PageConnections::IDD)

    , mCtrlConnections      ( )
    , mClientConnections    ( nullptr )
    , mDirectConnectModel   ( )
    , mDirectConnectService ( )
    , mConnectionHandler    ( handlerConnection )
    , mChatEnable           ( FALSE )
{

}

PageConnections::~PageConnections()
{
    cleanService();
}

void PageConnections::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange( pDX );
    DDX_Control( pDX, IDC_LIST_CONNECTIONS, mCtrlConnections );
}

BEGIN_MESSAGE_MAP(PageConnections, CPropertyPage)
    ON_BN_CLICKED( IDC_BUTTON_INITIATE_CHAT, &PageConnections::OnClickedButtonInitiateChat )
    ON_MESSAGE_VOID( WM_KICKIDLE, OnKickIdle )
    ON_UPDATE_COMMAND_UI( IDC_BUTTON_INITIATE_CHAT, &PageConnections::OnBnUpdateInitiateChat )
    ON_WM_DESTROY( )
END_MESSAGE_MAP()

void PageConnections::OnServiceStartup( bool isStarted, Component * owner )
{
    // do nothing
}

void PageConnections::OnServiceNetwork( bool isConnected, DispatcherThread * ownerThread )
{
    if ( isConnected && (ownerThread != nullptr) )
    {
        if ( mConnectionHandler.IsValid( ) && (mConnectionHandler.GetRegistered( ) == false) && (mClientConnections == nullptr) )
        {
            mClientConnections = DEBUG_NEW ConnectionList( NECommon::COMP_NAME_CENTRAL_SERVER, *ownerThread, mConnectionHandler );
        }
    }
}

void PageConnections::OnServiceConnection( bool isConnected, DispatcherThread * ownerThread )
{
    if ( isConnected && (ownerThread != nullptr) )
    {
        ASSERT( mConnectionHandler.IsValid( ) && (mConnectionHandler.GetRegistered( ) == false) );
        const DateTime & dateTime = mConnectionHandler.GetTimeConnect();
        mClientConnections->notifyOnBroadcastClientConnected( true );
        mClientConnections->notifyOnBroadcastClientDisconnected( true );
        mClientConnections->requestRegisterConnection( mConnectionHandler.GetNickName( ), mConnectionHandler.GetCookieDirect( ), mConnectionHandler.GetConnectCookie(), dateTime.isValid() ? dateTime : DateTime::getNow() );
    }
    else
    {
        if ( mClientConnections != nullptr )
            delete mClientConnections;
        mClientConnections = nullptr;
    }
}

void PageConnections::OnClientConnection( bool isConnected, DispatcherThread *dispThread )
{
    if ( isConnected )
    {
        if ( mClientConnections == nullptr )
        {
            ASSERT( mConnectionHandler.IsValid() == true );
            ASSERT( mConnectionHandler.GetRegistered() == false );
            mClientConnections = DEBUG_NEW ConnectionList( NECommon::COMP_NAME_CENTRAL_SERVER, *dispThread, mConnectionHandler );
        }
    }
    else
    {
        if ( mClientConnections != nullptr )
        {
            mClientConnections->notifyOnBroadcastClientConnected( false );
            mClientConnections->notifyOnBroadcastClientDisconnected( false );
        }
    }
}

void PageConnections::OnClientRegistration( bool isRegistered, DispatcherThread * dispThread )
{
    TRACE_SCOPE( distrbutedapp_ui_PageConnections_OnClientRegistration );
    if ( isRegistered )
    {
        ASSERT(mClientConnections != nullptr);
        const String & nickname   = mConnectionHandler.GetNickName();
        const uint32_t cookie       = mConnectionHandler.GetCookie();

        TRACE_DBG("The client with nickName [ %s ] and cookie [ %u ] is registered with success, the existing service name is [ %s ]."
                    , nickname.getString()
                    , cookie
                    , mDirectConnectService.getString() );

        mCtrlConnections.DeleteAllItems();
        const NECommon::ListConnections & listConnections = mConnectionHandler.GetConnectionList( );
        for ( uint32_t i = 0; i < listConnections.getSize(); ++ i )
        {
            const NECommon::sConnection & connection = listConnections.getAt(i);
            addConnection( connection );
        }

        loadModel(nickname, cookie);
    }
    else
    {
        TRACE_WARN("Client [ %s ] is unregistered", mConnectionHandler.GetNickName().getString());
    }
}

void PageConnections::OnAddConnection( NEConnectionManager::sConnection & data )
{
    addConnection(data);
}

void PageConnections::OnRemoveConnection( NEConnectionManager::sConnection & data )
{
    removeConnection(data);
}

void PageConnections::OnUpdateConnection( void )
{
    // do nothing
}

void PageConnections::OnDisconnectTriggered( void )
{
    mCtrlConnections.DeleteAllItems( );
    unloadModel();
    cleanService( );
}

// PageConnections message handlers

void PageConnections::OnClickedButtonInitiateChat( )
{
    NEDirectConnection::sInitiator          initiator;
    NEDirectConnection::ListParticipants    listParticipnats;
    if ( getSelectedConnections(initiator, listParticipnats) > 1 )
        DistributedDialog::GetDialog( )->AddChatPage( initiator, listParticipnats, true );
}

BOOL PageConnections::OnInitDialog( )
{
    CPropertyPage::OnInitDialog( );

    setHeaders();
    return TRUE;  // return TRUE unless you set the focus to a control
}

void PageConnections::OnKickIdle( )
{
    UpdateDialogControls( this, FALSE );
}

void PageConnections::OnBnUpdateInitiateChat( CCmdUI* pCmdUI )
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

void PageConnections::OnDestroy( )
{
    CPropertyPage::OnDestroy( );
}

const String & PageConnections::GetRegisteredName( void ) const
{
    return mConnectionHandler.GetNickName( );
}

const uint32_t PageConnections::GetRegisteredCookie( void ) const
{
    return mConnectionHandler.GetCookie( );
}

inline void PageConnections::addConnection( const NEConnectionManager::sConnection & connection )
{
    TRACE_SCOPE( distrbutedapp_ui_PageConnections_AddConnection );
    if ( mConnectionHandler.GetNickName() != connection.nickName )
    {
        TRACE_DBG( "Adding new connection of nickName [ %s ] and cookie [ %u ]", connection.nickName.getString( ), connection.cookie );
        int pos = mCtrlConnections.GetItemCount( );
        CString nickName( connection.nickName.getString() );
        CString timeConnect( DateTime( connection.connectTime ).formatTime( ).getBuffer( ) );
        uint32_t cookie = connection.cookie;

        LVITEM lv;
        NEMemory::zeroElement<LVITEM>( lv );

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
                    , mConnectionHandler.GetNickName().getString()
                    , mConnectionHandler.GetCookie()
                    , connection.nickName.getString()
                    , connection.cookie);
    }
}

inline int PageConnections::getSelectedConnections( NEDirectConnection::sInitiator & outParticipant, NEDirectConnection::ListParticipants & outListParticipants )
{
    outListParticipants.clear();
    UINT selected = mCtrlConnections.GetSelectedCount();
    int count = 0;
    if (selected != 0)
    {
        outListParticipants.resize(selected + 1);
        DateTime now = DateTime::getNow();

        outParticipant.nickName  = mConnectionHandler.GetNickName();
        outParticipant.cookie    = mConnectionHandler.GetCookie();
        outParticipant.sessionId = now.getTime();

        outListParticipants.setAt(count ++, outParticipant);

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

            outListParticipants.setAt(count ++, participant);

        } while (pos != nullptr);
    }
    return count;
}

inline void PageConnections::setHeaders( void )
{
    int count = MACRO_ARRAYLEN( PageConnections::HEADER_TITILES );
    CRect rc( 0, 0, 0, 0 );
    mCtrlConnections.GetClientRect( &rc );
    int width = rc.Width( ) / count;

    for ( int i = 0; i < count; ++ i )
    {
        CString str( HEADER_TITILES[i] );
        LVCOLUMN lv;
        NEMemory::zeroElement<LVCOLUMN>( lv );
        lv.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
        lv.fmt = LVCFMT_LEFT;
        lv.cx = width;
        lv.pszText = str.GetBuffer( );
        lv.cchTextMax = str.GetLength( );

        mCtrlConnections.InsertColumn( i, &lv );
    }
}

inline bool PageConnections::isServiceConnected( void ) const
{
    return (mClientConnections != nullptr ? mClientConnections->isConnected( ) : false);
}

inline void PageConnections::cleanService( void )
{
    if ( mClientConnections != nullptr )
    {
        delete mClientConnections;
        mClientConnections = nullptr;
    }
    if ( mDirectConnectModel.isEmpty( ) == false )
    {
        ComponentLoader::removeComponentModel( mDirectConnectModel );
        mDirectConnectModel.clear( );
    }
}

inline int PageConnections::findConnection( const NEConnectionManager::sConnection & connection ) const
{
    int result = NECommon::INVALID_INDEX;
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

inline void PageConnections::removeConnection( const NEConnectionManager::sConnection & connection )
{
    int pos = findConnection(connection);
    if ( pos != NECommon::INVALID_INDEX )
    {
        mCtrlConnections.EnsureVisible( pos, FALSE );
        mCtrlConnections.DeleteItem( pos );
    }
}

inline void PageConnections::unloadModel( void )
{
    if ( mDirectConnectModel.isEmpty( ) == false )
    {
        ComponentLoader::removeComponentModel( mDirectConnectModel );
    }
    mDirectConnectModel.clear( );
    mDirectConnectService.clear( );
}

inline bool PageConnections::loadModel( const String & nickName, const uint32_t cookie )
{
    TRACE_SCOPE( distrbutedapp_ui_PageConnections_LoadModel );

    bool result = false;

    String serviceName = DirectConnectionService::GetGeneratedService(nickName, cookie);
    if ( (mDirectConnectService.isEmpty() == true) || (mDirectConnectService != serviceName) )
    {
        unloadModel( );
        
        NEMemory::uAlign compData;
        compData.alignClsPtr.mElement = reinterpret_cast<NEMemory::_EmptyClass *>(this);
        NERegistry::Model model = DirectConnectionService::GetModel( nickName, cookie, compData );

        TRACE_DBG("Going to load model [ %s ] with service name [ %s ]", model.getModelName().getString(), serviceName.getString() );

        if ( ComponentLoader::addModelUnique( model ) )
        {
            mDirectConnectModel     = model.getModelName( );
            mDirectConnectService   = serviceName;
            result = ComponentLoader::loadComponentModel( mDirectConnectModel );
        }
    }
    else
    {
        TRACE_WARN( "The service [ %s ] is already running, ignoring creating new model for nick name [ %s ] with cookie [ %u ]", serviceName.getString( ), nickName.getString( ), cookie );
        ASSERT( mConnectionHandler.GetNickName( ).isEmpty( ) == false );
        ASSERT( mConnectionHandler.GetCookie( ) != NEDirectConnection::InvalidCookie );
        result = true;
    }
    return result;
}

void PageConnections::OnDefaultClicked( void )
{
    CButton * btnSend = reinterpret_cast<CButton *>(GetDlgItem( IDC_BUTTON_INITIATE_CHAT ));
    if ( btnSend != nullptr )
    {
        PostMessage( WM_COMMAND, MAKEWPARAM( IDC_BUTTON_INITIATE_CHAT, BN_CLICKED ), reinterpret_cast<LPARAM>(btnSend->GetSafeHwnd( )) );
    }
}
