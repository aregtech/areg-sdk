/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/ConfigManagerTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Areg framework unit test file.
 *              The shared configuration file and the configuration file of a process.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/appbase/AppDefs.hpp"
#include "areg/base/Process.hpp"
#include "areg/persist/ConfigManager.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/logging/LogScope.hpp"
#include "areg/logging/LoggingDefs.hpp"

#include <atomic>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <thread>
#include <vector>

namespace
{
    namespace fs = std::filesystem;

    //!< Returns an empty directory owned by the running test.
    fs::path test_dir()
    {
        const fs::path dir{ fs::current_path() / "logs" / (std::string("cfg_") + ::testing::UnitTest::GetInstance()->current_test_info()->name()) };
        std::error_code err;
        fs::remove_all(dir, err);
        fs::create_directories(dir, err);
        return dir;
    }

    //!< Writes the lines into the file, replacing its content.
    void write_file(const fs::path& path, const std::vector<std::string>& lines)
    {
        std::ofstream out(path, std::ios::trunc);
        for (const auto& line : lines)
        {
            out << line << "\n";
        }
    }

    //!< Returns the content of the file.
    std::string read_file(const fs::path& path)
    {
        std::ifstream in(path, std::ios::binary);
        return std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
    }

    //!< Returns the entry "log::<module>::<rest>" of the running process.
    std::string module_entry(const std::string& rest)
    {
        return std::string("log::") + areg::Process::instance().app_name().as_string() + "::" + rest;
    }

    //!< Returns the path of the configuration file of the running process in the directory.
    fs::path process_file(const fs::path& dir)
    {
        return dir / (std::string(areg::Process::instance().app_name().as_string()) + ".init");
    }

    //!< Returns the file location of a configuration read from the shared file.
    std::string file_location(const fs::path& shared)
    {
        areg::ConfigManager config;
        config.read_config(areg::String(shared.string()));
        return config.log_file_location().as_string();
    }
}

/**
 * \brief   The configuration file of the process is in the directory of the shared file.
 **/
TEST(ConfigManagerTest, process_file_is_beside_shared_file)
{
    const fs::path dir{ test_dir() };
    const fs::path shared{ dir / "areg.init" };
    write_file(shared, { "log::*::file::location = ./logs/all.log" });

    areg::ConfigManager config;
    ASSERT_TRUE(config.read_config(areg::String(shared.string())));
    EXPECT_EQ(fs::path(config.process_config_file().as_string()).lexically_normal(), process_file(dir).lexically_normal());
}

/**
 * \brief   A save never writes the shared file and writes the file of the process instead.
 **/
TEST(ConfigManagerTest, save_never_writes_shared_file)
{
    const fs::path dir{ test_dir() };
    const fs::path shared{ dir / "areg.init" };
    write_file(shared, { "log::*::file::location = ./logs/all.log", module_entry("file::location = ./logs/module.log") });
    const std::string before{ read_file(shared) };

    areg::ConfigManager config;
    ASSERT_TRUE(config.read_config(areg::String(shared.string())));
    config.set_file_location("./logs/process.log");

    EXPECT_FALSE(config.save_config(areg::String(shared.string())));
    EXPECT_FALSE(config.save_config(areg::String((dir / "." / "areg.init").string())));
    EXPECT_FALSE(config.save_config(areg::String(areg::DEFAULT_CONFIG_FILE)));
    EXPECT_TRUE(config.save_config());

    EXPECT_EQ(before, read_file(shared));
    EXPECT_EQ(file_location(shared), std::string("./logs/process.log"));
}

/**
 * \brief   A value of the process file wins over the module entry of the shared file, which wins
 *          over the entry for all modules.
 **/
TEST(ConfigManagerTest, process_file_wins_over_shared_file)
{
    const fs::path dir{ test_dir() };
    const fs::path shared{ dir / "areg.init" };

    write_file(shared, { "log::*::file::location = ./logs/all.log" });
    EXPECT_EQ(file_location(shared), std::string("./logs/all.log"));

    write_file(shared, { "log::*::file::location = ./logs/all.log", module_entry("file::location = ./logs/module.log") });
    EXPECT_EQ(file_location(shared), std::string("./logs/module.log"));

    write_file(process_file(dir), { module_entry("file::location = ./logs/process.log") });
    EXPECT_EQ(file_location(shared), std::string("./logs/process.log"));
}

/**
 * \brief   The log scope entries are split by the file and the module they come from.
 **/
TEST(ConfigManagerTest, scope_entries_split_by_source)
{
    const fs::path dir{ test_dir() };
    const fs::path shared{ dir / "areg.init" };
    write_file(shared, { "log::*::scope::abc_x = DEBUG", module_entry("scope::abc_* = WARN") });
    write_file(process_file(dir), { module_entry("scope::abc_* = INFO") });

    areg::ConfigManager config;
    ASSERT_TRUE(config.read_config(areg::String(shared.string())));

    std::vector<areg::Property> processScopes;
    std::vector<areg::Property> moduleScopes;
    std::vector<areg::Property> sharedScopes;
    config.log_scope_layers(processScopes, moduleScopes, sharedScopes);

    ASSERT_EQ(processScopes.size(), 1u);
    ASSERT_EQ(moduleScopes.size(), 1u);
    ASSERT_EQ(sharedScopes.size(), 1u);
    EXPECT_EQ(processScopes[0].value_string(), areg::String("INFO"));
    EXPECT_EQ(moduleScopes[0].value_string(), areg::String("WARN"));
    EXPECT_EQ(sharedScopes[0].key().position(), areg::String("abc_x"));
}

/**
 * \brief   Parallel saves of one file all succeed, and the file holds one complete save.
 **/
TEST(ConfigManagerTest, parallel_saves_of_one_file_all_succeed)
{
    constexpr uint32_t THREADS{ 8u };
    constexpr uint32_t ROUNDS { 25u };

    const fs::path dir{ test_dir() };
    const fs::path shared{ dir / "areg.init" };
    const fs::path target{ dir / "parallel.init" };
    write_file(shared, { "log::*::file::location = ./logs/all.log" });

    std::atomic<uint32_t> failed{ 0u };
    std::vector<std::thread> threads;
    for (uint32_t i = 0u; i < THREADS; ++i)
    {
        threads.emplace_back([&shared, &target, &failed, i]()
            {
                areg::ConfigManager config;
                config.read_config(areg::String(shared.string()));
                config.set_file_location(areg::String("./logs/saver") + areg::String::make_string(i) + ".log");
                for (uint32_t round = 0u; round < ROUNDS; ++round)
                {
                    if (config.save_config(areg::String(target.string())) == false)
                    {
                        failed.fetch_add(1u);
                    }
                }
            });
    }

    for (auto& thread : threads)
    {
        thread.join();
    }

    EXPECT_EQ(failed.load(), 0u);

    areg::ConfigManager saved;
    saved.read_config(areg::String(target.string()));
    EXPECT_EQ(saved.readonly_properties().size(), 1u);
    EXPECT_EQ(std::string(saved.log_file_location().as_string()).rfind("./logs/saver", 0), 0u);
}

#if AREG_LOGGING

namespace
{
    //!< Returns the list of properties parsed from the lines.
    areg::ListProperties make_properties(const std::vector<std::string>& lines)
    {
        areg::ListProperties result;
        for (const auto& line : lines)
        {
            areg::Property prop;
            if (prop.parse(areg::String(line.c_str())))
            {
                result.add(prop);
            }
        }

        return result;
    }
}

/**
 * \brief   A scope takes its priority from the first layer with a matching entry, by name or by
 *          group: the process layer, then the module entries, then the entries for all modules.
 **/
TEST(ConfigManagerTest, scope_priority_comes_from_first_matching_layer)
{
    constexpr uint32_t prioDebug{ static_cast<uint32_t>(areg::LogPriority::PrioDebug) };
    constexpr uint32_t prioInfo { static_cast<uint32_t>(areg::LogPriority::PrioInfo) };
    constexpr uint32_t prioWarn { static_cast<uint32_t>(areg::LogPriority::PrioWarning) };

    areg::ConfigManager& config{ areg::Application::config_manager() };
    const bool wasConfigured{ config.is_configured() };
    const areg::ListProperties oldShared{ config.readonly_properties() };
    const areg::ListProperties oldProcess{ config.module_properties() };

    areg::LogScope exact("cfgtest_abc_x");
    areg::LogScope nested("cfgtest_abc_x_y");
    areg::LogScope other("cfgtest_other");

    std::vector<std::string> shared{ "log::*::scope::cfgtest_abc_x = DEBUG"
                                   , "log::*::scope::cfgtest_abc_x_* = DEBUG"
                                   , "log::*::scope::cfgtest_other = DEBUG" };
    config.set_configuration(make_properties(shared), areg::ListProperties());
    areg::restore_logging();
    EXPECT_EQ(areg::scope_priority("cfgtest_abc_x"), prioDebug);
    EXPECT_EQ(areg::scope_priority("cfgtest_abc_x_y"), prioDebug);

    shared.push_back(module_entry("scope::cfgtest_abc_* = WARN"));
    config.set_configuration(make_properties(shared), areg::ListProperties());
    areg::restore_logging();
    EXPECT_EQ(areg::scope_priority("cfgtest_abc_x"), prioWarn);
    EXPECT_EQ(areg::scope_priority("cfgtest_abc_x_y"), prioWarn);

    config.set_configuration(make_properties(shared), make_properties({ module_entry("scope::cfgtest_abc_* = INFO") }));
    areg::restore_logging();
    EXPECT_EQ(areg::scope_priority("cfgtest_abc_x"), prioInfo);
    EXPECT_EQ(areg::scope_priority("cfgtest_abc_x_y"), prioInfo);
    EXPECT_EQ(areg::scope_priority("cfgtest_other"), prioDebug);

    config.set_configuration(oldShared, oldProcess);
    areg::restore_logging();

    // A process that was not configured before reads its configuration file again at the next start.
    if (wasConfigured == false)
    {
        config.release_properties();
    }
}

#endif  // AREG_LOGGING
