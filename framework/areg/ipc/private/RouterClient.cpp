/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/ipc/private/RouterClient.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Service Connection client implementation
 ************************************************************************/
#include "areg/ipc/private/RouterClient.hpp"

#include "areg/ipc/RegistrationConsumer.hpp"
#include "areg/ipc/private/ConnectionDefs.hpp"

#include "areg/component/RemoteEventFactory.hpp"
#include "areg/component/StreamableEvent.hpp"
#include "areg/component/ResponseEvents.hpp"
#include "areg/component/RequestEvents.hpp"
#include "areg/component/ServiceDefs.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/base/Process.hpp"
#include "areg/logging/areg_log.h"
namespace areg {

DEF_LOG_SCOPE(areg_ipc_private_RouterClient, failed_send_message);
DEF_LOG_SCOPE(areg_ipc_private_RouterClient, failed_receive_message);
DEF_LOG_SCOPE(areg_ipc_private_RouterClient, failed_process_message);

DEF_LOG_SCOPE(areg_ipc_private_RouterClient, process_request_event);
DEF_LOG_SCOPE(areg_ipc_private_RouterClient, process_response_event);
DEF_LOG_SCOPE(areg_ipc_private_RouterClient, process_notify_request);

DEF_LOG_SCOPE(areg_ipc_private_RouterClient, register_service_provider);
DEF_LOG_SCOPE(areg_ipc_private_RouterClient, unregister_service_provider);
DEF_LOG_SCOPE(areg_ipc_private_RouterClient, register_service_consumer);
DEF_LOG_SCOPE(areg_ipc_private_RouterClient, unregister_service_consumer);

#if defined(AREG_LOG_DEBUG) && (AREG_LOG_DEBUG != 0)
    DEF_LOG_SCOPE(areg_ipc_private_RouterClient, process_received_message);
#endif  // defined(AREG_LOG_DEBUG) && (AREG_LOG_DEBUG != 0)

//////////////////////////////////////////////////////////////////////////
// RouterClient class implementation
//////////////////////////////////////////////////////////////////////////

RouterClient::RouterClient(ConnectionConsumer& connectionConsumer, RegistrationConsumer& registerConsumer)
    : ServiceClientConnectionBase   ( areg::COOKIE_ROUTER
                                    , areg::RemoteServiceKind::Router
                                    , static_cast<uint32_t>(areg::ConnectionType::Tcpip)
                                    , areg::MessageSource::SourceClient
                                    , connectionConsumer
                                    , static_cast<RemoteMessageHandler &>(self())
                                    , static_cast<DispatcherThread &>(self())
                                    , RouterClient::PREFIX_THREAD)
    , RegistrationProvider  ( )
    , DispatcherThread      (String(RouterClient::PREFIX_THREAD) + areg::CLIENT_DISPATCH_MESSAGE_THREAD, areg::SYSTEM_THREAD_STACK_BIG, areg::QUEUE_SIZE_MAXIMUM)
    , RemoteEventConsumer   ( )

    , mRegisterConsumer     (registerConsumer)
{
}

bool RouterClient::connect_service_host()
{
    bool result{ true };
    if (is_running() == false)
    {
        if (start(areg::WAIT_INFINITE) && wait_start(areg::WAIT_INFINITE))
        {
            result = ServiceClientConnectionBase::connect_service_host();
        }
        else
        {
            shutdown(areg::WAIT_INFINITE);
        }
    }
    else if (mClientConnection.is_valid() == false)
    {
        result = ServiceClientConnectionBase::connect_service_host();
    }
    else
    {
        result = false;
    }

    return result;
}

void RouterClient::disconnect_service_host()
{
    if (is_running())
    {
        ServiceClientConnectionBase::disconnect_service_host();
        wait_completion(areg::WAIT_INFINITE);
        shutdown(areg::DO_NOT_WAIT);
    }
}

void RouterClient::on_service_exit()
{
    ServiceClientConnectionBase::on_service_exit();
    trigger_exit();
}

bool RouterClient::is_host_pending() const
{
    Lock lock(mLock);
    return (is_running() && ((mClientConnection.is_valid() == false) || (connection_state() == ServiceClientConnectionBase::ConnectionPhase::ConnectionStarting)));
}

bool RouterClient::register_service_provider( const StubAddress & stubService )
{
    LOG_SCOPE( areg_ipc_private_RouterClient, register_service_provider );
    Lock lock( mLock );
    bool result{ false };
    if ( is_connection_started() )
    {
        LOG_DBG("Queuing to send register [ %s ] service message by connection [ %d ]"
                   , StubAddress::to_path(stubService).as_string()
                   , mClientConnection.cookie());

        result = send_message(areg::router_register_service(stubService, mClientConnection.cookie(), areg::COOKIE_ROUTER), areg::EventPriority::HighPrio );
    }

    return result;
}

void RouterClient::unregister_service_provider(const StubAddress & stubService, const areg::DisconnectReason reason )
{
    LOG_SCOPE( areg_ipc_private_RouterClient, unregister_service_provider );

    Lock lock( mLock );
    if ( is_connection_started() )
    {
        LOG_DBG("Queuing to send unregister [ %s ] service message by connection [ %d ]"
                   , StubAddress::to_path(stubService).as_string()
                   , mClientConnection.cookie());

        send_message(areg::router_unregister_service(stubService, reason, mClientConnection.cookie(), areg::COOKIE_ROUTER) );
    }
}

bool RouterClient::register_service_consumer(const ProxyAddress & proxyService)
{
    LOG_SCOPE( areg_ipc_private_RouterClient, register_service_consumer );
    Lock lock( mLock );
    bool result { false };
    if ( is_connection_started() )
    {
        LOG_DBG("Queuing to send register [ %s ] service client message by connection [ %d ]"
                   , ProxyAddress::to_path(proxyService).as_string()
                   , mClientConnection.cookie());

        result = send_message(areg::router_register_consumer(proxyService, mClientConnection.cookie(), areg::COOKIE_ROUTER), areg::EventPriority::HighPrio);
    }

    return result;
}

void RouterClient::unregister_service_consumer(const ProxyAddress & proxyService, const areg::DisconnectReason reason )
{
    LOG_SCOPE( areg_ipc_private_RouterClient, unregister_service_consumer );

    Lock lock( mLock );
    if ( is_connection_started() )
    {
        LOG_DBG("Queuing to send unregister [ %s ] service client message by connection [ %d ]"
                   , ProxyAddress::to_path(proxyService).as_string()
                   , mClientConnection.cookie());

        send_message(areg::router_unregister_consumer(proxyService, reason, mClientConnection.cookie(), areg::COOKIE_ROUTER) );
    }
}

void RouterClient::failed_send_message(const RemoteMessage & msgFailed, Socket & whichTarget )
{
    LOG_SCOPE( areg_ipc_private_RouterClient, failed_send_message );

    if (Application::is_servicing_ready())
    {
        uint32_t msgId{ msgFailed.message_id() };
        if ( areg::is_executable_id(msgId) || areg::is_connect_id(msgId) )
        {
            LOG_WARN("Failed to send message [ %u ] to target [ %llu ], source is [ %llu ], the target socket [ %u ] is [ %s : %s ]"
                       , msgId
                       , msgFailed.target()
                       , msgFailed.source()
                       , static_cast<uint32_t>(whichTarget.handle())
                       , whichTarget.is_valid() ? "VALID" : "INVALID"
                       , whichTarget.is_alive() ? "ALIVE" : "DEAD");

            msgFailed.move_to_begin();
            StreamableEvent * eventError = RemoteEventFactory::request_failed_event(msgFailed, mChannel);
            if ( eventError != nullptr )
            {
                LOG_DBG("Replying with failure event [ %s ]", eventError->class_string());
                eventError->deliver_event();
            }

            if ( whichTarget.is_valid() && (whichTarget.is_alive() == false))
            {
                LOG_DBG("Trying to reconnect");
                cancel_connection( );
                send_command( ServiceEventData::ServiceCommand::CMD_ServiceLost, areg::EventPriority::NormalPrio );
            }
        }
        else
        {
            LOG_WARN("The failed message, it is neither executable, nor connection notification. Ignoring to generate request failed event.");
        }
    }
    else
    {
        LOG_WARN("Ignore send message failure, the application is closing");
    }
}

void RouterClient::failed_receive_message( Socket & whichSource )
{
    LOG_SCOPE( areg_ipc_private_RouterClient, failed_receive_message );

    if (Application::is_servicing_ready())
    {
        if (whichSource.is_valid())
        {
            LOG_WARN("Failed to receive message from socket [ %lu ], which [ %s : %s ], going to reconnect"
                       , static_cast<uint32_t>(whichSource.handle())
                       , whichSource.is_valid() ? "VALID" : "INVALID"
                       , whichSource.is_alive() ? "ALIVE" : "DEAD");
            cancel_connection();
            send_command(ServiceEventData::ServiceCommand::CMD_ServiceLost, areg::EventPriority::NormalPrio);
        }
        else
        {
            LOG_WARN("Ignoring sending reconnect event, the socket is invalid");
        }
    }
    else
    {
        LOG_WARN("Ignore receive message failure, the application is closing");
    }
}

void RouterClient::failed_process_message( const RemoteMessage & msgUnprocessed )
{
    LOG_SCOPE( areg_ipc_private_RouterClient, failed_process_message );

    if (Application::is_servicing_ready())
    {
        uint32_t msgId{ msgUnprocessed.message_id() };
        if ( areg::is_executable_id(msgId) )
        {
            LOG_DBG("The message [ %u ] for target [ %llu ] and from source [ %llu ] is unprocessed, replying with failed message"
                      , msgId
                      , msgUnprocessed.target()
                      , msgUnprocessed.source());

            msgUnprocessed.move_to_begin();
            StreamableEvent * eventError = RemoteEventFactory::request_failed_event(msgUnprocessed, mChannel);
            if ( eventError != nullptr )
            {
                RemoteMessage data;
                if ( RemoteEventFactory::stream_from_event( data, *eventError, mChannel) )
                {
                    send_message(data);
                }
            }
        }
        else
        {
            LOG_WARN("The unprocessed message is neither executable, nor connection notification. Ignoring to generate request failed event.");
        }
    }
    else
    {
        LOG_WARN("Ignore processing failure message, the application is closing");
    }
}

void RouterClient::process_received_message( RemoteMessage & msgReceived, Socket & whichSource )
{
#if defined(AREG_LOG_DEBUG) && (AREG_LOG_DEBUG != 0)
    LOG_SCOPE( areg_ipc_private_RouterClient, process_received_message );
    if (!msgReceived.is_valid() || !whichSource.is_valid())
    {
        LOG_WARN("Invalid message from host [ %s : %u ], ignore processing"
                    , whichSource.address().host_address().as_string()
                    , whichSource.address().host_port( ) );
        return;
    }
#else  // defined(AREG_LOG_DEBUG) && (AREG_LOG_DEBUG != 0)
    if (!msgReceived.is_valid() || !whichSource.is_valid())
    {
        return;
    }
#endif  // defined(AREG_LOG_DEBUG) && (AREG_LOG_DEBUG != 0)

    areg::FuncIdRange msgId{ static_cast<areg::FuncIdRange>(msgReceived.message_id()) };
    areg::MessageResult result{ static_cast<areg::MessageResult>(msgReceived.result()) };
#if defined(AREG_LOG_DEBUG) && (AREG_LOG_DEBUG != 0)
    LOG_DBG("Processing received valid message [ %u ], result [ %s ]", msgId, areg::as_string(result));
#endif  // defined(AREG_LOG_DEBUG) && (AREG_LOG_DEBUG != 0)

    switch ( msgId )
    {
    case areg::FuncIdRange::SystemServiceNotifyConnection:
        service_connection_event(msgReceived);
        break;

    case areg::FuncIdRange::SystemServiceNotifyRegister:
    {
        ASSERT( mClientConnection.cookie() == msgReceived.target() );
        areg::RegistrationAction reqType;
        msgReceived >> reqType;
#if defined(AREG_LOG_DEBUG) && (AREG_LOG_DEBUG != 0)
        LOG_DBG("Remote routing service registration notification of type [ %s ]", areg::as_string(reqType));
#endif  // defined(AREG_LOG_DEBUG) && (AREG_LOG_DEBUG != 0)

        switch ( reqType )
        {
        case areg::RegistrationAction::RegisterConsumer:
        {
            ProxyAddress proxy{ msgReceived };
            areg::DisconnectReason reason { areg::DisconnectReason::UndefinedReason };
            msgReceived >> reason;
            proxy.set_source( mChannel.source() );
            if ( result == areg::MessageResult::Succeed )
            {
                mRegisterConsumer.on_consumer_registered(proxy);
            }
            else
            {
                mRegisterConsumer.on_consumer_unregistered(proxy, reason, areg::COOKIE_ANY);
            }
        }
        break;

        case areg::RegistrationAction::RegisterProvider:
        {
            StubAddress stub(msgReceived);
            stub.set_source( mChannel.source() );
            if ( result == areg::MessageResult::Succeed )
            {
                mRegisterConsumer.on_provider_registered( stub );
            }
            else
            {
                mRegisterConsumer.on_provider_unregistered( stub, areg::DisconnectReason::UndefinedReason, areg::COOKIE_ANY );
            }
        }
        break;

        case areg::RegistrationAction::UnregisterConsumer:
        {
            ProxyAddress proxy{ msgReceived };
            areg::DisconnectReason reason { areg::DisconnectReason::UndefinedReason };
            msgReceived >> reason;
            proxy.set_source( mChannel.source() );
            mRegisterConsumer.on_consumer_unregistered(proxy, reason, areg::COOKIE_ANY);
        }
        break;

        case areg::RegistrationAction::UnregisterProvider:
        {
            StubAddress stub(msgReceived);
            areg::DisconnectReason reason{areg::DisconnectReason::UndefinedReason};
            msgReceived >> reason;
            stub.set_source( mChannel.source() );
            mRegisterConsumer.on_provider_unregistered(stub, reason, areg::COOKIE_ANY);
        }
        break;

        default:
            ASSERT(false);
            break;
        }
    }
    break;

    case areg::FuncIdRange::ServiceLastId:                    // fall through
    case areg::FuncIdRange::SystemServiceQueryInstances:      // fall through
    case areg::FuncIdRange::SystemServiceRequestRegister:     // fall through
    case areg::FuncIdRange::SystemServiceDisconnect:          // fall through
    case areg::FuncIdRange::SystemServiceConnect:             // fall through
    case areg::FuncIdRange::ResponseServiceProviderConnection:// fall through
    case areg::FuncIdRange::RequestServiceProviderConnection: // fall through
    case areg::FuncIdRange::ResponseServiceProviderVersion:   // fall through
    case areg::FuncIdRange::RequestServiceProviderVersion:    // fall through
    case areg::FuncIdRange::RequestRegisterService:           // fall through
    case areg::FuncIdRange::ComponentCleanup:                 // fall through
    case areg::FuncIdRange::SystemServiceNotifyInstances:     // fall through
    case areg::FuncIdRange::ServiceLogRegisterScopes:         // fall through
    case areg::FuncIdRange::ServiceLogUpdateScopes:           // fall through
    case areg::FuncIdRange::ServiceLogQueryScopes:            // fall through
    case areg::FuncIdRange::ServiceLogScopesUpdated:          // fall through
    case areg::FuncIdRange::ServiceSaveLogConfiguration:      // fall through
    case areg::FuncIdRange::ServiceLogConfigurationSaved:     // fall through
    case areg::FuncIdRange::ServiceLogMessage:                // fall through
        break;

    case areg::FuncIdRange::AttributeLastId:          // fall through
    case areg::FuncIdRange::AttributeFirstId:         // fall through
    case areg::FuncIdRange::ResponseLastId:           // fall through
    case areg::FuncIdRange::ResponseFirstId:          // fall through
    case areg::FuncIdRange::RequestLastId:            // fall through
    case areg::FuncIdRange::RequestFirstId:           // fall through
    case areg::FuncIdRange::EmptyFunctionId:          // fall through
    default:
    {
        if ( areg::is_executable_id(static_cast<uint32_t>(msgId)) )
        {
            StreamableEvent * eventRemote = RemoteEventFactory::event_from_stream(msgReceived, mChannel);
            if ( eventRemote != nullptr )
            {
                eventRemote->deliver_event();
            }
            else
            {
                failed_process_message(msgReceived);
            }
        }
#if defined(AREG_LOG_DEBUG) && (AREG_LOG_DEBUG != 0)
        else
        {
            LOG_WARN("The message [ %u ] was not processed on client service side", msgId);
        }
#endif  // defined(AREG_LOG_DEBUG) && (AREG_LOG_DEBUG != 0)
    }
    break;
    }
}


void RouterClient::process_request_event( RemoteRequestEvent & reqEvent)
{
    LOG_SCOPE( areg_ipc_private_RouterClient, process_request_event );

    if ( reqEvent.is_remote() )
    {
        RemoteMessage data;
        if ( RemoteEventFactory::stream_from_event( data, reqEvent, mChannel) )
        {
            LOG_DBG("Sending [ %s ] event: remote message [ %u ] from source [ %llu ] to target [ %llu ]"
                      , reqEvent.class_string()
                      , data.message_id()
                      , data.source()
                      , data.target());

            send_message(data);
        }
        else
        {
            LOG_ERR("Failed to create remote request data with message [ %u ]", reqEvent.request_id() );
        }
    }
    else
    {
        LOG_WARN("Request event with message [ %u ] is not remote, ignoring sending event", reqEvent.request_id());
    }
}

void RouterClient::process_notify_request( RemoteNotifyRequestEvent & reqNotifyEvent )
{
    LOG_SCOPE( areg_ipc_private_RouterClient, process_notify_request );

    if ( reqNotifyEvent.is_remote() )
    {
        RemoteMessage data;
        if ( RemoteEventFactory::stream_from_event( data, reqNotifyEvent, mChannel) )
        {
            LOG_DBG("Send [ %s ] event: remote message [ %u ] from source [ %llu ] to target [ %llu ]"
                      , reqNotifyEvent.class_string()
                      , data.message_id()
                      , data.source()
                      , data.target());

            send_message(data);
        }
        else
        {
            LOG_ERR("Failed to create remote notify request message [ %u ]", reqNotifyEvent.request_id() );
        }
    }
    else
    {
        LOG_WARN("Notify request [ %u ] is not remote, ignoring sending event", reqNotifyEvent.request_id());
    }
}


void RouterClient::process_response_event(RemoteResponseEvent & respEvent)
{
    LOG_SCOPE( areg_ipc_private_RouterClient, process_response_event );

    if ( respEvent.is_remote() )
    {
        RemoteMessage data;
        if ( RemoteEventFactory::stream_from_event( data, respEvent, mChannel) )
        {
            LOG_DBG("Forwarding [ %s ] message [ %u ] from source [ %llu ] to target [ %llu ]"
                      , respEvent.class_string()
                      , data.message_id()
                      , data.source()
                      , data.target());

            send_message(data);
        }
        else
        {
            LOG_ERR("Failed to create remote response message [ %u ]", respEvent.response_id() );
        }
    }
    else
    {
        LOG_WARN("Response event with message [ %u ] is not remote, ignoring", respEvent.response_id());
    }
}

bool RouterClient::post_event(Event & eventElem)
{
    if ( eventElem.is_remote() )
    {
        eventElem.set_event_consumer( static_cast<RemoteEventConsumer *>(this) );
    }

    return EventDispatcher::post_event(eventElem);
}

void RouterClient::ready_for_events(bool is_ready)
{
    if (is_ready)
    {
        register_consumer_commands();
        DispatcherThread::ready_for_events(true);
        set_connection_state(ServiceClientConnectionBase::ConnectionPhase::DisconnectState);
    }
    else
    {
        DispatcherThread::ready_for_events(false);
        set_connection_state(ServiceClientConnectionBase::ConnectionPhase::ConnectionStopped);
        unregister_consumer_commands();
    }
}

} // namespace areg
