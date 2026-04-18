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
    , mTimerConsumer    ( self() )
    , mInstanceMap      (  )
    , mEventSendStop    ( false, false )
    , mLock             ( )
    , mSendFn           ( )
    , mSendMoveFn       ( )
    , mAcceptFn         ( )
    , mLostFn           ( )
{
    if (mNumPairs == 0)
    {
        mSendFn     = [this](const RemoteMessage & d, areg::EventPriority p) { return do_send_shared(d, p); };
        mSendMoveFn = [this](RemoteMessage && d,       areg::EventPriority p) { return do_send_shared(std::move(d), p); };
        mAcceptFn   = [this](areg::SocketAccepted & s)                        { return do_accept_client_shared(s); };
        mLostFn     = [this](ITEM_ID c)                                       { do_client_lost_shared(c); };
    }
    else
    {
        mSendFn     = [this](const RemoteMessage & d, areg::EventPriority p) { return do_send_pool(d, p); };
        mSendMoveFn = [this](RemoteMessage && d,       areg::EventPriority p) { return do_send_pool(std::move(d), p); };
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
    bool result{ false };
    if ((mService == service) && ((mConnectTypes & connectTypes) != 0))
    {
        if ((mConnectTypes & static_cast<uint32_t>(areg::ConnectionType::Tcpip)) != 0)
        {
            ConnectionConfiguration config(mService, areg::ConnectionType::Tcpip);
            if (config.is_configured() && config.connection_enable_flag())
            {
                String address{ config.connection_address() };
                uint16_t port{ config.connection_port() };
                result = mServerConnection.set_address(address, port);
                mServerConnection.set_socket_buffers(config.socket_send_buffer(), config.socket_recv_buffer());
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

    // Suppress spurious callbacks during deliberate shutdown (stop_connection() closes
    // all sockets which wakes blocked recv threads, causing them to call this method).
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

    // In pool mode, remove the client from the pool pair's local map and multiplexer
    // so it stops being monitored.  In shared mode this is a no-op.
    if ( cookie != areg::COOKIE_UNKNOWN )
    {
        mLostFn(cookie);
        remove_instance(cookie);
        RemoteMessage msgDisconnect = areg::create_disconnect_request(cookie, channel);
        // Use HighPrio so crash-detected disconnects are processed with the same urgency
        // as graceful disconnects.
        send_communication_message(ServiceEventData::ServiceCommand::CMD_ServiceReceivedMsg, msgDisconnect, areg::EventPriority::HighPrio);
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

        // Pool mode: pre-start dedicated thread pairs before the global send/receive threads.
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
                                  static_cast<ConnectionHandler&>(self())
                                , static_cast<RemoteMessageHandler&>(self())
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
                // Stop any pairs that started successfully, clear the list, and bail.
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

    // Set the flag BEFORE closing sockets so connection_lost() callbacks triggered by
    // the socket interrupts are silently ignored instead of re-enqueueing disconnect work.
    mShuttingDown.store(true, std::memory_order_release);

    // Signal the global receive thread to exit.
    mThreadReceive.trigger_exit();

    // Snapshot and clear the pool list so the threads can be stopped outside the lock.
    ClientPairList pairsToStop;
    {
        Lock lock(mLock);
        pairsToStop = std::move(mClientPairs);
    }

    // Signal all pool receive threads to stop (resets each multiplexer → unblocks wait()).
    // Signal all pool send threads to exit before interrupting connections so they can
    // finish draining any already-queued events.
    for ( auto & pair : pairsToStop )
    {
        if ( pair )
        {
            pair->stop_threads();
        }
    }

    // Interrupt every accepted socket BEFORE stopping the per-slot send threads.
    // Under high data rate a send thread may be blocked permanently inside ::send()
    // (the remote TCP receive buffer is full and the kernel call never returns).
    // trigger_exit() posts the EXIT event but the thread never reaches it while blocked.
    // ::shutdown(SD_BOTH) on every accepted socket causes the blocked ::send() to return
    // with an error.  _do_send() detects the error and the send thread processes the
    // EXIT event and exits cleanly.
    mServerConnection.interrupt_connections();

    // Queue graceful disconnect notifications while sockets are still alive.
    disconnect_services();
    disconnect_service( areg::EventPriority::HighPrio);

    // Give the global send thread a bounded window to deliver queued disconnect notifications.
    // If it times out, an unresponsive client is holding up the send thread — interrupt again.
    constexpr uint32_t DISCONNECT_DRAIN_TIMEOUT_MS{ 5000u };
    if ( !mThreadSend.wait_completion( DISCONNECT_DRAIN_TIMEOUT_MS ) )
    {
        LOG_WARN("Send thread did not finish within %u ms -- interrupting connections to unblock", DISCONNECT_DRAIN_TIMEOUT_MS);
        mServerConnection.interrupt_connections();
    }

    mThreadSend.wait_completion( areg::WAIT_INFINITE );

    // Reset the multiplexer (unblocks global receive thread), interrupts remaining
    // sockets, clears all maps, and closes the server socket.
    mServerConnection.close_socket();

    mThreadSend.shutdown( areg::WAIT_INFINITE );
    mThreadReceive.shutdown( areg::WAIT_INFINITE );

    // Wait for all pool send and receive threads to finish.
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
    // Shared mode: socket stays on the global mThreadReceive — nothing to do.
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

    // Unregister from the global ServerReceiveThread multiplexer — the pool receive
    // thread will own this socket from here on.
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
    // Shared mode: socket remains on mThreadReceive; no extra cleanup needed.
}

void ServiceCommunicationBase::do_client_lost_pool( ITEM_ID cookie )
{
    if ( mClientPairs.empty() )
        return;

    const uint32_t idx = static_cast<uint32_t>(cookie) % mNumPairs;
    mClientPairs[idx]->remove_socket(cookie);
}

bool ServiceCommunicationBase::do_send_shared( const RemoteMessage & data, areg::EventPriority prio )
{
    return SendMessageEvent::send_event( SendMessageEventData( data )
                                       , static_cast<SendMessageEventConsumer &>(mThreadSend)
                                       , static_cast<DispatcherThread &>(mThreadSend)
                                       , prio );
}

bool ServiceCommunicationBase::do_send_shared( RemoteMessage && data, areg::EventPriority prio )
{
    SendMessageEvent * evt = SendMessageEvent::make_event(prio);
    if ( evt == nullptr )
        return false;

    evt->data() = SendMessageEventData(std::move(data));
    return SendMessageEvent::send_event(evt
                                       , static_cast<SendMessageEventConsumer &>(mThreadSend)
                                       , static_cast<DispatcherThread &>(mThreadSend)
                                       , prio);
}

bool ServiceCommunicationBase::do_send_pool( const RemoteMessage & data, areg::EventPriority prio )
{
    if ( mClientPairs.empty() )
        return do_send_shared(data, prio);

    const uint32_t idx = static_cast<uint32_t>(data.target()) % mNumPairs;
    ClientSendThread & sendThread = mClientPairs[idx]->send_thread();
    return SendMessageEvent::send_event( SendMessageEventData( data )
                                       , static_cast<SendMessageEventConsumer &>(sendThread)
                                       , static_cast<DispatcherThread &>(sendThread)
                                       , prio );
}

bool ServiceCommunicationBase::do_send_pool( RemoteMessage && data, areg::EventPriority prio )
{
    if ( mClientPairs.empty() )
        return do_send_shared(std::move(data), prio);

    const uint32_t idx = static_cast<uint32_t>(data.target()) % mNumPairs;
    ClientSendThread & sendThread = mClientPairs[idx]->send_thread();

    SendMessageEvent * evt = SendMessageEvent::make_event(prio);
    if ( evt == nullptr )
        return false;

    evt->data() = SendMessageEventData(std::move(data));
    return SendMessageEvent::send_event(evt
                                       , static_cast<SendMessageEventConsumer &>(sendThread)
                                       , static_cast<DispatcherThread &>(sendThread)
                                       , prio);
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

bool ServiceCommunicationBase::send_message( const RemoteMessage & data, areg::EventPriority eventPrio /*= areg::EventPriority::NormalPrio*/ )
{
    return mSendFn(data, eventPrio);
}

bool ServiceCommunicationBase::send_message( RemoteMessage && data, areg::EventPriority eventPrio /*= areg::EventPriority::NormalPrio*/ )
{
    return mSendMoveFn(std::move(data), eventPrio);
}

#ifdef DEBUG
void ServiceCommunicationBase::failed_send_message(const RemoteMessage & msgFailed, Socket & whichTarget )
#else  // DEBUG
void ServiceCommunicationBase::failed_send_message(const RemoteMessage& /*msgFailed*/, Socket& whichTarget)
#endif // DEBUG
{
    LOG_SCOPE( areg_aregextend_service_ServiceCommunicatonBase, failed_send_message );

#ifdef DEBUG

    ASSERT( !whichTarget.is_valid() || (whichTarget.handle( ) == mServerConnection.target_client(msgFailed).handle()) );

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

void ServiceCommunicationBase::process_received_message(RemoteMessage & msgReceived, Socket & whichSource)
{
    DEBUG_LOG_SCOPE(areg_aregextend_service_ServiceCommunicatonBase, process_received_message);

    if (!msgReceived.is_valid())
    {
        DEBUG_LOG_WARN("Received invalid message from source [ %u ], ignoring to process", static_cast<uint32_t>(msgReceived.source()));
        return;
    }

    const ITEM_ID source{ msgReceived.source() };
    const ITEM_ID target{ msgReceived.target() };
    const areg::FuncIdRange msgId{ static_cast<areg::FuncIdRange>( msgReceived.message_id() ) };

    if ( (source >= areg::COOKIE_REMOTE_SERVICE) && areg::is_executable_id(static_cast<uint32_t>(msgId)) )
    {
        if ( target != areg::TARGET_UNKNOWN )
        {
            // Skip the redundant client_by_cookie() validity pre-check here.
            // The send thread's _do_send() resolves the socket and silently drops
            // the message if the target has since disconnected.  Doing an extra
            // exclusive-SpinLock lookup on mLock from the receive thread's hot
            // path costs ~200–400 ns per forwarded message and doubles the lock
            // contention on mLock without providing any correctness guarantee
            // (the target can disconnect between this check and the actual send).
#if defined(AREG_LOG_DEBUG) && (AREG_LOG_DEBUG != 0)
            // Milestone log every 100k forwarded messages.
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
                            , static_cast<uint32_t>(source)
                            , static_cast<uint32_t>(target));
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
                    , static_cast<uint32_t>(source)
                    , static_cast<uint32_t>(cookie)
                    , static_cast<const char *>(whichSource.address().host_address())
                    , static_cast<int32_t>(whichSource.address().host_port( ))
                    , static_cast<id_type>(target));

    if ( (source == cookie) && (msgId != areg::FuncIdRange::SystemServiceConnect) )
    {
        if ( msgId == areg::FuncIdRange::SystemServiceDisconnect )
        {
            remove_instance( cookie );
            // Close the TCP connection immediately in the receive thread.
            mServerConnection.close_connection( cookie );
        }

        // Dispatch system messages at high priority.
        send_communication_message( ServiceEventData::ServiceCommand::CMD_ServiceReceivedMsg, msgReceived, areg::EventPriority::HighPrio );
    }
    else if ( (source == areg::SOURCE_UNKNOWN) && (msgId == areg::FuncIdRange::SystemServiceConnect) )
    {
        areg::ConnectedInstance instance{};
        msgReceived >> instance;
        instance.ciTimestamp = static_cast<TIME64>(DateTime::now());
        instance.ciCookie = cookie;
        add_instance(cookie, instance);
        RemoteMessage msgConnect(connect_message(mServerConnection.channel_id(), cookie, areg::MessageSource::SourceService));
        DEBUG_LOG_DBG("Received request connect message, sending response [ %s ] of id [ %u ], to new target [ %u ], connection socket [ %u ], checksum [ %u ]"
                    , areg::as_string( static_cast<areg::FuncIdRange>(msgConnect.message_id()))
                    , static_cast<uint32_t>(msgConnect.message_id())
                    , static_cast<uint32_t>(msgConnect.target())
                    , static_cast<uint32_t>(whichSource.handle())
                    , msgConnect.checksum());

        send_message( msgConnect );
    }
    else
    {
        DEBUG_LOG_WARN("Ignoring to process message [ %s ] of id [ %u ] from source [ %u ]"
                    , areg::as_string(msgId)
                    , static_cast<uint32_t>(msgId)
                    , static_cast<uint32_t>(source));
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

RemoteMessage ServiceCommunicationBase::connect_message(const ITEM_ID & source, const ITEM_ID & target, areg::MessageSource msgSource) const
{
    RemoteMessage result{ areg::create_connect_notify(source, target) };
    result.move_to_end();
    result << msgSource;
    return result;
}

RemoteMessage ServiceCommunicationBase::disconnect_message( const ITEM_ID & source, const ITEM_ID & target ) const
{
    return areg::create_disconnect_notify(source, target);
}

} // namespace areg::ext
