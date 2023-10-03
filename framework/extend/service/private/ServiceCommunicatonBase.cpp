/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        extend/service/private/ServiceCommunicatonBase.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Service communication base class
 ************************************************************************/
#include "extend/service/ServiceCommunicatonBase.hpp"

#include "areg/ipc/NEConnection.hpp"
#include "areg/ipc/ConnectionConfiguration.hpp"
#include "areg/trace/GETrace.h"

DEF_TRACE_SCOPE(areg_extend_service_ServiceCommunicatonBase_connectServiceHost);
DEF_TRACE_SCOPE(areg_extend_service_ServiceCommunicatonBase_reconnectServiceHost);
DEF_TRACE_SCOPE(areg_extend_service_ServiceCommunicatonBase_disconnectServiceHost);
DEF_TRACE_SCOPE(areg_extend_service_ServiceCommunicatonBase_connectionLost);
DEF_TRACE_SCOPE(areg_extend_service_ServiceCommunicatonBase_processReceivedMessage);

DEF_TRACE_SCOPE(areg_extend_service_ServiceCommunicatonBase_onServiceStart);
DEF_TRACE_SCOPE(areg_extend_service_ServiceCommunicatonBase_onServiceStop);
DEF_TRACE_SCOPE(areg_extend_service_ServiceCommunicatonBase_onServiceRestart);
DEF_TRACE_SCOPE(areg_extend_service_ServiceCommunicatonBase_onServiceExit);

DEF_TRACE_SCOPE(areg_extend_service_ServiceCommunicatonBase_startConnection);
DEF_TRACE_SCOPE(areg_extend_service_ServiceCommunicatonBase_restartConnection);
DEF_TRACE_SCOPE(areg_extend_service_ServiceCommunicatonBase_stopConnection);

DEF_TRACE_SCOPE(areg_extend_service_ServiceCommunicatonBase_failedSendMessage);
DEF_TRACE_SCOPE(areg_extend_service_ServiceCommunicatonBase_failedReceiveMessage);

//////////////////////////////////////////////////////////////////////////
// ServiceCommunicatonBase class implementation
//////////////////////////////////////////////////////////////////////////

ServiceCommunicatonBase::ServiceCommunicatonBase( const ITEM_ID & serviceId
                                                , const String & dispatcher
                                                , ServiceCommunicatonBase::eConnectionBehavior behavior /*= ServiceCommunicatonBase::eConnectionBehavior::DefaultAccept*/ )
    : IERemoteMessageHandler        ( )
    , IEServiceConnectionConsumer   ( )
    , IEServiceConnectionProvider   ( )
    , DispatcherThread              ( dispatcher )
    , IEServiceEventConsumerBase    ( )
    , IEServiceConnectionHandler    ( )

    , mConnectBehavior  ( behavior )
    , mServerConnection ( serviceId )
    , mTimerConnect     ( static_cast<IETimerConsumer &>(mTimerConsumer), NEConnection::SERVER_CONNECT_TIMER_NAME.data( ) )
    , mThreadSend       ( static_cast<IERemoteMessageHandler&>(self()), mServerConnection )
    , mThreadReceive    ( static_cast<IEServiceConnectionHandler&>(self()), static_cast<IERemoteMessageHandler&>(self()), mServerConnection )
    , mIsServiceEnabled ( true )    // TODO: by default it should be disabled and enabled via init file
    , mConfigFile       ( )
    , mWhiteList        ( )
    , mBlackList        ( )
    , mEventConsumer    ( self() )
    , mTimerConsumer    ( self() )
    , mInstanceMap      (  )
    , mEventSendStop    ( false, false )
    , mLock             ( )
{
}

void ServiceCommunicatonBase::addInstance(const ITEM_ID & cookie, const sConnectedInstance & instance)
{
    Lock lock(mLock);
    mInstanceMap.addIfUnique(cookie, instance);
}

void ServiceCommunicatonBase::removeInstance(const ITEM_ID & cookie)
{
    Lock lock(mLock);
    mInstanceMap.removeAt(cookie);
}

void ServiceCommunicatonBase::removeAllInstances(void)
{
    Lock lock(mLock);
    mInstanceMap.release();
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
        mIsServiceEnabled       = NEConnection::DEFAULT_REMOTE_SERVICE_ENABLED;
        return mServerConnection.setAddress( NEConnection::DEFAULT_REMOTE_SERVICE_HOST.data( ), NEConnection::DEFAULT_REMOTE_SERVICE_PORT );
    }
}

void ServiceCommunicatonBase::applyServiceConnectionData(const String & hostName, unsigned short portNr)
{
    mServerConnection.setAddress( hostName, portNr );
}

bool ServiceCommunicatonBase::connectServiceHost(void)
{
    TRACE_SCOPE(areg_extend_service_ServiceCommunicatonBase_connectServiceHost);

    Lock lock(mLock);

    bool result{ false };
    if ( mServerConnection.isValid() == false && isRunning() == false )
    {
        if ( createThread( NECommon::WAIT_INFINITE ) && waitForDispatcherStart(NECommon::WAIT_INFINITE) )
        {
            result = true;
            sendCommand( ServiceEventData::eServiceEventCommands::CMD_StartService );
        }

        TRACE_DBG( "Created remote servicing thread with [ %s ]", result ? "SUCCESS" : "FAIL" );
    }
    else
    {
        result = isRunning( ) && mServerConnection.isValid( );
        ASSERT(isRunning());
    }

    return result;
}

bool ServiceCommunicatonBase::reconnectServiceHost(void)
{
    TRACE_SCOPE(areg_extend_service_ServiceCommunicatonBase_reconnectServiceHost);

    Lock lock(mLock);
    bool result = true;
    if (isRunning() == false)
    {
        if (createThread(NECommon::WAIT_INFINITE) && waitForDispatcherStart(NECommon::WAIT_INFINITE))
        {
            result = sendCommand( ServiceEventData::eServiceEventCommands::CMD_RestartService );
        }

        TRACE_DBG("Created remote servicing thread with [ %s ]", result ? "SUCCESS" : "FAIL");
    }
    else
    {
        TRACE_WARN("The servicing thread is running, restarting servicing.");
        result = sendCommand( ServiceEventData::eServiceEventCommands::CMD_RestartService );
    }

    return result;
}

void ServiceCommunicatonBase::disconnectServiceHost(void)
{
    TRACE_SCOPE(areg_extend_service_ServiceCommunicatonBase_disconnectServiceHost);
    if ( isRunning() )
    {
        sendCommand( ServiceEventData::eServiceEventCommands::CMD_ServiceExit, Event::eEventPriority::EventPriorityHigh );
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
    bool result{ false };
    if ( clientSocket.isValid( ) && clientSocket.isAlive() )
    {
        Lock lock(mLock);
        const String & ipAddress = clientSocket.getAddress( ).getHostAddress( );
        result =  ((mConnectBehavior == eConnectionBehavior::DefaultAccept) && (mBlackList.contains( ipAddress ) == false)) ||
                  ((mConnectBehavior == eConnectionBehavior::DefaultReject) && (mWhiteList.contains( ipAddress ) == true ));

    }

    return result;
}

void ServiceCommunicatonBase::connectionLost( SocketAccepted & clientSocket )
{
    TRACE_SCOPE(areg_extend_service_ServiceCommunicatonBase_connectionLost);
    const ITEM_ID & cookie { mServerConnection.getCookie(clientSocket) };
    const ITEM_ID & channel{ mServerConnection.getChannelId() };

    TRACE_WARN("Client lost connection: cookie [ %u ], socket [ %d ], host [ %s : %d ], closing connection"
                , static_cast<uint32_t>(cookie)
                , clientSocket.getHandle()
                , clientSocket.getAddress().getHostAddress().getString()
                , clientSocket.getAddress().getHostPort());

    mServerConnection.closeConnection(clientSocket);
    if ( cookie != NEService::COOKIE_UNKNOWN )
    {
        removeInstance(cookie);
        RemoteMessage msgDisconnect = NEConnection::createDisconnectRequest(cookie, channel);
        sendCommunicationMessage(ServiceEventData::eServiceEventCommands::CMD_ServiceReceivedMsg, msgDisconnect, Event::eEventPriority::EventPriorityNormal);
    }
}

void ServiceCommunicatonBase::connectionFailure(void)
{
    if ( isServiceHostConnected())
    {
        reconnectServiceHost();
    }
}

void ServiceCommunicatonBase::disconnectServices( void )
{
    removeAllInstances();
}

void ServiceCommunicatonBase::onServiceReconnectTimerExpired( void )
{
    Lock lock( mLock );
    startConnection( );
}

void ServiceCommunicatonBase::onServiceStart(void)
{
    TRACE_SCOPE(areg_extend_service_ServiceCommunicatonBase_onServiceStart);
    Lock lock( mLock );
    mEventSendStop.resetEvent();
    startConnection();
}

void ServiceCommunicatonBase::onServiceStop(void)
{
    TRACE_SCOPE(areg_extend_service_ServiceCommunicatonBase_onServiceStop);

    do
    {
        Lock lock(mLock);
        stopConnection();
    } while (false);

    mEventSendStop.setEvent();
}

void ServiceCommunicatonBase::onServiceRestart( void )
{
    TRACE_SCOPE(areg_extend_service_ServiceCommunicatonBase_onServiceRestart);
    restartConnection();
}

void ServiceCommunicatonBase::onServiceExit( void )
{
    TRACE_SCOPE( areg_extend_service_ServiceCommunicatonBase_onServiceExit );
    onServiceStop( );
    triggerExit( );
}

bool ServiceCommunicatonBase::startConnection(void)
{
    TRACE_SCOPE(areg_extend_service_ServiceCommunicatonBase_startConnection);
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
        if ( startSendThread( ) && startReceiveThread( ) )
        {
            result = true;
            TRACE_DBG( "The threads are created. Ready to send-receive messages." );
        }
        else
        {
            TRACE_ERR( "Failed to create send-receive threads, cannot communicate. Stop remote service" );
            mServerConnection.closeSocket( );
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
    TRACE_SCOPE(areg_extend_service_ServiceCommunicatonBase_restartConnection);
    TRACE_DBG("Going to start connection. Address [ %u ], port [ %d ]"
                , mServerConnection.getAddress().getHostAddress().getString()
                , mServerConnection.getAddress().getHostPort());

    stopConnection();
    return startConnection();
}

void ServiceCommunicatonBase::stopConnection(void)
{
    TRACE_SCOPE(areg_extend_service_ServiceCommunicatonBase_stopConnection);
    TRACE_WARN("Stopping remote servicing connection");

    mThreadReceive.triggerExit();
    mServerConnection.disableReceive( );

    disconnectServices( );
    disconnectService( Event::eEventPriority::EventPriorityNormal );

    // Wait without triggering exit.
    mThreadSend.completionWait( NECommon::WAIT_INFINITE );
    mServerConnection.closeSocket( );
    // Trigger exit and clean resources.
    mThreadSend.shutdownThread( NECommon::WAIT_INFINITE );
    mThreadReceive.shutdownThread( NECommon::WAIT_INFINITE );
}

bool ServiceCommunicatonBase::startSendThread( void )
{
    return mThreadSend.createThread( NECommon::WAIT_INFINITE ) && 
           mThreadSend.waitForDispatcherStart( NECommon::WAIT_INFINITE );
}

bool ServiceCommunicatonBase::startReceiveThread( void )
{
    return mThreadReceive.createThread( NECommon::WAIT_INFINITE ) &&
           mThreadReceive.waitForDispatcherStart( NECommon::WAIT_INFINITE );
}

void ServiceCommunicatonBase::failedSendMessage(const RemoteMessage & msgFailed, Socket & whichTarget )
{
    TRACE_SCOPE(areg_extend_service_ServiceCommunicatonBase_failedSendMessage);

#ifdef DEBUG

    const ITEM_ID & cookie = msgFailed.getTarget( );
    SocketAccepted client = mServerConnection.getClientByCookie( cookie );
    ASSERT( (client.isValid() == false) || (whichTarget.getHandle( ) == client.getHandle( )) );

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
    TRACE_SCOPE(areg_extend_service_ServiceCommunicatonBase_failedReceiveMessage);

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

void ServiceCommunicatonBase::processReceivedMessage(const RemoteMessage & msgReceived, Socket & whichSource)
{
    TRACE_SCOPE(areg_extend_service_ServiceCommunicatonBase_processReceivedMessage);
    if ( msgReceived.isValid() )
    {
        const ITEM_ID & cookie = mServerConnection.getCookie(whichSource.getHandle());
        const ITEM_ID & source = msgReceived.getSource();
        const ITEM_ID & target = msgReceived.getTarget();
        NEService::eFuncIdRange msgId  = static_cast<NEService::eFuncIdRange>( msgReceived.getMessageId() );

        TRACE_DBG("Received message [ %s ] of id [ 0x%X ] from source [ %u ] ( connection cookie = %u ) of client host [ %s : %d ] for target [ %u ]"
                        , NEService::getString(msgId)
                        , static_cast<uint32_t>(msgId)
                        , static_cast<uint32_t>(source)
                        , static_cast<uint32_t>(cookie)
                        , static_cast<const char *>(whichSource.getAddress().getHostAddress())
                        , static_cast<int>(whichSource.getAddress().getHostPort( ))
                        , static_cast<id_type>(target));

        if ( (source >= NEService::COOKIE_REMOTE_SERVICE) && NEService::isExecutableId(static_cast<uint32_t>(msgId)) )
        {
            TRACE_DBG("Forwarding message [ 0x%X ] to send to target [ %u ]", static_cast<uint32_t>(msgId), static_cast<uint32_t>(target));
            if ( target != NEService::TARGET_UNKNOWN )
            {
                sendMessage(msgReceived);
            }
        }
        else if ( (source == cookie) && (msgId != NEService::eFuncIdRange::SystemServiceConnect) )
        {
            TRACE_DBG("Going to process received message [ 0x%X ]", static_cast<uint32_t>(msgId));
            if ( msgId == NEService::eFuncIdRange::SystemServiceDisconnect )
            {
                removeInstance( cookie );
            }

            sendCommunicationMessage( ServiceEventData::eServiceEventCommands::CMD_ServiceReceivedMsg, msgReceived );
        }
        else if ( (source == NEService::SOURCE_UNKNOWN) && (msgId == NEService::eFuncIdRange::SystemServiceConnect) )
        {
            sConnectedInstance instance{};
            msgReceived >> instance.ciSource;
            msgReceived >> instance.ciInstance;
            addInstance(cookie, instance);
            RemoteMessage msgConnect(createServiceConnectMessage(mServerConnection.getChannelId(), cookie));
            TRACE_DBG("Received request connect message, sending response [ %s ] of id [ 0x%X ], to new target [ %u ], connection socket [ %u ], checksum [ %u ]"
                        , NEService::getString( static_cast<NEService::eFuncIdRange>(msgConnect.getMessageId()))
                        , static_cast<uint32_t>(msgConnect.getMessageId())
                        , static_cast<uint32_t>(msgConnect.getTarget())
                        , static_cast<uint32_t>(whichSource.getHandle())
                        , msgConnect.getChecksum());

            sendMessage( msgConnect );
        }
        else
        {
            TRACE_WARN("Ignoring to process message [ %s ] of id [ 0x%X ] from source [ %u ]"
                        , NEService::getString(msgId)
                        , static_cast<uint32_t>(msgId)
                        , static_cast<uint32_t>(source));
        }
    }
    else
    {
        TRACE_WARN("Received invalid message from source [ %u ], ignoring to process", static_cast<uint32_t>(msgReceived.getSource()));
    }
}

void ServiceCommunicatonBase::readyForEvents( bool isReady )
{
    if ( isReady )
    {
        ServiceServerEvent::addListener( static_cast<IEServiceServerEventConsumer &>(mEventConsumer), static_cast<DispatcherThread &>(self( )) );
        DispatcherThread::readyForEvents( true );
    }
    else
    {
        DispatcherThread::readyForEvents( false );
        ServiceServerEvent::removeListener( static_cast<IEServiceServerEventConsumer &>(mEventConsumer), static_cast<DispatcherThread &>(self( )) );
    }
}

bool ServiceCommunicatonBase::postEvent( Event & eventElem )
{
    return EventDispatcher::postEvent( eventElem );
}

RemoteMessage ServiceCommunicatonBase::createServiceConnectMessage(const ITEM_ID & source, const ITEM_ID & target) const
{
    RemoteMessage result{ NEConnection::createConnectNotify(source, target) };
    result.moveToEnd();
    result << NEService::eMessageSource::MessageSourceService;
    return result;
}

RemoteMessage ServiceCommunicatonBase::createServiceDisconnectMessage( const ITEM_ID & source, const ITEM_ID & target ) const
{
    return NEConnection::createDisconnectNotify(source, target);
}
