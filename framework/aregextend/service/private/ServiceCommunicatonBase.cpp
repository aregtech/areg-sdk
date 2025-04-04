/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        aregextend/service/private/ServiceCommunicatonBase.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Service communication base class
 ************************************************************************/
#include "aregextend/service/ServiceCommunicatonBase.hpp"

#include "areg/base/DateTime.hpp"
#include "areg/ipc/NERemoteService.hpp"
#include "areg/ipc/ConnectionConfiguration.hpp"
#include "areg/ipc/private/NEConnection.hpp"
#include "areg/logging/GELog.h"

#include "aregextend/service/NESystemService.hpp"

DEF_LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase_connectServiceHost);
DEF_LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase_reconnectServiceHost);
DEF_LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase_disconnectServiceHost);
DEF_LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase_connectionLost);
DEF_LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase_processReceivedMessage);

DEF_LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase_onServiceStart);
DEF_LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase_onServiceStop);
DEF_LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase_onServiceRestart);
DEF_LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase_onServiceExit);

DEF_LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase_startConnection);
DEF_LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase_restartConnection);
DEF_LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase_stopConnection);

DEF_LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase_failedSendMessage);
DEF_LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase_failedReceiveMessage);

//////////////////////////////////////////////////////////////////////////
// ServiceCommunicatonBase class implementation
//////////////////////////////////////////////////////////////////////////

ServiceCommunicatonBase::ServiceCommunicatonBase( const ITEM_ID & serviceId
                                                , NERemoteService::eRemoteServices service
                                                , unsigned int connectTypes
                                                , const String & dispatcher
                                                , ServiceCommunicatonBase::eConnectionBehavior behavior /*= ServiceCommunicatonBase::eConnectionBehavior::DefaultAccept*/ )
    : IERemoteMessageHandler        ( )
    , IEServiceConnectionConsumer   ( )
    , IEServiceConnectionProvider   ( )
    , DispatcherThread              ( dispatcher )
    , IEServiceEventConsumerBase    ( )
    , IEServiceConnectionHandler    ( )

    , mConnectBehavior  ( behavior )
    , mService          ( service )
    , mConnectTypes     ( connectTypes )
    , mServerConnection ( serviceId )
    , mTimerConnect     ( static_cast<IETimerConsumer &>(mTimerConsumer), NEConnection::SERVER_CONNECT_TIMER_NAME.data( ) )
    , mThreadSend       ( static_cast<IERemoteMessageHandler&>(self()), mServerConnection )
    , mThreadReceive    ( static_cast<IEServiceConnectionHandler&>(self()), static_cast<IERemoteMessageHandler&>(self()), mServerConnection )
    , mDataRateHelper   ( mThreadSend, mThreadReceive, NESystemService::DEFAULT_VERBOSE )
    , mWhiteList        ( )
    , mBlackList        ( )
    , mEventConsumer    ( self() )
    , mTimerConsumer    ( self() )
    , mInstanceMap      (  )
    , mEventSendStop    ( false, false )
    , mLock             ( )
{
}

void ServiceCommunicatonBase::addInstance(const ITEM_ID & cookie, const NEService::sServiceConnectedInstance & instance)
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

bool ServiceCommunicatonBase::setupServiceConnectionData(NERemoteService::eRemoteServices service, uint32_t connectTypes)
{
    bool result{ false };
    if ((mService == service) && ((mConnectTypes & connectTypes) != 0))
    {
        if ((mConnectTypes & static_cast<uint32_t>(NERemoteService::eConnectionTypes::ConnectTcpip)) != 0)
        {
            ConnectionConfiguration config(mService, NERemoteService::eConnectionTypes::ConnectTcpip);
            if (config.isConfigured() && config.getConnectionEnableFlag())
            {
                String address{ config.getConnectionAddress() };
                unsigned short port{ config.getConnectionPort() };
                result = mServerConnection.setAddress(address, port);
            }
        }
    }

    return result;
}

void ServiceCommunicatonBase::applyServiceConnectionData(const String & hostName, unsigned short portNr)
{
    mServerConnection.setAddress( hostName, portNr );
}

bool ServiceCommunicatonBase::connectServiceHost(void)
{
    LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase_connectServiceHost);

    Lock lock(mLock);

    bool result{ false };
    if ( mServerConnection.isValid() == false && isRunning() == false )
    {
        if ( createThread( NECommon::WAIT_INFINITE ) && waitForDispatcherStart(NECommon::WAIT_INFINITE) )
        {
            result = true;
            sendCommand( ServiceEventData::eServiceEventCommands::CMD_StartService );
        }

        LOG_DBG( "Created remote servicing thread with [ %s ]", result ? "SUCCESS" : "FAIL" );
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
    LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase_reconnectServiceHost);

    Lock lock(mLock);
    bool result = true;
    if (isRunning() == false)
    {
        if (createThread(NECommon::WAIT_INFINITE) && waitForDispatcherStart(NECommon::WAIT_INFINITE))
        {
            result = sendCommand( ServiceEventData::eServiceEventCommands::CMD_RestartService );
        }

        LOG_DBG("Created remote servicing thread with [ %s ]", result ? "SUCCESS" : "FAIL");
    }
    else
    {
        LOG_WARN("The servicing thread is running, restarting servicing.");
        result = sendCommand( ServiceEventData::eServiceEventCommands::CMD_RestartService );
    }

    return result;
}

void ServiceCommunicatonBase::disconnectServiceHost(void)
{
    LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase_disconnectServiceHost);
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
    LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase_connectionLost);
    const ITEM_ID & cookie { mServerConnection.getCookie(clientSocket) };
    const ITEM_ID & channel{ mServerConnection.getChannelId() };

    LOG_WARN("Client lost connection: cookie [ %u ], socket [ %d ], host [ %s : %d ], closing connection"
                , static_cast<uint32_t>(cookie)
                , clientSocket.getHandle()
                , clientSocket.getAddress().getHostAddress().getString()
                , clientSocket.getAddress().getHostPort());

    if ( cookie != NEService::COOKIE_UNKNOWN )
    {
        removeInstance(cookie);
        RemoteMessage msgDisconnect = NERemoteService::createDisconnectRequest(cookie, channel);
        sendCommunicationMessage(ServiceEventData::eServiceEventCommands::CMD_ServiceReceivedMsg, msgDisconnect, Event::eEventPriority::EventPriorityNormal);
    }

    mServerConnection.closeConnection(clientSocket);
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
    LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase_onServiceStart);
    Lock lock( mLock );
    mEventSendStop.resetEvent();
    startConnection();
}

void ServiceCommunicatonBase::onServiceStop(void)
{
    LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase_onServiceStop);

    do
    {
        Lock lock(mLock);
        stopConnection();
    } while (false);

    mEventSendStop.setEvent();
}

void ServiceCommunicatonBase::onServiceRestart( void )
{
    LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase_onServiceRestart);
    restartConnection();
}

void ServiceCommunicatonBase::onServiceExit( void )
{
    LOG_SCOPE( areg_aregextend_service_ServiceCommunicatonBase_onServiceExit );
    onServiceStop( );
    triggerExit( );
}

void ServiceCommunicatonBase::onChannelConnected(const ITEM_ID& /*cookie*/)
{
}

bool ServiceCommunicatonBase::startConnection(void)
{
    LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase_startConnection);
    LOG_DBG("Going to start connection. Address [ %s ], port [ %d ]"
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
        LOG_DBG("Created socket [ %d ], going to create send-receive threads", static_cast<uint32_t>(mServerConnection.getSocketHandle()));
        if ( startSendThread( ) && startReceiveThread( ) )
        {
            result = true;
            LOG_DBG( "The threads are created. Ready to send-receive messages." );
        }
        else
        {
            LOG_ERR( "Failed to create send-receive threads, cannot communicate. Stop remote service" );
            mServerConnection.closeSocket( );
        }
    }
    else
    {
        LOG_ERR("Failed to create remote servicing socket.");
    }

    if ( result == false )
    {
        LOG_WARN("Remote servicing failed, trigger timer with [ %u ] ms timeout to re-establish remote servicing", NEConnection::DEFAULT_RETRY_CONNECT_TIMEOUT);
        mTimerConnect.startTimer( NEConnection::DEFAULT_RETRY_CONNECT_TIMEOUT, static_cast<DispatcherThread &>(self()), 1);
    }

    return result;
}

bool ServiceCommunicatonBase::restartConnection( void )
{
    LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase_restartConnection);
    LOG_DBG("Going to start connection. Address [ %s ], port [ %d ]"
                , mServerConnection.getAddress().getHostAddress().getString()
                , mServerConnection.getAddress().getHostPort());

    stopConnection();
    return startConnection();
}

void ServiceCommunicatonBase::stopConnection(void)
{
    LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase_stopConnection);
    LOG_WARN("Stopping remote servicing connection");

    mThreadReceive.triggerExit();

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

#ifdef DEBUG
void ServiceCommunicatonBase::failedSendMessage(const RemoteMessage & msgFailed, Socket & whichTarget )
#else  // DEBUG
void ServiceCommunicatonBase::failedSendMessage(const RemoteMessage& /*msgFailed*/, Socket& whichTarget)
#endif // DEBUG
{
    LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase_failedSendMessage);

#ifdef DEBUG

    const ITEM_ID & cookie = msgFailed.getTarget( );
    SocketAccepted client = mServerConnection.getClientByCookie( cookie );
    ASSERT( (client.isValid() == false) || (whichTarget.getHandle( ) == client.getHandle( )) );

#endif // DEBUG

    LOG_WARN("Failed to send message to [ %s ] client [ %d ], probably the connection is lost, closing connection"
                    , whichTarget.isValid() ? "VALID" : "INVALID"
                    , static_cast<int32_t>(whichTarget.getHandle()));

    if ( whichTarget.isValid())
    {
        connectionLost( static_cast<SocketAccepted &>(whichTarget) );
    }
}

void ServiceCommunicatonBase::failedReceiveMessage(Socket & whichSource)
{
    LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase_failedReceiveMessage);

    SocketAccepted client = mServerConnection.getClientByHandle(whichSource.getHandle());
    LOG_WARN("Failed to receive message from [ %s ] client [ %d ], probably the connection with socket [ %d ] is lost, closing connection"
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
    LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase_processReceivedMessage);
    if ( msgReceived.isValid() )
    {
        const ITEM_ID & cookie = mServerConnection.getCookie(whichSource.getHandle());
        const ITEM_ID & source = msgReceived.getSource();
        const ITEM_ID & target = msgReceived.getTarget();
        NEService::eFuncIdRange msgId  = static_cast<NEService::eFuncIdRange>( msgReceived.getMessageId() );

        LOG_DBG("Received message [ %s ] of id [ 0x%X ] from source [ %u ] ( connection cookie = %u ) of client host [ %s : %d ] for target [ %u ]"
                        , NEService::getString(msgId)
                        , static_cast<uint32_t>(msgId)
                        , static_cast<uint32_t>(source)
                        , static_cast<uint32_t>(cookie)
                        , static_cast<const char *>(whichSource.getAddress().getHostAddress())
                        , static_cast<int>(whichSource.getAddress().getHostPort( ))
                        , static_cast<id_type>(target));

        if ( (source >= NEService::COOKIE_REMOTE_SERVICE) && NEService::isExecutableId(static_cast<uint32_t>(msgId)) )
        {
            LOG_DBG("Forwarding message [ 0x%X ] to send to target [ %u ]", static_cast<uint32_t>(msgId), static_cast<uint32_t>(target));
            if ( target != NEService::TARGET_UNKNOWN )
            {
                sendMessage(msgReceived);
            }
        }
        else if ( (source == cookie) && (msgId != NEService::eFuncIdRange::SystemServiceConnect) )
        {
            LOG_DBG("Going to process received message [ 0x%X ]", static_cast<uint32_t>(msgId));
            if ( msgId == NEService::eFuncIdRange::SystemServiceDisconnect )
            {
                removeInstance( cookie );
            }

            sendCommunicationMessage( ServiceEventData::eServiceEventCommands::CMD_ServiceReceivedMsg, msgReceived );
        }
        else if ( (source == NEService::SOURCE_UNKNOWN) && (msgId == NEService::eFuncIdRange::SystemServiceConnect) )
        {
            NEService::sServiceConnectedInstance instance{};
            msgReceived >> instance;
            instance.ciTimestamp = static_cast<TIME64>(DateTime::getNow());
            instance.ciCookie = cookie;
            addInstance(cookie, instance);
            RemoteMessage msgConnect(createServiceConnectMessage(mServerConnection.getChannelId(), cookie, NEService::eMessageSource::MessageSourceService));
            LOG_DBG("Received request connect message, sending response [ %s ] of id [ 0x%X ], to new target [ %u ], connection socket [ %u ], checksum [ %u ]"
                        , NEService::getString( static_cast<NEService::eFuncIdRange>(msgConnect.getMessageId()))
                        , static_cast<uint32_t>(msgConnect.getMessageId())
                        , static_cast<uint32_t>(msgConnect.getTarget())
                        , static_cast<uint32_t>(whichSource.getHandle())
                        , msgConnect.getChecksum());

            sendMessage( msgConnect );
        }
        else
        {
            LOG_WARN("Ignoring to process message [ %s ] of id [ 0x%X ] from source [ %u ]"
                        , NEService::getString(msgId)
                        , static_cast<uint32_t>(msgId)
                        , static_cast<uint32_t>(source));
        }
    }
    else
    {
        LOG_WARN("Received invalid message from source [ %u ], ignoring to process", static_cast<uint32_t>(msgReceived.getSource()));
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

RemoteMessage ServiceCommunicatonBase::createServiceConnectMessage(const ITEM_ID & source, const ITEM_ID & target, NEService::eMessageSource msgSource) const
{
    RemoteMessage result{ NERemoteService::createConnectNotify(source, target) };
    result.moveToEnd();
    result << msgSource;
    return result;
}

RemoteMessage ServiceCommunicatonBase::createServiceDisconnectMessage( const ITEM_ID & source, const ITEM_ID & target ) const
{
    return NERemoteService::createDisconnectNotify(source, target);
}
