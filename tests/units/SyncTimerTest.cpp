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
#include <memory>
#include <thread>
#include <vector>

#if defined(__linux__)
    #include <fstream>
    #include <string>
    #include <sys/syscall.h>
    #include <unistd.h>
#endif  // defined(__linux__)

namespace
{
    using Clock = std::chrono::steady_clock;

    int64_t elapsed_ms(Clock::time_point since)
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - since).count();
    }

#if defined(__linux__)
    // Returns the last PID allocated in the PID namespace, or -1 if it cannot be read.
    int64_t last_pid()
    {
        std::ifstream file("/proc/sys/kernel/ns_last_pid");
        int64_t pid{ -1 };
        return (file >> pid) ? pid : -1;
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

// A periodic timer wakes the thread waiting on it once per period.
TEST(SyncTimerTest, periodic_timer_wakes_its_waiter_every_period)
{
    constexpr uint32_t  PERIOD_MS   { 50u };
    constexpr int64_t   RUN_MS      { 1'500 };
    constexpr uint32_t  MIN_WAKES   { 15u };

    areg::SyncTimer timer(PERIOD_MS, true);
    ASSERT_TRUE(timer.set_timer());

    uint32_t wakes{ 0u };
    const Clock::time_point started{ Clock::now() };
    while (elapsed_ms(started) < RUN_MS)
    {
        if (timer.lock(static_cast<uint32_t>(RUN_MS)))
        {
            ++wakes;
        }
    }

    timer.unlock();
    EXPECT_GE(wakes, MIN_WAKES) << "a " << PERIOD_MS << " ms timer woke its waiter " << wakes << " times in " << RUN_MS << " ms";
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

// The ticks of a periodic timer create no thread. Tasks of other processes count too, so one quiet try of ten passes.
TEST(SyncTimerTest, periodic_timer_creates_no_thread_per_tick)
{
    constexpr uint32_t  PERIOD_MS   { 2u };
    constexpr uint32_t  TICKS       { 200u };
    constexpr int64_t   MAX_TASKS   { static_cast<int64_t>(TICKS) / 4 };
    constexpr int       MAX_TRIES   { 10 };
    if (last_pid() < 0)
    {
        GTEST_SKIP() << "/proc/sys/kernel/ns_last_pid is not readable";
    }

    areg::SyncTimer timer(PERIOD_MS, true);
    ASSERT_TRUE(timer.set_timer());
    ASSERT_TRUE(timer.lock(1'000u));

    int64_t best{ INT64_MAX };
    for (int attempt = 0; (attempt < MAX_TRIES) && (best >= MAX_TASKS); ++attempt)
    {
        const int64_t before{ last_pid() };
        uint32_t ticks{ 0u };
        while ((ticks < TICKS) && timer.lock(1'000u))
        {
            ++ticks;
        }

        ASSERT_EQ(ticks, TICKS);
        best = std::min(best, last_pid() - before);
    }

    timer.cancel_timer();
    EXPECT_LT(best, MAX_TASKS) << TICKS << " ticks created " << best << " tasks";
}

// Many armed timers are served by at most one extra thread.
TEST(SyncTimerTest, many_timers_share_one_thread)
{
    constexpr uint32_t  COUNT   { 50u };
    // Starts the helper thread a sanitizer creates on the first thread start, so it is in the baseline.
    pid_t first{ 0 };
    std::thread([&first]() { first = thread_id(); }).join();
    ASSERT_TRUE(thread_gone(first, 1'000)) << "the first thread outlived its join";
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
            EXPECT_TRUE(timer->lock(1'000u));
        }

        most = std::max(most, thread_count());
    }

    timers.clear();
    EXPECT_LE(most - before, 1) << COUNT << " timers ran on " << (most - before) << " extra threads";
    EXPECT_EQ(thread_count_reaching(before, 1'000), before) << "the timer thread outlived the last timer";
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
    EXPECT_TRUE(timer.lock(1'000u));
    EXPECT_GE(elapsed_ms(started), 4);
}

// Cancel releases one waiter of an auto-reset timer, at once, like one expiration.
TEST(SyncTimerTest, cancel_releases_one_waiter_of_auto_reset_timer)
{
    areg::SyncTimer timer(10'000u, false, true);
    ASSERT_TRUE(timer.set_timer());

    std::atomic<int> released{ 0 };
    std::atomic<int64_t> firstMs{ -1 };
    Clock::time_point cancelled{ Clock::now() };
    auto waiter = [&]()
    {
        if (timer.lock(1'000u))
        {
            int64_t none{ -1 };
            firstMs.compare_exchange_strong(none, elapsed_ms(cancelled));
            released.fetch_add(1);
        }
    };

    std::thread first(waiter);
    std::thread second(waiter);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    cancelled = Clock::now();
    EXPECT_TRUE(timer.cancel_timer());
    first.join();
    second.join();

    RecordProperty("cancel_release_ms", static_cast<int>(firstMs.load()));
    EXPECT_EQ(released.load(), 1);
    EXPECT_GE(firstMs.load(), 0);
    EXPECT_LT(firstMs.load(), 100) << "cancel released the waiter after " << firstMs.load() << " ms";
}

// Cancel releases every waiter of a manual-reset timer, which stays signaled until it is set again.
TEST(SyncTimerTest, cancel_releases_all_waiters_of_manual_reset_timer)
{
    constexpr int WAITERS{ 3 };
    areg::SyncTimer timer(10'000u, false, false);
    ASSERT_TRUE(timer.set_timer());

    std::atomic<int> released{ 0 };
    std::vector<std::thread> waiters;
    for (int i = 0; i < WAITERS; ++i)
    {
        waiters.emplace_back([&]() { if (timer.lock(2'000u)) released.fetch_add(1); });
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
    auto waiter = [&]() { if (timer.lock(600u)) released.fetch_add(1); };

    ASSERT_TRUE(timer.set_timer());
    std::thread first(waiter);
    std::thread second(waiter);
    first.join();
    second.join();

    EXPECT_EQ(released.load(), 1);
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
        waiters.emplace_back([&]() { if (timer.lock(1'000u)) released.fetch_add(1); });
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
    constexpr int64_t   RUN_MS      { 200 };
    areg::SyncTimer timer(PERIOD_MS, true, true);
    ASSERT_TRUE(timer.set_timer());

    std::this_thread::sleep_for(std::chrono::milliseconds(RUN_MS));
    EXPECT_TRUE(timer.lock(0u));

    uint32_t wakes{ 0u };
    const Clock::time_point started{ Clock::now() };
    while (elapsed_ms(started) < RUN_MS)
    {
        const int64_t left{ RUN_MS - elapsed_ms(started) };
        if (timer.lock(static_cast<uint32_t>(left > 0 ? left : 0)))
        {
            ++wakes;
        }
    }

    timer.cancel_timer();
    EXPECT_LE(wakes, (RUN_MS / PERIOD_MS) + 2) << "missed periods were queued: " << wakes << " wakes";
    EXPECT_GE(wakes, (RUN_MS / PERIOD_MS) / 2);
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
    EXPECT_TRUE(timer.lock(1'000u));
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
            if (timer->lock(1'000u))
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
