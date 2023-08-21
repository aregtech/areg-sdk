/************************************************************************
 * \file        service/connectivity/ServiceCommunicatonBase.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, service server connection
 ************************************************************************/
#include "service/connectivity/ServiceCommunicatonBase.hpp"

#include "service/connectivity/IEServiceMessageDispatcher.hpp"
#include "areg/ipc/NEConnection.hpp"
#include "areg/ipc/ConnectionConfiguration.hpp"
#include "areg/trace/GETrace.h"

DEF_TRACE_SCOPE(mcrouter_tcp_private_ServiceCommunicatonBase_connectServiceHost);
DEF_TRACE_SCOPE(mcrouter_tcp_private_ServiceCommunicatonBase_reconnectServiceHost);
DEF_TRACE_SCOPE(mcrouter_tcp_private_ServiceCommunicatonBase_disconnectServiceHost);
DEF_TRACE_SCOPE(mcrouter_tcp_private_ServiceCommunicatonBase_registerServiceProvider);
DEF_TRACE_SCOPE(mcrouter_tcp_private_ServiceCommunicatonBase_unregisterServiceProvider);
DEF_TRACE_SCOPE(mcrouter_tcp_private_ServiceCommunicatonBase_registerServiceConsumer);
DEF_TRACE_SCOPE(mcrouter_tcp_private_ServiceCommunicatonBase_unregisterServiceConsumer);
DEF_TRACE_SCOPE(mcrouter_tcp_private_ServiceCommunicatonBase_connectionLost);
DEF_TRACE_SCOPE(mcrouter_tcp_private_ServiceCommunicatonBase_registeredRemoteServiceProvider);
DEF_TRACE_SCOPE(mcrouter_tcp_private_ServiceCommunicatonBase_registeredRemoteServiceConsumer);
DEF_TRACE_SCOPE(mcrouter_tcp_private_ServiceCommunicatonBase_unregisteredRemoteServiceProvider);
DEF_TRACE_SCOPE(mcrouter_tcp_private_ServiceCommunicatonBase_unregisteredRemoteServiceConsumer);
DEF_TRACE_SCOPE(mcrouter_tcp_private_ServiceCommunicatonBase_processReceivedMessage);

DEF_TRACE_SCOPE(mcrouter_tcp_private_ServiceCommunicatonBase_onServiceStart);
DEF_TRACE_SCOPE(mcrouter_tcp_private_ServiceCommunicatonBase_onServiceStop);
DEF_TRACE_SCOPE(mcrouter_tcp_private_ServiceCommunicatonBase_onServiceRestart);
DEF_TRACE_SCOPE(mcrouter_tcp_private_ServiceCommunicatonBase_onServiceMessageReceived);
DEF_TRACE_SCOPE(mcrouter_tcp_private_ServiceCommunicatonBase_onServiceMessageSend);

DEF_TRACE_SCOPE(mcrouter_tcp_private_ServiceCommunicatonBase_startConnection);
DEF_TRACE_SCOPE(mcrouter_tcp_private_ServiceCommunicatonBase_restartConnection);
DEF_TRACE_SCOPE(mcrouter_tcp_private_ServiceCommunicatonBase_stopConnection);

DEF_TRACE_SCOPE(mcrouter_tcp_private_ServiceCommunicatonBase_failedSendMessage);
DEF_TRACE_SCOPE(mcrouter_tcp_private_ServiceCommunicatonBase_failedReceiveMessage);

//////////////////////////////////////////////////////////////////////////
// ServiceCommunicatonBase::ServerServiceEventConsumer class implementation
//////////////////////////////////////////////////////////////////////////

ServiceCommunicatonBase::ServerServiceEventConsumer::ServerServiceEventConsumer( ServiceCommunicatonBase & service, IEServiceMessageDispatcher& msgDispatcher)
    : IEServerServiceEventConsumer  ( )
    , mService                      ( service )
    , mDispatcher                   ( msgDispatcher )
{
}

void ServiceCommunicatonBase::ServerServiceEventConsumer::processEvent( const ServerServiceEventData & data )
{
    switch ( data.getCommand() )
    {
    case ServerServiceEventData::eServerServiceCommands::CMD_StartService:
        mService.onServiceStart( );
        break;

    case ServerServiceEventData::eServerServiceCommands::CMD_StopService:
        mService.onServiceStop( );
        break;

    case ServerServiceEventData::eServerServiceCommands::CMD_RestartService:
        mService.onServiceRestart( );
        break;

    case ServerServiceEventData::eServerServiceCommands::CMD_ServiceReceivedMsg:
        mDispatcher.onServiceMessageReceived( data.getMessage() );
        break;

    case ServerServiceEventData::eServerServiceCommands::CMD_ServiceSendMsg:
        mDispatcher.onServiceMessageSend( data.getMessage() );
        break;

    case ServerServiceEventData::eServerServiceCommands::CMD_ExitService:
        mService.onServiceStop( );
        mService.triggerExitEvent( );
        break;

    default:
        ASSERT(false);
        break;
    }
}

//////////////////////////////////////////////////////////////////////////
// ServiceCommunicatonBase::TimerConsumer class implementation
//////////////////////////////////////////////////////////////////////////

ServiceCommunicatonBase::TimerConsumer::TimerConsumer( ServiceCommunicatonBase & service )
    : IETimerConsumer   ( )
    , mService          ( service )
{
}

void ServiceCommunicatonBase::TimerConsumer::processTimer( Timer & timer )
{
    if (&timer == &mService.mTimerConnect)
    {
        mService.onTimerExpired();
    }
}

//////////////////////////////////////////////////////////////////////////
// ServiceCommunicatonBase class implementation
//////////////////////////////////////////////////////////////////////////

ServiceCommunicatonBase::ServiceCommunicatonBase(ITEM_ID serviceId, IEServiceMessageDispatcher& msgDispatcher)
    : IERemoteMessageHandler        ( )
    , IEServiceConnectionConsumer   ( )
    , IEServiceConnectionProvider   ( )
    , DispatcherThread              ( NEConnection::SERVER_DISPATCH_MESSAGE_THREAD.data( ) )
    , IEServiceConnectionHandler( )

    , mServerConnection ( serviceId )
    , mTimerConnect     ( static_cast<IETimerConsumer &>(mTimerConsumer), NEConnection::SERVER_CONNECT_TIMER_NAME.data( ) )
    , mThreadSend       ( static_cast<IERemoteMessageHandler&>(self()), mServerConnection )
    , mThreadReceive    ( static_cast<IEServiceConnectionHandler&>(self()), static_cast<IERemoteMessageHandler&>(self()), mServerConnection )
    , mIsServiceEnabled ( true )    // TODO: by default it should be disabled and enabled via init file
    , mConfigFile       ( )
    , mWhiteList        ( )
    , mBlackList        ( )
    , mEventConsumer    ( self(), msgDispatcher )
    , mTimerConsumer    ( self() )
    , mEventSendStop    ( false, false )
    , mLock             ( )
{
}

bool ServiceCommunicatonBase::setupServiceConnectionHost(const String &configFile)
{
    ConnectionConfiguration configConnect;
     if ( configConnect.loadConfiguration(configFile) )
    {
        mConfigFile             = configConnect.getConfigFileName();
        mIsServiceEnabled       = configConnect.getConnectionEnableFlag(CONNECT_TYPE);
        String hostName         = configConnect.getConnectionHost(CONNECT_TYPE);
        unsigned short hostPort = configConnect.getConnectionPort(CONNECT_TYPE);

        return mServerConnection.setAddress( hostName, hostPort );
    }
    else
    {
        mIsServiceEnabled       = NEConnection::DEFAULT_REMOVE_SERVICE_ENABLED;
        return mServerConnection.setAddress( NEConnection::DEFAULT_REMOTE_SERVICE_HOST.data( ), NEConnection::DEFAULT_REMOTE_SERVICE_PORT );
    }
}

void ServiceCommunicatonBase::applyServiceConnectionData(const String & hostName, unsigned short portNr)
{
    mServerConnection.setAddress( hostName, portNr );
}

bool ServiceCommunicatonBase::connectServiceHost(void)
{
    TRACE_SCOPE(mcrouter_tcp_private_ServiceCommunicatonBase_connectServiceHost);

    Lock lock(mLock);

    bool result = true;
    if ( mServerConnection.isValid() == false && isRunning() == false )
    {
        if ( createThread( NECommon::WAIT_INFINITE ) && waitForDispatcherStart(NECommon::WAIT_INFINITE) )
        {
            sendCommand( ServerServiceEventData::eServerServiceCommands::CMD_StartService );
        }

        TRACE_DBG( "Created remote servicing thread with [ %s ]", result ? "SUCCESS" : "FAIL" );
    }
#ifdef DEBUG
    else
    {
        ASSERT(isRunning());
    }
#endif // DEBUG

    return result;
}

bool ServiceCommunicatonBase::reconnectServiceHost(void)
{
    TRACE_SCOPE(mcrouter_tcp_private_ServiceCommunicatonBase_reconnectServiceHost);

    Lock lock(mLock);
    bool result = true;
    if (isRunning() == false)
    {
        if (createThread(NECommon::WAIT_INFINITE) && waitForDispatcherStart(NECommon::WAIT_INFINITE))
        {
            result = sendCommand( ServerServiceEventData::eServerServiceCommands::CMD_RestartService );
        }

        TRACE_DBG("Created remote servicing thread with [ %s ]", result ? "SUCCESS" : "FAIL");
    }
    else
    {
        TRACE_WARN("The servicing thread is running, restarting servicing.");
        result = sendCommand( ServerServiceEventData::eServerServiceCommands::CMD_RestartService );
    }

    return result;
}

void ServiceCommunicatonBase::disconnectServiceHost(void)
{
    TRACE_SCOPE(mcrouter_tcp_private_ServiceCommunicatonBase_disconnectServiceHost);
    if ( isRunning() )
    {
        sendCommand( ServerServiceEventData::eServerServiceCommands::CMD_ExitService, Event::eEventPriority::EventPriorityHigh );
        mEventSendStop.lock();
    }
}

bool ServiceCommunicatonBase::isServiceHostConnected(void) const
{
    Lock lock( mLock );
    return (mServerConnection.isValid() && isRunning());
}

bool ServiceCommunicatonBase::isServiceHostSetup(void) const
{
    Lock lock(mLock);
    return mServerConnection.getAddress().isValid();
}

bool ServiceCommunicatonBase::isRemoteServicingEnabled(void) const
{
    Lock lock(mLock);
    return mIsServiceEnabled;
}

void ServiceCommunicatonBase::enableRemoteServicing( bool enable )
{
    Lock lock( mLock );
    if ( isRunning( ) && (enable == false) )
    {
        disconnectServiceHost();
    }

    mIsServiceEnabled = enable;
}

bool ServiceCommunicatonBase::canAcceptConnection(const SocketAccepted & clientSocket)
{
    return clientSocket.isValid() ? canAcceptConnection( clientSocket.getAddress() ) : false;
}

void ServiceCommunicatonBase::connectionLost( SocketAccepted & clientSocket )
{
    TRACE_SCOPE(mcrouter_tcp_private_ServiceCommunicatonBase_connectionLost);
    ITEM_ID cookie = mServerConnection.getCookie(clientSocket);

    TRACE_WARN("Client lost connection: cookie [ %u ], socket [ %d ], host [ %s : %d ], closing connection"
                , static_cast<uint32_t>(cookie)
                , clientSocket.getHandle()
                , clientSocket.getAddress().getHostAddress().getString()
                , clientSocket.getAddress().getHostPort());

    mServerConnection.closeConnection(clientSocket);
    if ( cookie != NEService::COOKIE_UNKNOWN )
    {
        RemoteMessage msgDisconnect = NEConnection::createDisconnectRequest(cookie);
        sendCommunicationMessage(ServerServiceEventData::eServerServiceCommands::CMD_ServiceReceivedMsg, msgDisconnect, Event::eEventPriority::EventPriorityHigh);
    }
}

void ServiceCommunicatonBase::connectionFailure(void)
{
    if ( isServiceHostConnected())
    {
        reconnectServiceHost();
    }
}

void ServiceCommunicatonBase::onTimerExpired( void )
{
    Lock lock( mLock );
    startConnection();
}

void ServiceCommunicatonBase::onServiceStart(void)
{
    TRACE_SCOPE(mcrouter_tcp_private_ServiceCommunicatonBase_onServiceStart);
    Lock lock( mLock );
    mEventSendStop.resetEvent();
    startConnection();
}

void ServiceCommunicatonBase::onServiceStop(void)
{
    TRACE_SCOPE(mcrouter_tcp_private_ServiceCommunicatonBase_onServiceStop);

    do
    {
        Lock lock(mLock);
        stopConnection();
    } while (false);

    mThreadReceive.completionWait( NECommon::DO_NOT_WAIT);
    mThreadSend.completionWait( NECommon::DO_NOT_WAIT);
    mThreadReceive.shutdownThread();
    mThreadSend.shutdownThread();

    mEventSendStop.setEvent();
}

void ServiceCommunicatonBase::onServiceRestart( void )
{
    TRACE_SCOPE(mcrouter_tcp_private_ServiceCommunicatonBase_onServiceRestart);
    Lock lock(mLock);
    restartConnection();
}

bool ServiceCommunicatonBase::canAcceptConnection(const NESocket::SocketAddress & addrConnect) const
{
    Lock lock(mLock);
    return (isAddressInBlackList( addrConnect ) == false);
}

bool ServiceCommunicatonBase::startConnection(void)
{
    TRACE_SCOPE(mcrouter_tcp_private_ServiceCommunicatonBase_startConnection);
    TRACE_DBG("Going to start connection. Address [ %u ], port [ %d ]"
                , mServerConnection.getAddress().getHostAddress().getString()
                , mServerConnection.getAddress().getHostPort());


    ASSERT(mServerConnection.getAddress().isValid());
    ASSERT(mServerConnection.isValid() == false);
    ASSERT(mThreadReceive.isRunning() == false);
    ASSERT(mThreadSend.isRunning() == false);

    bool result = false;
    mTimerConnect.stopTimer();

    if ( mServerConnection.createSocket() )
    {
        TRACE_DBG("Created socket [ %d ], going to create send-receive threads", static_cast<uint32_t>(mServerConnection.getSocketHandle()));

        if ( mThreadReceive.createThread( NECommon::WAIT_INFINITE ) && mThreadSend.createThread( NECommon::WAIT_INFINITE ) )
        {
            VERIFY( mThreadReceive.waitForDispatcherStart( NECommon::WAIT_INFINITE ) );
            VERIFY( mThreadSend.waitForDispatcherStart( NECommon::WAIT_INFINITE ) );
            result = true;

            TRACE_DBG("The threads are created. Ready to send-receive messages.");
        }
        else
        {
            TRACE_ERR("Failed to create send-receive threads, cannot communicate. Stop remote service");
            mServerConnection.closeSocket();
        }
    }
    else
    {
        TRACE_ERR("Failed to create remote servicing socket.");
    }

    if ( result == false )
    {
        TRACE_WARN("Remote servicing failed, trigger timer with [ %u ] ms timeout to re-establish remote servicing", NEConnection::DEFAULT_RETRY_CONNECT_TIMEOUT);
        mTimerConnect.startTimer( NEConnection::DEFAULT_RETRY_CONNECT_TIMEOUT, static_cast<DispatcherThread &>(self()), 1);
    }

    return result;
}

bool ServiceCommunicatonBase::restartConnection( void )
{
    TRACE_SCOPE(mcrouter_tcp_private_ServiceCommunicatonBase_restartConnection);
    TRACE_DBG("Going to start connection. Address [ %u ], port [ %d ]"
                , mServerConnection.getAddress().getHostAddress().getString()
                , mServerConnection.getAddress().getHostPort());

    stopConnection();
    mThreadReceive.completionWait(NECommon::WAIT_INFINITE);
    mThreadSend.completionWait(NECommon::WAIT_INFINITE);
    mThreadReceive.destroyThread(NECommon::DO_NOT_WAIT);
    mThreadSend.destroyThread(NECommon::DO_NOT_WAIT);

    return startConnection();
}

void ServiceCommunicatonBase::stopConnection(void)
{
    TRACE_SCOPE(mcrouter_tcp_private_ServiceCommunicatonBase_stopConnection);
    TRACE_WARN("Stopping remote servicing connection");

    mThreadReceive.triggerExitEvent();
    mThreadReceive.destroyThread( NECommon::DO_NOT_WAIT );
    mServerConnection.disableReceive( );

    TEArrayList<StubAddress>  stubList;
    TEArrayList<ProxyAddress> proxyList;
    extractRemoteServiceAddresses(NEService::COOKIE_ANY, stubList, proxyList);

    for ( uint32_t i = 0; i < stubList.getSize(); ++ i )
    {
        unregisteredRemoteServiceProvider( stubList[i], NEService::eDisconnectReason::ReasonRouterDisconnected, NEService::COOKIE_ANY );
    }

    for ( uint32_t i = 0; i < proxyList.getSize(); ++ i )
    {
        unregisteredRemoteServiceConsumer( proxyList[i], NEService::eDisconnectReason::ReasonRouterDisconnected, NEService::COOKIE_ANY );
    }

    _disconnectService( Event::eEventPriority::EventPriorityHigh );

    mThreadSend.destroyThread( NECommon::WAIT_INFINITE );
    mServerConnection.closeSocket();
}

void ServiceCommunicatonBase::disconnectedRemoteServiceChannel(const Channel & /* channel */)
{

}

void ServiceCommunicatonBase::lostRemoteServiceChannel(const Channel & /* channel */)
{
}

void ServiceCommunicatonBase::failedSendMessage(const RemoteMessage & msgFailed, Socket & whichTarget )
{
    TRACE_SCOPE(mcrouter_tcp_private_ServiceCommunicatonBase_failedSendMessage);

#ifdef DEBUG

    ITEM_ID cookie = msgFailed.getTarget( );
    SocketAccepted client = mServerConnection.getClientByCookie( cookie );
    ASSERT( whichTarget.getHandle( ) == client.getHandle( ) );

#endif // DEBUG

    TRACE_WARN("Failed to send message to [ %s ] client [ %d ], probably the connection is lost, closing connection"
                    , whichTarget.isValid() ? "VALID" : "INVALID"
                    , static_cast<int32_t>(whichTarget.getHandle()));

    if ( whichTarget.isValid())
    {
        connectionLost( static_cast<SocketAccepted &>(whichTarget) );
    }
}

void ServiceCommunicatonBase::failedReceiveMessage(Socket & whichSource)
{
    TRACE_SCOPE(mcrouter_tcp_private_ServiceCommunicatonBase_failedReceiveMessage);

    SocketAccepted client = mServerConnection.getClientByHandle(whichSource.getHandle());
    TRACE_WARN("Failed to receive message from [ %s ] client [ %d ], probably the connection with socket [ %d ] is lost, closing connection"
                        , client.isValid() ? "VALID" : "INVALID"
                        , static_cast<int32_t>(client.getHandle())
                        , static_cast<int32_t>(whichSource.getHandle()));

    if (client.isValid())
    {
        connectionLost(client);
    }
}

void ServiceCommunicatonBase::failedProcessMessage(const RemoteMessage & /* msgUnprocessed */)
{

}

void ServiceCommunicatonBase::processReceivedMessage(const RemoteMessage & msgReceived, Socket & whichSource)
{
    TRACE_SCOPE(mcrouter_tcp_private_ServiceCommunicatonBase_processReceivedMessage);
    if ( msgReceived.isValid() )
    {
        ITEM_ID cookie = mServerConnection.getCookie(whichSource.getHandle());
        ITEM_ID source = static_cast<ITEM_ID>(msgReceived.getSource());
        ITEM_ID target = static_cast<ITEM_ID>(msgReceived.getTarget());
        NEService::eFuncIdRange msgId  = static_cast<NEService::eFuncIdRange>( msgReceived.getMessageId() );

        TRACE_DBG("Received message [ %s ] of id [ 0x%X ] from source [ %u ] ( connection cookie = %u ) of client host [ %s : %d ] for target [ %u ]"
                        , NEService::getString(msgId)
                        , static_cast<uint32_t>(msgId)
                        , static_cast<uint32_t>(source)
                        , static_cast<uint32_t>(cookie)
                        , static_cast<const char *>(whichSource.getAddress().getHostAddress())
                        , static_cast<int>(whichSource.getAddress().getHostPort( ))
                        , static_cast<id_type>(target));

        if ( (source > NEService::COOKIE_ROUTER) && NEService::isExecutableId(static_cast<uint32_t>(msgId)) )
        {
            TRACE_DBG("Forwarding message [ 0x%X ] to send to target [ %u ]", static_cast<uint32_t>(msgId), static_cast<uint32_t>(target));
            if ( target != NEService::TARGET_UNKNOWN )
            {
                _sendMessage(msgReceived);
            }
        }
        else if ( (source == cookie) && (msgId != NEService::eFuncIdRange::ServiceRouterConnect) )
        {
            TRACE_DBG("Going to process received message [ 0x%X ]", static_cast<uint32_t>(msgId));
            sendCommunicationMessage( ServerServiceEventData::eServerServiceCommands::CMD_ServiceReceivedMsg, msgReceived );
        }
        else if ( (source == NEService::SOURCE_UNKNOWN) && (msgId == NEService::eFuncIdRange::ServiceRouterConnect) )
        {
            RemoteMessage msgConnect = NEConnection::createConnectNotify(cookie);
            TRACE_DBG("Received request connect message, sending response [ %s ] of id [ 0x%X ], to new target [ %u ], connection socket [ %u ], checksum [ %u ]"
                        , NEService::getString( static_cast<NEService::eFuncIdRange>(msgConnect.getMessageId()))
                        , static_cast<uint32_t>(msgConnect.getMessageId())
                        , static_cast<uint32_t>(msgConnect.getTarget())
                        , static_cast<uint32_t>(whichSource.getHandle())
                        , msgConnect.getChecksum());

            _sendMessage( msgConnect );
        }
        else
        {
            TRACE_WARN("Ignoring to process message [ %s ] of id [ 0x%X ] from source [ %u ]"
                        , NEService::getString(msgId)
                        , static_cast<uint32_t>(msgId)
                        , static_cast<uint32_t>(source));
            ASSERT(false);
        }
    }
    else
    {
        TRACE_WARN("Received invalid message from source [ %u ], ignoring to process", static_cast<uint32_t>(msgReceived.getSource()));
    }
}

bool ServiceCommunicatonBase::runDispatcher(void)
{
    startEventListener( );

    bool result = DispatcherThread::runDispatcher();

    stopEventListener( );

    return result;
}

bool ServiceCommunicatonBase::postEvent(Event & eventElem)
{
    return EventDispatcher::postEvent(eventElem);
}

inline bool ServiceCommunicatonBase::_sendMessage( const RemoteMessage & data, Event::eEventPriority eventPrio /*= Event::eEventPriority::EventPriorityNormal*/ )
{
    return SendMessageEvent::sendEvent( SendMessageEventData( data )
                                        , static_cast<IESendMessageEventConsumer &>(mThreadSend)
                                        , static_cast<DispatcherThread &>(mThreadSend)
                                        , eventPrio );
}

inline void ServiceCommunicatonBase::_disconnectService( Event::eEventPriority eventPrio )
{
    SendMessageEvent::sendEvent( SendMessageEventData( false )
                                 , static_cast<IESendMessageEventConsumer &>(mThreadSend)
                                 , static_cast<DispatcherThread &>(mThreadSend)
                                 , eventPrio );
}
