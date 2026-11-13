#include "units/GUnitTest.hpp"
#include "units/ThreadCoverageHelpers.hpp"
#include "areg/base/SyncPrimitives.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/ThreadConsumer.hpp"
#if defined(AREG_TEST_PRIVATE_POSIX)
    #include "areg/base/private/posix/SyncLockAndWaitPosix.hpp"
#endif  // defined(AREG_TEST_PRIVATE_POSIX)

#include <array>
#include <atomic>
#include <memory>
#include <thread>

#if defined(_WIN32)
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif  // NOMINMAX
    #include <Windows.h>
#endif  // defined(_WIN32)

namespace
{
    uint32_t take_available_permits(areg::Semaphore& semaphore)
    {
        uint32_t count{ 0u };
        while ((count < semaphore.max_count()) && semaphore.lock(areg::DO_NOT_WAIT))
        {
            ++count;
        }
        return count;
    }

    void return_permits(areg::Semaphore& semaphore, uint32_t count)
    {
        for (uint32_t index = 0u; index < count; ++index)
        {
            EXPECT_TRUE(semaphore.unlock());
        }
    }
}

TEST(MultiLockCoverageTest, mixed_wait_all_acquires_and_releases_each_object)
{
    areg::Mutex mutex;
    areg::Semaphore semaphore(3, 1);
    areg::SyncEvent event(false, true);
    areg::SyncTimer timer(COVERAGE_NEVER_MS, false, false);
    ASSERT_TRUE(timer.cancel_timer());
    areg::SyncObject* objects[]{ &mutex, &semaphore, &event, &timer };
    areg::MultiLock multi(objects, 4, false);
    ASSERT_EQ(multi.lock(COVERAGE_WAIT_MS, true), areg::MultiLock::LOCK_INDEX_ALL);
    EXPECT_FALSE(semaphore.lock(areg::DO_NOT_WAIT));
    EXPECT_FALSE(event.lock(areg::DO_NOT_WAIT));
    EXPECT_TRUE(timer.lock(areg::DO_NOT_WAIT));
    EXPECT_TRUE(multi.unlock());
    EXPECT_TRUE(semaphore.lock(areg::DO_NOT_WAIT));
    EXPECT_FALSE(semaphore.lock(areg::DO_NOT_WAIT));
    EXPECT_TRUE(semaphore.unlock());
    EXPECT_TRUE(event.lock(areg::DO_NOT_WAIT));
    EXPECT_FALSE(event.lock(areg::DO_NOT_WAIT));
}

TEST(MultiLockCoverageTest, wait_any_selects_each_supported_object_kind)
{
    for (int32_t selected = 0; selected < 4; ++selected)
    {
        SCOPED_TRACE(selected);
        areg::SyncEvent empty(true, true);
        areg::SyncEvent event(false, true);
        areg::Semaphore semaphore(1, 1);
        areg::Mutex mutex;
        areg::SyncTimer timer(COVERAGE_NEVER_MS, false, true);
        ASSERT_TRUE(timer.cancel_timer());
        areg::SyncObject* objects[]{ &empty, &event, &semaphore, &mutex, &timer };
        areg::SyncObject* selectedObjects[]{ objects[0], objects[selected + 1] };
        areg::MultiLock multi(selectedObjects, 2, false);
        EXPECT_EQ(multi.lock(COVERAGE_WAIT_MS, false), 1);
        EXPECT_TRUE(multi.unlock(1));
        EXPECT_FALSE(multi.unlock(0));
    }
}

TEST(MultiLockCoverageTest, wait_all_timeout_does_not_consume_ready_resources)
{
    areg::Mutex mutex;
    areg::Semaphore semaphore(1, 1);
    areg::SyncEvent ready(false, true);
    areg::SyncEvent missing(true, true);
    areg::SyncObject* objects[]{ &mutex, &semaphore, &ready, &missing };
    areg::MultiLock multi(objects, 4, false);
    EXPECT_EQ(multi.lock(1u, true), areg::MultiLock::LOCK_INDEX_TIMEOUT);
    EXPECT_FALSE(multi.unlock(0));
    EXPECT_FALSE(multi.unlock(1));
    EXPECT_TRUE(semaphore.lock(areg::DO_NOT_WAIT));
    EXPECT_FALSE(semaphore.lock(areg::DO_NOT_WAIT));
    EXPECT_TRUE(semaphore.unlock());
    EXPECT_TRUE(ready.lock(areg::DO_NOT_WAIT));
    EXPECT_FALSE(ready.lock(areg::DO_NOT_WAIT));
    EXPECT_TRUE(mutex.try_lock());
    EXPECT_TRUE(mutex.unlock());
}

TEST(MultiLockCoverageTest, wait_any_timeout_can_be_retried_after_signal)
{
    areg::SyncEvent event(true, true);
    areg::Semaphore semaphore(1, 0);
    areg::SyncObject* objects[]{ &event, &semaphore };
    areg::MultiLock multi(objects, 2, false);
    EXPECT_EQ(multi.lock(1u), areg::MultiLock::LOCK_INDEX_TIMEOUT);
    EXPECT_FALSE(multi.unlock(0));
    EXPECT_FALSE(multi.unlock(1));
    ASSERT_TRUE(semaphore.unlock());
    EXPECT_EQ(multi.lock(areg::DO_NOT_WAIT), 1);
    EXPECT_TRUE(multi.unlock(1));
    EXPECT_TRUE(semaphore.lock(areg::DO_NOT_WAIT));
    EXPECT_FALSE(semaphore.lock(areg::DO_NOT_WAIT));
    EXPECT_TRUE(semaphore.unlock());
}

TEST(MultiLockCoverageTest, wait_all_accepts_maximum_number_of_objects)
{
    std::array<areg::SyncEvent, areg::MAXIMUM_WAITING_OBJECTS> events;
    std::array<areg::SyncObject*, areg::MAXIMUM_WAITING_OBJECTS> pointers{};
    for (size_t index = 0u; index < events.size(); ++index)
    {
        pointers[index] = &events[index];
        ASSERT_TRUE(events[index].set_signaled());
    }
    areg::MultiLock multi(pointers.data(), static_cast<int32_t>(pointers.size()), false);
    EXPECT_EQ(multi.lock(areg::DO_NOT_WAIT, true), areg::MultiLock::LOCK_INDEX_ALL);
    for (auto& event : events)
    {
        EXPECT_FALSE(event.lock(areg::DO_NOT_WAIT));
    }
    EXPECT_TRUE(multi.unlock());
    for (auto& event : events)
    {
        EXPECT_TRUE(event.lock(areg::DO_NOT_WAIT));
        EXPECT_FALSE(event.lock(areg::DO_NOT_WAIT));
    }
}

TEST(MultiLockCoverageTest, wait_any_handoff_has_exact_count_and_order)
{
    CoverageDeadline deadline;
    constexpr uint32_t COUNT{ 1'024u };
    areg::SyncEvent data(true, true);
    areg::SyncEvent stop(true, false);
    CoverageGate acknowledged;
    std::atomic<uint32_t> payload{ 0u };
    uint32_t count{ 0u };
    std::thread consumer([&]()
        {
            areg::SyncObject* objects[]{ &data, &stop };
            areg::MultiLock multi(objects, 2, false);
            for (;;)
            {
                const int32_t result{ multi.lock(COVERAGE_WAIT_MS) };
                if (result == 1)
                {
                    break;
                }
                if (result != 0)
                {
                    ADD_FAILURE() << "MultiLock did not observe data or stop";
                    return;
                }
                EXPECT_EQ(payload.load(), count + 1u);
                ++count;
                acknowledged.arrive();
            }
        });
    for (uint32_t value = 1u; value <= COUNT; ++value)
    {
        payload.store(value);
        EXPECT_TRUE(data.set_signaled());
        if (!acknowledged.wait(value))
        {
            ADD_FAILURE() << "No acknowledgement for value " << value;
            break;
        }
    }
    EXPECT_TRUE(stop.set_signaled());
    consumer.join();
    EXPECT_EQ(count, COUNT);
    EXPECT_FALSE(data.lock(areg::DO_NOT_WAIT));
}

TEST(MultiLockCoverageTest, staged_wait_all_requires_both_signals_and_consumes_both)
{
    CoverageDeadline deadline;
    areg::SyncEvent first(true, true);
    areg::SyncEvent second(true, true);
    CoverageGate started;
    std::atomic<bool> acquired{ false };
    std::thread waiter([&]()
        {
            areg::SyncObject* objects[]{ &first, &second };
            areg::MultiLock multi(objects, 2, false);
            started.arrive();
            acquired.store(multi.lock(COVERAGE_WAIT_MS, true) == areg::MultiLock::LOCK_INDEX_ALL);
        });
    EXPECT_TRUE(started.wait());
    EXPECT_TRUE(first.set_signaled());
    EXPECT_FALSE(acquired.load());
    EXPECT_TRUE(second.set_signaled());
    waiter.join();
    EXPECT_TRUE(acquired.load());
    EXPECT_FALSE(first.lock(areg::DO_NOT_WAIT));
    EXPECT_FALSE(second.lock(areg::DO_NOT_WAIT));
}

TEST(MultiLockCoverageTest, wait_all_wakes_when_mutex_owner_releases)
{
    CoverageDeadline deadline;
    areg::Mutex mutex;
    areg::SyncEvent event(false, true);
    ASSERT_TRUE(mutex.lock());
    CoverageGate starting;
    bool acquired{ false };
    std::thread waiter([&]()
        {
            areg::SyncObject* objects[]{ &event, &mutex };
            areg::MultiLock multi(objects, 2, false);
            starting.arrive();
            acquired = multi.lock(COVERAGE_WAIT_MS, true) == areg::MultiLock::LOCK_INDEX_ALL;
            if (acquired)
            {
                EXPECT_TRUE(multi.unlock(1));
            }
        });
    EXPECT_TRUE(starting.wait());
    EXPECT_TRUE(mutex.unlock());
    waiter.join();
    EXPECT_TRUE(acquired);
    EXPECT_FALSE(event.lock(areg::DO_NOT_WAIT));
    EXPECT_TRUE(mutex.try_lock());
    EXPECT_TRUE(mutex.unlock());
}

TEST(MultiLockCoverageTest, wait_all_wakes_when_semaphore_is_released)
{
    CoverageDeadline deadline;
    areg::Semaphore semaphore(1, 0);
    areg::SyncEvent event(false, true);
    CoverageGate starting;
    bool acquired{ false };
    std::thread waiter([&]()
        {
            areg::SyncObject* objects[]{ &event, &semaphore };
            areg::MultiLock multi(objects, 2, false);
            starting.arrive();
            acquired = multi.lock(COVERAGE_WAIT_MS, true) == areg::MultiLock::LOCK_INDEX_ALL;
            if (acquired)
            {
                EXPECT_TRUE(multi.unlock(1));
            }
        });
    EXPECT_TRUE(starting.wait());
    EXPECT_TRUE(semaphore.unlock());
    waiter.join();
    EXPECT_TRUE(acquired);
    EXPECT_FALSE(event.lock(areg::DO_NOT_WAIT));
    const uint32_t permits{ take_available_permits(semaphore) };
    EXPECT_EQ(permits, 1u);
    return_permits(semaphore, permits);
}

TEST(MultiLockCoverageTest, wait_all_updates_semaphore_counts)
{
    areg::Semaphore first(3, 1);
    areg::Semaphore second(3, 1);
    areg::SyncObject* objects[]{ &first, &second };
    areg::MultiLock multi(objects, 2, false);
    ASSERT_EQ(multi.lock(areg::DO_NOT_WAIT, true), areg::MultiLock::LOCK_INDEX_ALL);
    EXPECT_EQ(first.current_count(), 0u);
    EXPECT_EQ(second.current_count(), 0u);
    EXPECT_TRUE(multi.unlock());
    EXPECT_EQ(first.current_count(), 1u);
    EXPECT_EQ(second.current_count(), 1u);
}

TEST(MultiLockCoverageTest, wait_any_updates_semaphore_count)
{
    areg::SyncEvent event(true, false);
    areg::Semaphore semaphore(3, 1);
    areg::SyncObject* objects[]{ &event, &semaphore };
    areg::MultiLock multi(objects, 2, false);
    ASSERT_EQ(multi.lock(areg::DO_NOT_WAIT), 1);
    EXPECT_EQ(semaphore.current_count(), 0u);
    EXPECT_TRUE(multi.unlock(1));
    EXPECT_EQ(semaphore.current_count(), 1u);
}

TEST(MultiLockCoverageTest, wait_all_mutex_excludes_others_until_its_single_release)
{
    CoverageDeadline deadline;
    areg::SyncEvent event(false, false);
    areg::Mutex mutex;
    areg::SyncObject* objects[]{ &event, &mutex };
    auto other_can_lock = [&]()
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
        };
    areg::MultiLock multi(objects, 2, false);
    ASSERT_EQ(multi.lock(areg::DO_NOT_WAIT, true), areg::MultiLock::LOCK_INDEX_ALL);
    EXPECT_FALSE(other_can_lock());
    EXPECT_TRUE(multi.unlock(1));
    EXPECT_FALSE(multi.unlock(1));
    EXPECT_TRUE(other_can_lock());
}

TEST(MultiLockCoverageTest, repeated_unlock_does_not_create_permits)
{
    areg::Semaphore semaphore(3, 1);
    areg::SyncObject* objects[]{ &semaphore };
    areg::MultiLock multi(objects, 1, false);
    ASSERT_EQ(multi.lock(areg::DO_NOT_WAIT), 0);
    EXPECT_TRUE(multi.unlock());
    EXPECT_TRUE(multi.unlock());
    const uint32_t permits{ take_available_permits(semaphore) };
    EXPECT_EQ(permits, 1u);
    return_permits(semaphore, permits);
}

TEST(MultiLockCoverageTest, indexed_unlock_is_not_repeated_by_destructor)
{
    areg::Semaphore first(3, 1);
    areg::Semaphore second(3, 1);
    areg::SyncObject* objects[]{ &first, &second };
    {
        areg::MultiLock multi(objects, 2, true);
        EXPECT_TRUE(multi.unlock(0));
    }
    const uint32_t firstPermits{ take_available_permits(first) };
    const uint32_t secondPermits{ take_available_permits(second) };
    EXPECT_EQ(firstPermits, 1u);
    EXPECT_EQ(secondPermits, 1u);
    return_permits(first, firstPermits);
    return_permits(second, secondPermits);
}

TEST(MultiLockCoverageTest, single_object_wait_all_reports_all)
{
    areg::SyncEvent event(false, true);
    areg::SyncObject* objects[]{ &event };
    areg::MultiLock multi(objects, 1, false);
    EXPECT_EQ(multi.lock(areg::DO_NOT_WAIT, true), areg::MultiLock::LOCK_INDEX_ALL);
    EXPECT_FALSE(event.lock(areg::DO_NOT_WAIT));
    EXPECT_TRUE(multi.unlock());
}

TEST(MultiLockCoverageTest, single_object_wait_all_times_out_while_the_object_is_not_signaled)
{
    areg::SyncEvent event(true, true);
    areg::SyncObject* objects[]{ &event };
    areg::MultiLock multi(objects, 1, false);
    EXPECT_EQ(multi.lock(areg::DO_NOT_WAIT, true), areg::MultiLock::LOCK_INDEX_TIMEOUT);
    EXPECT_TRUE(event.set_signaled());
    EXPECT_EQ(multi.lock(areg::DO_NOT_WAIT, true), areg::MultiLock::LOCK_INDEX_ALL);
}

#if !defined(_WIN32)

TEST(MultiLockCoverageTest, wait_all_with_an_object_without_handle_reports_an_invalid_index_and_keeps_the_others)
{
    areg::SyncEvent pending(true, true);
    areg::SyncEvent hollow(areg::NullTag{});
    areg::SyncObject* objects[]{ &pending, &hollow };
    areg::MultiLock multi(objects, 2, false);
    EXPECT_EQ(multi.lock(COVERAGE_WAIT_MS, true), areg::MultiLock::LOCK_INDEX_INVALID);
    EXPECT_TRUE(pending.set_signaled());
    EXPECT_TRUE(pending.lock(areg::DO_NOT_WAIT));
    EXPECT_FALSE(pending.lock(areg::DO_NOT_WAIT));
}

TEST(MultiLockCoverageTest, wait_all_with_a_leading_object_without_handle_does_not_report_all)
{
    areg::SyncEvent hollow(areg::NullTag{});
    areg::SyncEvent pending(true, false);
    areg::SyncObject* objects[]{ &hollow, &pending };
    areg::MultiLock multi(objects, 2, false);
    EXPECT_NE(multi.lock(areg::DO_NOT_WAIT, true), areg::MultiLock::LOCK_INDEX_ALL);
    EXPECT_FALSE(pending.lock(areg::DO_NOT_WAIT));
}

#endif  // !defined(_WIN32)

namespace
{
    //!< A thread routine that returns when the gate opens.
    class GateConsumer final : public areg::ThreadConsumer
    {
    public:
        explicit GateConsumer(CoverageGate & gate)
            : mGate(gate)
        {
        }

        void on_run() override
        {
            static_cast<void>(mGate.wait());
        }

    private:
        CoverageGate & mGate;
    };
}

TEST(MultiLockCoverageTest, wait_any_without_a_time_limit_returns_the_signaled_event)
{
    areg::SyncEvent first(true, true);
    areg::SyncEvent second(false, true);
    areg::SyncEvent* events[]{ &first, &second };
    EXPECT_EQ(areg::SyncEvent::wait_any(events, 2, areg::WAIT_INFINITE), 1);
    EXPECT_EQ(areg::SyncEvent::wait_any(events, 2, areg::DO_NOT_WAIT), areg::SyncEvent::WAIT_ANY_TIMEOUT);
}

TEST(MultiLockCoverageTest, undefined_priority_keeps_the_priority_of_a_running_thread)
{
    CoverageDeadline deadline;
    CoverageGate release;
    GateConsumer consumer(release);
    areg::Thread thread(consumer, "MultiLockCoveragePriority");
    ASSERT_TRUE(thread.start(areg::WAIT_INFINITE));
    const areg::Thread::ThreadPriority current{ thread.priority() };
    EXPECT_EQ(thread.set_priority(areg::Thread::ThreadPriority::Undefined), current);
    EXPECT_EQ(thread.priority(), current);
    release.arrive();
    EXPECT_EQ(thread.shutdown(areg::WAIT_INFINITE), areg::Thread::ThreadCompletion::Completed);
    EXPECT_EQ(thread.set_priority(areg::Thread::ThreadPriority::High), areg::Thread::ThreadPriority::Undefined);
}

#if defined(_WIN32)

namespace
{
    std::atomic<uint32_t> gCompletionRuns{ 0u };

    void CALLBACK _count_completion(ULONG_PTR /*data*/)
    {
        gCompletionRuns.fetch_add(1u);
    }
}

TEST(MultiLockCoverageTest, alertable_wait_returns_the_completion_index_when_a_routine_runs)
{
    areg::SyncEvent event(true, true);
    areg::SyncObject* objects[]{ &event };
    areg::MultiLock multi(objects, 1, false);
    gCompletionRuns.store(0u);
    ASSERT_NE(::QueueUserAPC(&_count_completion, ::GetCurrentThread(), 0u), 0u);
    EXPECT_EQ(multi.lock(COVERAGE_WAIT_MS, false, true), areg::MultiLock::LOCK_INDEX_COMPLETION);
    EXPECT_EQ(gCompletionRuns.load(), 1u);
    EXPECT_EQ(multi.lock(areg::DO_NOT_WAIT, false, true), areg::MultiLock::LOCK_INDEX_TIMEOUT);
}

TEST(MultiLockCoverageTest, mutex_abandoned_by_its_thread_is_taken_by_the_next_lock)
{
    areg::Mutex single(false);
    areg::Mutex listed(false);
    std::thread owner([&single, &listed]()
        {
            static_cast<void>(single.lock(areg::WAIT_INFINITE));
            static_cast<void>(listed.lock(areg::WAIT_INFINITE));
        });
    owner.join();

    EXPECT_TRUE(single.lock(COVERAGE_WAIT_MS));
    EXPECT_TRUE(single.unlock());

    areg::SyncObject* objects[]{ &listed };
    areg::MultiLock multi(objects, 1, false);
    EXPECT_EQ(multi.lock(COVERAGE_WAIT_MS, false), 0);
    EXPECT_TRUE(multi.unlock());
    EXPECT_TRUE(listed.try_lock());
    EXPECT_TRUE(listed.unlock());
}

#endif  // defined(_WIN32)

#if defined(AREG_TEST_PRIVATE_POSIX)

TEST(MultiLockCoverageTest, wait_all_released_by_a_destroyed_object_reports_an_invalid_index)
{
    CoverageDeadline deadline;
    auto doomed{ std::make_unique<areg::SyncEvent>(true, false) };
    areg::SyncEvent other(true, false);
    areg::os::WaitablePosix* waitable{ reinterpret_cast<areg::os::WaitablePosix*>(doomed->handle()) };
    CoverageGate registered;
    int32_t index{ areg::MultiLock::LOCK_INDEX_TIMEOUT };
    std::thread waiter([&]()
        {
            areg::SyncObject* objects[]{ doomed.get(), &other };
            areg::MultiLock multi(objects, 2, false);
            registered.arrive();
            index = multi.lock(COVERAGE_WAIT_MS, true);
        });

    ASSERT_TRUE(registered.wait());
    while (!areg::os::SyncLockAndWaitPosix::is_waitable_registered(*waitable))
    {
        std::this_thread::yield();
    }

    doomed.reset();
    waiter.join();
    EXPECT_EQ(index, areg::MultiLock::LOCK_INDEX_INVALID);
    EXPECT_FALSE(other.lock(areg::DO_NOT_WAIT));
}

#endif  // defined(AREG_TEST_PRIVATE_POSIX)
