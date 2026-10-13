#ifndef AREG_LOG_BENCHMARKER_RESULT_PROCESSOR
#define AREG_LOG_BENCHMARKER_RESULT_PROCESSOR
#include "src/BenchmarkTypes.hpp"
class ResultProcessor
{
public:
    ResultProcessor();

    static SessionResult summarize(std::vector<uint64_t> mSamples, BenchmarkConfig mCfg, SampleType sType, double mClockSpeedNs);

};


#endif