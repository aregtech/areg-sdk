/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        benchmarks/SharedBufferBenchmark.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Aregtech
 * \brief       Areg Platform, benchmarks of the 'areg::SharedBuffer' streaming.
 *              Every request, response and event of the framework is
 *              serialized into a shared buffer, so this is the hottest path
 *              of the inter-process communication.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include <benchmark/benchmark.h>

#include "benchmarks/BenchmarkUtils.hpp"

#include "areg/base/ArrayList.hpp"
#include "areg/base/Containers.hpp"
#include "areg/base/SharedBuffer.hpp"
#include "areg/base/String.hpp"

#include <vector>

//!< Writes the plain 32-bit values into the buffer, the buffer grows by blocks.
void BM_SharedBuffer_WriteIntegers(benchmark::State& state)
{
    const std::vector<int> src{ bench::make_integers(static_cast<uint32_t>(state.range(0))) };

    for (auto _ : state)
    {
        areg::SharedBuffer buffer;
        for (int value : src)
        {
            buffer << static_cast<int32_t>(value);
        }

        benchmark::DoNotOptimize(buffer);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_SharedBuffer_WriteIntegers)->Arg(1024)->Arg(8192);

//!< Reads the plain 32-bit values back from the buffer.
void BM_SharedBuffer_ReadIntegers(benchmark::State& state)
{
    const std::vector<int> src{ bench::make_integers(4096u) };
    areg::SharedBuffer buffer;
    for (int value : src)
    {
        buffer << static_cast<int32_t>(value);
    }

    for (auto _ : state)
    {
        buffer.reset();
        int64_t sum{ 0 };
        for (uint32_t i = 0; i < src.size(); ++i)
        {
            int32_t value{ 0 };
            buffer >> value;
            sum += value;
        }

        benchmark::DoNotOptimize(sum);
    }
}
BENCHMARK(BM_SharedBuffer_ReadIntegers);

//!< Writes the strings into the buffer, each string is serialized with its terminator.
void BM_SharedBuffer_WriteStrings(benchmark::State& state)
{
    const std::vector<areg::String> src{ bench::make_strings(1024u) };

    for (auto _ : state)
    {
        areg::SharedBuffer buffer;
        for (const areg::String& entry : src)
        {
            buffer << entry;
        }

        benchmark::DoNotOptimize(buffer);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_SharedBuffer_WriteStrings);

//!< Reads the strings back from the buffer, each read allocates a new string.
void BM_SharedBuffer_ReadStrings(benchmark::State& state)
{
    const std::vector<areg::String> src{ bench::make_strings(1024u) };
    areg::SharedBuffer buffer;
    for (const areg::String& entry : src)
    {
        buffer << entry;
    }

    for (auto _ : state)
    {
        buffer.reset();
        uint32_t total{ 0 };
        for (uint32_t i = 0; i < src.size(); ++i)
        {
            areg::String value;
            buffer >> value;
            total += static_cast<uint32_t>(value.length());
        }

        benchmark::DoNotOptimize(total);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_SharedBuffer_ReadStrings);

//!< Writes raw binary blocks, the way the socket data is pushed into the buffer.
void BM_SharedBuffer_WriteRaw(benchmark::State& state)
{
    const uint32_t chunk{ static_cast<uint32_t>(state.range(0)) };
    const std::vector<uint8_t> data(chunk, 0xA5u);

    for (auto _ : state)
    {
        areg::SharedBuffer buffer;
        for (uint32_t i = 0; i < 128u; ++i)
        {
            buffer.write(data.data(), chunk);
        }

        benchmark::DoNotOptimize(buffer);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_SharedBuffer_WriteRaw)->Arg(64)->Arg(1024);

//!< Serializes a complete array of strings, the typical service call parameter.
void BM_SharedBuffer_SerializeStringArray(benchmark::State& state)
{
    const std::vector<areg::String> src{ bench::make_strings(1024u) };
    areg::StringArray list;
    for (const areg::String& entry : src)
    {
        list.add(entry);
    }

    for (auto _ : state)
    {
        areg::SharedBuffer buffer;
        buffer << list;
        benchmark::DoNotOptimize(buffer);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_SharedBuffer_SerializeStringArray);

//!< Deserializes a complete array of strings out of the buffer.
void BM_SharedBuffer_DeserializeStringArray(benchmark::State& state)
{
    const std::vector<areg::String> src{ bench::make_strings(1024u) };
    areg::StringArray list;
    for (const areg::String& entry : src)
    {
        list.add(entry);
    }

    areg::SharedBuffer buffer;
    buffer << list;

    for (auto _ : state)
    {
        buffer.reset();
        areg::StringArray result;
        buffer >> result;
        benchmark::DoNotOptimize(result);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_SharedBuffer_DeserializeStringArray);

//!< Serializes a hash map with the string keys, used when passing the tables between processes.
void BM_SharedBuffer_SerializeStringMap(benchmark::State& state)
{
    const std::vector<areg::String> src{ bench::make_strings(1024u) };
    areg::StringToStringHashMap map;
    for (const areg::String& entry : src)
    {
        map.set_value_at(entry, entry);
    }

    for (auto _ : state)
    {
        areg::SharedBuffer buffer;
        buffer << map;
        benchmark::DoNotOptimize(buffer);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_SharedBuffer_SerializeStringMap);

//!< Copies the shared buffer, the copy shares the underlying data and only bumps the reference.
void BM_SharedBuffer_Share(benchmark::State& state)
{
    const std::vector<int> src{ bench::make_integers(4096u) };
    areg::SharedBuffer origin;
    for (int value : src)
    {
        origin << static_cast<int32_t>(value);
    }

    for (auto _ : state)
    {
        for (uint32_t i = 0; i < 256u; ++i)
        {
            areg::SharedBuffer copy(origin);
            benchmark::DoNotOptimize(copy);
        }

        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_SharedBuffer_Share);

