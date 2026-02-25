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

void ServiceCommunicationBase::addInstance(const ITEM_ID & cookie, const NEService::ConnectedInstance & instance)
{
    Lock lock(mLock);
    mInstanceMap.add_if_unique(cookie, instance);
}

void ServiceCommunicationBase::removeInstance(const ITEM_ID & cookie)
{
    Lock lock(mLock);
    mInstanceMap.remove_at(cookie);
}

void ServiceCommunicationBase::removeAllInstances()
{
    Lock lock(mLock);
    mInstanceMap.release();
}

bool ServiceCommunicationBase::setup_connection_data(NERemoteService::RemoteServiceKind service, uint32_t connectTypes)
{
    bool result{ false };
    if ((mService == service) && ((mConnectTypes & connectTypes) != 0))
    {
        if ((mConnectTypes & static_cast<uint32_t>(NERemoteService::ConnectionType::Tcpip)) != 0)
        {
            ConnectionConfiguration config(mService, NERemoteService::ConnectionType::Tcpip);
            if (config.is_configured() && config.connection_enable_flag())
            {
                String address{ config.connection_address() };
                uint16_t port{ config.connection_port() };
                result = mServerConnection.set_address(address, port);
            }
        }
    }

    return result;
}

void ServiceCommunicationBase::apply_connection_data(const String & hostName, uint16_t portNr)
{
    mServerConnection.set_address( hostName, portNr );
}

bool ServiceCommunicationBase::connect_service_host()
{
    LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase_connectServiceHost);

    Lock lock(mLock);

    bool result{ false };
    if ( mServerConnection.is_valid() == false && is_running() == false )
    {
        if ( create_thread( NECommon::WAIT_INFINITE ) && wait_start(NECommon::WAIT_INFINITE) )
        {
            result = true;
            send_command( ServiceEventData::ServiceCommand::CMD_StartService );
        }

        LOG_DBG( "Created remote servicing thread with [ %s ]", result ? "SUCCESS" : "FAIL" );
    }
    else
    {
        result = is_running( ) && mServerConnection.is_valid( );
        ASSERT(is_running());
    }

    return result;
}

bool ServiceCommunicationBase::reconnect_service_host()
{
    LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase_reconnectServiceHost);

    Lock lock(mLock);
    bool result = true;
    if (is_running() == false)
    {
        if (create_thread(NECommon::WAIT_INFINITE) && wait_start(NECommon::WAIT_INFINITE))
        {
            result = send_command( ServiceEventData::ServiceCommand::CMD_RestartService );
        }

        LOG_DBG("Created remote servicing thread with [ %s ]", result ? "SUCCESS" : "FAIL");
    }
    else
    {
        LOG_WARN("The servicing thread is running, restarting servicing.");
        result = send_command( ServiceEventData::ServiceCommand::CMD_RestartService );
    }

    return result;
}

void ServiceCommunicationBase::disconnect_service_host()
{
    LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase_disconnectServiceHost);
    if ( is_running() )
    {
        send_command( ServiceEventData::ServiceCommand::CMD_ServiceExit, Event::EventPriority::HighPrio );
        mEventSendStop.lock();
    }
}

bool ServiceCommunicationBase::is_host_connected() const
{
    Lock lock( mLock );
    return (mServerConnection.is_valid() && is_running());
}

bool ServiceCommunicationBase::is_host_pending() const
{
    Lock lock(mLock);
    return ((mServerConnection.is_valid() == false) && is_running());
}

bool ServiceCommunicationBase::is_host_setup() const
{
    Lock lock(mLock);
    return mServerConnection.address().is_valid();
}

bool ServiceCommunicationBase::canAcceptConnection(const SocketAccepted & clientSocket)
{
    bool result{ false };
    if ( clientSocket.is_valid( ) && clientSocket.is_alive() )
    {
        Lock lock(mLock);
        const String & ipAddress = clientSocket.address( ).host_address( );
        result =  ((mConnectBehavior == ConnectionPolicy::Accept) && (mBlackList.contains( ipAddress ) == false)) ||
                  ((mConnectBehavior == ConnectionPolicy::Reject) && (mWhiteList.contains( ipAddress ) == true ));

    }

    return result;
}

void ServiceCommunicationBase::connectionLost( SocketAccepted & clientSocket )
{
    LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase_connectionLost);
    const ITEM_ID & cookie { mServerConnection.cookie(clientSocket) };
    const ITEM_ID & channel{ mServerConnection.getChannelId() };

    LOG_WARN("Client lost connection: cookie [ %u ], socket [ %d ], host [ %s : %d ], closing connection"
                , static_cast<uint32_t>(cookie)
                , clientSocket.handle()
                , clientSocket.address().host_address().as_string()
                , clientSocket.address().host_port());

    if ( cookie != NEService::COOKIE_UNKNOWN )
    {
        removeInstance(cookie);
        RemoteMessage msgDisconnect = NERemoteService::create_disconnect_request(cookie, channel);
        sendCommunicationMessage(ServiceEventData::ServiceCommand::CMD_ServiceReceivedMsg, msgDisconnect, Event::EventPriority::NormalPrio);
    }

    mServerConnection.close_connection(clientSocket);
}

void ServiceCommunicationBase::connectionFailure()
{
    if ( is_host_connected())
    {
        reconnect_service_host();
    }
}

void ServiceCommunicationBase::disconnectServices()
{
    removeAllInstances();
}

void ServiceCommunicationBase::on_reconnect_timer()
{
    Lock lock( mLock );
    start_connection( );
}

void ServiceCommunicationBase::on_service_start()
{
    LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase_onServiceStart);
    Lock lock( mLock );
    mEventSendStop.reset();
    start_connection();
}

void ServiceCommunicationBase::on_service_stop()
{
    LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase_onServiceStop);

    do
    {
        Lock lock(mLock);
        stop_connection();
    } while (false);

    mEventSendStop.set_event();
}

void ServiceCommunicationBase::on_service_restart()
{
    LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase_onServiceRestart);
    restartConnection();
}

void ServiceCommunicationBase::on_service_exit()
{
    LOG_SCOPE( areg_aregextend_service_ServiceCommunicatonBase_onServiceExit );
    on_service_stop( );
    trigger_exit( );
}

void ServiceCommunicationBase::on_channel_connected(const ITEM_ID& /*cookie*/)
{
}

bool ServiceCommunicationBase::start_connection()
{
    LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase_startConnection);
    LOG_DBG("Going to start connection. Address [ %s ], port [ %d ]"
                , mServerConnection.address().host_address().as_string()
                , mServerConnection.address().host_port());


    ASSERT(mServerConnection.address().is_valid());
    ASSERT(mServerConnection.is_valid() == false);
    ASSERT(mThreadReceive.is_running() == false);
    ASSERT(mThreadSend.is_running() == false);

    bool result = false;
    mTimerConnect.stop_timer();

    if ( mServerConnection.create_socket() )
    {
        LOG_DBG("Created socket [ %d ], going to create send-receive threads", static_cast<uint32_t>(mServerConnection.socket_handle()));
        if ( startSendThread( ) && startReceiveThread( ) )
        {
            result = true;
            LOG_DBG( "The threads are created. Ready to send-receive messages." );
        }
        else
        {
            LOG_ERR( "Failed to create send-receive threads, cannot communicate. Stop remote service" );
            mServerConnection.close_socket( );
        }
    }
    else
    {
        LOG_ERR("Failed to create remote servicing socket.");
    }

    if ( result == false )
    {
        LOG_WARN("Remote servicing failed, trigger timer with [ %u ] ms timeout to re-establish remote servicing", NEConnection::DEFAULT_RETRY_CONNECT_TIMEOUT);
        mTimerConnect.start_timer( NEConnection::DEFAULT_RETRY_CONNECT_TIMEOUT, static_cast<DispatcherThread &>(self()), 1);
    }

    return result;
}

bool ServiceCommunicationBase::restartConnection()
{
    LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase_restartConnection);
    LOG_DBG("Going to start connection. Address [ %s ], port [ %d ]"
                , mServerConnection.address().host_address().as_string()
                , mServerConnection.address().host_port());

    stop_connection();
    return start_connection();
}

void ServiceCommunicationBase::stop_connection()
{
    LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase_stopConnection);
    LOG_WARN("Stopping remote servicing connection");

    mThreadReceive.trigger_exit();

    disconnectServices( );
    disconnect_service( Event::EventPriority::NormalPrio );

    // Wait without triggering exit.
    mThreadSend.completion_wait( NECommon::WAIT_INFINITE );
    mServerConnection.close_socket( );
    // Trigger exit and clean resources.
    mThreadSend.shutdown_thread( NECommon::WAIT_INFINITE );
    mThreadReceive.shutdown_thread( NECommon::WAIT_INFINITE );
}

bool ServiceCommunicationBase::startSendThread()
{
    return mThreadSend.create_thread( NECommon::WAIT_INFINITE ) && 
           mThreadSend.wait_start( NECommon::WAIT_INFINITE );
}

bool ServiceCommunicationBase::startReceiveThread()
{
    return mThreadReceive.create_thread( NECommon::WAIT_INFINITE ) &&
           mThreadReceive.wait_start( NECommon::WAIT_INFINITE );
}

#ifdef DEBUG
void ServiceCommunicationBase::failed_send_message(const RemoteMessage & msgFailed, Socket & whichTarget )
#else  // DEBUG
void ServiceCommunicationBase::failed_send_message(const RemoteMessage& /*msgFailed*/, Socket& whichTarget)
#endif // DEBUG
{
    LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase_failedSendMessage);

#ifdef DEBUG

    const ITEM_ID & cookie = msgFailed.target( );
    SocketAccepted client = mServerConnection.client_by_cookie( cookie );
    ASSERT( (client.is_valid() == false) || (whichTarget.handle( ) == client.handle( )) );

#endif // DEBUG

    LOG_WARN("Failed to send message to [ %s ] client [ %d ], probably the connection is lost, closing connection"
                    , whichTarget.is_valid() ? "VALID" : "INVALID"
                    , static_cast<int32_t>(whichTarget.handle()));

    if ( whichTarget.is_valid())
    {
        connectionLost( static_cast<SocketAccepted &>(whichTarget) );
    }
}

void ServiceCommunicationBase::failed_receive_message(Socket & whichSource)
{
    LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase_failedReceiveMessage);

    SocketAccepted client = mServerConnection.client_by_handle(whichSource.handle());
    LOG_WARN("Failed to receive message from [ %s ] client [ %d ], probably the connection with socket [ %d ] is lost, closing connection"
                        , client.is_valid() ? "VALID" : "INVALID"
                        , static_cast<int32_t>(client.handle())
                        , static_cast<int32_t>(whichSource.handle()));

    if (client.is_valid())
    {
        connectionLost(client);
    }
}

void ServiceCommunicationBase::process_received_message(const RemoteMessage & msgReceived, Socket & whichSource)
{
    LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase_processReceivedMessage);
    if ( msgReceived.is_valid() )
    {
        const ITEM_ID & cookie = mServerConnection.cookie(whichSource.handle());
        const ITEM_ID & source = msgReceived.source();
        const ITEM_ID & target = msgReceived.target();
        NEService::FuncIdRange msgId  = static_cast<NEService::FuncIdRange>( msgReceived.message_id() );

        LOG_DBG("Received message [ %s ] of id [ 0x%X ] from source [ %u ] ( connection cookie = %u ) of client host [ %s : %d ] for target [ %u ]"
                        , NEService::as_string(msgId)
                        , static_cast<uint32_t>(msgId)
                        , static_cast<uint32_t>(source)
                        , static_cast<uint32_t>(cookie)
                        , static_cast<const char *>(whichSource.address().host_address())
                        , static_cast<int32_t>(whichSource.address().host_port( ))
                        , static_cast<id_type>(target));

        if ( (source >= NEService::COOKIE_REMOTE_SERVICE) && NEService::is_executable_id(static_cast<uint32_t>(msgId)) )
        {
            LOG_DBG("Forwarding message [ 0x%X ] to send to target [ %u ]", static_cast<uint32_t>(msgId), static_cast<uint32_t>(target));
            if ( target != NEService::TARGET_UNKNOWN )
            {
                send_message(msgReceived);
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
            NEService::ConnectedInstance instance{};
            msgReceived >> instance;
            instance.ciTimestamp = static_cast<TIME64>(DateTime::now());
            instance.ciCookie = cookie;
            addInstance(cookie, instance);
            RemoteMessage msgConnect(connect_message(mServerConnection.getChannelId(), cookie, NEService::MessageSource::SourceService));
            LOG_DBG("Received request connect message, sending response [ %s ] of id [ 0x%X ], to new target [ %u ], connection socket [ %u ], checksum [ %u ]"
                        , NEService::as_string( static_cast<NEService::FuncIdRange>(msgConnect.message_id()))
                        , static_cast<uint32_t>(msgConnect.message_id())
                        , static_cast<uint32_t>(msgConnect.target())
                        , static_cast<uint32_t>(whichSource.handle())
                        , msgConnect.checksum());

            send_message( msgConnect );
        }
        else
        {
            LOG_WARN("Ignoring to process message [ %s ] of id [ 0x%X ] from source [ %u ]"
                        , NEService::as_string(msgId)
                        , static_cast<uint32_t>(msgId)
                        , static_cast<uint32_t>(source));
        }
    }
    else
    {
        LOG_WARN("Received invalid message from source [ %u ], ignoring to process", static_cast<uint32_t>(msgReceived.source()));
    }
}

void ServiceCommunicationBase::ready_for_events( bool is_ready )
{
    if ( is_ready )
    {
        ServiceServerEvent::add_listener( static_cast<ServiceServerEventConsumer &>(mEventConsumer), static_cast<DispatcherThread &>(self( )) );
        DispatcherThread::ready_for_events( true );
    }
    else
    {
        DispatcherThread::ready_for_events( false );
        ServiceServerEvent::remove_listener( static_cast<ServiceServerEventConsumer &>(mEventConsumer), static_cast<DispatcherThread &>(self( )) );
    }
}

bool ServiceCommunicationBase::post_event( Event & eventElem )
{
    return EventDispatcher::post_event( eventElem );
}

RemoteMessage ServiceCommunicationBase::connect_message(const ITEM_ID & source, const ITEM_ID & target, NEService::MessageSource msgSource) const
{
    RemoteMessage result{ NERemoteService::create_connect_notify(source, target) };
    result.move_to_end();
    result << msgSource;
    return result;
}

RemoteMessage ServiceCommunicationBase::disconnect_message( const ITEM_ID & source, const ITEM_ID & target ) const
{
    return NERemoteService::create_disconnect_notify(source, target);
}
