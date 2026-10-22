#ifndef AREG_LOG_BENCHMARK_TYPES
#define AREG_LOG_BENCHMARK_TYPES


#include <vector>
#include <cstdint>

/**
 * @brief   Indicates which sampling test to use for the current benchmark session
 * 
 */
enum class SampleType
{
    LogScope,
    DebugString,
    DebugOneArg,
    DebugTwoArg,
    DebugThreeMixedArg,
    DebugTenMixedArg,
    DebugLongString,
    DebugDynamicString,

};

/**
 * @brief   Indicates the set destination for logging messages to go
 * 
 */
enum class LogDestination
{
    File,
    Database,
    Collector,
    All
};

/**
 * @brief   Holds the configuration used for the benchmark
 * 
 */
struct BenchmarkConfig 
{
    int nSamples = 10000;
    int nThreads = 8;
    int nWarmUps = 0;
    LogDestination logDest = LogDestination::All;
    int maxArgs = 8;
    int nIntArgs = 4;
    int nFloatArgs = 2;
    int nStringArgs = 2;
    int strArgLen = 16;
    
};


/**
 * @brief   Stores the results from a single benchmark session
 * 
 */
struct SessionResult
{
    
    BenchmarkConfig cfg;
    SampleType sampleType;
    double min_us = { 0.0 };
    double max_us = { 0.0 };
    double mean_us = { 0.0 };
    double p50_us = { 0.0 };
    double p90_us = { 0.0 };
    double p99_us = { 0.0 };
    double p999_us = { 0.0 };
    double sd_us = { 0.0 };
    double clock_speed_us = { 0.0 };
};


#endif