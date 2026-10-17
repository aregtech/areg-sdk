#include "src/InputParser.hpp"
#include <unordered_set>
#include <optional>
#include <charconv>
#include <iostream>


static std::optional<int> parseInt(std::string_view input)
{
    int value{};

    const auto [ptr, error] = std::from_chars(
        input.data(),
        input.data() + input.size(),
        value
    );

    if (error != std::errc{} || ptr != input.data() + input.size())
    {
        return std::nullopt;
    }

    return value;
}

std::optional<BenchmarkConfig> InputParser::parseInput(int argc, char* argv[]) {
    std::vector<std::string> args;
    for (int i = 1; i < argc; i++) {
            if (argv[i][0] != '-' || argv[i][1] < 'a' || argv[i][1] > 'z') {
                throw std::invalid_argument("Unknown flags found!");
            } else {
                args.push_back(std::string(argv[i]));
            }
        }
    std::vector<ParsedArg> pList;
    for (std::string arg : args) {
        int pos = 1;
        while (pos < arg.size() && arg[pos] >= 'a' && arg[pos] <= 'z') {
            pos++;
        }
        ParsedArg p;
        p.flag = arg.substr(1,pos-1);
        if (arg[pos] == '=') {
            pos++;
            while (pos < arg.size()) {
                p.value.push_back(arg[pos]);
                pos++;
            }
        }
        pList.push_back(p);
    }
    return buildConfig(pList);
}

std::optional<BenchmarkConfig> InputParser::buildConfig(std::vector<ParsedArg> pList) {
    BenchmarkConfig cfg;
    std::unordered_set<std::string> numericalFlags = {"l", "w", "t", "a", "af", "ad", "as", "sl"};
    int argVal{};
    for (ParsedArg p : pList) {
        if (numericalFlags.find(p.flag) != numericalFlags.end()) {
            std::optional<int> optArgVal = parseInt(p.value);

            if (optArgVal.has_value()) {
                argVal = optArgVal.value();
            } else {
                std::cerr << "Argument value has to be a valid integer\n";
                return std::nullopt;
            }
        }
        if (p.flag == "l") {
            if (argVal < 1000 || argVal > 100000) {
                std::cerr << "Number of loops must be between 1000 and 100,000!\n";
                return std::nullopt;
            }
            cfg.nSamples = argVal;
        
        } else if (p.flag == "w") {
            if (argVal < 0 || argVal > 100) {
                std::cerr << "Number of warm-up sessions must be between 0 and 100!\n";
                return std::nullopt;
            }
            cfg.nWarmUps = argVal;

        } else if (p.flag == "t") {
            if (argVal != 1 && argVal != 2 && argVal != 4 &&
                argVal != 8 && argVal != 16) {
                std::cerr << "Number of threads must be 1, 2, 4, 8, or 16!\n";
                return std::nullopt;
            }
            cfg.nThreads = argVal;

        } else if (p.flag == "a") {
            if (argVal < 1 || argVal > 12) {
                std::cerr << "Max number of arguments must be between 1 and 12!\n";
                return std::nullopt;
            }
            cfg.maxArgs = argVal;

        } else if (p.flag == "af") {
            if (argVal < 0 || argVal > 12) {
                std::cerr << "Number of floating-point arguments must be between 0 and 12!\n";
                return std::nullopt;
            }
            cfg.nFloatArgs = argVal;

        } else if (p.flag == "ad") {
            if (argVal < 0 || argVal > 12) {
                std::cerr << "Number of integer arguments must be between 0 and 12!\n";
                return std::nullopt;
            }
            cfg.nIntArgs = argVal;

        } else if (p.flag == "as") {
            if (argVal < 0 || argVal > 12) {
                std::cerr << "Number of string arguments must be between 0 and 12!\n";
                return std::nullopt;
            }
            cfg.nStringArgs = argVal;

        } else if (p.flag == "sl") {
            if (argVal < 4 || argVal > 64) {
                std::cerr << "String length must be between 4 and 64!\n";
                return std::nullopt;
            }
            cfg.strArgLen= argVal;

        } else if (p.flag == "s") {

            if (p.value == "collect") {
                cfg.logDest = LogDestination::Collector;
            } else if (p.value == "file") {
                cfg.logDest = LogDestination::File;
            } else if (p.value == "db") {
                cfg.logDest = LogDestination::Database;
            } else if (p.value == "all") {
                cfg.logDest = LogDestination::All;
            } else {
                std::cerr << "Unknown output destination!\n";
                return std::nullopt;
            }

        } else {
            std::cerr << "Unknown flag found!\n";
            return std::nullopt;
        }
    }    

    if (cfg.nFloatArgs + cfg.nIntArgs + cfg.nStringArgs > cfg.maxArgs) {
        std::cerr << "Number of arguments exceed max arguments!\n";
        return std::nullopt;
    }

    return cfg;
}