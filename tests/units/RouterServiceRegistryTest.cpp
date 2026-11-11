/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/RouterServiceRegistryTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Router registry, endpoint value objects and registration wire contracts.
 ************************************************************************/

#include "units/GUnitTest.hpp"

#include "mtrouter/service/private/ServiceRegistry.hpp"
#include "areg/base/Version.hpp"
#include "areg/ipc/RemoteServiceDefs.hpp"

#include <cstring>
#include <functional>
#include <utility>

namespace
{
    areg::StubAddress provider_address(ITEM_ID source = 0x100u, uint32_t role = 0x1234u)
    {
        areg::StubAddress address(0x4321u, areg::Version(1u, 2u, 3u), areg::ServiceType::Public,
                                 role, 0x1111u, 0x2222u);
        address.set_cookie(source);
        address.set_source(source);
        return address;
    }

    areg::ProxyAddress consumer_address(ITEM_ID source = 0x101u, uint32_t thread = 0x3333u, uint32_t role = 0x1234u)
    {
        areg::ProxyAddress address(0x4321u, areg::Version(1u, 2u, 3u), areg::ServiceType::Public,
                                  role, thread, 0x4444u);
        address.set_cookie(source);
        address.set_source(source);
        return address;
    }

    TEST(RouterServiceStubTest, copy_move_identity_and_connection_state)
    {
        const areg::StubAddress address{ provider_address() };
        const areg::ProxyAddress consumer{ consumer_address() };
        const areg::StubAddress different{ provider_address(0x100u, 0x5678u) };
        ServiceStub empty;
        ASSERT_TRUE(address.is_valid() && consumer.is_valid()) << "numeric endpoints are structurally valid";
        ASSERT_TRUE(!empty.is_valid() && !empty.is_connected() && !empty.is_waiting()
                      && empty.service_status() == areg::ServiceConnectionState::Unknown) << "default stub is unknown and invalid";

        empty.set_service_status(areg::ServiceConnectionState::Connected);
        ASSERT_TRUE(!empty.is_connected() && empty.service_status() == areg::ServiceConnectionState::Unknown) << "invalid stub cannot connect";

        ServiceStub pending(address);
        ServiceStub copied(pending);
        ServiceStub moved(std::move(copied));
        ServiceStub assigned;
        assigned = pending;
        ServiceStub moveAssigned;
        moveAssigned = std::move(assigned);
        ServiceStub fromRvalue(provider_address());
        ServiceStub addressAssigned;
        addressAssigned = address;
        ServiceStub addressMoveAssigned;
        addressMoveAssigned = provider_address();
        const ServiceStub * values[]{ &pending, &moved, &moveAssigned, &fromRvalue, &addressAssigned, &addressMoveAssigned };
        for (const ServiceStub * value : values)
        {
            ASSERT_TRUE(value->is_valid() && value->is_waiting() && !value->is_connected()
                       && value->service_address() == address && value->service_address().source() == 0x100u) << "copy and move preserve the pending stub endpoint";
        }

        ASSERT_TRUE(pending == address && pending == consumer && pending == moved && !(pending == different)
                   && static_cast<uint32_t>(pending) == static_cast<uint32_t>(static_cast<const areg::ServiceAddress &>(address))
                   && std::hash<ServiceStub>{}(pending) == std::hash<ServiceStub>{}(moved)) << "stub identity and hash agree with service identity";

        pending.set_service_status(areg::ServiceConnectionState::Connected);
        ASSERT_TRUE(pending.is_connected() && pending.is_valid()) << "a sourced stub connects";

        pending.set_service_status(areg::ServiceConnectionState::Pending);
        ASSERT_TRUE(pending.is_waiting() && !pending.is_valid()
                   && pending.service_address().source() == areg::SOURCE_UNKNOWN) << "pending resets the departed provider source";

        pending.set_service_status(areg::ServiceConnectionState::Connected);
        ASSERT_TRUE(pending.is_waiting() && !pending.is_connected()) << "a stub without a source cannot reconnect";

        pending.set_service(address);
        ASSERT_TRUE(pending.is_connected() && pending.is_valid() && pending.service_address().source() == 0x100u) << "setting a complete provider reconnects it";
    }

    TEST(RouterServiceStubTest, service_and_consumer_lookup_keys)
    {
        const areg::StubAddress address{ provider_address() };
        const areg::ProxyAddress consumer{ consumer_address() };
        const areg::ServiceAddress service(address);
        ServiceStub byService(service);
        ServiceStub byMovedService{ areg::ServiceAddress(service) };
        ServiceStub byConsumer(consumer);
        ServiceStub assigned;
        assigned = consumer;
        ASSERT_TRUE(!byService.is_valid() && !byMovedService.is_valid()
                     && byService.service_status() == areg::ServiceConnectionState::Unknown
                     && byMovedService.service_status() == areg::ServiceConnectionState::Unknown) << "service-only lookup keys are not connected provider endpoints";
        ASSERT_TRUE(byConsumer == address && assigned == address && byService == byMovedService
                     && std::equal_to<ServiceStub>{}(byConsumer, byService)
                     && std::hash<ServiceStub>{}(assigned) == std::hash<ServiceStub>{}(byService)) << "proxy and service lookup keys match the same provider identity";
    }

    TEST(RouterServiceStubTest, invalid_and_unsourced_endpoints_do_not_connect)
    {
        const areg::StubAddress invalid;
        const areg::ProxyAddress invalidConsumer;
        ServiceStub copied(invalid);
        ServiceStub moved{ areg::StubAddress() };
        ServiceStub fromConsumer(invalidConsumer);
        ServiceStub assigned(provider_address());
        assigned = invalid;
        ServiceStub moveAssigned(provider_address());
        moveAssigned = areg::StubAddress();
        ServiceStub proxyAssigned(provider_address());
        proxyAssigned = invalidConsumer;
        const ServiceStub * values[]{ &copied, &moved, &fromConsumer, &assigned, &moveAssigned, &proxyAssigned };
        for (const ServiceStub * value : values)
        {
            ASSERT_TRUE(!value->is_valid() && !value->is_connected() && !value->is_waiting()
                       && value->service_status() == areg::ServiceConnectionState::Unknown) << "invalid provider and consumer addresses do not become registered stubs";
        }

        assigned.set_service(invalid);
        ASSERT_TRUE(!assigned.is_valid() && assigned.service_status() == areg::ServiceConnectionState::Unknown) << "setting an invalid provider leaves the stub unknown";

        areg::StubAddress unsourced{ provider_address() };
        unsourced.set_source(areg::SOURCE_UNKNOWN);
        moved.set_service(unsourced);
        ASSERT_TRUE(moved.is_waiting() && !moved.is_connected() && !moved.is_valid()) << "a structurally valid but unsourced provider waits instead of connecting";
    }

    TEST(RouterServiceProxyTest, copy_move_identity_and_target_lifecycle)
    {
        const areg::ProxyAddress address{ consumer_address() };
        const areg::StubAddress provider{ provider_address() };
        ServiceProxy empty;
        ASSERT_TRUE(!empty.is_valid() && !empty.is_connected() && !empty.is_waiting()
                   && empty.service_status() == areg::ServiceConnectionState::Unknown
                   && !empty.stub_available(provider) && !empty.stub_unavailable()) << "invalid proxy remains unknown";

        ServiceProxy pending(address);
        ServiceProxy copied(pending);
        ServiceProxy moved(std::move(copied));
        ServiceProxy assigned;
        assigned = pending;
        ServiceProxy moveAssigned;
        moveAssigned = std::move(assigned);
        ServiceProxy fromRvalue(consumer_address());
        ServiceProxy addressAssigned;
        addressAssigned = address;
        ServiceProxy addressMoveAssigned;
        addressMoveAssigned = consumer_address();
        const ServiceProxy * values[]{ &pending, &moved, &moveAssigned, &fromRvalue, &addressAssigned, &addressMoveAssigned };
        for (const ServiceProxy * value : values)
        {
            ASSERT_TRUE(value->is_valid() && value->is_waiting() && !value->is_connected()
                       && value->service_address() == address && value->service_address().source() == 0x101u) << "copy and move preserve the pending consumer endpoint";
        }

        ASSERT_TRUE(pending == moved && pending == address && pending == provider
                   && !(pending == consumer_address(0x102u)) && !(pending == consumer_address(0x101u, 0x5555u))
                   && !(pending == consumer_address(0x101u, 0x3333u, 0x5678u))) << "proxy identity distinguishes process, thread and service";

        ServiceProxy fromProvider(provider);
        ASSERT_TRUE(fromProvider.service_status() == areg::ServiceConnectionState::Unknown && fromProvider == provider) << "provider-only lookup has service identity but no consumer connection";
        ASSERT_TRUE(pending.stub_available(provider) && pending.is_connected()
                      && pending.service_address().target() == 0x100u) << "availability connects the proxy to the provider source";

        ServiceProxy connectedCopy(pending);
        ServiceProxy connectedMove(std::move(connectedCopy));
        assigned = pending;
        moveAssigned = std::move(assigned);
        ASSERT_TRUE(connectedMove.is_connected() && moveAssigned.is_connected()
                   && connectedMove.service_address().target() == 0x100u && moveAssigned.service_address().target() == 0x100u) << "copy and move preserve a connected proxy and target";
        ASSERT_TRUE(pending.stub_unavailable() && pending.is_waiting()
                      && pending.service_address().target() == areg::TARGET_UNKNOWN) << "unavailability clears the target";

        const areg::StubAddress invalid;
        ASSERT_TRUE(!pending.stub_available(invalid) && pending.is_waiting()) << "an invalid provider cannot connect a waiting proxy";

        addressAssigned = areg::ProxyAddress();
        addressMoveAssigned = areg::ProxyAddress();
        const areg::ProxyAddress invalidProxy;
        ServiceProxy invalidCopy(invalidProxy);
        ServiceProxy invalidMove{ areg::ProxyAddress() };
        ASSERT_TRUE(!addressAssigned.is_valid() && !addressMoveAssigned.is_valid() && !invalidCopy.is_valid() && !invalidMove.is_valid()
                     && invalidCopy.service_status() == areg::ServiceConnectionState::Unknown
                     && invalidMove.service_status() == areg::ServiceConnectionState::Unknown) << "invalid consumer copies and moves remain unknown";
    }

    TEST(RouterServiceRegistryTest, missing_endpoints_leave_registry_empty)
    {
        ServiceRegistry registry;
        const areg::StubAddress provider{ provider_address() };
        const areg::ProxyAddress consumer{ consumer_address() };
        ServiceProxy removed;
        ListServiceProxies proxies;
        areg::ArrayList<areg::StubAddress> providers;
        areg::ArrayList<areg::ProxyAddress> consumers;
        registry.service_list(areg::COOKIE_ANY, providers, consumers);
        registry.service_sources(0x100u, providers, consumers);
        ASSERT_TRUE(!registry.is_service_registered(provider) && !registry.is_service_registered(consumer)
                     && !registry.stub_service(provider).is_valid() && !registry.proxy_service(consumer).is_valid()
                     && registry.proxy_service_list(provider).is_empty()
                     && registry.service_status(provider) == areg::ServiceConnectionState::Unknown
                     && registry.service_status(consumer) == areg::ServiceConnectionState::Unknown) << "missing registrations have unknown state and empty lookups";
        ASSERT_TRUE(!registry.unregister_service_proxy(consumer, removed).is_valid() && !removed.is_valid()
                     && !registry.unregister_service_provider(provider, proxies).is_valid()
                     && !registry.disconnect_proxy(consumer).is_valid() && registry.is_empty()
                     && providers.is_empty() && consumers.is_empty()) << "missing cleanup leaves an empty registry";
    }

    void registry_lifecycle(bool providerFirst)
    {
        ServiceRegistry registry;
        const areg::StubAddress provider{ provider_address() };
        const areg::ProxyAddress consumer{ consumer_address() };
        ServiceProxy proxy;
        ListServiceProxies proxies;
        if (providerFirst)
        {
            ASSERT_TRUE(registry.register_service_provider(provider, proxies).is_connected() && proxies.is_empty()) << "provider can register without consumers";
        }
        else
        {
            registry.register_service_proxy(consumer, proxy);
            ASSERT_TRUE(proxy.is_waiting() && !registry.stub_service(consumer).is_valid()) << "consumer waits without a provider";
        }

        registry.register_service_proxy(consumer, proxy);
        registry.register_service_provider(provider, proxies);
        ASSERT_TRUE(registry.size() == 1u && proxies.size() == 1u && registry.is_service_registered(provider)
                   && registry.is_service_registered(consumer) && registry.service_status(provider) == areg::ServiceConnectionState::Connected
                   && registry.service_status(consumer) == areg::ServiceConnectionState::Connected
                   && registry.proxy_service(consumer).service_address().target() == 0x100u) << "one provider and one consumer connect in either registration order";

        registry.register_service_proxy(consumer, proxy);
        ASSERT_TRUE(registry.proxy_service_list(provider).size() == 1u && proxy.is_connected()) << "duplicate consumer registration is idempotent";

        registry.unregister_service_provider(provider, proxies);
        ASSERT_TRUE(proxies.size() == 1u && registry.service_status(consumer) == areg::ServiceConnectionState::Pending
                   && registry.proxy_service(consumer).service_address().target() == areg::TARGET_UNKNOWN
                   && !registry.stub_service(provider).is_valid()) << "departed provider leaves the consumer pending with no target";

        areg::ArrayList<areg::StubAddress> providers;
        areg::ArrayList<areg::ProxyAddress> consumers;
        registry.service_list(areg::COOKIE_ANY, providers, consumers);
        ASSERT_TRUE(providers.is_empty() && consumers.size() == 1u && consumers[0] == consumer) << "enumeration retains the waiting consumer but excludes the departed provider";

        const areg::StubAddress replacement{ provider_address(0x102u) };
        registry.register_service_provider(replacement, proxies);
        ASSERT_TRUE(proxies.size() == 1u && registry.proxy_service(consumer).is_connected()
                   && registry.proxy_service(consumer).service_address().target() == 0x102u) << "a replacement provider reconnects the waiting consumer";

        registry.unregister_service_proxy(consumer, proxy);
        ASSERT_TRUE(proxy.is_connected() && proxy == consumer && !registry.is_service_registered(consumer)
                   && registry.stub_service(replacement).is_connected()) << "consumer removal preserves its provider";

        registry.unregister_service_provider(replacement, proxies);
        ASSERT_TRUE(proxies.is_empty() && registry.is_empty()) << "the last provider removal erases the registry entry";
    }

    TEST(RouterServiceRegistryTest, removing_last_waiting_consumer_erases_placeholder)
    {
        ServiceRegistry registry;
        const areg::ProxyAddress first{ consumer_address() };
        const areg::ProxyAddress second{ consumer_address(0x102u, 0x5555u) };
        ServiceProxy proxy;
        registry.register_service_proxy(first, proxy);
        registry.register_service_proxy(second, proxy);
        registry.unregister_service_proxy(first, proxy);
        ASSERT_TRUE(proxy == first && proxy.is_waiting() && registry.proxy_service_list(second).size() == 1u
                   && registry.is_service_registered(second)) << "removing one waiting consumer preserves the other";

        registry.unregister_service_proxy(second, proxy);
        ASSERT_TRUE(proxy == second && proxy.is_waiting() && registry.is_empty()) << "removing the last waiting consumer erases the placeholder";
    }

    TEST(RouterServiceRegistryTest, disconnect_and_reregister_preserve_sibling)
    {
        ServiceRegistry registry;
        const areg::StubAddress provider{ provider_address() };
        const areg::ProxyAddress first{ consumer_address() };
        const areg::ProxyAddress second{ consumer_address(0x102u, 0x5555u) };
        const areg::ProxyAddress missing{ consumer_address(0x103u) };
        ServiceProxy proxy;
        ListServiceProxies proxies;
        registry.register_service_provider(provider, proxies);
        registry.register_service_proxy(first, proxy);
        registry.register_service_proxy(second, proxy);
        ASSERT_TRUE(registry.disconnect_proxy(first).is_connected() && registry.proxy_service(first).is_waiting()
                   && registry.proxy_service(first).service_address().target() == areg::TARGET_UNKNOWN
                   && registry.proxy_service(second).is_connected() && registry.is_service_registered(first)) << "disconnect retains one consumer as pending without affecting its sibling";
        ASSERT_TRUE(registry.disconnect_proxy(missing).is_connected() && !registry.is_service_registered(missing)
                      && registry.proxy_service_list(provider).size() == 2u) << "disconnecting a missing consumer preserves the provider and peers";

        registry.register_service_proxy(first, proxy);
        ASSERT_TRUE(proxy.is_connected() && proxy.service_address().target() == 0x100u
                     && registry.proxy_service_list(provider).size() == 2u) << "registration reconnects the pending consumer without duplication";
    }

    TEST(RouterServiceRegistryTest, cookie_and_source_filters_are_distinct)
    {
        ServiceRegistry registry;
        areg::StubAddress provider{ provider_address() };
        areg::ProxyAddress consumer{ consumer_address() };
        provider.set_source(0x200u);
        consumer.set_source(0x201u);
        ServiceProxy proxy;
        ListServiceProxies proxies;
        registry.register_service_provider(provider, proxies);
        registry.register_service_proxy(consumer, proxy);
        areg::ArrayList<areg::StubAddress> providers;
        areg::ArrayList<areg::ProxyAddress> consumers;
        registry.service_list(areg::COOKIE_ANY, providers, consumers);
        ASSERT_TRUE(providers.size() == 1u && consumers.size() == 1u && providers[0] == provider && consumers[0] == consumer) << "wildcard cookie enumeration returns both registered endpoints";

        providers.clear();
        consumers.clear();
        registry.service_list(0x100u, providers, consumers);
        ASSERT_TRUE(providers.size() == 1u && consumers.is_empty()) << "provider cookie filter excludes the consumer";

        providers.clear();
        registry.service_list(0x101u, providers, consumers);
        ASSERT_TRUE(providers.is_empty() && consumers.size() == 1u) << "consumer cookie filter excludes the provider";

        consumers.clear();
        registry.service_sources(0x200u, providers, consumers);
        ASSERT_TRUE(providers.size() == 1u && consumers.is_empty() && providers[0].source() == 0x200u) << "source filter uses the provider source rather than its cookie";

        providers.clear();
        registry.service_sources(0x201u, providers, consumers);
        ASSERT_TRUE(providers.is_empty() && consumers.size() == 1u && consumers[0].source() == 0x201u) << "source filter uses the consumer source rather than its cookie";

        consumers.clear();
        registry.service_list(0x999u, providers, consumers);
        registry.service_sources(0x999u, providers, consumers);
        ASSERT_TRUE(providers.is_empty() && consumers.is_empty()) << "unmatched cookie and source filters return no endpoints";
    }

    TEST(RouterListServiceProxiesTest, unique_endpoints_selection_copy_move_and_availability)
    {
        ListServiceProxies list;
        const areg::ProxyAddress first{ consumer_address() };
        const areg::ProxyAddress sibling{ consumer_address(0x101u, 0x5555u) };
        const areg::ProxyAddress second{ consumer_address(0x102u, 0x6666u) };
        const areg::StubAddress provider{ provider_address() };
        const ListServiceProxies & view{ list };
        ASSERT_TRUE(list.service(first) == nullptr && !view.service(first).is_valid()
                   && list.stub_service_available(provider) == 0 && list.stub_service_unavailable() == 0) << "empty list lookups and availability updates report no consumers";

        list.register_service(first);
        list.register_service(sibling);
        list.register_service(second);
        list.register_service(first);
        ASSERT_TRUE(list.size() == 3u && list.is_service_registered(first)
                   && list.stub_service_available(provider) == 3 && view.service(second).is_connected()
                   && view.service(second).service_address().target() == 0x100u) << "availability connects all unique consumer endpoints";

        ListServiceProxies selected;
        selected.register_service(second);
        ASSERT_TRUE(list.specific_service(selected, 0x101u) == 2 && selected.size() == 3u
                   && selected.is_service_registered(first) && selected.is_service_registered(sibling)
                   && list.specific_service(selected, 0x999u) == 0 && selected.size() == 3u) << "cookie selection appends exactly its two matching threads";

        ListServiceProxies copied(list);
        ListServiceProxies moved(std::move(copied));
        ListServiceProxies assigned;
        assigned = list;
        ListServiceProxies moveAssigned;
        moveAssigned = std::move(assigned);
        ASSERT_TRUE(moved.size() == 3u && moveAssigned.size() == 3u && moved.service(first)->is_connected()
                   && moveAssigned.service(second)->is_connected()) << "copy and move preserve the list entries and connection states";

        const ServiceProxy removed{ list.unregister_service(first) };
        ASSERT_TRUE(removed == first && removed.is_connected() && list.size() == 2u && !list.is_service_registered(first)
                     && !list.unregister_service(first).is_valid() && list.stub_service_unavailable() == 2
                     && view.service(sibling).is_waiting() && view.service(sibling).service_address().target() == areg::TARGET_UNKNOWN) << "removal returns its endpoint and unavailability resets every remaining target";
    }

    TEST(RouterRegistrationWireTest, factories_preserve_routing_identity_version_and_reason)
    {
        const areg::StubAddress provider{ provider_address() };
        const areg::ProxyAddress consumer{ consumer_address() };
        struct Case
        {
            areg::MessageEnvelope message;
            areg::RegistrationAction action;
            bool provider;
            bool notification;
            areg::DisconnectReason reason;
        };
        const Case cases[]
        {
            { areg::router_register_service(provider, 0x100u, areg::COOKIE_ROUTER),
              areg::RegistrationAction::RegisterProvider, true, false, areg::DisconnectReason::UndefinedReason },
            { areg::router_unregister_service(provider, areg::DisconnectReason::ProviderRestarting, 0x100u, areg::COOKIE_ROUTER),
              areg::RegistrationAction::UnregisterProvider, true, false, areg::DisconnectReason::ProviderRestarting },
            { areg::router_register_consumer(consumer, 0x100u, areg::COOKIE_ROUTER),
              areg::RegistrationAction::RegisterConsumer, false, false, areg::DisconnectReason::UndefinedReason },
            { areg::router_unregister_consumer(consumer, areg::DisconnectReason::ConsumerDisconnected, 0x100u, areg::COOKIE_ROUTER),
              areg::RegistrationAction::UnregisterConsumer, false, false, areg::DisconnectReason::ConsumerDisconnected },
            { areg::service_registered_event(provider, areg::COOKIE_ROUTER, 0x100u),
              areg::RegistrationAction::RegisterProvider, true, true, areg::DisconnectReason::UndefinedReason },
            { areg::service_unregistered_event(provider, areg::DisconnectReason::ProviderRestarting, areg::COOKIE_ROUTER, 0x100u),
              areg::RegistrationAction::UnregisterProvider, true, true, areg::DisconnectReason::ProviderRestarting },
            { areg::client_registered_event(consumer, areg::COOKIE_ROUTER, 0x100u),
              areg::RegistrationAction::RegisterConsumer, false, true, areg::DisconnectReason::UndefinedReason },
            { areg::client_unregistered_event(consumer, areg::DisconnectReason::ConsumerDisconnected, areg::COOKIE_ROUTER, 0x100u),
              areg::RegistrationAction::UnregisterConsumer, false, true, areg::DisconnectReason::ConsumerDisconnected }
        };
        for (const Case & entry : cases)
        {
            ASSERT_TRUE(entry.message.is_valid()) << "registration factory produces a valid envelope";

            const areg::EventHeader & header{ *entry.message.header() };
            const areg::Endpoint & endpoint{ entry.provider ? header.provider : header.consumer };
            ASSERT_TRUE(header.messageId == static_cast<uint32_t>(entry.notification ? areg::FuncIdRange::SystemServiceNotifyRegister
                                                                                  : areg::FuncIdRange::SystemServiceRequestRegister)
                       && header.callType == static_cast<uint8_t>(entry.action)
                       && header.result == static_cast<uint32_t>(entry.reason)
                       && header.source == (entry.notification ? static_cast<uint32_t>(areg::COOKIE_ROUTER) : 0x100u)
                       && header.target == (entry.notification ? 0x100u : static_cast<uint32_t>(areg::COOKIE_ROUTER))
                       && header.bufHeader.biUsed == 0u && areg::wire_size(header) == areg::WIRE_HEADER_SIZE) << "registration action, reason and routing need only the wire header";
            ASSERT_TRUE(header.rawService.service == 0x4321u && header.rawService.role == 0x1234u
                          && endpoint.id == (entry.provider ? 0x100u : 0x101u)
                          && endpoint.thread == (entry.provider ? 0x1111u : 0x3333u)
                          && endpoint.number == (entry.provider ? 0x2222u : 0x4444u)
                          && endpoint.major == 1u && endpoint.minor == 2u && endpoint.patch == 3u
                          && endpoint.type == static_cast<uint16_t>(areg::ServiceType::Public)) << "wire endpoint carries the exact service identity, version and instance";

            areg::EventHeader decoded{ };
            std::memcpy(areg::wire_header(decoded), areg::wire_data(header), areg::WIRE_HEADER_SIZE);
            if (entry.provider)
            {
                const areg::StubAddress restored(decoded);
                ASSERT_TRUE(restored == provider && restored.thread() == provider.thread()
                           && static_cast<const areg::ServiceAddress &>(restored) == static_cast<const areg::ServiceAddress &>(provider)) << "provider address survives a wire-header round trip";
            }
            else
            {
                const areg::ProxyAddress restored(decoded);
                ASSERT_TRUE(restored == consumer && restored.thread() == consumer.thread()
                           && static_cast<const areg::ServiceAddress &>(restored) == static_cast<const areg::ServiceAddress &>(consumer)) << "consumer address survives a wire-header round trip";
            }
        }
    }

    TEST(RouterRegistrationWireTest, local_sources_and_services_cannot_register_remotely)
    {
        const areg::StubAddress provider{ provider_address() };
        const areg::ProxyAddress consumer{ consumer_address() };
        const areg::StubAddress localProvider(0x4321u, areg::Version(1u, 2u, 3u), areg::ServiceType::Local,
                                              0x1234u, 0x1111u, 0x2222u);
        const areg::ProxyAddress localConsumer(0x4321u, areg::Version(1u, 2u, 3u), areg::ServiceType::Local,
                                               0x1234u, 0x3333u, 0x4444u);
        const ITEM_ID invalidSources[]{ areg::COOKIE_UNKNOWN, areg::COOKIE_LOCAL };
        for (ITEM_ID source : invalidSources)
        {
            ASSERT_TRUE(!areg::router_register_service(provider, source, areg::COOKIE_ROUTER).is_valid()
                       && !areg::router_register_consumer(consumer, source, areg::COOKIE_ROUTER).is_valid()
                       && !areg::router_unregister_service(provider, areg::DisconnectReason::ProviderDisconnected,
                                                          source, areg::COOKIE_ROUTER).is_valid()
                       && !areg::router_unregister_consumer(consumer, areg::DisconnectReason::ConsumerDisconnected,
                                                           source, areg::COOKIE_ROUTER).is_valid()) << "unknown and local sources cannot register remote services";
        }

        ASSERT_TRUE(!areg::router_register_service(localProvider, 0x100u, areg::COOKIE_ROUTER).is_valid()
                     && !areg::router_register_consumer(localConsumer, 0x101u, areg::COOKIE_ROUTER).is_valid()
                     && !areg::service_registered_event(localProvider, areg::COOKIE_ROUTER, 0x101u).is_valid()
                     && !areg::client_registered_event(localConsumer, areg::COOKIE_ROUTER, 0x100u).is_valid()) << "local-only services never produce remote registration messages";
    }
}

TEST(RouterServiceRegistryTest, provider_first_lifecycle)
{
    ASSERT_NO_FATAL_FAILURE(registry_lifecycle(true));
}

TEST(RouterServiceRegistryTest, consumer_first_lifecycle)
{
    ASSERT_NO_FATAL_FAILURE(registry_lifecycle(false));
}

// Reproduces the defect: the wildcard was compared with every source, so it matched none of them.
TEST(RouterServiceRegistryTest, source_wildcard_returns_all_endpoints)
{
    ServiceRegistry registry;
    areg::StubAddress provider{ provider_address() };
    areg::ProxyAddress consumer{ consumer_address() };
    provider.set_source(0x200u);
    consumer.set_source(0x201u);
    ServiceProxy proxy;
    ListServiceProxies proxies;
    registry.register_service_provider(provider, proxies);
    registry.register_service_proxy(consumer, proxy);
    areg::ArrayList<areg::StubAddress> providers;
    areg::ArrayList<areg::ProxyAddress> consumers;

    registry.service_sources(areg::COOKIE_ANY, providers, consumers);

    ASSERT_EQ(1u, providers.size());
    ASSERT_EQ(1u, consumers.size());
    EXPECT_EQ(0x200u, providers[0].source());
    EXPECT_EQ(0x201u, consumers[0].source());

    areg::ArrayList<areg::StubAddress> exactProviders;
    areg::ArrayList<areg::ProxyAddress> exactConsumers;
    registry.service_sources(0x200u, exactProviders, exactConsumers);
    EXPECT_EQ(1u, exactProviders.size());
    EXPECT_EQ(0u, exactConsumers.size());
}

/**
 * \brief   The service list filters the providers and the consumers by the cookie: any cookie
 *          returns both, the cookie of one side returns only that side, an unknown cookie none.
 **/
TEST(RouterServiceRegistryTest, service_list_filters_by_cookie)
{
    ServiceRegistry registry;
    const areg::StubAddress provider{ provider_address(0x200u) };
    const areg::ProxyAddress consumer{ consumer_address(0x201u) };
    ListServiceProxies proxies;
    ServiceProxy proxy;
    registry.register_service_provider(provider, proxies);
    registry.register_service_proxy(consumer, proxy);

    const auto count = [&registry](ITEM_ID cookie) -> std::pair<uint32_t, uint32_t>
    {
        areg::ArrayList<areg::StubAddress> providers;
        areg::ArrayList<areg::ProxyAddress> consumers;
        registry.service_list(cookie, providers, consumers);
        return { providers.size(), consumers.size() };
    };

    EXPECT_EQ(count(areg::COOKIE_ANY), std::make_pair(1u, 1u));
    EXPECT_EQ(count(0x200u), std::make_pair(1u, 0u));
    EXPECT_EQ(count(0x201u), std::make_pair(0u, 1u));
    EXPECT_EQ(count(0x777u), std::make_pair(0u, 0u));

    areg::ArrayList<areg::StubAddress> stubs;
    areg::ArrayList<areg::ProxyAddress> sources;
    registry.service_sources(0x201u, stubs, sources);
    EXPECT_EQ(stubs.size(), 0u);
    ASSERT_EQ(sources.size(), 1u);
    EXPECT_EQ(sources[0].source(), 0x201u);
}

/**
 * \brief   Unregistering a provider that is not registered returns the invalid provider and
 *          leaves the registered one in place.
 **/
TEST(RouterServiceRegistryTest, unregister_of_an_unknown_provider_changes_nothing)
{
    ServiceRegistry registry;
    ListServiceProxies proxies;
    registry.register_service_provider(provider_address(0x200u), proxies);

    ListServiceProxies removed;
    const ServiceStub & result{ registry.unregister_service_provider(provider_address(0x200u, 0x9999u), removed) };
    EXPECT_FALSE(result.is_valid());
    EXPECT_TRUE(removed.is_empty());
    areg::ArrayList<areg::StubAddress> providers;
    areg::ArrayList<areg::ProxyAddress> consumers;
    registry.service_list(areg::COOKIE_ANY, providers, consumers);
    ASSERT_EQ(providers.size(), 1u);
    EXPECT_EQ(providers[0], provider_address(0x200u));
}

/**
 * \brief   Disconnecting a consumer of an unknown service, or an unknown consumer of a known
 *          service, changes nothing; a known consumer loses its provider.
 **/
TEST(RouterServiceRegistryTest, disconnect_proxy_acts_only_on_a_registered_consumer)
{
    ServiceRegistry registry;
    const areg::StubAddress provider{ provider_address(0x200u) };
    const areg::ProxyAddress consumer{ consumer_address(0x201u) };
    ListServiceProxies proxies;
    ServiceProxy proxy;
    registry.register_service_provider(provider, proxies);
    registry.register_service_proxy(consumer, proxy);
    ASSERT_EQ(proxy.service_status(), areg::ServiceConnectionState::Connected);

    EXPECT_FALSE(registry.disconnect_proxy(consumer_address(0x201u, 0x3333u, 0x9999u)).is_valid());

    const ServiceStub & known{ registry.disconnect_proxy(consumer_address(0x202u, 0x7777u)) };
    EXPECT_TRUE(known.is_valid());
    const ListServiceProxies & list{ registry.proxy_service_list(provider) };
    ASSERT_EQ(list.size(), 1u);
    EXPECT_EQ(list.value_at(list.first_position()).service_status(), areg::ServiceConnectionState::Connected);

    EXPECT_TRUE(registry.disconnect_proxy(consumer).is_valid());
    EXPECT_NE(list.value_at(list.first_position()).service_status(), areg::ServiceConnectionState::Connected);
}

/**
 * \brief   A consumer registered with a provider that is not connected waits for it.
 **/
TEST(RouterServiceRegistryTest, consumer_of_a_provider_that_is_not_connected_waits)
{
    const areg::StubAddress provider{ provider_address(0x200u) };
    const ServiceStub pending(provider);
    ASSERT_TRUE(pending.is_waiting());

    ListServiceProxies proxies;
    const ServiceProxy & proxy{ proxies.register_service(consumer_address(0x201u), pending) };
    EXPECT_NE(proxy.service_status(), areg::ServiceConnectionState::Connected);
    EXPECT_EQ(proxies.size(), 1u);
}
