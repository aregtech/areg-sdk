#include "units/GUnitTest.hpp"
#include "units/EventTestSupport.hpp"

#include "areg/component/EventConsumer.hpp"

#include <atomic>
#include <future>
#include <memory>
#include <thread>
#include <vector>

struct LifecycleValueData
{
    uint32_t producer;
    uint32_t sequence;
};

struct LifecycleInternalData
{
    uint32_t sequence;
};

AREG_DECLARE_EVENT(LifecycleValueData, LifecycleValueEvent, LifecycleValueConsumer);
AREG_DECLARE_INTERNAL_EVENT(LifecycleInternalData, LifecycleInternalEvent, LifecycleInternalConsumer);

namespace
{
    class ValueListener final : public LifecycleValueConsumer
    {
    public:
        std::vector<LifecycleValueData> mValues;
        std::vector<bool> mRegistrations;
        std::function<void(const LifecycleValueData &)> mAction;

        void consumer_registered(bool registered) override
        {
            mRegistrations.push_back(registered);
        }

    private:
        void process_event(const LifecycleValueData & data) override
        {
            mValues.push_back(data);
            if (mAction)
            {
                mAction(data);
            }
        }
    };

    class RawListener final : public areg::EventConsumer
    {
    public:
        bool mAllowed{ true };
        uint32_t mPreprocessed{ 0u };
        uint32_t mProcessed{ 0u };

        bool preprocess_event(areg::Event &) override
        {
            ++mPreprocessed;
            return mAllowed;
        }

        void start_event_processing(areg::Event &) override
        {
            ++mProcessed;
        }
    };

    class InternalListener final : public LifecycleInternalConsumer
    {
    public:
        explicit InternalListener(std::vector<uint32_t> & order)
            : mOrder(order)
        {
        }

    private:
        void process_event(const LifecycleInternalData & data) override
        {
            mOrder.push_back(data.sequence);
            if (data.sequence < 3u)
            {
                EXPECT_TRUE(LifecycleInternalEvent::send_event(LifecycleInternalData{ data.sequence + 1u }));
            }
        }

        std::vector<uint32_t> & mOrder;
    };

    struct LifetimeToken
    {
        explicit LifetimeToken(std::atomic<uint32_t> & destroyed)
            : mDestroyed(destroyed)
        {
        }

        ~LifetimeToken()
        {
            mDestroyed.fetch_add(1u);
        }

        std::atomic<uint32_t> & mDestroyed;
    };

    struct LifetimeData
    {
        // JUSTIFICATION: the last queued payload owner releases the destruction counter token.
        std::shared_ptr<LifetimeToken> token;
    };
}

AREG_DECLARE_EVENT(LifetimeData, LifecycleLifetimeEvent, LifecycleLifetimeConsumer);

TEST(EventDispatchContractTest, preprocessing_veto_and_explicit_consumer_override)
{
    RawListener stored;
    RawListener overrideListener;
    areg::Event event(areg::EventType::EventCustomExternal);
    event.set_event_consumer(&stored);
    event.dispatch_self(nullptr);
    EXPECT_EQ(stored.mPreprocessed, 1u);
    EXPECT_EQ(stored.mProcessed, 1u);

    stored.mAllowed = false;
    event.dispatch_self(nullptr);
    EXPECT_EQ(stored.mPreprocessed, 2u);
    EXPECT_EQ(stored.mProcessed, 1u);

    event.dispatch_self(&overrideListener);
    EXPECT_EQ(overrideListener.mPreprocessed, 1u);
    EXPECT_EQ(overrideListener.mProcessed, 1u);
    EXPECT_EQ(stored.mPreprocessed, 2u);
    event.set_event_consumer(nullptr);
    event.dispatch_self(nullptr);
    EXPECT_EQ(stored.mProcessed, 1u);
    EXPECT_EQ(overrideListener.mProcessed, 1u);
}

TEST(DispatcherLifecycleTest, stopped_thread_rejects_posts_and_listener_registration)
{
    ValueListener listener;
    EventTestDispatcher dispatcher("LifecycleStopped");
    EXPECT_FALSE(LifecycleValueEvent::add_listener(listener, dispatcher));
    EXPECT_FALSE(LifecycleValueEvent::remove_listener(listener, dispatcher));
    EXPECT_FALSE(LifecycleValueEvent::send_event(LifecycleValueData{ 0u, 1u }, dispatcher));
    LifecycleValueEvent event(LifecycleValueData{ 0u, 2u });
    EXPECT_FALSE(event.register_for_thread(&dispatcher));
    EXPECT_FALSE(event.register_for_thread(static_cast<areg::DispatcherThread *>(nullptr)));
    EXPECT_FALSE(event.deliver_event());
    EXPECT_TRUE(event.is_valid());
    EXPECT_TRUE(listener.mValues.empty());
    EXPECT_TRUE(listener.mRegistrations.empty());
}

TEST(DispatcherLifecycleTest, batch_before_start_returns_the_entire_refused_count)
{
    EventTestDispatcher dispatcher("LifecycleBatchBeforeStart");
    areg::Event batch[3]
    {
          areg::Event(areg::EventType::EventCustomExternal, areg::EventPriority::CriticalPrio)
        , areg::Event(areg::EventType::EventCustomExternal, areg::EventPriority::HighPrio)
        , areg::Event(areg::EventType::EventCustomExternal)
    };
    EXPECT_EQ(dispatcher.queue_events(batch, 3u), 3u);
    for (const areg::Event & event : batch)
    {
        EXPECT_TRUE(event.is_valid());
    }
    areg::Event output[3];
    EXPECT_EQ(dispatcher.pop_events(output, 3u), 0u);
}

TEST(DispatcherLifecycleTest, duplicate_registration_and_remove_consumer_count_are_exact)
{
    ValueListener listener;
    ValueListener survivor;
    EventTestDispatcher dispatcher("LifecycleRegistrations");
    EventTestRun run(dispatcher);
    ASSERT_TRUE(dispatcher.start_ready());
    ASSERT_TRUE(LifecycleValueEvent::add_listener(listener, dispatcher));
    EXPECT_FALSE(LifecycleValueEvent::add_listener(listener, dispatcher));
    ASSERT_TRUE(dispatcher.register_event_consumer(LifecycleInternalEvent::CLASS_ID, listener));
    ASSERT_TRUE(LifecycleValueEvent::add_listener(survivor, dispatcher));
    EXPECT_EQ(listener.mRegistrations, (std::vector<bool>{ true, true }));

    EXPECT_EQ(dispatcher.remove_consumer(listener), 2);
    EXPECT_EQ(dispatcher.remove_consumer(listener), 0);
    EXPECT_EQ(listener.mRegistrations, (std::vector<bool>{ true, true, false, false }));
    EXPECT_FALSE(dispatcher.has_registered_consumer(LifecycleInternalEvent::CLASS_ID));
    EXPECT_TRUE(dispatcher.has_registered_consumer(LifecycleValueEvent::CLASS_ID));
    ASSERT_TRUE(LifecycleValueEvent::send_event(LifecycleValueData{ 0u, 7u }, dispatcher));
    ASSERT_TRUE(dispatcher.barrier());
    EXPECT_TRUE(listener.mValues.empty());
    ASSERT_EQ(survivor.mValues.size(), 1u);
    EXPECT_EQ(survivor.mValues[0].sequence, 7u);
    EXPECT_EQ(dispatcher.remove_consumer(survivor), 1);
    EXPECT_FALSE(dispatcher.has_registered_consumer(LifecycleValueEvent::CLASS_ID));
}

TEST(DispatcherLifecycleTest, unregister_absent_listener_reports_failure)
{
    ValueListener registered;
    ValueListener absent;
    EventTestDispatcher dispatcher("LifecycleAbsent");
    EventTestRun run(dispatcher);
    ASSERT_TRUE(dispatcher.start_ready());
    ASSERT_TRUE(LifecycleValueEvent::add_listener(registered, dispatcher));
    EXPECT_FALSE(LifecycleValueEvent::remove_listener(absent, dispatcher));
    EXPECT_FALSE(dispatcher.unregister_event_consumer(LifecycleInternalEvent::CLASS_ID, registered));
    EXPECT_TRUE(absent.mRegistrations.empty());
    EXPECT_TRUE(dispatcher.has_registered_consumer(LifecycleValueEvent::CLASS_ID));
    ASSERT_TRUE(LifecycleValueEvent::send_event(LifecycleValueData{ 0u, 1u }, dispatcher));
    ASSERT_TRUE(dispatcher.barrier());
    EXPECT_EQ(registered.mValues.size(), 1u);
    EXPECT_TRUE(absent.mValues.empty());

    EXPECT_TRUE(LifecycleValueEvent::remove_listener(registered, dispatcher));
    EXPECT_FALSE(LifecycleValueEvent::remove_listener(registered, dispatcher));
    EXPECT_EQ(registered.mRegistrations, (std::vector<bool>{ true, false }));
    EXPECT_FALSE(dispatcher.has_registered_consumer(LifecycleValueEvent::CLASS_ID));
}

TEST(DispatcherLifecycleTest, event_instance_and_numeric_registration_use_target_dispatcher)
{
    ValueListener listener;
    EventTestDispatcher dispatcher("LifecycleInstance");
    EventTestRun run(dispatcher);
    ASSERT_TRUE(dispatcher.start_ready());
    const UniqueNumber number{ static_cast<UniqueNumber>(dispatcher.address()) };
    EXPECT_EQ(&areg::DispatcherThread::dispatcher_thread(number), &dispatcher);
    EXPECT_EQ(&areg::DispatcherThread::dispatcher_thread(dispatcher.address()), &dispatcher);
    EXPECT_EQ(&dispatcher.event_dispatcher().dispatcher_thread(), &dispatcher);

    LifecycleValueEvent event(LifecycleValueData{ 0u, 9u });
    ASSERT_TRUE(event.register_for_thread(number));
    EXPECT_FALSE(event.is_event_registered());
    ASSERT_TRUE(event.add_event_listener(listener));
    EXPECT_TRUE(event.is_event_registered());
    EXPECT_FALSE(areg::Event::add_listener(LifecycleValueEvent::CLASS_ID, listener, number));
    ASSERT_TRUE(event.deliver_event());
    ASSERT_TRUE(dispatcher.barrier());
    ASSERT_EQ(listener.mValues.size(), 1u);
    EXPECT_EQ(listener.mValues[0].sequence, 9u);

    LifecycleValueEvent registration(LifecycleValueData{ 0u, 0u });
    ASSERT_TRUE(registration.register_for_thread(&dispatcher));
    EXPECT_TRUE(registration.remove_event_listener(listener));
    EXPECT_FALSE(registration.is_event_registered());
    EXPECT_FALSE(areg::Event::remove_listener(LifecycleValueEvent::CLASS_ID, listener, number));
    EXPECT_EQ(listener.mRegistrations, (std::vector<bool>{ true, false }));
}

TEST(DispatcherLifecycleTest, self_post_uses_current_consumer_lookup_and_targeted_delivery)
{
    ValueListener listener;
    EventTestDispatcher dispatcher("LifecycleSelfPost");
    EventTestRun run(dispatcher);
    ASSERT_TRUE(dispatcher.start_ready());
    ASSERT_TRUE(LifecycleValueEvent::add_listener(listener, dispatcher));
    ASSERT_TRUE(dispatcher.invoke([&]()
        {
            EXPECT_EQ(&areg::DispatcherThread::current_dispatcher_thread(), &dispatcher);
            EXPECT_EQ(&areg::DispatcherThread::dispatcher_thread(areg::IGNORE_VALUE), &dispatcher);
            EXPECT_EQ(areg::DispatcherThread::find_consumer_thread(LifecycleValueEvent::CLASS_ID), &dispatcher);
            EXPECT_EQ(areg::DispatcherThread::find_consumer_thread(0x613AFFFFu), nullptr);
            EXPECT_TRUE(LifecycleValueEvent::send_event(LifecycleValueData{ 0u, 1u }));
            EXPECT_TRUE(LifecycleValueEvent::send_event(LifecycleValueData{ 0u, 2u }, listener));
        }));
    ASSERT_TRUE(dispatcher.barrier());
    ASSERT_EQ(listener.mValues.size(), 2u);
    EXPECT_EQ(listener.mValues[0].sequence, 1u);
    EXPECT_EQ(listener.mValues[1].sequence, 2u);
}

TEST(DispatcherLifecycleTest, foreign_sender_discovers_the_only_registered_consumer_thread)
{
    ValueListener listener;
    EventTestDispatcher dispatcher("LifecycleForeignLookup");
    EventTestRun run(dispatcher);
    ASSERT_TRUE(dispatcher.start_ready());
    ASSERT_TRUE(LifecycleValueEvent::add_listener(listener, dispatcher.name()));
    ASSERT_TRUE(LifecycleValueEvent::send_event(LifecycleValueData{ 1u, 17u }));
    ASSERT_TRUE(dispatcher.barrier());
    ASSERT_EQ(listener.mValues.size(), 1u);
    EXPECT_EQ(listener.mValues[0].sequence, 17u);
    EXPECT_TRUE(LifecycleValueEvent::remove_listener(listener, dispatcher.name()));
    EXPECT_FALSE(LifecycleValueEvent::send_event(LifecycleValueData{ 1u, 18u }));
}

TEST(DispatcherLifecycleTest, listener_added_during_a_callback_starts_with_the_next_broadcast)
{
    ValueListener first;
    ValueListener added;
    EventTestDispatcher dispatcher("LifecycleAddDuringCallback");
    EventTestRun run(dispatcher);
    first.mAction = [&](const LifecycleValueData & data)
        {
            if (data.sequence == 1u)
            {
                EXPECT_TRUE(LifecycleValueEvent::add_listener(added, dispatcher));
            }
        };
    ASSERT_TRUE(dispatcher.start_ready());
    ASSERT_TRUE(LifecycleValueEvent::add_listener(first, dispatcher));
    ASSERT_TRUE(LifecycleValueEvent::send_event(LifecycleValueData{ 0u, 1u }, dispatcher));
    ASSERT_TRUE(dispatcher.barrier());
    EXPECT_EQ(first.mValues.size(), 1u);
    EXPECT_TRUE(added.mValues.empty());
    ASSERT_TRUE(LifecycleValueEvent::send_event(LifecycleValueData{ 0u, 2u }, dispatcher));
    ASSERT_TRUE(dispatcher.barrier());
    EXPECT_EQ(first.mValues.size(), 2u);
    ASSERT_EQ(added.mValues.size(), 1u);
    EXPECT_EQ(added.mValues[0].sequence, 2u);
}

TEST(DispatcherLifecycleTest, remove_consumer_inside_callback_clears_all_its_event_classes)
{
    ValueListener listener;
    EventTestDispatcher dispatcher("LifecycleRemoveDuringCallback");
    EventTestRun run(dispatcher);
    listener.mAction = [&](const LifecycleValueData &)
        {
            EXPECT_EQ(dispatcher.remove_consumer(listener), 2);
        };
    ASSERT_TRUE(dispatcher.start_ready());
    ASSERT_TRUE(LifecycleValueEvent::add_listener(listener, dispatcher));
    ASSERT_TRUE(dispatcher.register_event_consumer(LifecycleInternalEvent::CLASS_ID, listener));
    ASSERT_TRUE(LifecycleValueEvent::send_event(LifecycleValueData{ 0u, 1u }, dispatcher));
    ASSERT_TRUE(dispatcher.barrier());
    EXPECT_FALSE(dispatcher.has_registered_consumer(LifecycleValueEvent::CLASS_ID));
    EXPECT_FALSE(dispatcher.has_registered_consumer(LifecycleInternalEvent::CLASS_ID));
    ASSERT_TRUE(LifecycleValueEvent::send_event(LifecycleValueData{ 0u, 2u }, dispatcher));
    ASSERT_TRUE(dispatcher.barrier());
    EXPECT_EQ(listener.mValues.size(), 1u);
    EXPECT_EQ(listener.mRegistrations, (std::vector<bool>{ true, true, false, false }));
}

TEST(DispatcherLifecycleTest, two_dispatchers_exchange_a_targeted_request_and_acknowledgement)
{
    ValueListener request;
    ValueListener acknowledgement;
    std::promise<void> acknowledged;
    EventTestDispatcher sender("LifecycleRoundTripSender");
    EventTestDispatcher receiver("LifecycleRoundTripReceiver");
    EventTestRun senderRun(sender);
    EventTestRun receiverRun(receiver);
    request.mAction = [&](const LifecycleValueData & data)
        {
            EXPECT_EQ(&areg::DispatcherThread::current_dispatcher_thread(), &receiver);
            EXPECT_TRUE(LifecycleValueEvent::send_event(data, acknowledgement, sender));
        };
    acknowledgement.mAction = [&](const LifecycleValueData & data)
        {
            EXPECT_EQ(&areg::DispatcherThread::current_dispatcher_thread(), &sender);
            EXPECT_EQ(data.sequence, 42u);
            acknowledged.set_value();
        };
    ASSERT_TRUE(sender.start_ready());
    ASSERT_TRUE(receiver.start_ready());
    ASSERT_TRUE(sender.invoke([&]()
        {
            EXPECT_TRUE(LifecycleValueEvent::send_event(LifecycleValueData{ 1u, 42u }, request, receiver));
        }));
    ASSERT_EQ(acknowledged.get_future().wait_for(EVENT_TEST_WAIT), std::future_status::ready);
    ASSERT_TRUE(sender.barrier());
    ASSERT_TRUE(receiver.barrier());
    EXPECT_EQ(request.mValues.size(), 1u);
    EXPECT_EQ(acknowledgement.mValues.size(), 1u);
}

TEST(DispatcherLifecycleTest, priority_overtakes_queued_normals_with_fifo_inside_each_lane)
{
    ValueListener listener;
    EventTestGate gate;
    std::promise<void> entered;
    EventTestDispatcher dispatcher("LifecyclePriority");
    EventTestRun run(dispatcher, &gate);
    ASSERT_TRUE(dispatcher.start_ready());
    ASSERT_TRUE(LifecycleValueEvent::add_listener(listener, dispatcher));
    ASSERT_TRUE(dispatcher.post_action([&]()
        {
            entered.set_value();
            EXPECT_TRUE(gate.wait());
        }));
    ASSERT_EQ(entered.get_future().wait_for(EVENT_TEST_WAIT), std::future_status::ready);

    using Priority = areg::EventPriority;
    ASSERT_TRUE(LifecycleValueEvent::send_event(LifecycleValueData{ 0u, 1u }, dispatcher, Priority::NormalPrio));
    ASSERT_TRUE(LifecycleValueEvent::send_event(LifecycleValueData{ 0u, 2u }, dispatcher, Priority::HighPrio));
    ASSERT_TRUE(LifecycleValueEvent::send_event(LifecycleValueData{ 0u, 3u }, dispatcher, Priority::CriticalPrio));
    ASSERT_TRUE(LifecycleValueEvent::send_event(LifecycleValueData{ 0u, 4u }, dispatcher, Priority::HighPrio));
    ASSERT_TRUE(LifecycleValueEvent::send_event(LifecycleValueData{ 0u, 5u }, dispatcher, Priority::CriticalPrio));
    ASSERT_TRUE(LifecycleValueEvent::send_event(LifecycleValueData{ 0u, 6u }, dispatcher, Priority::NormalPrio));
    gate.open();
    ASSERT_TRUE(dispatcher.barrier());
    std::vector<uint32_t> order;
    for (const auto & data : listener.mValues)
    {
        order.push_back(data.sequence);
    }
    EXPECT_EQ(order, (std::vector<uint32_t>{ 3u, 5u, 2u, 4u, 1u, 6u }));
}

TEST(DispatcherLifecycleTest, internal_chain_finishes_before_the_next_external_event)
{
    std::vector<uint32_t> order;
    InternalListener internal(order);
    ValueListener external;
    external.mAction = [&](const LifecycleValueData &) { order.push_back(4u); };
    EventTestGate gate;
    std::promise<void> entered;
    EventTestDispatcher dispatcher("LifecycleInternalOrder");
    EventTestRun run(dispatcher, &gate);
    ASSERT_TRUE(dispatcher.start_ready());
    ASSERT_TRUE(LifecycleInternalEvent::add_listener(internal, dispatcher));
    ASSERT_TRUE(LifecycleValueEvent::add_listener(external, dispatcher));
    ASSERT_TRUE(dispatcher.post_action([&]()
        {
            entered.set_value();
            EXPECT_TRUE(gate.wait());
            EXPECT_TRUE(LifecycleInternalEvent::send_event(LifecycleInternalData{ 1u }));
        }));
    ASSERT_EQ(entered.get_future().wait_for(EVENT_TEST_WAIT), std::future_status::ready);
    ASSERT_TRUE(LifecycleValueEvent::send_event(LifecycleValueData{ 0u, 4u }, dispatcher));
    gate.open();
    ASSERT_TRUE(dispatcher.barrier());
    EXPECT_EQ(order, (std::vector<uint32_t>{ 1u, 2u, 3u, 4u }));
}

TEST(DispatcherLifecycleTest, foreign_internal_event_wakes_an_idle_dispatcher)
{
    std::vector<uint32_t> order;
    InternalListener listener(order);
    EventTestDispatcher dispatcher("LifecycleForeignInternal");
    EventTestRun run(dispatcher);
    ASSERT_TRUE(dispatcher.start_ready());
    ASSERT_TRUE(LifecycleInternalEvent::add_listener(listener, dispatcher));
    ASSERT_TRUE(LifecycleInternalEvent::send_event(LifecycleInternalData{ 3u }, dispatcher));
    ASSERT_TRUE(dispatcher.barrier());
    EXPECT_EQ(order, (std::vector<uint32_t>{ 3u }));
}

TEST(DispatcherLifecycleTest, several_producers_deliver_every_sequence_once_in_producer_order)
{
    constexpr uint32_t PRODUCERS{ 4u };
    constexpr uint32_t COUNT{ 512u };
    ValueListener listener;
    std::atomic<uint32_t> failures{ 0u };
    EventTestDispatcher dispatcher("LifecycleProducerOrder", 4096u);
    EventTestRun run(dispatcher);
    ASSERT_TRUE(dispatcher.start_ready());
    ASSERT_TRUE(LifecycleValueEvent::add_listener(listener, dispatcher));
    std::vector<std::thread> producers;
    for (uint32_t producer = 0u; producer < PRODUCERS; ++producer)
    {
        producers.emplace_back([&, producer]()
            {
                for (uint32_t sequence = 0u; sequence < COUNT; ++sequence)
                {
                    if (!LifecycleValueEvent::send_event(LifecycleValueData{ producer, sequence }, dispatcher))
                    {
                        failures.fetch_add(1u);
                    }
                }
            });
    }
    for (std::thread & producer : producers)
    {
        producer.join();
    }
    ASSERT_TRUE(dispatcher.barrier());
    EXPECT_EQ(failures.load(), 0u);
    ASSERT_EQ(listener.mValues.size(), PRODUCERS * COUNT);
    std::vector<uint32_t> next(PRODUCERS, 0u);
    for (const auto & value : listener.mValues)
    {
        ASSERT_LT(value.producer, PRODUCERS);
        EXPECT_EQ(value.sequence, next[value.producer]++);
    }
    EXPECT_EQ(next, std::vector<uint32_t>(PRODUCERS, COUNT));
}

TEST(DispatcherLifecycleTest, drained_exit_delivers_backlog_rejects_new_work_and_restarts_cleanly)
{
    ValueListener listener;
    EventTestGate gate;
    std::promise<void> entered;
    EventTestDispatcher dispatcher("LifecycleDrainRestart");
    EventTestRun run(dispatcher, &gate);
    ASSERT_TRUE(dispatcher.start_ready());
    ASSERT_TRUE(LifecycleValueEvent::add_listener(listener, dispatcher));
    ASSERT_TRUE(dispatcher.post_action([&]()
        {
            entered.set_value();
            EXPECT_TRUE(gate.wait());
        }));
    ASSERT_EQ(entered.get_future().wait_for(EVENT_TEST_WAIT), std::future_status::ready);
    for (uint32_t i = 0u; i < 32u; ++i)
    {
        ASSERT_TRUE(LifecycleValueEvent::send_event(LifecycleValueData{ 0u, i }, dispatcher));
    }
    dispatcher.trigger_exit_drained();
    EXPECT_FALSE(LifecycleValueEvent::send_event(LifecycleValueData{ 0u, 99u }, dispatcher));
    gate.open();
    ASSERT_TRUE(dispatcher.wait_completion(EVENT_TEST_WAIT_MS));
    EXPECT_FALSE(dispatcher.is_ready());
    ASSERT_EQ(listener.mValues.size(), 32u);
    for (uint32_t i = 0u; i < 32u; ++i)
    {
        EXPECT_EQ(listener.mValues[i].sequence, i);
    }
    EXPECT_FALSE(dispatcher.has_registered_consumer(LifecycleValueEvent::CLASS_ID));
    EXPECT_EQ(listener.mRegistrations, (std::vector<bool>{ true, false }));
    dispatcher.shutdown(areg::WAIT_INFINITE);
    ASSERT_TRUE(dispatcher.start_ready());
    ASSERT_TRUE(LifecycleValueEvent::add_listener(listener, dispatcher));
    ASSERT_TRUE(LifecycleValueEvent::send_event(LifecycleValueData{ 0u, 100u }, dispatcher));
    ASSERT_TRUE(dispatcher.barrier());
    ASSERT_EQ(listener.mValues.size(), 33u);
    EXPECT_EQ(listener.mValues.back().sequence, 100u);
}

TEST(DispatcherLifecycleTest, immediate_exit_releases_every_queued_nontrivial_payload)
{
    std::atomic<uint32_t> destroyed{ 0u };
    EventTestGate gate;
    std::promise<void> entered;
    EventTestDispatcher dispatcher("LifecycleImmediateCleanup");
    EventTestRun run(dispatcher, &gate);
    ASSERT_TRUE(dispatcher.start_ready());
    ASSERT_TRUE(dispatcher.post_action([&]()
        {
            entered.set_value();
            EXPECT_TRUE(gate.wait());
        }));
    ASSERT_EQ(entered.get_future().wait_for(EVENT_TEST_WAIT), std::future_status::ready);
    for (uint32_t i = 0u; i < 32u; ++i)
    {
        ASSERT_TRUE(LifecycleLifetimeEvent::send_event(LifetimeData{ std::make_shared<LifetimeToken>(destroyed) }, dispatcher));
    }
    EXPECT_EQ(destroyed.load(), 0u);
    dispatcher.trigger_exit();
    gate.open();
    ASSERT_TRUE(dispatcher.wait_completion(EVENT_TEST_WAIT_MS));
    EXPECT_EQ(destroyed.load(), 32u);
    EXPECT_FALSE(dispatcher.is_ready());
    EXPECT_FALSE(LifecycleLifetimeEvent::send_event(LifetimeData{ std::make_shared<LifetimeToken>(destroyed) }, dispatcher));
    EXPECT_EQ(destroyed.load(), 33u);
}

TEST(DispatcherLifecycleTest, pulse_exit_wakes_idle_thread_and_unregisters_its_listeners)
{
    ValueListener listener;
    EventTestDispatcher dispatcher("LifecyclePulseExit");
    EventTestRun run(dispatcher);
    ASSERT_TRUE(dispatcher.start_ready());
    ASSERT_TRUE(LifecycleValueEvent::add_listener(listener, dispatcher));
    EXPECT_TRUE(dispatcher.pulse_exit());
    ASSERT_TRUE(dispatcher.wait_completion(EVENT_TEST_WAIT_MS));
    EXPECT_FALSE(dispatcher.is_ready());
    EXPECT_EQ(listener.mRegistrations, (std::vector<bool>{ true, false }));
    EXPECT_TRUE(listener.mValues.empty());
}

TEST(DispatcherLifecycleTest, null_dispatcher_listener_operations_are_safe_failures)
{
    RawListener listener;
    areg::DispatcherThread & nullDispatcher{ areg::DispatcherThread::current_dispatcher_thread() };
    EXPECT_FALSE(nullDispatcher.is_running());
    EXPECT_FALSE(nullDispatcher.register_event_consumer(0x613A1111u, listener));
    EXPECT_FALSE(nullDispatcher.unregister_event_consumer(0x613A1111u, listener));
    EXPECT_EQ(nullDispatcher.remove_consumer(listener), 0);
    EXPECT_FALSE(nullDispatcher.has_registered_consumer(0x613A1111u));
    EXPECT_FALSE(areg::Event::add_listener(0x613A1111u, listener, areg::IGNORE_VALUE));
    EXPECT_FALSE(areg::Event::remove_listener(0x613A1111u, listener, areg::IGNORE_VALUE));
}

TEST(DispatcherLifecycleTest, release_shared_accepts_null_and_deletes_an_unshared_stopped_dispatcher)
{
    class DestructionDispatcher final : public EventTestDispatcher
    {
    public:
        explicit DestructionDispatcher(uint32_t & destroyed)
            : EventTestDispatcher("LifecycleUnsharedRelease")
            , mDestroyed(destroyed)
        {
        }

        ~DestructionDispatcher() override
        {
            ++mDestroyed;
        }

    private:
        uint32_t & mDestroyed;
    };

    uint32_t destroyed{ 0u };
    areg::DispatcherThread::release_shared(nullptr);
    EXPECT_EQ(destroyed, 0u);
    auto dispatcher{ std::make_unique<DestructionDispatcher>(destroyed) };
    areg::DispatcherThread::release_shared(dispatcher.release());
    EXPECT_EQ(destroyed, 1u);
}

TEST(DispatcherLifecycleTest, the_null_dispatcher_of_a_plain_thread_refuses_every_operation)
{
    areg::DispatcherThread & none{ areg::DispatcherThread::current_dispatcher_thread() };
    EXPECT_FALSE(none.is_valid());
    EXPECT_FALSE(none.start());
    RawListener listener;
    EXPECT_FALSE(none.register_event_consumer(1u, listener));
    EXPECT_FALSE(none.has_registered_consumer(1u));
    EXPECT_FALSE(none.unregister_event_consumer(1u, listener));
    EXPECT_EQ(none.remove_consumer(listener), 0);
    EXPECT_EQ(&areg::DispatcherThread::dispatcher_thread(areg::ThreadAddress::invalid_thread_address()), &none);
}

TEST(DispatcherLifecycleTest, exit_requested_after_start_returns_ends_a_dispatcher_not_yet_in_its_loop)
{
    class HeldStartDispatcher final : public EventTestDispatcher
    {
    public:
        HeldStartDispatcher(EventTestGate & gate, std::promise<void> & entered)
            : EventTestDispatcher("LifecycleHeldStart")
            , mGate(gate)
            , mEntered(entered)
        {
        }

        bool start_dispatcher() override
        {
            mEntered.set_value();
            EXPECT_TRUE(mGate.wait());
            return EventTestDispatcher::start_dispatcher();
        }

    private:
        EventTestGate &         mGate;
        std::promise<void> &    mEntered;
    };

    EventTestGate gate;
    std::promise<void> entered;
    HeldStartDispatcher dispatcher(gate, entered);
    ASSERT_TRUE(dispatcher.start(EVENT_TEST_WAIT_MS));
    ASSERT_EQ(entered.get_future().wait_for(EVENT_TEST_WAIT), std::future_status::ready);

    dispatcher.trigger_exit();
    gate.open();
    const bool ended{ dispatcher.wait_completion(EVENT_TEST_WAIT_MS) };
    EXPECT_TRUE(ended);
    if (!ended)
    {
        dispatcher.trigger_exit();
    }

    dispatcher.shutdown(areg::WAIT_INFINITE);
    EXPECT_FALSE(dispatcher.is_running());
}
