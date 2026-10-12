#include "src/InputParser.hpp"
#include <unordered_set>

BenchmarkConfig InputParser::parseInput(int argc, char* argv[]) {
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

BenchmarkConfig InputParser::buildConfig(std::vector<ParsedArg> pList) {
    BenchmarkConfig cfg;
    for (ParsedArg p : pList) {
        int argVal;
        std::unordered_set<std::string> numericalFlags = {"l", "w", "t", "a", "af", "ad", "as", "sl"};
        if (numericalFlags.find(p.flag) != numericalFlags.end()) {
            try {
                argVal = stoi(p.value);
            } catch (const std::invalid_argument&) {
                throw std::invalid_argument("Flag value must be an integer!");
            } catch (const std::out_of_range&) {
                throw std::invalid_argument("Integer is too large!");
            }
        }
        if (p.flag == "l") {
            if (argVal < 1000 || argVal > 100000) {
                throw std::out_of_range("Number of loops must be between 1000 and 100,000!");
            }
            cfg.nSamples = argVal;
        
        } else if (p.flag == "w") {
            if (argVal < 0 || argVal > 100) {
                throw std::out_of_range(
                    "Number of warm-up sessions must be between 0 and 100!"
                );
            }
            cfg.nWarmUps = argVal;

        } else if (p.flag == "t") {
            if (argVal != 1 && argVal != 2 && argVal != 4 &&
                argVal != 8 && argVal != 16) {
                throw std::out_of_range(
                    "Number of threads must be 1, 2, 4, 8, or 16!"
                );
            }
            cfg.nThreads = argVal;

        } else if (p.flag == "a") {
            if (argVal < 1 || argVal > 12) {
                throw std::out_of_range(
                    "Max number of arguments must be between 1 and 12!"
                );
            }
            cfg.maxArgs = argVal;

        } else if (p.flag == "af") {
            if (argVal < 0 || argVal > 12) {
                throw std::out_of_range(
                    "Number of floating-point arguments must be between 0 and 12!"
                );
            }
            cfg.nFloatArgs = argVal;

        } else if (p.flag == "ad") {
            if (argVal < 0 || argVal > 12) {
                throw std::out_of_range(
                    "Number of integer arguments must be between 0 and 12!"
                );
            }
            cfg.nIntArgs = argVal;

        } else if (p.flag == "as") {
            if (argVal < 0 || argVal > 12) {
                throw std::out_of_range(
                    "Number of string arguments must be between 0 and 12!"
                );
            }
            cfg.nStringArgs = argVal;

        } else if (p.flag == "sl") {
            if (argVal < 4 || argVal > 64) {
                throw std::out_of_range(
                    "String length must be between 4 and 64!"
                );
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
                throw std::invalid_argument("Unknown output destination!");
            }

        } else {
            throw std::invalid_argument("Unknown flag found!");
        }
    }    

    if (cfg.nFloatArgs + cfg.nIntArgs + cfg.nStringArgs > cfg.maxArgs) {
        throw std::out_of_range("Number of arguments exceed max arguments!");
    }

    return cfg;
}