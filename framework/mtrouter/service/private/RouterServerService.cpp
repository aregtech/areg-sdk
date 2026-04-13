/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        mtrouter/service/private/RouterServerService.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Message router connection service.
 ************************************************************************/
#include "mtrouter/service/RouterServerService.hpp"

#include "areg/ipc/private/ConnectionDefs.hpp"
#include "areg/ipc/ConnectionConfiguration.hpp"
#include "areg/ipc/RemoteServiceDefs.hpp"
#include "areg/logging/areg_log.h"
#include "aregextend/service/SystemServiceDefs.hpp"

DEF_LOG_SCOPE(mtrouter_service_RouterServerService, register_service_provider);
DEF_LOG_SCOPE(mtrouter_service_RouterServerService, unregister_service_provider);
DEF_LOG_SCOPE(mtrouter_service_RouterServerService, register_service_consumer);
DEF_LOG_SCOPE(mtrouter_service_RouterServerService, unregister_service_consumer);

DEF_LOG_SCOPE(mtrouter_service_RouterServerService, on_provider_registered);
DEF_LOG_SCOPE(mtrouter_service_RouterServerService, on_consumer_registered);
DEF_LOG_SCOPE(mtrouter_service_RouterServerService, on_provider_unregistered);
DEF_LOG_SCOPE(mtrouter_service_RouterServerService, on_consumer_unregistered);

DEF_LOG_SCOPE(mtrouter_service_RouterServerService, on_message_received);
DEF_LOG_SCOPE(mtrouter_service_RouterServerService, on_message_send);

//////////////////////////////////////////////////////////////////////////
// RouterServerService class implementation
//////////////////////////////////////////////////////////////////////////

RouterServerService::RouterServerService()
    : areg::ext::ServiceCommunicationBase   ( areg::COOKIE_ROUTER
                                            , areg::RemoteServiceKind::Router
                                            , static_cast<uint32_t>(areg::ConnectionType::Tcpip)
                                            , areg::SYSTEM_THREAD_STACK_BIG
                                            , areg::SERVER_DISPATCH_MESSAGE_THREAD
                                            , ServiceCommunicationBase::ConnectionPolicy::Accept
                                            , areg::ext::DEFAULT_COMMUNICATION_PAIR_COUNT )
    , areg::RegistrationConsumer ( )
    , areg::RegistrationProvider ( )

    , mServiceRegistry          ( )
{
}

bool RouterServerService::register_service_provider(const areg::StubAddress & /* stubService */)
{
    LOG_SCOPE( mtrouter_service_RouterServerService, register_service_provider );
    LOG_ERR("Method is not implemented, this should not be called");
    return false;
}

void RouterServerService::unregister_service_provider(const areg::StubAddress & /* stubService */, const areg::DisconnectReason /*reason*/ )
{
    LOG_SCOPE( mtrouter_service_RouterServerService, unregister_service_provider );
    LOG_ERR("Method is not implemented, this should not be called");
}

bool RouterServerService::register_service_consumer(const areg::ProxyAddress & /* proxyService */)
{
    LOG_SCOPE( mtrouter_service_RouterServerService, register_service_consumer );
    LOG_ERR("Method is not implemented, this should not be called");
    return false;
}

void RouterServerService::unregister_service_consumer(const areg::ProxyAddress & /* proxyService */, const areg::DisconnectReason /*reason*/ )
{
    LOG_SCOPE( mtrouter_service_RouterServerService, unregister_service_consumer );
    LOG_ERR("Method is not implemented, this should not be called");
}

void RouterServerService::on_message_received(const areg::RemoteMessage &msgReceived)
{
    LOG_SCOPE( mtrouter_service_RouterServerService, on_message_received );

    ASSERT( msgReceived.is_valid() );
    areg::FuncIdRange msgId { static_cast<areg::FuncIdRange>(msgReceived.message_id()) };
    const ITEM_ID & source{ msgReceived.source() };

    LOG_DBG("Processing received valid message [ %s ] of id [ 0x%X ] from source [ %u ] to target [ %u ]"
                    , areg::as_string(msgId)
                    , static_cast<uint32_t>(msgId)
                    , static_cast<uint32_t>(source)
                    , static_cast<uint32_t>(msgReceived.target()));

    switch( msgId )
    {
    case areg::FuncIdRange::SystemServiceRequestRegister:
        {
            areg::RegistrationAction reqType;
            msgReceived >> reqType;
            LOG_DBG("Routing service received registration request message [ %s ]", areg::as_string(reqType));
            switch ( reqType )
            {
            case areg::RegistrationAction::RegisterProvider:
                {
                    areg::StubAddress stubService(msgReceived);
                    stubService.set_source(source);
                    on_provider_registered(stubService);
                }
                break;

            case areg::RegistrationAction::RegisterConsumer:
                {
                    areg::ProxyAddress proxyService(msgReceived);
                    proxyService.set_source(source);
                    on_consumer_registered(proxyService);
                }
                break;

            case areg::RegistrationAction::UnregisterProvider:
                {
                    areg::StubAddress stubService(msgReceived);
                    areg::DisconnectReason reason{areg::DisconnectReason::UndefinedReason};
                    msgReceived >> reason;
                    stubService.set_source(source);
                    on_provider_unregistered(stubService, reason, stubService.cookie());
                }
                break;

            case areg::RegistrationAction::UnregisterConsumer:
                {
                    areg::ProxyAddress proxyService(msgReceived);
                    areg::DisconnectReason reason { areg::DisconnectReason::UndefinedReason };
                    msgReceived >> reason;
                    proxyService.set_source(source);
                    on_consumer_unregistered(proxyService, reason, proxyService.cookie());
                }
                break;

            default:
                ASSERT(false);
                break;  // do nothing
            }
        }
        break;

    case areg::FuncIdRange::SystemServiceDisconnect:
        {
            ITEM_ID cookie = areg::COOKIE_UNKNOWN;
            msgReceived >> cookie;
            remove_instance(cookie);
            mServerConnection.close_connection(cookie);

            areg::ArrayList<areg::StubAddress>  listStubs;
            areg::ArrayList<areg::ProxyAddress> listProxies;
            mServiceRegistry.service_sources(cookie, listStubs, listProxies);

            LOG_DBG("Routing service received disconnect message from cookie [ %u ], [ %d ] stubs and [ %d ] proxies are going to be disconnected"
                        , static_cast<uint32_t>(cookie)
                        , listStubs.size()
                        , listProxies.size());

            for (uint32_t i = 0; i < listProxies.size(); ++i)
            {
                on_consumer_unregistered(listProxies[i], areg::DisconnectReason::ConsumerDisconnected, cookie);
            }

            for (uint32_t i = 0; i < listStubs.size(); ++i)
            {
                on_provider_unregistered(listStubs[i], areg::DisconnectReason::ProviderDisconnected, cookie);
            }
        }
        break;

    case areg::FuncIdRange::ServiceLastId:                    // fall through
    case areg::FuncIdRange::SystemServiceQueryInstances:      // fall through
    case areg::FuncIdRange::SystemServiceConnect:             // fall through
    case areg::FuncIdRange::RequestServiceProviderConnection: // fall through
    case areg::FuncIdRange::ResponseServiceProviderVersion:   // fall through
    case areg::FuncIdRange::RequestServiceProviderVersion:    // fall through
    case areg::FuncIdRange::RequestRegisterService:           // fall through
    case areg::FuncIdRange::ComponentCleanup:                 // fall through
    case areg::FuncIdRange::SystemServiceNotifyRegister:      // fall through
    case areg::FuncIdRange::SystemServiceNotifyConnection:    // fall through
    case areg::FuncIdRange::SystemServiceNotifyInstances:     // fall through
    case areg::FuncIdRange::ServiceLogRegisterScopes:         // fall through
    case areg::FuncIdRange::ServiceLogUpdateScopes:           // fall through
    case areg::FuncIdRange::ServiceLogQueryScopes:            // fall through
    case areg::FuncIdRange::ServiceLogScopesUpdated:          // fall through
    case areg::FuncIdRange::ServiceSaveLogConfiguration:      // fall through
    case areg::FuncIdRange::ServiceLogConfigurationSaved:     // fall through
    case areg::FuncIdRange::ServiceLogMessage:                // fall through
        break;

    case areg::FuncIdRange::ResponseServiceProviderConnection:// fall through
    case areg::FuncIdRange::AttributeLastId:                  // fall through
    case areg::FuncIdRange::AttributeFirstId:                 // fall through
    case areg::FuncIdRange::ResponseLastId:                   // fall through
    case areg::FuncIdRange::ResponseFirstId:                  // fall through
    case areg::FuncIdRange::RequestLastId:                    // fall through
    case areg::FuncIdRange::RequestFirstId:                   // fall through
    case areg::FuncIdRange::EmptyFunctionId:                  // fall through
    default:
        if ( areg::is_executable_id(static_cast<uint32_t>(msgId)) || areg::is_connect_id( static_cast<uint32_t>(msgId)) )
        {
            LOG_DBG("Message [ %u ] is executable, going to forward to target [ %u ], received from source [ %u ]"
                       , static_cast<uint32_t>(msgId)
                       , msgReceived.target()
                       , source);

            if ( msgReceived.target() != areg::TARGET_UNKNOWN )
            {
                send_message( msgReceived );
            }
        }
        else
        {
            LOG_ERR("Message [ %u ] is not executable, ignoring to forward to target [ %u ] from source [ %u ]"
                            , static_cast<uint32_t>(msgId)
                            , static_cast<uint32_t>(msgReceived.target())
                            , static_cast<uint32_t>(source));
            ASSERT(false);
        }
        break;
    }
}

void RouterServerService::on_message_send(const areg::RemoteMessage &msgSend)
{
    LOG_SCOPE( mtrouter_service_RouterServerService, on_message_send );

    areg::FuncIdRange msgId = static_cast<areg::FuncIdRange>( msgSend.message_id() );
    LOG_DBG("Sending message [ %s ] of id [ 0x%X ] is going to send to target [ %u ] from source [ %u ]"
                    , areg::as_string(msgId)
                    , static_cast<uint32_t>(msgId)
                    , static_cast<uint32_t>(msgSend.target())
                    , static_cast<uint32_t>(msgSend.source()));

    if ( areg::is_executable_id( static_cast<uint32_t>(msgId)) )
    {
        if ( msgSend.target( ) != areg::TARGET_UNKNOWN )
        {
            send_message( msgSend );
        }
    }
    else
    {
        LOG_ERR("The message [ %u ] is neither executable, nor router notification. Ignoring sending message", static_cast<uint32_t>(msgId));
        ASSERT(false);
    }
}

void RouterServerService::disconnect_services()
{
    ServiceCommunicationBase::disconnect_services( );

    areg::ArrayList<areg::StubAddress>  stubList;
    areg::ArrayList<areg::ProxyAddress> proxyList;
    extract_service_addresses(areg::COOKIE_ANY, stubList, proxyList);

    for ( uint32_t i = 0; i < stubList.size(); ++ i )
    {
        on_provider_unregistered( stubList[i], areg::DisconnectReason::ServiceDisconnected, areg::COOKIE_ANY );
    }

    for ( uint32_t i = 0; i < proxyList.size(); ++ i )
    {
        on_consumer_unregistered( proxyList[i], areg::DisconnectReason::ServiceDisconnected, areg::COOKIE_ANY );
    }

    mServiceRegistry.clear( );
}

void RouterServerService::extract_service_addresses( const ITEM_ID & cookie, areg::ArrayList<areg::StubAddress> & out_listStubs, areg::ArrayList<areg::ProxyAddress> & out_lisProxies ) const
{
    mServiceRegistry.service_list(cookie, out_listStubs, out_lisProxies);
}

void RouterServerService::on_provider_registered(const areg::StubAddress & stub)
{
    LOG_SCOPE( mtrouter_service_RouterServerService, on_provider_registered );
    ASSERT(stub.is_service_public());

    LOG_DBG("Going to register remote stub [ %s ]", areg::StubAddress::to_path(stub).as_string());
    if ( mServiceRegistry.service_status(stub) != areg::ServiceConnectionState::Connected )
    {
        ListServiceProxies listProxies;
        const ServiceStub & stubService = mServiceRegistry.register_service_provider(stub, listProxies);
        if ( stubService.service_status() == areg::ServiceConnectionState::Connected && listProxies.is_empty() == false )
        {
            LOG_DBG("Stub [ %s ] is connected, sending notification messages to [ %d ] waiting proxies"
                        , areg::StubAddress::to_path(stubService.service_address()).as_string()
                        , listProxies.size());

            areg::ArrayList<ITEM_ID> sendList;
            for (ListServiceProxiesBase::LISTPOS pos = listProxies.first_position(); listProxies.is_valid_position(pos); pos = listProxies.next_position(pos) )
            {
                const ServiceProxy & proxyService = listProxies.value_at(pos);
                const areg::ProxyAddress & addrProxy    = proxyService.service_address();
                if ( (proxyService.service_status() == areg::ServiceConnectionState::Connected) && (addrProxy.source() != stub.source()) )
                {
                    areg::RemoteMessage msgRegisterProxy = areg::client_registered_event(addrProxy, mServerConnection.channel_id(), stub.source());
                    send_message(msgRegisterProxy);

                    LOG_DBG("Send to stub [ %s ] the proxy [ %s ] registration notification. Send message [ %s ] of id [ 0x%X ] from source [ %u ] to target [ %u ]"
                                , stub.to_string().as_string()
                                , addrProxy.to_string().as_string()
                                , areg::as_string( static_cast<areg::FuncIdRange>(msgRegisterProxy.message_id()))
                                , static_cast<uint32_t>(msgRegisterProxy.message_id())
                                , static_cast<uint32_t>(msgRegisterProxy.source())
                                , static_cast<uint32_t>(msgRegisterProxy.target()));

                    if ( sendList.add_if_unique(addrProxy.source()) )
                    {
                        areg::RemoteMessage msgRegisterProvider  = areg::service_registered_event(stub, mServerConnection.channel_id(), addrProxy.source());
                        send_message(msgRegisterProvider);

                        LOG_DBG("Send to proxy [ %s ] the provider [ %s ] registration notification. Send message [ %s ] of id [ 0x%X ] from source [ %u ] to target [ %u ]"
                                    , addrProxy.to_string().as_string()
                                    , stub.to_string().as_string()
                                    , areg::as_string( static_cast<areg::FuncIdRange>(msgRegisterProvider.message_id()))
                                    , static_cast<uint32_t>(msgRegisterProvider.message_id())
                                    , static_cast<uint32_t>(msgRegisterProvider.source())
                                    , static_cast<uint32_t>(msgRegisterProvider.target()));
                    }
                    else
                    {
                        // ignore, it already has registered provider
                        LOG_DBG("Ignoring sending provider registration message to target [ %u ], target already notified", static_cast<uint32_t>(addrProxy.source()));
                    }
                }
                else
                {
                     // ignore, it already has registered provider locally or proxy is not connected
                    LOG_DBG("ignoring sending stub registration message, Provider [ %s ] and Proxy [ %s ] have same origin."
                                    , areg::StubAddress::to_path(stub).as_string()
                                    , areg::ProxyAddress::to_path(addrProxy).as_string());
                }
            }
        }
        else
        {
            // ignore, stub is not connected or list is empty
            LOG_DBG("Ignoring sending stub [ %s ] connected notification, the proxy list is empty [ %s ]"
                            , areg::StubAddress::to_path(stub).as_string()
                            , listProxies.is_empty() ? "YES" : "NO");
        }
    }
    else
    {
        // ignore, stub is already connected
        LOG_DBG("Stub [ %s ] is already marked as connected, ignoring registration", areg::StubAddress::to_path(stub).as_string());
    }
}

void RouterServerService::on_consumer_registered(const areg::ProxyAddress & proxy)
{
    LOG_SCOPE( mtrouter_service_RouterServerService, on_consumer_registered );
    if ( mServiceRegistry.service_status(proxy) != areg::ServiceConnectionState::Connected )
    {
        ServiceProxy proxyService;
        const ServiceStub & stubService   = mServiceRegistry.register_service_proxy(proxy, proxyService);
        const areg::StubAddress & addrStub= stubService.service_address();

        LOG_DBG("Registered proxy [ %s ], for connection with stub [ %s ], connection status is [ %s ]"
                    , areg::ProxyAddress::to_path(proxy).as_string()
                    , areg::StubAddress::to_path(addrStub).as_string()
                    , areg::as_string( proxyService.service_status()));

        if ( (proxyService.service_status() == areg::ServiceConnectionState::Connected) && (proxy.source() != addrStub.source()) )
        {
            areg::RemoteMessage msgRegisterProxy = areg::client_registered_event(proxy, mServerConnection.channel_id(), addrStub.source());
            send_message(msgRegisterProxy);

            LOG_DBG("Send to stub [ %s ] the proxy [ %s ] registration notification. Send message [ %s ] of id [ 0x%X ] from source [ %u ] to target [ %u ]"
                        , addrStub.to_string().as_string()
                        , proxy.to_string().as_string()
                        , areg::as_string( static_cast<areg::FuncIdRange>(msgRegisterProxy.message_id()))
                        , static_cast<uint32_t>(msgRegisterProxy.message_id())
                        , static_cast<uint32_t>(msgRegisterProxy.source())
                        , static_cast<uint32_t>(msgRegisterProxy.target()));

            areg::RemoteMessage msgRegisterProvider  = areg::service_registered_event(addrStub, mServerConnection.channel_id(), proxy.source());
            send_message(msgRegisterProvider);

            LOG_DBG("Send to proxy [ %s ] the provider [ %s ] registration notification. Send message [ %s ] of id [ 0x%X ] from source [ %u ] to target [ %u ]"
                        , proxy.to_string().as_string()
                        , addrStub.to_string().as_string()
                        , areg::as_string( static_cast<areg::FuncIdRange>(msgRegisterProvider.message_id()))
                        , static_cast<uint32_t>(msgRegisterProvider.message_id())
                        , static_cast<uint32_t>(msgRegisterProvider.source())
                        , static_cast<uint32_t>(msgRegisterProvider.target()));
        }
        else
        {
            // ignore, it is done locally
            LOG_DBG("Ignore send provider registration message, Provider [ %s ] and Proxy [ %s ] have same origin."
                            , areg::StubAddress::to_path(addrStub).as_string()
                            , areg::ProxyAddress::to_path(proxy).as_string());
        }
    }
    else
    {
        LOG_DBG("Proxy [ %s ] is already having connected status, ignoring registration", areg::ProxyAddress::to_path(proxy).as_string());
    }
}

void RouterServerService::on_provider_unregistered(const areg::StubAddress & stub, areg::DisconnectReason reason, const ITEM_ID & cookie /*= areg::COOKIE_ANY*/ )
{
    LOG_SCOPE( mtrouter_service_RouterServerService, on_provider_unregistered );
    if ( mServiceRegistry.service_status(stub) == areg::ServiceConnectionState::Connected )
    {
        ListServiceProxies listProxies;
        mServiceRegistry.unregister_service_provider(stub, listProxies);
        LOG_DBG("Unregistered stub [ %s ], [ %d ] proxies are going to be notified"
                        , stub.to_string().as_string()
                        , listProxies.size());

        LOG_DBG("Filter sources [ %u ] of proxy list", static_cast<uint32_t>(cookie));

        areg::ArrayList<ITEM_ID> sendList;
        for (ListServiceProxiesBase::LISTPOS pos = listProxies.first_position(); listProxies.is_valid_position(pos); pos = listProxies.next_position(pos) )
        {
            const ServiceProxy & proxyService = listProxies.value_at(pos);
            const areg::ProxyAddress & addrProxy    = proxyService.service_address();

            if ( (cookie == areg::COOKIE_ANY) || (addrProxy.source() != cookie) )
            {
                // no need to send message to unregistered stub, only to proxy side
                if (sendList.add_if_unique(addrProxy.source()) )
                {
                    send_message(areg::service_unregistered_event(stub, reason, mServerConnection.channel_id(), addrProxy.source( )), areg::EventPriority::HighPrio );

                    LOG_INFO("Send stub [ %s ] disconnect message to proxy [ %s ]"
                                    , stub.to_string().as_string()
                                    , addrProxy.to_string().as_string());
                }
                else
                {
                    // ignore, it already has unregistered stub
                    LOG_DBG("Ignore unregistered stub message for proxy [ %s ], it was already notified", addrProxy.to_string().as_string());
                }
            }
            else
            {
                // ignore, it already has unregistered stub locally or proxy status did not changed
                ServiceProxy dummy;
                mServiceRegistry.unregister_service_proxy(addrProxy, dummy);
                LOG_DBG("Proxy [ %s ] is marked as ignored by source [ %u ], remove and skip", addrProxy.to_string().as_string(), static_cast<uint32_t>(cookie));
            }
        }
    }
    else
    {
        // ignore, stub is already disconnected
        LOG_DBG("Ignore unregistering stub [ %s ], it is already unregistered", stub.to_string().as_string());
    }
}

void RouterServerService::on_consumer_unregistered(const areg::ProxyAddress & proxy, areg::DisconnectReason reason, const ITEM_ID & cookie /*= areg::COOKIE_ANY*/ )
{
    LOG_SCOPE( mtrouter_service_RouterServerService, on_consumer_unregistered );
    LOG_DBG("Unregistering services of proxy [ %s ] related to cookie [ %u ]"
                    , areg::ProxyAddress::to_path(proxy).as_string()
                    , static_cast<uint32_t>(cookie));

    areg::RemoteMessage msgRegisterProxy;
    ServiceProxy svcProxy;
    const ServiceStub * svcStub     = nullptr;

    if (proxy.source() == cookie)
    {
        svcStub = &mServiceRegistry.unregister_service_proxy(proxy, svcProxy);
    }
    else
    {
        svcStub = &mServiceRegistry.disconnect_proxy(proxy);
    }

    ASSERT(svcStub != nullptr);
    const areg::StubAddress & addrStub    = svcStub->service_address();

    if ((svcStub->service_status() == areg::ServiceConnectionState::Connected) && (proxy.source() != addrStub.source()))
    {
        send_message(areg::client_unregistered_event(proxy, reason, mServerConnection.channel_id(), addrStub.source( )), areg::EventPriority::HighPrio);
        LOG_INFO("Send proxy [ %s ] disconnect message to stub [ %s ]", proxy.to_string().as_string(), addrStub.to_string().as_string());
    }
    else
    {
        LOG_DBG("Ignore notifying proxy [ %s ] disconnect", proxy.to_string().as_string());
    }
}

void RouterServerService::on_service_channel_connected(const areg::Channel & /* channel */)
{

}

void RouterServerService::on_service_channel_disconnected(const areg::Channel & /* channel */)
{

}

void RouterServerService::on_service_channel_lost(const areg::Channel & /* channel */)
{
}

void RouterServerService::failed_process_message(const areg::RemoteMessage & /* msgUnprocessed */)
{
}

void RouterServerService::on_connection_started()
{
}

void RouterServerService::on_connection_stopped()
{
}

void RouterServerService::on_connection_lost()
{
}
