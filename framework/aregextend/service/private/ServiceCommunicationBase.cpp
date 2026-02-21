/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        aregextend/service/private/ServiceCommunicationBase.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Service communication base class
 ************************************************************************/
#include "aregextend/service/ServiceCommunicationBase.hpp"

#include "areg/base/DateTime.hpp"
#include "areg/ipc/RemoteServiceDefs.hpp"
#include "areg/ipc/ConnectionConfiguration.hpp"
#include "areg/ipc/private/ConnectionDefs.hpp"
#include "areg/logging/GELog.h"

#include "aregextend/service/SystemServiceDefs.hpp"

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
// ServiceCommunicationBase class implementation
//////////////////////////////////////////////////////////////////////////

ServiceCommunicationBase::ServiceCommunicationBase( const ITEM_ID & serviceId
                                                , NERemoteService::RemoteServiceKind service
                                                , uint32_t connectTypes
                                                , const String & dispatcher
                                                , ServiceCommunicationBase::ConnectionPolicy behavior /*= ServiceCommunicationBase::ConnectionPolicy::Accept*/ )
    : RemoteMessageHandler        ( )
    , ConnectionConsumer   ( )
    , ConnectionProvider   ( )
    , DispatcherThread              ( dispatcher, NECommon::DEFAULT_BLOCK_SIZE, NECommon::QUEUE_SIZE_MAXIMUM )
    , ServiceEventConsumer    ( )
    , ConnectionHandler    ( )

    , mConnectBehavior  ( behavior )
    , mService          ( service )
    , mConnectTypes     ( connectTypes )
    , mServerConnection ( serviceId )
    , mTimerConnect     ( static_cast<TimerConsumer &>(mTimerConsumer), NEConnection::SERVER_CONNECT_TIMER_NAME.data( ) )
    , mThreadSend       ( static_cast<RemoteMessageHandler&>(self()), mServerConnection )
    , mThreadReceive    ( static_cast<ConnectionHandler&>(self()), static_cast<RemoteMessageHandler&>(self()), mServerConnection )
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

void ServiceCommunicationBase::addInstance(const ITEM_ID & cookie, const NEService::sServiceConnectedInstance & instance)
{
    Lock lock(mLock);
    mInstanceMap.addIfUnique(cookie, instance);
}

void ServiceCommunicationBase::removeInstance(const ITEM_ID & cookie)
{
    Lock lock(mLock);
    mInstanceMap.removeAt(cookie);
}

void ServiceCommunicationBase::removeAllInstances()
{
    Lock lock(mLock);
    mInstanceMap.release();
}

bool ServiceCommunicationBase::setupServiceConnectionData(NERemoteService::RemoteServiceKind service, uint32_t connectTypes)
{
    bool result{ false };
    if ((mService == service) && ((mConnectTypes & connectTypes) != 0))
    {
        if ((mConnectTypes & static_cast<uint32_t>(NERemoteService::ConnectionType::Tcpip)) != 0)
        {
            ConnectionConfiguration config(mService, NERemoteService::ConnectionType::Tcpip);
            if (config.isConfigured() && config.getConnectionEnableFlag())
            {
                String address{ config.getConnectionAddress() };
                uint16_t port{ config.getConnectionPort() };
                result = mServerConnection.setAddress(address, port);
            }
        }
    }

    return result;
}

void ServiceCommunicationBase::applyServiceConnectionData(const String & hostName, uint16_t portNr)
{
    mServerConnection.setAddress( hostName, portNr );
}

bool ServiceCommunicationBase::connectServiceHost()
{
    LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase_connectServiceHost);

    Lock lock(mLock);

    bool result{ false };
    if ( mServerConnection.isValid() == false && isRunning() == false )
    {
        if ( createThread( NECommon::WAIT_INFINITE ) && waitForDispatcherStart(NECommon::WAIT_INFINITE) )
        {
            result = true;
            sendCommand( ServiceEventData::ServiceCommand::CMD_StartService );
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

bool ServiceCommunicationBase::reconnectServiceHost()
{
    LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase_reconnectServiceHost);

    Lock lock(mLock);
    bool result = true;
    if (isRunning() == false)
    {
        if (createThread(NECommon::WAIT_INFINITE) && waitForDispatcherStart(NECommon::WAIT_INFINITE))
        {
            result = sendCommand( ServiceEventData::ServiceCommand::CMD_RestartService );
        }

        LOG_DBG("Created remote servicing thread with [ %s ]", result ? "SUCCESS" : "FAIL");
    }
    else
    {
        LOG_WARN("The servicing thread is running, restarting servicing.");
        result = sendCommand( ServiceEventData::ServiceCommand::CMD_RestartService );
    }

    return result;
}

void ServiceCommunicationBase::disconnectServiceHost()
{
    LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase_disconnectServiceHost);
    if ( isRunning() )
    {
        sendCommand( ServiceEventData::ServiceCommand::CMD_ServiceExit, Event::EventPriority::HighPrio );
        mEventSendStop.lock();
    }
}

bool ServiceCommunicationBase::isServiceHostConnected() const
{
    Lock lock( mLock );
    return (mServerConnection.isValid() && isRunning());
}

bool ServiceCommunicationBase::isServiceHostPending() const
{
    Lock lock(mLock);
    return ((mServerConnection.isValid() == false) && isRunning());
}

bool ServiceCommunicationBase::isServiceHostSetup() const
{
    Lock lock(mLock);
    return mServerConnection.getAddress().isValid();
}

bool ServiceCommunicationBase::canAcceptConnection(const SocketAccepted & clientSocket)
{
    bool result{ false };
    if ( clientSocket.isValid( ) && clientSocket.isAlive() )
    {
        Lock lock(mLock);
        const String & ipAddress = clientSocket.getAddress( ).getHostAddress( );
        result =  ((mConnectBehavior == ConnectionPolicy::Accept) && (mBlackList.contains( ipAddress ) == false)) ||
                  ((mConnectBehavior == ConnectionPolicy::Reject) && (mWhiteList.contains( ipAddress ) == true ));

    }

    return result;
}

void ServiceCommunicationBase::connectionLost( SocketAccepted & clientSocket )
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
        sendCommunicationMessage(ServiceEventData::ServiceCommand::CMD_ServiceReceivedMsg, msgDisconnect, Event::EventPriority::NormalPrio);
    }

    mServerConnection.closeConnection(clientSocket);
}

void ServiceCommunicationBase::connectionFailure()
{
    if ( isServiceHostConnected())
    {
        reconnectServiceHost();
    }
}

void ServiceCommunicationBase::disconnectServices()
{
    removeAllInstances();
}

void ServiceCommunicationBase::onServiceReconnectTimerExpired()
{
    Lock lock( mLock );
    startConnection( );
}

void ServiceCommunicationBase::onServiceStart()
{
    LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase_onServiceStart);
    Lock lock( mLock );
    mEventSendStop.resetEvent();
    startConnection();
}

void ServiceCommunicationBase::onServiceStop()
{
    LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase_onServiceStop);

    do
    {
        Lock lock(mLock);
        stopConnection();
    } while (false);

    mEventSendStop.setEvent();
}

void ServiceCommunicationBase::onServiceRestart()
{
    LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase_onServiceRestart);
    restartConnection();
}

void ServiceCommunicationBase::onServiceExit()
{
    LOG_SCOPE( areg_aregextend_service_ServiceCommunicatonBase_onServiceExit );
    onServiceStop( );
    triggerExit( );
}

void ServiceCommunicationBase::onChannelConnected(const ITEM_ID& /*cookie*/)
{
}

bool ServiceCommunicationBase::startConnection()
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

bool ServiceCommunicationBase::restartConnection()
{
    LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase_restartConnection);
    LOG_DBG("Going to start connection. Address [ %s ], port [ %d ]"
                , mServerConnection.getAddress().getHostAddress().getString()
                , mServerConnection.getAddress().getHostPort());

    stopConnection();
    return startConnection();
}

void ServiceCommunicationBase::stopConnection()
{
    LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase_stopConnection);
    LOG_WARN("Stopping remote servicing connection");

    mThreadReceive.triggerExit();

    disconnectServices( );
    disconnectService( Event::EventPriority::NormalPrio );

    // Wait without triggering exit.
    mThreadSend.completionWait( NECommon::WAIT_INFINITE );
    mServerConnection.closeSocket( );
    // Trigger exit and clean resources.
    mThreadSend.shutdownThread( NECommon::WAIT_INFINITE );
    mThreadReceive.shutdownThread( NECommon::WAIT_INFINITE );
}

bool ServiceCommunicationBase::startSendThread()
{
    return mThreadSend.createThread( NECommon::WAIT_INFINITE ) && 
           mThreadSend.waitForDispatcherStart( NECommon::WAIT_INFINITE );
}

bool ServiceCommunicationBase::startReceiveThread()
{
    return mThreadReceive.createThread( NECommon::WAIT_INFINITE ) &&
           mThreadReceive.waitForDispatcherStart( NECommon::WAIT_INFINITE );
}

#ifdef DEBUG
void ServiceCommunicationBase::failedSendMessage(const RemoteMessage & msgFailed, Socket & whichTarget )
#else  // DEBUG
void ServiceCommunicationBase::failedSendMessage(const RemoteMessage& /*msgFailed*/, Socket& whichTarget)
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

void ServiceCommunicationBase::failedReceiveMessage(Socket & whichSource)
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

void ServiceCommunicationBase::processReceivedMessage(const RemoteMessage & msgReceived, Socket & whichSource)
{
    LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase_processReceivedMessage);
    if ( msgReceived.isValid() )
    {
        const ITEM_ID & cookie = mServerConnection.getCookie(whichSource.getHandle());
        const ITEM_ID & source = msgReceived.getSource();
        const ITEM_ID & target = msgReceived.getTarget();
        NEService::FuncIdRange msgId  = static_cast<NEService::FuncIdRange>( msgReceived.getMessageId() );

        LOG_DBG("Received message [ %s ] of id [ 0x%X ] from source [ %u ] ( connection cookie = %u ) of client host [ %s : %d ] for target [ %u ]"
                        , NEService::getString(msgId)
                        , static_cast<uint32_t>(msgId)
                        , static_cast<uint32_t>(source)
                        , static_cast<uint32_t>(cookie)
                        , static_cast<const char *>(whichSource.getAddress().getHostAddress())
                        , static_cast<int32_t>(whichSource.getAddress().getHostPort( ))
                        , static_cast<id_type>(target));

        if ( (source >= NEService::COOKIE_REMOTE_SERVICE) && NEService::isExecutableId(static_cast<uint32_t>(msgId)) )
        {
            LOG_DBG("Forwarding message [ 0x%X ] to send to target [ %u ]", static_cast<uint32_t>(msgId), static_cast<uint32_t>(target));
            if ( target != NEService::TARGET_UNKNOWN )
            {
                sendMessage(msgReceived);
            }
        }
        else if ( (source == cookie) && (msgId != NEService::FuncIdRange::SystemServiceConnect) )
        {
            LOG_DBG("Going to process received message [ 0x%X ]", static_cast<uint32_t>(msgId));
            if ( msgId == NEService::FuncIdRange::SystemServiceDisconnect )
            {
                removeInstance( cookie );
            }

            sendCommunicationMessage( ServiceEventData::ServiceCommand::CMD_ServiceReceivedMsg, msgReceived );
        }
        else if ( (source == NEService::SOURCE_UNKNOWN) && (msgId == NEService::FuncIdRange::SystemServiceConnect) )
        {
            NEService::sServiceConnectedInstance instance{};
            msgReceived >> instance;
            instance.ciTimestamp = static_cast<TIME64>(DateTime::getNow());
            instance.ciCookie = cookie;
            addInstance(cookie, instance);
            RemoteMessage msgConnect(createServiceConnectMessage(mServerConnection.getChannelId(), cookie, NEService::MessageSource::SourceService));
            LOG_DBG("Received request connect message, sending response [ %s ] of id [ 0x%X ], to new target [ %u ], connection socket [ %u ], checksum [ %u ]"
                        , NEService::getString( static_cast<NEService::FuncIdRange>(msgConnect.getMessageId()))
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

void ServiceCommunicationBase::readyForEvents( bool isReady )
{
    if ( isReady )
    {
        ServiceServerEvent::addListener( static_cast<ServiceServerEventConsumer &>(mEventConsumer), static_cast<DispatcherThread &>(self( )) );
        DispatcherThread::readyForEvents( true );
    }
    else
    {
        DispatcherThread::readyForEvents( false );
        ServiceServerEvent::removeListener( static_cast<ServiceServerEventConsumer &>(mEventConsumer), static_cast<DispatcherThread &>(self( )) );
    }
}

bool ServiceCommunicationBase::postEvent( Event & eventElem )
{
    return EventDispatcher::postEvent( eventElem );
}

RemoteMessage ServiceCommunicationBase::createServiceConnectMessage(const ITEM_ID & source, const ITEM_ID & target, NEService::MessageSource msgSource) const
{
    RemoteMessage result{ NERemoteService::createConnectNotify(source, target) };
    result.moveToEnd();
    result << msgSource;
    return result;
}

RemoteMessage ServiceCommunicationBase::createServiceDisconnectMessage( const ITEM_ID & source, const ITEM_ID & target ) const
{
    return NERemoteService::createDisconnectNotify(source, target);
}
