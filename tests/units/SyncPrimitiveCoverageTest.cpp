#include "units/GUnitTest.hpp"
#include "units/ThreadCoverageHelpers.hpp"
#include "areg/base/SyncPrimitives.hpp"
#include "areg/base/Thread.hpp"

#include <array>
#include <atomic>
#include <chrono>
#include <thread>
#include <vector>

namespace
{
    bool acquire_from_other_thread(areg::Mutex& mutex)
    {
        bool acquired{ false };
        std::thread other([&]()
            {
                acquired = mutex.try_lock();
                if (acquired)
                {
                    EXPECT_TRUE(mutex.unlock());
                }
            });
        other.join();
        return acquired;
    }

    class CoverageLockable final : public areg::Lockable
    {
    public:
        CoverageLockable()
            : areg::Lockable(areg::SyncObject::SyncKind::SoNolock)
        {
        }
    };
}

TEST(SyncEventCoverageTest, hollow_event_operations_report_failure)
{
    areg::SyncEvent event(areg::NullTag{});
    EXPECT_FALSE(event.is_valid());
    EXPECT_EQ(event.handle(), nullptr);
    EXPECT_EQ(static_cast<SYNCHANDLE>(event), nullptr);
    EXPECT_EQ(event.type(), areg::SyncObject::SyncKind::SoEvent);
    EXPECT_FALSE(event.is_auto_reset());
    EXPECT_FALSE(event.lock(areg::DO_NOT_WAIT));
    EXPECT_FALSE(event.lock(areg::WAIT_INFINITE));
    EXPECT_FALSE(event.unlock());
    EXPECT_FALSE(event.set_signaled());
    EXPECT_FALSE(event.reset());
    event.pulse_event();
    EXPECT_FALSE(event.lock(areg::DO_NOT_WAIT));
}

TEST(SyncEventCoverageTest, auto_reset_signal_is_one_token_not_a_counter)
{
    areg::SyncEvent event(true, true);
    EXPECT_TRUE(event.is_valid());
    EXPECT_TRUE(event.is_auto_reset());
    EXPECT_EQ(event.type(), areg::SyncObject::SyncKind::SoEvent);
    EXPECT_EQ(static_cast<SYNCHANDLE>(event), event.handle());
    EXPECT_FALSE(event.lock(areg::DO_NOT_WAIT));
    ASSERT_TRUE(event.set_signaled());
    ASSERT_TRUE(event.set_signaled());
    EXPECT_TRUE(event.lock(areg::DO_NOT_WAIT));
    EXPECT_FALSE(event.lock(areg::DO_NOT_WAIT));
    ASSERT_TRUE(event.unlock());
    ASSERT_TRUE(event.reset());
    EXPECT_FALSE(event.lock(areg::DO_NOT_WAIT));
}

TEST(SyncEventCoverageTest, manual_reset_signal_persists_until_reset)
{
    areg::SyncEvent event(false, false);
    EXPECT_FALSE(event.is_auto_reset());
    EXPECT_TRUE(event.lock(areg::DO_NOT_WAIT));
    EXPECT_TRUE(event.lock(areg::DO_NOT_WAIT));
    EXPECT_TRUE(event.reset());
    EXPECT_FALSE(event.lock(areg::DO_NOT_WAIT));
    EXPECT_TRUE(event.unlock());
    EXPECT_TRUE(event.lock(areg::DO_NOT_WAIT));
    EXPECT_TRUE(event.lock(areg::DO_NOT_WAIT));
    EXPECT_TRUE(event.reset());
    EXPECT_FALSE(event.lock(1u));
}

TEST(SyncEventCoverageTest, pulse_without_waiters_does_not_leave_a_signal)
{
    areg::SyncEvent automatic(true, true);
    areg::SyncEvent manual(true, false);
    automatic.pulse_event();
    manual.pulse_event();
    EXPECT_FALSE(automatic.lock(areg::DO_NOT_WAIT));
    EXPECT_FALSE(manual.lock(areg::DO_NOT_WAIT));
    EXPECT_TRUE(automatic.set_signaled());
    EXPECT_TRUE(manual.set_signaled());
    EXPECT_TRUE(automatic.lock(areg::DO_NOT_WAIT));
    EXPECT_TRUE(manual.lock(areg::DO_NOT_WAIT));
}

TEST(SyncEventCoverageTest, pulsing_a_signaled_event_clears_the_signal)
{
    areg::SyncEvent automatic(false, true);
    areg::SyncEvent manual(false, false);
    automatic.pulse_event();
    manual.pulse_event();
    EXPECT_FALSE(automatic.lock(areg::DO_NOT_WAIT));
    EXPECT_FALSE(manual.lock(areg::DO_NOT_WAIT));
}

namespace
{
    bool pulse_until_released(areg::SyncEvent& event)
    {
        std::atomic<bool> done{ false };
        bool released{ false };
        std::thread waiter([&]()
            {
                released = event.lock(COVERAGE_WAIT_MS);
                done = true;
            });
        while (!done.load())
        {
            event.pulse_event();
            std::this_thread::yield();
        }
        waiter.join();
        return released;
    }
}

TEST(SyncEventCoverageTest, pulse_releases_a_waiting_thread_and_leaves_no_signal)
{
    CoverageDeadline deadline;
    for (bool autoReset : { false, true })
    {
        SCOPED_TRACE(autoReset);
        areg::SyncEvent event(true, autoReset);
        EXPECT_TRUE(pulse_until_released(event));
        EXPECT_FALSE(event.lock(areg::DO_NOT_WAIT));
    }
}

TEST(SyncEventCoverageTest, wait_any_validates_list_and_preserves_index_priority)
{
    areg::SyncEvent first(true, true);
    areg::SyncEvent second(false, true);
    areg::SyncEvent* events[]{ &first, &second };
    EXPECT_EQ(areg::SyncEvent::wait_any(nullptr, 2, areg::DO_NOT_WAIT), areg::SyncEvent::WAIT_ANY_TIMEOUT);
    EXPECT_EQ(areg::SyncEvent::wait_any(events, 0, areg::DO_NOT_WAIT), areg::SyncEvent::WAIT_ANY_TIMEOUT);
    EXPECT_EQ(areg::SyncEvent::wait_any(events, -1, areg::DO_NOT_WAIT), areg::SyncEvent::WAIT_ANY_TIMEOUT);
    EXPECT_EQ(areg::SyncEvent::wait_any(events, 2, areg::DO_NOT_WAIT), 1);
    EXPECT_EQ(areg::SyncEvent::wait_any(first, second, areg::DO_NOT_WAIT), areg::SyncEvent::WAIT_ANY_TIMEOUT);
    ASSERT_TRUE(first.set_signaled());
    ASSERT_TRUE(second.set_signaled());
    EXPECT_EQ(areg::SyncEvent::wait_any(events, 2, areg::DO_NOT_WAIT), 0);
    EXPECT_EQ(areg::SyncEvent::wait_any(events, 2, areg::DO_NOT_WAIT), 1);
    EXPECT_EQ(areg::SyncEvent::wait_any(events, 2, 1u), areg::SyncEvent::WAIT_ANY_TIMEOUT);
}

TEST(SyncEventCoverageTest, wait_any_accepts_maximum_distinct_events)
{
    std::array<areg::SyncEvent, areg::MAXIMUM_WAITING_OBJECTS> events;
    std::array<areg::SyncEvent*, areg::MAXIMUM_WAITING_OBJECTS> pointers{};
    for (size_t index = 0u; index < events.size(); ++index)
    {
        pointers[index] = &events[index];
    }
    ASSERT_TRUE(events.back().set_signaled());
    EXPECT_EQ(areg::SyncEvent::wait_any(pointers.data(), static_cast<int32_t>(pointers.size()), areg::DO_NOT_WAIT)
        , areg::MAXIMUM_WAITING_OBJECTS - 1);
    EXPECT_EQ(areg::SyncEvent::wait_any(pointers.data(), static_cast<int32_t>(pointers.size()), areg::DO_NOT_WAIT)
        , areg::SyncEvent::WAIT_ANY_TIMEOUT);
}

TEST(SyncEventCoverageTest, wait_any_handoff_delivers_every_value_in_order)
{
    CoverageDeadline deadline;
    constexpr uint32_t COUNT{ 2'048u };
    areg::SyncEvent data(true, true);
    areg::SyncEvent stop(true, false);
    areg::SyncEvent consumed(true, true);
    uint32_t payload{ 0u };
    std::vector<uint32_t> received;
    std::thread receiver([&]()
        {
            for (;;)
            {
                const int32_t index{ areg::SyncEvent::wait_any(data, stop, COVERAGE_WAIT_MS) };
                if (index == 1)
                {
                    break;
                }
                if (index != 0)
                {
                    ADD_FAILURE() << "Timed out waiting for data or stop";
                    break;
                }
                received.push_back(payload);
                EXPECT_TRUE(consumed.set_signaled());
            }
        });
    for (uint32_t value = 1u; value <= COUNT; ++value)
    {
        payload = value;
        EXPECT_TRUE(data.set_signaled());
        if (!consumed.lock(COVERAGE_WAIT_MS))
        {
            ADD_FAILURE() << "Consumer did not acknowledge value " << value;
            break;
        }
    }
    EXPECT_TRUE(stop.set_signaled());
    receiver.join();
    ASSERT_EQ(received.size(), COUNT);
    for (uint32_t index = 0u; index < COUNT; ++index)
    {
        EXPECT_EQ(received[index], index + 1u);
    }
}

TEST(SyncEventCoverageTest, manual_reset_releases_all_waiters_and_future_waiters)
{
    CoverageDeadline deadline;
    constexpr uint32_t WAITERS{ areg::MAXIMUM_WAITING_OBJECTS + 1u };
    areg::SyncEvent event(true, false);
    CoverageGate ready;
    std::atomic<uint32_t> released{ 0u };
    std::vector<std::thread> waiters;
    for (uint32_t index = 0u; index < WAITERS; ++index)
    {
        waiters.emplace_back([&]()
            {
                ready.arrive();
                if (event.lock(COVERAGE_WAIT_MS))
                {
                    ++released;
                }
                else
                {
                    ADD_FAILURE() << "Manual-reset event did not release a waiter";
                }
            });
    }
    EXPECT_TRUE(ready.wait(WAITERS));
    EXPECT_TRUE(event.set_signaled());
    for (auto& waiter : waiters)
    {
        waiter.join();
    }
    EXPECT_EQ(released.load(), WAITERS);
    EXPECT_TRUE(event.lock(areg::DO_NOT_WAIT));
    EXPECT_TRUE(event.reset());
    EXPECT_FALSE(event.lock(areg::DO_NOT_WAIT));
}

TEST(SemaphoreCoverageTest, capacity_exhaustion_and_over_release_keep_exact_count)
{
    areg::Semaphore semaphore(3, 2);
    EXPECT_TRUE(semaphore.is_valid());
    EXPECT_EQ(semaphore.type(), areg::SyncObject::SyncKind::SoSemaphore);
    EXPECT_EQ(semaphore.max_count(), 3u);
    EXPECT_EQ(semaphore.current_count(), 2u);
    EXPECT_FALSE(semaphore.try_lock());
    EXPECT_EQ(semaphore.current_count(), 2u);
    EXPECT_TRUE(semaphore.lock(areg::DO_NOT_WAIT));
    EXPECT_EQ(semaphore.current_count(), 1u);
    EXPECT_TRUE(semaphore.lock(areg::DO_NOT_WAIT));
    EXPECT_EQ(semaphore.current_count(), 0u);
    EXPECT_FALSE(semaphore.lock(1u));
    EXPECT_EQ(semaphore.current_count(), 0u);
    EXPECT_TRUE(semaphore.unlock());
    EXPECT_TRUE(semaphore.unlock());
    EXPECT_TRUE(semaphore.unlock());
    EXPECT_EQ(semaphore.current_count(), 3u);
    EXPECT_FALSE(semaphore.unlock());
    EXPECT_EQ(semaphore.current_count(), 3u);
}

TEST(SemaphoreCoverageTest, single_permit_handoff_preserves_order_and_final_capacity)
{
    CoverageDeadline deadline;
    constexpr uint32_t COUNT{ 1'024u };
    areg::Semaphore available(1, 0);
    areg::Semaphore acknowledged(1, 0);
    std::atomic<uint32_t> payload{ 0u };
    uint32_t observed{ 0u };
    std::thread receiver([&]()
        {
            for (uint32_t value = 1u; value <= COUNT; ++value)
            {
                if (!available.lock(COVERAGE_WAIT_MS))
                {
                    ADD_FAILURE() << "No permit for value " << value;
                    return;
                }
                EXPECT_EQ(payload.load(), value);
                ++observed;
                EXPECT_TRUE(acknowledged.unlock());
            }
        });
    for (uint32_t value = 1u; value <= COUNT; ++value)
    {
        payload.store(value);
        EXPECT_TRUE(available.unlock());
        if (!acknowledged.lock(COVERAGE_WAIT_MS))
        {
            ADD_FAILURE() << "No acknowledgement for value " << value;
            break;
        }
    }
    receiver.join();
    EXPECT_EQ(observed, COUNT);
    EXPECT_EQ(available.current_count(), 0u);
    EXPECT_EQ(acknowledged.current_count(), 0u);
    EXPECT_FALSE(available.lock(areg::DO_NOT_WAIT));
    EXPECT_FALSE(acknowledged.lock(areg::DO_NOT_WAIT));
    EXPECT_TRUE(available.unlock());
    EXPECT_EQ(available.current_count(), 1u);
}

TEST(MutexCoverageTest, recursive_acquisition_excludes_others_until_final_release)
{
    CoverageDeadline deadline;
    areg::Mutex mutex;
    EXPECT_TRUE(acquire_from_other_thread(mutex));
    ASSERT_TRUE(mutex.try_lock());
    EXPECT_FALSE(acquire_from_other_thread(mutex));
    ASSERT_TRUE(mutex.lock(1u));
    ASSERT_TRUE(mutex.try_lock());
    EXPECT_FALSE(acquire_from_other_thread(mutex));
    EXPECT_TRUE(mutex.unlock());
    EXPECT_FALSE(acquire_from_other_thread(mutex));
    EXPECT_TRUE(mutex.unlock());
    EXPECT_FALSE(acquire_from_other_thread(mutex));
    EXPECT_TRUE(mutex.unlock());
    EXPECT_TRUE(acquire_from_other_thread(mutex));
    EXPECT_FALSE(mutex.unlock());
}

TEST(MutexCoverageTest, non_owner_release_and_timeout_do_not_release_owner_lock)
{
    CoverageDeadline deadline;
    areg::Mutex mutex;
    ASSERT_TRUE(mutex.lock());
    std::thread other([&]()
        {
            EXPECT_FALSE(mutex.unlock());
            EXPECT_FALSE(mutex.try_lock());
            EXPECT_FALSE(mutex.lock(1u));
        });
    other.join();
    EXPECT_FALSE(acquire_from_other_thread(mutex));
    EXPECT_TRUE(mutex.unlock());
    EXPECT_TRUE(acquire_from_other_thread(mutex));
}

TEST(MutexCoverageTest, initially_owned_mutex_excludes_another_thread)
{
    CoverageDeadline deadline;
    areg::Mutex mutex(true);
    EXPECT_FALSE(acquire_from_other_thread(mutex));
    EXPECT_TRUE(mutex.unlock());
    EXPECT_TRUE(acquire_from_other_thread(mutex));
}

TEST(CriticalSectionCoverageTest, recursion_and_try_lock_release_exactly_one_level)
{
    CoverageDeadline deadline;
    areg::CriticalSection critical;
    auto other_can_lock = [&]()
        {
            bool acquired{ false };
            std::thread other([&]()
                {
                    acquired = critical.try_lock();
                    if (acquired)
                    {
                        EXPECT_TRUE(critical.unlock());
                    }
                });
            other.join();
            return acquired;
        };
    ASSERT_TRUE(critical.lock());
    ASSERT_TRUE(critical.try_lock());
    EXPECT_FALSE(other_can_lock());
    EXPECT_TRUE(critical.unlock());
    EXPECT_FALSE(other_can_lock());
    EXPECT_TRUE(critical.unlock());
    EXPECT_TRUE(other_can_lock());
}

TEST(ScopedLockCoverageTest, scoped_mutex_restores_access_after_early_return)
{
    CoverageDeadline deadline;
    areg::Mutex mutex;
    auto locked_scope = [&]()
        {
            areg::ScopedLock lock(mutex);
            EXPECT_FALSE(acquire_from_other_thread(mutex));
            return 42u;
        };
    EXPECT_EQ(locked_scope(), 42u);
    EXPECT_TRUE(acquire_from_other_thread(mutex));
}

TEST(ScopedLockCoverageTest, manual_lock_wrapper_does_not_lock_or_release_implicitly)
{
    areg::Semaphore semaphore(3, 1);
    {
        areg::Lock lock(semaphore, false);
        EXPECT_EQ(semaphore.current_count(), 1u);
        EXPECT_TRUE(lock.lock(areg::DO_NOT_WAIT));
        EXPECT_EQ(semaphore.current_count(), 0u);
        EXPECT_FALSE(lock.lock(areg::DO_NOT_WAIT));
        EXPECT_TRUE(lock.unlock());
        EXPECT_EQ(semaphore.current_count(), 1u);
    }
    EXPECT_EQ(semaphore.current_count(), 1u);
}

TEST(ScopedLockCoverageTest, automatic_lock_wrapper_returns_one_permit_at_scope_exit)
{
    areg::Semaphore semaphore(3, 1);
    {
        areg::Lock lock(semaphore);
        EXPECT_EQ(semaphore.current_count(), 0u);
        EXPECT_FALSE(semaphore.lock(areg::DO_NOT_WAIT));
    }
    EXPECT_EQ(semaphore.current_count(), 1u);
    EXPECT_TRUE(semaphore.lock(areg::DO_NOT_WAIT));
    EXPECT_FALSE(semaphore.lock(areg::DO_NOT_WAIT));
    EXPECT_TRUE(semaphore.unlock());
}

TEST(ScopedLockCoverageTest, no_lock_object_and_default_try_lock_follow_their_contracts)
{
    areg::NolockSyncObject noLock;
    EXPECT_TRUE(noLock.is_valid());
    EXPECT_EQ(noLock.type(), areg::SyncObject::SyncKind::SoNolock);
    EXPECT_TRUE(noLock.lock());
    EXPECT_TRUE(noLock.lock(areg::DO_NOT_WAIT));
    EXPECT_TRUE(noLock.try_lock());
    EXPECT_TRUE(noLock.unlock());
    {
        areg::ScopedLock scope(noLock);
        EXPECT_TRUE(noLock.lock(1u));
    }
    CoverageLockable base;
    EXPECT_FALSE(base.try_lock());
    EXPECT_FALSE(base.lock(areg::DO_NOT_WAIT));
    EXPECT_FALSE(base.unlock());
}

TEST(WaitCoverageTest, negative_duration_and_past_deadline_are_invalid)
{
    areg::Wait wait;
    EXPECT_EQ(wait.wait_for(areg::Wait::Duration{ -1 }), areg::Wait::WaitResolution::Invalid);
    EXPECT_EQ(wait.wait_until(std::chrono::steady_clock::now() - std::chrono::seconds(1))
        , areg::Wait::WaitResolution::Invalid);
}

TEST(WaitCoverageTest, positive_sub_microsecond_durations_are_ignored)
{
    areg::Wait wait;
    EXPECT_EQ(wait.wait_for(areg::Wait::Duration{ 1 }), areg::Wait::WaitResolution::Ignored);
    EXPECT_EQ(wait.wait_for(areg::Wait::ONE_MUS - areg::Wait::ONE_NS), areg::Wait::WaitResolution::Ignored);
}

TEST(WaitCoverageTest, microsecond_and_millisecond_waits_report_their_resolution)
{
    areg::Wait wait;
    EXPECT_EQ(wait.wait_for(areg::Wait::ONE_MUS), areg::Wait::WaitResolution::Microsecond);
    EXPECT_EQ(wait.wait_for(areg::Wait::MIN_WAIT), areg::Wait::WaitResolution::Millisecond);
    EXPECT_EQ(wait.wait(5u), areg::Wait::WaitResolution::Millisecond);
}

/**
 * \brief   Test every synchronization object is destroyed through the base class pointer, and a locked mutex
 *          is released by its destructor so that another object of the same kind can be owned.
 **/
TEST(SyncPrimitiveCoverageTest, objects_are_destroyed_through_base_pointer)
{
    areg::SyncObject* objects[]
    {
          new areg::Mutex(true)
        , new areg::SyncEvent(true, true)
        , new areg::Semaphore(2, 1)
        , new areg::CriticalSection()
        , new areg::SyncTimer(10u)
    };

    for (areg::SyncObject* object : objects)
    {
        ASSERT_NE(object, nullptr);
        delete object;
    }

    areg::Lockable* lockables[]{ new areg::Mutex(true), new areg::Semaphore(1, 0), new areg::CriticalSection() };
    for (areg::Lockable* lockable : lockables)
        delete lockable;
}

/**
 * \brief   Test the destructors run at the end of a scope, for every synchronization object.
 **/
TEST(SyncPrimitiveCoverageTest, objects_are_destroyed_at_end_of_scope)
{
    do { areg::Mutex mutex(true); } while (false);
    do { areg::SyncEvent event(false, false); } while (false);
    do { areg::Semaphore semaphore(3, 3); } while (false);
    do { areg::CriticalSection section; } while (false);
    do { areg::SyncTimer timer(5u, true, false, false); } while (false);
    SUCCEED();
}

/**
 * \brief   Test the Wait clock conversion helpers: a converted time point refers to the same moment,
 *          the distance helpers have the right sign.
 **/
TEST(SyncPrimitiveCoverageTest, wait_clock_conversion_and_distance)
{
    constexpr auto tolerance{ std::chrono::seconds(1) };
    const auto steadyNow{ std::chrono::steady_clock::now() };
    const auto systemNow{ std::chrono::system_clock::now() };

    const auto system{ areg::Wait::convert_to_system_clock(steadyNow + std::chrono::seconds(60)) };
    EXPECT_LT(system - (systemNow + std::chrono::seconds(60)), tolerance);
    EXPECT_GT(system - (systemNow + std::chrono::seconds(60)), -tolerance);

    const auto steady{ areg::Wait::convert_to_steady_clock(systemNow + std::chrono::seconds(60)) };
    EXPECT_LT(steady - (steadyNow + std::chrono::seconds(60)), tolerance);
    EXPECT_GT(steady - (steadyNow + std::chrono::seconds(60)), -tolerance);

    EXPECT_GT(areg::Wait::from_now(std::chrono::steady_clock::now() + std::chrono::seconds(60)), std::chrono::seconds(50));
    EXPECT_LT(areg::Wait::from_now(std::chrono::steady_clock::now() - std::chrono::seconds(60)), std::chrono::seconds(-50));
    EXPECT_GT(areg::Wait::until_now(std::chrono::steady_clock::now() - std::chrono::seconds(60)), std::chrono::seconds(50));
    EXPECT_LT(areg::Wait::until_now(std::chrono::steady_clock::now() + std::chrono::seconds(60)), std::chrono::seconds(-50));
}
