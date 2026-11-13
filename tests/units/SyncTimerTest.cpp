/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/SyncTimerTest.cpp
 * \author      Artak Avetyan
 * \brief       Areg Platform, tests of the waitable timer: a periodic timer wakes its waiter at
 *              every period, and it never ends another wait of the thread that set it.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/SyncPrimitives.hpp"

#include <atomic>
#include <chrono>
#include <cstdlib>
#include <future>
#include <memory>
#include <thread>
#include <vector>

#if defined(__SANITIZE_THREAD__)
    #define AREG_TEST_TSAN
#elif defined(__has_feature)
    #if __has_feature(thread_sanitizer)
        #define AREG_TEST_TSAN
    #endif  // __has_feature(thread_sanitizer)
#endif  // defined(__SANITIZE_THREAD__)

#if defined(_POSIX) && !defined(__CYGWIN__)
    #include <sys/wait.h>
    #include <unistd.h>
#endif  // defined(_POSIX) && !defined(__CYGWIN__)

#if defined(__linux__)
    #include <dirent.h>
    #include <fstream>
    #include <set>
    #include <string>
    #include <sys/syscall.h>
    #include <unistd.h>
#endif  // defined(__linux__)

namespace
{
    using Clock = std::chrono::steady_clock;

    //!< Milliseconds a wait that must end is given before the test fails.
    constexpr uint32_t  DEADLINE_MS { 30'000u };

    //!< A timeout that no test outlives: a timer with it never expires during a test.
    constexpr uint32_t  NEVER_MS    { 3'600'000u };

    int64_t elapsed_ms(Clock::time_point since)
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - since).count();
    }

    //!< Waits until 'count' reaches 'expected' or the deadline passes. Returns the last value read.
    int wait_count(const std::atomic<int> & count, int expected)
    {
        const Clock::time_point start{ Clock::now() };
        int value{ count.load() };
        while ((value < expected) && (elapsed_ms(start) < static_cast<int64_t>(DEADLINE_MS)))
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            value = count.load();
        }

        return value;
    }

#if defined(__linux__)
    // Adds the IDs of the threads of this process to 'tids'. Returns false if they cannot be read.
    bool collect_thread_ids(std::set<long>& tids)
    {
        DIR* dir{ ::opendir("/proc/self/task") };
        if (dir == nullptr)
            return false;

        for (const dirent* entry{ ::readdir(dir) }; entry != nullptr; entry = ::readdir(dir))
        {
            if (entry->d_name[0] != '.')
            {
                tids.insert(std::atol(entry->d_name));
            }
        }

        ::closedir(dir);
        return true;
    }

    // Runs 'work' while another thread records every thread of this process. Returns the threads that were not there before.
    template<typename Work>
    std::set<long> threads_created_during(Work work)
    {
        std::set<long> before;
        EXPECT_TRUE(collect_thread_ids(before));
        std::set<long> seen;
        std::atomic<bool> stop{ false };
        std::atomic<long> samplerId{ 0 };
        std::thread sampler([&]()
            {
                samplerId = static_cast<long>(::syscall(SYS_gettid));
                while (stop.load() == false)
                {
                    collect_thread_ids(seen);
                }
            });

        work();
        stop = true;
        sampler.join();
        seen.erase(samplerId.load());
        for (long tid : before)
        {
            seen.erase(tid);
        }

        return seen;
    }

    // Returns the number of threads of this process, or -1 if it cannot be read.
    int64_t thread_count()
    {
        std::ifstream file("/proc/self/status");
        std::string line;
        while (std::getline(file, line))
        {
            if (line.rfind("Threads:", 0) == 0)
                return std::atoll(line.c_str() + 8);
        }

        return -1;
    }

    // Returns the ID of the calling thread in the kernel.
    pid_t thread_id()
    {
        return static_cast<pid_t>(::syscall(SYS_gettid));
    }

    // Waits up to 'timeoutMs' until the thread 'tid' of this process is gone. Returns false if it is still there.
    // The kernel removes a joined thread from the process a moment after the join returns.
    bool thread_gone(pid_t tid, int64_t timeoutMs)
    {
        const std::string task{ "/proc/self/task/" + std::to_string(tid) };
        const Clock::time_point start{ Clock::now() };
        bool gone{ ::access(task.c_str(), F_OK) != 0 };
        while ((gone == false) && (elapsed_ms(start) < timeoutMs))
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            gone = (::access(task.c_str(), F_OK) != 0);
        }

        return gone;
    }

    // Returns the number of threads once it falls to 'expected', or the last count read after 'timeoutMs'.
    int64_t thread_count_reaching(int64_t expected, int64_t timeoutMs)
    {
        const Clock::time_point start{ Clock::now() };
        int64_t count{ thread_count() };
        while ((count > expected) && (elapsed_ms(start) < timeoutMs))
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            count = thread_count();
        }

        return count;
    }
#endif  // defined(__linux__)
}

// A periodic timer keeps waking the thread waiting on it, never before its period.
TEST(SyncTimerTest, periodic_timer_wakes_its_waiter_every_period)
{
    constexpr uint32_t  PERIOD_MS   { 50u };
    constexpr uint32_t  WAKES       { 15u };

    areg::SyncTimer timer(PERIOD_MS, true);
    const Clock::time_point started{ Clock::now() };
    ASSERT_TRUE(timer.set_timer());

    uint32_t wakes{ 0u };
    while ((wakes < WAKES) && timer.lock(DEADLINE_MS))
    {
        ++wakes;
    }

    const int64_t took{ elapsed_ms(started) };
    timer.unlock();
    EXPECT_EQ(wakes, WAKES);
    EXPECT_GE(took, static_cast<int64_t>((WAKES - 1u) * PERIOD_MS)) << WAKES << " wakes of a " << PERIOD_MS << " ms timer took " << took << " ms";
}

// A thread that set a periodic timer waits for a mutex and an event as long as it takes.
TEST(SyncTimerTest, timer_does_not_end_other_waits_of_its_thread)
{
    constexpr int64_t HOLD_MS{ 300 };
    areg::Mutex mutex(false);
    areg::SyncEvent event(true, true);
    ASSERT_TRUE(mutex.lock(areg::WAIT_INFINITE));

    bool mutexTaken{ false };
    bool eventTaken{ false };
    int64_t mutexMs{ -1 };
    int64_t eventMs{ -1 };
    const Clock::time_point started{ Clock::now() };
    std::thread waiter([&]()
    {
        areg::SyncTimer timer(5u, true);
        if (timer.set_timer() == false)
            return;

        mutexTaken = mutex.lock(areg::WAIT_INFINITE);
        mutexMs = elapsed_ms(started);
        if (mutexTaken)
        {
            mutex.unlock();
        }

        eventTaken = event.lock(areg::WAIT_INFINITE);
        eventMs = elapsed_ms(started);
        timer.unlock();
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(HOLD_MS));
    mutex.unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(HOLD_MS));
    event.set_signaled();
    waiter.join();

    EXPECT_TRUE(mutexTaken);
    EXPECT_GE(mutexMs, HOLD_MS - 20);
    EXPECT_TRUE(eventTaken);
    EXPECT_GE(eventMs, 2 * HOLD_MS - 20);
}

#if defined(__linux__)

// The ticks of a periodic timer create no thread in this process.
TEST(SyncTimerTest, periodic_timer_creates_no_thread_per_tick)
{
    constexpr uint32_t  PERIOD_MS   { 2u };
    constexpr uint32_t  TICKS       { 200u };

    areg::SyncTimer timer(PERIOD_MS, true);
    ASSERT_TRUE(timer.set_timer());
    ASSERT_TRUE(timer.lock(DEADLINE_MS));

    uint32_t ticks{ 0u };
    const std::set<long> created{ threads_created_during([&]()
        {
            while ((ticks < TICKS) && timer.lock(DEADLINE_MS))
            {
                ++ticks;
            }
        }) };

    timer.cancel_timer();
    EXPECT_EQ(ticks, TICKS);
    EXPECT_TRUE(created.empty()) << TICKS << " ticks created " << created.size() << " threads";
}

// Control: the thread sampler sees threads that live only for one short step each.
TEST(SyncTimerTest, thread_sampler_sees_short_lived_threads)
{
    constexpr uint32_t  STEPS   { 200u };
    const std::set<long> created{ threads_created_during([]()
        {
            for (uint32_t i = 0u; i < STEPS; ++i)
            {
                std::thread([]() { std::this_thread::sleep_for(std::chrono::milliseconds(1)); }).join();
            }
        }) };

    EXPECT_FALSE(created.empty());
}

// Many armed timers are served by at most one extra thread.
TEST(SyncTimerTest, many_timers_share_one_thread)
{
    constexpr uint32_t  COUNT   { 50u };
    // Starts the helper thread a sanitizer creates on the first thread start, so it is in the baseline.
    pid_t first{ 0 };
    std::thread([&first]() { first = thread_id(); }).join();
    ASSERT_TRUE(thread_gone(first, DEADLINE_MS)) << "the first thread outlived its join";
    const int64_t before{ thread_count() };
    if (before < 0)
    {
        GTEST_SKIP() << "/proc/self/status is not readable";
    }

    std::vector<std::unique_ptr<areg::SyncTimer>> timers;
    for (uint32_t i = 0; i < COUNT; ++i)
    {
        timers.emplace_back(std::make_unique<areg::SyncTimer>(1u + (i % 5u), true));
        ASSERT_TRUE(timers.back()->set_timer());
    }

    int64_t most{ before };
    for (int round = 0; round < 20; ++round)
    {
        for (auto & timer : timers)
        {
            EXPECT_TRUE(timer->lock(DEADLINE_MS));
        }

        most = std::max(most, thread_count());
    }

    timers.clear();
    EXPECT_LE(most - before, 1) << COUNT << " timers ran on " << (most - before) << " extra threads";
    EXPECT_EQ(thread_count_reaching(before, DEADLINE_MS), before) << "the timer thread outlived the last timer";
}

#endif  // defined(__linux__)

// A cancelled timer can be set again.
TEST(SyncTimerTest, cancelled_timer_can_be_set_again)
{
    areg::SyncTimer timer(5u, false);
    ASSERT_TRUE(timer.set_timer());
    ASSERT_TRUE(timer.cancel_timer());
    ASSERT_TRUE(timer.set_timer());

    const Clock::time_point started{ Clock::now() };
    EXPECT_TRUE(timer.lock(DEADLINE_MS));
    EXPECT_GE(elapsed_ms(started), 4);
}

// Cancel releases one waiter of an auto-reset timer, like one expiration.
TEST(SyncTimerTest, cancel_releases_one_waiter_of_auto_reset_timer)
{
    areg::SyncTimer timer(NEVER_MS, false, true);
    ASSERT_TRUE(timer.set_timer());

    std::atomic<int> released{ 0 };
    auto waiter = [&]()
    {
        if (timer.lock(DEADLINE_MS))
        {
            released.fetch_add(1);
        }
    };

    std::thread first(waiter);
    std::thread second(waiter);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_TRUE(timer.cancel_timer());
    const int byFirstCancel{ wait_count(released, 1) };
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    const int afterFirstCancel{ released.load() };

    // The second cancel releases the other waiter.
    EXPECT_TRUE(timer.cancel_timer());
    first.join();
    second.join();

    EXPECT_EQ(byFirstCancel, 1);
    EXPECT_EQ(afterFirstCancel, 1);
    EXPECT_EQ(released.load(), 2);
}

// Cancel releases every waiter of a manual-reset timer, which stays signaled until it is set again.
TEST(SyncTimerTest, cancel_releases_all_waiters_of_manual_reset_timer)
{
    constexpr int WAITERS{ 3 };
    areg::SyncTimer timer(NEVER_MS, false, false);
    ASSERT_TRUE(timer.set_timer());

    std::atomic<int> released{ 0 };
    std::vector<std::thread> waiters;
    for (int i = 0; i < WAITERS; ++i)
    {
        waiters.emplace_back([&]() { if (timer.lock(DEADLINE_MS)) released.fetch_add(1); });
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_TRUE(timer.cancel_timer());
    for (auto & waiter : waiters)
    {
        waiter.join();
    }

    EXPECT_EQ(released.load(), WAITERS);
    EXPECT_TRUE(timer.lock(0u)) << "a cancelled manual-reset timer stays signaled";
    EXPECT_TRUE(timer.lock(0u));

    ASSERT_TRUE(timer.set_timer());
    EXPECT_FALSE(timer.lock(50u)) << "set_timer clears the signal";
    EXPECT_TRUE(timer.cancel_timer());
}

// One expiration of an auto-reset timer releases one waiter.
TEST(SyncTimerTest, expiry_of_auto_reset_timer_releases_one_waiter)
{
    areg::SyncTimer timer(50u, false, true);
    std::atomic<int> released{ 0 };
    auto waiter = [&]() { if (timer.lock(DEADLINE_MS)) released.fetch_add(1); };

    ASSERT_TRUE(timer.set_timer());
    std::thread first(waiter);
    std::thread second(waiter);
    const int byExpiry{ wait_count(released, 1) };
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    const int afterExpiry{ released.load() };

    // A cancel releases the other waiter.
    EXPECT_TRUE(timer.cancel_timer());
    first.join();
    second.join();

    EXPECT_EQ(byExpiry, 1);
    EXPECT_EQ(afterExpiry, 1);
    EXPECT_EQ(released.load(), 2);
}

// One expiration of a manual-reset timer releases every waiter.
TEST(SyncTimerTest, expiry_of_manual_reset_timer_releases_all_waiters)
{
    constexpr int WAITERS{ 3 };
    areg::SyncTimer timer(50u, false, false);
    std::atomic<int> released{ 0 };

    ASSERT_TRUE(timer.set_timer());
    std::vector<std::thread> waiters;
    for (int i = 0; i < WAITERS; ++i)
    {
        waiters.emplace_back([&]() { if (timer.lock(DEADLINE_MS)) released.fetch_add(1); });
    }

    for (auto & waiter : waiters)
    {
        waiter.join();
    }

    EXPECT_EQ(released.load(), WAITERS);
    EXPECT_TRUE(timer.lock(0u));
}

// A periodic timer that nobody waits for keeps one expiration, not one per missed period.
TEST(SyncTimerTest, periodic_timer_does_not_queue_missed_periods)
{
    constexpr uint32_t  PERIOD_MS   { 20u };
    constexpr uint32_t  MISSED      { 10u };
    constexpr uint32_t  TRIES       { 2u * MISSED };
    areg::SyncTimer timer(PERIOD_MS, true, true);
    ASSERT_TRUE(timer.set_timer());

    std::this_thread::sleep_for(std::chrono::milliseconds(MISSED * PERIOD_MS));
    EXPECT_TRUE(timer.lock(DEADLINE_MS));

    // Queued periods would all be taken at once. One may be pending at the start, and one more
    // may expire for each period boundary the tries cross.
    uint32_t taken{ 0u };
    const Clock::time_point started{ Clock::now() };
    for (uint32_t i = 0u; i < TRIES; ++i)
    {
        taken += timer.lock(0u) ? 1u : 0u;
    }

    const int64_t took{ elapsed_ms(started) };
    timer.cancel_timer();
    EXPECT_LE(static_cast<int64_t>(taken), 2 + took / static_cast<int64_t>(PERIOD_MS)) << "missed periods were queued: " << taken << " taken in " << took << " ms";
}

// The expirations of a thread's timer never end its alertable wait on other objects.
TEST(SyncTimerTest, timer_does_not_end_alertable_wait_of_its_thread)
{
    constexpr uint32_t WAIT_MS{ 200u };
    areg::SyncTimer timer(5u, true);
    areg::SyncEvent event(true, false);
    ASSERT_TRUE(timer.set_timer());

    areg::SyncObject * objects[]{ &event };
    areg::MultiLock multi(objects, 1, false);
    const Clock::time_point started{ Clock::now() };
    const int32_t result{ multi.lock(WAIT_MS, false, true) };
    const int64_t waited{ elapsed_ms(started) };
    timer.cancel_timer();

    EXPECT_EQ(result, areg::MultiLock::LOCK_INDEX_TIMEOUT);
    EXPECT_GE(waited, static_cast<int64_t>(WAIT_MS) - 20);
}

// A one-shot timer fires once, after its timeout.
TEST(SyncTimerTest, one_shot_timer_fires_once)
{
    areg::SyncTimer timer(20u, false);
    const Clock::time_point started{ Clock::now() };
    ASSERT_TRUE(timer.set_timer());
    EXPECT_TRUE(timer.lock(DEADLINE_MS));
    EXPECT_GE(elapsed_ms(started), 19);
    EXPECT_FALSE(timer.lock(100u));
}

// A timer destroyed while it expires is never used after its destruction.
TEST(SyncTimerTest, destroy_while_firing)
{
    constexpr int ROUNDS{ 1'000 };
    std::atomic<int> woken{ 0 };
    for (int i = 0; i < ROUNDS; ++i)
    {
        auto timer = std::make_unique<areg::SyncTimer>(1u, true);
        ASSERT_TRUE(timer->set_timer());
        std::thread waiter([&timer, &woken]()
        {
            if (timer->lock(DEADLINE_MS))
            {
                woken.fetch_add(1, std::memory_order_relaxed);
            }
        });

        waiter.join();
        if ((i % 2) == 0)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(900 + (i % 7) * 50));
        }

        timer.reset();
    }

    EXPECT_EQ(woken.load(), ROUNDS);
}

#if GTEST_HAS_DEATH_TEST

// A static timer armed at exit leaves the process cleanly.
TEST(SyncTimerTest, static_timer_after_main)
{
    ::testing::GTEST_FLAG( death_test_style ) = "threadsafe";
    EXPECT_EXIT(
    {
        static areg::SyncTimer timer(1u, true);
        timer.set_timer();
        timer.lock(1'000u);
        std::exit(0);
    }, ::testing::ExitedWithCode(0), "");
}

#endif  // GTEST_HAS_DEATH_TEST

// A timer set by several threads at once stays one timer: it is signaled once by a cancel, and the
// timers of the process keep firing and can be destroyed.
TEST(SyncTimerTest, timer_set_by_several_threads_at_once_stays_one_timer)
{
    constexpr uint32_t THREADS{ 4u };
    constexpr uint32_t ROUNDS { 20'000u };
    areg::SyncTimer timer(NEVER_MS, false, true);
    std::promise<void> go;
    const std::shared_future<void> started{ go.get_future().share() };
    std::atomic<uint32_t> failed{ 0u };
    std::vector<std::thread> setters;
    for (uint32_t i = 0u; i < THREADS; ++i)
    {
        setters.emplace_back([&timer, &failed, started]()
            {
                started.wait();
                for (uint32_t round = 0u; round < ROUNDS; ++round)
                {
                    if (timer.set_timer() == false)
                    {
                        failed.fetch_add(1u);
                    }
                }
            });
    }

    go.set_value();
    for (std::thread & setter : setters)
    {
        setter.join();
    }

    EXPECT_EQ(failed.load(), 0u);
    EXPECT_FALSE(timer.lock(areg::DO_NOT_WAIT));
    EXPECT_TRUE(timer.cancel_timer());
    EXPECT_TRUE(timer.lock(DEADLINE_MS));
    EXPECT_FALSE(timer.lock(areg::DO_NOT_WAIT));

    areg::SyncTimer other(1u, false, true);
    ASSERT_TRUE(other.set_timer());
    EXPECT_TRUE(other.lock(DEADLINE_MS));
}

#if defined(_POSIX) && !defined(__CYGWIN__)
/**
 * \brief   A child forked while a timer of the parent is armed starts with no armed timer: its
 *          own timer fires. The timers of the parent still fire after the fork.
 **/
TEST(SyncTimerTest, forked_child_runs_its_own_timer_and_the_parent_keeps_its_timers)
{
#if defined(AREG_TEST_TSAN)
    GTEST_SKIP() << "the thread sanitizer refuses a new thread after a fork of a multithreaded process";
#endif  // defined(AREG_TEST_TSAN)

    constexpr uint32_t SHORT_MS{ 5u };
    constexpr uint32_t DEADLINE_MS{ 10'000u };
    areg::SyncTimer armed(60'000u, false);
    ASSERT_TRUE(armed.set_timer());

    const pid_t child{ ::fork() };
    ASSERT_NE(child, -1);
    if (child == 0)
    {
        ::alarm(20u);
        areg::SyncTimer own(SHORT_MS, false);
        const bool fired{ own.set_timer() && own.lock(DEADLINE_MS) };
        std::_Exit(fired ? 0 : 1);
    }

    int status{ 0 };
    ASSERT_EQ(::waitpid(child, &status, 0), child);
    EXPECT_TRUE(WIFEXITED(status) && (WEXITSTATUS(status) == 0));

    EXPECT_TRUE(armed.cancel_timer());
    areg::SyncTimer after(SHORT_MS, false);
    ASSERT_TRUE(after.set_timer());
    EXPECT_TRUE(after.lock(DEADLINE_MS));
}
#endif  // defined(_POSIX) && !defined(__CYGWIN__)
