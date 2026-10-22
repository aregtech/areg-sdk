#ifndef AREG_LOG_BENCHMARKER_RESULT_PROCESSOR
#define AREG_LOG_BENCHMARKER_RESULT_PROCESSOR
#include "src/BenchmarkTypes.hpp"

/**
 * @brief  Converts the samples collected during a benchmark session into its summary statistics
 * 
 */
class ResultProcessor
{
public:
    ResultProcessor();

    /**
     * @brief   Summarises the result of a benchmark session
     * 
     * @param samples        List of runtime samples collected
     * @param cfg            Benchmark configuration used during the session
     * @param sampleType     Type of sampling test done
     * @param clockSpeedNs   The estimated speed of the clock
     * 
     * @return   Returns the summary of statistics of the benchmark session
     */
    static SessionResult summarize( std::vector<uint64_t> samples
                                    , BenchmarkConfig cfg 
                                    , SampleType sampleType
                                    , double clockSpeedNs );

};


#endif