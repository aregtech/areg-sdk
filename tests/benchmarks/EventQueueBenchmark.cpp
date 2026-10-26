/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        benchmarks/EventQueueBenchmark.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Aregtech
 * \brief       Areg Platform, benchmarks of the priority lane of the event queue,
 *              and a probe of the processor clock logged with every run.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include <benchmark/benchmark.h>

#include "areg/base/Thread.hpp"
#include "areg/component/Event.hpp"
#include "areg/component/private/EventQueue.hpp"

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>

namespace
{
    //!< The ring capacity of the measured queues.
    constexpr uint32_t  RING_CAPACITY   { 1024u };

    //!< The most events the producers may keep in the priority lane before they wait.
    constexpr uint64_t  LANE_LIMIT      { 256u };

    //!< The most events the consumer takes in one call.
    constexpr uint32_t  POP_BATCH       { 64u };

    //!< Returns a new high priority event without data.
    inline areg::Event make_high_event()
    {
        return areg::Event(areg::EventType::EventCustomExternal, areg::EventPriority::HighPrio);
    }

    //!< A queue with one consumer thread that drains the priority lane while producers push.
    class DrainedQueue
    {
    public:
        DrainedQueue()
            : mQueue    (RING_CAPACITY)
            , mPushed   (0u)
            , mPopped   (0u)
            , mWaiting  (0u)
            , mStop     (false)
        {
            mQueue.acquire_lanes();
            mConsumer = std::thread([this]() { _drain(); });
        }

        ~DrainedQueue()
        {
            mStop.store(true, std::memory_order_release);
            mQueue.exit_queue(true);
            mConsumer.join();
            mQueue.remove_all_events();
        }

        //!< Pushes one high priority event, waiting while the lane holds LANE_LIMIT events.
        void push()
        {
            if ((mPushed.load(std::memory_order_relaxed) - mPopped.load(std::memory_order_relaxed)) >= LANE_LIMIT)
            {
                std::unique_lock<std::mutex> lock(mLock);
                mWaiting.fetch_add(1u, std::memory_order_seq_cst);
                mDrained.wait(lock, [this]() { return ((mPushed.load(std::memory_order_relaxed) - mPopped.load(std::memory_order_relaxed)) < LANE_LIMIT); });
                mWaiting.fetch_sub(1u, std::memory_order_relaxed);
            }

            mPushed.fetch_add(1u, std::memory_order_relaxed);
            areg::Event event{ make_high_event() };
            static_cast<void>(mQueue.push_event(event));
        }

    private:
        //!< Pops the events in batches until the queue is stopped.
        void _drain()
        {
            std::unique_ptr<areg::Event[]> batch{ std::make_unique<areg::Event[]>(POP_BATCH) };
            while (mStop.load(std::memory_order_acquire) == false)
            {
                if (mQueue.wait_event(areg::WAIT_INFINITE) == false)
                    continue;

                const uint32_t count{ mQueue.pop_events(batch.get(), POP_BATCH) };
                for (uint32_t i = 0u; i < count; ++i)
                {
                    batch[i] = areg::Event();
                }

                mPopped.fetch_add(count, std::memory_order_seq_cst);
                if (mWaiting.load(std::memory_order_seq_cst) != 0u)
                {
                    { std::lock_guard<std::mutex> lock(mLock); }
                    mDrained.notify_all();
                }
            }
        }

        areg::EventQueue        mQueue;
        std::atomic<uint64_t>   mPushed;
        std::atomic<uint64_t>   mPopped;
        std::atomic<uint32_t>   mWaiting;
        std::atomic<bool>       mStop;
        std::mutex              mLock;
        std::condition_variable mDrained;
        std::thread             mConsumer;
    };

    //!< The queue shared by the threads of one producer benchmark.
    DrainedQueue *  _drainedQueue   { nullptr };
}

//////////////////////////////////////////////////////////////////////////
// Clock probe
//////////////////////////////////////////////////////////////////////////

//!< The cost of one processor pause instruction: a probe of the clock speed of the run.
void BM_ClockProbe_Pause(benchmark::State& state)
{
    for (auto _ : state)
    {
        areg::Thread::cpu_pause();
    }
}
BENCHMARK(BM_ClockProbe_Pause);

//////////////////////////////////////////////////////////////////////////
// Priority lane, one thread
//////////////////////////////////////////////////////////////////////////

//!< One high priority event pushed and popped by one thread.
void BM_EventQueue_HighPrio_PushPop(benchmark::State& state)
{
    areg::EventQueue queue(RING_CAPACITY);
    queue.acquire_lanes();
    for (auto _ : state)
    {
        areg::Event event{ make_high_event() };
        static_cast<void>(queue.push_event(event));
        benchmark::DoNotOptimize(queue.pop_event());
    }

    queue.remove_all_events();
}
BENCHMARK(BM_EventQueue_HighPrio_PushPop);

//!< One high priority event pushed and one popped while the lane holds state.range(0) events.
//!< The cost must not grow with the depth of the lane.
void BM_EventQueue_HighPrio_PushAtDepth(benchmark::State& state)
{
    const uint32_t depth{ static_cast<uint32_t>(state.range(0)) };
    areg::EventQueue queue(RING_CAPACITY);
    queue.acquire_lanes();
    for (uint32_t i = 0u; i < depth; ++i)
    {
        areg::Event event{ make_high_event() };
        static_cast<void>(queue.push_event(event));
    }

    for (auto _ : state)
    {
        areg::Event event{ make_high_event() };
        static_cast<void>(queue.push_event(event));
        benchmark::DoNotOptimize(queue.pop_event());
    }

    queue.remove_all_events();
}
BENCHMARK(BM_EventQueue_HighPrio_PushAtDepth)->Arg(16)->Arg(2048)->Arg(32768);

//////////////////////////////////////////////////////////////////////////
// Priority lane, several producers and one consumer
//////////////////////////////////////////////////////////////////////////

//!< Every benchmark thread pushes high priority events into one queue that a consumer drains.
void BM_EventQueue_HighPrio_Producers(benchmark::State& state)
{
    if (state.thread_index() == 0)
    {
        _drainedQueue = new DrainedQueue();
    }

    for (auto _ : state)
    {
        _drainedQueue->push();
    }

    if (state.thread_index() == 0)
    {
        delete _drainedQueue;
        _drainedQueue = nullptr;
    }
}
#if !defined(CODSPEED_ANALYSIS)
// The CodSpeed simulation runs one thread at a time, and this case needs the consumer thread.
BENCHMARK(BM_EventQueue_HighPrio_Producers)->Threads(1)->Threads(2)->Threads(4)->UseRealTime();
#endif  // !defined(CODSPEED_ANALYSIS)
