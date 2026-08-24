/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        benchmarks/ContainerBenchmark.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Aregtech
 * \brief       Areg Platform, benchmarks of the Areg Framework containers.
 *              The containers hold the registered services, the proxies,
 *              the timers and the log scopes, so their behavior directly
 *              influences the runtime of the whole framework.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include <benchmark/benchmark.h>

#include "benchmarks/BenchmarkUtils.hpp"

#include "areg/base/ArrayList.hpp"
#include "areg/base/Containers.hpp"
#include "areg/base/HashMap.hpp"
#include "areg/base/LinkedList.hpp"
#include "areg/base/OrderedMap.hpp"
#include "areg/base/RingStack.hpp"
#include "areg/base/SortedLinkedList.hpp"
#include "areg/base/Stack.hpp"
#include "areg/base/String.hpp"

#include <vector>

//////////////////////////////////////////////////////////////////////////
// ArrayList
//////////////////////////////////////////////////////////////////////////

//!< Appends the elements one by one, the array grows and reallocates the buffer.
void BM_ArrayList_Add(benchmark::State& state)
{
    const std::vector<int> src{ bench::make_integers(static_cast<uint32_t>(state.range(0))) };

    for (auto _ : state)
    {
        areg::ArrayList<int> list;
        for (int value : src)
        {
            list.add(value);
        }

        benchmark::DoNotOptimize(list);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_ArrayList_Add)->Arg(1024)->Arg(8192);

//!< Adds only the unique elements, every insert performs a linear search first.
void BM_ArrayList_AddIfUnique(benchmark::State& state)
{
    const std::vector<int> src{ bench::make_integers(512u) };

    for (auto _ : state)
    {
        areg::ArrayList<int> list;
        for (int value : src)
        {
            list.add_if_unique(value);
        }

        benchmark::DoNotOptimize(list);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_ArrayList_AddIfUnique);

//!< Linear search of the elements that are not in the list, i.e. the worst case.
void BM_ArrayList_Contains(benchmark::State& state)
{
    const std::vector<int> src{ bench::make_integers(4096u) };
    areg::ArrayList<int> list;
    for (int value : src)
    {
        list.add(value);
    }

    for (auto _ : state)
    {
        uint32_t found{ 0 };
        for (int i = 0; i < 128; ++i)
        {
            found += list.contains(-i) ? 1u : 0u;
        }

        benchmark::DoNotOptimize(found);
    }
}
BENCHMARK(BM_ArrayList_Contains);

//!< Inserts in the middle of the array, every insert shifts the tail of the buffer.
void BM_ArrayList_InsertAt(benchmark::State& state)
{
    const std::vector<int> src{ bench::make_integers(1024u) };

    for (auto _ : state)
    {
        areg::ArrayList<int> list;
        for (int value : src)
        {
            list.insert_at(list.size() / 2u, value);
        }

        benchmark::DoNotOptimize(list);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_ArrayList_InsertAt);

//!< Sorts the array of integers in the ascending order.
void BM_ArrayList_Sort(benchmark::State& state)
{
    const std::vector<int> src{ bench::make_integers(4096u) };
    areg::ArrayList<int> origin;
    for (int value : src)
    {
        origin.add(value);
    }

    for (auto _ : state)
    {
        areg::ArrayList<int> list(origin);
        list.sort([](int lhs, int rhs) { return lhs < rhs; });
        benchmark::DoNotOptimize(list);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_ArrayList_Sort);

//!< Copies the array of strings, i.e. copies every single string entry.
void BM_ArrayList_CopyStrings(benchmark::State& state)
{
    const std::vector<areg::String> src{ bench::make_strings(1024u) };
    areg::StringArray origin;
    for (const areg::String& entry : src)
    {
        origin.add(entry);
    }

    for (auto _ : state)
    {
        areg::StringArray copy(origin);
        benchmark::DoNotOptimize(copy);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_ArrayList_CopyStrings);

//////////////////////////////////////////////////////////////////////////
// LinkedList and SortedLinkedList
//////////////////////////////////////////////////////////////////////////

//!< Pushes the elements at the tail of the doubly linked list.
void BM_LinkedList_PushLast(benchmark::State& state)
{
    const std::vector<int> src{ bench::make_integers(2048u) };

    for (auto _ : state)
    {
        areg::LinkedList<int> list;
        for (int value : src)
        {
            list.push_last(value);
        }

        benchmark::DoNotOptimize(list);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_LinkedList_PushLast);

//!< Walks over the linked list positions, the usual way to process the queued entries.
void BM_LinkedList_Traverse(benchmark::State& state)
{
    const std::vector<int> src{ bench::make_integers(2048u) };
    areg::LinkedList<int> list;
    for (int value : src)
    {
        list.push_last(value);
    }

    for (auto _ : state)
    {
        int64_t sum{ 0 };
        areg::LinkedList<int>::LISTPOS pos = list.first_position();
        while (list.is_valid_position(pos))
        {
            sum += list.value_at(pos);
            pos = list.next_position(pos);
        }

        benchmark::DoNotOptimize(sum);
    }
}
BENCHMARK(BM_LinkedList_Traverse);

//!< Inserts the elements keeping the list sorted, every insert searches the position.
void BM_SortedLinkedList_Add(benchmark::State& state)
{
    const std::vector<int> src{ bench::make_integers(static_cast<uint32_t>(state.range(0))) };

    for (auto _ : state)
    {
        areg::SortedLinkedList<int> list(true);
        for (int value : src)
        {
            list.add(value);
        }

        benchmark::DoNotOptimize(list);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_SortedLinkedList_Add)->Arg(256)->Arg(1024);

//////////////////////////////////////////////////////////////////////////
// HashMap and OrderedMap
//////////////////////////////////////////////////////////////////////////

//!< Fills the hash map with the integer keys.
void BM_HashMap_InsertInteger(benchmark::State& state)
{
    const std::vector<int> src{ bench::make_integers(static_cast<uint32_t>(state.range(0))) };

    for (auto _ : state)
    {
        areg::HashMap<int, int> map;
        for (int value : src)
        {
            map.set_value_at(value, value);
        }

        benchmark::DoNotOptimize(map);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_HashMap_InsertInteger)->Arg(1024)->Arg(8192);

//!< Looks up every key of the hash map with the integer keys.
void BM_HashMap_FindInteger(benchmark::State& state)
{
    const std::vector<int> src{ bench::make_integers(4096u) };
    areg::HashMap<int, int> map;
    for (int value : src)
    {
        map.set_value_at(value, value);
    }

    for (auto _ : state)
    {
        int64_t sum{ 0 };
        for (int value : src)
        {
            int result{ 0 };
            if (map.find(value, result))
            {
                sum += result;
            }
        }

        benchmark::DoNotOptimize(sum);
    }
}
BENCHMARK(BM_HashMap_FindInteger);

//!< Fills the hash map with the string keys, i.e. the way the service registry works.
void BM_HashMap_InsertString(benchmark::State& state)
{
    const std::vector<areg::String> src{ bench::make_strings(2048u) };

    for (auto _ : state)
    {
        areg::StringToIntegerHashMap map;
        uint32_t index{ 0 };
        for (const areg::String& entry : src)
        {
            map.set_value_at(entry, index++);
        }

        benchmark::DoNotOptimize(map);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_HashMap_InsertString);

//!< Looks up every string key, the hash of the key is recalculated on each search.
void BM_HashMap_FindString(benchmark::State& state)
{
    const std::vector<areg::String> src{ bench::make_strings(2048u) };
    areg::StringToIntegerHashMap map;
    uint32_t index{ 0 };
    for (const areg::String& entry : src)
    {
        map.set_value_at(entry, index++);
    }

    for (auto _ : state)
    {
        uint64_t sum{ 0 };
        for (const areg::String& entry : src)
        {
            uint32_t result{ 0 };
            if (map.find(entry, result))
            {
                sum += result;
            }
        }

        benchmark::DoNotOptimize(sum);
    }
}
BENCHMARK(BM_HashMap_FindString);

//!< Fills the ordered (tree based) map with the string keys.
void BM_OrderedMap_InsertString(benchmark::State& state)
{
    const std::vector<areg::String> src{ bench::make_strings(2048u) };

    for (auto _ : state)
    {
        areg::StringToIntegerMap map;
        uint32_t index{ 0 };
        for (const areg::String& entry : src)
        {
            map.set_value_at(entry, index++);
        }

        benchmark::DoNotOptimize(map);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_OrderedMap_InsertString);

//!< Looks up every string key of the ordered map, i.e. performs the tree lookups.
void BM_OrderedMap_FindString(benchmark::State& state)
{
    const std::vector<areg::String> src{ bench::make_strings(2048u) };
    areg::StringToIntegerMap map;
    uint32_t index{ 0 };
    for (const areg::String& entry : src)
    {
        map.set_value_at(entry, index++);
    }

    for (auto _ : state)
    {
        uint64_t sum{ 0 };
        for (const areg::String& entry : src)
        {
            uint32_t result{ 0 };
            if (map.find(entry, result))
            {
                sum += result;
            }
        }

        benchmark::DoNotOptimize(sum);
    }
}
BENCHMARK(BM_OrderedMap_FindString);

//////////////////////////////////////////////////////////////////////////
// Stack and RingStack
//////////////////////////////////////////////////////////////////////////

//!< Pushes and pops the elements of the stack, the base of the event queues.
void BM_Stack_PushPop(benchmark::State& state)
{
    const std::vector<int> src{ bench::make_integers(2048u) };

    for (auto _ : state)
    {
        areg::Stack<int> stack;
        for (int value : src)
        {
            stack.push_last(value);
        }

        int64_t sum{ 0 };
        while (stack.is_empty() == false)
        {
            sum += stack.pop_first();
        }

        benchmark::DoNotOptimize(sum);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_Stack_PushPop);

//!< Pushes and pops the elements of the fixed size ring buffer.
void BM_RingStack_PushPop(benchmark::State& state)
{
    const std::vector<int> src{ bench::make_integers(2048u) };
    constexpr uint32_t ringSize{ 256u };

    for (auto _ : state)
    {
        areg::RingStack<int> ring(ringSize, areg::OverlapPolicy::Shift);
        int64_t sum{ 0 };
        for (int value : src)
        {
            ring.push(value);
            if (ring.is_full())
            {
                sum += ring.pop();
            }
        }

        benchmark::DoNotOptimize(sum);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_RingStack_PushPop);

