/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/ListenerDispatchTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests for the dispatch of an event to its listeners.
 *              Covers a removal from another thread during and between callbacks, a
 *              callback that removes listeners on its own thread, more listeners than
 *              the dispatcher copies on the stack, and PlainSpinLock.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/SyncPrimitives.hpp"
#include "areg/component/DispatcherThread.hpp"
#include "areg/component/EventTemplate.hpp"

#include <atomic>
#include <chrono>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

//! The value an event carries to its listeners.
class LdData
{
public:
    LdData() = default;
    explicit LdData(int value) : mValue(value) {}
    LdData(const LdData & src) = default;
    LdData & operator = (const LdData & src) = default;

    inline int value() const { return mValue; }

private:
    int mValue{ 0 };
};

AREG_DECLARE_EVENT(LdData, LdEvent, LdEventConsumer);

namespace
{
    //!< A call that makes no progress for this long fails the test.
    constexpr std::chrono::seconds      _waitLimit{ 5 };

    //!< How long a remover is given to return too early.
    constexpr std::chrono::milliseconds _earlyLimit{ 100 };

    //!< How long the concurrent scenario runs.
    constexpr std::chrono::milliseconds _stressTime{ 500 };

    //!< A dispatcher thread with no component.
    class LdDispatcher : public areg::DispatcherThread
    {
    public:
        explicit LdDispatcher(const char * name)
            : areg::DispatcherThread(name, areg::DEFAULT_STACK_SIZE, 1024u)
        {
        }

        bool post_event(areg::Event & eventElem) override
        {
            return areg::EventDispatcher::post_event(eventElem);
        }

        //! Starts the thread and waits until it accepts events.
        bool start_ready()
        {
            if (start(areg::WAIT_INFINITE) == false)
                return false;

            const auto until{ std::chrono::steady_clock::now() + _waitLimit };
            while ((is_ready() == false) && (std::chrono::steady_clock::now() < until))
            {
                std::this_thread::yield();
            }

            return is_ready();
        }
    };

    //! Counts its calls and runs an optional action inside the callback.
    class LdListener : public LdEventConsumer
    {
    public:
        std::atomic<int>        mCalls{ 0 };
        std::function<void()>   mAction;

        void process_event(const LdData & /*data*/) final
        {
            if (mAction)
            {
                mAction();
            }

            mCalls.fetch_add(1);
        }
    };

    //! Waits until the counter reaches the value or the limit passes.
    bool wait_calls(const std::atomic<int> & counter, int value)
    {
        const auto until{ std::chrono::steady_clock::now() + _waitLimit };
        while ((counter.load() < value) && (std::chrono::steady_clock::now() < until))
        {
            std::this_thread::yield();
        }

        return (counter.load() >= value);
    }

    //! Posts one event to the thread and waits until the marker listener received it.
    bool drain(LdDispatcher & dispatcher, LdListener & marker)
    {
        const int before{ marker.mCalls.load() };
        return LdEvent::send_event(LdData(0), marker, dispatcher) && wait_calls(marker.mCalls, before + 1);
    }
}

/**
 * \brief   remove_listener() from another thread returns only after the running callback ended.
 **/
TEST(ListenerDispatchTest, RemoveFromOtherThreadWaitsForRunningCallback)
{
    LdDispatcher dispatcher("LdTest_wait");
    ASSERT_TRUE(dispatcher.start_ready());

    std::promise<void> entered;
    std::promise<void> release;
    std::shared_future<void> released{ release.get_future().share() };
    std::atomic<bool> finished{ false };

    LdListener listener;
    listener.mAction = [&entered, released, &finished]()
        {
            entered.set_value();
            released.wait();
            finished = true;
        };

    ASSERT_TRUE(LdEvent::add_listener(listener, dispatcher));
    ASSERT_TRUE(LdEvent::send_event(LdData(1), dispatcher));
    ASSERT_EQ(entered.get_future().wait_for(_waitLimit), std::future_status::ready);

    std::atomic<bool> finishedAtReturn{ false };
    auto remover = std::async(std::launch::async, [&]()
        {
            static_cast<void>(LdEvent::remove_listener(listener, dispatcher));
            finishedAtReturn = finished.load();
        });

    EXPECT_EQ(remover.wait_for(_earlyLimit), std::future_status::timeout);
    release.set_value();
    ASSERT_EQ(remover.wait_for(_waitLimit), std::future_status::ready);
    EXPECT_TRUE(finishedAtReturn.load());

    dispatcher.shutdown(areg::WAIT_INFINITE);
}

/**
 * \brief   A callback removes itself on its own thread without waiting and is not called again.
 **/
TEST(ListenerDispatchTest, CallbackRemovesItselfWithoutWaiting)
{
    LdDispatcher dispatcher("LdTest_self");
    ASSERT_TRUE(dispatcher.start_ready());

    LdListener listener;
    LdListener marker;
    listener.mAction = [&listener, &dispatcher]()
        {
            static_cast<void>(LdEvent::remove_listener(listener, dispatcher));
        };

    ASSERT_TRUE(LdEvent::add_listener(listener, dispatcher));
    ASSERT_TRUE(LdEvent::send_event(LdData(1), dispatcher));
    ASSERT_TRUE(wait_calls(listener.mCalls, 1));

    static_cast<void>(LdEvent::send_event(LdData(2), dispatcher));
    ASSERT_TRUE(drain(dispatcher, marker));
    EXPECT_EQ(listener.mCalls.load(), 1);

    dispatcher.shutdown(areg::WAIT_INFINITE);
}

/**
 * \brief   A listener removed by an earlier callback of the same event is not called.
 **/
TEST(ListenerDispatchTest, ListenerRemovedDuringDispatchIsSkipped)
{
    LdDispatcher dispatcher("LdTest_skip");
    ASSERT_TRUE(dispatcher.start_ready());

    LdListener first;
    LdListener second;
    LdListener marker;
    first.mAction = [&second, &dispatcher]()
        {
            static_cast<void>(LdEvent::remove_listener(second, dispatcher));
        };

    ASSERT_TRUE(LdEvent::add_listener(first, dispatcher));
    ASSERT_TRUE(LdEvent::add_listener(second, dispatcher));
    ASSERT_TRUE(LdEvent::send_event(LdData(1), dispatcher));
    ASSERT_TRUE(wait_calls(first.mCalls, 1));
    ASSERT_TRUE(drain(dispatcher, marker));

    EXPECT_EQ(second.mCalls.load(), 0);

    static_cast<void>(LdEvent::remove_listener(first, dispatcher));
    dispatcher.shutdown(areg::WAIT_INFINITE);
}

/**
 * \brief   Every listener is called once when there are more than the dispatcher copies on the stack.
 **/
TEST(ListenerDispatchTest, ManyListenersAreAllCalled)
{
    constexpr int count{ 21 };
    LdDispatcher dispatcher("LdTest_many");
    ASSERT_TRUE(dispatcher.start_ready());

    std::vector<std::unique_ptr<LdListener>> listeners;
    for (int i = 0; i < count; ++i)
    {
        listeners.push_back(std::make_unique<LdListener>());
        ASSERT_TRUE(LdEvent::add_listener(*listeners.back(), dispatcher));
    }

    ASSERT_TRUE(LdEvent::send_event(LdData(1), dispatcher));
    for (const auto & listener : listeners)
    {
        ASSERT_TRUE(wait_calls(listener->mCalls, 1));
    }

    LdListener marker;
    ASSERT_TRUE(drain(dispatcher, marker));
    for (const auto & listener : listeners)
    {
        EXPECT_EQ(listener->mCalls.load(), 1);
        static_cast<void>(LdEvent::remove_listener(*listener, dispatcher));
    }

    dispatcher.shutdown(areg::WAIT_INFINITE);
}

/**
 * \brief   Listeners added and removed from another thread while events are dispatched.
 *          A removed listener is destroyed at once, so a late call is a use after free.
 **/
TEST(ListenerDispatchTest, ConcurrentAddRemoveWhileDispatching)
{
    LdDispatcher dispatcher("LdTest_churn");
    ASSERT_TRUE(dispatcher.start_ready());

    LdListener steady;
    ASSERT_TRUE(LdEvent::add_listener(steady, dispatcher));

    std::atomic<bool> stop{ false };
    auto sender = std::async(std::launch::async, [&]()
        {
            while (stop.load() == false)
            {
                static_cast<void>(LdEvent::send_event(LdData(1), dispatcher));
                std::this_thread::yield();
            }
        });

    int rounds{ 0 };
    const auto until{ std::chrono::steady_clock::now() + _stressTime };
    while (std::chrono::steady_clock::now() < until)
    {
        auto churn{ std::make_unique<LdListener>() };
        ASSERT_TRUE(LdEvent::add_listener(*churn, dispatcher));
        std::this_thread::yield();
        static_cast<void>(LdEvent::remove_listener(*churn, dispatcher));
        ++rounds;
    }

    stop = true;
    ASSERT_EQ(sender.wait_for(_waitLimit), std::future_status::ready);
    EXPECT_GT(rounds, 0);
    EXPECT_GT(steady.mCalls.load(), 0);

    static_cast<void>(LdEvent::remove_listener(steady, dispatcher));
    dispatcher.shutdown(areg::WAIT_INFINITE);
}

/**
 * \brief   PlainSpinLock gives mutual exclusion to threads that contend on it.
 **/
TEST(PlainSpinLockTest, MutualExclusionUnderContention)
{
    constexpr int threads{ 4 };
    constexpr int rounds{ 20000 };
    areg::PlainSpinLock lock;
    int counter{ 0 };

    std::vector<std::thread> workers;
    for (int t = 0; t < threads; ++t)
    {
        workers.emplace_back([&lock, &counter]()
            {
                for (int i = 0; i < rounds; ++i)
                {
                    std::lock_guard<areg::PlainSpinLock> guard(lock);
                    ++counter;
                }
            });
    }

    for (auto & worker : workers)
    {
        worker.join();
    }

    EXPECT_EQ(counter, threads * rounds);
}
