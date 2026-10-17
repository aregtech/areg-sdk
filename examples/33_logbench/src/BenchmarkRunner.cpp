#include "src/BenchmarkRunner.hpp"
#include "src/ResultProcessor.hpp"

#include <chrono>
#include <iostream>
#include <numeric>
#include <iomanip>

DEF_LOG_SCOPE(logging_bench, test_dynamic_log);
DEF_LOG_SCOPE(logging_bench, bench_log_scope);
DEF_LOG_SCOPE(logging_bench, bench_dbg_string);
DEF_LOG_SCOPE(logging_bench, bench_dbg_1_arg);
DEF_LOG_SCOPE(logging_bench, bench_dbg_2_arg);
DEF_LOG_SCOPE(logging_bench, bench_dbg_3_mixed);
DEF_LOG_SCOPE(logging_bench, bench_dbg_10_mixed);
DEF_LOG_SCOPE(logging_bench, bench_long_string);
DEF_LOG_SCOPE(logging_bench, bench_dbg_dynamic);

namespace {

void visualise_header()
{
    std::cout
        << "+------------+----------+----------+----------+----------+----------+----------+----------+----------+\n"
        << "| Bench Type |  Min(us) |  Max(us) | Mean(us) |  P50(us) |  P90(us) |  P99(us)   P999(us) |   SD(us) |\n"
        << "+------------+----------+----------+----------+----------+----------+----------+----------+----------+\n";
}

void visualise_results(const SessionResult& results)
{
    std::string benchType;

    switch(results.sampleType) {
        case SampleType::LogScope:
            benchType = "LgScp";
            break;
        case SampleType::DebugOneArg:
            benchType = "Dbg1a";
            break;
        case SampleType::DebugTwoArg:
            benchType = "Dbg2a";
            break;
        case SampleType::DebugThreeMixedArg:
            benchType = "Dbg3ma";
            break;
        case SampleType::DebugTenMixedArg:
            benchType = "Dbg10ma";
            break;
        case SampleType::DebugLongString:
            benchType = "DbgLngStr";
            break;
        case SampleType::DebugString:
            benchType = "PureStr";
            break;
        case SampleType::DebugDynamicString:
            benchType = "DynStr";
            break;
        default:
            break;
    }

    std::cout
        << "| " << std::setw(10) << benchType
        << " | " << std::setw(8) << results.min_us
        << " | " << std::setw(8) << results.max_us
        << " | " << std::setw(8) << std::fixed << std::setprecision(2) << results.mean_us
        << " | " << std::setw(8) << results.p50_us
        << " | " << std::setw(8) << results.p90_us
        << " | " << std::setw(8) << results.p99_us
        << " | " << std::setw(8) << results.p999_us
        << " | " << std::setw(8) << results.sd_us
        << " |\n";
}

void visualise_footer()
{
    std::cout
        << "+------------+----------+----------+----------+----------+----------+----------+----------+----------+\n";
}

constexpr SampleType sampleTypeVals[] = {
    SampleType::LogScope,
    SampleType::DebugString,
    SampleType::DebugOneArg,
    SampleType::DebugTwoArg,
    SampleType::DebugThreeMixedArg,
    SampleType::DebugTenMixedArg,
    SampleType::DebugLongString,
    SampleType::DebugDynamicString,

};

}





BenchmarkRunner::BenchmarkRunner(BenchmarkConfig mCfg)
    : mCfg(mCfg)
{
    mClockSpeedNs = measure_clock_cost(1000u);
}

void BenchmarkRunner::init_benchmark() {
    mSamples = {};
}

void BenchmarkRunner::run_benchmark() {
    std::cout << "Overhead latency test: " << std::endl;
    visualise_header();
    for (int i = 0; i < mCfg.nWarmUps; i++) {
        run_single_session(SampleType::LogScope);
        init_benchmark();
    }

    for (SampleType sampleType : sampleTypeVals) {
        if (sampleType == SampleType::DebugDynamicString) {
            run_dynamic_log_session();
        } else {
            run_single_session(sampleType);
            
        }
        SessionResult results = ResultProcessor::summarize(mSamples, mCfg, sampleType, mClockSpeedNs);
        visualise_results(results);
        init_benchmark();
        
    }
    visualise_footer();
}

void BenchmarkRunner::run_dynamic_log_session() {
    LOGGING_CONFIGURE_AND_START(nullptr, true);
    LOG_SCOPE(logging_bench, test_dynamic_log);
    auto make_log_call = [&](const char* format, auto&&... args)
    {
        LOG_DBG(format, args...);
    };

    // Starts runtime dispatch to find the right template function that matches number of specified arguments
    dispatch_n_int_args<0>(mCfg.nIntArgs, mCfg.nStringArgs, mCfg.nFloatArgs, make_log_call);

    LOGGING_STOP();
}

void BenchmarkRunner::run_single_session(SampleType sampleType) {
    LOGGING_CONFIGURE_AND_START(nullptr, true);
    for (int i = 0; i < mCfg.nSamples; i++) {
        uint64_t sampleTime = run_sample(sampleType);
        mSamples.push_back(sampleTime);
    }
    LOGGING_STOP();
    
}

uint64_t BenchmarkRunner::run_sample(SampleType sampleType) {
    uint64_t sample = 0;
    switch (sampleType)
    {
    case SampleType::LogScope:
        sample = bench_log_scope();
        break;

    case SampleType::DebugString:
        sample = bench_dbg_string();
        break;
    
    case SampleType::DebugOneArg:
        sample = bench_dbg_1_arg();
        break;
    
    case SampleType::DebugTwoArg:
        sample = bench_dbg_2_arg();
        break;
    
    case SampleType::DebugThreeMixedArg:
        sample = bench_dbg_3_mixed();
        break;
    
    case SampleType::DebugTenMixedArg:
        sample = bench_dbg_10_mixed();
        break;
    
    case SampleType::DebugLongString:
        sample = bench_long_string();
        break;
    default:
        break;
    }
    return sample;
}



uint64_t BenchmarkRunner::bench_log_scope() {
    uint64_t begin = now_ns();
    LOG_SCOPE(logging_bench, bench_log_scope);
    uint64_t end = now_ns();
    return end - begin;
}

uint64_t BenchmarkRunner::bench_dbg_string() 
{
    LOG_SCOPE(logging_bench, bench_dbg_string);
    uint64_t begin = now_ns();
    LOG_DBG("Hello World!");
    uint64_t end = now_ns();
    return (end - begin) / 1000;
}

uint64_t BenchmarkRunner::bench_dbg_1_arg() 
{
    LOG_SCOPE(logging_bench, bench_dbg_1_arg);
    int num1 = 11;
    uint64_t begin = now_ns();
    LOG_DBG("Value = %d",num1);
    uint64_t end = now_ns();
    return end - begin;
}

uint64_t BenchmarkRunner::bench_dbg_2_arg() 
{
    LOG_SCOPE(logging_bench, bench_dbg_2_arg);
    int key = 1;
    int val = 20;
    uint64_t begin = now_ns();
    LOG_DBG("Key = %d, Value = %d", key, val);
    uint64_t end = now_ns();
    return end - begin;
}

uint64_t BenchmarkRunner::bench_dbg_3_mixed() 
{
    LOG_SCOPE(logging_bench, bench_dbg_3_mixed);
    std::string name = "AREG";
    unsigned int key = 1;
    double val = 0.5;
    uint64_t begin = now_ns();
    LOG_DBG("Name = %s, Key = %u, Value = %.2f", name.c_str(), key, val);
    uint64_t end = now_ns();
    return end - begin;
}

uint64_t BenchmarkRunner::bench_dbg_10_mixed() 
{
    LOG_SCOPE(logging_bench, bench_dbg_10_mixed);

    std::string name = "worker";
    int count = -42;
    unsigned int key = 123;
    double value = 3.14159;
    const char* status = "active";
    long long timestamp = 123456789LL;
    unsigned long long bytes = 987654321ULL;
    char grade = 'A';
    unsigned int flags = 0xFF;
    double latency = 22.4567;

    uint64_t begin = now_ns();
    LOG_DBG(
        "Name=%s Count=%d Key=%u Value=%.2f Status=%s "
        "Timestamp=%lld Bytes=%llu Grade=%c Flags=%x Latency=%.2f",
        name.c_str(),
        count,
        key,
        value,
        status,
        timestamp,
        bytes,
        grade,
        flags,
        latency
    );

    uint64_t end = now_ns();
    return end - begin;
}

uint64_t BenchmarkRunner::bench_long_string() 
{
    LOG_SCOPE(logging_bench, bench_long_string);
    int STR_LEN = 300;
    std::string str;
    for (int i = 0; i < STR_LEN; i++) {
        str.push_back('s');
    }
    uint64_t begin = now_ns();
    LOG_DBG(str.c_str());
    uint64_t end = now_ns();
    return end - begin;
}
