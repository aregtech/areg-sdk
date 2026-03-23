/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
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
#include "areg/logging/areg_log.h"
namespace areg {

DEF_LOG_SCOPE(areg_ipc_private_ServiceClientConnectionBase, on_reconnect_timer);
DEF_LOG_SCOPE(areg_ipc_private_ServiceClientConnectionBase, on_service_start);
DEF_LOG_SCOPE(areg_ipc_private_ServiceClientConnectionBase, on_service_stop);
DEF_LOG_SCOPE(areg_ipc_private_ServiceClientConnectionBase, on_connection_started);
DEF_LOG_SCOPE(areg_ipc_private_ServiceClientConnectionBase, on_connection_stopped);
DEF_LOG_SCOPE(areg_ipc_private_ServiceClientConnectionBase, on_connection_lost);
DEF_LOG_SCOPE(areg_ipc_private_ServiceClientConnectionBase, on_channel_connected);
DEF_LOG_SCOPE(areg_ipc_private_ServiceClientConnectionBase, service_connection_event);
DEF_LOG_SCOPE(areg_ipc_private_ServiceClientConnectionBase, start_connection);
DEF_LOG_SCOPE(areg_ipc_private_ServiceClientConnectionBase, cancel_connection);

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
    : ConnectionProvider   ( )
    , ServiceEventConsumer    ( )

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
    , mLock                 ( )

    , mTimerConnect         ( static_cast<TimerConsumer &>(mTimerConsumer), prefixName + areg::CLIENT_CONNECT_TIMER_NAME )
    , mThreadReceive        (messageHandler, mClientConnection, prefixName)
    , mThreadSend           (messageHandler, mClientConnection, prefixName)
    , mTimerConsumer        ( static_cast<ServiceEventConsumer &>(self()) )
{
    ASSERT((target > areg::TARGET_LOCAL) && (target < areg::COOKIE_REMOTE_SERVICE));
}

ServiceClientConnectionBase::~ServiceClientConnectionBase()
{
}

void ServiceClientConnectionBase::service_connection_event(const RemoteMessage& msgReceived)
{
    LOG_SCOPE( areg_ipc_private_ServiceClientConnectionBase, service_connection_event );

    msgReceived.move_to_begin();
    ITEM_ID cookie{ areg::COOKIE_UNKNOWN };
    areg::ServiceConnectionState connection{ areg::ServiceConnectionState::Unknown };
    msgReceived >> cookie;
    msgReceived >> connection;
    LOG_DBG("Remote service connection notification: status [ %s ], cookie [ %llu ]", areg::as_string(connection), cookie);

    switch (connection)
    {
    case areg::ServiceConnectionState::Connected:
    case areg::ServiceConnectionState::Pending:
        {
            if (msgReceived.result() == areg::MESSAGE_SUCCESS)
            {
                Lock lock(mLock);
                ASSERT(cookie == msgReceived.target());
                mClientConnection.set_cookie(cookie);
                on_channel_connected(cookie);
                send_command(ServiceEventData::ServiceCommand::CMD_ServiceStarted);
            }
            else
            {
                cancel_connection();
                on_channel_connected(areg::COOKIE_UNKNOWN);
                send_command(ServiceEventData::ServiceCommand::CMD_ServiceLost);
            }
        }
        break;

    case areg::ServiceConnectionState::ConnectionLost:
    case areg::ServiceConnectionState::Disconnected:
    case areg::ServiceConnectionState::Failed:
        {
            cancel_connection();
            on_channel_connected(areg::COOKIE_UNKNOWN);
            send_command(ServiceEventData::ServiceCommand::CMD_ServiceLost);
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

    return mClientConnection.set_address(config.connection_address(), config.connection_port());
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
    send_command(ServiceEventData::ServiceCommand::CMD_ServiceExit, areg::EventPriority::NormalPrio);
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

RemoteMessage ServiceClientConnectionBase::connect_message(const ITEM_ID & source, const ITEM_ID & target, areg::MessageSource msgSource) const
{
    return areg::create_connect_request(source, target, msgSource);
}

RemoteMessage ServiceClientConnectionBase::disconnect_message(const ITEM_ID & source, const ITEM_ID & target) const
{
    return areg::create_disconnect_request(source, target);
}

void ServiceClientConnectionBase::on_reconnect_timer()
{
    LOG_SCOPE( areg_ipc_private_ServiceClientConnectionBase, on_reconnect_timer );
    on_service_start( );
}

void ServiceClientConnectionBase::on_service_start()
{
    LOG_SCOPE( areg_ipc_private_ServiceClientConnectionBase, on_service_start );
    LOG_DBG("Starting remote servicing");

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
    LOG_SCOPE( areg_ipc_private_ServiceClientConnectionBase, on_service_stop );
    LOG_DBG("Stopping remote servicing");

    set_connection_state(ConnectionPhase::ConnectionStopping);

    mTimerConnect.stop_timer( );

    Channel channel{ mChannel };
    mChannel.invalidate();

    mThreadReceive.trigger_exit( );

    if ((channel.target() > areg::COOKIE_LOCAL) && (channel.target() < areg::COOKIE_REMOTE_SERVICE))
    {
        send_message(disconnect_message(channel.cookie(), mTarget));
    }

    disconnect_service( areg::EventPriority::NormalPrio );

    mThreadSend.wait_completion( areg::WAIT_INFINITE );
    mThreadSend.shutdown( areg::DO_NOT_WAIT );
    mClientConnection.close_socket( );
    mThreadReceive.shutdown( areg::WAIT_INFINITE );

    mConnectionConsumer.on_service_channel_disconnected( channel );
}

void ServiceClientConnectionBase::on_service_restart()
{
    on_service_stop( );
    on_service_start( );
}

void ServiceClientConnectionBase::on_connection_started()
{
    LOG_SCOPE( areg_ipc_private_ServiceClientConnectionBase, on_connection_started );
    ASSERT(is_connection_started());
    if ( mClientConnection.cookie() != areg::COOKIE_LOCAL )
    {
        LOG_DBG("Succeeded to start router service client, cookie [ %llu ]", mClientConnection.cookie());

        mChannel.set_cookie( mClientConnection.cookie() );
        mChannel.set_source( mMessageDispatcher.id());
        mChannel.set_target( mTarget );
        set_connection_state(ConnectionPhase::ConnectionStarted);
        mConnectionConsumer.on_service_channel_connected(mChannel);
    }
}

void ServiceClientConnectionBase::on_connection_stopped()
{
    LOG_SCOPE( areg_ipc_private_ServiceClientConnectionBase, on_connection_stopped );
    LOG_DBG("Client service is stopped. Resetting cookie");

    set_connection_state(ConnectionPhase::ConnectionStopped);
    mTimerConnect.stop_timer( );

    Channel channel = mChannel;
    mChannel.invalidate();

    cancel_connection( );

    mThreadReceive.shutdown( areg::WAIT_INFINITE );
    mThreadSend.shutdown( areg::WAIT_INFINITE );
    mConnectionConsumer.on_service_channel_disconnected( channel );

    if ( Application::is_servicing_ready( ) )
    {
        mTimerConnect.start_timer(areg::DEFAULT_RETRY_CONNECT_TIMEOUT, mMessageDispatcher, 1 );
    }
}

void ServiceClientConnectionBase::on_connection_lost()
{
    LOG_SCOPE( areg_ipc_private_ServiceClientConnectionBase, on_connection_lost );
    LOG_WARN("Client service lost connection. Resetting cookie and trying to restart, current connection state [ %s ]"
                , ServiceClientConnectionBase::as_string(connection_state()));

    set_connection_state(ConnectionPhase::ConnectionStopped);
    Channel channel = mChannel;
    mChannel.invalidate();

    if (!Application::is_servicing_ready() || !mTimerConnect.is_stopped())
    {
        ASSERT(!mThreadReceive.is_running());
        ASSERT(!mThreadSend.is_running());
        ASSERT(!mClientConnection.is_valid());
        LOG_WARN("Ignoring lost connection event, either servicing state is not allowed, or application is closing.");
        return;
    }

    LOG_DBG( "Restarting lost connection with remote service" );
    mThreadReceive.shutdown( areg::WAIT_INFINITE );
    mThreadSend.shutdown( areg::WAIT_INFINITE );
    mConnectionConsumer.on_service_channel_lost( channel );
    mTimerConnect.start_timer(areg::DEFAULT_RETRY_CONNECT_TIMEOUT, mMessageDispatcher, 1);
}

void ServiceClientConnectionBase::on_service_exit()
{
    on_service_stop( );
}

void ServiceClientConnectionBase::on_message_received( const RemoteMessage & /* msgReceived */ )
{
}

void ServiceClientConnectionBase::on_message_send( const RemoteMessage & /* msgSend */ )
{
}

void ServiceClientConnectionBase::on_channel_connected(const ITEM_ID& cookie)
{
    LOG_SCOPE( areg_ipc_private_ServiceClientConnectionBase, on_channel_connected );
    Lock lock(mLock);

    if (cookie < areg::COOKIE_REMOTE_SERVICE)
    {
        LOG_INFO("Disconnecting remote channel [ source = %llu, target = %llu, cookie = %llu ]", mChannel.source(), mChannel.target(), mChannel.cookie());
        mChannel.invalidate();
        return;
    }

    mChannel.set_cookie(cookie);
    mChannel.set_source(mMessageDispatcher.id());
    mChannel.set_target(mTarget);
    LOG_DBG("Connected remote channel [ source = %llu, target = %llu, cookie = %llu ]", mChannel.source(), mChannel.target(), mChannel.cookie());
}

bool ServiceClientConnectionBase::start_connection()
{
    LOG_SCOPE( areg_ipc_private_ServiceClientConnectionBase, start_connection );

    ASSERT(mClientConnection.address().is_valid());
    ASSERT(!mClientConnection.is_valid());
    ASSERT(!mThreadReceive.is_running());
    ASSERT(!mThreadSend.is_running());

    mTimerConnect.stop_timer();

    bool result{ mClientConnection.create_socket() &&
                 mThreadReceive.start(areg::WAIT_INFINITE) &&
                 mThreadSend.start(areg::WAIT_INFINITE) };

    if (result)
    {
        VERIFY( mThreadReceive.wait_start( areg::WAIT_INFINITE ) );
        VERIFY( mThreadSend.wait_start( areg::WAIT_INFINITE ) );
        LOG_DBG("Client service starting connection with remote routing service.");
        result = mClientConnection.send_message(connect_message(areg::COOKIE_UNKNOWN, mTarget, mMessageSource));
    }

    if (!result)
    {
        LOG_WARN("Client service failed to start connection, going to repeat connection in [ %u ] ms", areg::DEFAULT_RETRY_CONNECT_TIMEOUT);
        mThreadSend.shutdown( areg::DO_NOT_WAIT );
        mThreadReceive.shutdown( areg::DO_NOT_WAIT );
        mClientConnection.close_socket();
        mTimerConnect.start_timer(areg::DEFAULT_RETRY_CONNECT_TIMEOUT, mMessageDispatcher, 1);
    }

    return result;
}

void ServiceClientConnectionBase::cancel_connection()
{
    LOG_SCOPE( areg_ipc_private_ServiceClientConnectionBase, cancel_connection );
    LOG_WARN("Canceling client service connection");

    mClientConnection.close_socket();
    mClientConnection.set_cookie( areg::COOKIE_UNKNOWN );

    mThreadReceive.shutdown( areg::DO_NOT_WAIT );
    mThreadSend.shutdown( areg::DO_NOT_WAIT );
}

} // namespace areg
