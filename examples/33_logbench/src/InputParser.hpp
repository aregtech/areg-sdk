#ifndef AREG_LOG_BENCHMARK_INPUT_PARSER
#define AREG_LOG_BENCHMARK_INPUT_PARSER

#include "src/BenchmarkTypes.hpp"

#include <vector>

struct ParsedArg
{
    std::string flag;
    std::string value; 
};


/**
 * Converts the input arguments into a config object for the benchmarker
 */
class InputParser 
{

public:

    static std::optional<BenchmarkConfig> parseInput(int argc, char* argv[]);

private:

    static std::optional<BenchmarkConfig> buildConfig(std::vector<ParsedArg> pList);

};

#endif