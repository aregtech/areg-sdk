/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        benchmarks/StringBenchmark.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Aregtech
 * \brief       Areg Platform, benchmarks of the 'areg::String' operations.
 *              The strings are used all over the framework: service and
 *              component names, addresses, log scopes and message payloads.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include <benchmark/benchmark.h>

#include "benchmarks/BenchmarkUtils.hpp"

#include "areg/base/MathDefs.hpp"
#include "areg/base/String.hpp"

#include <string>
#include <vector>

//////////////////////////////////////////////////////////////////////////
// Construction and modification
//////////////////////////////////////////////////////////////////////////

//!< Creates strings out of a 'const char *' source, the most common way to build a string.
void BM_String_ConstructFromChars(benchmark::State& state)
{
    const std::vector<areg::String> src{ bench::make_strings(static_cast<uint32_t>(state.range(0))) };

    for (auto _ : state)
    {
        for (const areg::String& entry : src)
        {
            areg::String copy(entry.as_string());
            benchmark::DoNotOptimize(copy);
        }

        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_String_ConstructFromChars)->Arg(256)->Arg(2048);

//!< Copies existing strings, i.e. measures the cost of the buffer duplication.
void BM_String_Copy(benchmark::State& state)
{
    const std::vector<areg::String> src{ bench::make_strings(1024u) };

    for (auto _ : state)
    {
        for (const areg::String& entry : src)
        {
            areg::String copy(entry);
            benchmark::DoNotOptimize(copy);
        }

        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_String_Copy);

//!< Builds a single string by appending elements one by one, triggers the buffer reallocation.
void BM_String_Append(benchmark::State& state)
{
    const std::vector<areg::String> src{ bench::make_strings(static_cast<uint32_t>(state.range(0))) };

    for (auto _ : state)
    {
        areg::String result;
        for (const areg::String& entry : src)
        {
            result.append(entry).append('/');
        }

        benchmark::DoNotOptimize(result);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_String_Append)->Arg(256)->Arg(2048);

//!< Concatenates strings by the '+' operator, each step creates a temporary object.
void BM_String_Concatenate(benchmark::State& state)
{
    const std::vector<areg::String> src{ bench::make_strings(256u) };

    for (auto _ : state)
    {
        areg::String result;
        for (const areg::String& entry : src)
        {
            result = result + entry + ':';
        }

        benchmark::DoNotOptimize(result);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_String_Concatenate);

//!< Formats a string, the operation used by the logging and the debug output.
void BM_String_Format(benchmark::State& state)
{
    for (auto _ : state)
    {
        for (int i = 0; i < 256; ++i)
        {
            areg::String result;
            result.format("component_%d.service_%d[%s]", i, i * 3, "remote");
            benchmark::DoNotOptimize(result);
        }

        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_String_Format);

//////////////////////////////////////////////////////////////////////////
// Search, compare and transform
//////////////////////////////////////////////////////////////////////////

//!< Searches a phrase in a large text, scanning it from the beginning to the end.
void BM_String_FindPhrase(benchmark::State& state)
{
    const areg::String text{ bench::make_text(4096u) };

    for (auto _ : state)
    {
        areg::CharPos pos{ areg::START_POS };
        uint32_t found{ 0 };
        do
        {
            pos = text.find_first("areg", pos);
            if (pos == areg::INVALID_POS)
            {
                break;
            }

            ++found;
            pos += 4u;
        } while (true);

        benchmark::DoNotOptimize(found);
    }
}
BENCHMARK(BM_String_FindPhrase);

//!< Searches a phrase backwards, starting at the end of the text.
void BM_String_FindLast(benchmark::State& state)
{
    const areg::String text{ bench::make_text(4096u) };

    for (auto _ : state)
    {
        areg::CharPos pos = text.find_last("areg");
        benchmark::DoNotOptimize(pos);
    }
}
BENCHMARK(BM_String_FindLast);

//!< Replaces every occurrence of a phrase, the buffer is rebuilt on each match.
void BM_String_ReplaceAll(benchmark::State& state)
{
    const areg::String text{ bench::make_text(2048u) };

    for (auto _ : state)
    {
        areg::String copy(text);
        copy.replace("areg", "areg-sdk");
        benchmark::DoNotOptimize(copy);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_String_ReplaceAll);

//!< Compares strings, both case sensitive and case insensitive.
void BM_String_Compare(benchmark::State& state)
{
    const std::vector<areg::String> src{ bench::make_strings(1024u) };
    const bool caseSensitive{ state.range(0) != 0 };

    for (auto _ : state)
    {
        int32_t sum{ 0 };
        for (uint32_t i = 1; i < src.size(); ++i)
        {
            sum += static_cast<int32_t>(src[i - 1].compare(src[i], caseSensitive));
        }

        benchmark::DoNotOptimize(sum);
    }
}
BENCHMARK(BM_String_Compare)->Arg(1)->Arg(0);

//!< Converts the text to the upper case, in place.
void BM_String_MakeUpper(benchmark::State& state)
{
    const areg::String text{ bench::make_text(2048u) };

    for (auto _ : state)
    {
        areg::String copy(text);
        copy.make_upper();
        benchmark::DoNotOptimize(copy);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_String_MakeUpper);

//!< Splits the text by the space delimiter, the operation used by the command line parsers.
void BM_String_Split(benchmark::State& state)
{
    const areg::String text{ bench::make_text(2048u) };

    for (auto _ : state)
    {
        auto parts = text.split(' ');
        benchmark::DoNotOptimize(parts);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_String_Split);

//!< Extracts sub-strings, the operation used when parsing the service addresses.
void BM_String_Substring(benchmark::State& state)
{
    const areg::String text{ bench::make_text(1024u) };

    for (auto _ : state)
    {
        areg::String result;
        areg::CharPos pos{ areg::START_POS };
        uint32_t count{ 0 };
        while (pos != areg::INVALID_POS)
        {
            pos = text.substring(result, " ", pos);
            ++count;
        }

        benchmark::DoNotOptimize(count);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_String_Substring);

//////////////////////////////////////////////////////////////////////////
// Conversions and hashing
//////////////////////////////////////////////////////////////////////////

//!< Converts numbers into strings, used when generating identifiers and log messages.
void BM_String_FromNumber(benchmark::State& state)
{
    const std::vector<int> numbers{ bench::make_integers(1024u) };

    for (auto _ : state)
    {
        for (int number : numbers)
        {
            areg::String result{ areg::String::make_string(static_cast<int32_t>(number)) };
            benchmark::DoNotOptimize(result);
        }

        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_String_FromNumber);

//!< Parses the numbers back from the string representation.
void BM_String_ToNumber(benchmark::State& state)
{
    const std::vector<int> numbers{ bench::make_integers(1024u) };
    std::vector<areg::String> src;
    src.reserve(numbers.size());
    for (int number : numbers)
    {
        src.push_back(areg::String::make_string(static_cast<int32_t>(number)));
    }

    for (auto _ : state)
    {
        int64_t sum{ 0 };
        for (const areg::String& entry : src)
        {
            sum += entry.to_int32();
        }

        benchmark::DoNotOptimize(sum);
    }
}
BENCHMARK(BM_String_ToNumber);

//!< Computes the CRC32 checksum of the strings, the base of the string hashing in the framework.
void BM_String_Crc32(benchmark::State& state)
{
    const std::vector<areg::String> src{ bench::make_strings(1024u) };

    for (auto _ : state)
    {
        uint32_t crc{ 0 };
        for (const areg::String& entry : src)
        {
            crc ^= areg::crc32_calculate(entry.as_string());
        }

        benchmark::DoNotOptimize(crc);
    }
}
BENCHMARK(BM_String_Crc32);

