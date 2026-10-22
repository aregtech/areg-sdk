#ifndef AREG_LOG_BENCHMARK_RUNNER
#define AREG_LOG_BENCHMARK_RUNNER


#include "areg/logging/areg_log.h"
#include "src/BenchmarkTypes.hpp"

#include <vector>
#include <cstdint>

/**
 * @brief  Runs logging performance benchmarks and collects their results.
 * 
 */
class BenchmarkRunner

{

private:

    BenchmarkConfig mCfg;

    std::vector<uint64_t> mSamples; // Holds result of each sample in a single sessions.

    double mClockSpeedNs; // Speed of calling now_ns().

    static constexpr int MAX_ARG{12};


public:

    
    BenchmarkRunner(BenchmarkConfig mCfg);

    // Benchmarking Functions

    /**
     * @brief   Initialises the benchmark by resetting any collected results.
     * 
     */
    void init_benchmark();

    /**
     * @brief   Runs a benchmarking session with the specified sample test.
     * 
     * @param sampleType   Type of sample tested for.
     */
    void run_single_session(SampleType sampleType);

    /**
     * @brief   Dispatches the right benchmarking method based on sample type.
     * 
     * @param sampleType   Type of sample tested for.
     * 
     * @return   The elapsed time in nanoseconds.
     */
    uint64_t run_sample(SampleType sampleType);

    /**
     * @brief   Runs the entire benchmarking loop.
     * 
     */
    void run_benchmark();


    /**
     * @brief   Runs dedicated benchmarking session for the dynamically constructed mixed debug log case.
     * 
     */
    void run_dynamic_log_session();

private:

    
    
    /**
     * \brief   Benchmarks the overhead of a logging scope.
     *
     * \return  The elapsed time in nanoseconds.
     */
    uint64_t bench_log_scope();

    /**
     * \brief   Benchmarks the overhead of logging a debug string without arguments.
     *
     * \return  The elapsed time in nanoseconds.
     */
    uint64_t bench_dbg_string();

    /**
     * \brief   Benchmarks the overhead of logging a debug message with one argument.
     *
     * \return  The elapsed time in nanoseconds.
     */
    uint64_t bench_dbg_1_arg();

    /**
     * \brief   Benchmarks the overhead of logging a debug message with two arguments.
     *
     * \return  The elapsed time in nanoseconds.
     */
    uint64_t bench_dbg_2_arg();

    /**
     * \brief   Benchmarks the overhead of logging a debug message with three arguments of mixed types.
     *
     * \return  The elapsed time in nanoseconds.
     */
    uint64_t bench_dbg_3_mixed();

    /**
     * \brief   Benchmarks the overhead of logging a debug message with ten arguments of mixed types.
     *
     * \return  The elapsed time in nanoseconds.
     */
    uint64_t bench_dbg_10_mixed();

    /**
     * \brief   Benchmarks the overhead of logging a long debug string.
     *
     * \return  The elapsed time in nanoseconds.
     */
    uint64_t bench_long_string();

    /**
     * \brief   Returns the current time from a monotonic clock in nanoseconds.
     *
     * \return  The current steady clock timestamp in nanoseconds.
     */
    static uint64_t now_ns() noexcept
    {
        return static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::chrono::steady_clock::now().time_since_epoch()
            ).count()
            );
    }

    /**
     * \brief   Measures the average overhead of calling the steady clock.
     *
     * \param   mSamples    The number of clock calls to measure.
     *
     * \return  The average time per clock call in nanoseconds, or 0.0
     *          if the sample count is zero.
     */
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

    /**
     * \brief   Generates a format string for a logging message with mixed arguments.
     *
     * \details Constructs a format string containing integer, string, and
     *          floating-point placeholders in that order.
     *
     * \tparam  NIntArgs    The number of integer arguments.
     * \tparam  NStrArgs    The number of string arguments.
     * \tparam  NFltArgs    The number of floating-point arguments.
     *
     * \return  The generated logging format string.
     */
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

    /**
     * \brief   Dispatches a logging benchmark based on the number of floating-point arguments.
     *
     * \details Recursively selects the template instantiation matching the
     *          requested number of floating-point arguments. Once matched,
     *          generates the format string and invokes the benchmark.
     *
     * \tparam  NIntArgs    The number of integer arguments.
     * \tparam  NStrArgs    The number of string arguments.
     * \tparam  NFltArgs    The current floating-point argument count being checked.
     * \tparam  LoggingCall The type of the logging callable.
     *
     * \param   nFlt        The requested number of floating-point arguments.
     * \param   logCall     The logging callable to benchmark.
     */
    template <int NIntArgs, int NStrArgs, int NFltArgs, typename LoggingCall>
    void dispatch_n_flt_args(int nFlt, LoggingCall&& logCall) 
    {
        if (nFlt == NFltArgs) {
            std::string formatStr = makeFormat<NIntArgs,NStrArgs,NFltArgs>();
            bench_dyn_log(std::make_index_sequence<NIntArgs>{}, 
                        std::make_index_sequence<NStrArgs>{}, 
                        std::make_index_sequence<NFltArgs>{}, 
                        makeFormat<NIntArgs,NStrArgs,NFltArgs>(), logCall);
            
            return;
        }
        if constexpr (NFltArgs < MAX_ARG) {
            dispatch_n_flt_args<NIntArgs,NStrArgs,NFltArgs+1>(nFlt,logCall);
        }

    }

    /**
     * \brief   Dispatches a logging benchmark based on the number of string arguments.
     *
     * \details Recursively selects the template instantiation matching the
     *          requested number of string arguments, then delegates to the
     *          floating-point argument dispatcher.
     *
     * \tparam  NIntArgs    The number of integer arguments.
     * \tparam  NStrArgs    The current string argument count being checked.
     * \tparam  LoggingCall The type of the logging callable.
     *
     * \param   nStr        The requested number of string arguments.
     * \param   nFlt        The requested number of floating-point arguments.
     * \param   logCall     The logging callable to benchmark.
     */
    template <int NIntArgs, int NStrArgs, typename LoggingCall>
    void dispatch_n_str_args(int nStr, int nFlt, LoggingCall&& logCall) 
    {
        if (nStr == NStrArgs) {
            dispatch_n_flt_args<NIntArgs, NStrArgs, 0>(nFlt, logCall);
            return;
        }
        if constexpr (NStrArgs < MAX_ARG) {
            dispatch_n_str_args<NIntArgs,NStrArgs+1>(nStr, nFlt, logCall);
        }
        

    }

    /**
     * \brief   Dispatches a logging benchmark based on the number of integer arguments.
     *
     * \details Recursively selects the template instantiation matching the
     *          requested number of integer arguments, then delegates to the
     *          string argument dispatcher.
     *
     * \tparam  NIntArgs    The current integer argument count being checked.
     * \tparam  LoggingCall The type of the logging callable.
     *
     * \param   nInt        The requested number of integer arguments.
     * \param   nStr        The requested number of string arguments.
     * \param   nFlt        The requested number of floating-point arguments.
     * \param   logCall     The logging callable to benchmark.
     */
    template <int NIntArgs, typename LoggingCall>
    void dispatch_n_int_args(int nInt, int nStr, int nFlt, LoggingCall&& logCall) 
    {
        if (nInt == NIntArgs) {
            dispatch_n_str_args<NIntArgs,0>(nStr,nFlt,logCall);
            return;
        }

        if constexpr (NIntArgs < MAX_ARG) {
            dispatch_n_int_args<NIntArgs+1>(nInt, nStr, nFlt, logCall);
        }
    }


    /**
     * \brief   Benchmarks the overhead of a logging call with specified mixed arguments.
     *
     * \details Executes the supplied logging callable for the configured
     *          number of samples. Measures the elapsed time of each call
     *          using a steady clock and stores the results in mSamples.
     *          Argument values are generated from the provided index sequences.
     *
     * \tparam  NIntArgs    Index sequence for integer arguments.
     * \tparam  NStrArgs    Index sequence for string arguments.
     * \tparam  NFltArgs    Index sequence for floating-point arguments.
     * \tparam  LoggingCall The type of the logging callable.
     *
     * \param   formatString    The format string used by the logging call.
     * \param   logCall         The logging callable to benchmark.
     */
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