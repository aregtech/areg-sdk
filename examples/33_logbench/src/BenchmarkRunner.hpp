#ifndef AREG_LOG_BENCHMARK_RUNNER
#define AREG_LOG_BENCHMARK_RUNNER


#include "areg/logging/areg_log.h"
#include "src/BenchmarkTypes.hpp"

#include <vector>
#include <cstdint>

// Max possible number of arguments for a log message
constexpr int MAX_ARG = 12;

/**
 * Runs logging performance benchmarks and collects their results.
 */

class BenchmarkRunner

{

private:

    BenchmarkConfig mCfg;

    std::vector<uint64_t> mSamples; // Holds result of each sample in a single sessions

    double mClockSpeedNs; // Speed of calling now_ns()


public:

    // Constructor
    
    BenchmarkRunner(BenchmarkConfig mCfg);

    // Benchmarking Functions

    void init_benchmark();

    void run_single_session(SampleType sampleType);

    uint64_t run_sample(SampleType sampleType);

    void run_benchmark();

    void run_dynamic_log_session();

    // Sample Functions
private:

    static uint64_t now_ns() noexcept
    {
        return static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::chrono::steady_clock::now().time_since_epoch()
            ).count()
            );
    }

    double measure_clock_cost(uint32_t mSamples) noexcept
    {
        if (mSamples == 0u)
            return 0.0;

        // Warm the clock path up first, then time the loop as a whole: timing every single
        // call would measure the timing itself.
        volatile uint64_t sink{ 0u };
        for (uint32_t i = 0u; i < 1000u; ++i)
            sink = now_ns();

        const uint64_t begin{ now_ns() };
        for (uint32_t i = 0u; i < mSamples; ++i)
            sink = now_ns();

        const uint64_t end{ now_ns() };
        static_cast<void>(sink);
        return static_cast<double>(end - begin) / static_cast<double>(mSamples);
    }
    
    uint64_t bench_log_scope();

    uint64_t bench_dbg_string();

    uint64_t bench_dbg_1_arg();

    uint64_t bench_dbg_2_arg();

    uint64_t bench_dbg_3_mixed();

    uint64_t bench_dbg_10_mixed();

    uint64_t bench_long_string();

    uint64_t bench_dbg_dynamic();

    // Templates for generating and benchmarking dynamic logging call
    template <int NIntArgs, int NStrArgs, int NFltArgs>
    std::string makeFormat() {
        int count = 0;
        std::string out = "";
        for (int i = 0; i < NIntArgs; i++) {
            count++;
            out.append("Arg " + std::to_string(count) + ": %d, ");
        }
        for (int i = 0; i < NStrArgs; i++) {
            count++;
            out.append("Arg " + std::to_string(count) + ": %s, ");
        }
        for (int i = 0; i < NFltArgs; i++) {
            count++;
            out.append("Arg " + std::to_string(count) + ": %.2f, ");
        }
        return out.substr(0,out.size()-2);
    };


    template <int NIntArgs, int NStrArgs, int NFltArgs, typename LoggingCall>
    void dispatch_n_flt_args(int nFlt, LoggingCall&& logCall) {
        if (nFlt == NFltArgs) {
            std::string formatStr = makeFormat<NIntArgs,NStrArgs,NFltArgs>();
            bench_dyn_log(std::make_index_sequence<3>{}, std::make_index_sequence<3>{}, std::make_index_sequence<3>{}, makeFormat<3,3,3>(), logCall);
            
            return;
        }
        if constexpr (NFltArgs < MAX_ARG) {
            dispatch_n_flt_args<NIntArgs,NStrArgs,NFltArgs+1>(nFlt,logCall);
        }

    }

    template <int NIntArgs, int NStrArgs, typename LoggingCall>
    void dispatch_n_str_args(int nStr, int nFlt, LoggingCall&& logCall) {
        if (nStr == NStrArgs) {
            dispatch_n_flt_args<NIntArgs, NStrArgs, 0>(nFlt, logCall);
            return;
        }
        if constexpr (NStrArgs < MAX_ARG) {
            dispatch_n_str_args<NIntArgs,NStrArgs+1>(nStr, nFlt, logCall);
        }
        

    }

    template <int NIntArgs, typename LoggingCall>
    void dispatch_n_int_args(int nInt, int nStr, int nFlt, LoggingCall&& logCall) {
        if (nInt == NIntArgs) {
            dispatch_n_str_args<NIntArgs,0>(nStr,nFlt,logCall);
            return;
        }

        if constexpr (NIntArgs < MAX_ARG) {
            dispatch_n_int_args<NIntArgs+1>(nInt, nStr, nFlt, logCall);
        }
    }


    template <size_t... NIntArgs, size_t... NStrArgs, size_t... NFltArgs, typename LoggingCall>
    void bench_dyn_log(
        std::index_sequence<NIntArgs...>, 
        std::index_sequence<NStrArgs...>, 
        std::index_sequence<NFltArgs...>, 
        const std::string& formatString, 
        LoggingCall&& logCall
    ) 
    {
        for (int i = 0; i < mCfg.nSamples; i++) {
            uint64_t begin = now_ns();
            logCall(formatString.c_str(), ((void)NIntArgs, 42)..., ((void)NStrArgs, "Hello")..., ((void)NFltArgs, 3.14)...);
            uint64_t end = now_ns();
            uint64_t sample = end - begin;
            mSamples.push_back(sample);
        }
        
    }

    



};



#endif