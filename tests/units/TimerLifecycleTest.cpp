#include "units/GUnitTest.hpp"
#include "units/EventTestSupport.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/component/Timer.hpp"
#include "areg/component/TimerConsumer.hpp"

#include <condition_variable>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace
{
    constexpr uint32_t NEVER_MS{ 3'600'000u };

    class TimerListener final : public areg::TimerConsumer
    {
    public:
        std::function<void(areg::Timer &, uint32_t)> mAction;

        bool wait_count(areg::Timer & timer, uint32_t expected)
        {
            std::unique_lock<std::mutex> guard(mMutex);
            return mChanged.wait_for(guard, EVENT_TEST_WAIT, [&]()
                {
                    return mThreads[&timer].size() >= expected;
                });
        }

        uint32_t count(areg::Timer & timer)
        {
            std::lock_guard<std::mutex> guard(mMutex);
            return static_cast<uint32_t>(mThreads[&timer].size());
        }

        std::vector<std::thread::id> threads(areg::Timer & timer)
        {
            std::lock_guard<std::mutex> guard(mMutex);
            return mThreads[&timer];
        }

    private:
        void process_timer(areg::Timer & timer) override
        {
            uint32_t current{ 0u };
            {
                std::lock_guard<std::mutex> guard(mMutex);
                auto & threads{ mThreads[&timer] };
                threads.push_back(std::this_thread::get_id());
                current = static_cast<uint32_t>(threads.size());
            }
            if (mAction)
            {
                mAction(timer, current);
            }
            mChanged.notify_all();
        }

        std::mutex mMutex;
        std::condition_variable mChanged;
        std::map<areg::Timer *, std::vector<std::thread::id>> mThreads;
    };

    class TimerTestRun
    {
    public:
        explicit TimerTestRun(EventTestDispatcher & dispatcher, EventTestGate * gate = nullptr)
            : mDispatcher(dispatcher)
            , mGate(gate)
            , mTimers()
        {
        }

        ~TimerTestRun()
        {
            for (areg::Timer * timer : mTimers)
            {
                timer->stop_timer();
            }
            if (mGate != nullptr)
            {
                mGate->open();
            }
            mDispatcher.shutdown(areg::WAIT_INFINITE);
        }

        void watch(areg::Timer & timer)
        {
            mTimers.push_back(&timer);
        }

    private:
        EventTestDispatcher & mDispatcher;
        EventTestGate * mGate;
        std::vector<areg::Timer *> mTimers;
    };

    class TimerPostingDispatcher final : public EventTestDispatcher
    {
    public:
        explicit TimerPostingDispatcher(areg::TimerConsumer & consumer)
            : EventTestDispatcher("LifecycleThrottle", 1024u)
            , mConsumer(consumer)
            , mMutex()
            , mChanged()
            , mPosted(0u)
        {
        }

        bool post_event(areg::Event & event) override
        {
            const bool watched{ event.event_consumer() == &mConsumer };
            const bool posted{ EventTestDispatcher::post_event(event) };
            if (watched && posted)
            {
                {
                    std::lock_guard<std::mutex> guard(mMutex);
                    ++mPosted;
                }
                mChanged.notify_all();
            }
            return posted;
        }

        bool wait_posted(uint32_t count)
        {
            std::unique_lock<std::mutex> guard(mMutex);
            return mChanged.wait_for(guard, EVENT_TEST_WAIT, [&]() { return mPosted >= count; });
        }

    private:
        areg::TimerConsumer & mConsumer;
        std::mutex mMutex;
        std::condition_variable mChanged;
        uint32_t mPosted;
    };
}

class TimerLifecycleTest : public testing::Test
{
protected:
    void SetUp() override
    {
        areg::Application::setup(false, false, false, true, false);
    }

    void TearDown() override
    {
        areg::Application::release();
    }
};

TEST_F(TimerLifecycleTest, construction_attributes_and_idempotent_stop)
{
    TimerListener consumer;
    areg::Timer timer(consumer, "LifecycleInitial", 0u, -1, areg::EventPriority::HighPrio);
    EXPECT_EQ(&timer.consumer(), &consumer);
    EXPECT_TRUE(timer.name().starts_with("LifecycleInitial"));
    EXPECT_EQ(timer.timer_type(), areg::TimerBase::TimerType::PerThreadTimer);
    EXPECT_EQ(timer.priority(), areg::EventPriority::HighPrio);
    EXPECT_FALSE(timer.is_active());
    EXPECT_TRUE(timer.is_stopped());
    EXPECT_FALSE(timer.is_valid());
    EXPECT_EQ(timer.handle(), nullptr);
    timer.set_priority(areg::EventPriority::CriticalPrio);
    timer.set_timeout(42u);
    timer.set_event_count(3u);
    EXPECT_EQ(timer.priority(), areg::EventPriority::CriticalPrio);
    EXPECT_EQ(timer.timeout(), 42u);
    EXPECT_EQ(timer.event_count(), 3u);
    timer.stop_timer();
    timer.stop_timer();
    EXPECT_FALSE(timer.is_active());
    EXPECT_TRUE(timer.is_stopped());
    EXPECT_EQ(timer.event_count(), 0u);
    EXPECT_EQ(consumer.count(timer), 0u);
}

TEST_F(TimerLifecycleTest, stopped_target_and_non_dispatcher_caller_report_failure)
{
    TimerListener consumer;
    areg::Timer timer(consumer, "LifecycleInvalidTarget");
    EventTestDispatcher stopped("LifecycleTimerStopped");
    EXPECT_FALSE(timer.start_timer(5u, stopped, areg::Timer::ONE_TIME));
    EXPECT_FALSE(timer.is_active());
    EXPECT_EQ(timer.timeout(), 5u);
    EXPECT_EQ(timer.event_count(), areg::Timer::ONE_TIME);
    EXPECT_FALSE(timer.start_timer(5u, areg::Timer::ONE_TIME));
    EXPECT_FALSE(timer.is_active());
    EXPECT_EQ(consumer.count(timer), 0u);
    timer.stop_timer();
    EXPECT_TRUE(timer.is_stopped());
}

TEST_F(TimerLifecycleTest, zero_event_count_does_not_arm_an_idle_timer)
{
    TimerListener consumer;
    areg::Timer timer(consumer, "LifecycleZeroIdle");
    EventTestDispatcher dispatcher("LifecycleZeroIdleThread");
    TimerTestRun run(dispatcher);
    run.watch(timer);
    ASSERT_TRUE(dispatcher.start_ready());
    EXPECT_FALSE(timer.start_timer(NEVER_MS, dispatcher, 0u));
    EXPECT_FALSE(timer.is_active());
    EXPECT_EQ(timer.event_count(), 0u);
    ASSERT_TRUE(dispatcher.barrier());
    EXPECT_EQ(consumer.count(timer), 0u);
}

TEST_F(TimerLifecycleTest, invalid_timeout_reports_failure_and_leaves_the_timer_inactive)
{
    TimerListener consumer;
    areg::Timer timer(consumer, "LifecycleInvalidTimeout");
    EventTestDispatcher dispatcher("LifecycleInvalidTimeoutThread");
    TimerTestRun run(dispatcher);
    run.watch(timer);
    ASSERT_TRUE(dispatcher.start_ready());
    EXPECT_FALSE(timer.start_timer(areg::INVALID_TIMEOUT, dispatcher, areg::Timer::ONE_TIME));
    EXPECT_FALSE(timer.is_active());
    EXPECT_TRUE(timer.is_stopped());
    ASSERT_TRUE(dispatcher.barrier());
    EXPECT_EQ(consumer.count(timer), 0u);
    ASSERT_TRUE(timer.start_timer(1u, dispatcher, areg::Timer::ONE_TIME));
    ASSERT_TRUE(consumer.wait_count(timer, 1u));
    ASSERT_TRUE(dispatcher.barrier());
    EXPECT_EQ(consumer.count(timer), 1u);
}

TEST_F(TimerLifecycleTest, zero_count_restart_clears_active_state)
{
    TimerListener consumer;
    areg::Timer timer(consumer, "LifecycleZeroRestart");
    EventTestDispatcher dispatcher("LifecycleZeroRestartThread");
    TimerTestRun run(dispatcher);
    run.watch(timer);
    ASSERT_TRUE(dispatcher.start_ready());
    ASSERT_TRUE(timer.start_timer(NEVER_MS, dispatcher));
    ASSERT_TRUE(timer.is_active());
    EXPECT_FALSE(timer.start_timer(NEVER_MS, dispatcher, 0u));
    EXPECT_EQ(timer.event_count(), 0u);
    EXPECT_FALSE(timer.is_active());
    EXPECT_FALSE(timer.start_timer(NEVER_MS, dispatcher, 0u));
    EXPECT_FALSE(timer.is_active());

    ASSERT_TRUE(timer.start_timer(1u, dispatcher, areg::Timer::ONE_TIME));
    EXPECT_TRUE(timer.is_active());
    ASSERT_TRUE(consumer.wait_count(timer, 1u));
    ASSERT_TRUE(dispatcher.barrier());
    EXPECT_EQ(consumer.count(timer), 1u);
    timer.stop_timer();
    EXPECT_TRUE(timer.is_stopped());
}

TEST_F(TimerLifecycleTest, one_shot_fires_exactly_once_on_the_explicit_dispatcher)
{
    TimerListener consumer;
    areg::Timer timer(consumer, "LifecycleOneShot");
    EventTestDispatcher dispatcher("LifecycleOneShotThread");
    TimerTestRun run(dispatcher);
    run.watch(timer);
    ASSERT_TRUE(dispatcher.start_ready());
    std::thread::id expected;
    ASSERT_TRUE(dispatcher.invoke([&]() { expected = std::this_thread::get_id(); }));
    ASSERT_TRUE(timer.start_timer(1u, dispatcher, areg::Timer::ONE_TIME));
    ASSERT_TRUE(consumer.wait_count(timer, 1u));
    ASSERT_TRUE(dispatcher.barrier());
    EXPECT_EQ(consumer.threads(timer), (std::vector<std::thread::id>{ expected }));
    EXPECT_EQ(timer.event_count(), 0u);
    EXPECT_FALSE(timer.is_active());
    EXPECT_TRUE(timer.is_valid());
    timer.stop_timer();
    EXPECT_TRUE(timer.is_stopped());
}

TEST_F(TimerLifecycleTest, counted_periodic_timer_delivers_exact_count_and_can_restart)
{
    TimerListener consumer;
    areg::Timer timer(consumer, "LifecycleCounted");
    EventTestDispatcher dispatcher("LifecycleCountedThread");
    TimerTestRun run(dispatcher);
    run.watch(timer);
    ASSERT_TRUE(dispatcher.start_ready());
    ASSERT_TRUE(timer.start_timer(1u, dispatcher, 4u));
    ASSERT_TRUE(consumer.wait_count(timer, 4u));
    ASSERT_TRUE(dispatcher.barrier());
    EXPECT_EQ(consumer.count(timer), 4u);
    EXPECT_EQ(timer.event_count(), 0u);
    EXPECT_FALSE(timer.is_active());
    ASSERT_TRUE(timer.start_timer(1u, dispatcher, 3u));
    ASSERT_TRUE(consumer.wait_count(timer, 7u));
    ASSERT_TRUE(dispatcher.barrier());
    EXPECT_EQ(consumer.count(timer), 7u);
    EXPECT_EQ(timer.event_count(), 0u);
    EXPECT_FALSE(timer.is_active());
}

TEST_F(TimerLifecycleTest, implicit_target_start_and_rearm_inside_callback_use_owner_thread)
{
    TimerListener consumer;
    areg::Timer timer(consumer, "LifecycleCallbackRearm");
    EventTestDispatcher dispatcher("LifecycleCallbackThread");
    TimerTestRun run(dispatcher);
    run.watch(timer);
    ASSERT_TRUE(dispatcher.start_ready());
    consumer.mAction = [&](areg::Timer & expired, uint32_t count)
        {
            EXPECT_EQ(&expired, &timer);
            EXPECT_EQ(&areg::DispatcherThread::current_dispatcher_thread(), &dispatcher);
            if (count == 1u)
            {
                EXPECT_TRUE(expired.start_timer(1u, areg::Timer::ONE_TIME));
            }
        };
    ASSERT_TRUE(dispatcher.invoke([&]() { EXPECT_TRUE(timer.start_timer(1u, areg::Timer::ONE_TIME)); }));
    ASSERT_TRUE(consumer.wait_count(timer, 2u));
    ASSERT_TRUE(dispatcher.barrier());
    EXPECT_EQ(consumer.count(timer), 2u);
    EXPECT_FALSE(timer.is_active());
    EXPECT_EQ(timer.event_count(), 0u);
}

TEST_F(TimerLifecycleTest, active_restart_replaces_the_far_future_expiry)
{
    TimerListener consumer;
    areg::Timer timer(consumer, "LifecycleReplaceActive");
    EventTestDispatcher dispatcher("LifecycleReplaceActiveThread");
    TimerTestRun run(dispatcher);
    run.watch(timer);
    ASSERT_TRUE(dispatcher.start_ready());
    ASSERT_TRUE(timer.start_timer(NEVER_MS, dispatcher));
    EXPECT_TRUE(timer.is_active());
    ASSERT_TRUE(timer.start_timer(1u, dispatcher, 2u));
    ASSERT_TRUE(consumer.wait_count(timer, 2u));
    ASSERT_TRUE(dispatcher.barrier());
    EXPECT_EQ(consumer.count(timer), 2u);
    EXPECT_EQ(timer.timeout(), 1u);
    EXPECT_EQ(timer.event_count(), 0u);
    EXPECT_FALSE(timer.is_active());
}

TEST_F(TimerLifecycleTest, continuous_timer_stops_and_does_not_gain_callbacks_after_a_barrier)
{
    TimerListener consumer;
    areg::Timer timer(consumer, "LifecycleContinuous");
    areg::Timer witness(consumer, "LifecycleStopWitness");
    EventTestDispatcher dispatcher("LifecycleContinuousThread", 4096u);
    TimerTestRun run(dispatcher);
    run.watch(timer);
    run.watch(witness);
    ASSERT_TRUE(dispatcher.start_ready());
    ASSERT_TRUE(timer.start_timer(1u, dispatcher));
    ASSERT_TRUE(consumer.wait_count(timer, 3u));
    timer.stop_timer();
    ASSERT_TRUE(dispatcher.barrier());
    const uint32_t stoppedCount{ consumer.count(timer) };
    EXPECT_GE(stoppedCount, 3u);
    EXPECT_FALSE(timer.is_active());
    EXPECT_TRUE(timer.is_stopped());
    EXPECT_EQ(timer.event_count(), 0u);
    ASSERT_TRUE(witness.start_timer(1u, dispatcher, areg::Timer::ONE_TIME));
    ASSERT_TRUE(consumer.wait_count(witness, 1u));
    ASSERT_TRUE(dispatcher.barrier());
    EXPECT_EQ(consumer.count(timer), stoppedCount);
    EXPECT_EQ(consumer.count(witness), 1u);
}

TEST_F(TimerLifecycleTest, many_counted_timers_share_one_thread_without_loss_or_cross_delivery)
{
    constexpr uint32_t COUNT{ 16u };
    TimerListener consumer;
    std::vector<std::unique_ptr<areg::Timer>> timers;
    EventTestDispatcher dispatcher("LifecycleManyTimers", 1024u);
    TimerTestRun run(dispatcher);
    ASSERT_TRUE(dispatcher.start_ready());
    std::thread::id expected;
    ASSERT_TRUE(dispatcher.invoke([&]() { expected = std::this_thread::get_id(); }));
    for (uint32_t i = 0u; i < COUNT; ++i)
    {
        timers.push_back(std::make_unique<areg::Timer>(consumer));
        run.watch(*timers.back());
        ASSERT_TRUE(timers.back()->start_timer(1u + i % 3u, dispatcher, 1u + i % 4u));
    }
    for (uint32_t i = 0u; i < COUNT; ++i)
    {
        ASSERT_TRUE(consumer.wait_count(*timers[i], 1u + i % 4u));
    }
    ASSERT_TRUE(dispatcher.barrier());
    for (uint32_t i = 0u; i < COUNT; ++i)
    {
        EXPECT_EQ(consumer.threads(*timers[i]), std::vector<std::thread::id>(1u + i % 4u, expected));
        EXPECT_EQ(timers[i]->event_count(), 0u);
        EXPECT_FALSE(timers[i]->is_active());
    }
}

TEST_F(TimerLifecycleTest, throttled_continuous_timer_resumes_when_dispatcher_drains)
{
    TimerListener consumer;
    areg::Timer timer(consumer, "LifecycleThrottledTimer", 0u, 1);
    EventTestGate gate;
    std::promise<void> entered;
    TimerPostingDispatcher dispatcher(consumer);
    TimerTestRun run(dispatcher, &gate);
    run.watch(timer);
    ASSERT_TRUE(dispatcher.start_ready());
    ASSERT_TRUE(dispatcher.post_action([&]()
        {
            entered.set_value();
            EXPECT_TRUE(gate.wait());
        }));
    ASSERT_EQ(entered.get_future().wait_for(EVENT_TEST_WAIT), std::future_status::ready);
    ASSERT_TRUE(timer.start_timer(1u, dispatcher));
    ASSERT_TRUE(dispatcher.wait_posted(2u));
    EXPECT_EQ(consumer.count(timer), 0u);
    gate.open();
    ASSERT_TRUE(consumer.wait_count(timer, 4u));
    timer.stop_timer();
    ASSERT_TRUE(dispatcher.barrier());
    EXPECT_GE(consumer.count(timer), 4u);
    EXPECT_TRUE(timer.is_stopped());
    EXPECT_FALSE(timer.is_active());
}

TEST_F(TimerLifecycleTest, timer_can_move_to_a_restarted_dispatcher_without_a_stale_target)
{
    TimerListener consumer;
    areg::Timer timer(consumer, "LifecycleTargetRestart");
    EventTestDispatcher dispatcher("LifecycleTargetRestartThread");
    TimerTestRun run(dispatcher);
    run.watch(timer);
    ASSERT_TRUE(dispatcher.start_ready());
    ASSERT_TRUE(timer.start_timer(1u, dispatcher, areg::Timer::ONE_TIME));
    ASSERT_TRUE(consumer.wait_count(timer, 1u));
    timer.stop_timer();
    dispatcher.shutdown(areg::WAIT_INFINITE);
    EXPECT_FALSE(timer.start_timer(NEVER_MS, dispatcher, areg::Timer::ONE_TIME));
    EXPECT_FALSE(timer.is_active());
    ASSERT_TRUE(dispatcher.start_ready());
    ASSERT_TRUE(timer.start_timer(1u, dispatcher, areg::Timer::ONE_TIME));
    ASSERT_TRUE(consumer.wait_count(timer, 2u));
    ASSERT_TRUE(dispatcher.barrier());
    EXPECT_EQ(consumer.count(timer), 2u);
    EXPECT_FALSE(timer.is_active());
}

TEST(TimerManagerUnavailableTest, start_reports_failure_without_a_running_timer_manager)
{
    TimerListener consumer;
    areg::Timer timer(consumer, "LifecycleManagerUnavailable");
    EventTestDispatcher dispatcher("LifecycleManagerUnavailableThread");
    TimerTestRun run(dispatcher);
    run.watch(timer);
    ASSERT_TRUE(dispatcher.start_ready());
    EXPECT_FALSE(timer.start_timer(NEVER_MS, dispatcher, areg::Timer::ONE_TIME));
    EXPECT_FALSE(timer.is_active());
    ASSERT_TRUE(dispatcher.barrier());
    EXPECT_EQ(consumer.count(timer), 0u);
    timer.stop_timer();
    EXPECT_TRUE(timer.is_stopped());
}
