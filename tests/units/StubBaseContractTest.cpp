/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Copyright (c) 2017-2026 Aregtech (Artak Avetyan).
 * Areg SDK is dual-licensed under Apache 2.0 and commercial licenses.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/SharedBuffer.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/ResponseEvents.hpp"
#include "areg/component/StubBase.hpp"

#include <limits>
#include <utility>
#include <vector>

namespace
{
    using namespace areg;

    constexpr uint32_t REQUESTS[]{ REQUEST_ID_FIRST, REQUEST_ID_FIRST + 1 };
    constexpr uint32_t RESPONSES[]{ RESPONSE_ID_FIRST, RESPONSE_ID_FIRST + 1 };
    constexpr uint32_t ATTRIBUTES[]{ ATTRIBUTE_ID_FIRST, ATTRIBUTE_ID_FIRST + 1 };
    constexpr uint32_t RESPONSE_MAP[]{ RESPONSE_ID_FIRST, RESPONSE_ID_FIRST + 1 };
    constexpr uint32_t PARAM_COUNTS[]{ 1, 0 };
    const InterfaceData INTERFACE
    {
          "StubContractService", Version(1, 2, 3), ServiceType::Local
        , 2, 2, 2, REQUESTS, RESPONSES, ATTRIBUTES, RESPONSE_MAP, PARAM_COUNTS
    };

    constexpr uint32_t BROADCAST{ RESPONSE_ID_FIRST + 2 };
    constexpr uint32_t BROADCAST_RESPONSES[]{ RESPONSE_ID_FIRST, RESPONSE_ID_FIRST + 1, BROADCAST };
    constexpr uint32_t BROADCAST_PARAM_COUNTS[]{ 1, 0, 0 };
    const InterfaceData BROADCAST_INTERFACE
    {
          "StubContractBroadcastService", Version(1, 0, 0), ServiceType::Local
        , 2, 3, 2, REQUESTS, BROADCAST_RESPONSES, ATTRIBUTES, RESPONSE_MAP, BROADCAST_PARAM_COUNTS
    };

    ProxyAddress consumer_address(const ServiceAddress& service, const char* threadName)
    {
        const ThreadAddress thread{ String(threadName) };
        const UniqueNumber threadNumber = static_cast<uint32_t>(thread);
        ProxyAddress result(static_cast<const ServiceItem&>(service), static_cast<uint32_t>(service)
                            , threadNumber, threadNumber);
        result.set_channel(Channel(threadNumber, 0, COOKIE_LOCAL));
        return result;
    }

    class ContractStub final : public StubBase
    {
    public:
        explicit ContractStub(Component& component, const InterfaceData& data = INTERFACE)
            : StubBase(component, data)
            , mErrors()
            , mResponseAttempts()
        {
            // Explicit endpoint identities keep these registry-only fixtures independent of live dispatchers.
            const ServiceAddress service{ static_cast<const ServiceAddress&>(address()) };
            const uint32_t threadNumber = static_cast<uint32_t>(component.master_thread().address());
            StubAddress registered(service, threadNumber, static_cast<uint32_t>(service));
            registered.set_source(threadNumber);
            process_registered_event(registered, ServiceConnectionState::Connected);
        }

        using StubBase::Listener;
        using StubBase::StubListenerList;
        using StubBase::add_notification_listener;
        using StubBase::attribute_ids;
        using StubBase::cancel_current_request;
        using StubBase::clear_all_listeners;
        using StubBase::exist;
        using StubBase::find_listeners;
        using StubBase::impl_version;
        using StubBase::invalidate_attribute;
        using StubBase::is_busy;
        using StubBase::number_of_attributes;
        using StubBase::number_of_requests;
        using StubBase::number_of_responses;
        using StubBase::prepare_request;
        using StubBase::prepare_response_event;
        using StubBase::remove_notification_listener;
        using StubBase::request_ids;
        using StubBase::response_ids;
        using StubBase::send_error_notification;
        using StubBase::send_response_notification;
        using StubBase::send_update_notification;
        using StubBase::mSessionId;

        bool begin_request(const ProxyAddress& proxy, SequenceNumber sequence, uint32_t response = RESPONSES[0])
        {
            Listener listener(REQUESTS[0], sequence, proxy);
            return can_execute_request(listener, response, sequence);
        }

        ServiceResponseEvent default_response(const ProxyAddress& proxy) const
        {
            return StubBase::create_response(proxy, RESPONSES[0], ResultType::RequestOK, SharedBuffer{});
        }

        ServiceResponseEvent default_response_event(const ProxyAddress& proxy) const
        {
            return StubBase::create_response_event(proxy, RESPONSES[0], ResultType::RequestOK, 64);
        }

        //!< Returns the targets of the next response or broadcast; empty when it is dropped.
        StubListenerList respond(uint32_t response = RESPONSES[0])
        {
            StubListenerList targets;
            const ServiceResponseEvent event{ prepare_response_event(response, ResultType::RequestOK, 0, targets) };
            EXPECT_EQ(event.is_valid(), !targets.is_empty());
            return targets;
        }

        //!< Sends the error of a request the way a generated error_request() does.
        void fail_request(ResultType result, uint32_t request = REQUESTS[0], uint32_t response = RESPONSES[0])
        {
            StubListenerList listeners;
            if (find_listeners(response, listeners) > 0)
                send_error_notification(listeners, LocalResponseEvent(listeners[0].proxy(), result, request, SEQUENCE_NUMBER_NOTIFY, 0));
        }

        //!< Returns how many times a response with the result was created for the message.
        uint32_t attempts(uint32_t message, ResultType result) const
        {
            uint32_t count = 0;
            for (const auto& attempt : mResponseAttempts)
                count += ((attempt.first == message) && (attempt.second == result)) ? 1u : 0u;
            return count;
        }

        void send_notification(uint32_t) override
        {
        }

        void error_request(uint32_t message, bool cancel) override
        {
            mErrors.emplace_back(message, cancel);
        }

        std::vector<std::pair<uint32_t, bool>> mErrors;
        mutable std::vector<std::pair<uint32_t, ResultType>> mResponseAttempts;

    protected:
        ServiceResponseEvent create_response(const ProxyAddress& proxy, uint32_t message
                                            , ResultType result, const SharedBuffer& data) const override
        {
            mResponseAttempts.emplace_back(message, result);
            return StubBase::create_response(proxy, message, result, data);
        }

        ServiceResponseEvent create_response_event(const ProxyAddress& proxy, uint32_t message
                                                  , ResultType result, uint32_t reserve) const override
        {
            return LocalResponseEvent(proxy, result, message, SEQUENCE_NUMBER_NOTIFY, reserve);
        }

        void process_request_event(ServiceRequestEvent&) override
        {
        }

        void process_attribute_event(ServiceRequestEvent&) override
        {
        }
    };

    class StubBaseContractTest : public testing::Test
    {
    protected:
        StubBaseContractTest()
            : mThread(String("StubContractThread"))
            , mComponent(String("StubContractProvider"), mThread)
            , mStub(mComponent)
            , mFirst(consumer_address(mStub.address(), "StubContractConsumerA"))
            , mSecond(consumer_address(mStub.address(), "StubContractConsumerB"))
        {
        }

        void SetUp() override
        {
            ASSERT_TRUE(mStub.address().is_valid());
            ASSERT_TRUE(mFirst.is_valid());
            ASSERT_TRUE(mSecond.is_valid());
            ASSERT_TRUE(mStub.address().is_proxy_compatible(mFirst));
            ASSERT_TRUE(mStub.address().is_proxy_compatible(mSecond));
            ASSERT_NE(static_cast<uint32_t>(mFirst), static_cast<uint32_t>(mSecond));
        }

        ComponentThread mThread;
        Component mComponent;
        ContractStub mStub;
        ProxyAddress mFirst;
        ProxyAddress mSecond;
    };
}

TEST_F(StubBaseContractTest, metadata_and_registry_follow_component_ownership)
{
    EXPECT_EQ(&mComponent, &mStub.component());
    EXPECT_EQ(&mThread, &mStub.component_thread());
    EXPECT_EQ(mComponent.role_name(), mStub.service_role());
    EXPECT_EQ(String(INTERFACE.idServiceName), mStub.service_name());
    EXPECT_EQ(Version(1, 2, 3), mStub.impl_version());
    EXPECT_EQ(2u, mStub.number_of_requests());
    EXPECT_EQ(2u, mStub.number_of_responses());
    EXPECT_EQ(2u, mStub.number_of_attributes());
    EXPECT_EQ(REQUESTS, mStub.request_ids());
    EXPECT_EQ(RESPONSES, mStub.response_ids());
    EXPECT_EQ(ATTRIBUTES, mStub.attribute_ids());
    EXPECT_EQ(&mStub, StubBase::find_stub(mStub.address()));
    EXPECT_EQ(&mStub, StubBase::find_stub(static_cast<UniqueNumber>(mStub.address())));
    EXPECT_EQ(mStub.address(), StubBase::registered_address(mStub.address()));
    EXPECT_EQ(&mStub, mComponent.find_provider(String(INTERFACE.idServiceName)));
    EXPECT_EQ(nullptr, mComponent.find_provider(String("UnknownContractService")));
    EXPECT_EQ(&mComponent, Component::find_by_name(mComponent.role_name()));
    EXPECT_EQ(&mComponent, Component::find_by_address(mComponent.address()));
    EXPECT_TRUE(Component::exist(mComponent.role_name()));
    EXPECT_EQ(nullptr, mComponent.find_event_consumer(0x12345678));
    EXPECT_EQ(1u, mComponent.extract_service_addresses().size());
}

//!< A response that lost its block while it was written is dropped, for every listener.
TEST_F(StubBaseContractTest, invalid_response_is_dropped)
{
    ContractStub::StubListenerList listeners;
    listeners.add(ContractStub::Listener(REQUESTS[0], 1u, mFirst));
    listeners.add(ContractStub::Listener(REQUESTS[0], 2u, mSecond));

    ServiceResponseEvent invalid{ MessageEnvelope{} };
    mStub.send_response_notification(listeners, invalid);
    mStub.send_update_notification(listeners, invalid);
    EXPECT_FALSE(invalid.is_valid());
}

TEST_F(StubBaseContractTest, deleting_stub_removes_only_its_registry_entry)
{
    StubAddress removed;
    {
        Component other(String("StubContractOtherProvider"), mThread);
        ContractStub stub(other);
        removed = stub.address();
        ASSERT_TRUE(removed.is_valid());
        ASSERT_NE(static_cast<uint32_t>(mStub.address()), static_cast<uint32_t>(removed));
        ASSERT_EQ(&stub, StubBase::find_stub(removed));
    }

    EXPECT_EQ(nullptr, StubBase::find_stub(removed));
    EXPECT_EQ(removed, StubBase::registered_address(removed));
    EXPECT_EQ(&mStub, StubBase::find_stub(mStub.address()));
}

TEST_F(StubBaseContractTest, notification_registration_is_unique_per_message_and_proxy)
{
    EXPECT_FALSE(mStub.exist(ATTRIBUTES[0], mFirst));
    EXPECT_FALSE(mStub.add_notification_listener(ATTRIBUTES[0], ProxyAddress{}));
    EXPECT_FALSE(mStub.exist(ATTRIBUTES[0], ProxyAddress{}));
    EXPECT_TRUE(mStub.add_notification_listener(ATTRIBUTES[0], mFirst));
    EXPECT_FALSE(mStub.add_notification_listener(ATTRIBUTES[0], mFirst));
    EXPECT_TRUE(mStub.add_notification_listener(ATTRIBUTES[0], mSecond));
    EXPECT_TRUE(mStub.add_notification_listener(ATTRIBUTES[1], mFirst));
    EXPECT_TRUE(mStub.exist(ATTRIBUTES[0], mFirst));
    EXPECT_FALSE(mStub.is_busy(ATTRIBUTES[0]));
    ContractStub::StubListenerList listeners;
    ASSERT_EQ(2u, mStub.find_listeners(ATTRIBUTES[0], listeners));
    EXPECT_TRUE(listeners[0].proxy_matches(mFirst));
    EXPECT_TRUE(listeners[1].proxy_matches(mSecond));
    EXPECT_EQ(mFirst, listeners[0].proxy());
}

TEST_F(StubBaseContractTest, unsubscribe_compacts_without_removing_other_subscribers)
{
    ASSERT_TRUE(mStub.add_notification_listener(ATTRIBUTES[0], mFirst));
    ASSERT_TRUE(mStub.add_notification_listener(ATTRIBUTES[0], mSecond));
    mStub.remove_notification_listener(ATTRIBUTES[0], mFirst);
    mStub.remove_notification_listener(ATTRIBUTES[0], mFirst);
    mStub.remove_notification_listener(ATTRIBUTES[1], mFirst);
    EXPECT_FALSE(mStub.exist(ATTRIBUTES[0], mFirst));
    EXPECT_TRUE(mStub.exist(ATTRIBUTES[0], mSecond));
    ContractStub::StubListenerList remaining;
    ASSERT_EQ(1u, mStub.find_listeners(ATTRIBUTES[0], remaining));
    EXPECT_TRUE(remaining[0].proxy_matches(mSecond));
    mStub.remove_notification_listener(ATTRIBUTES[0], mSecond);
    EXPECT_TRUE(mStub.add_notification_listener(ATTRIBUTES[0], mFirst));
    EXPECT_TRUE(mStub.exist(ATTRIBUTES[0], mFirst));
}

TEST_F(StubBaseContractTest, clearing_proxy_reports_exact_removed_subscriptions_and_keeps_its_request)
{
    ASSERT_TRUE(mStub.add_notification_listener(ATTRIBUTES[0], mFirst));
    ASSERT_TRUE(mStub.add_notification_listener(ATTRIBUTES[0], mSecond));
    ASSERT_TRUE(mStub.add_notification_listener(ATTRIBUTES[1], mFirst));
    ASSERT_TRUE(mStub.begin_request(mFirst, 41));
    IntegerArray removed;
    mStub.clear_all_listeners(mFirst, removed);
    EXPECT_EQ(2u, removed.size());
    EXPECT_TRUE(removed.contains(ATTRIBUTES[0]));
    EXPECT_TRUE(removed.contains(ATTRIBUTES[1]));
    EXPECT_FALSE(removed.contains(RESPONSES[0]));
    EXPECT_FALSE(mStub.exist(ATTRIBUTES[0], mFirst));
    EXPECT_TRUE(mStub.is_busy(RESPONSES[0]));
    EXPECT_TRUE(mStub.exist(ATTRIBUTES[0], mSecond));
    IntegerArray none;
    mStub.clear_all_listeners(mFirst, none);
    EXPECT_TRUE(none.is_empty());
    EXPECT_TRUE(mStub.is_busy(RESPONSES[0]));
}

TEST_F(StubBaseContractTest, disconnect_clears_only_the_matching_service_consumer)
{
    ASSERT_TRUE(mStub.add_notification_listener(ATTRIBUTES[0], mFirst));
    ASSERT_TRUE(mStub.add_notification_listener(ATTRIBUTES[0], mSecond));
    const ServiceAddress other(String(INTERFACE.idServiceName), INTERFACE.idVersion, INTERFACE.idServiceType, String("OtherStubContractRole"));
    ProxyAddress wrong = consumer_address(other, "StubContractOtherConsumer");
    ASSERT_TRUE(wrong.is_valid());
    EXPECT_FALSE(mStub.consumer_connected(wrong, ServiceConnectionState::Disconnected));
    EXPECT_TRUE(mStub.exist(ATTRIBUTES[0], mFirst));
    EXPECT_TRUE(mStub.consumer_connected(mFirst, ServiceConnectionState::Connected));
    EXPECT_TRUE(mStub.exist(ATTRIBUTES[0], mFirst));
    EXPECT_TRUE(mStub.consumer_connected(mFirst, ServiceConnectionState::Disconnected));
    EXPECT_FALSE(mStub.exist(ATTRIBUTES[0], mFirst));
    EXPECT_TRUE(mStub.exist(ATTRIBUTES[0], mSecond));
}

TEST_F(StubBaseContractTest, busy_request_is_refused_without_replacing_the_pending_caller)
{
    ASSERT_TRUE(mStub.begin_request(mFirst, 41));
    EXPECT_TRUE(mStub.is_busy(RESPONSES[0]));
    EXPECT_FALSE(mStub.begin_request(mSecond, 42));
    ASSERT_EQ(1u, mStub.mResponseAttempts.size());
    EXPECT_EQ(REQUESTS[0], mStub.mResponseAttempts[0].first);
    EXPECT_EQ(ResultType::RequestBusy, mStub.mResponseAttempts[0].second);
    ContractStub::StubListenerList listeners;
    ASSERT_EQ(1u, mStub.find_listeners(RESPONSES[0], listeners));
    EXPECT_TRUE(listeners[0].proxy_matches(mFirst));
    EXPECT_EQ(41u, listeners[0].mSequenceNr);
}

TEST_F(StubBaseContractTest, unblock_sessions_are_unique_consumed_once_and_restorable_out_of_order)
{
    EXPECT_EQ(std::numeric_limits<SessionID>::max(), mStub.unblock_current_request());
    EXPECT_FALSE(mStub.prepare_response(999));
    ASSERT_TRUE(mStub.begin_request(mFirst, 41));
    const SessionID first = mStub.unblock_current_request();
    EXPECT_NE(std::numeric_limits<SessionID>::max(), first);
    EXPECT_FALSE(mStub.is_busy(RESPONSES[0]));
    ASSERT_TRUE(mStub.begin_request(mSecond, 42));
    const SessionID second = mStub.unblock_current_request();
    EXPECT_NE(first, second);
    ASSERT_TRUE(mStub.prepare_response(second));
    EXPECT_FALSE(mStub.prepare_response(second));
    ContractStub::StubListenerList listeners;
    auto response = mStub.prepare_response_event(RESPONSES[0], ResultType::RequestOK, 64, listeners);
    ASSERT_TRUE(response.is_valid());
    ASSERT_EQ(1u, listeners.size());
    EXPECT_TRUE(listeners[0].proxy_matches(mSecond));
    EXPECT_EQ(42u, listeners[0].mSequenceNr);
    EXPECT_EQ(RESPONSES[0], response.response_id());
    EXPECT_EQ(static_cast<uint32_t>(mSecond), response.target_proxy());
    EXPECT_FALSE(mStub.is_busy(RESPONSES[0]));
    ASSERT_TRUE(mStub.prepare_response(first));
    listeners.clear();
    const auto firstResponse = mStub.prepare_response_event(RESPONSES[0], ResultType::RequestOK, 0, listeners);
    ASSERT_TRUE(firstResponse.is_valid());
    ASSERT_EQ(1u, listeners.size());
    EXPECT_TRUE(listeners[0].proxy_matches(mFirst));
    EXPECT_EQ(41u, listeners[0].mSequenceNr);
    EXPECT_FALSE(mStub.prepare_response(first));
}

TEST_F(StubBaseContractTest, unblocking_one_request_preserves_subscriptions_and_other_responses)
{
    ASSERT_TRUE(mStub.add_notification_listener(RESPONSES[0], mSecond));
    ASSERT_TRUE(mStub.begin_request(mFirst, 41));
    const SessionID session = mStub.unblock_current_request();
    EXPECT_TRUE(mStub.exist(RESPONSES[0], mSecond));
    EXPECT_FALSE(mStub.is_busy(RESPONSES[0]));
    EXPECT_EQ(std::numeric_limits<SessionID>::max(), mStub.unblock_current_request());
    ASSERT_TRUE(mStub.begin_request(mSecond, 42, RESPONSES[1]));
    EXPECT_TRUE(mStub.prepare_response(session));
    EXPECT_TRUE(mStub.is_busy(RESPONSES[0]));
    EXPECT_TRUE(mStub.is_busy(RESPONSES[1]));
}

TEST_F(StubBaseContractTest, preparing_response_strips_one_shot_listeners_but_keeps_subscriptions)
{
    ASSERT_TRUE(mStub.add_notification_listener(RESPONSES[0], mFirst));
    ContractStub::StubListenerList listeners;
    EXPECT_FALSE(mStub.prepare_response_event(RESPONSES[0], ResultType::RequestOK, 0, listeners).is_valid());
    EXPECT_TRUE(listeners.is_empty());
    EXPECT_TRUE(mStub.exist(RESPONSES[0], mFirst));
    listeners.clear();
    EXPECT_FALSE(mStub.prepare_response_event(RESPONSES[1], ResultType::RequestOK, 0, listeners).is_valid());
    EXPECT_TRUE(listeners.is_empty());
    ASSERT_TRUE(mStub.begin_request(mSecond, 71, RESPONSES[1]));
    EXPECT_TRUE(mStub.prepare_response_event(RESPONSES[1], ResultType::RequestOK, 0, listeners).is_valid());
    EXPECT_EQ(1u, listeners.size());
    EXPECT_FALSE(mStub.is_busy(RESPONSES[1]));
    listeners.clear();
    EXPECT_FALSE(mStub.prepare_response_event(RESPONSES[1], ResultType::RequestOK, 0, listeners).is_valid());
}

TEST_F(StubBaseContractTest, error_all_visits_attributes_responses_then_requests_exactly_once)
{
    mStub.error_all_requests();
    const std::vector<std::pair<uint32_t, bool>> expected
    {
          { ATTRIBUTES[0], false }, { ATTRIBUTES[1], false }
        , { RESPONSES[0], false }, { RESPONSES[1], false }
        , { REQUESTS[0], false }, { REQUESTS[1], false }
    };
    EXPECT_EQ(expected, mStub.mErrors);
}

TEST_F(StubBaseContractTest, cancel_all_visits_only_requests_and_marks_cancellation)
{
    mStub.cancel_all_requests();
    const std::vector<std::pair<uint32_t, bool>> expected{ { REQUESTS[0], true }, { REQUESTS[1], true } };
    EXPECT_EQ(expected, mStub.mErrors);
}

TEST_F(StubBaseContractTest, invalidate_attribute_ignores_request_response_and_invalid_ids)
{
    mStub.invalidate_attribute(REQUESTS[0]);
    mStub.invalidate_attribute(RESPONSES[0]);
    mStub.invalidate_attribute(0);
    EXPECT_TRUE(mStub.mErrors.empty());
    mStub.invalidate_attribute(ATTRIBUTES[0]);
    ASSERT_EQ(1u, mStub.mErrors.size());
    EXPECT_EQ(ATTRIBUTES[0], mStub.mErrors[0].first);
    EXPECT_FALSE(mStub.mErrors[0].second);
}

TEST_F(StubBaseContractTest, base_response_factories_explicitly_return_invalid_events)
{
    EXPECT_FALSE(mStub.default_response(mFirst).is_valid());
    EXPECT_FALSE(mStub.default_response_event(mFirst).is_valid());
}

TEST_F(StubBaseContractTest, listener_equal_sequences_self_and_explicit_wildcard)
{
    ContractStub::Listener first(RESPONSES[0], 41, mFirst);
    const ContractStub::Listener same(RESPONSES[0], 41, mFirst);
    const ContractStub::Listener other(RESPONSES[0], 42, mFirst);
    const ContractStub::Listener any(RESPONSES[0], SEQUENCE_NUMBER_ANY);
    EXPECT_TRUE(first == first);
    EXPECT_TRUE(first == same);
    EXPECT_FALSE(first == other);
    EXPECT_TRUE(first == any);
}

TEST_F(StubBaseContractTest, notification_sequence_is_not_a_pending_request)
{
    const ContractStub::Listener notification(RESPONSES[0], SEQUENCE_NUMBER_NOTIFY, mFirst);
    const ContractStub::Listener pending(RESPONSES[0], 17, mFirst);
    EXPECT_FALSE(notification == pending);
    EXPECT_FALSE(pending == notification);
}

TEST_F(StubBaseContractTest, response_answers_the_caller_and_keeps_an_earlier_subscription)
{
    const ProxyAddress third{ consumer_address(mStub.address(), "StubContractConsumerC") };
    ASSERT_TRUE(mStub.add_notification_listener(RESPONSES[0], mSecond));
    ASSERT_TRUE(mStub.begin_request(mFirst, 41));
    EXPECT_FALSE(mStub.begin_request(third, 7));
    EXPECT_EQ(1u, mStub.attempts(REQUESTS[0], ResultType::RequestBusy));
    const auto targets = mStub.respond();
    ASSERT_EQ(2u, targets.size());
    EXPECT_TRUE(targets[0].proxy_matches(mFirst));
    EXPECT_EQ(41u, targets[0].mSequenceNr);
    EXPECT_TRUE(targets[1].proxy_matches(mSecond));
    EXPECT_EQ(SEQUENCE_NUMBER_NOTIFY, targets[1].mSequenceNr);
    EXPECT_FALSE(mStub.is_busy(RESPONSES[0]));
    EXPECT_TRUE(mStub.exist(RESPONSES[0], mSecond));
    EXPECT_TRUE(mStub.begin_request(mFirst, 42));
}

TEST_F(StubBaseContractTest, response_answers_the_caller_and_keeps_a_later_subscription)
{
    ASSERT_TRUE(mStub.begin_request(mFirst, 41));
    ASSERT_TRUE(mStub.add_notification_listener(RESPONSES[0], mSecond));
    const auto targets = mStub.respond();
    ASSERT_EQ(2u, targets.size());
    EXPECT_TRUE(targets[0].proxy_matches(mFirst));
    EXPECT_EQ(41u, targets[0].mSequenceNr);
    EXPECT_TRUE(targets[1].proxy_matches(mSecond));
    EXPECT_FALSE(mStub.is_busy(RESPONSES[0]));
    EXPECT_TRUE(mStub.exist(RESPONSES[0], mSecond));
}

TEST_F(StubBaseContractTest, prepared_session_is_answered_before_a_waiting_request)
{
    const ProxyAddress third{ consumer_address(mStub.address(), "StubContractConsumerC") };
    ASSERT_TRUE(mStub.begin_request(mFirst, 41));
    const SessionID session = mStub.unblock_current_request();
    ASSERT_NE(std::numeric_limits<SessionID>::max(), session);
    ASSERT_TRUE(mStub.add_notification_listener(RESPONSES[0], mSecond));
    ASSERT_TRUE(mStub.begin_request(third, 7));
    ASSERT_TRUE(mStub.prepare_response(session));
    auto targets = mStub.respond();
    ASSERT_EQ(2u, targets.size());
    EXPECT_TRUE(targets[0].proxy_matches(mFirst));
    EXPECT_EQ(41u, targets[0].mSequenceNr);
    EXPECT_TRUE(targets[1].proxy_matches(mSecond));
    EXPECT_TRUE(mStub.is_busy(RESPONSES[0]));
    targets = mStub.respond();
    ASSERT_EQ(2u, targets.size());
    EXPECT_TRUE(targets[0].proxy_matches(third));
    EXPECT_EQ(7u, targets[0].mSequenceNr);
    EXPECT_TRUE(targets[1].proxy_matches(mSecond));
    EXPECT_FALSE(mStub.is_busy(RESPONSES[0]));
    EXPECT_TRUE(mStub.exist(RESPONSES[0], mSecond));
}

TEST_F(StubBaseContractTest, subscribed_caller_receives_one_response_with_its_sequence)
{
    ASSERT_TRUE(mStub.add_notification_listener(RESPONSES[0], mFirst));
    ASSERT_TRUE(mStub.add_notification_listener(RESPONSES[0], mSecond));
    ASSERT_TRUE(mStub.begin_request(mFirst, 41));
    const auto targets = mStub.respond();
    ASSERT_EQ(2u, targets.size());
    EXPECT_TRUE(targets[0].proxy_matches(mFirst));
    EXPECT_EQ(41u, targets[0].mSequenceNr);
    EXPECT_TRUE(targets[1].proxy_matches(mSecond));
    EXPECT_EQ(SEQUENCE_NUMBER_NOTIFY, targets[1].mSequenceNr);
    EXPECT_TRUE(mStub.exist(RESPONSES[0], mFirst));
    EXPECT_TRUE(mStub.exist(RESPONSES[0], mSecond));
    EXPECT_FALSE(mStub.is_busy(RESPONSES[0]));
}

TEST_F(StubBaseContractTest, response_without_a_waiting_request_is_dropped_and_answers_once)
{
    ASSERT_TRUE(mStub.add_notification_listener(RESPONSES[0], mFirst));
    ASSERT_TRUE(mStub.add_notification_listener(RESPONSES[0], mSecond));
    EXPECT_TRUE(mStub.respond().is_empty());
    ASSERT_TRUE(mStub.begin_request(mFirst, 41));
    EXPECT_EQ(2u, mStub.respond().size());
    EXPECT_TRUE(mStub.respond().is_empty());
    EXPECT_TRUE(mStub.exist(RESPONSES[0], mFirst));
    EXPECT_TRUE(mStub.exist(RESPONSES[0], mSecond));
}

TEST_F(StubBaseContractTest, broadcast_reaches_every_subscriber_without_a_request)
{
    Component other(String("StubContractBroadcastProvider"), mThread);
    ContractStub stub(other, BROADCAST_INTERFACE);
    const ProxyAddress first{ consumer_address(stub.address(), "StubContractBroadcastA") };
    const ProxyAddress second{ consumer_address(stub.address(), "StubContractBroadcastB") };
    ASSERT_TRUE(stub.add_notification_listener(BROADCAST, first));
    ASSERT_TRUE(stub.add_notification_listener(BROADCAST, second));
    EXPECT_EQ(2u, stub.respond(BROADCAST).size());
    EXPECT_EQ(2u, stub.respond(BROADCAST).size());
    EXPECT_TRUE(stub.exist(BROADCAST, first));
    EXPECT_TRUE(stub.exist(BROADCAST, second));
}

TEST_F(StubBaseContractTest, one_prepared_session_per_response_and_requests_wait_for_it)
{
    ASSERT_TRUE(mStub.begin_request(mFirst, 41));
    const SessionID firstA = mStub.unblock_current_request();
    ASSERT_TRUE(mStub.begin_request(mSecond, 42));
    const SessionID secondA = mStub.unblock_current_request();
    ASSERT_TRUE(mStub.begin_request(mFirst, 43, RESPONSES[1]));
    const SessionID firstB = mStub.unblock_current_request();
    EXPECT_FALSE(mStub.is_busy(RESPONSES[0]));
    ASSERT_TRUE(mStub.prepare_response(firstA));
    EXPECT_TRUE(mStub.is_busy(RESPONSES[0]));
    EXPECT_FALSE(mStub.prepare_response(secondA));
    EXPECT_TRUE(mStub.prepare_response(firstB));
    EXPECT_FALSE(mStub.begin_request(mSecond, 44));
    EXPECT_EQ(1u, mStub.attempts(REQUESTS[0], ResultType::RequestBusy));
    auto targets = mStub.respond();
    ASSERT_EQ(1u, targets.size());
    EXPECT_EQ(41u, targets[0].mSequenceNr);
    targets = mStub.respond(RESPONSES[1]);
    ASSERT_EQ(1u, targets.size());
    EXPECT_EQ(43u, targets[0].mSequenceNr);
    ASSERT_TRUE(mStub.prepare_response(secondA));
    targets = mStub.respond();
    ASSERT_EQ(1u, targets.size());
    EXPECT_EQ(42u, targets[0].mSequenceNr);
    EXPECT_FALSE(mStub.prepare_response(firstA));
}

TEST_F(StubBaseContractTest, request_error_reaches_only_the_target_and_leaves_subscriptions)
{
    const ProxyAddress third{ consumer_address(mStub.address(), "StubContractConsumerC") };
    ASSERT_TRUE(mStub.add_notification_listener(RESPONSES[0], mSecond));
    ASSERT_TRUE(mStub.begin_request(mFirst, 41));
    const SessionID session = mStub.unblock_current_request();
    ASSERT_TRUE(mStub.begin_request(third, 7));
    mStub.fail_request(ResultType::RequestError);
    EXPECT_FALSE(mStub.is_busy(RESPONSES[0]));
    EXPECT_TRUE(mStub.exist(RESPONSES[0], mSecond));
    mStub.fail_request(ResultType::RequestError);
    EXPECT_TRUE(mStub.exist(RESPONSES[0], mSecond));
    ASSERT_TRUE(mStub.prepare_response(session));
    const auto targets = mStub.respond();
    ASSERT_EQ(2u, targets.size());
    EXPECT_EQ(41u, targets[0].mSequenceNr);
}

TEST_F(StubBaseContractTest, cancel_frees_the_response_cancels_sessions_and_keeps_subscriptions)
{
    const ProxyAddress third{ consumer_address(mStub.address(), "StubContractConsumerC") };
    ASSERT_TRUE(mStub.add_notification_listener(RESPONSES[0], mSecond));
    ASSERT_TRUE(mStub.begin_request(mFirst, 41));
    const SessionID session = mStub.unblock_current_request();
    ASSERT_TRUE(mStub.begin_request(third, 7));
    mStub.fail_request(ResultType::RequestCanceled);
    EXPECT_FALSE(mStub.is_busy(RESPONSES[0]));
    EXPECT_TRUE(mStub.exist(RESPONSES[0], mSecond));
    EXPECT_EQ(1u, mStub.attempts(REQUESTS[0], ResultType::RequestCanceled));
    EXPECT_FALSE(mStub.prepare_response(session));
    EXPECT_TRUE(mStub.respond().is_empty());
    EXPECT_TRUE(mStub.begin_request(mFirst, 42));
}

TEST_F(StubBaseContractTest, cancel_all_requests_cancels_sessions_without_waiting_listeners)
{
    ASSERT_TRUE(mStub.begin_request(mFirst, 41));
    const SessionID session = mStub.unblock_current_request();
    ASSERT_TRUE(mStub.begin_request(mSecond, 43, RESPONSES[1]));
    const SessionID other = mStub.unblock_current_request();
    mStub.cancel_all_requests();
    EXPECT_EQ(1u, mStub.attempts(REQUESTS[0], ResultType::RequestCanceled));
    EXPECT_EQ(1u, mStub.attempts(REQUESTS[1], ResultType::RequestCanceled));
    EXPECT_FALSE(mStub.prepare_response(session));
    EXPECT_FALSE(mStub.prepare_response(other));
}

TEST_F(StubBaseContractTest, response_error_fails_waiting_callers_and_spares_subscribers)
{
    ASSERT_TRUE(mStub.add_notification_listener(RESPONSES[0], mSecond));
    ASSERT_TRUE(mStub.begin_request(mFirst, 41));
    ContractStub::StubListenerList listeners;
    ASSERT_EQ(2u, mStub.find_listeners(RESPONSES[0], listeners));
    mStub.send_error_notification(listeners, LocalResponseEvent(mFirst, ResultType::DataInvalid, RESPONSES[0], SEQUENCE_NUMBER_NOTIFY, 0));
    EXPECT_EQ(1u, mStub.attempts(REQUESTS[0], ResultType::RequestError));
    EXPECT_FALSE(mStub.is_busy(RESPONSES[0]));
    EXPECT_TRUE(mStub.exist(RESPONSES[0], mSecond));
}

TEST_F(StubBaseContractTest, disconnected_caller_keeps_the_request_busy_and_only_subscribers_get_the_response)
{
    const ProxyAddress third{ consumer_address(mStub.address(), "StubContractConsumerC") };
    ASSERT_TRUE(mStub.add_notification_listener(RESPONSES[0], mFirst));
    ASSERT_TRUE(mStub.add_notification_listener(RESPONSES[0], mSecond));
    ASSERT_TRUE(mStub.begin_request(mFirst, 41));
    EXPECT_TRUE(mStub.consumer_connected(mFirst, ServiceConnectionState::Disconnected));
    EXPECT_FALSE(mStub.exist(RESPONSES[0], mFirst));
    EXPECT_TRUE(mStub.is_busy(RESPONSES[0]));
    EXPECT_FALSE(mStub.begin_request(third, 7));
    const auto targets = mStub.respond();
    ASSERT_EQ(1u, targets.size());
    EXPECT_TRUE(targets[0].proxy_matches(mSecond));
    EXPECT_EQ(SEQUENCE_NUMBER_NOTIFY, targets[0].mSequenceNr);
    EXPECT_FALSE(mStub.is_busy(RESPONSES[0]));
}

TEST_F(StubBaseContractTest, cancelled_sessions_of_disconnected_callers_send_nothing_and_become_invalid)
{
    ASSERT_TRUE(mStub.add_notification_listener(RESPONSES[0], mSecond));
    ASSERT_TRUE(mStub.begin_request(mFirst, 41));
    const SessionID session = mStub.unblock_current_request();
    ASSERT_TRUE(mStub.begin_request(mSecond, 43, RESPONSES[1]));
    const SessionID gone = mStub.unblock_current_request();
    mStub.clear_all_listeners(mFirst);
    mStub.clear_all_listeners(mSecond);
    mStub.cancel_all_requests();
    EXPECT_EQ(0u, mStub.attempts(REQUESTS[0], ResultType::RequestCanceled));
    EXPECT_EQ(0u, mStub.attempts(REQUESTS[1], ResultType::RequestCanceled));
    EXPECT_FALSE(mStub.prepare_response(gone));
    EXPECT_FALSE(mStub.prepare_response(session));
}

TEST_F(StubBaseContractTest, disconnected_session_response_reaches_only_the_subscribers)
{
    ASSERT_TRUE(mStub.add_notification_listener(RESPONSES[0], mSecond));
    ASSERT_TRUE(mStub.begin_request(mFirst, 41));
    const SessionID session = mStub.unblock_current_request();
    mStub.clear_all_listeners(mFirst);
    ASSERT_TRUE(mStub.prepare_response(session));
    const auto targets = mStub.respond();
    ASSERT_EQ(1u, targets.size());
    EXPECT_TRUE(targets[0].proxy_matches(mSecond));
    EXPECT_FALSE(mStub.is_busy(RESPONSES[0]));
}

TEST_F(StubBaseContractTest, session_ids_skip_the_invalid_id_and_open_sessions_after_a_wrap)
{
    ASSERT_TRUE(mStub.begin_request(mFirst, 41));
    mStub.mSessionId = 0u;
    const SessionID open = mStub.unblock_current_request();
    ASSERT_EQ(1u, open);
    ASSERT_TRUE(mStub.begin_request(mSecond, 42));
    mStub.mSessionId = std::numeric_limits<SessionID>::max() - 2u;
    const SessionID last = mStub.unblock_current_request();
    EXPECT_EQ(std::numeric_limits<SessionID>::max() - 1u, last);
    ASSERT_TRUE(mStub.begin_request(mFirst, 43));
    const SessionID wrapped = mStub.unblock_current_request();
    EXPECT_EQ(0u, wrapped);
    ASSERT_TRUE(mStub.begin_request(mSecond, 44));
    const SessionID next = mStub.unblock_current_request();
    EXPECT_EQ(2u, next);
    ASSERT_TRUE(mStub.prepare_response(open));
    const auto targets = mStub.respond();
    ASSERT_EQ(1u, targets.size());
    EXPECT_EQ(41u, targets[0].mSequenceNr);
}
