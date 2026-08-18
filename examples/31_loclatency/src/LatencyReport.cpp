/************************************************************************
 * \file        src/LatencyReport.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              Sample collection and result reporting of the local latency
 *              benchmark (example 31).
 ************************************************************************/

#include "src/LatencyReport.hpp"

#include "areg/base/DateTime.hpp"
#include "areg/base/File.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>

namespace
{
    constexpr double NS_PER_US{ 1000.0 };

    double _percentile_us(const std::vector<int64_t> & sorted, double fraction)
    {
        if (sorted.empty())
            return 0.0;

        size_t index{ static_cast<size_t>(fraction * static_cast<double>(sorted.size())) };
        if (index >= sorted.size())
            index = sorted.size() - 1u;

        return static_cast<double>(sorted[index]) / NS_PER_US;
    }

    const char * _build_type()
    {
#if defined(DEBUG) || defined(_DEBUG)
        return "debug";
#else
        return "release";
#endif
    }

    const char * _platform()
    {
#if defined(_WIN32)
        return "windows";
#elif defined(__APPLE__)
        return "macos";
#elif defined(__linux__)
        return "linux";
#else
        return "posix";
#endif
    }

    const char * _compiler()
    {
#if defined(_MSC_VER)
        return "msvc";
#elif defined(__clang__)
        return "clang";
#elif defined(__GNUC__)
        return "gnu";
#else
        return "unknown";
#endif
    }
}

void loclat::LatencyReport::restart(uint32_t expected)
{
    mTotalNs.clear();
    mInLegNs.clear();
    mTotalNs.reserve(expected);
    mInLegNs.reserve(expected);
}

loclat::RunResult loclat::LatencyReport::summarize( const loclat::RunOptions & options
                                                  , LocalLatency::LatencyMode mode
                                                  , uint32_t repeat
                                                  , uint64_t durationNs
                                                  , double clockCostNs )
{
    loclat::RunResult result;
    result.mTopology    = options.mTopology;
    result.mMode        = mode;
    result.mRepeat      = repeat;
    result.mPayload     = loclat::mode_payload_size(mode);
    result.mWarmup      = options.mWarmup;
    result.mSamples     = count();
    result.mClockCostNs = clockCostNs;
    result.mDurationMs  = static_cast<double>(durationNs) / 1000000.0;

    if (result.mSamples == 0u)
        return result;

    double sum{ 0.0 };
    for (const int64_t value : mTotalNs)
        sum += static_cast<double>(value);

    result.mMeanUs = (sum / static_cast<double>(result.mSamples)) / NS_PER_US;

    double variance{ 0.0 };
    for (const int64_t value : mTotalNs)
    {
        const double diff{ (static_cast<double>(value) / NS_PER_US) - result.mMeanUs };
        variance += diff * diff;
    }

    result.mStdDevUs = std::sqrt(variance / static_cast<double>(result.mSamples));

    std::sort(mTotalNs.begin(), mTotalNs.end());
    result.mMinUs  = static_cast<double>(mTotalNs.front()) / NS_PER_US;
    result.mMaxUs  = static_cast<double>(mTotalNs.back()) / NS_PER_US;
    result.mP50Us  = _percentile_us(mTotalNs, 0.50);
    result.mP90Us  = _percentile_us(mTotalNs, 0.90);
    result.mP99Us  = _percentile_us(mTotalNs, 0.99);
    result.mP999Us = _percentile_us(mTotalNs, 0.999);

    if (loclat::is_round_trip(mode))
    {
        std::sort(mInLegNs.begin(), mInLegNs.end());
        result.mInLegP50Us = _percentile_us(mInLegNs, 0.50);
    }

    if (result.mDurationMs > 0.0)
        result.mMsgPerSec = (static_cast<double>(result.mSamples) * 1000.0) / result.mDurationMs;

    return result;
}

void loclat::print_environment(double clockCostNs)
{
    std::printf("Environment : platform=%s compiler=%s build=%s pointer=%u-bit\n"
              , _platform(), _compiler(), _build_type(), static_cast<uint32_t>(sizeof(void *) * 8u));
    std::printf("Clock cost  : %.1f ns per reading (two readings per sample)\n\n", clockCostNs);
}

void loclat::print_table_header()
{
    std::printf(" topo  | mode     | bytes | rep |  samples |     min |     p50 |     p90 |     p99 |     p99.9 |       max |    mean |  stddev |   in-leg |      msg/s\n");
    std::printf(" ------+----------+-------+-----+----------+---------+---------+---------+---------+-----------+-----------+---------+---------+----------+-----------\n");
}

void loclat::print_table_row(const loclat::RunResult & result)
{
    std::printf(" %-5s | %-8s | %5u | %3u | %8u | %7.3f | %7.3f | %7.3f | %7.3f | %9.3f | %9.3f | %7.3f | %7.3f | %8.3f | %10.0f\n"
              , loclat::topology_as_str(result.mTopology)
              , loclat::mode_as_str(result.mMode)
              , result.mPayload
              , result.mRepeat
              , result.mSamples
              , result.mMinUs
              , result.mP50Us
              , result.mP90Us
              , result.mP99Us
              , result.mP999Us
              , result.mMaxUs
              , result.mMeanUs
              , result.mStdDevUs
              , result.mInLegP50Us
              , result.mMsgPerSec);
    std::fflush(stdout);
}

bool loclat::append_csv(const areg::String & path, const areg::String & label, const loclat::RunResult & result)
{
    if (path.is_empty())
        return false;

    const bool exists{ areg::File::has_file(path) };
    std::FILE * file{ nullptr };
#if defined(_MSC_VER)
    if (fopen_s(&file, path.as_string(), "a") != 0)
        file = nullptr;
#else
    file = std::fopen(path.as_string(), "a");
#endif
    if (file == nullptr)
    {
        std::printf("ERROR: cannot open the CSV file '%s' for writing.\n", path.as_string());
        return false;
    }

    if (!exists)
    {
        std::fprintf(file
                   , "timestamp,label,platform,compiler,build,topology,mode,payload_bytes,repeat,samples,warmup,"
                     "min_us,p50_us,p90_us,p99_us,p999_us,max_us,mean_us,stddev_us,inleg_p50_us,duration_ms,msg_per_sec,clock_cost_ns\n");
    }

    std::fprintf(file
               , "%s,%s,%s,%s,%s,%s,%s,%u,%u,%u,%u,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.0f,%.1f\n"
               , areg::DateTime::now().format_time("%Y-%m-%d %H:%M:%S").as_string()
               , label.as_string()
               , _platform()
               , _compiler()
               , _build_type()
               , loclat::topology_as_str(result.mTopology)
               , loclat::mode_as_str(result.mMode)
               , result.mPayload
               , result.mRepeat
               , result.mSamples
               , result.mWarmup
               , result.mMinUs
               , result.mP50Us
               , result.mP90Us
               , result.mP99Us
               , result.mP999Us
               , result.mMaxUs
               , result.mMeanUs
               , result.mStdDevUs
               , result.mInLegP50Us
               , result.mDurationMs
               , result.mMsgPerSec
               , result.mClockCostNs);

    std::fclose(file);
    return true;
}
