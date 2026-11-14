/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Copyright (c) 2017-2026 Aregtech (Artak Avetyan).
 * Areg SDK is dual-licensed under Apache 2.0 and commercial licenses.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "units/ChildProcess.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/base/SyncPrimitives.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/DispatcherThread.hpp"
#include "areg/component/Model.hpp"
#include "areg/component/ProxyBase.hpp"
#include "areg/component/ProxyListener.hpp"
#include "areg/component/RequestEvents.hpp"
#include "areg/component/ResponseEvents.hpp"
#include "areg/component/StubBase.hpp"
#include "areg/component/private/ServiceManagerEvents.hpp"

#if defined(AREG_TEST_PRIVATE_POSIX)
    #include "areg/component/private/ServiceManagerEventProcessor.hpp"
    #include "areg/ipc/ConnectionProvider.hpp"
    #include "areg/ipc/RegistrationProvider.hpp"
#endif  // defined(AREG_TEST_PRIVATE_POSIX)

#include <atomic>
#include <chrono>
#include <cstdio>
#include <functional>
#include <future>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace
{
    using namespace areg;

    constexpr uint32_t WAIT_MS{ 30'000u };

    constexpr uint32_t REQUESTS[]{ REQUEST_ID_FIRST };
    constexpr uint32_t RESPONSES[]{ RESPONSE_ID_FIRST };
    constexpr uint32_t ATTRIBUTES[]{ ATTRIBUTE_ID_FIRST };
    constexpr uint32_t PARAM_COUNTS[]{ 1 };
    const InterfaceData PUBLIC_INTERFACE
    {
          "CoreCoverageService", Version(1, 0, 0), ServiceType::Public
        , 1, 1, 1, REQUESTS, RESPONSES, ATTRIBUTES, RESPONSES, PARAM_COUNTS
    };

    //!< A consumer that ignores every notification.
    class CoreListener final : public ProxyListener
    {
    public:
        bool service_connected(ServiceConnectionState, ProxyBase &) override
        {
            return true;
        }

        void process_notification_event(NotificationEvent &) override
        {
        }

        void process_notification(uint32_t, ResultType, SequenceNumber) override
        {
        }
    };

    //!< A proxy of the public interface that sends nothing.
    class CoreProxy final : public ProxyBase
    {
    public:
        CoreProxy(const String & role, DispatcherThread * owner)
            : ProxyBase(role, PUBLIC_INTERFACE, owner)
        {
        }

        static ProxyBase * create(const String & role, DispatcherThread * owner)
        {
            return new CoreProxy(role, owner);
        }

    protected:
        void process_response_event(ServiceResponseEvent &) override
        {
        }

        void process_custom_event(Event &) override
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

        ServiceRequestEvent create_request(const SharedBuffer & data, uint32_t request) override
        {
            return LocalRequestEvent(data, proxy_address(), stub_address(), request);
        }

        ServiceRequestEvent create_request(uint32_t request, uint32_t reserve) override
        {
            return LocalRequestEvent(proxy_address(), stub_address(), request, reserve);
        }

        ServiceRequestEvent create_notification_request(uint32_t, RequestType) override
        {
            return ServiceRequestEvent(MessageEnvelope{});
        }

        ServiceResponseEvent create_request_failed(const ProxyAddress & target, uint32_t message
                                                  , ResultType reason, const SequenceNumber & sequence) const override
        {
            return LocalResponseEvent(target, reason, message, sequence);
        }
    };

    //!< A stub of the public interface that counts what reaches it.
    class CoreStub final : public StubBase
    {
    public:
        explicit CoreStub(Component & holder)
            : StubBase(holder, PUBLIC_INTERFACE)
        {
        }

        void send_notification(uint32_t) override
        {
        }

        void error_request(uint32_t, bool) override
        {
        }

        //!< Gives the event to the stub as the dispatcher does.
        void deliver(Event & event)
        {
            static_cast<EventConsumer &>(*this).start_event_processing(event);
        }

        uint32_t    mRequests   { 0u };
        uint32_t    mAttributes { 0u };
        uint32_t    mStubEvents { 0u };
        uint32_t    mGenerics   { 0u };
        mutable uint32_t    mUndelivered    { 0u };
        mutable uint32_t    mUndeliveredMsg { 0u };

        //!< Checks the value semantics of the listener entry of the stub.
        static void check_listener_values()
        {
            const ProxyAddress first(1u, Version(1, 0, 0), ServiceType::Public, 2u, 3u, 4u);
            ProxyAddress other(1u, Version(1, 0, 0), ServiceType::Public, 2u, 3u, 9u);
            ProxyAddress foreign(first);
            foreign.set_cookie(first.cookie() + 1u);

            const Listener empty;
            EXPECT_EQ(0u, empty.mMessageId);
            EXPECT_EQ(0u, empty.mFlags);
            EXPECT_EQ(0u, empty.mSequenceNr);

            const Listener source(7u, 9u, first);
            Listener copied;
            copied = source;
            EXPECT_EQ(7u, copied.mMessageId);
            EXPECT_EQ(9u, copied.mSequenceNr);
            EXPECT_TRUE(copied.proxy_matches(first));
            EXPECT_TRUE(copied.same_proxy(source));

            Listener moved;
            moved = Listener(source);
            EXPECT_EQ(7u, moved.mMessageId);
            EXPECT_EQ(9u, moved.mSequenceNr);
            EXPECT_TRUE(moved.proxy_matches(first));

            EXPECT_TRUE(source == copied);
            EXPECT_TRUE(source == source);
            EXPECT_TRUE(Listener(7u, 9u, first) == Listener(7u, SEQUENCE_NUMBER_ANY, other));
            EXPECT_FALSE(Listener(7u, 9u, first) == Listener(7u, 10u, first));
            EXPECT_FALSE(Listener(7u, 9u, first) == Listener(7u, 9u, other));
            EXPECT_FALSE(Listener(7u, 9u, first) == Listener(7u, 9u, foreign));
            EXPECT_FALSE(Listener(7u, 9u, first).same_proxy(Listener(7u, 9u, other)));
            EXPECT_FALSE(Listener(7u, 9u, first).same_proxy(Listener(7u, 9u, foreign)));
        }

    protected:
        void process_request_event(ServiceRequestEvent &) override
        {
            ++ mRequests;
        }

        void process_attribute_event(ServiceRequestEvent &) override
        {
            ++ mAttributes;
        }

        void process_stub_event(StubEvent & event) override
        {
            ++ mStubEvents;
            StubBase::process_stub_event(event);
        }

        void process_generic_event(Event & event) override
        {
            ++ mGenerics;
            StubBase::process_generic_event(event);
        }

        ServiceResponseEvent create_response(const ProxyAddress & target, uint32_t message, ResultType result, const SharedBuffer &) const override
        {
            if (result == ResultType::MessageUndelivered)
            {
                ++ mUndelivered;
                mUndeliveredMsg = message;
            }

            return LocalResponseEvent(target, result, message, 0u);
        }
    };

    //!< A dispatcher thread that runs one action and ends.
    class CoreThread final : public DispatcherThread
    {
    public:
        CoreThread(std::function<void(DispatcherThread &)> action, const char * name)
            : DispatcherThread(String(name), DEFAULT_STACK_SIZE, QUEUE_MIN_RING_CAPACITY, Bool::True, 0)
            , mAction(std::move(action))
            , mDone()
        {
        }

        std::future<void> completion()
        {
            return mDone.get_future();
        }

    protected:
        bool post_event(Event & event) override
        {
            return EventDispatcher::post_event(event);
        }

        void on_run() override
        {
            mAction(*this);
            mDone.set_value();
        }

    private:
        std::function<void(DispatcherThread &)> mAction;
        std::promise<void> mDone;
    };

    void on_core_thread(std::function<void(DispatcherThread &)> action, const char * name = "CoreCoverageThread")
    {
        CoreThread thread(std::move(action), name);
        auto done = thread.completion();
        ASSERT_TRUE(thread.start(WAIT_INFINITE));
        EXPECT_EQ(std::future_status::ready, done.wait_for(std::chrono::seconds(30)));
        EXPECT_EQ(Thread::ThreadCompletion::Completed, thread.shutdown(WAIT_INFINITE));
    }
}

/**
 * \brief   Every command has its own name, and a value that is no command has the error name.
 **/
TEST(ComponentCoreCoverageTest, service_manager_commands_have_distinct_names)
{
    using Command = ServiceManagerEventData::ServiceManagerCommand;
    const Command all[]
    {
          Command::CMD_RegisterProxy, Command::CMD_UnregisterProxy, Command::CMD_RegisterProvider
        , Command::CMD_UnregisterProvider, Command::CMD_StartConnection, Command::CMD_StopConnection
        , Command::CMD_RegisterConnection, Command::CMD_UnregisterConnection, Command::CMD_LostConnection
        , Command::CMD_ShutdownService, Command::CMD_LoadModel, Command::CMD_UnloadModel
        , Command::CMD_ThreadRunning, Command::CMD_ThreadStopped, Command::CMD_WatchdogExpired
        , Command::CMD_DeadlineExpired, Command::CMD_UserState, Command::CMD_RouterClientStopped
        , Command::CMD_Reconcile
    };

    std::vector<std::string> names;
    for (const Command command : all)
    {
        const std::string name{ ServiceManagerEventData::as_string(command) };
        EXPECT_NE(std::string::npos, name.find("ServiceManagerCommand::CMD_")) << name;
        for (const std::string & known : names)
            EXPECT_NE(known, name);

        names.push_back(name);
    }

    EXPECT_NE(std::string::npos, names.back().find("CMD_Reconcile"));
    const volatile uint32_t unknown{ 0xFFFFu };
    const std::string error{ ServiceManagerEventData::as_string(static_cast<Command>(static_cast<uint32_t>(unknown))) };
    EXPECT_EQ(0u, error.find("ERR"));
}

/**
 * \brief   A listener entry copies and moves with its message, sequence and proxy, and compares by
 *          sequence and proxy.
 **/
TEST(ComponentCoreCoverageTest, stub_listener_entries_copy_move_and_compare_by_sequence_and_proxy)
{
    ComponentThread thread(String("CoreCoverageListenerThread"));
    Component holder(String("CoreCoverageListenerRole"), thread);
    {
        CoreStub stub(holder);
        EXPECT_EQ(&stub, StubBase::find_stub(stub.address()));
        CoreStub::check_listener_values();
    }
}

/**
 * \brief   Removing all models leaves no registered model, and the names are free again.
 **/
TEST(ComponentCoreCoverageTest, removing_all_models_empties_the_registry)
{
    AREG_EXPECT_IN_CHILD
    (
        const String first("CoreRemoveAllFirst");
        const String second("CoreRemoveAllSecond");
        const auto build = [](const String & name, const String & thread, const String & role)
        {
            ComponentThreadEntry entry{ thread };
            entry.add_component(ComponentEntry(thread, role
                , [](const ComponentEntry & item, ComponentThread & owner) -> Component * { return new Component(item, owner); }
                , [](Component & item, const ComponentEntry &) { delete &item; }));
            Model model(name);
            model.add_thread(entry);
            return model;
        };

        AREG_CHILD_CHECK(ComponentLoader::add_model_unique(build(first, String("CoreRemoveAllThreadA"), String("CoreRemoveAllRoleA"))));
        AREG_CHILD_CHECK(ComponentLoader::add_model_unique(build(second, String("CoreRemoveAllThreadB"), String("CoreRemoveAllRoleB"))));
        AREG_CHILD_CHECK(ComponentLoader::exist(first) && ComponentLoader::exist(second));

        ComponentLoader::remove_component_model(String());
        AREG_CHILD_CHECK(ComponentLoader::exist(first) == false);
        AREG_CHILD_CHECK(ComponentLoader::exist(second) == false);
        AREG_CHILD_CHECK(ComponentLoader::find_component_entry(String("CoreRemoveAllRoleA")).is_valid() == false);
        AREG_CHILD_CHECK(ComponentLoader::add_model_unique(build(first, String("CoreRemoveAllThreadA"), String("CoreRemoveAllRoleA"))));
        ComponentLoader::remove_component_model(String());
    );
}

namespace
{
    constexpr char      DEADLINE_MODEL[]    { "CoreDeadlineModel" };
    constexpr char      DEADLINE_THREAD[]   { "CoreDeadlineThread" };
    constexpr char      OTHER_MODEL[]       { "CoreDeadlineOtherModel" };
    constexpr char      OTHER_THREAD[]      { "CoreDeadlineOtherThread" };

    //!< Signaled when the component of the deadline model entered its startup.
    SyncEvent           gEntered            { true, false };
    //!< Holds the component of the deadline model in its startup until signaled.
    SyncEvent           gHold               { true, false };

    //!< A component that stays in its startup until the hold is signaled.
    class CoreHeldComponent final : public Component
    {
    public:
        CoreHeldComponent(const ComponentEntry & entry, ComponentThread & owner)
            : Component(entry, owner)
        {
        }

    protected:
        void startup_component(ComponentThread & comThread) override
        {
            gEntered.set_signaled();
            static_cast<void>(gHold.lock(WAIT_INFINITE));
            Component::startup_component(comThread);
        }
    };

    Component * create_held(const ComponentEntry & entry, ComponentThread & owner)
    {
        return new CoreHeldComponent(entry, owner);
    }

    Component * create_plain(const ComponentEntry & entry, ComponentThread & owner)
    {
        return new Component(entry, owner);
    }

    void delete_core(Component & item, const ComponentEntry &)
    {
        delete &item;
    }

    Model deadline_model(const char * model, const char * thread, const char * role, FuncCreateComponent create, uint32_t lifecycleMs)
    {
        ComponentThreadEntry entry{ String(thread) };
        entry.mLifecycleTimeout = lifecycleMs;
        entry.mFailurePolicy = FailurePolicy::Ignore;
        entry.add_component(ComponentEntry(String(thread), String(role), create, &delete_core));
        Model result{ String(model) };
        result.add_thread(entry);
        return result;
    }

    //!< Returns after the monotonic clock passed the time point.
    void wait_for_clock(const std::chrono::steady_clock::time_point & bound)
    {
        SyncEvent never(true, false);
        for (auto now = std::chrono::steady_clock::now(); now < bound; now = std::chrono::steady_clock::now())
        {
            const auto left = std::chrono::duration_cast<std::chrono::milliseconds>(bound - now).count() + 1;
            static_cast<void>(never.lock(static_cast<uint32_t>(left)));
        }
    }
}

/**
 * \brief   A start deadline that passed while the watchdog manager does not run is not enforced: the
 *          starting thread is kept and another model still loads.
 **/
TEST(ComponentCoreCoverageTest, passed_start_deadline_without_the_watchdog_manager_does_not_stop_other_models)
{
    AREG_EXPECT_IN_CHILD
    (
        AREG_CHILD_CHECK(ComponentLoader::add_model_unique(deadline_model(DEADLINE_MODEL, DEADLINE_THREAD, "CoreDeadlineRole", &create_held, 1u)));
        AREG_CHILD_CHECK(ComponentLoader::add_model_unique(deadline_model(OTHER_MODEL, OTHER_THREAD, "CoreDeadlineOtherRole", &create_plain, 0u)));
        Application::setup(false, true, false, true, true);
        Application::stop_watchdog_manager();

        AREG_CHILD_CHECK(Application::load_model(DEADLINE_MODEL, false));
        AREG_CHILD_CHECK(gEntered.lock(WAIT_MS));
        AREG_CHILD_CHECK(Application::thread_state(DEADLINE_THREAD) == ThreadState::Starting);

        wait_for_clock(std::chrono::steady_clock::now() + std::chrono::milliseconds(3));
        AREG_CHILD_CHECK(Application::load_model(OTHER_MODEL, true));
        AREG_CHILD_CHECK(Application::is_model_loaded(OTHER_MODEL));
        AREG_CHILD_CHECK(Application::thread_state(DEADLINE_THREAD) == ThreadState::Starting);

        gHold.set_signaled();
        AREG_CHILD_CHECK(Application::unload_model(nullptr));
        Application::release();
    );
}

#if defined(AREG_TEST_PRIVATE_POSIX)

namespace
{
    enum class Call { RegisterProvider, UnregisterProvider, RegisterConsumer, UnregisterConsumer };

    struct Record
    {
        Call                    kind;
        uint32_t                number;
        areg::DisconnectReason  reason;
    };

    //!< Records the registrations the processor forwards.
    class FakeRegistration final : public RegistrationProvider
    {
    public:
        bool register_service_provider(const StubAddress & stubService) override
        {
            mCalls.push_back(Record{ Call::RegisterProvider, static_cast<uint32_t>(stubService), DisconnectReason::UndefinedReason });
            return true;
        }

        void unregister_service_provider(const StubAddress & stubService, const DisconnectReason reason) override
        {
            mCalls.push_back(Record{ Call::UnregisterProvider, static_cast<uint32_t>(stubService), reason });
        }

        bool register_service_consumer(const ProxyAddress & proxyService) override
        {
            mCalls.push_back(Record{ Call::RegisterConsumer, static_cast<uint32_t>(proxyService), DisconnectReason::UndefinedReason });
            return true;
        }

        void unregister_service_consumer(const ProxyAddress & proxyService, const DisconnectReason reason) override
        {
            mCalls.push_back(Record{ Call::UnregisterConsumer, static_cast<uint32_t>(proxyService), reason });
        }

        size_t count(Call kind, uint32_t number) const
        {
            size_t result{ 0u };
            for (const Record & record : mCalls)
                result += ((record.kind == kind) && (record.number == number)) ? 1u : 0u;

            return result;
        }

        std::vector<Record> mCalls;
    };

    //!< Records the connection requests the processor forwards.
    class FakeConnection final : public ConnectionProvider
    {
    public:
        bool setup_connection_data(RemoteServiceKind, uint32_t) override
        {
            ++ mSetups;
            return mAccept;
        }

        void apply_connection_data(const String &, uint16_t) override
        {
        }

        bool connect_service_host() override
        {
            ++ mConnects;
            return true;
        }

        bool reconnect_service_host() override
        {
            return true;
        }

        void disconnect_service_host() override
        {
            ++ mDisconnects;
        }

        bool is_host_connected() const override
        {
            return false;
        }

        bool is_host_pending() const override
        {
            return false;
        }

        bool is_host_setup() const override
        {
            return false;
        }

        MessageEnvelope connect_message(const ITEM_ID &, const ITEM_ID &, MessageSource) const override
        {
            return MessageEnvelope{};
        }

        MessageEnvelope disconnect_message(const ITEM_ID &, const ITEM_ID &) const override
        {
            return MessageEnvelope{};
        }

        bool        mAccept     { true };
        uint32_t    mSetups     { 0u };
        uint32_t    mConnects   { 0u };
        uint32_t    mDisconnects{ 0u };
    };

    StubAddress stub_with_channel(const StubAddress & stub, ITEM_ID source, ITEM_ID cookie)
    {
        StubAddress result(stub);
        result.set_channel(Channel(source, 0u, cookie));
        return result;
    }

    ProxyAddress proxy_with_channel(const ProxyAddress & proxy, ITEM_ID source, ITEM_ID target, ITEM_ID cookie)
    {
        ProxyAddress result(proxy);
        result.set_channel(Channel(source, target, cookie));
        return result;
    }

    //!< Feeds the event data to the processor.
    void feed(ServiceManagerEventProcessor & processor, const ServiceManagerEventData & data, FakeConnection & connection, FakeRegistration & registration)
    {
        const ServiceManagerEventData received{ data };
        processor.process_service_event(received.command(), received.read_stream(), connection, registration);
    }

    constexpr ITEM_ID REMOTE_COOKIE{ COOKIE_REMOTE_SERVICE + 5u };
}

/**
 * \brief   A local consumer that was registered and whose proxy is gone is unregistered by the
 *          processor, and a consumer whose proxy still exists is kept.
 **/
TEST(ComponentCoreCoverageTest, consumer_without_its_proxy_is_unregistered_and_one_with_its_proxy_is_kept)
{
    on_core_thread([](DispatcherThread & owner)
    {
        CoreListener listener;
        ServiceManagerEventProcessor processor;
        FakeConnection connection;
        FakeRegistration registration;
        ProxyAddress gone;
        ProxyAddress kept;
        {
            auto goneProxy = ProxyBase::acquire_proxy(String("CoreGoneProvider"), PUBLIC_INTERFACE, listener, CoreProxy::create, owner);
            auto keptProxy = ProxyBase::acquire_proxy(String("CoreKeptProvider"), PUBLIC_INTERFACE, listener, CoreProxy::create, owner);
            ASSERT_NE(nullptr, goneProxy);
            ASSERT_NE(nullptr, keptProxy);
            gone = proxy_with_channel(goneProxy->proxy_address(), 88u, 0u, COOKIE_LOCAL);
            kept = proxy_with_channel(keptProxy->proxy_address(), 88u, 0u, COOKIE_LOCAL);
            ASSERT_TRUE(gone.is_local_address());
            feed(processor, ServiceManagerEventData::register_proxy(gone), connection, registration);
            feed(processor, ServiceManagerEventData::register_proxy(kept), connection, registration);
            EXPECT_EQ(1u, registration.count(Call::RegisterConsumer, static_cast<uint32_t>(gone)));
            EXPECT_EQ(1u, registration.count(Call::RegisterConsumer, static_cast<uint32_t>(kept)));
            goneProxy->free_proxy(listener);
            keptProxy->free_proxy(listener);
        }

        const auto keptAgain = ProxyBase::acquire_proxy(String("CoreKeptProvider"), PUBLIC_INTERFACE, listener, CoreProxy::create, owner);
        ASSERT_NE(nullptr, keptAgain);
        ASSERT_EQ(nullptr, ProxyBase::find_proxy(gone));
        ASSERT_NE(nullptr, ProxyBase::find_proxy(kept));

        processor.unregister_missing_local(registration);
        EXPECT_EQ(1u, registration.count(Call::UnregisterConsumer, static_cast<uint32_t>(gone)));
        EXPECT_EQ(0u, registration.count(Call::UnregisterConsumer, static_cast<uint32_t>(kept)));
        for (const Record & record : registration.mCalls)
        {
            if (record.kind == Call::UnregisterConsumer)
                EXPECT_EQ(DisconnectReason::ConsumerDisconnected, record.reason);
        }

        processor.unregister_missing_local(registration);
        EXPECT_EQ(1u, registration.count(Call::UnregisterConsumer, static_cast<uint32_t>(gone)));
        keptAgain->free_proxy(listener);
    });
}

/**
 * \brief   The registration of a local provider or consumer that has no object is ignored.
 **/
TEST(ComponentCoreCoverageTest, registration_of_a_local_provider_or_consumer_without_object_is_ignored)
{
    ServiceManagerEventProcessor processor;
    FakeConnection connection;
    FakeRegistration registration;
    const StubAddress stub{ stub_with_channel(StubAddress(1u, Version(1, 0, 0), ServiceType::Public, 21u, 22u, 23u), 77u, COOKIE_LOCAL) };
    const ProxyAddress proxy{ proxy_with_channel(ProxyAddress(1u, Version(1, 0, 0), ServiceType::Public, 24u, 22u, 25u), 77u, 0u, COOKIE_LOCAL) };
    ASSERT_TRUE(stub.is_valid() && stub.is_local_address());
    ASSERT_TRUE(proxy.is_valid() && proxy.is_local_address());

    feed(processor, ServiceManagerEventData::register_stub(stub), connection, registration);
    feed(processor, ServiceManagerEventData::register_proxy(proxy), connection, registration);
    EXPECT_TRUE(registration.mCalls.empty());

    processor.unregister_missing_local(registration);
    EXPECT_TRUE(registration.mCalls.empty());
}

/**
 * \brief   An event whose address is not valid registers and unregisters nothing.
 **/
TEST(ComponentCoreCoverageTest, events_with_invalid_addresses_change_nothing)
{
    ServiceManagerEventProcessor processor;
    FakeConnection connection;
    FakeRegistration registration;
    const StubAddress stub;
    const ProxyAddress proxy;
    ASSERT_FALSE(stub.is_valid());
    ASSERT_FALSE(proxy.is_valid());

    feed(processor, ServiceManagerEventData::register_stub(stub), connection, registration);
    feed(processor, ServiceManagerEventData::unregister_stub(stub, DisconnectReason::ProviderDisconnected), connection, registration);
    feed(processor, ServiceManagerEventData::register_proxy(proxy), connection, registration);
    feed(processor, ServiceManagerEventData::unregister_proxy(proxy, DisconnectReason::ConsumerDisconnected), connection, registration);
    EXPECT_TRUE(registration.mCalls.empty());

    feed(processor, ServiceManagerEventData::register_connection(Channel(1u, 2u, COOKIE_LOCAL)), connection, registration);
    EXPECT_TRUE(registration.mCalls.empty());
}

/**
 * \brief   The connection commands reach the connection provider: a start connects only when the
 *          connection data is accepted, a stop and a shutdown disconnect.
 **/
TEST(ComponentCoreCoverageTest, connection_commands_reach_the_connection_provider)
{
    ServiceManagerEventProcessor processor;
    FakeConnection connection;
    FakeRegistration registration;

    feed(processor, ServiceManagerEventData::start_connection(RemoteServiceKind::Router, 1u), connection, registration);
    EXPECT_EQ(1u, connection.mSetups);
    EXPECT_EQ(1u, connection.mConnects);

    connection.mAccept = false;
    feed(processor, ServiceManagerEventData::start_connection(RemoteServiceKind::Router, 1u), connection, registration);
    EXPECT_EQ(2u, connection.mSetups);
    EXPECT_EQ(1u, connection.mConnects);

    feed(processor, ServiceManagerEventData::stop_connection(), connection, registration);
    EXPECT_EQ(1u, connection.mDisconnects);
    feed(processor, ServiceManagerEventData::shutdown_service_manager(), connection, registration);
    EXPECT_EQ(2u, connection.mDisconnects);
}

/**
 * \brief   A new router connection registers the public local providers and the consumers of remote
 *          providers once more; a lost or closed connection removes the remote entries, and the
 *          shutdown removes all.
 **/
TEST(ComponentCoreCoverageTest, connection_events_register_local_services_and_drop_remote_ones)
{
    const StubAddress remoteProvider{ stub_with_channel(StubAddress(1u, Version(1, 0, 0), ServiceType::Public, 31u, 32u, 33u), 78u, REMOTE_COOKIE) };
    const ProxyAddress remoteConsumer{ proxy_with_channel(ProxyAddress(1u, Version(1, 0, 0), ServiceType::Public, 34u, 32u, 35u), 79u, 0u, REMOTE_COOKIE) };
    ASSERT_TRUE(remoteProvider.is_remote_address());
    ASSERT_TRUE(remoteConsumer.is_remote_address());
    const uint32_t remoteProviderNumber{ static_cast<uint32_t>(remoteProvider) };
    const uint32_t remoteConsumerNumber{ static_cast<uint32_t>(remoteConsumer) };

    for (const bool lost : { false, true })
    {
        ServiceManagerEventProcessor processor;
        FakeConnection connection;
        FakeRegistration registration;

        feed(processor, ServiceManagerEventData::register_stub(remoteProvider), connection, registration);
        feed(processor, ServiceManagerEventData::register_proxy(remoteConsumer), connection, registration);
        EXPECT_EQ(0u, registration.count(Call::RegisterProvider, remoteProviderNumber));

        feed(processor, ServiceManagerEventData::register_connection(Channel(1u, 2u, COOKIE_LOCAL)), connection, registration);
        EXPECT_EQ(1u, registration.count(Call::RegisterConsumer, remoteConsumerNumber));
        EXPECT_EQ(0u, registration.count(Call::RegisterProvider, remoteProviderNumber));

        feed(processor, lost ? ServiceManagerEventData::lost_connection(Channel(1u, 2u, REMOTE_COOKIE))
                             : ServiceManagerEventData::unregister_connection(Channel(1u, 2u, REMOTE_COOKIE)), connection, registration);
        feed(processor, ServiceManagerEventData::register_connection(Channel(1u, 2u, COOKIE_LOCAL)), connection, registration);
        EXPECT_EQ(1u, registration.count(Call::RegisterConsumer, remoteConsumerNumber));

        feed(processor, ServiceManagerEventData::register_proxy(remoteConsumer), connection, registration);
        feed(processor, ServiceManagerEventData::shutdown_service_manager(), connection, registration);
        feed(processor, ServiceManagerEventData::register_connection(Channel(1u, 2u, COOKIE_LOCAL)), connection, registration);
        EXPECT_EQ(1u, registration.count(Call::RegisterConsumer, remoteConsumerNumber));
    }
}

namespace
{
    constexpr char      HOST_MODEL[]    { "CoreProcessorModel" };
    constexpr char      HOST_THREAD[]   { "CoreProcessorThread" };

    //!< The line of the first failed check of the scenarios, 0 if none.
    std::atomic<int>    gFailedLine     { 0 };
    //!< Signaled when the scenarios ended.
    SyncEvent           gScenarioDone   { true, false };

    #define CORE_CHECK(condition)                                           \
        do                                                                  \
        {                                                                   \
            if (!(condition))                                               \
            {                                                               \
                int expected{ 0 };                                          \
                gFailedLine.compare_exchange_strong(expected, __LINE__);    \
                return;                                                     \
            }                                                               \
        } while (false)

    //!< A local provider that was registered and whose object is gone is unregistered, one with its object is kept.
    void provider_scenario(ComponentThread & thread)
    {
        Component keeper(String("CoreScenarioKeptRole"), thread);
        Component leaver(String("CoreScenarioGoneRole"), thread);
        ServiceManagerEventProcessor processor;
        FakeConnection connection;
        FakeRegistration registration;
        CoreStub kept(keeper);
        StubAddress goneAddress;
        StubAddress keptAddress;
        {
            CoreStub gone(leaver);
            goneAddress = stub_with_channel(gone.address(), 77u, COOKIE_LOCAL);
            keptAddress = stub_with_channel(kept.address(), 77u, COOKIE_LOCAL);
            CORE_CHECK(goneAddress.is_valid() && keptAddress.is_valid());
            CORE_CHECK(static_cast<uint32_t>(goneAddress) != static_cast<uint32_t>(keptAddress));
            feed(processor, ServiceManagerEventData::register_stub(goneAddress), connection, registration);
            feed(processor, ServiceManagerEventData::register_stub(keptAddress), connection, registration);
            CORE_CHECK(registration.count(Call::RegisterProvider, static_cast<uint32_t>(goneAddress)) == 1u);
            CORE_CHECK(registration.count(Call::RegisterProvider, static_cast<uint32_t>(keptAddress)) == 1u);
        }

        CORE_CHECK(StubBase::find_stub(goneAddress) == nullptr);
        processor.unregister_missing_local(registration);
        CORE_CHECK(registration.count(Call::UnregisterProvider, static_cast<uint32_t>(goneAddress)) == 1u);
        CORE_CHECK(registration.count(Call::UnregisterProvider, static_cast<uint32_t>(keptAddress)) == 0u);
        for (const Record & record : registration.mCalls)
        {
            if (record.kind == Call::UnregisterProvider)
                CORE_CHECK(record.reason == DisconnectReason::ProviderDisconnected);
        }

        processor.unregister_missing_local(registration);
        CORE_CHECK(registration.count(Call::UnregisterProvider, static_cast<uint32_t>(goneAddress)) == 1u);
    }

    //!< The providers and consumers of a thread are unregistered with the reason, those of another thread stay.
    void thread_scenario(ComponentThread & thread)
    {
        CoreListener listener;
        Component holder(String("CoreScenarioThreadRole"), thread);
        ServiceManagerEventProcessor processor;
        FakeConnection connection;
        FakeRegistration registration;
        auto proxy = ProxyBase::acquire_proxy(String("CoreScenarioThreadProvider"), PUBLIC_INTERFACE, listener, CoreProxy::create, thread);
        CORE_CHECK(proxy != nullptr);
        CoreStub stub(holder);
        const StubAddress stubAddress{ stub_with_channel(stub.address(), 77u, COOKIE_LOCAL) };
        const ProxyAddress proxyAddress{ proxy_with_channel(proxy->proxy_address(), 88u, 0u, COOKIE_LOCAL) };
        feed(processor, ServiceManagerEventData::register_stub(stubAddress), connection, registration);
        feed(processor, ServiceManagerEventData::register_proxy(proxyAddress), connection, registration);

        processor.unregister_thread_services(ThreadAddress(String("CoreScenarioOtherThread")), DisconnectReason::ProviderRestarting, registration);
        CORE_CHECK(registration.count(Call::UnregisterProvider, static_cast<uint32_t>(stubAddress)) == 0u);
        CORE_CHECK(registration.count(Call::UnregisterConsumer, static_cast<uint32_t>(proxyAddress)) == 0u);

        processor.unregister_thread_services(thread.address(), DisconnectReason::ProviderRestarting, registration);
        CORE_CHECK(registration.count(Call::UnregisterProvider, static_cast<uint32_t>(stubAddress)) == 1u);
        CORE_CHECK(registration.count(Call::UnregisterConsumer, static_cast<uint32_t>(proxyAddress)) == 1u);
        for (const Record & record : registration.mCalls)
        {
            if ((record.kind == Call::UnregisterProvider) || (record.kind == Call::UnregisterConsumer))
                CORE_CHECK(record.reason == DisconnectReason::ProviderRestarting);
        }

        proxy->free_proxy(listener);
    }

    //!< A provider or a consumer is unregistered by its number and cookie only.
    void number_scenario(ComponentThread & thread)
    {
        CoreListener listener;
        Component first(String("CoreScenarioFirstRole"), thread);
        Component second(String("CoreScenarioSecondRole"), thread);
        ServiceManagerEventProcessor processor;
        FakeConnection connection;
        FakeRegistration registration;
        auto proxy = ProxyBase::acquire_proxy(String("CoreScenarioNumberProvider"), PUBLIC_INTERFACE, listener, CoreProxy::create, thread);
        CORE_CHECK(proxy != nullptr);
        CoreStub firstStub(first);
        CoreStub secondStub(second);
        const StubAddress firstAddress{ stub_with_channel(firstStub.address(), 77u, COOKIE_LOCAL) };
        const StubAddress secondAddress{ stub_with_channel(secondStub.address(), 77u, COOKIE_LOCAL) };
        const ProxyAddress proxyAddress{ proxy_with_channel(proxy->proxy_address(), 88u, 0u, COOKIE_LOCAL) };
        feed(processor, ServiceManagerEventData::register_stub(firstAddress), connection, registration);
        feed(processor, ServiceManagerEventData::register_stub(secondAddress), connection, registration);
        feed(processor, ServiceManagerEventData::register_proxy(proxyAddress), connection, registration);

        const uint32_t secondNumber{ static_cast<uint32_t>(secondAddress) };
        processor.unregister_by_number(secondNumber, COOKIE_LOCAL + 1u, DisconnectReason::ProviderDisconnected, true, registration);
        processor.unregister_by_number(secondNumber + 1u, COOKIE_LOCAL, DisconnectReason::ProviderDisconnected, true, registration);
        CORE_CHECK(registration.count(Call::UnregisterProvider, secondNumber) == 0u);
        processor.unregister_by_number(secondNumber, COOKIE_LOCAL, DisconnectReason::ProviderDisconnected, true, registration);
        CORE_CHECK(registration.count(Call::UnregisterProvider, secondNumber) == 1u);
        CORE_CHECK(registration.count(Call::UnregisterProvider, static_cast<uint32_t>(firstAddress)) == 0u);

        const uint32_t proxyNumber{ static_cast<uint32_t>(proxyAddress) };
        processor.unregister_by_number(proxyNumber, COOKIE_LOCAL + 1u, DisconnectReason::ConsumerDisconnected, false, registration);
        CORE_CHECK(registration.count(Call::UnregisterConsumer, proxyNumber) == 0u);
        processor.unregister_by_number(proxyNumber, COOKIE_LOCAL, DisconnectReason::ConsumerDisconnected, false, registration);
        CORE_CHECK(registration.count(Call::UnregisterConsumer, proxyNumber) == 1u);
        processor.unregister_by_number(proxyNumber, COOKIE_LOCAL, DisconnectReason::ConsumerDisconnected, false, registration);
        CORE_CHECK(registration.count(Call::UnregisterConsumer, proxyNumber) == 1u);

        proxy->free_proxy(listener);
    }

    //!< The events of the service manager and of the dispatcher reach the stub by their kind.
    void stub_event_scenario(ComponentThread & thread)
    {
        Component holder(String("CoreScenarioEventRole"), thread);
        CoreStub stub(holder);
        const StubAddress target{ stub.address() };
        ProxyAddress sender{ target };
        sender.set_channel(Channel(88u, 0u, COOKIE_LOCAL));

        LocalRequestEvent request(sender, target, REQUEST_ID_FIRST, 0u);
        stub.deliver(request);
        CORE_CHECK(stub.mUndelivered == 1u);
        CORE_CHECK(stub.mUndeliveredMsg == REQUEST_ID_FIRST);
        CORE_CHECK(stub.mRequests == 0u);

        stub.startup_service_interface(holder);
        stub.deliver(request);
        CORE_CHECK(stub.mRequests == 1u);
        CORE_CHECK(stub.mUndelivered == 1u);

        LocalRequestEvent other(sender, target, ATTRIBUTE_ID_FIRST, 0u);
        stub.deliver(other);
        CORE_CHECK(stub.mStubEvents == 1u);
        CORE_CHECK(stub.mRequests == 1u);

        LocalNotifyRequestEvent notify(sender, target, ATTRIBUTE_ID_FIRST, RequestType::StartNotify);
        stub.deliver(notify);
        CORE_CHECK(stub.mAttributes == 1u);

        LocalResponseEvent response(sender, ResultType::RequestOK, RESPONSE_ID_FIRST, 0u);
        stub.deliver(response);
        CORE_CHECK(stub.mGenerics == 1u);
        CORE_CHECK(stub.mRequests == 1u);

        stub.shutdown_service_interface(holder);
    }

    //!< A component that runs the scenarios on its running thread.
    class CoreScenarioHost final : public Component
    {
    public:
        CoreScenarioHost(const ComponentEntry & entry, ComponentThread & owner)
            : Component(entry, owner)
        {
        }

    protected:
        void startup_component(ComponentThread & comThread) override
        {
            Component::startup_component(comThread);
            provider_scenario(comThread);
            thread_scenario(comThread);
            number_scenario(comThread);
            stub_event_scenario(comThread);
            gScenarioDone.set_signaled();
        }
    };
}

/**
 * \brief   On a running component thread, the processor unregisters a provider whose object is gone and
 *          keeps one that has its object, unregisters the services of a thread with the given reason,
 *          and unregisters by number and cookie only.
 **/
TEST(ComponentCoreCoverageTest, processor_unregisters_providers_by_thread_number_and_missing_object)
{
    AREG_EXPECT_IN_CHILD
    (
        ComponentThreadEntry entry{ String(HOST_THREAD) };
        entry.add_component(ComponentEntry(String(HOST_THREAD), String("CoreProcessorHost")
            , [](const ComponentEntry & item, ComponentThread & owner) -> Component * { return new CoreScenarioHost(item, owner); }
            , &delete_core));
        Model model{ String(HOST_MODEL) };
        model.add_thread(entry);
        AREG_CHILD_CHECK(ComponentLoader::add_model_unique(model));
        Application::setup(false, true, false, true, false);
        AREG_CHILD_CHECK(Application::load_model(HOST_MODEL, true));
        AREG_CHILD_CHECK(gScenarioDone.lock(WAIT_MS));
        if (gFailedLine.load() != 0)
            std::fprintf(stderr, "scenario failed at line %d\n", gFailedLine.load());
        AREG_CHILD_CHECK(gFailedLine.load() == 0);
        AREG_CHILD_CHECK(Application::unload_model(nullptr));
        Application::release();
    );
}
#endif  // defined(AREG_TEST_PRIVATE_POSIX)
