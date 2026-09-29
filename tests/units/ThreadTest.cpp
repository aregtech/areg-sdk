/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/ThreadTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests for the Thread lifecycle.
 *              Covers restart after shutdown and after an exit request, start of
 *              a running thread, and start and shutdown called from different
 *              threads on one object, for Thread and DispatcherThread, and the
 *              targeting of an event to a running dispatcher.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/ThreadConsumer.hpp"
#include "areg/component/DispatcherThread.hpp"
#include "areg/component/Event.hpp"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <deque>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

namespace
{
    using areg::Thread;

    //!< How long each concurrent scenario runs.
    constexpr std::chrono::milliseconds STRESS_DURATION { 1000 };

    //!< A call that makes no progress for this long is reported as a hang.
    constexpr std::chrono::milliseconds HANG_LIMIT      { 5000 };

    //!< The routine polls Thread::wait_exit() until an exit is requested.
    class PollingConsumer : public areg::ThreadConsumer
    {
    public:
        void on_run() override
        {
            while (Thread::wait_exit(1u))
            {
            }
        }
    };

    //!< How long the deaf routine ignores exit requests.
    constexpr uint32_t DEAF_MS { 300u };

    //!< The routine sleeps for DEAF_MS and ignores exit requests.
    class DeafConsumer : public areg::ThreadConsumer
    {
    public:
        void on_run() override
        {
            Thread::sleep(DEAF_MS);
        }
    };

    //!< A dispatcher thread with no consumers.
    class IdleDispatcher : public areg::DispatcherThread
    {
    public:
        explicit IdleDispatcher(const char * name)
            : areg::DispatcherThread(name, areg::DEFAULT_STACK_SIZE, 64u)
        {
        }

        bool post_event(areg::Event & eventElem) override
        {
            return areg::EventDispatcher::post_event(eventElem);
        }
    };

    //!< Ends the process if the scope is not left within HANG_LIMIT.
    class HangGuard
    {
    public:
        HangGuard()
            : mWatchdog([this]()
                {
                    std::unique_lock<std::mutex> lock(mLock);
                    if (mSignal.wait_for(lock, HANG_LIMIT, [this]() { return mLeft; }) == false)
                    {
                        ADD_FAILURE() << "The test made no progress for " << HANG_LIMIT.count() << " ms";
                        std::fflush(stdout);
                        std::_Exit(EXIT_FAILURE);
                    }
                })
        {
        }

        ~HangGuard()
        {
            {
                std::lock_guard<std::mutex> lock(mLock);
                mLeft = true;
            }

            mSignal.notify_all();
            mWatchdog.join();
        }

    private:
        std::mutex              mLock;
        std::condition_variable mSignal;
        bool                    mLeft{ false };
        std::thread             mWatchdog;
    };

    //!< Runs the workers for STRESS_DURATION. A worker stalled longer than HANG_LIMIT ends the process.
    class Stress
    {
    public:
        template<typename Fn>
        void add(Fn fn)
        {
            const std::size_t index{ mProgress.size() };
            mProgress.emplace_back(0);
            mJobs.emplace_back([this, index, fn]()
                {
                    while (mStop.load() == false)
                    {
                        fn();
                        mProgress[index].fetch_add(1);
                    }

                    mFinished.fetch_add(1);
                });
        }

        void run()
        {
            std::vector<std::thread> threads;
            for (auto & job : mJobs)
            {
                threads.emplace_back(job);
            }

            const auto end{ std::chrono::steady_clock::now() + STRESS_DURATION };
            auto lastMove{ std::chrono::steady_clock::now() };
            long lastSum{ -1 };
            while (mFinished.load() != mJobs.size())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
                const auto now{ std::chrono::steady_clock::now() };
                if (now >= end)
                {
                    mStop.store(true);
                }

                long sum{ static_cast<long>(mFinished.load()) };
                for (const auto & value : mProgress)
                {
                    sum += value.load();
                }

                if (sum != lastSum)
                {
                    lastSum = sum;
                    lastMove = now;
                }
                else if (now - lastMove > HANG_LIMIT)
                {
                    ADD_FAILURE() << "A start() or shutdown() call made no progress for "
                                  << HANG_LIMIT.count() << " ms";
                    std::fflush(stdout);
                    std::_Exit(EXIT_FAILURE);
                }
            }

            for (auto & thread : threads)
            {
                thread.join();
            }
        }

        long progress(std::size_t index) const
        {
            return mProgress[index].load();
        }

    private:
        std::vector<std::function<void()>>  mJobs;
        std::deque<std::atomic<long>>       mProgress;
        std::atomic<bool>                   mStop{ false };
        std::atomic<std::size_t>            mFinished{ 0u };
    };
}

//////////////////////////////////////////////////////////////////////////
// One caller
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   A thread polling wait_exit() leaves on every shutdown(), and the object restarts.
 **/
TEST(ThreadTest, RestartsAfterEveryShutdown)
{
    PollingConsumer consumer;
    Thread thread(consumer, "ThreadTest_restart");

    Stress stress;
    stress.add([&thread]()
        {
            ASSERT_TRUE(thread.start(areg::WAIT_INFINITE));
            ASSERT_EQ(thread.shutdown(areg::WAIT_INFINITE), Thread::ThreadCompletion::Completed);
        });
    stress.run();

    EXPECT_GT(stress.progress(0), 0);
    EXPECT_FALSE(thread.is_running());
}

/**
 * \brief   start() of a running thread fails and leaves the thread running and stoppable.
 **/
TEST(ThreadTest, StartOfRunningThreadChangesNothing)
{
    HangGuard guard;
    PollingConsumer consumer;
    Thread thread(consumer, "ThreadTest_running");

    ASSERT_TRUE(thread.start(areg::WAIT_INFINITE));
    EXPECT_FALSE(thread.start(areg::DO_NOT_WAIT));
    EXPECT_TRUE(thread.is_running());
    EXPECT_FALSE(thread.start(areg::WAIT_INFINITE));
    EXPECT_TRUE(thread.is_running());
    EXPECT_FALSE(thread.is_exit_requested());

    EXPECT_EQ(thread.shutdown(areg::WAIT_INFINITE), Thread::ThreadCompletion::Completed);
    EXPECT_FALSE(thread.is_running());
}

/**
 * \brief   start() of a dispatcher that is still starting fails and leaves it stoppable.
 **/
TEST(ThreadTest, StartOfStartingDispatcherChangesNothing)
{
    HangGuard guard;
    IdleDispatcher dispatcher("ThreadTest_starting");

    ASSERT_TRUE(dispatcher.start(areg::DO_NOT_WAIT));
    EXPECT_FALSE(dispatcher.start(areg::DO_NOT_WAIT));
    EXPECT_FALSE(dispatcher.start(areg::DO_NOT_WAIT));
    EXPECT_TRUE(dispatcher.wait_start(areg::WAIT_INFINITE));

    EXPECT_EQ(dispatcher.shutdown(areg::WAIT_INFINITE), Thread::ThreadCompletion::Completed);
    EXPECT_FALSE(dispatcher.is_running());
}

/**
 * \brief   A thread that left on request_exit() restarts without a shutdown() in between.
 **/
TEST(ThreadTest, RestartsAfterExitRequestWithoutShutdown)
{
    HangGuard guard;
    PollingConsumer consumer;
    Thread thread(consumer, "ThreadTest_request");

    ASSERT_TRUE(thread.start(areg::WAIT_INFINITE));
    thread.request_exit();
    ASSERT_TRUE(thread.wait_completion(areg::WAIT_INFINITE));
    EXPECT_TRUE(thread.is_exit_requested());

    ASSERT_TRUE(thread.start(areg::WAIT_INFINITE));
    EXPECT_TRUE(thread.is_running());
    EXPECT_FALSE(thread.is_exit_requested());

    EXPECT_EQ(thread.shutdown(areg::WAIT_INFINITE), Thread::ThreadCompletion::Completed);
}

/**
 * \brief   After a shutdown() that timed out, start() creates no second routine on the object:
 *          Windows terminates the thread and restarts at once, POSIX refuses until it has left.
 **/
TEST(ThreadTest, StartAfterTimedOutShutdown)
{
    HangGuard guard;
    DeafConsumer consumer;
    Thread thread(consumer, "ThreadTest_deaf");

    ASSERT_TRUE(thread.start(areg::WAIT_INFINITE));
    const Thread::ThreadCompletion result{ thread.shutdown(50u) };

#ifdef WINDOWS
    EXPECT_EQ(result, Thread::ThreadCompletion::Terminated);
#else   // WINDOWS
    EXPECT_EQ(result, Thread::ThreadCompletion::Stuck);
    EXPECT_FALSE(thread.start(areg::DO_NOT_WAIT));
    const auto deadline{ std::chrono::steady_clock::now() + std::chrono::milliseconds(DEAF_MS * 2u) };
    while (thread.start(areg::WAIT_INFINITE) == false)
    {
        ASSERT_LT(std::chrono::steady_clock::now(), deadline);
        Thread::sleep(10u);
    }

    EXPECT_EQ(thread.shutdown(areg::WAIT_INFINITE), Thread::ThreadCompletion::Completed);
#endif  // WINDOWS

    ASSERT_TRUE(thread.start(areg::WAIT_INFINITE));
    EXPECT_EQ(thread.shutdown(areg::WAIT_INFINITE), Thread::ThreadCompletion::Completed);
}

/**
 * \brief   An event is targeted to a dispatcher only while it runs and is ready for events.
 **/
TEST(ThreadTest, EventTargetsOnlyARunningDispatcher)
{
    HangGuard guard;
    IdleDispatcher dispatcher("ThreadTest_target");

    {
        areg::Event event(areg::EventType::EventCustomExternal);
        EXPECT_FALSE(event.register_for_thread(&dispatcher));
    }

    ASSERT_TRUE(dispatcher.start(areg::WAIT_INFINITE));
    ASSERT_TRUE(dispatcher.wait_start(areg::WAIT_INFINITE));
    {
        areg::Event event(areg::EventType::EventCustomExternal);
        EXPECT_TRUE(event.register_for_thread(&dispatcher));
        EXPECT_EQ(event.target_dispatcher(), &dispatcher);
    }

    EXPECT_EQ(dispatcher.shutdown(areg::WAIT_INFINITE), Thread::ThreadCompletion::Completed);
    {
        areg::Event event(areg::EventType::EventCustomExternal);
        EXPECT_FALSE(event.register_for_thread(&dispatcher));
    }
}

//////////////////////////////////////////////////////////////////////////
// start() and shutdown() from different threads
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   start(WAIT_INFINITE) and shutdown() race on one Thread and neither hangs.
 **/
TEST(ThreadTest, ConcurrentStartAndShutdownNeverHang)
{
    PollingConsumer consumer;
    Thread thread(consumer, "ThreadTest_race");

    Stress stress;
    stress.add([&thread]() { static_cast<void>(thread.start(areg::WAIT_INFINITE)); });
    stress.add([&thread]() { static_cast<void>(thread.shutdown(areg::WAIT_INFINITE)); });
    stress.run();

    EXPECT_NE(thread.shutdown(areg::WAIT_INFINITE), Thread::ThreadCompletion::Stuck);
    EXPECT_FALSE(thread.is_running());
}

/**
 * \brief   start(WAIT_INFINITE) and shutdown() race on one DispatcherThread and neither hangs.
 **/
TEST(ThreadTest, ConcurrentStartAndShutdownOfDispatcherNeverHang)
{
    IdleDispatcher dispatcher("ThreadTest_disp");

    Stress stress;
    stress.add([&dispatcher]() { static_cast<void>(dispatcher.start(areg::WAIT_INFINITE)); });
    stress.add([&dispatcher]() { static_cast<void>(dispatcher.shutdown(areg::WAIT_INFINITE)); });
    stress.run();

    EXPECT_NE(dispatcher.shutdown(areg::WAIT_INFINITE), Thread::ThreadCompletion::Stuck);
    EXPECT_FALSE(dispatcher.is_running());
}

/**
 * \brief   start(DO_NOT_WAIT) and shutdown() race on one DispatcherThread and neither hangs.
 **/
TEST(ThreadTest, ConcurrentStartNoWaitAndShutdownOfDispatcherNeverHang)
{
    IdleDispatcher dispatcher("ThreadTest_nowait");

    Stress stress;
    stress.add([&dispatcher]() { static_cast<void>(dispatcher.start(areg::DO_NOT_WAIT)); });
    stress.add([&dispatcher]() { static_cast<void>(dispatcher.shutdown(areg::WAIT_INFINITE)); });
    stress.run();

    EXPECT_NE(dispatcher.shutdown(areg::WAIT_INFINITE), Thread::ThreadCompletion::Stuck);
    EXPECT_FALSE(dispatcher.is_running());
}

/**
 * \brief   Two shutdown() calls race one start() loop, and no run is released by the wrong stop.
 **/
TEST(ThreadTest, TwoShutdownsAndStartNeverHang)
{
    PollingConsumer consumer;
    Thread thread(consumer, "ThreadTest_twostop");

    Stress stress;
    stress.add([&thread]() { static_cast<void>(thread.start(areg::WAIT_INFINITE)); });
    stress.add([&thread]() { static_cast<void>(thread.shutdown(areg::WAIT_INFINITE)); });
    stress.add([&thread]() { static_cast<void>(thread.shutdown(areg::WAIT_INFINITE)); });
    stress.run();

    EXPECT_NE(thread.shutdown(areg::WAIT_INFINITE), Thread::ThreadCompletion::Stuck);
    EXPECT_FALSE(thread.is_running());

    ASSERT_TRUE(thread.start(areg::WAIT_INFINITE));
    EXPECT_TRUE(thread.is_running());
    EXPECT_EQ(thread.shutdown(areg::WAIT_INFINITE), Thread::ThreadCompletion::Completed);
}
