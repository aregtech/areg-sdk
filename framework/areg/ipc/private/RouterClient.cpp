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
#include "areg/logging/GELog.h"

DEF_LOG_SCOPE(areg_ipc_private_RouterClient_failedSendMessage);
DEF_LOG_SCOPE(areg_ipc_private_RouterClient_failedReceiveMessage);
DEF_LOG_SCOPE(areg_ipc_private_RouterClient_failedProcessMessage);
DEF_LOG_SCOPE(areg_ipc_private_RouterClient_processReceivedMessage);

DEF_LOG_SCOPE(areg_ipc_private_RouterClient_processRemoteRequestEvent);
DEF_LOG_SCOPE(areg_ipc_private_RouterClient_processRemoteResponseEvent);
DEF_LOG_SCOPE(areg_ipc_private_RouterClient_processRemoteNotifyRequestEvent);

DEF_LOG_SCOPE(areg_ipc_private_RouterClient_registerServiceProvider);
DEF_LOG_SCOPE(areg_ipc_private_RouterClient_unregisterServiceProvider);
DEF_LOG_SCOPE(areg_ipc_private_RouterClient_registerServiceConsumer);
DEF_LOG_SCOPE(areg_ipc_private_RouterClient_unregisterServiceConsumer);

//////////////////////////////////////////////////////////////////////////
// RouterClient class implementation
//////////////////////////////////////////////////////////////////////////

RouterClient::RouterClient(ConnectionConsumer& connectionConsumer, RegistrationConsumer& registerConsumer)
    : ServiceClientConnectionBase   ( NEService::COOKIE_ROUTER
                                    , NERemoteService::RemoteServiceKind::Router
                                    , static_cast<uint32_t>(NERemoteService::ConnectionType::Tcpip)
                                    , NEService::MessageSource::SourceClient
                                    , connectionConsumer
                                    , static_cast<RemoteMessageHandler &>(self())
                                    , static_cast<DispatcherThread &>(self())
                                    , RouterClient::PREFIX_THREAD)
    , RegistrationProvider     ( )
    , DispatcherThread              (String(RouterClient::PREFIX_THREAD) + NEConnection::CLIENT_DISPATCH_MESSAGE_THREAD, NECommon::STACK_SIZE_DEFAULT, NECommon::QUEUE_SIZE_MAXIMUM)
    , RemoteEventConsumer         ( )

    , mRegisterConsumer (registerConsumer)
{
}

bool RouterClient::connect_service_host()
{
    bool result{ true };
    if (is_running() == false)
    {
        if (create_thread(NECommon::WAIT_INFINITE) && wait_start(NECommon::WAIT_INFINITE))
        {
            result = ServiceClientConnectionBase::connect_service_host();
        }
        else
        {
            shutdown_thread(NECommon::WAIT_INFINITE);
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
        completion_wait(NECommon::WAIT_INFINITE);
        shutdown_thread(NECommon::DO_NOT_WAIT);
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
    LOG_SCOPE(areg_ipc_private_RouterClient_registerServiceProvider);
    Lock lock( mLock );
    bool result{ false };
    if ( is_connection_started() )
    {
        LOG_DBG("Queuing to send register [ %s ] service message by connection [ %d ]"
                   , StubAddress::to_path(stubService).as_string()
                   , mClientConnection.cookie());

        result = send_message(NERemoteService::router_register_service(stubService, mClientConnection.cookie(), NEService::COOKIE_ROUTER), Event::EventPriority::HighPrio );
    }

    return result;
}

void RouterClient::unregister_service_provider(const StubAddress & stubService, const NEService::DisconnectReason reason )
{
    LOG_SCOPE(areg_ipc_private_RouterClient_unregisterServiceProvider);

    Lock lock( mLock );
    if ( is_connection_started() )
    {
        LOG_DBG("Queuing to send unregister [ %s ] service message by connection [ %d ]"
                   , StubAddress::to_path(stubService).as_string()
                   , mClientConnection.cookie());

        send_message(NERemoteService::router_unregister_service(stubService, reason, mClientConnection.cookie(), NEService::COOKIE_ROUTER) );
    }
}

bool RouterClient::register_service_consumer(const ProxyAddress & proxyService)
{
    LOG_SCOPE(areg_ipc_private_RouterClient_registerServiceConsumer );
    Lock lock( mLock );
    bool result { false };
    if ( is_connection_started() )
    {
        LOG_DBG("Queuing to send register [ %s ] service client message by connection [ %d ]"
                   , ProxyAddress::to_path(proxyService).as_string()
                   , mClientConnection.cookie());

        result = send_message(NERemoteService::router_register_client(proxyService, mClientConnection.cookie(), NEService::COOKIE_ROUTER), Event::EventPriority::HighPrio);
    }

    return result;
}

void RouterClient::unregister_service_consumer(const ProxyAddress & proxyService, const NEService::DisconnectReason reason )
{
    LOG_SCOPE(areg_ipc_private_RouterClient_unregisterServiceConsumer);

    Lock lock( mLock );
    if ( is_connection_started() )
    {
        LOG_DBG("Queuing to send unregister [ %s ] service client message by connection [ %d ]"
                   , ProxyAddress::to_path(proxyService).as_string()
                   , mClientConnection.cookie());

        send_message(NERemoteService::router_unregister_client(proxyService, reason, mClientConnection.cookie(), NEService::COOKIE_ROUTER) );
    }
}

void RouterClient::failed_send_message(const RemoteMessage & msgFailed, Socket & whichTarget )
{
    LOG_SCOPE(areg_ipc_private_RouterClient_failedSendMessage);

    if (Application::is_servicing_ready())
    {
        uint32_t msgId{ msgFailed.message_id() };
        if ( NEService::is_executable_id(msgId) || NEService::is_connect_id(msgId) )
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
                LOG_DBG("Replying with failure event [ %s ]", eventError->runtime_class_name().as_string());
                eventError->deliver_event();
            }

            if ( whichTarget.is_valid() && (whichTarget.is_alive() == false))
            {
                LOG_DBG("Trying to reconnect");
                cancel_connection( );
                send_command( ServiceEventData::ServiceCommand::CMD_ServiceLost, Event::EventPriority::NormalPrio );
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
    LOG_SCOPE(areg_ipc_private_RouterClient_failedReceiveMessage);

    if (Application::is_servicing_ready())
    {
        if (whichSource.is_valid())
        {
            LOG_WARN("Failed to receive message from socket [ %lu ], which [ %s : %s ], going to reconnect"
                       , static_cast<uint32_t>(whichSource.handle())
                       , whichSource.is_valid() ? "VALID" : "INVALID"
                       , whichSource.is_alive() ? "ALIVE" : "DEAD");
            cancel_connection();
            send_command(ServiceEventData::ServiceCommand::CMD_ServiceLost, Event::EventPriority::NormalPrio);
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
    LOG_SCOPE(areg_ipc_private_RouterClient_failedProcessMessage);

    if (Application::is_servicing_ready())
    {
        uint32_t msgId{ msgUnprocessed.message_id() };
        if ( NEService::is_executable_id(msgId) )
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

void RouterClient::process_received_message( const RemoteMessage & msgReceived, Socket & whichSource )
{
    LOG_SCOPE(areg_ipc_private_RouterClient_processReceivedMessage);
    if ( msgReceived.is_valid() && whichSource.is_valid() )
    {
        NEService::FuncIdRange msgId{ static_cast<NEService::FuncIdRange>(msgReceived.message_id()) };
        NEMemory::MessageResult result{ static_cast<NEMemory::MessageResult>(msgReceived.result()) };
        LOG_DBG("Processing received valid message [ %u ], result [ %s ]", msgId, NEMemory::as_string(result));

        switch ( msgId )
        {
        case NEService::FuncIdRange::SystemServiceNotifyConnection:
            service_connection_event(msgReceived);
            break;

        case NEService::FuncIdRange::SystemServiceNotifyRegister:
            {
                ASSERT( mClientConnection.cookie() == msgReceived.target() );
                NEService::RegistrationAction reqType;
                msgReceived >> reqType;
                LOG_DBG("Remote routing service registration notification of type [ %s ]", NEService::as_string(reqType));

                switch ( reqType )
                {
                case NEService::RegistrationAction::RegisterClient:
                    {
                        ProxyAddress proxy(msgReceived);
                        NEService::DisconnectReason reason { NEService::DisconnectReason::UndefinedReason };
                        msgReceived >> reason;
                        proxy.set_source( mChannel.source() );
                        if ( result == NEMemory::MessageResult::Succeed )
                        {
                            mRegisterConsumer.on_consumer_registered(proxy);
                        }
                        else
                        {
                            mRegisterConsumer.on_consumer_unregistered(proxy, reason, NEService::COOKIE_ANY);
                        }
                    }
                    break;

                case NEService::RegistrationAction::RegisterStub:
                    {
                        StubAddress stub(msgReceived);
                        stub.set_source( mChannel.source() );
                        if ( result == NEMemory::MessageResult::Succeed )
                        {
                            mRegisterConsumer.on_provider_registered( stub );
                        }
                        else
                        {
                            mRegisterConsumer.on_provider_unregistered( stub, NEService::DisconnectReason::UndefinedReason, NEService::COOKIE_ANY );
                        }
                    }
                    break;

                case NEService::RegistrationAction::UnregisterClient:
                    {
                        ProxyAddress proxy(msgReceived);
                        NEService::DisconnectReason reason { NEService::DisconnectReason::UndefinedReason };
                        msgReceived >> reason;
                        proxy.set_source( mChannel.source() );
                        mRegisterConsumer.on_consumer_unregistered(proxy, reason, NEService::COOKIE_ANY);
                    }
                    break;

                case NEService::RegistrationAction::UnregisterStub:
                    {
                        StubAddress stub(msgReceived);
                        NEService::DisconnectReason reason{NEService::DisconnectReason::UndefinedReason};
                        msgReceived >> reason;
                        stub.set_source( mChannel.source() );
                        mRegisterConsumer.on_provider_unregistered(stub, reason, NEService::COOKIE_ANY);
                    }
                    break;

                default:
                    ASSERT(false);
                    break;
                }
            }
            break;

        case NEService::FuncIdRange::ServiceLastId:                    // fall through
        case NEService::FuncIdRange::SystemServiceQueryInstances:      // fall through
        case NEService::FuncIdRange::SystemServiceRequestRegister:     // fall through
        case NEService::FuncIdRange::SystemServiceDisconnect:          // fall through
        case NEService::FuncIdRange::SystemServiceConnect:             // fall through
        case NEService::FuncIdRange::ResponseServiceProviderConnection:// fall through
        case NEService::FuncIdRange::RequestServiceProviderConnection: // fall through
        case NEService::FuncIdRange::ResponseServiceProviderVersion:   // fall through
        case NEService::FuncIdRange::RequestServiceProviderVersion:    // fall through
        case NEService::FuncIdRange::RequestRegisterService:           // fall through
        case NEService::FuncIdRange::ComponentCleanup:                 // fall through
        case NEService::FuncIdRange::SystemServiceNotifyInstances:     // fall through
        case NEService::FuncIdRange::ServiceLogRegisterScopes:         // fall through
        case NEService::FuncIdRange::ServiceLogUpdateScopes:           // fall through
        case NEService::FuncIdRange::ServiceLogQueryScopes:            // fall through
        case NEService::FuncIdRange::ServiceLogScopesUpdated:          // fall through
        case NEService::FuncIdRange::ServiceSaveLogConfiguration:      // fall through
        case NEService::FuncIdRange::ServiceLogConfigurationSaved:     // fall through
        case NEService::FuncIdRange::ServiceLogMessage:                // fall through
            break;

        case NEService::FuncIdRange::AttributeLastId:          // fall through
        case NEService::FuncIdRange::AttributeFirstId:         // fall through
        case NEService::FuncIdRange::ResponseLastId:           // fall through
        case NEService::FuncIdRange::ResponseFirstId:          // fall through
        case NEService::FuncIdRange::RequestLastId:            // fall through
        case NEService::FuncIdRange::RequestFirstId:           // fall through
        case NEService::FuncIdRange::EmptyFunctionId:          // fall through
        default:
            {
                if ( NEService::is_executable_id(static_cast<uint32_t>(msgId)) )
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
                else
                {
                    LOG_WARN("The message [ %u ] was not processed on client service side", msgId);
                }
            }
            break;
        }
    }
    else
    {
        LOG_WARN("Invalid message from host [ %s : %u ], ignore processing"
                    , whichSource.address().host_address().as_string()
                    , whichSource.address().host_port( ) );
    }
}


void RouterClient::process_request_event( RemoteRequestEvent & requestEvent)
{
    LOG_SCOPE(areg_ipc_private_RouterClient_processRemoteRequestEvent);

    if ( requestEvent.is_remote() )
    {
        RemoteMessage data;
        if ( RemoteEventFactory::stream_from_event( data, requestEvent, mChannel) )
        {
            LOG_DBG("Sending [ %s ] event: remote message [ %u ] from source [ %llu ] to target [ %llu ]"
                      , requestEvent.runtime_class_name().as_string()
                      , data.message_id()
                      , data.source()
                      , data.target());

            send_message(data);
        }
        else
        {
            LOG_ERR("Failed to create remote request data with message [ %u ]", requestEvent.request_id() );
        }
    }
    else
    {
        LOG_WARN("Request event with message [ %u ] is not remote, ignoring sending event", requestEvent.request_id());
    }
}

void RouterClient::process_notify_request( RemoteNotifyRequestEvent & requestNotifyEvent )
{
    LOG_SCOPE(areg_ipc_private_RouterClient_processRemoteNotifyRequestEvent);

    if ( requestNotifyEvent.is_remote() )
    {
        RemoteMessage data;
        if ( RemoteEventFactory::stream_from_event( data, requestNotifyEvent, mChannel) )
        {
            LOG_DBG("Send [ %s ] event: remote message [ %u ] from source [ %llu ] to target [ %llu ]"
                      , requestNotifyEvent.runtime_class_name().as_string()
                      , data.message_id()
                      , data.source()
                      , data.target());

            send_message(data);
        }
        else
        {
            LOG_ERR("Failed to create remote notify request message [ %u ]", requestNotifyEvent.request_id() );
        }
    }
    else
    {
        LOG_WARN("Notify request [ %u ] is not remote, ignoring sending event", requestNotifyEvent.request_id());
    }
}


void RouterClient::process_response_event(RemoteResponseEvent & responseEvent)
{
    LOG_SCOPE(areg_ipc_private_RouterClient_processRemoteResponseEvent);

    if ( responseEvent.is_remote() )
    {
        RemoteMessage data;
        if ( RemoteEventFactory::stream_from_event( data, responseEvent, mChannel) )
        {
            LOG_DBG("Forwarding [ %s ] message [ %u ] from source [ %llu ] to target [ %llu ]"
                      , responseEvent.runtime_class_name().as_string()
                      , data.message_id()
                      , data.source()
                      , data.target());

            send_message(data);
        }
        else
        {
            LOG_ERR("Failed to create remote response message [ %u ]", responseEvent.response_id() );
        }
    }
    else
    {
        LOG_WARN("Response event with message [ %u ] is not remote, ignoring", responseEvent.response_id());
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
        register_client_commands();
        DispatcherThread::ready_for_events(true);
        set_connection_state(ServiceClientConnectionBase::ConnectionPhase::DisconnectState);
    }
    else
    {
        DispatcherThread::ready_for_events(false);
        set_connection_state(ServiceClientConnectionBase::ConnectionPhase::ConnectionStopped);
        unregister_client_commands();
    }
}

