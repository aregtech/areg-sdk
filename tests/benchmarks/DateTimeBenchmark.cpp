/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        benchmarks/DateTimeBenchmark.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Aregtech
 * \brief       Areg Platform, benchmarks of the 'areg::DateTime' operations.
 *              Every log message and every timer of the framework stamps
 *              and formats the time, so these calls are on the hot path.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include <benchmark/benchmark.h>

#include "benchmarks/BenchmarkUtils.hpp"

#include "areg/base/DateTime.hpp"
#include "areg/base/String.hpp"
#include "areg/base/UtilityDefs.hpp"

#include <vector>

namespace bench
{
    //!< A fixed timestamp, so that the measurements do not depend on the current time.
    constexpr TIME64    FIXED_TIMESTAMP { 1735689600000000LL };  // 2025-01-01 00:00:00 UTC, in microseconds

    //!< Builds a deterministic set of timestamps spread over a couple of years.
    inline std::vector<areg::DateTime> make_times(uint32_t count)
    {
        std::vector<areg::DateTime> result;
        result.reserve(count);
        std::mt19937 rnd{ bench::generator() };
        for (uint32_t i = 0; i < count; ++i)
        {
            result.emplace_back(FIXED_TIMESTAMP + static_cast<TIME64>(rnd()));
        }

        return result;
    }

} // namespace bench

//!< Reads the current timestamp, the very first operation of every log message.
void BM_DateTime_Now(benchmark::State& state)
{
    for (auto _ : state)
    {
        for (uint32_t i = 0; i < 256u; ++i)
        {
            areg::DateTime now{ areg::DateTime::now() };
            benchmark::DoNotOptimize(now);
        }
    }
}
BENCHMARK(BM_DateTime_Now);

//!< Splits the timestamp into the calendar fields, the arithmetic used by the formatters.
void BM_DateTime_Decompose(benchmark::State& state)
{
    const std::vector<areg::DateTime> times{ bench::make_times(1024u) };

    for (auto _ : state)
    {
        uint32_t sum{ 0 };
        for (const areg::DateTime& entry : times)
        {
            sum += entry.year()
                 + entry.month()
                 + entry.day()
                 + entry.hours()
                 + entry.minutes()
                 + entry.seconds()
                 + entry.milliseconds()
                 + entry.day_of_week();
        }

        benchmark::DoNotOptimize(sum);
    }
}
BENCHMARK(BM_DateTime_Decompose);

//!< Converts the timestamp into the calendar time structure.
void BM_DateTime_ToCalendarTime(benchmark::State& state)
{
    std::vector<areg::DateTime> times{ bench::make_times(1024u) };

    for (auto _ : state)
    {
        areg::CalendarTime calendar{ };
        for (areg::DateTime& entry : times)
        {
            entry.date_time(calendar);
            benchmark::DoNotOptimize(calendar);
        }
    }
}
BENCHMARK(BM_DateTime_ToCalendarTime);

//!< Formats the timestamp into the string, the operation performed for each log entry.
void BM_DateTime_FormatTime(benchmark::State& state)
{
    const std::vector<areg::DateTime> times{ bench::make_times(static_cast<uint32_t>(state.range(0))) };

    for (auto _ : state)
    {
        for (const areg::DateTime& entry : times)
        {
            areg::String formatted{ entry.format_time() };
            benchmark::DoNotOptimize(formatted);
        }

        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_DateTime_FormatTime)->Arg(64)->Arg(512);

//!< Compares the timestamps, the operation used when the timers are queued.
void BM_DateTime_Compare(benchmark::State& state)
{
    const std::vector<areg::DateTime> times{ bench::make_times(4096u) };

    for (auto _ : state)
    {
        uint32_t greater{ 0 };
        for (uint32_t i = 1; i < times.size(); ++i)
        {
            greater += times[i - 1] > times[i] ? 1u : 0u;
        }

        benchmark::DoNotOptimize(greater);
    }
}
BENCHMARK(BM_DateTime_Compare);

