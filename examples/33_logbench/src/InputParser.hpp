#ifndef AREG_LOG_BENCHMARK_INPUT_PARSER
#define AREG_LOG_BENCHMARK_INPUT_PARSER

#include "src/BenchmarkTypes.hpp"

#include <vector>

/**
 * @brief   Holds the flag and its user given value
 * 
 */
struct ParsedArg
{
    std::string flag;
    std::string value; 
};


/**
 * @brief Parses parameter flags from the user's input into a benchmark configuration
 * 
 */
class InputParser 
{

public:
    /**
     * @brief   Converts user input flags into a benchmark configuration
     * 
     * @param argc   Number of arguments
     * @param argv   List of input string arguments
     * 
     * @return   Returns a benchmark configuration if the parse was valid
     */
    static std::optional<BenchmarkConfig> parseInput(int argc, char* argv[]);

private:
    /**
     * @brief   Converts a list of parsed arguments into a benchmark configuration 
     * 
     * @param argList   List of parsed arguments
     * 
     * @return   Returns a benchmark configuration if the argument values were valid
     */
    static std::optional<BenchmarkConfig> buildConfig(std::vector<ParsedArg> argList);

};

#endif