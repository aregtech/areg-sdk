// chatter/PageConnections.cpp : implementation file
//

#include "chatter/res/stdafx.h"
#include "chatter/ui/PageConnections.hpp"
#include "chatter/ui/PageChat.hpp"
#include "chatter/ui/DistributedDialog.hpp"

#include "chatter/services/DirectConnectionService.hpp"
#include "chatter/services/ConnectionList.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/base/areg_global.h"
#include "areg/logging/areg_log.h"
#include "common/ChatDefs.hpp"

DEF_LOG_SCOPE( chatter_ui_PageConnections_OnClientRegistration );
DEF_LOG_SCOPE( chatter_ui_PageConnections_LoadModel );
DEF_LOG_SCOPE( chatter_ui_PageConnections_AddConnection );
DEF_LOG_SCOPE( chatter_ui_PageConnections_OnServiceConnection );
DEF_LOG_SCOPE( chatter_ui_PageConnections_OnServiceNetwork );
DEF_LOG_SCOPE( chatter_ui_PageConnections_OnClientConnection );

// PageConnections dialog

IMPLEMENT_DYNAMIC(PageConnections, CPropertyPage)

LPCTSTR PageConnections::HEADER_TITILES[] = 
{
      _T("Nick Name:")
    , _T("Connected at...")
};

PageConnections::PageConnections(aregext::ConnectionHandler & handlerConnection)
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
    ON_MESSAGE_VOID( WM_KICKIDLE, PageConnections::OnKickIdle )
    ON_UPDATE_COMMAND_UI( IDC_BUTTON_INITIATE_CHAT, &PageConnections::OnBnUpdateInitiateChat )
    ON_WM_DESTROY( )
END_MESSAGE_MAP()

void PageConnections::OnServiceStartup( bool /*isStarted*/, areg::Component* /*owner*/)
{
    // do nothing
}

void PageConnections::OnServiceNetwork( bool isConnected, areg::DispatcherThread * ownerThread )
{
    LOG_SCOPE(chatter_ui_PageConnections_OnServiceNetwork);
#if AREG_LOGGING
    uint32_t cookie = mConnectionHandler.GetCookie();
    const areg::String& nickName = mConnectionHandler.GetNickName();
    LOG_DBG("Handling network service: is [ %s ], owning thread [ %s ], connection handler [ %s ] (cookie = %s, nick name = %s), connection handler [ %s ], the connection SI [ %s ] ..."
                , isConnected ? "CONNECTED" : "DISCONNECTED"
                , ownerThread != nullptr ? "VALID" : "NULL"
                , mConnectionHandler.IsValid() ? "VALID" : "INVALID"
                , cookie != ConnectionManager::InvalidCookie ? areg::String::make_string(cookie).as_string() : "Invalid cookie"
                , nickName.as_string()
                , mConnectionHandler.GetRegistered() ? "REGISTERED" : "NOT REGISTERED"
                , mClientConnections != nullptr ? mClientConnections->service_name().as_string() : "NULL");
#endif  // AREG_LOGGING

    if ( isConnected && (ownerThread != nullptr) )
    {
        if ( mConnectionHandler.IsValid( ) && (mConnectionHandler.GetRegistered( ) == false) && (mClientConnections == nullptr) )
        {
            LOG_DBG("Create client object to get connections");
            mClientConnections = DEBUG_NEW ConnectionList( chat::COMP_NAME_CENTRAL_SERVER, *ownerThread, mConnectionHandler );
        }
    }
}

void PageConnections::OnServiceConnection( bool isConnected, areg::DispatcherThread * ownerThread )
{
    LOG_SCOPE(chatter_ui_PageConnections_OnServiceConnection);
    LOG_DBG("[ %s ] to the service", isConnected ? "CONNECTED" : "DISCONNECTED");

    if ( isConnected && (ownerThread != nullptr) )
    {
        LOG_DBG("Sends request to register the connection");
        ASSERT( mConnectionHandler.IsValid( ) && (mConnectionHandler.GetRegistered( ) == false) );
        const areg::DateTime & dateTime = mConnectionHandler.GetTimeConnect();
        mClientConnections->notifyOnBroadcastClientConnected( true );
        mClientConnections->notifyOnBroadcastClientDisconnected( true );
        mClientConnections->requestRegisterConnection( mConnectionHandler.GetNickName( ), mConnectionHandler.GetCookieDirect( ), mConnectionHandler.GetConnectCookie(), dateTime.is_valid() ? dateTime : areg::DateTime::now() );
    }
    else
    {
        if ( mClientConnections != nullptr )
            delete mClientConnections;
        mClientConnections = nullptr;
    }
}

void PageConnections::OnClientConnection( bool isConnected, areg::DispatcherThread *dispThread )
{
    LOG_SCOPE(chatter_ui_PageConnections_OnClientConnection);
    LOG_DBG("A client is [ %s ]", isConnected ? "CONNECTED" : "DISCONNECTED");

    if ( isConnected )
    {
        if ( mClientConnections == nullptr )
        {
            LOG_DBG("There is no connection client, creates one");
            ASSERT( mConnectionHandler.IsValid() == true );
            ASSERT( mConnectionHandler.GetRegistered() == false );
            mClientConnections = DEBUG_NEW ConnectionList( chat::COMP_NAME_CENTRAL_SERVER, *dispThread, mConnectionHandler );
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

void PageConnections::OnClientRegistration( bool isRegistered, areg::DispatcherThread * /*dispThread*/)
{
    LOG_SCOPE( chatter_ui_PageConnections_OnClientRegistration );
    if ( isRegistered )
    {
        ASSERT(mClientConnections != nullptr);
        const areg::String & nickname   = mConnectionHandler.GetNickName();
        const uint32_t cookie       = mConnectionHandler.GetCookie();

        LOG_DBG("The client with nickName [ %s ] and cookie [ %u ] is registered with success, the existing service name is [ %s ]."
                    , nickname.as_string()
                    , cookie
                    , mDirectConnectService.as_string() );

        ASSERT(::IsWindow(mCtrlConnections.GetSafeHwnd()));

        mCtrlConnections.DeleteAllItems();
        const chat::ListConnections & listConnections = mConnectionHandler.GetConnectionList( );
        for ( uint32_t i = 0; i < listConnections.size(); ++ i )
        {
            const chat::ConnectionRecord & connection = listConnections.value_at(i);
            addConnection( connection );
        }

        load_model(nickname, cookie);
    }
    else
    {
        LOG_WARN("Client [ %s ] is unregistered", mConnectionHandler.GetNickName().as_string());
    }
}

void PageConnections::OnAddConnection( ConnectionManager::ConnectionRecord & data )
{
    addConnection(data);
}

void PageConnections::OnRemoveConnection( ConnectionManager::ConnectionRecord & data )
{
    removeConnection(data);
}

void PageConnections::OnUpdateConnection()
{
    // do nothing
}

void PageConnections::OnDisconnectTriggered()
{
    mCtrlConnections.DeleteAllItems( );
    unload_model();
    cleanService( );
}

// PageConnections message handlers

void PageConnections::OnClickedButtonInitiateChat( )
{
    DirectConnection::sInitiator          initiator;
    DirectConnection::ListParticipants    listParticipnats;
    if ( getSelectedConnections(initiator, listParticipnats) > 1 )
        DistributedDialog::GetDialog( )->AddChatPage( initiator, listParticipnats, true );
}

BOOL PageConnections::OnInitDialog( )
{
    CPropertyPage::OnInitDialog( );

    ASSERT(::IsWindow(mCtrlConnections.GetSafeHwnd()));
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

const areg::String & PageConnections::GetRegisteredName() const
{
    return mConnectionHandler.GetNickName( );
}

uint32_t PageConnections::GetRegisteredCookie() const
{
    return mConnectionHandler.GetCookie( );
}

inline void PageConnections::addConnection( const ConnectionManager::ConnectionRecord & connection )
{
    LOG_SCOPE( chatter_ui_PageConnections_AddConnection );
    if ( mConnectionHandler.GetNickName() != connection.nickName )
    {
        LOG_DBG( "Adding new connection of nickName [ %s ] and cookie [ %u ]", connection.nickName.as_string( ), connection.cookie );
        int32_t pos = mCtrlConnections.GetItemCount( );
        CString nickName( connection.nickName.as_string() );
        CString timeConnect( areg::DateTime( connection.connectTime ).format_time( ).buffer( ) );
        uint32_t cookie = connection.cookie;

        LVITEM lv;
        areg::zeroElement<LVITEM>( lv );

        // Column nickname
        lv.mask = LVIF_TEXT | LVIF_PARAM;
        lv.iItem = pos;
        lv.iSubItem = 0;
        lv.pszText = nickName.GetBuffer( );
        lv.lParam = cookie;
        lv.cchTextMax = ConnectionManager::NicknameMaxLen;
        mCtrlConnections.InsertItem( &lv );
        mCtrlConnections.SetItemText( pos, 1, timeConnect.GetBuffer( ) );
        mCtrlConnections.EnsureVisible( pos, FALSE );
    }
    else
    {
        LOG_WARN("Ignoring adding new connection. Existing connection nickName [ %s ] cookie [ %u ], add connection nickaName [ %s ] cookie [ %u ]"
                    , mConnectionHandler.GetNickName().as_string()
                    , mConnectionHandler.GetCookie()
                    , connection.nickName.as_string()
                    , connection.cookie);
    }
}

inline int32_t PageConnections::getSelectedConnections( DirectConnection::sInitiator & outParticipant, DirectConnection::ListParticipants & outListParticipants )
{
    outListParticipants.clear();
    UINT selected = mCtrlConnections.GetSelectedCount();
    int32_t count = 0;
    if (selected != 0)
    {
        outListParticipants.resize(selected + 1);
        areg::DateTime now = areg::DateTime::now();

        outParticipant.nickName  = mConnectionHandler.GetNickName();
        outParticipant.cookie    = mConnectionHandler.GetCookie();
        outParticipant.sessionId = now.time();

        outListParticipants.set_value_at(count ++, outParticipant);

        POSITION pos = mCtrlConnections.GetFirstSelectedItemPosition();
        do
        {
            int32_t i = mCtrlConnections.GetNextSelectedItem(pos);
            ASSERT( i != -1 );
            CString nickName= mCtrlConnections.GetItemText(i, 0);
            LPARAM cookie   = mCtrlConnections.GetItemData(i);

            DirectConnection::Participant participant;
            participant.nickName    = static_cast<LPCTSTR>(nickName);
            participant.cookie      = static_cast<uint32_t>(cookie);
            participant.sessionId   = static_cast<uint64_t>(now);

            outListParticipants.set_value_at(count ++, participant);

        } while (pos != nullptr);
    }
    return count;
}

inline void PageConnections::setHeaders()
{
    int32_t count = std::size( PageConnections::HEADER_TITILES );
    CRect rc( 0, 0, 0, 0 );
    mCtrlConnections.GetClientRect( &rc );
    int32_t width = rc.Width( ) / count;

    for ( int i = 0; i < count; ++ i )
    {
        CString str( HEADER_TITILES[i] );
        LVCOLUMN lv;
        areg::zeroElement<LVCOLUMN>( lv );
        lv.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
        lv.fmt = LVCFMT_LEFT;
        lv.cx = width;
        lv.pszText = str.GetBuffer( );
        lv.cchTextMax = str.GetLength( );

        mCtrlConnections.InsertColumn( i, &lv );
    }
}

inline bool PageConnections::is_service_connected() const
{
    return (mClientConnections != nullptr ? mClientConnections->is_connected( ) : false);
}

inline void PageConnections::cleanService()
{
    if ( mClientConnections != nullptr )
    {
        delete mClientConnections;
        mClientConnections = nullptr;
    }
    if ( mDirectConnectModel.is_empty( ) == false )
    {
        areg::ComponentLoader::remove_component_model( mDirectConnectModel );
        mDirectConnectModel.clear( );
    }
}

inline int32_t PageConnections::findConnection( const ConnectionManager::ConnectionRecord & connection ) const
{
    int32_t result = areg::INVALID_INDEX;
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

inline void PageConnections::removeConnection( const ConnectionManager::ConnectionRecord & connection )
{
    int32_t pos = findConnection(connection);
    if ( pos != areg::INVALID_INDEX )
    {
        mCtrlConnections.EnsureVisible( pos, FALSE );
        mCtrlConnections.DeleteItem( pos );
    }
}

inline void PageConnections::unload_model()
{
    if ( mDirectConnectModel.is_empty( ) == false )
    {
        areg::ComponentLoader::remove_component_model( mDirectConnectModel );
    }
    mDirectConnectModel.clear( );
    mDirectConnectService.clear( );
}

inline bool PageConnections::load_model( const areg::String & nickName, const uint32_t cookie )
{
    LOG_SCOPE( chatter_ui_PageConnections_LoadModel );

    bool result = false;

    areg::String serviceName = DirectConnectionService::GetGeneratedService(nickName, cookie);
    if ( (mDirectConnectService.is_empty() == true) || (mDirectConnectService != serviceName) )
    {
        unload_model( );
        
        std::any data = std::make_any< PageConnections *>(this);
        areg::Model model = DirectConnectionService::GetModel( nickName, cookie, data );

        LOG_DBG("Going to load model [ %s ] with service name [ %s ]", model.model_name().as_string(), serviceName.as_string() );

        if ( areg::ComponentLoader::add_model_unique( model ) )
        {
            mDirectConnectModel     = model.model_name( );
            mDirectConnectService   = serviceName;
            result = areg::ComponentLoader::load_component_model( mDirectConnectModel );
        }
    }
    else
    {
        LOG_WARN( "The service [ %s ] is already running, ignoring creating new model for nick name [ %s ] with cookie [ %u ]", serviceName.as_string( ), nickName.as_string( ), cookie );
        ASSERT( mConnectionHandler.GetNickName( ).is_empty( ) == false );
        ASSERT( mConnectionHandler.GetCookie( ) != DirectConnection::InvalidCookie );
        result = true;
    }
    return result;
}

void PageConnections::OnDefaultClicked()
{
    CButton * btnSend = reinterpret_cast<CButton *>(GetDlgItem( IDC_BUTTON_INITIATE_CHAT ));
    if ( btnSend != nullptr )
    {
        PostMessage( WM_COMMAND, MAKEWPARAM( IDC_BUTTON_INITIATE_CHAT, BN_CLICKED ), reinterpret_cast<LPARAM>(btnSend->GetSafeHwnd( )) );
    }
}
