/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Copyright (c) 2017-2026 Aregtech (Artak Avetyan).
 * Areg SDK is dual-licensed under Apache 2.0 and commercial licenses.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/SharedBuffer.hpp"
#include "areg/component/DispatcherThread.hpp"
#include "areg/component/NotificationEvent.hpp"
#include "areg/component/ProxyBase.hpp"
#include "areg/component/ProxyListener.hpp"
#include "areg/component/RequestEvents.hpp"
#include "areg/component/ResponseEvents.hpp"
#include "areg/component/StubBase.hpp"

#include <chrono>
#include <functional>
#include <future>
#include <memory>
#include <tuple>
#include <utility>
#include <vector>

namespace
{
    using namespace areg;

    constexpr uint32_t REQUESTS[]{ REQUEST_ID_FIRST, REQUEST_ID_FIRST + 1 };
    constexpr uint32_t RESPONSES[]{ RESPONSE_ID_FIRST };
    constexpr uint32_t ATTRIBUTES[]{ ATTRIBUTE_ID_FIRST };
    constexpr uint32_t RESPONSE_MAP[]{ RESPONSE_ID_FIRST, RESPONSE_ID_NONE };
    constexpr uint32_t PARAM_COUNTS[]{ 1 };
    const InterfaceData INTERFACE
    {
          "ProxyContractService", Version(1, 0, 0), ServiceType::Local
        , 2, 1, 1, REQUESTS, RESPONSES, ATTRIBUTES, RESPONSE_MAP, PARAM_COUNTS
    };

    class ContractListener final : public ProxyListener
    {
    public:
        ContractListener()
            : mConnections()
            , mNotifications()
            , mDuringNotification()
        {
        }

        bool service_connected(ServiceConnectionState status, ProxyBase&) override
        {
            mConnections.push_back(status);
            return true;
        }

        void process_notification_event(NotificationEvent& event) override
        {
            const NotificationEventData& data = std::as_const(event).data();
            process_notification(data.notify_id(), data.notify_type(), data.sequence());
        }

        void process_notification(uint32_t message, ResultType result, SequenceNumber sequence) override
        {
            mNotifications.emplace_back(message, result, sequence);
            if (mDuringNotification)
                mDuringNotification();
        }

        std::vector<ServiceConnectionState> mConnections;
        std::vector<std::tuple<uint32_t, ResultType, SequenceNumber>> mNotifications;
        std::function<void()> mDuringNotification;
    };

    class ContractProxy final : public ProxyBase
    {
    public:
        ContractProxy(const String& role, DispatcherThread* owner)
            : ProxyBase(role, INTERFACE, owner)
            , mFailures()
        {
        }

        using ProxyBase::add_listener;
        using ProxyBase::is_listener_registered;
        using ProxyBase::notify_listeners;
        using ProxyBase::process_available_event;
        using ProxyBase::remove_listener;
        using ProxyBase::send_request_event;
        using ProxyBase::set_connection_status;
        using ProxyBase::set_state;
        using ProxyBase::service_connection_updated;

        static ProxyBase* create(const String& role, DispatcherThread* owner)
        {
            return new ContractProxy(role, owner);
        }

        DataState attribute_state() const
        {
            return proxy_data().attribute_state(ATTRIBUTES[0]);
        }

        void accept_response(ServiceResponseEvent& response)
        {
            process_response_event(response);
        }

        mutable std::vector<std::tuple<uint32_t, ResultType, SequenceNumber>> mFailures;

    protected:
        void process_response_event(ServiceResponseEvent& response) override
        {
            notify_listeners(response.response_id(), static_cast<ResultType>(response.envelope().result()), response.sequence_number());
        }

        void process_custom_event(Event&) override
        {
        }

        ServiceAvailableEvent create_service_available(NotificationConsumer& consumer) override
        {
            return ServiceAvailableEvent(consumer);
        }

        NotificationEvent create_client_notification(const NotificationEventData& data) const override
        {
            return NotificationEvent(data);
        }

        ServiceRequestEvent create_request(const SharedBuffer& data, uint32_t request) override
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

        ServiceResponseEvent create_request_failed(const ProxyAddress& target, uint32_t message
                                                  , ResultType reason, const SequenceNumber& sequence) const override
        {
            mFailures.emplace_back(message, reason, sequence);
            return LocalResponseEvent(target, reason, message, sequence);
        }
    };

    class ProxyContractThread final : public DispatcherThread
    {
    public:
        explicit ProxyContractThread(std::function<void(DispatcherThread&)> action, const char* name)
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
        bool post_event(Event& event) override
        {
            return EventDispatcher::post_event(event);
        }

        void on_run() override
        {
            mAction(*this);
            mDone.set_value();
        }

    private:
        std::function<void(DispatcherThread&)> mAction;
        std::promise<void> mDone;
    };

    void on_proxy_thread(std::function<void(DispatcherThread&)> action, const char* name = "ProxyContractThread")
    {
        ProxyContractThread thread(std::move(action), name);
        auto done = thread.completion();
        ASSERT_TRUE(thread.start(WAIT_INFINITE));
        EXPECT_EQ(std::future_status::ready, done.wait_for(std::chrono::seconds(30)));
        EXPECT_EQ(Thread::ThreadCompletion::Completed, thread.shutdown(WAIT_INFINITE));
    }

    class ProxyLease
    {
    public:
        ProxyLease(ContractListener& listener, DispatcherThread& owner)
            : mListener(listener)
            , mProxy(ProxyBase::acquire_proxy(String("ProxyContractProvider"), INTERFACE, listener, ContractProxy::create, owner))
        {
        }

        ~ProxyLease()
        {
            if (mProxy != nullptr)
                mProxy->free_proxy(mListener);
        }

        ContractProxy* operator->() const
        {
            return static_cast<ContractProxy*>(mProxy.get());
        }

        std::shared_ptr<ProxyBase> pointer() const
        {
            return mProxy;
        }

    private:
        ContractListener& mListener;
        std::shared_ptr<ProxyBase> mProxy;
    };
}

TEST(ProxyBaseContractTest, registry_shares_one_proxy_and_duplicate_acquire_does_not_add_a_client)
{
    on_proxy_thread([](DispatcherThread& owner)
    {
        ContractListener first;
        ContractListener second;
        ProxyAddress address;
        std::weak_ptr<ProxyBase> released;
        {
            ProxyLease lease(first, owner);
            ASSERT_NE(nullptr, lease.pointer());
            address = lease->proxy_address();
            released = lease.pointer();
            EXPECT_EQ(lease.pointer(), ProxyBase::find_proxy(address));
            EXPECT_EQ(lease.pointer(), ProxyBase::find_proxy(static_cast<UniqueNumber>(address)));
            EXPECT_EQ(&owner, &lease->proxy_dispatcher_thread());
            EXPECT_FALSE(lease->is_connected());
            EXPECT_EQ(ServiceConnectionState::Unknown, lease->connection_status());
            auto duplicate = ProxyBase::acquire_proxy(String("ProxyContractProvider"), INTERFACE
                                                     , first, ContractProxy::create, owner.name());
            auto shared = ProxyBase::acquire_proxy(String("ProxyContractProvider"), INTERFACE
                                                  , second, ContractProxy::create, static_cast<UniqueNumber>(owner.address()));
            ASSERT_EQ(lease.pointer(), duplicate);
            ASSERT_EQ(lease.pointer(), shared);
            ArrayList<std::shared_ptr<ProxyBase>> proxies;
            ASSERT_EQ(1, ProxyBase::find_thread_proxies(owner, proxies));
            EXPECT_EQ(lease.pointer(), proxies[0]);
            EXPECT_TRUE(lease->is_listener_registered(first));
            EXPECT_TRUE(lease->is_listener_registered(second));
            shared->free_proxy(second);
            EXPECT_FALSE(lease->is_listener_registered(second));
            EXPECT_TRUE(lease->is_listener_registered(first));
        }
        EXPECT_EQ(nullptr, ProxyBase::find_proxy(address));
        EXPECT_TRUE(released.expired());
        ArrayList<std::shared_ptr<ProxyBase>> proxies;
        EXPECT_EQ(0, ProxyBase::find_thread_proxies(owner, proxies));
    });
}

TEST(ProxyBaseContractTest, null_factory_reports_failure_without_registering_a_proxy)
{
    on_proxy_thread([](DispatcherThread& owner)
    {
        ContractListener listener;
        uint32_t calls = 0;
        const auto proxy = ProxyBase::acquire_proxy(String("ProxyContractMissingFactory"), INTERFACE, listener
            , [&calls](const String&, DispatcherThread*) -> ProxyBase*
            {
                ++calls;
                return nullptr;
            }, owner);
        EXPECT_EQ(nullptr, proxy);
        EXPECT_EQ(1u, calls);
        ArrayList<std::shared_ptr<ProxyBase>> proxies;
        EXPECT_EQ(0, ProxyBase::find_thread_proxies(owner, proxies));
    });
}

TEST(ProxyBaseContractTest, notification_and_pending_listeners_have_distinct_lifetimes)
{
    on_proxy_thread([](DispatcherThread& owner)
    {
        ContractListener persistent;
        ContractListener first;
        ContractListener second;
        ProxyLease proxy(persistent, owner);
        ASSERT_NE(nullptr, proxy.pointer());
        EXPECT_FALSE(proxy->has_any_listener(RESPONSES[0]));
        EXPECT_FALSE(proxy->has_notification_listener(RESPONSES[0]));
        ASSERT_TRUE(proxy->add_listener(RESPONSES[0], SEQUENCE_NUMBER_NOTIFY, &persistent, true));
        EXPECT_FALSE(proxy->add_listener(RESPONSES[0], SEQUENCE_NUMBER_NOTIFY, &persistent, true));
        ASSERT_TRUE(proxy->add_listener(RESPONSES[0], 11, &first, true));
        ASSERT_TRUE(proxy->add_listener(RESPONSES[0], 12, &second, true));
        proxy->notify_listeners(RESPONSES[0], ResultType::RequestOK, 12);
        EXPECT_TRUE(first.mNotifications.empty());
        ASSERT_EQ(1u, second.mNotifications.size());
        EXPECT_EQ(12u, std::get<2>(second.mNotifications[0]));
        EXPECT_EQ(1u, persistent.mNotifications.size());
        proxy->notify_listeners(RESPONSES[0], ResultType::RequestOK, 12);
        EXPECT_EQ(1u, second.mNotifications.size());
        EXPECT_EQ(2u, persistent.mNotifications.size());
        proxy->notify_listeners(RESPONSES[0], ResultType::RequestOK, 11);
        EXPECT_EQ(1u, first.mNotifications.size());
        EXPECT_TRUE(proxy->has_notification_listener(RESPONSES[0]));
        proxy->remove_listener(RESPONSES[0], SEQUENCE_NUMBER_NOTIFY, &persistent);
        EXPECT_FALSE(proxy->has_any_listener(RESPONSES[0]));
        EXPECT_FALSE(proxy->has_notification_listener(RESPONSES[0]));
    });
}

TEST(ProxyBaseContractTest, pending_only_listener_is_not_a_subscription)
{
    on_proxy_thread([](DispatcherThread& owner)
    {
        ContractListener listener;
        ProxyLease proxy(listener, owner);
        ASSERT_NE(nullptr, proxy.pointer());
        ASSERT_TRUE(proxy->add_listener(RESPONSES[0], 19, &listener, true));
        EXPECT_TRUE(proxy->has_any_listener(RESPONSES[0]));
        EXPECT_FALSE(proxy->has_notification_listener(RESPONSES[0]));
        proxy->remove_listener(RESPONSES[0], 20, &listener);
        EXPECT_TRUE(proxy->has_any_listener(RESPONSES[0]));
        proxy->notify_listeners(RESPONSES[0], ResultType::RequestOK, 19);
        EXPECT_FALSE(proxy->has_any_listener(RESPONSES[0]));
        EXPECT_EQ(1u, listener.mNotifications.size());
    });
}

TEST(ProxyBaseContractTest, clearing_notifications_preserves_connection_and_other_consumers)
{
    on_proxy_thread([](DispatcherThread& owner)
    {
        ContractListener first;
        ContractListener second;
        ProxyLease proxy(first, owner);
        ASSERT_NE(nullptr, proxy.pointer());
        ASSERT_TRUE(proxy->add_listener(ATTRIBUTES[0], SEQUENCE_NUMBER_NOTIFY, &first, true));
        ASSERT_TRUE(proxy->add_listener(ATTRIBUTES[0], SEQUENCE_NUMBER_NOTIFY, &second, true));
        ASSERT_TRUE(proxy->add_listener(RESPONSES[0], 11, &first, true));
        proxy->clear_all_notifications(first);
        EXPECT_TRUE(proxy->is_listener_registered(first));
        EXPECT_FALSE(proxy->has_any_listener(RESPONSES[0]));
        EXPECT_TRUE(proxy->has_notification_listener(ATTRIBUTES[0]));
        proxy->notify_listeners(ATTRIBUTES[0], ResultType::DataOK, SEQUENCE_NUMBER_NOTIFY);
        EXPECT_TRUE(first.mNotifications.empty());
        EXPECT_EQ(1u, second.mNotifications.size());
        proxy->clear_all_notifications(second);
        proxy->clear_all_notifications(second);
        EXPECT_FALSE(proxy->has_any_listener(ATTRIBUTES[0]));
        EXPECT_EQ(DataState::DataIsUnavailable, proxy->attribute_state());
    });
}

TEST(ProxyBaseContractTest, removing_a_later_consumer_during_notification_retracts_the_snapshot)
{
    on_proxy_thread([](DispatcherThread& owner)
    {
        ContractListener first;
        ContractListener second;
        ProxyLease proxy(first, owner);
        ASSERT_NE(nullptr, proxy.pointer());
        ASSERT_TRUE(proxy->add_listener(RESPONSES[0], SEQUENCE_NUMBER_NOTIFY, &first, true));
        ASSERT_TRUE(proxy->add_listener(RESPONSES[0], SEQUENCE_NUMBER_NOTIFY, &second, true));
        first.mDuringNotification = [&] { proxy->clear_all_notifications(second); };
        proxy->notify_listeners(RESPONSES[0], ResultType::RequestOK, SEQUENCE_NUMBER_NOTIFY);
        EXPECT_EQ(1u, first.mNotifications.size());
        EXPECT_TRUE(second.mNotifications.empty());
        proxy->notify_listeners(RESPONSES[0], ResultType::RequestOK, SEQUENCE_NUMBER_NOTIFY);
        EXPECT_EQ(2u, first.mNotifications.size());
        EXPECT_TRUE(second.mNotifications.empty());
    });
}

TEST(ProxyBaseContractTest, heap_sized_notification_snapshot_delivers_every_subscriber_exactly_once)
{
    on_proxy_thread([](DispatcherThread& owner)
    {
        ContractListener ownerListener;
        ProxyLease proxy(ownerListener, owner);
        ASSERT_NE(nullptr, proxy.pointer());
        std::vector<std::unique_ptr<ContractListener>> clients;
        for (uint32_t i = 0; i < 17; ++i)
        {
            clients.emplace_back(new ContractListener);
            ASSERT_TRUE(proxy->add_listener(RESPONSES[0], SEQUENCE_NUMBER_NOTIFY, clients.back().get(), true));
        }
        proxy->notify_listeners(RESPONSES[0], ResultType::RequestOK, SEQUENCE_NUMBER_NOTIFY);
        for (const auto& client : clients)
        {
            ASSERT_EQ(1u, client->mNotifications.size());
            EXPECT_EQ(RESPONSES[0], std::get<0>(client->mNotifications[0]));
            EXPECT_EQ(ResultType::RequestOK, std::get<1>(client->mNotifications[0]));
            proxy->clear_all_notifications(*client);
        }
        EXPECT_FALSE(proxy->has_any_listener(RESPONSES[0]));
    });
}

TEST(ProxyBaseContractTest, invalid_request_does_not_allocate_a_sequence_or_listener)
{
    on_proxy_thread([](DispatcherThread& owner)
    {
        ContractListener listener;
        ProxyLease proxy(listener, owner);
        ASSERT_NE(nullptr, proxy.pointer());
        ServiceRequestEvent invalid(MessageEnvelope{});
        EXPECT_EQ(SEQUENCE_NUMBER_ANY, proxy->send_request_event(invalid, &listener));
        EXPECT_FALSE(proxy->has_any_listener(RESPONSES[0]));
        EXPECT_TRUE(proxy->mFailures.empty());
    });
}

TEST(ProxyBaseContractTest, disconnected_requests_have_distinct_sequences_and_one_way_has_no_failure)
{
    on_proxy_thread([](DispatcherThread& owner)
    {
        ContractListener listener;
        ProxyLease proxy(listener, owner);
        ASSERT_NE(nullptr, proxy.pointer());
        LocalRequestEvent first(proxy->proxy_address(), StubAddress{}, REQUESTS[0]);
        LocalRequestEvent second(proxy->proxy_address(), StubAddress{}, REQUESTS[0]);
        const SequenceNumber firstSequence = proxy->send_request_event(first, &listener);
        const SequenceNumber secondSequence = proxy->send_request_event(second, &listener);
        EXPECT_NE(firstSequence, secondSequence);
        EXPECT_EQ(firstSequence + 1, secondSequence);
        EXPECT_EQ(firstSequence, first.sequence_number());
        EXPECT_EQ(secondSequence, second.sequence_number());
        ASSERT_EQ(2u, proxy->mFailures.size());
        EXPECT_EQ(std::make_tuple(REQUESTS[0], ResultType::ServiceUnavailable, firstSequence), proxy->mFailures[0]);
        EXPECT_EQ(std::make_tuple(REQUESTS[0], ResultType::ServiceUnavailable, secondSequence), proxy->mFailures[1]);
        LocalRequestEvent oneWay(proxy->proxy_address(), StubAddress{}, REQUESTS[1]);
        EXPECT_EQ(SEQUENCE_NUMBER_ANY, proxy->send_request_event(oneWay, nullptr));
        EXPECT_EQ(2u, proxy->mFailures.size());
        proxy->clear_all_notifications(listener);
        EXPECT_FALSE(proxy->has_any_listener(RESPONSES[0]));
    });
}

TEST(ProxyBaseContractTest, failure_factory_preserves_target_reason_and_sequence_and_unknown_target_is_invalid)
{
    on_proxy_thread([](DispatcherThread& owner)
    {
        ContractListener listener;
        ProxyLease proxy(listener, owner);
        ASSERT_NE(nullptr, proxy.pointer());
        auto failure = ProxyBase::request_failure_event(proxy->proxy_address(), REQUESTS[0], ResultType::MessageUndelivered, 55);
        ASSERT_TRUE(failure.is_valid());
        EXPECT_EQ(REQUESTS[0], failure.response_id());
        EXPECT_EQ(55u, failure.sequence_number());
        EXPECT_EQ(static_cast<uint32_t>(ResultType::MessageUndelivered), failure.envelope().result());
        EXPECT_EQ(static_cast<uint32_t>(proxy->proxy_address()), failure.target_proxy());
        EXPECT_FALSE(owner.is_ready());
        EXPECT_FALSE(failure.deliver_event());
        ProxyAddress unknown(INTERFACE, String("ProxyContractUnknown"), owner.name());
        EXPECT_FALSE(ProxyBase::request_failure_event(unknown, REQUESTS[0], ResultType::ServiceUnavailable, 55).is_valid());
    });
}

TEST(ProxyBaseContractTest, stop_and_detach_are_idempotent_and_release_both_registries)
{
    on_proxy_thread([](DispatcherThread& owner)
    {
        ContractListener listener;
        ProxyLease proxy(listener, owner);
        ASSERT_NE(nullptr, proxy.pointer());
        const ProxyAddress address = proxy->proxy_address();
        ASSERT_TRUE(proxy->add_listener(RESPONSES[0], 31, &listener, true));
        proxy->set_state(ATTRIBUTES[0], DataState::DataIsOK);
        proxy->stop_proxy();
        proxy->stop_proxy();
        EXPECT_FALSE(proxy->is_connected());
        EXPECT_EQ(ServiceConnectionState::Disconnected, proxy->connection_status());
        EXPECT_FALSE(proxy->has_any_listener(RESPONSES[0]));
        EXPECT_FALSE(proxy->is_listener_registered(listener));
        EXPECT_EQ(DataState::DataIsUnavailable, proxy->attribute_state());
        proxy->detach_from_registry();
        proxy->detach_from_registry();
        EXPECT_EQ(nullptr, ProxyBase::find_proxy(address));
        ArrayList<std::shared_ptr<ProxyBase>> proxies;
        EXPECT_EQ(0, ProxyBase::find_thread_proxies(owner, proxies));
    });
}

TEST(ProxyBaseContractTest, connection_changes_ignore_incompatible_and_repeated_updates_and_reset_cached_state)
{
    on_proxy_thread([](DispatcherThread& owner)
    {
        ContractListener listener;
        ProxyLease proxy(listener, owner);
        ASSERT_NE(nullptr, proxy.pointer());
        const StubAddress server(INTERFACE, String("ProxyContractProvider"), owner.name());
        const StubAddress incompatible(INTERFACE, String("ProxyContractOtherProvider"), owner.name());
        ASSERT_TRUE(server.is_valid());
        ASSERT_TRUE(incompatible.is_valid());
        ASSERT_FALSE(incompatible.is_proxy_compatible(proxy->proxy_address()));
        ASSERT_TRUE(server.is_proxy_compatible(proxy->proxy_address()));
        const Channel channel(static_cast<uint32_t>(owner.address()), server.source(), COOKIE_LOCAL);
        proxy->service_connection_updated(incompatible, channel, ServiceConnectionState::Connected);
        EXPECT_FALSE(proxy->is_connected());
        EXPECT_TRUE(listener.mConnections.empty());
        proxy->service_connection_updated(server, channel, ServiceConnectionState::Connected);
        EXPECT_TRUE(proxy->is_connected());
        EXPECT_EQ(server, proxy->stub_address());
        EXPECT_EQ(ServiceConnectionState::Connected, proxy->connection_status());
        ASSERT_EQ(1u, listener.mConnections.size());
        EXPECT_EQ(ServiceConnectionState::Connected, listener.mConnections[0]);
        proxy->service_connection_updated(server, channel, ServiceConnectionState::Connected);
        EXPECT_EQ(1u, listener.mConnections.size());
        proxy->set_state(ATTRIBUTES[0], DataState::DataIsOK);
        proxy->service_connection_updated(server, channel, ServiceConnectionState::Disconnected);
        EXPECT_FALSE(proxy->is_connected());
        EXPECT_FALSE(proxy->stub_address().is_valid());
        EXPECT_EQ(DataState::DataIsUnavailable, proxy->attribute_state());
        ASSERT_EQ(2u, listener.mConnections.size());
        EXPECT_EQ(ServiceConnectionState::Disconnected, listener.mConnections[1]);
        proxy->service_connection_updated(server, channel, ServiceConnectionState::Disconnected);
        EXPECT_EQ(2u, listener.mConnections.size());
    });
}

TEST(ProxyBaseContractTest, available_client_is_notified_once_and_free_then_stop_disconnect_each_client_once)
{
    on_proxy_thread([](DispatcherThread& owner)
    {
        ContractListener first;
        ContractListener second;
        ProxyLease proxy(first, owner);
        ASSERT_NE(nullptr, proxy.pointer());
        proxy->process_available_event(first);
        EXPECT_TRUE(first.mConnections.empty());
        const StubAddress server(INTERFACE, String("ProxyContractProvider"), owner.name());
        ASSERT_TRUE(server.is_valid());
        ASSERT_TRUE(server.is_proxy_compatible(proxy->proxy_address()));
        const Channel channel(static_cast<uint32_t>(owner.address()), server.source(), COOKIE_LOCAL);
        proxy->service_connection_updated(server, channel, ServiceConnectionState::Connected);
        auto attached = ProxyBase::acquire_proxy(String("ProxyContractProvider"), INTERFACE, second, ContractProxy::create, owner);
        ASSERT_EQ(proxy.pointer(), attached);
        proxy->process_available_event(second);
        proxy->process_available_event(second);
        ASSERT_EQ(1u, second.mConnections.size());
        EXPECT_EQ(ServiceConnectionState::Connected, second.mConnections[0]);
        attached->free_proxy(second);
        proxy->process_available_event(second);
        ASSERT_EQ(2u, second.mConnections.size());
        EXPECT_EQ(ServiceConnectionState::Disconnected, second.mConnections[1]);
        EXPECT_EQ(1u, first.mConnections.size());
        proxy->stop_proxy();
        proxy->stop_proxy();
        ASSERT_EQ(2u, first.mConnections.size());
        EXPECT_EQ(ServiceConnectionState::Disconnected, first.mConnections[1]);
        EXPECT_EQ(2u, second.mConnections.size());
    });
}

TEST(ProxyBaseContractTest, adding_a_consumer_during_notification_affects_only_the_next_snapshot)
{
    on_proxy_thread([](DispatcherThread& owner)
    {
        ContractListener first;
        ContractListener later;
        ProxyLease proxy(first, owner);
        ASSERT_NE(nullptr, proxy.pointer());
        ASSERT_TRUE(proxy->add_listener(RESPONSES[0], SEQUENCE_NUMBER_NOTIFY, &first, true));
        first.mDuringNotification = [&]
        {
            static_cast<void>(proxy->add_listener(RESPONSES[0], SEQUENCE_NUMBER_NOTIFY, &later, true));
        };
        proxy->notify_listeners(RESPONSES[0], ResultType::RequestOK, SEQUENCE_NUMBER_NOTIFY);
        EXPECT_EQ(1u, first.mNotifications.size());
        EXPECT_TRUE(later.mNotifications.empty());
        proxy->notify_listeners(RESPONSES[0], ResultType::RequestOK, SEQUENCE_NUMBER_NOTIFY);
        EXPECT_EQ(2u, first.mNotifications.size());
        EXPECT_EQ(1u, later.mNotifications.size());
        proxy->clear_all_notifications(later);
    });
}

TEST(ProxyBaseContractTest, disconnected_subscriptions_do_not_install_listeners)
{
    on_proxy_thread([](DispatcherThread& owner)
    {
        ContractListener listener;
        ProxyLease proxy(listener, owner);
        ASSERT_NE(nullptr, proxy.pointer());
        proxy->set_notification(ATTRIBUTES[0], &listener, true);
        proxy->set_notification(RESPONSES[0], &listener, true);
        EXPECT_FALSE(proxy->has_any_listener(ATTRIBUTES[0]));
        EXPECT_FALSE(proxy->has_any_listener(RESPONSES[0]));
        EXPECT_TRUE(listener.mNotifications.empty());
        EXPECT_TRUE(proxy->is_listener_registered(listener));
    });
}

TEST(ProxyBaseContractTest, connected_request_to_missing_provider_reports_undelivered_with_its_sequence)
{
    StubAddress missing;
    on_proxy_thread([&](DispatcherThread& provider)
    {
        missing = StubAddress(INTERFACE, String("ProxyContractProvider"), provider.name());
        ASSERT_TRUE(missing.is_valid());
    }, "ProxyContractMissingThread");
    ASSERT_TRUE(missing.is_valid());
    ASSERT_FALSE(DispatcherThread::dispatcher_thread(missing.thread()).is_valid());
    ASSERT_EQ(nullptr, StubBase::find_stub(missing));
    on_proxy_thread([&](DispatcherThread& owner)
    {
        ContractListener listener;
        ProxyLease proxy(listener, owner);
        ASSERT_NE(nullptr, proxy.pointer());
        ASSERT_TRUE(missing.is_proxy_compatible(proxy->proxy_address()));
        const Channel channel(static_cast<uint32_t>(owner.address()), missing.source(), COOKIE_LOCAL);
        proxy->service_connection_updated(missing, channel, ServiceConnectionState::Connected);
        LocalRequestEvent request(proxy->proxy_address(), missing, REQUESTS[0]);
        const SequenceNumber sequence = proxy->send_request_event(request, &listener);
        EXPECT_NE(SEQUENCE_NUMBER_ANY, sequence);
        ASSERT_EQ(1u, proxy->mFailures.size());
        EXPECT_EQ(std::make_tuple(REQUESTS[0], ResultType::MessageUndelivered, sequence), proxy->mFailures[0]);
        EXPECT_TRUE(proxy->has_any_listener(RESPONSES[0]));
        proxy->clear_all_notifications(listener);
        EXPECT_FALSE(proxy->has_any_listener(RESPONSES[0]));
    });
}

TEST(ProxyBaseContractTest, subscribed_caller_is_notified_once_with_its_sequence)
{
    on_proxy_thread([](DispatcherThread& owner)
    {
        ContractListener caller;
        ContractListener other;
        ProxyLease proxy(caller, owner);
        ASSERT_NE(nullptr, proxy.pointer());
        ASSERT_TRUE(proxy->add_listener(RESPONSES[0], SEQUENCE_NUMBER_NOTIFY, &caller, true));
        ASSERT_TRUE(proxy->add_listener(RESPONSES[0], SEQUENCE_NUMBER_NOTIFY, &other, true));
        ASSERT_TRUE(proxy->add_listener(RESPONSES[0], 41, &caller, true));
        proxy->notify_listeners(RESPONSES[0], ResultType::RequestOK, 41);
        ASSERT_EQ(1u, caller.mNotifications.size());
        EXPECT_EQ(std::make_tuple(RESPONSES[0], ResultType::RequestOK, SequenceNumber{ 41 }), caller.mNotifications[0]);
        EXPECT_EQ(1u, other.mNotifications.size());
        EXPECT_TRUE(proxy->has_notification_listener(RESPONSES[0]));
        proxy->notify_listeners(RESPONSES[0], ResultType::RequestOK, SEQUENCE_NUMBER_NOTIFY);
        EXPECT_EQ(2u, caller.mNotifications.size());
        EXPECT_EQ(2u, other.mNotifications.size());
    });
}

TEST(ProxyBaseContractTest, request_failures_reach_only_the_waiting_caller)
{
    on_proxy_thread([](DispatcherThread& owner)
    {
        const ResultType failures[]{ ResultType::RequestError, ResultType::RequestBusy, ResultType::RequestCanceled
                                   , ResultType::MessageUndelivered, ResultType::ServiceUnavailable };
        ContractListener subscriber;
        ContractListener caller;
        ProxyLease proxy(subscriber, owner);
        ASSERT_NE(nullptr, proxy.pointer());
        ASSERT_TRUE(proxy->add_listener(RESPONSES[0], SEQUENCE_NUMBER_NOTIFY, &subscriber, true));
        SequenceNumber sequence = 50;
        for (const ResultType failure : failures)
        {
            ++sequence;
            ASSERT_TRUE(proxy->add_listener(RESPONSES[0], sequence, &caller, true));
            proxy->notify_listeners(RESPONSES[0], failure, sequence);
            ASSERT_FALSE(caller.mNotifications.empty());
            EXPECT_EQ(std::make_tuple(RESPONSES[0], failure, sequence), caller.mNotifications.back());
        }

        EXPECT_EQ(5u, caller.mNotifications.size());
        EXPECT_TRUE(subscriber.mNotifications.empty());
        proxy->notify_listeners(RESPONSES[0], ResultType::RequestError, 99);
        proxy->notify_listeners(RESPONSES[0], ResultType::RequestOK, 51);
        EXPECT_EQ(5u, caller.mNotifications.size());
        EXPECT_EQ(1u, subscriber.mNotifications.size());
        EXPECT_TRUE(proxy->has_notification_listener(RESPONSES[0]));
    });
}

TEST(ProxyBaseContractTest, subscription_event_leaves_the_waiting_caller_pending)
{
    on_proxy_thread([](DispatcherThread& owner)
    {
        ContractListener subscriber;
        ContractListener caller;
        ProxyLease proxy(subscriber, owner);
        ASSERT_NE(nullptr, proxy.pointer());
        ASSERT_TRUE(proxy->add_listener(RESPONSES[0], SEQUENCE_NUMBER_NOTIFY, &subscriber, true));
        ASSERT_TRUE(proxy->add_listener(RESPONSES[0], 61, &caller, true));
        proxy->notify_listeners(RESPONSES[0], ResultType::RequestOK, SEQUENCE_NUMBER_NOTIFY);
        EXPECT_EQ(1u, subscriber.mNotifications.size());
        EXPECT_TRUE(caller.mNotifications.empty());
        proxy->notify_listeners(RESPONSES[0], ResultType::RequestOK, 61);
        EXPECT_EQ(2u, subscriber.mNotifications.size());
        ASSERT_EQ(1u, caller.mNotifications.size());
        EXPECT_EQ(61u, std::get<2>(caller.mNotifications[0]));
    });
}
