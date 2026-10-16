/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        benchmarks/LockBenchmark.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Aregtech
 * \brief       Areg Platform, benchmarks of the locks and of the locking containers.
 *              The same file builds before and after a change of the locking, so the
 *              two builds can be measured against each other.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include <benchmark/benchmark.h>

#include "areg/base/ResourceMap.hpp"
#include "areg/base/Stack.hpp"
#include "areg/base/SyncPrimitives.hpp"

#include <condition_variable>
#include <mutex>
#include <thread>

namespace
{
    //!< Returns the object through a reference the compiler cannot see through, as a caller holding a SyncObject& does.
    areg::SyncObject & opaque(areg::SyncObject & object)
    {
        areg::SyncObject * result{ &object };
        benchmark::DoNotOptimize(result);
        return *result;
    }

    //!< Keeps a second thread asleep while it exists. glibc skips the atomic operations of a mutex
    //!< while the process has one thread; an areg process always has several.
    class SecondThread
    {
    public:
        SecondThread()
            : mThread([this]() { std::unique_lock<std::mutex> lock(mLock); mWake.wait(lock, [this]() { return mStop; }); })
        {
        }

        ~SecondThread()
        {
            {
                std::lock_guard<std::mutex> lock(mLock);
                mStop = true;
            }

            mWake.notify_one();
            mThread.join();
        }

    private:
        std::mutex              mLock;
        std::condition_variable mWake;
        bool                    mStop{ false };
        std::thread             mThread;
    };

    struct Item
    {
        int value{ 0 };
    };
}

//////////////////////////////////////////////////////////////////////////
// One lock, no contention
//////////////////////////////////////////////////////////////////////////

//!< SpinLock locked and unlocked by direct calls, as a lock of a known type is.
void BM_SpinLock_Direct(benchmark::State& state)
{
    SecondThread second;
    areg::SpinLock spin;
    for (auto _ : state)
    {
        spin.lock(areg::WAIT_INFINITE);
        spin.unlock();
    }
}
BENCHMARK(BM_SpinLock_Direct);

//!< SpinLock locked by Lock through a SyncObject reference: virtual calls and the validity check.
void BM_SpinLock_Lock(benchmark::State& state)
{
    SecondThread second;
    areg::SpinLock spin;
    areg::SyncObject & sync{ opaque(spin) };
    for (auto _ : state)
    {
        areg::Lock lock(sync);
    }
}
BENCHMARK(BM_SpinLock_Lock);

//!< SpinLock taken again by its owner: the recursive path.
void BM_SpinLock_Recursive(benchmark::State& state)
{
    SecondThread second;
    areg::SpinLock spin;
    spin.lock(areg::WAIT_INFINITE);
    for (auto _ : state)
    {
        spin.lock(areg::WAIT_INFINITE);
        spin.unlock();
    }

    spin.unlock();
}
BENCHMARK(BM_SpinLock_Recursive);

//!< PlainSpinLock, the non-recursive lock with an inline fast path.
void BM_PlainSpinLock(benchmark::State& state)
{
    SecondThread second;
    areg::PlainSpinLock spin;
    for (auto _ : state)
    {
        spin.lock();
        spin.unlock();
    }
}
BENCHMARK(BM_PlainSpinLock);

//!< Mutex locked by Lock through a SyncObject reference.
void BM_Mutex_Lock(benchmark::State& state)
{
    SecondThread second;
    areg::Mutex mutex(false);
    areg::SyncObject & sync{ opaque(mutex) };
    for (auto _ : state)
    {
        areg::Lock lock(sync);
    }
}
BENCHMARK(BM_Mutex_Lock);

//!< std::mutex, the reference.
void BM_StdMutex(benchmark::State& state)
{
    SecondThread second;
    std::mutex mutex;
    for (auto _ : state)
    {
        std::lock_guard<std::mutex> lock(mutex);
    }
}
BENCHMARK(BM_StdMutex);

//////////////////////////////////////////////////////////////////////////
// Locking containers
//////////////////////////////////////////////////////////////////////////

//!< ConcurrentStack, one push and one pop.
void BM_ConcurrentStack_PushPop(benchmark::State& state)
{
    SecondThread second;
    areg::ConcurrentStack<int> stack;
    for (auto _ : state)
    {
        static_cast<void>(stack.push_last(1));
        benchmark::DoNotOptimize(stack.pop_first());
    }
}
BENCHMARK(BM_ConcurrentStack_PushPop);

//!< Stack without a lock, one push and one pop: the cost of the container alone.
void BM_NolockStack_PushPop(benchmark::State& state)
{
    SecondThread second;
    areg::Stack<int> stack;
    for (auto _ : state)
    {
        static_cast<void>(stack.push_last(1));
        benchmark::DoNotOptimize(stack.pop_first());
    }
}
BENCHMARK(BM_NolockStack_PushPop);

//!< One ConcurrentStack shared by all threads, each pushes and pops.
void BM_ConcurrentStack_Contended(benchmark::State& state)
{
    SecondThread second;
    static areg::ConcurrentStack<int> stack;
    for (auto _ : state)
    {
        static_cast<void>(stack.push_last(1));
        benchmark::DoNotOptimize(stack.pop_first());
    }
}
#if defined(CODSPEED_ANALYSIS)
// The CodSpeed simulation runs one thread at a time, so it measures only the uncontended case.
BENCHMARK(BM_ConcurrentStack_Contended);
#else   // defined(CODSPEED_ANALYSIS)
BENCHMARK(BM_ConcurrentStack_Contended)->Threads(1)->Threads(2)->Threads(4)->Threads(8)->UseRealTime();
#endif  // defined(CODSPEED_ANALYSIS)

//!< ConcurrentResourceMap, a lookup in 1024 entries.
void BM_ConcurrentResourceMap_Find(benchmark::State& state)
{
    SecondThread second;
    static Item items[1024];
    areg::ConcurrentResourceMap<uint32_t, Item *> map;
    for (uint32_t i = 0u; i < 1024u; ++i)
    {
        map.register_resource_object(i, &items[i]);
    }

    uint32_t key{ 0u };
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(map.find_resource_object(key));
        key = (key + 1u) & 1023u;
    }
}
BENCHMARK(BM_ConcurrentResourceMap_Find);
