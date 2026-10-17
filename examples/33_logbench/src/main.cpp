#include "BenchmarkRunner.hpp"
#include "InputParser.hpp"
#include <optional>


int main(int argc, char * argv[]) 
{
    
    
    std::optional<BenchmarkConfig> cfg = InputParser::parseInput(argc, argv);
    if (!cfg.has_value()) {
        return 1;
    }
    BenchmarkRunner runner(cfg.value());
    runner.init_benchmark();
    runner.run_benchmark();
    return 0;
}