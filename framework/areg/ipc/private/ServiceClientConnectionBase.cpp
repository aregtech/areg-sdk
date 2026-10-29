/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/ipc/private/ServiceClientConnectionBase.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Service Connection client implementation
 ************************************************************************/
#include "areg/ipc/ServiceClientConnectionBase.hpp"

#include "areg/component/ServiceDefs.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/ipc/ConnectionConsumer.hpp"
#include "areg/ipc/ConnectionConfiguration.hpp"
#include "areg/ipc/RemoteServiceDefs.hpp"
#include "areg/ipc/private/ConnectionDefs.hpp"

#include "areg/base/private/DebugDefs.hpp"

namespace areg {
//////////////////////////////////////////////////////////////////////////
// Local helper methods
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Waits until the I/O thread enters its dispatcher loop or exits prematurely.
 *
 * \param   ioThread    The message receive or send thread to wait for.
 * \return  Returns true if the thread dispatcher is started and ready for events.
 **/
static bool _wait_io_thread_ready( DispatcherThread & ioThread )
{
    while ( !ioThread.wait_start( areg::WAIT_10_MILLISECONDS ) )
    {
        if ( !ioThread.is_running() )
            return false;
    }

    return true;
}

/**
 * \brief   Stops the send thread, waits for it and closes the socket, under the lock that also
 *          guards the start of the send thread. The receive thread must be stopped before.
 *
 * \param   startLock   The lock of the send thread's start and stop.
 * \param   sendThread  The send thread to stop.
 * \param   connection  The connection whose socket is closed.
 * \param   closing     True to discard what the thread still holds.
 **/
static void _close_connection( Mutex & startLock, ClientSendThread & sendThread, ClientConnection & connection, bool closing )
{
    Lock lock( startLock );
    if ( closing )
    {
        sendThread.set_closing();
    }

    sendThread.shutdown( areg::WAIT_INFINITE );
    connection.close_socket();
}

//////////////////////////////////////////////////////////////////////////
// ServiceClientConnectionBase class implementation
//////////////////////////////////////////////////////////////////////////

ServiceClientConnectionBase::ServiceClientConnectionBase( const ITEM_ID & target
                                                        , areg::RemoteServiceKind service
                                                        , uint32_t connectTypes
                                                        , areg::MessageSource msgSource
                                                        , ConnectionConsumer& connectionConsumer
                                                        , RemoteMessageHandler & messageHandler
                                                        , DispatcherThread & messageDispatcher
                                                        , const String & prefixName)
    : ConnectionProvider    ( )
    , ServiceEventConsumer  ( )
    , ReconnectTimerConsumer(static_cast<ServiceEventConsumer&>(*this))

    , mTarget               (target)
    , mService              (service)
    , mConnectTypes         (connectTypes)
    , mMessageSource        (msgSource)
    , mClientConnection     ( )
    , mConnectionConsumer   (connectionConsumer)
    , mMessageDispatcher    (messageDispatcher)
    , mChannel              ( )
    , mConnectionState      ( ConnectionPhase::ConnectionStopped )
    , mEventConsumer        ( static_cast<ServiceEventConsumer &>(self()) )
    , mMessageConsumer      ( static_cast<ServiceEventConsumer &>(self()) )
    , mLock                 ( )

    , mTimerConnect         ( static_cast<TimerConsumer &>(self()), prefixName + areg::CLIENT_CONNECT_TIMER_NAME, areg::INVALID_TIMEOUT, Timer::IGNORE_TIMER_QUEUE, areg::EventPriority::HighPrio )
    , mThreadReceive        (messageHandler, mClientConnection, prefixName)
    , mThreadSend           (messageHandler, mClientConnection, prefixName)
    , mSendStartLock        ( false )
{
    ASSERT((target > areg::TARGET_LOCAL) && (target < areg::COOKIE_REMOTE_SERVICE));
}

ServiceClientConnectionBase::~ServiceClientConnectionBase()
{
}

void ServiceClientConnectionBase::ensure_send_thread()
{
    Lock lock( mSendStartLock );
    if ( mThreadSend.is_running() )
        return;

    // The readiness poll stays inside the lock, so a second producer finds the thread ready.
    // On failure nothing is torn down: the receive side restarts the connection.
    static_cast<void>(mThreadSend.start( areg::WAIT_INFINITE ) && _wait_io_thread_ready( mThreadSend ));
}

bool ServiceClientConnectionBase::try_send_inline( MessageEnvelope & data )
{
    if ( mThreadSend.send_gate().is_clear() == false )
        return false;

    areg::EventHeader * hdr{ data.header() };
    if ( hdr == nullptr )
        return false;

    const uint32_t wireSize{ areg::wire_size(*hdr) };
    if ( wireSize > areg::INLINE_SEND_MAX_BYTES )
        return false;

    const SOCKETHANDLE hSocket{ mClientConnection.live_handle() };
    if ( areg::is_valid_socket(hSocket) == false )
        return false;

    if ( areg::EventDispatcherBase::take_inline_send_credit() == false )
        return false;

    areg::SocketWriter & writer{ areg::SocketWriter::writer_of(hSocket) };
    if ( writer.try_acquire() == false )
        return false;

    // The socket is closed only after the writer lock is released.
    if ( mClientConnection.live_handle() != hSocket )
    {
        writer.release();
        return false;
    }

    hdr->internal1 = 0u;
    hdr->internal2 = 0u;
    ASSERT( hdr->custom == 0u );

    int32_t sent{ 0 };
    {
        AREG_LT_SCOPE(areg::LtStage::SendSyscall);
        const areg::IoBuffer ioBuffer{ areg::wire_data(*hdr), wireSize };
        sent = areg::try_send_data_v(hSocket, &ioBuffer, 1u, wireSize);
    }

    if ( sent > 0 )
    {
        writer.release();
        mThreadSend.accumulate_sent(static_cast<uint64_t>(sent), 1u);
        return true;
    }
    else if ( sent == 0 )
    {
        writer.release();
        return false;
    }

    // A negative result may follow a partial write: the first bytes are already on the wire,
    // so the message cannot be queued again and the connection has to go.
    mThreadSend.report_failed_send(data, mClientConnection.socket());
    writer.release();
    return true;
}

void ServiceClientConnectionBase::service_connection_event(const MessageEnvelope& msgReceived)
{
    msgReceived.move_to_begin();
    ITEM_ID cookie{ areg::COOKIE_UNKNOWN };
    areg::ServiceConnectionState connection{ areg::ServiceConnectionState::Unknown };
    msgReceived >> cookie;
    msgReceived >> connection;

    switch (connection)
    {
    case areg::ServiceConnectionState::Connected:
    case areg::ServiceConnectionState::Pending:
    {
        if (msgReceived.result() == areg::MESSAGE_SUCCESS)
        {
            Lock lock(mLock);
            ASSERT(cookie == static_cast<ITEM_ID>(msgReceived.target()));
            mClientConnection.set_cookie(cookie);
            on_channel_connected(cookie);
            send_command(ServiceEventData::ServiceCommand::CMD_ServiceStarted);
        }
        else
        {
            on_channel_connected(areg::COOKIE_UNKNOWN);
            notify_connection_lost();
        }
    }
    break;

    case areg::ServiceConnectionState::ConnectionLost:
    case areg::ServiceConnectionState::Disconnected:
    case areg::ServiceConnectionState::Failed:
    {
        on_channel_connected(areg::COOKIE_UNKNOWN);
        notify_connection_lost();
    }
    break;

    case areg::ServiceConnectionState::Unknown:   // fall through
    case areg::ServiceConnectionState::Rejected:            // fall through
    case areg::ServiceConnectionState::Shutdown:            // fall through
    default:
    {
        cancel_connection();
        on_channel_connected(areg::COOKIE_UNKNOWN);
        send_command(ServiceEventData::ServiceCommand::CMD_ServiceStopped);
    }
    break;
    }
}

bool ServiceClientConnectionBase::is_connection_allowed() const
{
    return Application::is_servicing_available();
}

bool ServiceClientConnectionBase::setup_connection_data(areg::RemoteServiceKind service, uint32_t connectTypes)
{
    Lock lock( mLock );

    if ((mService != service) || ((mConnectTypes & connectTypes) == 0))
        return false;

    if ((mConnectTypes & static_cast<uint32_t>(areg::ConnectionType::Tcpip)) == 0)
        return false;

    ConnectionConfiguration config(service, areg::ConnectionType::Tcpip);
    if (!config.is_configured() || !config.connection_enable_flag())
        return false;

    const bool result = mClientConnection.set_address(config.connection_address(), config.connection_port());
    mClientConnection.set_socket_buffers(config.socket_send_buffer(), config.socket_recv_buffer());
    mClientConnection.set_send_timeout(config.socket_refusal_timeout());
    return result;
}

void ServiceClientConnectionBase::apply_connection_data( const String & hostName, uint16_t portNr )
{
    Lock lock( mLock );
    mClientConnection.set_address( hostName, portNr );
}

bool ServiceClientConnectionBase::connect_service_host()
{
    Lock lock( mLock );

    if (mClientConnection.is_valid())
        return false;

    if ((mConnectTypes & static_cast<uint32_t>(areg::ConnectionType::Tcpip)) == 0)
        return false;

    ConnectionConfiguration config(mService, areg::ConnectionType::Tcpip);
    if (!config.is_configured() || !config.connection_enable_flag())
        return false;

    mClientConnection.set_socket_buffers(config.socket_send_buffer(), config.socket_recv_buffer());
    mClientConnection.set_send_timeout(config.socket_refusal_timeout());
    send_command(ServiceEventData::ServiceCommand::CMD_StartService);
    return true;
}

bool ServiceClientConnectionBase::reconnect_service_host()
{
    disconnect_service_host( );
    send_command(ServiceEventData::ServiceCommand::CMD_StartService );

    return true;
}

void ServiceClientConnectionBase::disconnect_service_host()
{
    send_command(ServiceEventData::ServiceCommand::CMD_ServiceExit, areg::EventPriority::CriticalPrio);
}

bool ServiceClientConnectionBase::is_host_connected() const
{
    Lock lock( mLock );
    return is_connection_started();
}

bool ServiceClientConnectionBase::is_host_pending() const
{
    Lock lock(mLock);
    return (!mClientConnection.is_valid() && (connection_state() == ConnectionPhase::ConnectionStarting));
}

bool ServiceClientConnectionBase::is_host_setup() const
{
    Lock lock(mLock);
    return mClientConnection.address().is_valid();
}

MessageEnvelope ServiceClientConnectionBase::connect_message(const ITEM_ID & source, const ITEM_ID & target, areg::MessageSource msgSource) const
{
    return areg::create_connect_request(source, target, msgSource);
}

MessageEnvelope ServiceClientConnectionBase::disconnect_message(const ITEM_ID & source, const ITEM_ID & target) const
{
    return areg::create_disconnect_request(source, target);
}

void ServiceClientConnectionBase::on_reconnect_timer()
{
    mTimerConnect.stop_timer( );
    on_service_start( );
}

void ServiceClientConnectionBase::on_service_start()
{
    if (connection_state() == ConnectionPhase::ConnectionStopping)
    {
        return;
    }

    if (!is_connection_allowed())
    {
        // The application did not reach the servicing state yet. Retry later
        mTimerConnect.stop_timer();
        static_cast<void>(mTimerConnect.start_timer(areg::DEFAULT_RETRY_CONNECT_TIMEOUT, mMessageDispatcher, 1));
        return;
    }

    mChannel.set_cookie( areg::COOKIE_LOCAL );
    mChannel.set_source( areg::SOURCE_UNKNOWN );
    mChannel.set_target( areg::TARGET_UNKNOWN );
    if ( start_connection( ) )
    {
        set_connection_state( ConnectionPhase::ConnectionStarting );
    }
}

void ServiceClientConnectionBase::on_service_stop()
{
    set_connection_state(ConnectionPhase::ConnectionStopping);

    mTimerConnect.stop_timer( );

    Channel channel{ mChannel };
    mChannel.invalidate();

    mThreadReceive.trigger_exit( );

    if ((channel.target() > areg::COOKIE_LOCAL) && (channel.target() < areg::COOKIE_REMOTE_SERVICE))
    {
        send_message(disconnect_message(channel.cookie(), mTarget));
    }

    do
    {
        // The send thread is started and stopped only under this lock.
        Lock lock( mSendStartLock );
        mThreadSend.set_closing();
        mThreadSend.trigger_exit_drained();
        if (!mThreadSend.wait_completion( areg::SEND_QUEUE_FLUSH_TIMEOUT ))
        {
            mThreadSend.trigger_exit();
            mThreadSend.wait_completion( areg::WAIT_INFINITE );
        }

        mThreadSend.shutdown( areg::WAIT_INFINITE );
    } while (false);

    mClientConnection.interrupt_socket( );
    mThreadReceive.shutdown( areg::WAIT_INFINITE );
    _close_connection( mSendStartLock, mThreadSend, mClientConnection, true );

    mMessageDispatcher.remove_event_type( ServiceClientEvent::CLASS_ID );

    mConnectionConsumer.on_service_channel_disconnected( channel );
}

void ServiceClientConnectionBase::on_service_restart()
{
    on_service_stop( );
    set_connection_state( ConnectionPhase::ConnectionStopped );
    on_service_start( );
}

void ServiceClientConnectionBase::on_connection_started()
{
    Channel channel;
    do
    {
        Lock lock(mLock);

        // The connection may be canceled between the handshake acknowledgment and this event;
        if ( !is_connection_started() )
        {
            return;
        }

        mChannel.set_cookie( mClientConnection.cookie() );
        mChannel.set_source( mMessageDispatcher.number() );
        mChannel.set_target( mTarget );
        set_connection_state(ConnectionPhase::ConnectionStarted);
        channel = mChannel;
    } while (false);

    mConnectionConsumer.on_service_channel_connected(channel);
}

void ServiceClientConnectionBase::on_connection_stopped()
{
    // Capture the state before changing it
    const ConnectionPhase prevState{ connection_state() };
    set_connection_state(ConnectionPhase::ConnectionStopped);
    mTimerConnect.stop_timer( );

    Channel channel = mChannel;
    mChannel.invalidate();

    cancel_connection( );

    mThreadReceive.shutdown( areg::WAIT_INFINITE );
    _close_connection( mSendStartLock, mThreadSend, mClientConnection, false );
    mConnectionConsumer.on_service_channel_disconnected( channel );

    if ( is_connection_allowed( ) && (prevState != ConnectionPhase::ConnectionStopping) )
    {
        if (!mTimerConnect.start_timer(areg::DEFAULT_RETRY_CONNECT_TIMEOUT, mMessageDispatcher, 1))
        {
            send_command(ServiceEventData::ServiceCommand::CMD_StartService);
        }
    }
}

void ServiceClientConnectionBase::on_connection_lost()
{
    if (mClientConnection.is_valid())
    {
        return;
    }

    const ConnectionPhase prevState{ connection_state() };
    set_connection_state(ConnectionPhase::ConnectionStopped);
    Channel channel = mChannel;
    mChannel.invalidate();

    // The socket is interrupted; it is closed once both I/O threads stopped.
    mThreadReceive.shutdown( areg::WAIT_INFINITE );
    _close_connection( mSendStartLock, mThreadSend, mClientConnection, false );

    if (!is_connection_allowed() || !mTimerConnect.is_stopped() ||
        (prevState == ConnectionPhase::ConnectionStopping))
    {
        return;
    }

    mConnectionConsumer.on_service_channel_lost( channel );
    if (!mTimerConnect.start_timer(areg::DEFAULT_RETRY_CONNECT_TIMEOUT, mMessageDispatcher, 1))
    {
        send_command(ServiceEventData::ServiceCommand::CMD_StartService);
    }
}

void ServiceClientConnectionBase::on_service_exit()
{
    on_service_stop( );
}

void ServiceClientConnectionBase::on_message_received( const MessageEnvelope & /* msgReceived */ )
{
}

void ServiceClientConnectionBase::on_message_send( const MessageEnvelope & /* msgSend */ )
{
}

void ServiceClientConnectionBase::on_channel_connected(const ITEM_ID& cookie)
{
    Lock lock(mLock);

    if (cookie < areg::COOKIE_REMOTE_SERVICE)
    {
        mChannel.invalidate();
        return;
    }

    mChannel.set_cookie(cookie);
    mChannel.set_source( mMessageDispatcher.number() );
    mChannel.set_target(mTarget);
}

bool ServiceClientConnectionBase::start_connection()
{
    ASSERT(mClientConnection.address().is_valid());
    ASSERT(!mClientConnection.is_valid());
    ASSERT(!mThreadReceive.is_running());

    mTimerConnect.stop_timer();

    // The send thread may still run, started by a message sent while the previous connection was
    // already gone. It is stopped, and the new socket is created before it can start again.
    bool created{ false };
    do
    {
        Lock lock( mSendStartLock );
        mThreadSend.set_closing();
        mThreadSend.shutdown( areg::WAIT_INFINITE );
        mClientConnection.close_socket();
        created = mClientConnection.create_socket_fd();
    } while (false);

    if ( !created )
    {
        if (!mTimerConnect.start_timer(areg::DEFAULT_RETRY_CONNECT_TIMEOUT, mMessageDispatcher, 1))
        {
            if (is_connection_allowed())
                send_command(ServiceEventData::ServiceCommand::CMD_StartService);
        }

        return false;
    }

    // Store handshake in the receive thread before starting it.
    mThreadReceive.set_handshake(connect_message(areg::COOKIE_UNKNOWN, mTarget, mMessageSource));

    // The send thread is not started here: it is a fallback, and the first message that has to
    // be queued starts it, see ensure_send_thread().
    bool result{ mThreadReceive.start(areg::WAIT_INFINITE) &&
                 _wait_io_thread_ready(mThreadReceive) };

    if (!result)
    {
        // The interrupt ends blocked socket calls, so the shutdown barriers below are bounded.
        mClientConnection.interrupt_socket();
        mThreadReceive.shutdown( areg::WAIT_INFINITE );
        _close_connection( mSendStartLock, mThreadSend, mClientConnection, false );
        if (!mTimerConnect.start_timer(areg::DEFAULT_RETRY_CONNECT_TIMEOUT, mMessageDispatcher, 1))
        {
            if (is_connection_allowed())
                send_command(ServiceEventData::ServiceCommand::CMD_StartService);
        }
    }

    return result;
}

void ServiceClientConnectionBase::cancel_connection()
{
    // The interrupt ends blocked receive / send calls of the I/O threads.
    mClientConnection.interrupt_socket();

    mThreadReceive.trigger_exit();
    mThreadSend.trigger_exit();
}

void ServiceClientConnectionBase::notify_connection_lost( areg::EventPriority eventPrio )
{
    // Interrupt the socket BEFORE queuing the lost-connection command
    cancel_connection();
    send_command( ServiceEventData::ServiceCommand::CMD_ServiceLost, eventPrio );
}

} // namespace areg
