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
#include "areg/base/EventEnvelope.hpp"
#include "areg/component/Event.hpp"
#include "areg/component/EventConsumer.hpp"
#include "areg/component/ExitEvent.hpp"

#include <cstring>
#include "areg/ipc/RemoteServiceDefs.hpp"
#include "areg/ipc/ConnectionConfiguration.hpp"
#include "areg/ipc/private/ConnectionDefs.hpp"
#include "areg/logging/areg_log.h"

#include "aregextend/service/SystemServiceDefs.hpp"

namespace areg::ext {

DEF_LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase, connect_service_host);
DEF_LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase, reconnect_service_host);
DEF_LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase, disconnect_service_host);
DEF_LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase, connection_lost);

DEF_LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase, on_service_start);
DEF_LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase, on_service_stop);
DEF_LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase, on_service_restart);
DEF_LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase, on_service_exit);

DEF_LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase, start_connection);
DEF_LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase, restart_connection);
DEF_LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase, stop_connection);

DEF_LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase, failed_send_message);
DEF_LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase, failed_receive_message);

DEF_LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase, connection_failure);
DEF_LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase, do_accept_client_pool);

DEBUG_DEF_LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase, process_received_message);

//////////////////////////////////////////////////////////////////////////
// ServiceCommunicationBase class implementation
//////////////////////////////////////////////////////////////////////////

ServiceCommunicationBase::ServiceCommunicationBase( const ITEM_ID & serviceId
                                                , areg::RemoteServiceKind service
                                                , uint32_t connectTypes
                                                , uint32_t stackSizeKb
                                                , const String & dispatcher
                                                , ServiceCommunicationBase::ConnectionPolicy behavior /*= ServiceCommunicationBase::ConnectionPolicy::Accept*/
                                                , uint32_t numPairs /*= 0u*/ )
    : RemoteMessageHandler  ( )
    , ConnectionConsumer    ( )
    , ConnectionProvider    ( )
    , DispatcherThread      ( dispatcher, stackSizeKb, areg::QUEUE_SIZE_MAXIMUM )
    , ServiceEventConsumer  ( )
    , ConnectionHandler     ( )

    , mConnectBehavior  ( behavior )
    , mService          ( service )
    , mConnectTypes     ( connectTypes )
    , mNumPairs         ( numPairs )
    , mServerConnection ( serviceId )
    , mTimerConnect     ( static_cast<TimerConsumer &>(mTimerConsumer), areg::SERVER_CONNECT_TIMER_NAME.data( ) )
    , mThreadSend       ( static_cast<RemoteMessageHandler&>(self()), mServerConnection )
    , mThreadReceive    ( static_cast<ConnectionHandler&>(self()), static_cast<RemoteMessageHandler&>(self()), mServerConnection )
    , mClientPairs      ( )
    , mShuttingDown     ( false )
    , mDataRateHelper   ( self() , areg::ext::DEFAULT_VERBOSE)
    , mWhiteList        ( )
    , mBlackList        ( )
    , mEventConsumer    ( self() )
    , mMessageConsumer  ( self() )
    , mTimerConsumer    ( self() )
    , mInstanceMap      (  )
    , mEventSendStop    ( false, false )
    , mLock             ( )
    , mSendFn           ( )
    , mSendMoveFn       ( )
    , mAcceptFn         ( )
    , mLostFn           ( )
{
    update_dispatch_mode();
}

void ServiceCommunicationBase::update_dispatch_mode()
{
    if (mNumPairs == 0)
    {
        mSendFn     = [this](const areg::EventEnvelope & d, areg::EventPriority p)  { return do_send_shared(d, p); };
        mSendMoveFn = [this](areg::EventEnvelope && d,     areg::EventPriority p)  { return do_send_shared(std::move(d), p); };
        mAcceptFn   = [this](areg::SocketAccepted & s)                             { return do_accept_client_shared(s); };
        mLostFn     = [this](ITEM_ID c)                                            { do_client_lost_shared(c); };
    }
    else
    {
        mSendFn     = [this](const areg::EventEnvelope & d, areg::EventPriority p)  { return do_send_pool(d, p); };
        mSendMoveFn = [this](areg::EventEnvelope && d,     areg::EventPriority p)   { return do_send_pool(std::move(d), p); };
        mAcceptFn   = [this](areg::SocketAccepted & s)                        { return do_accept_client_pool(s); };
        mLostFn     = [this](ITEM_ID c)                                       { do_client_lost_pool(c); };
    }
}

void ServiceCommunicationBase::add_instance(const ITEM_ID & cookie, const areg::ConnectedInstance & instance)
{
    Lock lock(mLock);
    mInstanceMap.add_if_unique(cookie, instance);
}

void ServiceCommunicationBase::remove_instance(const ITEM_ID & cookie)
{
    Lock lock(mLock);
    mInstanceMap.remove_at(cookie);
}

void ServiceCommunicationBase::remove_all_instances()
{
    Lock lock(mLock);
    mInstanceMap.release();
}

bool ServiceCommunicationBase::setup_connection_data(areg::RemoteServiceKind service, uint32_t connectTypes)
{
    if ((mService != service) || ((mConnectTypes & connectTypes) == 0))
        return false;
    
    if ((mConnectTypes & static_cast<uint32_t>(areg::ConnectionType::Tcpip)) == 0)
        return false;
    
    ConnectionConfiguration config(mService, areg::ConnectionType::Tcpip);
    if (!config.is_configured() || !config.connection_enable_flag())
        return false;
    
    String address{ config.connection_address() };
    uint16_t port{ config.connection_port() };
    bool result = mServerConnection.set_address(address, port);
    mServerConnection.set_socket_buffers(config.socket_send_buffer(), config.socket_recv_buffer());
    mServerConnection.set_send_timeout(config.socket_send_timeout());

    const uint32_t configPairs{ config.pool_pairs() };
    if (configPairs != mNumPairs)
    {
        mNumPairs = configPairs;
        update_dispatch_mode();
    }

    return result;
}

void ServiceCommunicationBase::apply_connection_data(const String & hostName, uint16_t portNr)
{
    mServerConnection.set_address( hostName, portNr );
}

bool ServiceCommunicationBase::connect_service_host()
{
    LOG_SCOPE( areg_aregextend_service_ServiceCommunicatonBase, connect_service_host );

    Lock lock(mLock);

    bool result{ false };
    if ( mServerConnection.is_valid() == false && is_running() == false )
    {
        if ( start( areg::WAIT_INFINITE ) && wait_start(areg::WAIT_INFINITE) )
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
    LOG_SCOPE( areg_aregextend_service_ServiceCommunicatonBase, reconnect_service_host );

    Lock lock(mLock);
    bool result = true;
    if (is_running() == false)
    {
        if (start(areg::WAIT_INFINITE) && wait_start(areg::WAIT_INFINITE))
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
    LOG_SCOPE( areg_aregextend_service_ServiceCommunicatonBase, disconnect_service_host );
    if ( is_running() )
    {
        send_command( ServiceEventData::ServiceCommand::CMD_ServiceExit, areg::EventPriority::HighPrio );
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

bool ServiceCommunicationBase::can_accept_connection(const SocketAccepted & clientSocket)
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

void ServiceCommunicationBase::connection_lost( SocketAccepted & clientSocket )
{
    LOG_SCOPE( areg_aregextend_service_ServiceCommunicatonBase, connection_lost );

    if ( mShuttingDown.load(std::memory_order_relaxed) )
    {
        LOG_DBG("Ignoring connection_lost during shutdown for socket [ %u ]", static_cast<uint32_t>(clientSocket.handle()));
        return;
    }

    const ITEM_ID & cookie { mServerConnection.cookie(clientSocket) };
    const ITEM_ID & channel{ mServerConnection.channel_id() };

    LOG_WARN("Client lost connection: cookie [ %u ], socket [ %d ], host [ %s : %d ], closing connection"
                , static_cast<uint32_t>(cookie)
                , clientSocket.handle()
                , clientSocket.address().host_address().as_string()
                , clientSocket.address().host_port());

    if ( cookie != areg::COOKIE_UNKNOWN )
    {
        mLostFn(cookie);
        remove_instance(cookie);
        areg::EventEnvelope msgDisconnect{ areg::create_disconnect_request(cookie, channel) };
        send_received_message(std::move(msgDisconnect), areg::EventPriority::HighPrio);
    }

    mServerConnection.close_connection(clientSocket);
}

void ServiceCommunicationBase::connection_failure()
{
    LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase, connection_failure);
    LOG_WARN("Host is [ %s ]", is_host_connected() ? "CONNECTED, reconnect service host" : "DISCONNECTED, doing nothing");

    if (is_host_connected())
    {
        reconnect_service_host();
    }
}

void ServiceCommunicationBase::disconnect_services()
{
    remove_all_instances();
}

void ServiceCommunicationBase::on_reconnect_timer()
{
    start_connection( );
}

void ServiceCommunicationBase::on_service_start()
{
    LOG_SCOPE( areg_aregextend_service_ServiceCommunicatonBase, on_service_start );
    mEventSendStop.reset();
    start_connection();
}

void ServiceCommunicationBase::on_service_stop()
{
    LOG_SCOPE( areg_aregextend_service_ServiceCommunicatonBase, on_service_stop);
    stop_connection();
    mEventSendStop.set_signaled();
}

void ServiceCommunicationBase::on_service_restart()
{
    LOG_SCOPE( areg_aregextend_service_ServiceCommunicatonBase, on_service_restart );
    restart_connection();
}

void ServiceCommunicationBase::on_service_exit()
{
    LOG_SCOPE( areg_aregextend_service_ServiceCommunicatonBase, on_service_exit );
    on_service_stop( );
    trigger_exit( );
}

void ServiceCommunicationBase::on_channel_connected(const ITEM_ID& /*cookie*/)
{
}

bool ServiceCommunicationBase::start_connection()
{
    LOG_SCOPE( areg_aregextend_service_ServiceCommunicatonBase, start_connection );
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

        // Pool mode: pre-start dedicated thread pairs before the global send/receive threads
        if ( mNumPairs > 0 )
        {
            ASSERT(mClientPairs.empty());
            mClientPairs.reserve(mNumPairs);
            bool poolOk = true;

            for ( uint32_t i = 0; i < mNumPairs; ++i )
            {
                areg::String sendName;
                areg::String recvName;
                sendName.format("AregPoolSend_%u", i);
                recvName.format("AregPoolRecv_%u", i);

                auto pair = std::make_unique<ClientConnectionPair>(
                                  static_cast<RemoteMessageHandler&>(self())
                                , mServerConnection
                                , mThreadSend
                                , mThreadReceive
                                , sendName.as_string()
                                , recvName.as_string() );

                if ( !pair->start() )
                {
                    LOG_ERR("Failed to start pool pair [ %u ], aborting pool creation", i);
                    poolOk = false;
                    break;
                }

                mClientPairs.push_back( std::move(pair) );
            }

            if ( !poolOk )
            {
                // Stop any pairs that started successfully, clear the list, and bail
                for ( auto & p : mClientPairs )
                {
                    if ( p ) p->stop();
                }

                mClientPairs.clear();
                mServerConnection.close_socket();

                LOG_WARN("Remote servicing failed (pool creation), trigger timer with [ %u ] ms timeout to retry", areg::DEFAULT_RETRY_CONNECT_TIMEOUT);
                mTimerConnect.start_timer(areg::DEFAULT_RETRY_CONNECT_TIMEOUT, static_cast<DispatcherThread &>(self()), 1);
                return false;
            }

            LOG_DBG("Started [ %u ] pool thread pairs successfully", mNumPairs);
        }

        if ( start_send_thread( ) && start_receive_thread( ) )
        {
            result = true;
            LOG_DBG( "The threads are created. Ready to send-receive messages." );
        }
        else
        {
            LOG_ERR( "Failed to create send-receive threads, cannot communicate. Stop remote service" );

            // Stop any pool pairs that were already started.
            for ( auto & p : mClientPairs )
            {
                if ( p ) p->stop();
            }

            mClientPairs.clear();
            mServerConnection.close_socket( );
        }
    }
    else
    {
        LOG_ERR("Failed to create remote servicing socket.");
    }

    if ( !result )
    {
        LOG_WARN("Remote servicing failed, trigger timer with [ %u ] ms timeout to re-establish remote servicing", areg::DEFAULT_RETRY_CONNECT_TIMEOUT);
        mTimerConnect.start_timer( areg::DEFAULT_RETRY_CONNECT_TIMEOUT, static_cast<DispatcherThread &>(self()), 1);
    }

    return result;
}

bool ServiceCommunicationBase::restart_connection()
{
    LOG_SCOPE( areg_aregextend_service_ServiceCommunicatonBase, restart_connection );
    LOG_DBG("Going to start connection. Address [ %s ], port [ %d ]"
                , mServerConnection.address().host_address().as_string()
                , mServerConnection.address().host_port());

    stop_connection();
    return start_connection();
}

void ServiceCommunicationBase::stop_connection()
{
    LOG_SCOPE( areg_aregextend_service_ServiceCommunicatonBase, stop_connection );
    LOG_WARN("Stopping remote servicing connection: connected instances [ %u ], send-thread running [ %s ], recv-thread running [ %s ], pool pairs [ %u ]"
                , mInstanceMap.size()
                , mThreadSend.is_running() ? "YES" : "NO"
                , mThreadReceive.is_running() ? "YES" : "NO"
                , static_cast<uint32_t>(mClientPairs.size()));

    mShuttingDown.store(true, std::memory_order_release);
    mThreadReceive.trigger_exit();
    ClientPairList pairsToStop;
    {
        Lock lock(mLock);
        pairsToStop = std::move(mClientPairs);
    }

    for ( auto & pair : pairsToStop )
    {
        if ( pair )
        {
            pair->stop_threads();
        }
    }

    mServerConnection.interrupt_connections();

    // Queue graceful disconnect notifications while sockets are still alive.
    disconnect_services();

    mThreadSend.trigger_exit();
    constexpr uint32_t DISCONNECT_TIMEOUT_MS{ 2500u };
    if ( !mThreadSend.wait_completion(DISCONNECT_TIMEOUT_MS) )
    {
        LOG_WARN("Send thread did not finish within %u ms -- interrupting connections to unblock", DISCONNECT_TIMEOUT_MS);
        mServerConnection.interrupt_connections();
    }

    mThreadSend.wait_completion( areg::WAIT_INFINITE );
    mServerConnection.close_socket();

    mThreadSend.shutdown( areg::WAIT_INFINITE );
    mThreadReceive.shutdown( areg::WAIT_INFINITE );

    for ( auto & pair : pairsToStop )
    {
        if ( pair )
        {
            pair->shutdown_threads( areg::WAIT_INFINITE );
        }
    }

    mShuttingDown.store(false, std::memory_order_release);
}

bool ServiceCommunicationBase::on_client_accepted( SocketAccepted & clientSocket )
{
    return mAcceptFn(clientSocket);
}

bool ServiceCommunicationBase::do_accept_client_shared( SocketAccepted & /*clientSocket*/ )
{
    return false;
}

bool ServiceCommunicationBase::do_accept_client_pool( SocketAccepted & clientSocket )
{
    LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase, do_accept_client_pool);

    if ( mShuttingDown.load(std::memory_order_acquire) )
    {
        LOG_WARN("do_accept_client_pool during shutdown for socket [ %u ], rejecting", static_cast<uint32_t>(clientSocket.handle()));
        return false;
    }

    const ITEM_ID cookie{ mServerConnection.cookie(clientSocket) };
    if ( cookie == areg::COOKIE_UNKNOWN )
    {
        LOG_WARN("do_accept_client_pool: no cookie assigned to socket [ %u ], falling back to shared path", static_cast<uint32_t>(clientSocket.handle()));
        return false;
    }

    if ( mClientPairs.empty() )
    {
        LOG_WARN("do_accept_client_pool: pool is empty (stopped?), falling back to shared path");
        return false;
    }

    mServerConnection.unregister_from_multiplexer(clientSocket.handle());

    const uint32_t idx = static_cast<uint32_t>(cookie) % mNumPairs;
    mClientPairs[idx]->add_socket(clientSocket, cookie);

    LOG_DBG("Pool mode: routed socket [ %u ] (cookie [ %u ]) to pool pair [ %u ]"
                , static_cast<uint32_t>(clientSocket.handle())
                , static_cast<uint32_t>(cookie)
                , idx);

    return true;
}

void ServiceCommunicationBase::do_client_lost_shared( ITEM_ID /*cookie*/ )
{
}

void ServiceCommunicationBase::do_client_lost_pool( ITEM_ID cookie )
{
    if ( mClientPairs.empty() )
        return;

    const uint32_t idx = static_cast<uint32_t>(cookie) % mNumPairs;
    mClientPairs[idx]->remove_socket(cookie);
}

bool ServiceCommunicationBase::do_send_shared( const areg::EventEnvelope & data, areg::EventPriority prio )
{
    areg::EventEnvelope copy{ data };
    return do_send_shared(std::move(copy), prio);
}

bool ServiceCommunicationBase::do_send_shared( areg::EventEnvelope && data, areg::EventPriority prio )
{
    areg::Event evt(std::move(data));
    evt.set_event_priority(prio);
    evt.set_event_consumer(&mThreadSend);
    evt.set_target_dispatcher(&mThreadSend);
    evt.deliver_event();
    return true;
}

bool ServiceCommunicationBase::do_send_pool( const areg::EventEnvelope & data, areg::EventPriority prio )
{
    if ( mClientPairs.empty() )
        return do_send_shared(data, prio);

    areg::EventEnvelope copy{ data };
    return do_send_pool(std::move(copy), prio);
}

bool ServiceCommunicationBase::do_send_pool( areg::EventEnvelope && data, areg::EventPriority prio )
{
    if ( mClientPairs.empty() )
        return do_send_shared(std::move(data), prio);

    const uint32_t idx{ data.target() % mNumPairs };
    PoolSendThread & sendThread{ mClientPairs[idx]->send_thread() };
    areg::Event evt(std::move(data));
    evt.set_event_priority(prio);
    evt.set_event_consumer(&sendThread);
    evt.set_target_dispatcher(&sendThread);
    evt.deliver_event();
    return true;
}


void ServiceCommunicationBase::enable_data_rate(bool enable) noexcept
{
    mThreadReceive.set_data_rate_enabled(enable);
    mThreadSend.set_data_rate_enabled(enable);
}

bool ServiceCommunicationBase::start_send_thread()
{
    return mThreadSend.start( areg::WAIT_INFINITE ) &&
           mThreadSend.wait_start( areg::WAIT_INFINITE );
}

bool ServiceCommunicationBase::start_receive_thread()
{
    return mThreadReceive.start( areg::WAIT_INFINITE ) &&
           mThreadReceive.wait_start( areg::WAIT_INFINITE );
}

bool ServiceCommunicationBase::send_message( const areg::EventEnvelope & data, areg::EventPriority eventPrio /*= areg::EventPriority::NormalPrio*/ )
{
    return mSendFn(data, eventPrio);
}

bool ServiceCommunicationBase::send_message( areg::EventEnvelope && data, areg::EventPriority eventPrio /*= areg::EventPriority::NormalPrio*/ )
{
    return mSendMoveFn(std::move(data), eventPrio);
}

#ifdef DEBUG
void ServiceCommunicationBase::failed_send_message(const areg::EventEnvelope & msgFailed, Socket & whichTarget )
#else  // DEBUG
void ServiceCommunicationBase::failed_send_message(const areg::EventEnvelope& /*msgFailed*/, Socket& whichTarget)
#endif // DEBUG
{
    LOG_SCOPE( areg_aregextend_service_ServiceCommunicatonBase, failed_send_message );

#ifdef DEBUG

    SocketAccepted target{ mServerConnection.target_client(msgFailed) };
    ASSERT( !whichTarget.is_valid() || !target.is_valid() || (whichTarget.handle( ) == target.handle()) );

#endif // DEBUG

    LOG_WARN("Failed to send message to [ %s ] client [ %d ], probably the connection is lost, closing connection"
                    , whichTarget.is_valid() ? "VALID" : "INVALID"
                    , static_cast<int32_t>(whichTarget.handle()));

    if ( whichTarget.is_valid())
    {
        connection_lost( static_cast<SocketAccepted &>(whichTarget) );
    }
}

void ServiceCommunicationBase::failed_receive_message(Socket & whichSource)
{
    LOG_SCOPE( areg_aregextend_service_ServiceCommunicatonBase, failed_receive_message );

    SocketAccepted client = mServerConnection.client_by_handle(whichSource.handle());
    LOG_WARN("Failed to receive message from [ %s ] client [ %d ], probably the connection with socket [ %d ] is lost, closing connection"
                        , client.is_valid() ? "VALID" : "INVALID"
                        , static_cast<int32_t>(client.handle())
                        , static_cast<int32_t>(whichSource.handle()));

    if (client.is_valid())
    {
        connection_lost(client);
    }
}

void ServiceCommunicationBase::process_received_message(areg::EventEnvelope & msgReceived, Socket & whichSource)
{
    DEBUG_LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase, process_received_message);

    if (!msgReceived.is_valid())
    {
        DEBUG_LOG_WARN("Received invalid message from source [ %u ], ignoring to process", msgReceived.source());
        return;
    }

    const uint32_t source{ msgReceived.source() };
    const uint32_t target{ msgReceived.target() };
    const areg::FuncIdRange msgId{ static_cast<areg::FuncIdRange>( msgReceived.message_id() ) };

    if ( (source >= static_cast<uint32_t>(areg::COOKIE_REMOTE_SERVICE)) && areg::is_executable_id(static_cast<uint32_t>(msgId)) )
    {
        if ( target != static_cast<uint32_t>(areg::TARGET_UNKNOWN) )
        {
#if defined(AREG_LOG_DEBUG) && (AREG_LOG_DEBUG != 0)
            // Log every 100k forwarded messages.
            static uint32_t s_fwdCount{ 0u };
            static uint32_t s_fwdBytes{ 0u };
            ++s_fwdCount;
            s_fwdBytes += msgReceived.size_used();
            if ((s_fwdCount % 100000u) == 0u)
            {
                DEBUG_LOG_INFO("Fast-path forward milestone: [ %u ] messages, [ %u ] bytes total (last msg: id=%u src=%u tgt=%u)"
                            , s_fwdCount
                            , s_fwdBytes
                            , static_cast<uint32_t>(msgId)
                            , source
                            , target);
            }
#endif  // defined(AREG_LOG_DEBUG) && (AREG_LOG_DEBUG != 0)

            send_message(std::move(msgReceived));
        }

        return;
    }

    // Slow path: system messages need the connection cookie.
    const ITEM_ID cookie{ mServerConnection.cookie(whichSource.handle()) };
    DEBUG_LOG_DBG("Received message [ %s ] of id [ %u ] from source [ %u ] ( connection cookie = %u ) of client host [ %s : %d ] for target [ %u ]"
                    , areg::as_string(msgId)
                    , static_cast<uint32_t>(msgId)
                    , source
                    , static_cast<uint32_t>(cookie)
                    , static_cast<const char *>(whichSource.address().host_address())
                    , static_cast<int32_t>(whichSource.address().host_port( ))
                    , target);

    if ( (source == static_cast<uint32_t>(cookie)) && (msgId != areg::FuncIdRange::SystemServiceConnect) )
    {
        if ( msgId == areg::FuncIdRange::SystemServiceDisconnect )
        {
            remove_instance( cookie );
            mServerConnection.close_connection( cookie );
        }

        send_received_message( std::move(msgReceived), areg::EventPriority::HighPrio );
    }
    else if ( (source == static_cast<uint32_t>(areg::SOURCE_UNKNOWN)) && (msgId == areg::FuncIdRange::SystemServiceConnect) )
    {
        areg::ConnectedInstance instance{};
        msgReceived >> instance;
        instance.ciTimestamp = static_cast<TIME64>(DateTime::now());
        instance.ciCookie = cookie;
        add_instance(cookie, instance);
        areg::EventEnvelope msgConnect{ connect_message(mServerConnection.channel_id(), cookie, areg::MessageSource::SourceService) };
        DEBUG_LOG_DBG("Received request connect message, sending response [ %s ] of id [ %u ], to new target [ %u ], connection socket [ %u ], checksum [ %u ]"
                    , areg::as_string( static_cast<areg::FuncIdRange>(msgConnect.message_id()))
                    , msgConnect.message_id()
                    , msgConnect.target()
                    , static_cast<uint32_t>(whichSource.handle())
                    , msgConnect.checksum());

        send_message( msgConnect );
    }
    else
    {
        DEBUG_LOG_WARN("Ignoring to process message [ %s ] of id [ %u ] from source [ %u ]"
                    , areg::as_string(msgId)
                    , static_cast<uint32_t>(msgId)
                    , source);
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

areg::EventEnvelope ServiceCommunicationBase::connect_message(const ITEM_ID & source, const ITEM_ID & target, areg::MessageSource msgSource) const
{
    areg::EventEnvelope result{ areg::create_connect_notify(source, target) };
    result.move_to_end();
    result << msgSource;
    return result;
}

areg::EventEnvelope ServiceCommunicationBase::disconnect_message( const ITEM_ID & source, const ITEM_ID & target ) const
{
    return areg::create_disconnect_notify(source, target);
}

} // namespace areg::ext
