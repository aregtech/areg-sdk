/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Copyright (c) 2017-2026 Aregtech (Artak Avetyan)
 * Distributed under the licenses in LICENSE.txt.
 *
 * \file        units/LogObserverAppTest.cpp
 * \brief       Areg Platform, tests of the log observer application started from its main
 *              function: the help, and the options run from the command line.
 ************************************************************************/

#include "units/GUnitTest.hpp"
#include "units/ChildProcess.hpp"
#include "areg/base/Process.hpp"
#include "areglogger/client/LogObserverApi.h"
#include "logobserver/app/LogObserver.hpp"

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <system_error>
#include <vector>

#if defined(_WIN32)
    #include <io.h>
#else   // defined(_WIN32)
    #include <unistd.h>
#endif  // defined(_WIN32)

#if AREG_LOGGING

namespace
{
    namespace fs = std::filesystem;

    //!< Runs the log observer with the options and returns what it printed on the standard output.
    std::string run_observer(const std::vector<std::string> & options)
    {
        const fs::path out{ fs::temp_directory_path() / ("areg-observer-app-" + std::to_string(areg::Process::instance().id()) + ".txt") };
        std::fflush(stdout);
#if defined(_WIN32)
        const int saved{ ::_dup(::_fileno(stdout)) };
#else   // defined(_WIN32)
        const int saved{ ::dup(::fileno(stdout)) };
#endif  // defined(_WIN32)
        AREG_CHILD_CHECK(saved >= 0);
#if defined(_WIN32)
        FILE * stream{ nullptr };
        AREG_CHILD_CHECK(::freopen_s(&stream, out.string().c_str(), "w", stdout) == 0);
#else   // defined(_WIN32)
        AREG_CHILD_CHECK(std::freopen(out.string().c_str(), "w", stdout) != nullptr);
#endif  // defined(_WIN32)

        std::vector<std::string> storage{ "logobserver" };
        storage.insert(storage.end(), options.begin(), options.end());
        std::vector<char *> argv;
        for (std::string & value : storage)
        {
            argv.push_back(value.data());
        }

        LogObserver::instance().log_main(static_cast<int32_t>(argv.size()), argv.data());

        std::fflush(stdout);
#if defined(_WIN32)
        AREG_CHILD_CHECK(::_dup2(saved, ::_fileno(stdout)) == 0);
        ::_close(saved);
#else   // defined(_WIN32)
        AREG_CHILD_CHECK(::dup2(saved, ::fileno(stdout)) >= 0);
        ::close(saved);
#endif  // defined(_WIN32)

        std::ifstream input(out, std::ios::binary);
        const std::string text{ std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>() };
        input.close();
        std::error_code error;
        fs::remove(out, error);
        return text;
    }

    //!< Writes a configuration whose log collector connection is switched off and returns its path.
    std::string write_config(const fs::path & dir)
    {
        std::error_code error;
        fs::create_directories(dir, error);
        const fs::path config{ dir / "observer.init" };
        std::ofstream file(config, std::ios::trunc);
        file << "log::*::enable = false\n"
             << "log::*::enable::db = false\n"
             << "logger::*::service = logcollector\n"
             << "logger::*::connect = tcpip\n"
             << "logger::*::enable::tcpip = false\n"
             << "logger::*::address::tcpip = 127.0.0.1\n"
             << "logger::*::port::tcpip = 1\n";
        file.close();
        AREG_CHILD_CHECK(file.good());
        return config.generic_string();
    }
}

/**
 * \brief   The help option prints the usage and starts no observer.
 **/
TEST(LogObserverAppTest, help_prints_the_usage_and_starts_nothing)
{
    AREG_EXPECT_IN_CHILD(
        const std::string text{ run_observer({ "--help" }) };
        AREG_CHILD_CHECK(text.find("--help") != std::string::npos);
        AREG_CHILD_CHECK(log_observer_is_initialized() == false);
    );
}

/**
 * \brief   Options run from the command line without a log collector connection report it, report
 *          each option, and end the observer.
 **/
TEST(LogObserverAppTest, command_line_options_without_a_collector_connection_report_and_end)
{
    AREG_EXPECT_IN_CHILD(
        const fs::path dir{ fs::temp_directory_path() / ("areg-observer-app-" + std::to_string(areg::Process::instance().id())) };
        const std::string config{ write_config(dir) };
        const std::string text{ run_observer({ "--load=" + config, "--instances", "--query", "--pause" }) };
        AREG_CHILD_CHECK(text.find("Failed to trigger the connection with the log collector") != std::string::npos);
        AREG_CHILD_CHECK(log_observer_is_initialized() == false);
        std::error_code error;
        fs::remove_all(dir, error);
    );
}

#endif  // AREG_LOGGING
