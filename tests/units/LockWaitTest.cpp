/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/LockWaitTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests for the waits of SpinLock, CriticalSection,
 *              SocketWriter and the thread exit. A thread that waits for another
 *              one sleeps and uses no processor time. Also covers recursion,
 *              ownership and mutual exclusion under contention.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/SocketDefs.hpp"
#include "areg/base/SyncPrimitives.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/ThreadConsumer.hpp"

#include <atomic>
#include <chrono>
#include <cstdint>
#include <thread>
#include <vector>

#if defined(_WIN32)
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif  // NOMINMAX
    #include <Windows.h>
#else   // !defined(_WIN32)
    #include <time.h>
#endif  // defined(_WIN32)

namespace
{
    using Clock = std::chrono::steady_clock;

    //!< How long the owner keeps the lock while another thread waits for it.
    constexpr uint32_t  HOLD_MS         { 300u };

    //!< The most processor time a waiter may use, in microseconds: a fifth of the hold.
    constexpr uint64_t  WAIT_CPU_MAX_US { HOLD_MS * 1000u / 5u };

    //!< Threads of the contention tests.
    constexpr uint32_t  STRESS_THREADS  { 8u };

    //!< Increments per thread in the contention tests.
    constexpr uint32_t  STRESS_LOOPS    { 20000u };

    //!< Every this many increments a thread keeps the lock 1 ms, so that the others sleep.
    constexpr uint32_t  STRESS_LONG_HOLD{ 2000u };

    //!< A socket handle no test opens, used only to pick a writer lock.
    const SOCKETHANDLE  TEST_SOCKET     { static_cast<SOCKETHANDLE>(97) };

    //!< Returns the processor time the calling thread has used, in microseconds.
    uint64_t thread_cpu_us()
    {
#if defined(_WIN32)
        FILETIME created{}, exited{}, kernel{}, user{};
        ::GetThreadTimes(::GetCurrentThread(), &created, &exited, &kernel, &user);
        const uint64_t k{ (static_cast<uint64_t>(kernel.dwHighDateTime) << 32) | kernel.dwLowDateTime };
        const uint64_t u{ (static_cast<uint64_t>(user.dwHighDateTime) << 32) | user.dwLowDateTime };
        return (k + u) / 10u;
#else   // !defined(_WIN32)
        struct timespec ts {};
        ::clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ts);
        return static_cast<uint64_t>(ts.tv_sec) * 1000000u + static_cast<uint64_t>(ts.tv_nsec) / 1000u;
#endif  // defined(_WIN32)
    }

    //!< What a waiter measured while it waited.
    struct WaitCost
    {
        uint64_t    cpuUs   { 0u }; //!< Processor time of the waiter.
        uint64_t    wallMs  { 0u }; //!< Elapsed time of the wait.
    };

    /**
     * \brief   A holder thread takes the lock with lockFn and keeps it HOLD_MS, a waiter thread
     *          takes it with lockFn too and measures its own processor time.
     **/
    template<typename LockFn, typename UnlockFn>
    WaitCost measure_wait(LockFn lockFn, UnlockFn unlockFn)
    {
        std::atomic<bool> held{ false };
        std::thread holder([&]()
            {
                lockFn();
                held.store(true);
                std::this_thread::sleep_for(std::chrono::milliseconds(HOLD_MS));
                unlockFn();
            });

        while (held.load() == false)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        WaitCost cost{};
        std::thread waiter([&]()
            {
                const Clock::time_point start{ Clock::now() };
                const uint64_t cpu{ thread_cpu_us() };
                lockFn();
                cost.cpuUs  = thread_cpu_us() - cpu;
                cost.wallMs = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - start).count());
                unlockFn();
            });

        waiter.join();
        holder.join();
        return cost;
    }

    /**
     * \brief   STRESS_THREADS threads increment one counter under the lock STRESS_LOOPS times
     *          each. Returns the final value of the counter.
     **/
    template<typename LockFn, typename UnlockFn>
    uint64_t run_contention(LockFn lockFn, UnlockFn unlockFn)
    {
        uint64_t counter{ 0u };
        std::vector<std::thread> threads;
        for (uint32_t i = 0u; i < STRESS_THREADS; ++i)
        {
            threads.emplace_back([&]()
                {
                    for (uint32_t n = 1u; n <= STRESS_LOOPS; ++n)
                    {
                        lockFn();
                        const uint64_t value{ counter };
                        if ((n % STRESS_LONG_HOLD) == 0u)
                        {
                            std::this_thread::sleep_for(std::chrono::milliseconds(1));
                        }

                        counter = value + 1u;
                        unlockFn();
                    }
                });
        }

        for (std::thread & thr : threads)
        {
            thr.join();
        }

        return counter;
    }

    //!< The routine returns at once; its exit keeps the thread in the Exiting state HOLD_MS.
    class SlowExitConsumer : public areg::ThreadConsumer
    {
    public:
        void on_run() override
        {
        }

        int32_t on_exit() override
        {
            mInExit.store(true);
            std::this_thread::sleep_for(std::chrono::milliseconds(HOLD_MS));
            return areg::ThreadConsumer::on_exit();
        }

        std::atomic<bool>   mInExit{ false };   //!< Set when on_exit() starts.
    };
}

/**
 * \brief   A thread waiting for a SpinLock sleeps instead of spinning.
 **/
TEST(LockWaitTest, SpinLockWaiterSleeps)
{
    areg::SpinLock lock;
    const WaitCost cost{ measure_wait([&]() { lock.lock(); }, [&]() { lock.unlock(); }) };

    EXPECT_GE(cost.wallMs, HOLD_MS / 2u);
    EXPECT_LT(cost.cpuUs, WAIT_CPU_MAX_US) << "the waiter used " << cost.cpuUs << " us of processor time in " << cost.wallMs << " ms";
}

/**
 * \brief   A thread waiting for a CriticalSection sleeps instead of spinning.
 **/
TEST(LockWaitTest, CriticalSectionWaiterSleeps)
{
    areg::CriticalSection lock;
    const WaitCost cost{ measure_wait([&]() { lock.lock(); }, [&]() { lock.unlock(); }) };

    EXPECT_GE(cost.wallMs, HOLD_MS / 2u);
    EXPECT_LT(cost.cpuUs, WAIT_CPU_MAX_US) << "the waiter used " << cost.cpuUs << " us of processor time in " << cost.wallMs << " ms";
}

/**
 * \brief   A thread waiting for the writer lock of a socket sleeps instead of spinning.
 **/
TEST(LockWaitTest, SocketWriterWaiterSleeps)
{
    areg::SocketWriter & writer{ areg::SocketWriter::writer_of(TEST_SOCKET) };
    const WaitCost cost{ measure_wait([&]() { writer.acquire(); }, [&]() { writer.release(); }) };

    EXPECT_GE(cost.wallMs, HOLD_MS / 2u);
    EXPECT_LT(cost.cpuUs, WAIT_CPU_MAX_US) << "the waiter used " << cost.cpuUs << " us of processor time in " << cost.wallMs << " ms";
}

/**
 * \brief   Destroying a thread object whose routine is still exiting sleeps until the routine
 *          leaves, instead of spinning.
 **/
TEST(LockWaitTest, ThreadExitWaiterSleeps)
{
    SlowExitConsumer consumer;
    areg::Thread * thread{ new areg::Thread(consumer, "LockWaitTest_exit") };
    ASSERT_TRUE(thread->start(areg::WAIT_INFINITE));

    while (consumer.mInExit.load() == false)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    const Clock::time_point start{ Clock::now() };
    const uint64_t cpu{ thread_cpu_us() };
    delete thread;
    const uint64_t cpuUs{ thread_cpu_us() - cpu };
    const uint64_t wallMs{ static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - start).count()) };

    EXPECT_GE(wallMs, HOLD_MS / 2u);
    EXPECT_LT(cpuUs, WAIT_CPU_MAX_US) << "the destructor used " << cpuUs << " us of processor time in " << wallMs << " ms";
}

/**
 * \brief   SpinLock is recursive, only its owner releases it, and another thread gets it
 *          only after the last release.
 **/
TEST(LockWaitTest, SpinLockRecursionAndOwnership)
{
    areg::SpinLock lock;
    auto tryFromOther = [&]() -> bool
        {
            bool acquired{ false };
            std::thread other([&]()
                {
                    acquired = lock.try_lock();
                    if (acquired)
                    {
                        EXPECT_TRUE(lock.unlock());
                    }
                });
            other.join();
            return acquired;
        };

    ASSERT_TRUE(lock.lock());
    ASSERT_TRUE(lock.lock());
    ASSERT_TRUE(lock.try_lock());
    EXPECT_FALSE(tryFromOther());

    bool otherUnlocked{ true };
    std::thread other([&]() { otherUnlocked = lock.unlock(); });
    other.join();
    EXPECT_FALSE(otherUnlocked);

    EXPECT_TRUE(lock.unlock());
    EXPECT_TRUE(lock.unlock());
    EXPECT_FALSE(tryFromOther());
    EXPECT_TRUE(lock.unlock());
    EXPECT_FALSE(lock.unlock());
    EXPECT_TRUE(tryFromOther());
}

/**
 * \brief   SpinLock keeps mutual exclusion while threads spin and sleep on it.
 **/
TEST(LockWaitTest, SpinLockContention)
{
    areg::SpinLock lock;
    const uint64_t counter{ run_contention([&]() { lock.lock(); lock.lock(); }, [&]() { lock.unlock(); lock.unlock(); }) };
    EXPECT_EQ(counter, static_cast<uint64_t>(STRESS_THREADS) * STRESS_LOOPS);
}

/**
 * \brief   CriticalSection keeps mutual exclusion while threads spin and sleep on it.
 **/
TEST(LockWaitTest, CriticalSectionContention)
{
    areg::CriticalSection lock;
    const uint64_t counter{ run_contention([&]() { lock.lock(); }, [&]() { lock.unlock(); }) };
    EXPECT_EQ(counter, static_cast<uint64_t>(STRESS_THREADS) * STRESS_LOOPS);
}

/**
 * \brief   SocketWriter keeps mutual exclusion while threads spin and sleep on it, and
 *          try_acquire() fails while another thread owns the lock.
 **/
TEST(LockWaitTest, SocketWriterContention)
{
    areg::SocketWriter & writer{ areg::SocketWriter::writer_of(TEST_SOCKET) };
    ASSERT_TRUE(writer.try_acquire());
    bool acquired{ true };
    std::thread other([&]() { acquired = writer.try_acquire(); });
    other.join();
    EXPECT_FALSE(acquired);
    writer.release();

    const uint64_t counter{ run_contention([&]() { writer.acquire(); }, [&]() { writer.release(); }) };
    EXPECT_EQ(counter, static_cast<uint64_t>(STRESS_THREADS) * STRESS_LOOPS);
}
