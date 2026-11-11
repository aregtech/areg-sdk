/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Copyright (c) 2017-2026 Aregtech (Artak Avetyan).
 * Areg SDK is dual-licensed under Apache 2.0 and commercial licenses.
 *
 * \file        units/NameNumberCollisionTest.cpp
 * \brief       Names that give one number: event classes, address parts, name checks,
 *              registries of threads, components, providers and proxies, local pairing
 *              and the router registry.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "units/EventTestSupport.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/base/SharedBuffer.hpp"
#include "areg/base/Thread.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentAddress.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/NotificationEvent.hpp"
#include "areg/component/ProxyAddress.hpp"
#include "areg/component/ProxyBase.hpp"
#include "areg/component/ProxyListener.hpp"
#include "areg/component/RequestEvents.hpp"
#include "areg/component/ResponseEvents.hpp"
#include "areg/component/StubAddress.hpp"
#include "areg/component/StubBase.hpp"
#include "areg/ipc/RemoteServiceDefs.hpp"
#include "areg/logging/LogScope.hpp"
#include "areg/logging/private/ScopeController.hpp"
#include "mtrouter/service/private/ServiceRegistry.hpp"

#include <any>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <future>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <utility>

//////////////////////////////////////////////////////////////////////////
// Event classes of one name in two namespaces
//////////////////////////////////////////////////////////////////////////

namespace nc_first
{
    struct CollisionData
    {
        int32_t value;
    };

    AREG_DECLARE_EVENT(CollisionData, CollisionEvent, CollisionConsumer);
}

namespace nc_second
{
    struct CollisionData
    {
        int32_t value;
    };

    AREG_DECLARE_EVENT(CollisionData, CollisionEvent, CollisionConsumer);
}

//////////////////////////////////////////////////////////////////////////
// An event of an anonymous namespace; NameNumberCollisionPeer.cpp has one of the same names
//////////////////////////////////////////////////////////////////////////

namespace
{
    struct AnonStatus
    {
        int32_t value;
    };

    AREG_DECLARE_EVENT(AnonStatus, AnonStatusEvent, AnonStatusConsumer);

    class AnonConsumer final : public AnonStatusConsumer
    {
    public:
        std::atomic<int32_t> mCalls{ 0 };

    protected:
        void process_event(const AnonStatus & /*data*/) override
        {
            mCalls.fetch_add(1);
        }
    };
}

uint32_t nc_peer_event_id();
bool nc_peer_listen(areg::DispatcherThread & dispatcher);
bool nc_peer_forget(areg::DispatcherThread & dispatcher);
int32_t nc_peer_calls();

namespace
{
    using namespace areg;

    //!< Two role names of equal length with one CRC32C number, 0x9304CF4D.
    constexpr char NAME_FIRST[]     { "Cls1371838" };
    constexpr char NAME_SECOND[]    { "Cls2000402" };

    //!< Two role names of one service with one service address number.
    constexpr char ROLE_FIRST[]     { "Device.1371838" };
    constexpr char ROLE_SECOND[]    { "Device.2000402" };

    //!< Thread names with the reserved numbers 0x00000000 and 0xFFFFFFFF.
    constexpr char NAME_ZERO[]      { "Role107750_46631" };
    constexpr char NAME_ALL_ONES[]  { "Role26697_23413" };

    constexpr char SERVICE_NAME[]   { "NcTelemetry" };

    class FirstConsumer final : public nc_first::CollisionConsumer
    {
    public:
        std::atomic<int32_t> mCalls{ 0 };

    protected:
        void process_event(const nc_first::CollisionData & /*data*/) override
        {
            mCalls.fetch_add(1);
        }
    };

    class SecondConsumer final : public nc_second::CollisionConsumer
    {
    public:
        std::atomic<int32_t> mCalls{ 0 };

    protected:
        void process_event(const nc_second::CollisionData & /*data*/) override
        {
            mCalls.fetch_add(1);
        }
    };

    //!< The FNV-1a check of a service address, written independently of the framework.
    uint32_t expected_name_check(std::string_view service, ServiceType type, std::string_view role)
    {
        uint32_t hash{ 2'166'136'261u };
        const auto add = [&hash](uint8_t value) { hash = (hash ^ value) * 16'777'619u; };
        const uint16_t kind{ static_cast<uint16_t>(type) };
        for (char ch : service)
            add(static_cast<uint8_t>(ch));
        add(0u);
        add(static_cast<uint8_t>(kind & 0xFFu));
        add(static_cast<uint8_t>(kind >> 8));
        add(0u);
        for (char ch : role)
            add(static_cast<uint8_t>(ch));
        return (hash != 0u ? hash : 1u);
    }

    ServiceAddress service_address(const char * role, ServiceType type = ServiceType::Public)
    {
        return ServiceAddress(String(SERVICE_NAME), Version(1u, 0u, 0u), type, String(role));
    }
}

//////////////////////////////////////////////////////////////////////////
// P1: event class numbers
//////////////////////////////////////////////////////////////////////////

TEST(NameNumberEventTest, same_event_name_in_two_namespaces_has_two_numbers)
{
    EXPECT_NE(nc_first::CollisionEvent::CLASS_ID, nc_second::CollisionEvent::CLASS_ID);
    EXPECT_TRUE(areg::crc32_valid(nc_first::CollisionEvent::CLASS_ID));
    EXPECT_TRUE(areg::crc32_valid(nc_second::CollisionEvent::CLASS_ID));
}

TEST(NameNumberEventTest, type_signature_names_the_namespace)
{
    const std::string_view first{ areg::type_signature<nc_first::CollisionData>() };
    const std::string_view second{ areg::type_signature<nc_second::CollisionData>() };
    EXPECT_NE(first.find("nc_first::CollisionData"), std::string_view::npos);
    EXPECT_NE(second.find("nc_second::CollisionData"), std::string_view::npos);
    EXPECT_EQ(areg::type_class_id<nc_first::CollisionData>(), areg::crc32_calculate(first));
    EXPECT_NE(areg::type_class_id<nc_first::CollisionData>(), areg::type_class_id<nc_second::CollisionData>());
}

TEST(NameNumberEventTest, event_reaches_only_the_consumer_of_its_namespace)
{
    // Reproduces the defect: the consumer of the event of the same name in another namespace was called.
    EventTestDispatcher dispatcher{ String("NcEventDispatcher") };
    ASSERT_TRUE(dispatcher.start_ready());
    FirstConsumer first;
    SecondConsumer second;
    ASSERT_TRUE(nc_first::CollisionEvent::add_listener(first, dispatcher));
    ASSERT_TRUE(nc_second::CollisionEvent::add_listener(second, dispatcher));

    ASSERT_TRUE(nc_first::CollisionEvent::send_event(nc_first::CollisionData{ 42 }, dispatcher));
    ASSERT_TRUE(dispatcher.barrier());
    EXPECT_EQ(1, first.mCalls.load());
    EXPECT_EQ(0, second.mCalls.load());

    EXPECT_TRUE(nc_first::CollisionEvent::remove_listener(first, dispatcher));
    EXPECT_TRUE(nc_second::CollisionEvent::remove_listener(second, dispatcher));
    dispatcher.shutdown(areg::WAIT_INFINITE);
}

TEST(NameNumberEventTest, anonymous_types_of_two_files_have_two_numbers)
{
    // Reproduces the defect: the events of an anonymous namespace of two files had one number.
    EXPECT_TRUE(areg::is_anonymous_type(areg::type_signature<AnonStatus>())) << areg::type_signature<AnonStatus>();
    EXPECT_FALSE(areg::is_anonymous_type(areg::type_signature<nc_first::CollisionData>()));
    EXPECT_NE(AnonStatusEvent::CLASS_ID, nc_peer_event_id()) << AnonStatusEvent::CLASS_ID << " " << nc_peer_event_id();
    EXPECT_TRUE(areg::crc32_valid(nc_peer_event_id()));
    EXPECT_EQ(areg::type_class_id<nc_first::CollisionData>(__FILE__), areg::type_class_id<nc_first::CollisionData>());
    EXPECT_NE(areg::type_class_id<AnonStatus>(__FILE__), areg::type_class_id<AnonStatus>("other.cpp"));
}

TEST(NameNumberEventTest, anonymous_event_reaches_only_the_consumer_of_its_file)
{
    // Reproduces the defect: the consumer of the event of the same names in another file was called.
    EventTestDispatcher dispatcher{ String("NcAnonDispatcher") };
    ASSERT_TRUE(dispatcher.start_ready());
    AnonConsumer own;
    const int32_t peerBefore{ nc_peer_calls() };
    ASSERT_TRUE(AnonStatusEvent::add_listener(own, dispatcher));
    ASSERT_TRUE(nc_peer_listen(dispatcher));

    ASSERT_TRUE(AnonStatusEvent::send_event(AnonStatus{ 7 }, dispatcher));
    ASSERT_TRUE(dispatcher.barrier());
    EXPECT_EQ(1, own.mCalls.load());
    EXPECT_EQ(peerBefore, nc_peer_calls());

    EXPECT_TRUE(AnonStatusEvent::remove_listener(own, dispatcher));
    EXPECT_TRUE(nc_peer_forget(dispatcher));
    dispatcher.shutdown(areg::WAIT_INFINITE);
}

//////////////////////////////////////////////////////////////////////////
// Names longer than ITEM_NAMES_MAX_LENGTH
//////////////////////////////////////////////////////////////////////////

namespace
{
    //!< A role name of 69 characters and its first 64 characters.
    constexpr char LONG_NAME[]      { "Sensor.Building7.Floor3.Room12.Temperature.Humidity.Calibrated.Ch0042" };
    constexpr char LONG_NAME_CUT[]  { "Sensor.Building7.Floor3.Room12.Temperature.Humidity.Calibrated.C" };
}

TEST(NameNumberLengthTest, name_of_64_characters_keeps_its_number)
{
    static_assert(sizeof(LONG_NAME_CUT) == areg::ITEM_NAMES_MAX_LENGTH + 1u, "64 characters and the terminator");
    const String name{ LONG_NAME_CUT };
    EXPECT_EQ(areg::crc32_calculate(LONG_NAME_CUT), areg::item_name_number(name));
    EXPECT_EQ(areg::crc32_calculate(LONG_NAME_CUT), static_cast<uint32_t>(ThreadAddress(name)));
    EXPECT_EQ(areg::crc32_calculate(LONG_NAME_CUT), static_cast<uint32_t>(ThreadAddress(LONG_NAME_CUT)));
    EXPECT_EQ(name, ComponentAddress(ThreadAddress(String("T")), name).role_name());
}

TEST(NameNumberLengthTest, parsed_long_names_are_cut)
{
    ThreadAddress thread;
    thread.from_string(LONG_NAME);
    EXPECT_EQ(String(LONG_NAME_CUT), thread.name());
    EXPECT_EQ(areg::crc32_calculate(LONG_NAME_CUT), static_cast<uint32_t>(thread));

    const ServiceAddress shortRole(String(SERVICE_NAME), Version(1u, 0u, 0u), ServiceType::Local, String(LONG_NAME_CUT));
    const String path{ shortRole.to_string() + String(&LONG_NAME[areg::ITEM_NAMES_MAX_LENGTH]) };
    ServiceAddress parsed;
    parsed.from_string(path.as_string());
    EXPECT_EQ(String(LONG_NAME_CUT), parsed.role_name());
    EXPECT_EQ(shortRole, parsed);

    const ServiceItem shortItem(String(LONG_NAME_CUT), Version(1u, 0u, 0u), ServiceType::Local);
    const String itemPath{ String(LONG_NAME) + shortItem.to_string().substring(static_cast<areg::CharPos>(areg::ITEM_NAMES_MAX_LENGTH)) };
    ServiceItem parsedItem;
    parsedItem.from_string(itemPath.as_string());
    EXPECT_EQ(String(LONG_NAME_CUT), parsedItem.service_name());
    EXPECT_EQ(static_cast<uint32_t>(shortItem), static_cast<uint32_t>(parsedItem));
}

#if defined(DEBUG) && GTEST_HAS_DEATH_TEST

TEST(NameNumberLengthDeathTest, long_name_asserts_in_debug)
{
    ::testing::GTEST_FLAG(death_test_style) = "threadsafe";
    const String name{ LONG_NAME };
    EXPECT_DEATH(static_cast<void>(areg::item_name_number(name)), "");
    EXPECT_DEATH(static_cast<void>(ThreadAddress(name)), "");
    EXPECT_DEATH(static_cast<void>(ThreadAddress(LONG_NAME)), "");
    EXPECT_DEATH(static_cast<void>(ComponentAddress(ThreadAddress(String("T")), name)), "");
    EXPECT_DEATH(static_cast<void>(ServiceAddress(String(SERVICE_NAME), Version(1u, 0u, 0u), ServiceType::Local, name)), "");
    EXPECT_DEATH(static_cast<void>(ServiceItem(name, Version(1u, 0u, 0u), ServiceType::Local)), "");
    EXPECT_DEATH(static_cast<void>(ServiceRole(name)), "");
    EXPECT_DEATH(static_cast<void>(ComponentEntry(String("T"), name, nullptr, nullptr)), "");
}

#elif !defined(DEBUG)

TEST(NameNumberLengthTest, long_name_gives_the_number_of_its_first_64_characters)
{
    // Reproduces the defect: one long name gave two numbers, depending on the constructor.
    const String name{ LONG_NAME };
    const uint32_t expected{ areg::crc32_calculate(LONG_NAME_CUT) };
    EXPECT_EQ(expected, areg::item_name_number(name));
    EXPECT_EQ(expected, static_cast<uint32_t>(ThreadAddress(name)));
    EXPECT_EQ(expected, static_cast<uint32_t>(ThreadAddress(LONG_NAME)));
    EXPECT_EQ(expected, static_cast<uint32_t>(ServiceRole(name)));
    EXPECT_EQ(expected, static_cast<uint32_t>(ServiceRole(String(LONG_NAME_CUT))));

    const ComponentAddress component(ThreadAddress(String("T")), name);
    EXPECT_EQ(String(LONG_NAME_CUT), component.role_name());
    const ServiceAddress provider(String(SERVICE_NAME), Version(1u, 0u, 0u), ServiceType::Local, component.role_name());
    const ServiceAddress consumer(String(SERVICE_NAME), Version(1u, 0u, 0u), ServiceType::Local, name);
    EXPECT_EQ(provider, consumer);
    EXPECT_EQ(provider.name_check(), consumer.name_check());

    const ServiceItem item(name, Version(1u, 0u, 0u), ServiceType::Local);
    EXPECT_EQ(String(LONG_NAME_CUT), item.service_name());
    EXPECT_EQ(String(LONG_NAME_CUT), ComponentEntry(name, name, nullptr, nullptr).mRoleName);
}

#endif  // defined(DEBUG) && GTEST_HAS_DEATH_TEST

//////////////////////////////////////////////////////////////////////////
// P3: names hashed one after another
//////////////////////////////////////////////////////////////////////////

TEST(NameNumberAddressTest, role_and_thread_split_differently_give_two_numbers)
{
    // Reproduces the defect: role "NcFoo" on thread "Bar909" and role "NcFo" on thread "oBar909" had one number.
    EventTestDispatcher bar{ String("Bar909") };
    EventTestDispatcher obar{ String("oBar909") };
    ASSERT_TRUE(bar.start_ready() && obar.start_ready());

    const StubAddress stubFoo(String(SERVICE_NAME), Version(1u, 0u, 0u), ServiceType::Local, String("NcFoo"), String("Bar909"));
    const StubAddress stubFo(String(SERVICE_NAME), Version(1u, 0u, 0u), ServiceType::Local, String("NcFo"), String("oBar909"));
    ASSERT_TRUE(stubFoo.is_valid() && stubFo.is_valid());
    EXPECT_NE(static_cast<uint32_t>(stubFoo), static_cast<uint32_t>(stubFo));

    const ProxyAddress proxyFoo(String(SERVICE_NAME), Version(1u, 0u, 0u), ServiceType::Local, String("NcFoo"), String("Bar909"));
    const ProxyAddress proxyFo(String(SERVICE_NAME), Version(1u, 0u, 0u), ServiceType::Local, String("NcFo"), String("oBar909"));
    ASSERT_TRUE(proxyFoo.is_valid() && proxyFo.is_valid());
    EXPECT_NE(static_cast<uint32_t>(proxyFoo), static_cast<uint32_t>(proxyFo));

    const ComponentAddress compFoo(ThreadAddress(String("NcFoo")), String("Bar909"));
    const ComponentAddress compFo(ThreadAddress(String("NcFo")), String("oBar909"));
    ASSERT_TRUE(compFoo.is_valid() && compFo.is_valid());
    EXPECT_NE(static_cast<uint32_t>(compFoo), static_cast<uint32_t>(compFo));

    bar.shutdown(areg::WAIT_INFINITE);
    obar.shutdown(areg::WAIT_INFINITE);
}

//////////////////////////////////////////////////////////////////////////
// P4: the name check of a service address
//////////////////////////////////////////////////////////////////////////

TEST(NameNumberCheckTest, two_roles_with_one_number_have_two_name_checks)
{
    const ServiceAddress first{ service_address(ROLE_FIRST) };
    const ServiceAddress second{ service_address(ROLE_SECOND) };
    const ServiceAddress same{ service_address(ROLE_FIRST) };
    ASSERT_EQ(static_cast<uint32_t>(first), static_cast<uint32_t>(second));
    EXPECT_EQ(first.name_check(), expected_name_check(SERVICE_NAME, ServiceType::Public, ROLE_FIRST));
    EXPECT_EQ(second.name_check(), expected_name_check(SERVICE_NAME, ServiceType::Public, ROLE_SECOND));
    EXPECT_NE(first.name_check(), second.name_check());
    EXPECT_TRUE(first.has_name_conflict(second));
    EXPECT_TRUE(second.has_name_conflict(first));
    EXPECT_EQ(first.name_check(), same.name_check());
    EXPECT_FALSE(first.has_name_conflict(same));
}

TEST(NameNumberCheckTest, unknown_name_check_is_no_conflict)
{
    ServiceAddress numeric(0x4321u, Version(1u, 0u, 0u), ServiceType::Public, 0x1234u);
    const ServiceAddress named{ service_address(ROLE_FIRST) };
    EXPECT_EQ(ServiceAddress::NAME_CHECK_UNKNOWN, numeric.name_check());
    EXPECT_FALSE(numeric.has_name_conflict(named));
    EXPECT_FALSE(named.has_name_conflict(numeric));

    numeric.set_name_check(named.name_check());
    EXPECT_FALSE(numeric.has_name_conflict(named));
    numeric.set_name_check(named.name_check() + 1u);
    EXPECT_TRUE(numeric.has_name_conflict(named));
}

TEST(NameNumberCheckTest, name_check_needs_every_name_part)
{
    EXPECT_EQ(ServiceAddress::NAME_CHECK_UNKNOWN, ServiceAddress().name_check());
    EXPECT_EQ(ServiceAddress::NAME_CHECK_UNKNOWN, service_address(ROLE_FIRST, ServiceType::Invalid).name_check());
    EXPECT_EQ(ServiceAddress::NAME_CHECK_UNKNOWN, ServiceAddress(String(), Version(1u, 0u, 0u), ServiceType::Public, String(ROLE_FIRST)).name_check());
    EXPECT_EQ(ServiceAddress::NAME_CHECK_UNKNOWN, ServiceAddress(String(SERVICE_NAME), Version(1u, 0u, 0u), ServiceType::Public, String()).name_check());
    EXPECT_EQ(ServiceAddress::NAME_CHECK_UNKNOWN, ServiceAddress(String("INVALID_SERVICE"), Version(1u, 0u, 0u), ServiceType::Public, String(ROLE_FIRST)).name_check());

    const ServiceItem item(String(SERVICE_NAME), Version(1u, 0u, 0u), ServiceType::Local);
    EXPECT_EQ(expected_name_check(SERVICE_NAME, ServiceType::Local, ROLE_FIRST), ServiceAddress(item, String(ROLE_FIRST)).name_check());
    EXPECT_EQ(ServiceAddress::NAME_CHECK_UNKNOWN, ServiceAddress(item, String("#77")).name_check());
    EXPECT_EQ(ServiceAddress::NAME_CHECK_UNKNOWN, ServiceAddress(item, 77u).name_check());
}

TEST(NameNumberCheckTest, name_check_follows_copy_move_and_role)
{
    const ServiceAddress source{ service_address(ROLE_FIRST) };
    const uint32_t check{ source.name_check() };

    ServiceAddress copied(source);
    ServiceAddress assigned;
    assigned = source;
    EXPECT_EQ(check, copied.name_check());
    EXPECT_EQ(check, assigned.name_check());

    ServiceAddress moved(std::move(copied));
    EXPECT_EQ(check, moved.name_check());
    EXPECT_EQ(ServiceAddress::NAME_CHECK_UNKNOWN, copied.name_check());

    ServiceAddress moveAssigned;
    moveAssigned = std::move(assigned);
    EXPECT_EQ(check, moveAssigned.name_check());
    EXPECT_EQ(ServiceAddress::NAME_CHECK_UNKNOWN, assigned.name_check());

    moveAssigned.set_role_name(String(ROLE_SECOND));
    EXPECT_EQ(expected_name_check(SERVICE_NAME, ServiceType::Public, ROLE_SECOND), moveAssigned.name_check());
    moveAssigned.set_role_name(String("#77"));
    EXPECT_EQ(ServiceAddress::NAME_CHECK_UNKNOWN, moveAssigned.name_check());
}

TEST(NameNumberCheckTest, name_check_travels_in_streams_and_not_in_endpoints)
{
    const ServiceAddress source{ service_address(ROLE_FIRST) };

    SharedBuffer buffer;
    buffer << source;
    buffer.reset();
    ServiceAddress streamed;
    buffer >> streamed;
    EXPECT_EQ(source, streamed);
    EXPECT_TRUE(streamed.role_name().is_empty());
    EXPECT_EQ(source.name_check(), streamed.name_check());

    RawService raw{ };
    Endpoint endpoint{ };
    source.to_endpoint(raw, endpoint);
    ServiceAddress fromEndpoint{ source };
    fromEndpoint.from_endpoint(raw, endpoint);
    EXPECT_EQ(source, fromEndpoint);
    EXPECT_EQ(ServiceAddress::NAME_CHECK_UNKNOWN, fromEndpoint.name_check());
    EXPECT_EQ(ServiceAddress::NAME_CHECK_UNKNOWN, ServiceAddress(raw, endpoint).name_check());

    ServiceAddress parsed;
    parsed.from_string(source.to_string().as_string());
    EXPECT_EQ(source.name_check(), parsed.name_check());
}

TEST(NameNumberCheckTest, provider_of_another_role_with_one_number_is_not_compatible)
{
    // Reproduces the defect: a consumer of another role name with the provider's number was compatible.
    EventTestDispatcher thread{ String("NcPairingThread") };
    ASSERT_TRUE(thread.start_ready());
    const StubAddress provider(String(SERVICE_NAME), Version(1u, 0u, 0u), ServiceType::Local, String(ROLE_FIRST), String("NcPairingThread"));
    const ProxyAddress right(String(SERVICE_NAME), Version(1u, 0u, 0u), ServiceType::Local, String(ROLE_FIRST), String("NcPairingThread"));
    const ProxyAddress other(String(SERVICE_NAME), Version(1u, 0u, 0u), ServiceType::Local, String(ROLE_SECOND), String("NcPairingThread"));
    ASSERT_EQ(static_cast<const ServiceAddress &>(right), static_cast<const ServiceAddress &>(other));

    EXPECT_TRUE(provider.is_proxy_compatible(right));
    EXPECT_TRUE(right.is_provider_compatible(provider));
    EXPECT_FALSE(provider.is_proxy_compatible(other));
    EXPECT_FALSE(other.is_provider_compatible(provider));

    // Copies keep the check of the address they are made of.
    EXPECT_EQ(provider.name_check(), ServiceAddress(provider).name_check());
    EXPECT_EQ(other.name_check(), ServiceAddress(other).name_check());
    thread.shutdown(areg::WAIT_INFINITE);
}

//////////////////////////////////////////////////////////////////////////
// P4: the router registry and the wire
//////////////////////////////////////////////////////////////////////////

namespace
{
    constexpr uint32_t CHECK_FIRST  { 0x0A0B0C0Du };
    constexpr uint32_t CHECK_OTHER  { 0x0E0F1011u };

    StubAddress numeric_provider(uint32_t nameCheck, ITEM_ID source = 0x100u)
    {
        StubAddress address(0x4321u, Version(1u, 2u, 3u), ServiceType::Public, 0x1234u, 0x1111u, 0x2222u);
        address.set_cookie(source);
        address.set_source(source);
        address.set_name_check(nameCheck);
        return address;
    }

    ProxyAddress numeric_consumer(uint32_t nameCheck, uint32_t thread, const Version & version = Version(1u, 2u, 3u))
    {
        ProxyAddress address(0x4321u, version, ServiceType::Public, 0x1234u, thread, 0x4444u);
        address.set_cookie(0x101u);
        address.set_source(0x101u);
        address.set_name_check(nameCheck);
        return address;
    }

    void router_consumer_of_another_name(bool providerFirst)
    {
        ServiceRegistry registry;
        ListServiceProxies proxies;
        ServiceProxy proxy;
        const StubAddress provider{ numeric_provider(CHECK_FIRST) };
        const ProxyAddress other{ numeric_consumer(CHECK_OTHER, 0x3333u) };
        const ProxyAddress right{ numeric_consumer(CHECK_FIRST, 0x3334u) };
        const ProxyAddress unknown{ numeric_consumer(ServiceAddress::NAME_CHECK_UNKNOWN, 0x3335u) };
        const ProxyAddress newer{ numeric_consumer(CHECK_FIRST, 0x3336u, Version(2u, 0u, 0u)) };
        if (providerFirst)
        {
            static_cast<void>(registry.register_service_provider(provider, proxies));
        }

        static_cast<void>(registry.register_service_proxy(other, proxy));
        static_cast<void>(registry.register_service_proxy(right, proxy));
        static_cast<void>(registry.register_service_proxy(unknown, proxy));
        static_cast<void>(registry.register_service_proxy(newer, proxy));
        if (providerFirst == false)
        {
            static_cast<void>(registry.register_service_provider(provider, proxies));
        }

        EXPECT_EQ(ServiceConnectionState::Connected, registry.proxy_service(right).service_status());
        EXPECT_EQ(ServiceConnectionState::Connected, registry.proxy_service(unknown).service_status());
        EXPECT_NE(ServiceConnectionState::Connected, registry.proxy_service(other).service_status());
        EXPECT_NE(ServiceConnectionState::Connected, registry.proxy_service(newer).service_status());
    }
}

TEST(NameNumberRouterTest, consumer_of_another_name_check_does_not_connect_provider_first)
{
    // Reproduces the defect: the router connected a consumer of another role name with the provider's number.
    router_consumer_of_another_name(true);
}

TEST(NameNumberRouterTest, waiting_consumer_of_another_name_check_or_version_stays_waiting)
{
    // Reproduces the defect: a waiting consumer of another role name or version was connected when the provider came.
    router_consumer_of_another_name(false);
}

TEST(NameNumberWireTest, registration_messages_carry_the_name_check)
{
    const StubAddress provider{ numeric_provider(CHECK_FIRST) };
    const ProxyAddress consumer{ numeric_consumer(CHECK_OTHER, 0x3333u) };
    const ConnectedInstance instance{ };
    struct Case
    {
        MessageEnvelope message;
        uint32_t        expected;
    };

    Case cases[]
    {
          { router_register_service(provider, 0x100u, COOKIE_ROUTER), CHECK_FIRST }
        , { router_unregister_service(provider, DisconnectReason::ProviderDisconnected, 0x100u, COOKIE_ROUTER), CHECK_FIRST }
        , { router_register_consumer(consumer, 0x101u, COOKIE_ROUTER), CHECK_OTHER }
        , { router_unregister_consumer(consumer, DisconnectReason::ConsumerDisconnected, 0x101u, COOKIE_ROUTER), CHECK_OTHER }
        , { service_registered_event(provider, COOKIE_ROUTER, 0x101u), CHECK_FIRST }
        , { service_unregistered_event(provider, DisconnectReason::ProviderDisconnected, COOKIE_ROUTER, 0x101u), CHECK_FIRST }
        , { client_registered_event(consumer, COOKIE_ROUTER, 0x100u), CHECK_OTHER }
        , { client_unregistered_event(consumer, DisconnectReason::ConsumerDisconnected, COOKIE_ROUTER, 0x100u), CHECK_OTHER }
        , { service_duplicate_event(provider, instance, COOKIE_ROUTER, 0x101u), CHECK_FIRST }
    };

    for (Case & entry : cases)
    {
        ASSERT_TRUE(entry.message.is_valid());
        EXPECT_EQ(entry.expected, entry.message.header()->reserved);
    }
}

//////////////////////////////////////////////////////////////////////////
// P2: threads
//////////////////////////////////////////////////////////////////////////

TEST(NameNumberThreadTest, thread_of_another_name_with_the_same_number_is_refused)
{
    // Reproduces the defect: the second thread replaced the first in the registry of names.
    EventTestDispatcher first{ String(NAME_FIRST) };
    EventTestDispatcher second{ String(NAME_SECOND) };
    ASSERT_TRUE(first.start_ready());
    EXPECT_FALSE(second.start(EVENT_TEST_WAIT_MS));
    EXPECT_EQ(static_cast<Thread *>(&first), Thread::find_by_address(ThreadAddress(String(NAME_FIRST))));

    first.shutdown(areg::WAIT_INFINITE);
    ASSERT_TRUE(second.start_ready());
    EXPECT_EQ(static_cast<Thread *>(&second), Thread::find_by_address(ThreadAddress(String(NAME_SECOND))));
    second.shutdown(areg::WAIT_INFINITE);
}

TEST(NameNumberThreadTest, thread_name_with_a_reserved_number_is_refused)
{
    EventTestDispatcher zero{ String(NAME_ZERO) };
    EventTestDispatcher allOnes{ String(NAME_ALL_ONES) };
    ASSERT_EQ(areg::CHECKSUM_IGNORE, static_cast<uint32_t>(zero.address()));
    ASSERT_EQ(areg::CHECKSUM_INVALID, static_cast<uint32_t>(allOnes.address()));
    EXPECT_FALSE(zero.start(EVENT_TEST_WAIT_MS));
    EXPECT_FALSE(allOnes.start(EVENT_TEST_WAIT_MS));
}

TEST(NameNumberThreadTest, stopping_one_of_two_threads_of_one_name_keeps_the_other)
{
    // Reproduces the defect: the first thread removed the entry of the second when it stopped.
    EventTestDispatcher first{ String("NcSameThreadName") };
    EventTestDispatcher second{ String("NcSameThreadName") };
    ASSERT_TRUE(first.start_ready());
    ASSERT_TRUE(second.start_ready());

    first.shutdown(areg::WAIT_INFINITE);
    EXPECT_EQ(static_cast<Thread *>(&second), Thread::find_by_address(ThreadAddress(String("NcSameThreadName"))));
    second.shutdown(areg::WAIT_INFINITE);
    EXPECT_EQ(nullptr, Thread::find_by_address(ThreadAddress(String("NcSameThreadName"))));
}

//////////////////////////////////////////////////////////////////////////
// P2: components, providers, proxies and local pairing
//////////////////////////////////////////////////////////////////////////

namespace
{
    constexpr uint32_t REQUESTS[]   { REQUEST_ID_FIRST };
    constexpr uint32_t RESPONSES[]  { RESPONSE_ID_FIRST };
    constexpr uint32_t ATTRIBUTES[] { ATTRIBUTE_ID_FIRST };
    constexpr uint32_t PARAMS[]     { 0 };
    const InterfaceData NC_INTERFACE
    {
          SERVICE_NAME, Version(1, 0, 0), ServiceType::Local
        , 1, 1, 1, REQUESTS, RESPONSES, ATTRIBUTES, RESPONSES, PARAMS
    };

    //!< The connection states seen by the consumers, by consumer role name.
    class NcObservations
    {
    public:
        void record(const String & consumer, bool connected)
        {
            {
                std::lock_guard<std::mutex> lock(mLock);
                std::pair<int32_t, int32_t> & entry{ mRows[std::string(consumer.as_string())] };
                (connected ? entry.first : entry.second) += 1;
            }
            mChanged.notify_all();
        }

        bool wait_connected(const char * consumer)
        {
            std::unique_lock<std::mutex> lock(mLock);
            return mChanged.wait_for(lock, EVENT_TEST_WAIT, [this, consumer]() { return mRows[consumer].first != 0; });
        }

        int32_t connected(const char * consumer)
        {
            std::lock_guard<std::mutex> lock(mLock);
            return mRows[consumer].first;
        }

        int32_t disconnected(const char * consumer)
        {
            std::lock_guard<std::mutex> lock(mLock);
            return mRows[consumer].second;
        }

    private:
        std::mutex                                              mLock;
        std::condition_variable                                 mChanged;
        std::map<std::string, std::pair<int32_t, int32_t>>      mRows;
    };

    //!< The setup of a component: the role names its proxies consume and where it records.
    struct NcSetup
    {
        std::vector<std::string>    consumes;
        NcObservations *            observations;
        std::vector<bool> *         acquired;
    };

    class NcProxy final : public ProxyBase
    {
    public:
        NcProxy(const String & role, DispatcherThread * owner)
            : ProxyBase(role, NC_INTERFACE, owner)
        {
        }

        static ProxyBase * create(const String & role, DispatcherThread * owner)
        {
            return new NcProxy(role, owner);
        }

    protected:
        void process_response_event(ServiceResponseEvent & /*eventElem*/) override
        {
        }

        void process_custom_event(Event & /*eventElem*/) override
        {
        }

        ServiceAvailableEvent create_service_available(NotificationConsumer & consumer) override
        {
            return ServiceAvailableEvent(consumer);
        }

        NotificationEvent create_client_notification(const NotificationEventData & data) const override
        {
            return NotificationEvent(data);
        }

        ServiceRequestEvent create_request(const SharedBuffer & args, uint32_t reqId) override
        {
            return LocalRequestEvent(args, proxy_address(), stub_address(), reqId);
        }

        ServiceRequestEvent create_request(uint32_t reqId, uint32_t reserve) override
        {
            return LocalRequestEvent(proxy_address(), stub_address(), reqId, reserve);
        }

        ServiceRequestEvent create_notification_request(uint32_t msgId, RequestType reqType) override
        {
            return LocalNotifyRequestEvent(proxy_address(), stub_address(), msgId, reqType);
        }

        ServiceResponseEvent create_request_failed(const ProxyAddress & addrProxy, uint32_t msgId, ResultType reason, const SequenceNumber & seqNr) const override
        {
            return LocalResponseEvent(addrProxy, reason, msgId, seqNr);
        }
    };

    class NcComponent;

    //!< The test components by role name.
    std::mutex                              _componentsLock;
    std::map<std::string, NcComponent *>    _components;

    //!< A component that runs actions in its thread.
    class NcComponent : public Component, public LifecycleActionConsumer
    {
    public:
        NcComponent(const ComponentEntry & entry, ComponentThread & owner)
            : Component(entry, owner)
            , LifecycleActionConsumer()
        {
            std::lock_guard<std::mutex> lock(_componentsLock);
            _components[std::string(entry.mRoleName.as_string())] = this;
        }

        ~NcComponent() override
        {
            std::lock_guard<std::mutex> lock(_componentsLock);
            _components.erase(std::string(role_name().as_string()));
        }

        void startup_component(ComponentThread & owner) override
        {
            Component::startup_component(owner);
            LifecycleActionEvent::add_listener(static_cast<LifecycleActionConsumer &>(*this), owner);
        }

        void shutdown_component(ComponentThread & owner) override
        {
            LifecycleActionEvent::remove_listener(static_cast<LifecycleActionConsumer &>(*this), owner);
            Component::shutdown_component(owner);
        }

    protected:
        void process_event(const LifecycleActionData & data) override
        {
            data.action();
        }
    };

    class NcProvider final : public NcComponent, public StubBase
    {
    public:
        NcProvider(const ComponentEntry & entry, ComponentThread & owner)
            : NcComponent(entry, owner)
            , StubBase(static_cast<Component &>(*this), NC_INTERFACE)
        {
        }

        void send_notification(uint32_t /*msgId*/) override
        {
        }

        void error_request(uint32_t /*msgId*/, bool /*msgCancel*/) override
        {
        }

    protected:
        void process_request_event(ServiceRequestEvent & /*eventElem*/) override
        {
        }

        void process_attribute_event(ServiceRequestEvent & /*eventElem*/) override
        {
        }
    };

    class NcConsumer final : public NcComponent, public ProxyListener
    {
    public:
        NcConsumer(const ComponentEntry & entry, ComponentThread & owner)
            : NcComponent(entry, owner)
            , ProxyListener()
            , mSetup(*std::any_cast<NcSetup *>(entry.data()))
            , mProxies()
        {
            for (const std::string & role : mSetup.consumes)
            {
                std::shared_ptr<ProxyBase> proxy{ ProxyBase::acquire_proxy(String(role.c_str()), NC_INTERFACE, *this, NcProxy::create, owner) };
                if (mSetup.acquired != nullptr)
                {
                    mSetup.acquired->push_back(proxy.get() != nullptr);
                }

                if (proxy.get() != nullptr)
                {
                    mProxies.push_back(proxy);
                }
            }
        }

        ~NcConsumer() override
        {
            for (std::shared_ptr<ProxyBase> & proxy : mProxies)
            {
                proxy->free_proxy(*this);
            }
        }

        bool service_connected(ServiceConnectionState status, ProxyBase & /*proxy*/) override
        {
            mSetup.observations->record(role_name(), is_service_connected(status));
            return true;
        }

        void process_notification_event(NotificationEvent & /*eventElem*/) override
        {
        }

        void process_notification(uint32_t /*msgId*/, ResultType /*result*/, SequenceNumber /*seqNr*/) override
        {
        }

    private:
        NcSetup &                                   mSetup;
        std::vector<std::shared_ptr<ProxyBase>>     mProxies;
    };

    template<typename ComponentType>
    Component * create_component(const ComponentEntry & entry, ComponentThread & owner)
    {
        return new ComponentType(entry, owner);
    }

    void delete_component(Component & component, const ComponentEntry & /*entry*/)
    {
        delete &component;
    }

    //!< A model of one thread with the components of the given roles.
    template<typename ComponentType>
    Model nc_model(const char * name, const char * thread, std::initializer_list<const char *> roles, NcSetup * setup = nullptr)
    {
        ComponentThreadEntry owner{ String(thread) };
        for (const char * role : roles)
        {
            ComponentEntry entry(String(thread), String(role), create_component<ComponentType>, delete_component);
            if (setup != nullptr)
            {
                entry.set_data(setup);
            }

            owner.add_component(entry);
        }

        Model model{ String(name) };
        model.add_thread(owner);
        return model;
    }

    //!< Runs the action in the thread of the component and waits until it ran.
    bool run_in(const char * role, std::function<void()> action = []() {})
    {
        NcComponent * target{ nullptr };
        {
            std::lock_guard<std::mutex> lock(_componentsLock);
            const auto pos{ _components.find(role) };
            target = (pos != _components.end() ? pos->second : nullptr);
        }

        if (target == nullptr)
            return false;

        const auto done{ std::make_shared<std::promise<void>>() };
        std::future<void> ran{ done->get_future() };
        if (LifecycleActionEvent::send_event(LifecycleActionData{ [act = std::move(action), done]() { act(); done->set_value(); } }
                                            , static_cast<LifecycleActionConsumer &>(*target), target->master_thread()) == false)
            return false;

        return (ran.wait_for(EVENT_TEST_WAIT) == std::future_status::ready);
    }

    //!< Sets up the application for one test and removes its models at the end.
    class NcApplication
    {
    public:
        NcApplication()
            : mModels()
        {
            Application::setup(false, true, false, true, false);
        }

        ~NcApplication()
        {
            for (auto it = mModels.rbegin(); it != mModels.rend(); ++ it)
            {
                ComponentLoader::unload_component_model(true, String(it->c_str()));
            }

            for (const std::string & name : mModels)
            {
                ComponentLoader::remove_component_model(String(name.c_str()));
            }

            Application::release();
        }

        bool add(const Model & model)
        {
            mModels.push_back(std::string(model.model_name().as_string()));
            return ComponentLoader::add_model_unique(model);
        }

        bool load(const Model & model)
        {
            return add(model) && ComponentLoader::load_component_model(model.model_name(), true);
        }

        void unload(const char * name)
        {
            ComponentLoader::unload_component_model(true, String(name));
        }

    private:
        std::vector<std::string>    mModels;
    };
}

TEST(NameNumberComponentTest, component_of_another_role_with_the_same_number_is_refused)
{
    // Reproduces the defect: the second component replaced the first and was found by the first name.
    NcApplication app;
    ASSERT_TRUE(app.load(nc_model<NcComponent>("NcModelFirst", "NcComponentThreadFirst", { NAME_FIRST })));
    Component * first{ Component::find_by_name(String(NAME_FIRST)) };
    ASSERT_NE(nullptr, first);

    ASSERT_TRUE(app.add(nc_model<NcComponent>("NcModelSecond", "NcComponentThreadSecond", { NAME_SECOND })));
    static_cast<void>(ComponentLoader::load_component_model(String("NcModelSecond"), true));
    EXPECT_EQ(first, Component::find_by_name(String(NAME_FIRST)));
    EXPECT_EQ(nullptr, Component::find_by_name(String(NAME_SECOND)));
    EXPECT_TRUE(Component::exist(String(NAME_FIRST)));
    EXPECT_FALSE(Component::exist(String(NAME_SECOND)));

    {
        const Component direct(String(NAME_SECOND), first->master_thread());
        EXPECT_EQ(first, Component::find_by_name(String(NAME_FIRST)));
        EXPECT_EQ(nullptr, Component::find_by_name(String(NAME_SECOND)));
    }

    EXPECT_EQ(first, Component::find_by_name(String(NAME_FIRST)));
    EXPECT_EQ(nullptr, Component::find_by_name(String()));

    // A component made in the thread of the first, by role name only.
    ASSERT_TRUE(run_in(NAME_FIRST, [first]()
        {
            const Component current{ String(NAME_SECOND) };
            EXPECT_EQ(first, Component::find_by_name(String(NAME_FIRST)));
            EXPECT_EQ(nullptr, Component::find_by_name(String(NAME_SECOND)));
        }));
    EXPECT_EQ(first, Component::find_by_name(String(NAME_FIRST)));
}

TEST(NameNumberComponentTest, proxy_of_another_role_in_one_thread_is_refused)
{
    // Reproduces the defect: the second proxy of one number was the proxy of the first role name.
    NcObservations observations;
    std::vector<bool> acquired;
    NcSetup setup{ { ROLE_FIRST, ROLE_SECOND }, &observations, &acquired };
    NcApplication app;
    ASSERT_TRUE(app.load(nc_model<NcProvider>("NcProvider", "NcProviderThread", { ROLE_FIRST })));
    ASSERT_TRUE(app.load(nc_model<NcConsumer>("NcConsumers", "NcConsumerThread", { "NcConsumer" }, &setup)));

    EXPECT_TRUE(observations.wait_connected("NcConsumer"));
    ASSERT_TRUE(run_in("NcConsumer"));
    ASSERT_EQ(2u, acquired.size());
    EXPECT_TRUE(acquired[0]);
    EXPECT_FALSE(acquired[1]);
    EXPECT_EQ(1, observations.connected("NcConsumer"));
}

namespace
{
    //!< A consumer of the other role and one of the provider's role, in two threads, in this order.
    void local_consumer_of_another_role(bool providerFirst)
    {
        NcObservations observations;
        NcSetup other{ { ROLE_SECOND }, &observations, nullptr };
        NcSetup right{ { ROLE_FIRST }, &observations, nullptr };
        NcApplication app;
        if (providerFirst)
        {
            ASSERT_TRUE(app.load(nc_model<NcProvider>("NcProvider", "NcProviderThread", { ROLE_FIRST })));
        }

        // A component thread registers its proxies before it handles an event.
        ASSERT_TRUE(app.load(nc_model<NcConsumer>("NcOther", "NcOtherThread", { "NcOther" }, &other)));
        ASSERT_TRUE(run_in("NcOther"));
        ASSERT_TRUE(app.load(nc_model<NcConsumer>("NcRight", "NcRightThread", { "NcRight" }, &right)));
        ASSERT_TRUE(run_in("NcRight"));
        if (providerFirst == false)
        {
            ASSERT_TRUE(app.load(nc_model<NcProvider>("NcProvider", "NcProviderThread", { ROLE_FIRST })));
        }

        ASSERT_TRUE(observations.wait_connected("NcRight"));
        ASSERT_TRUE(run_in("NcOther"));
        EXPECT_EQ(0, observations.connected("NcOther"));
    }
}

TEST(NameNumberLocalTest, consumer_of_another_role_does_not_connect_to_the_provider)
{
    // Reproduces the defect: the consumer of another role name with the provider's number connected to it.
    local_consumer_of_another_role(true);
}

TEST(NameNumberLocalTest, waiting_consumer_of_another_role_stays_waiting)
{
    // Reproduces the defect: the waiting consumer of another role name connected when the provider came.
    local_consumer_of_another_role(false);
}

TEST(NameNumberLocalTest, provider_of_another_role_neither_replaces_nor_removes_the_first)
{
    // Reproduces the defect: the second provider took the place of the first, and removed it when it left.
    NcObservations observations;
    NcSetup right{ { ROLE_FIRST }, &observations, nullptr };
    NcSetup other{ { ROLE_SECOND }, &observations, nullptr };
    NcSetup later{ { ROLE_FIRST }, &observations, nullptr };
    NcApplication app;
    ASSERT_TRUE(app.load(nc_model<NcProvider>("NcProvider", "NcProviderThread", { ROLE_FIRST })));
    ASSERT_TRUE(app.load(nc_model<NcConsumer>("NcRight", "NcRightThread", { "NcRight" }, &right)));
    ASSERT_TRUE(observations.wait_connected("NcRight"));

    // The second provider is made in the thread of the first: the component registry refuses its role name first.
    Component * first{ Component::find_by_name(String(ROLE_FIRST)) };
    ASSERT_NE(nullptr, first);
    const ComponentEntry entry(String("NcProviderThread"), String(ROLE_SECOND), create_component<NcProvider>, delete_component);
    NcProvider * second{ nullptr };
    ASSERT_TRUE(run_in(ROLE_FIRST, [&]()
        {
            second = new NcProvider(entry, first->master_thread());
            second->startup_component(first->master_thread());
        }));
    ASSERT_NE(nullptr, second);
    const StubAddress secondAddress(NC_INTERFACE, String(ROLE_SECOND), String("NcProviderThread"));
    EXPECT_EQ(String(ROLE_FIRST), StubBase::registered_address(secondAddress).role_name());
    EXPECT_EQ(first, Component::find_by_name(String(ROLE_FIRST)));

    ASSERT_TRUE(app.load(nc_model<NcConsumer>("NcOther", "NcOtherThread", { "NcOther" }, &other)));
    ASSERT_TRUE(run_in("NcOther"));
    ASSERT_TRUE(run_in(ROLE_FIRST, [&]()
        {
            second->shutdown_component(first->master_thread());
            delete second;
        }));
    EXPECT_EQ(String(ROLE_FIRST), StubBase::registered_address(secondAddress).role_name());

    // The later consumer connects after the service manager handled the leave of the second provider.
    ASSERT_TRUE(app.load(nc_model<NcConsumer>("NcLater", "NcLaterThread", { "NcLater" }, &later)));
    ASSERT_TRUE(observations.wait_connected("NcLater"));
    ASSERT_TRUE(run_in("NcRight"));
    ASSERT_TRUE(run_in("NcOther"));
    EXPECT_EQ(1, observations.connected("NcRight"));
    EXPECT_EQ(0, observations.disconnected("NcRight"));
    EXPECT_EQ(0, observations.connected("NcOther"));
}

#if !defined(DEBUG)

TEST(NameNumberLengthTest, provider_and_consumer_of_a_long_role_connect)
{
    // Reproduces the defect: the provider used the first 64 characters of its role, the consumer all of them.
    NcObservations observations;
    NcSetup consumer{ { LONG_NAME }, &observations, nullptr };
    NcApplication app;
    ASSERT_TRUE(app.load(nc_model<NcProvider>("NcLongProvider", "NcLongProviderThread", { LONG_NAME })));
    ASSERT_TRUE(app.load(nc_model<NcConsumer>("NcLongConsumer", "NcLongConsumerThread", { "NcLongConsumer" }, &consumer)));
    EXPECT_TRUE(observations.wait_connected("NcLongConsumer"));
    EXPECT_NE(nullptr, Component::find_by_name(String(LONG_NAME)));
    EXPECT_NE(nullptr, Component::find_by_name(String(LONG_NAME_CUT)));
}

#if AREG_LOGGING

TEST(NameNumberScopeTest, leaving_scope_keeps_the_entry_of_another_scope_of_its_number)
{
    // Reproduces the defect: a scope that left removed the entry of the other scope of its number.
    ScopeController controller;
    LogScope * first{ new LogScope("nc_scope.same_number") };
    LogScope second("nc_scope.same_number");
    controller.register_scope(*first);
    controller.register_scope(second);
    controller.unregister_scope(*first);
    delete first;
    EXPECT_TRUE(controller.is_scope_registered(static_cast<uint32_t>(second)));
    controller.unregister_scope(second);
    EXPECT_FALSE(controller.is_scope_registered(static_cast<uint32_t>(second)));
}

#endif  // AREG_LOGGING

#endif  // !defined(DEBUG)
