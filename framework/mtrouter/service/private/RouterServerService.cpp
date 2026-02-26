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
#include "areg/logging/GELog.h"

DEF_LOG_SCOPE(mtrouter_service_RouterServerService_registerServiceProvider);
DEF_LOG_SCOPE(mtrouter_service_RouterServerService_unregisterServiceProvider);
DEF_LOG_SCOPE(mtrouter_service_RouterServerService_registerServiceConsumer);
DEF_LOG_SCOPE(mtrouter_service_RouterServerService_unregisterServiceConsumer);

DEF_LOG_SCOPE(mtrouter_service_RouterServerService_registeredRemoteServiceProvider);
DEF_LOG_SCOPE(mtrouter_service_RouterServerService_registeredRemoteServiceConsumer);
DEF_LOG_SCOPE(mtrouter_service_RouterServerService_unregisteredRemoteServiceProvider);
DEF_LOG_SCOPE(mtrouter_service_RouterServerService_unregisteredRemoteServiceConsumer);

DEF_LOG_SCOPE(mtrouter_service_RouterServerService_onServiceMessageReceived);
DEF_LOG_SCOPE(mtrouter_service_RouterServerService_onServiceMessageSend);

//////////////////////////////////////////////////////////////////////////
// RouterServerService class implementation
//////////////////////////////////////////////////////////////////////////

RouterServerService::RouterServerService()
    : ServiceCommunicationBase   ( NEService::COOKIE_ROUTER, NERemoteService::RemoteServiceKind::Router, static_cast<uint32_t>(NERemoteService::ConnectionType::Tcpip), NEConnection::SERVER_DISPATCH_MESSAGE_THREAD, ServiceCommunicationBase::ConnectionPolicy::Accept )
    , RegistrationConsumer ( )
    , RegistrationProvider ( )

    , mServiceRegistry          ( )
{
}

bool RouterServerService::register_service_provider(const StubAddress & /* stubService */)
{
    LOG_SCOPE(mtrouter_service_RouterServerService_registerServiceProvider);
    LOG_ERR("Method is not implemented, this should not be called");
    return false;
}

void RouterServerService::unregister_service_provider(const StubAddress & /* stubService */, const NEService::DisconnectReason /*reason*/ )
{
    LOG_SCOPE(mtrouter_service_RouterServerService_unregisterServiceProvider);
    LOG_ERR("Method is not implemented, this should not be called");
}

bool RouterServerService::register_service_consumer(const ProxyAddress & /* proxyService */)
{
    LOG_SCOPE(mtrouter_service_RouterServerService_registerServiceConsumer);
    LOG_ERR("Method is not implemented, this should not be called");
    return false;
}

void RouterServerService::unregister_service_consumer(const ProxyAddress & /* proxyService */, const NEService::DisconnectReason /*reason*/ )
{
    LOG_SCOPE(mtrouter_service_RouterServerService_unregisterServiceConsumer);
    LOG_ERR("Method is not implemented, this should not be called");
}

void RouterServerService::on_message_received(const RemoteMessage &msgReceived)
{
    LOG_SCOPE(mtrouter_service_RouterServerService_onServiceMessageReceived);

    ASSERT( msgReceived.is_valid() );
    NEService::FuncIdRange msgId { static_cast<NEService::FuncIdRange>(msgReceived.message_id()) };
    const ITEM_ID & source{ msgReceived.source() };

    LOG_DBG("Processing received valid message [ %s ] of id [ 0x%X ] from source [ %u ] to target [ %u ]"
                    , NEService::as_string(msgId)
                    , static_cast<uint32_t>(msgId)
                    , static_cast<uint32_t>(source)
                    , static_cast<uint32_t>(msgReceived.target()));

    switch( msgId )
    {
    case NEService::FuncIdRange::SystemServiceRequestRegister:
        {
            NEService::RegistrationAction reqType;
            msgReceived >> reqType;
            LOG_DBG("Routing service received registration request message [ %s ]", NEService::as_string(reqType));
            switch ( reqType )
            {
            case NEService::RegistrationAction::RegisterStub:
                {
                    StubAddress stubService(msgReceived);
                    stubService.set_source(source);
                    on_provider_registered(stubService);
                }
                break;

            case NEService::RegistrationAction::RegisterClient:
                {
                    ProxyAddress proxyService(msgReceived);
                    proxyService.set_source(source);
                    on_consumer_registered(proxyService);
                }
                break;

            case NEService::RegistrationAction::UnregisterStub:
                {
                    StubAddress stubService(msgReceived);
                    NEService::DisconnectReason reason{NEService::DisconnectReason::UndefinedReason};
                    msgReceived >> reason;
                    stubService.set_source(source);
                    on_provider_unregistered(stubService, reason, stubService.cookie());
                }
                break;

            case NEService::RegistrationAction::UnregisterClient:
                {
                    ProxyAddress proxyService(msgReceived);
                    NEService::DisconnectReason reason { NEService::DisconnectReason::UndefinedReason };
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

    case NEService::FuncIdRange::SystemServiceDisconnect:
        {
            ITEM_ID cookie = NEService::COOKIE_UNKNOWN;
            msgReceived >> cookie;
            removeInstance(cookie);
            mServerConnection.close_connection(cookie);

            ArrayList<StubAddress>  listStubs;
            ArrayList<ProxyAddress> listProxies;
            mServiceRegistry.service_sources(cookie, listStubs, listProxies);

            LOG_DBG("Routing service received disconnect message from cookie [ %u ], [ %d ] stubs and [ %d ] proxies are going to be disconnected"
                        , static_cast<uint32_t>(cookie)
                        , listStubs.size()
                        , listProxies.size());

            for (uint32_t i = 0; i < listProxies.size(); ++i)
            {
                on_consumer_unregistered(listProxies[i], NEService::DisconnectReason::ConsumerDisconnected, cookie);
            }

            for (uint32_t i = 0; i < listStubs.size(); ++i)
            {
                on_provider_unregistered(listStubs[i], NEService::DisconnectReason::ProviderDisconnected, cookie);
            }
        }
        break;

    case NEService::FuncIdRange::ServiceLastId:                    // fall through
    case NEService::FuncIdRange::SystemServiceQueryInstances:      // fall through
    case NEService::FuncIdRange::SystemServiceConnect:             // fall through
    case NEService::FuncIdRange::RequestServiceProviderConnection: // fall through
    case NEService::FuncIdRange::ResponseServiceProviderVersion:   // fall through
    case NEService::FuncIdRange::RequestServiceProviderVersion:    // fall through
    case NEService::FuncIdRange::RequestRegisterService:           // fall through
    case NEService::FuncIdRange::ComponentCleanup:                 // fall through
    case NEService::FuncIdRange::SystemServiceNotifyRegister:      // fall through
    case NEService::FuncIdRange::SystemServiceNotifyConnection:    // fall through
    case NEService::FuncIdRange::SystemServiceNotifyInstances:     // fall through
    case NEService::FuncIdRange::ServiceLogRegisterScopes:         // fall through
    case NEService::FuncIdRange::ServiceLogUpdateScopes:           // fall through
    case NEService::FuncIdRange::ServiceLogQueryScopes:            // fall through
    case NEService::FuncIdRange::ServiceLogScopesUpdated:          // fall through
    case NEService::FuncIdRange::ServiceSaveLogConfiguration:      // fall through
    case NEService::FuncIdRange::ServiceLogConfigurationSaved:     // fall through
    case NEService::FuncIdRange::ServiceLogMessage:                // fall through
        break;

    case NEService::FuncIdRange::ResponseServiceProviderConnection:// fall through
    case NEService::FuncIdRange::AttributeLastId:                  // fall through
    case NEService::FuncIdRange::AttributeFirstId:                 // fall through
    case NEService::FuncIdRange::ResponseLastId:                   // fall through
    case NEService::FuncIdRange::ResponseFirstId:                  // fall through
    case NEService::FuncIdRange::RequestLastId:                    // fall through
    case NEService::FuncIdRange::RequestFirstId:                   // fall through
    case NEService::FuncIdRange::EmptyFunctionId:                  // fall through
    default:
        if ( NEService::is_executable_id(static_cast<uint32_t>(msgId)) || NEService::is_connect_id( static_cast<uint32_t>(msgId)) )
        {
            LOG_DBG("Message [ %u ] is executable, going to forward to target [ %u ], received from source [ %u ]"
                       , static_cast<uint32_t>(msgId)
                       , msgReceived.target()
                       , source);

            if ( msgReceived.target() != NEService::TARGET_UNKNOWN )
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

void RouterServerService::on_message_send(const RemoteMessage &msgSend)
{
    LOG_SCOPE(mtrouter_service_RouterServerService_onServiceMessageSend);

    NEService::FuncIdRange msgId = static_cast<NEService::FuncIdRange>( msgSend.message_id() );
    LOG_DBG("Sending message [ %s ] of id [ 0x%X ] is going to send to target [ %u ] from source [ %u ]"
                    , NEService::as_string(msgId)
                    , static_cast<uint32_t>(msgId)
                    , static_cast<uint32_t>(msgSend.target())
                    , static_cast<uint32_t>(msgSend.source()));

    if ( NEService::is_executable_id( static_cast<uint32_t>(msgId)) )
    {
        if ( msgSend.target( ) != NEService::TARGET_UNKNOWN )
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

    ArrayList<StubAddress>  stubList;
    ArrayList<ProxyAddress> proxyList;
    extract_service_addresses(NEService::COOKIE_ANY, stubList, proxyList);

    for ( uint32_t i = 0; i < stubList.size(); ++ i )
    {
        on_provider_unregistered( stubList[i], NEService::DisconnectReason::ServiceDisconnected, NEService::COOKIE_ANY );
    }

    for ( uint32_t i = 0; i < proxyList.size(); ++ i )
    {
        on_consumer_unregistered( proxyList[i], NEService::DisconnectReason::ServiceDisconnected, NEService::COOKIE_ANY );
    }

    mServiceRegistry.clear( );
}

void RouterServerService::extract_service_addresses( const ITEM_ID & cookie, ArrayList<StubAddress> & out_listStubs, ArrayList<ProxyAddress> & out_lisProxies ) const
{
    mServiceRegistry.service_list(cookie, out_listStubs, out_lisProxies);
}

void RouterServerService::on_provider_registered(const StubAddress & stub)
{
    LOG_SCOPE(mtrouter_service_RouterServerService_registeredRemoteServiceProvider);
    ASSERT(stub.is_service_public());

    LOG_DBG("Going to register remote stub [ %s ]", StubAddress::to_path(stub).as_string());
    if ( mServiceRegistry.service_status(stub) != NEService::ServiceConnectionState::Connected )
    {
        ListServiceProxies listProxies;
        const ServiceStub & stubService = mServiceRegistry.register_service_stub(stub, listProxies);
        if ( stubService.service_status() == NEService::ServiceConnectionState::Connected && listProxies.is_empty() == false )
        {
            LOG_DBG("Stub [ %s ] is connected, sending notification messages to [ %d ] waiting proxies"
                        , StubAddress::to_path(stubService.service_address()).as_string()
                        , listProxies.size());

            ArrayList<ITEM_ID> sendList;
            for (ListServiceProxiesBase::LISTPOS pos = listProxies.first_position(); listProxies.is_valid_position(pos); pos = listProxies.next_position(pos) )
            {
                const ServiceProxy & proxyService = listProxies.value_at_position(pos);
                const ProxyAddress & addrProxy    = proxyService.service_address();
                if ( (proxyService.service_status() == NEService::ServiceConnectionState::Connected) && (addrProxy.source() != stub.source()) )
                {
                    RemoteMessage msgRegisterProxy = NERemoteService::client_registered_event(addrProxy, mServerConnection.getChannelId(), stub.source());
                    send_message(msgRegisterProxy);

                    LOG_DBG("Send to stub [ %s ] the proxy [ %s ] registration notification. Send message [ %s ] of id [ 0x%X ] from source [ %u ] to target [ %u ]"
                                , stub.to_string().as_string()
                                , addrProxy.to_string().as_string()
                                , NEService::as_string( static_cast<NEService::FuncIdRange>(msgRegisterProxy.message_id()))
                                , static_cast<uint32_t>(msgRegisterProxy.message_id())
                                , static_cast<uint32_t>(msgRegisterProxy.source())
                                , static_cast<uint32_t>(msgRegisterProxy.target()));

                    if ( sendList.add_if_unique(addrProxy.source()) )
                    {
                        RemoteMessage msgRegisterStub  = NERemoteService::service_registered_event(stub, mServerConnection.getChannelId(), addrProxy.source());
                        send_message(msgRegisterStub);

                        LOG_DBG("Send to proxy [ %s ] the stub [ %s ] registration notification. Send message [ %s ] of id [ 0x%X ] from source [ %u ] to target [ %u ]"
                                    , addrProxy.to_string().as_string()
                                    , stub.to_string().as_string()
                                    , NEService::as_string( static_cast<NEService::FuncIdRange>(msgRegisterStub.message_id()))
                                    , static_cast<uint32_t>(msgRegisterStub.message_id())
                                    , static_cast<uint32_t>(msgRegisterStub.source())
                                    , static_cast<uint32_t>(msgRegisterStub.target()));
                    }
                    else
                    {
                        // ignore, it already has registered stub
                        LOG_DBG("Ignoring sending stub registration message to target [ %u ], target already notified", static_cast<uint32_t>(addrProxy.source()));
                    }
                }
                else
                {
                     // ignore, it already has registered stub locally or proxy is not connected
                    LOG_DBG("ignoring sending stub registration message, Stub [ %s ] and Proxy [ %s ] have same origin."
                                    , StubAddress::to_path(stub).as_string()
                                    , ProxyAddress::to_path(addrProxy).as_string());
                }
            }
        }
        else
        {
            // ignore, stub is not connected or list is empty
            LOG_DBG("Ignoring sending stub [ %s ] connected notification, the proxy list is empty [ %s ]"
                            , StubAddress::to_path(stub).as_string()
                            , listProxies.is_empty() ? "YES" : "NO");
        }
    }
    else
    {
        // ignore, stub is already connected
        LOG_DBG("Stub [ %s ] is already marked as connected, ignoring registration", StubAddress::to_path(stub).as_string());
    }
}

void RouterServerService::on_consumer_registered(const ProxyAddress & proxy)
{
    LOG_SCOPE(mtrouter_service_RouterServerService_registeredRemoteServiceConsumer);
    if ( mServiceRegistry.service_status(proxy) != NEService::ServiceConnectionState::Connected )
    {
        ServiceProxy proxyService;
        const ServiceStub & stubService   = mServiceRegistry.register_service_proxy(proxy, proxyService);
        const StubAddress & addrStub      = stubService.service_address();

        LOG_DBG("Registered proxy [ %s ], for connection with stub [ %s ], connection status is [ %s ]"
                    , ProxyAddress::to_path(proxy).as_string()
                    , StubAddress::to_path(addrStub).as_string()
                    , NEService::as_string( proxyService.service_status()));

        if ( (proxyService.service_status() == NEService::ServiceConnectionState::Connected) && (proxy.source() != addrStub.source()) )
        {
            RemoteMessage msgRegisterProxy = NERemoteService::client_registered_event(proxy, mServerConnection.getChannelId(), addrStub.source());
            send_message(msgRegisterProxy);

            LOG_DBG("Send to stub [ %s ] the proxy [ %s ] registration notification. Send message [ %s ] of id [ 0x%X ] from source [ %u ] to target [ %u ]"
                        , addrStub.to_string().as_string()
                        , proxy.to_string().as_string()
                        , NEService::as_string( static_cast<NEService::FuncIdRange>(msgRegisterProxy.message_id()))
                        , static_cast<uint32_t>(msgRegisterProxy.message_id())
                        , static_cast<uint32_t>(msgRegisterProxy.source())
                        , static_cast<uint32_t>(msgRegisterProxy.target()));

            RemoteMessage msgRegisterStub  = NERemoteService::service_registered_event(addrStub, mServerConnection.getChannelId(), proxy.source());
            send_message(msgRegisterStub);

            LOG_DBG("Send to proxy [ %s ] the stub [ %s ] registration notification. Send message [ %s ] of id [ 0x%X ] from source [ %u ] to target [ %u ]"
                        , proxy.to_string().as_string()
                        , addrStub.to_string().as_string()
                        , NEService::as_string( static_cast<NEService::FuncIdRange>(msgRegisterStub.message_id()))
                        , static_cast<uint32_t>(msgRegisterStub.message_id())
                        , static_cast<uint32_t>(msgRegisterStub.source())
                        , static_cast<uint32_t>(msgRegisterStub.target()));
        }
        else
        {
            // ignore, it is done locally
            LOG_DBG("Ignore send stub registration message, Stub [ %s ] and Proxy [ %s ] have same origin."
                            , StubAddress::to_path(addrStub).as_string()
                            , ProxyAddress::to_path(proxy).as_string());
        }
    }
    else
    {
        LOG_DBG("Proxy [ %s ] is already having connected status, ignoring registration", ProxyAddress::to_path(proxy).as_string());
    }
}

void RouterServerService::on_provider_unregistered(const StubAddress & stub, NEService::DisconnectReason reason, const ITEM_ID & cookie /*= NEService::COOKIE_ANY*/ )
{
    LOG_SCOPE(mtrouter_service_RouterServerService_unregisteredRemoteServiceProvider);
    if ( mServiceRegistry.service_status(stub) == NEService::ServiceConnectionState::Connected )
    {
        ListServiceProxies listProxies;
        mServiceRegistry.unregister_service_stub(stub, listProxies);
        LOG_DBG("Unregistered stub [ %s ], [ %d ] proxies are going to be notified"
                        , stub.to_string().as_string()
                        , listProxies.size());

        LOG_DBG("Filter sources [ %u ] of proxy list", static_cast<uint32_t>(cookie));

        ArrayList<ITEM_ID> sendList;
        for (ListServiceProxiesBase::LISTPOS pos = listProxies.first_position(); listProxies.is_valid_position(pos); pos = listProxies.next_position(pos) )
        {
            const ServiceProxy & proxyService = listProxies.value_at_position(pos);
            const ProxyAddress & addrProxy    = proxyService.service_address();

            if ( (cookie == NEService::COOKIE_ANY) || (addrProxy.source() != cookie) )
            {
                // no need to send message to unregistered stub, only to proxy side
                if (sendList.add_if_unique(addrProxy.source()) )
                {
                    send_message(NERemoteService::service_unregistered_event( stub, reason, mServerConnection.getChannelId(), addrProxy.source( ) ) );

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

void RouterServerService::on_consumer_unregistered(const ProxyAddress & proxy, NEService::DisconnectReason reason, const ITEM_ID & cookie /*= NEService::COOKIE_ANY*/ )
{
    LOG_SCOPE(mtrouter_service_RouterServerService_unregisteredRemoteServiceConsumer);
    LOG_DBG("Unregistering services of proxy [ %s ] related to cookie [ %u ]"
                    , ProxyAddress::to_path(proxy).as_string()
                    , static_cast<uint32_t>(cookie));

    RemoteMessage msgRegisterProxy;
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
    const StubAddress & addrStub    = svcStub->service_address();

    if ((svcStub->service_status() == NEService::ServiceConnectionState::Connected) && (proxy.source() != addrStub.source()))
    {
        send_message(NERemoteService::client_unregistered_event( proxy, reason, mServerConnection.getChannelId(), addrStub.source( ) ) );

        LOG_INFO("Send proxy [ %s ] disconnect message to stub [ %s ]"
                        , proxy.to_string().as_string()
                        , addrStub.to_string().as_string());
    }
    else
    {
        LOG_DBG("Ignore notifying proxy [ %s ] disconnect"
                        , proxy.to_string().as_string());
    }
}

void RouterServerService::on_service_channel_connected(const Channel & /* channel */)
{

}

void RouterServerService::on_service_channel_disconnected(const Channel & /* channel */)
{

}

void RouterServerService::on_service_channel_lost(const Channel & /* channel */)
{
}

void RouterServerService::failed_process_message(const RemoteMessage & /* msgUnprocessed */)
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
