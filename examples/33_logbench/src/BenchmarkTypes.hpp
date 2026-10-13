#ifndef AREG_LOG_BENCHMARK_TYPES
#define AREG_LOG_BENCHMARK_TYPES


#include <vector>
#include <cstdint>
/**
 * Indicates the sampling function to use
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

enum class LogDestination
{
    File,
    Database,
    Collector,
    All
};

/**
 * Contains the settings of the benchmark, used in the construction of the BenchmarkRunner object
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
 * Stores stats from a single session
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