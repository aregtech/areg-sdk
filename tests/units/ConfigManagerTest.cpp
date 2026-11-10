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
#include "areg/base/File.hpp"
#include "areg/base/FileBuffer.hpp"
#include "areg/base/Identifier.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/Version.hpp"
#include "areg/persist/Property.hpp"
#include "areg/persist/PropertyKey.hpp"
#include "areg/persist/ConfigListener.hpp"
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
 * \brief   A value a listener sets after the read is kept: reading the files of the process does not drop it.
 **/
TEST(ConfigManagerTest, listener_value_survives_the_read)
{
    class LocationListener final : public areg::ConfigListener
    {
    protected:
        void prepare_save_configuration(areg::ConfigManager& /*config*/) final
        {
        }

        void post_save_configuration(areg::ConfigManager& /*config*/) final
        {
        }

        void prepare_read_configuration(areg::ConfigManager& /*config*/) final
        {
        }

        void on_setup_configuration(const areg::ListProperties& /*listReadonly*/, const areg::ListProperties& /*listWritable*/, areg::ConfigManager& /*config*/) final
        {
        }

        void post_read_configuration(areg::ConfigManager& config) final
        {
            config.set_file_location("./logs/listener.log", true);
        }
    };

    const fs::path dir{ test_dir() };
    const fs::path shared{ dir / "areg.init" };
    write_file(shared, { "log::*::file::location = ./logs/all.log" });

    LocationListener listener;
    areg::ConfigManager config;
    ASSERT_TRUE(config.read_config(areg::String(shared.string()), &listener));
    EXPECT_EQ(std::string(config.log_file_location().as_string()), std::string("./logs/listener.log"));
}

/**
 * \brief   A temporary value wins over the value saved in the process file, and the save keeps the saved value.
 **/
TEST(ConfigManagerTest, temporary_value_wins_over_saved_value)
{
    const fs::path dir{ test_dir() };
    const fs::path shared{ dir / "areg.init" };
    write_file(shared, { "log::*::file::location = ./logs/all.log" });
    write_file(process_file(dir), { module_entry("file::location = ./logs/process.log") });

    areg::ConfigManager config;
    ASSERT_TRUE(config.read_config(areg::String(shared.string())));
    config.set_file_location("./logs/temporary.log", true);
    EXPECT_EQ(std::string(config.log_file_location().as_string()), std::string("./logs/temporary.log"));

    ASSERT_TRUE(config.save_config());
    const std::string saved{ read_file(process_file(dir)) };
    EXPECT_NE(saved.find("./logs/process.log"), std::string::npos);
    EXPECT_EQ(saved.find("./logs/temporary.log"), std::string::npos);
    EXPECT_EQ(file_location(shared), std::string("./logs/process.log"));
}

/**
 * \brief   The last value set is in force, temporary or not; only a non-temporary value is saved.
 **/
TEST(ConfigManagerTest, last_set_value_wins)
{
    const fs::path dir{ test_dir() };
    const fs::path shared{ dir / "areg.init" };
    write_file(shared, { "log::*::file::location = ./logs/all.log" });
    write_file(process_file(dir), { module_entry("file::location = ./logs/process.log") });

    areg::ConfigManager config;
    ASSERT_TRUE(config.read_config(areg::String(shared.string())));

    config.set_file_location("./logs/temporary.log", true);
    config.set_file_location("./logs/saved.log", false);
    EXPECT_EQ(std::string(config.log_file_location().as_string()), std::string("./logs/saved.log"));

    config.set_file_location("./logs/again.log", true);
    EXPECT_EQ(std::string(config.log_file_location().as_string()), std::string("./logs/again.log"));

    config.set_file_location("./logs/saved.log", true);
    EXPECT_EQ(std::string(config.log_file_location().as_string()), std::string("./logs/saved.log"));

    ASSERT_TRUE(config.save_config());
    EXPECT_EQ(file_location(shared), std::string("./logs/saved.log"));

    config.set_file_location("./logs/again.log", true);
    const areg::ConfigKey& key{ areg::log_file_location() };
    config.remove_module_property(areg::String(key.section), areg::String(key.property), areg::String(key.position), areg::ConfigEntry::LogFileLocation);
    EXPECT_EQ(std::string(config.log_file_location().as_string()), std::string("./logs/all.log"));
}

/**
 * \brief   A temporary entry given after the saved entry of the same key still wins once it is set again.
 **/
TEST(ConfigManagerTest, temporary_value_set_again_wins_whatever_its_place)
{
    const std::string key{ module_entry("file::location") };
    areg::ListProperties writable;
    writable.add(areg::Property(key.c_str(), "./logs/saved.log"));
    writable.add(areg::Property(key.c_str(), "./logs/temporary.log", nullptr, true));

    areg::ConfigManager config;
    config.set_configuration(areg::ListProperties(), writable);
    config.set_file_location("./logs/again.log", true);
    EXPECT_EQ(std::string(config.log_file_location().as_string()), std::string("./logs/again.log"));
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
 * \brief   Saves from many threads through the one configuration manager of a process all succeed,
 *          and the file holds one complete save.
 **/
TEST(ConfigManagerTest, parallel_saves_of_one_file_all_succeed)
{
    constexpr uint32_t THREADS{ 8u };
    constexpr uint32_t ROUNDS { 25u };

    const fs::path dir{ test_dir() };
    const fs::path shared{ dir / "areg.init" };
    const fs::path target{ dir / "parallel.init" };
    write_file(shared, { "log::*::file::location = ./logs/all.log" });

    areg::ConfigManager config;
    ASSERT_TRUE(config.read_config(areg::String(shared.string())));

    std::atomic<uint32_t> failed{ 0u };
    std::vector<std::thread> threads;
    for (uint32_t i = 0u; i < THREADS; ++i)
    {
        threads.emplace_back([&config, &target, &failed, i]()
            {
                for (uint32_t round = 0u; round < ROUNDS; ++round)
                {
                    config.set_file_location(areg::String("./logs/saver") + areg::String::make_string(i) + ".log");
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
    EXPECT_EQ(saved.log_file_location(), config.log_file_location());
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

    areg::LogScope exact("cfgtest_abc_x.run");
    areg::LogScope nested("cfgtest_abc_x_y.run");
    areg::LogScope other("cfgtest_other.run");

    std::vector<std::string> shared{ "log::*::scope::cfgtest_abc_x.run = DEBUG"
                                   , "log::*::scope::cfgtest_abc_x_* = DEBUG"
                                   , "log::*::scope::cfgtest_other.run = DEBUG" };
    config.set_configuration(make_properties(shared), areg::ListProperties());
    areg::restore_logging();
    EXPECT_EQ(areg::scope_priority("cfgtest_abc_x.run"), prioDebug);
    EXPECT_EQ(areg::scope_priority("cfgtest_abc_x_y.run"), prioDebug);

    shared.push_back(module_entry("scope::cfgtest_abc_* = WARN"));
    config.set_configuration(make_properties(shared), areg::ListProperties());
    areg::restore_logging();
    EXPECT_EQ(areg::scope_priority("cfgtest_abc_x.run"), prioWarn);
    EXPECT_EQ(areg::scope_priority("cfgtest_abc_x_y.run"), prioWarn);

    config.set_configuration(make_properties(shared), make_properties({ module_entry("scope::cfgtest_abc_* = INFO") }));
    areg::restore_logging();
    EXPECT_EQ(areg::scope_priority("cfgtest_abc_x.run"), prioInfo);
    EXPECT_EQ(areg::scope_priority("cfgtest_abc_x_y.run"), prioInfo);
    EXPECT_EQ(areg::scope_priority("cfgtest_other.run"), prioDebug);

    config.set_configuration(oldShared, oldProcess);
    areg::restore_logging();

    // A process that was not configured before reads its configuration file again at the next start.
    if (wasConfigured == false)
    {
        config.release_properties();
    }
}

#endif  // AREG_LOGGING

//////////////////////////////////////////////////////////////////////////
// Values, setters, lists and notifications
//////////////////////////////////////////////////////////////////////////

namespace
{
    //!< Returns the text of the string.
    std::string txt(const areg::String& value)
    {
        return std::string(value.as_string());
    }

    //!< Returns the list of properties parsed from the lines.
    areg::ListProperties parse_list(const std::vector<std::string>& lines)
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

    //!< Returns the entry "<section>::<module of the process>::<rest>".
    std::string own(const std::string& section, const std::string& rest)
    {
        return section + "::" + std::string(areg::Process::instance().app_name().as_string()) + "::" + rest;
    }

    //!< Writes the lines into the memory file and returns the memory file to the start.
    void fill(areg::FileBuffer& buffer, const std::vector<std::string>& lines)
    {
        if (buffer.is_opened() == false)
        {
            buffer.open();
        }

        for (const auto& line : lines)
        {
            buffer.write_line(areg::String(line.c_str()));
        }

        buffer.move_to_begin();
    }

    //!< Returns the lines of the memory file.
    std::vector<std::string> lines_of(const areg::FileBuffer& buffer)
    {
        std::vector<std::string> result;
        areg::String line;
        buffer.move_to_begin();
        while (buffer.read_line(line) > 0)
        {
            result.push_back(txt(line));
        }

        return result;
    }

    constexpr uint32_t BUFFER_MODE{ static_cast<uint32_t>(areg::FileBase::OpenMode::Write) | static_cast<uint32_t>(areg::FileBase::OpenMode::Text) };

    //!< The configuration for all modules that sets every value to a non-default one.
    const std::vector<std::string>& shared_lines()
    {
        static const std::vector<std::string> lines{
              "config::*::version = 3.1.4"
            , "config::*::default::blocksize = 64"
            , "config::*::queue::capacity = 2048"
            , "config::*::queue::timeout = 500"
            , "config::*::queue::drop = true"
            , "log::*::version = 2.5.0"
            , "log::*::target = file | debug"
            , "log::*::enable = true"
            , "log::*::enable::file = true"
            , "log::*::enable::debug = false"
            , "log::*::file::location = ./logs/shared.log"
            , "log::*::file::append = true"
            , "log::*::remote::queue = 77"
            , "log::*::layout::enter = ENTER"
            , "log::*::layout::message = MESSAGE"
            , "log::*::layout::exit = EXIT"
            , "log::*::db::name = shared.db"
            , "service::*::list = router | logger"
            , "router::*::connect = tcpip | udp"
            , "router::*::enable::tcpip = false"
            , "router::*::address::tcpip = 10.0.0.5"
            , "router::*::port::tcpip = 9191"
            , "net::*::tcpip::sndbuf = 16"
            , "net::*::tcpip::rcvbuf = 32"
            , "net::*::tcpip::drain = 5"
            , "net::*::tcpip::pairs = 3"
            , "net::*::tcpip::timeout = 1500"
            , "net::*::tcpip::cache = 8"
            , "net::*::tcpip::keepalive = 40" };
        return lines;
    }

    //!< Records the notifications of the configuration in the order they come.
    class RecordingListener final : public areg::ConfigListener
    {
    public:
        std::vector<std::string>    events;

    protected:
        void prepare_save_configuration(areg::ConfigManager& /*config*/) final
        {
            events.push_back("prepare_save");
        }

        void post_save_configuration(areg::ConfigManager& /*config*/) final
        {
            events.push_back("post_save");
        }

        void prepare_read_configuration(areg::ConfigManager& /*config*/) final
        {
            events.push_back("prepare_read");
        }

        void post_read_configuration(areg::ConfigManager& /*config*/) final
        {
            events.push_back("post_read");
        }

        void on_setup_configuration(const areg::ListProperties& listReadonly, const areg::ListProperties& listWritable, areg::ConfigManager& /*config*/) final
        {
            events.push_back("setup:" + std::to_string(listReadonly.size()) + ":" + std::to_string(listWritable.size()));
        }
    };
}

/**
 * \brief   A configuration that was not read gives the documented default of every value.
 **/
TEST(ConfigManagerTest, defaults_when_not_configured)
{
    areg::ConfigManager config;
    EXPECT_FALSE(config.is_configured());
    EXPECT_TRUE(config.config_file().is_empty());
    EXPECT_EQ(config.readonly_properties().size(), 0u);
    EXPECT_EQ(config.module_properties().size(), 0u);

    EXPECT_TRUE(config.config_version() == areg::Version(areg::CONFIG_VERSION.data()));
    EXPECT_TRUE(config.log_version() == areg::Version(areg::CONFIG_VERSION.data()));
    EXPECT_TRUE(config.log_targets().empty());
    EXPECT_EQ(config.logging_status(), areg::DEFAULT_LOG_ENABLED);
    EXPECT_FALSE(config.log_enabled(areg::String("file")));
    EXPECT_FALSE(config.log_enabled(areg::LogTarget::File));
    EXPECT_EQ(txt(config.log_file_location()), std::string(areg::DEFAULT_LOG_FILE));
    EXPECT_FALSE(config.log_file_append());
    EXPECT_EQ(config.remote_queue_size(), areg::DEFAULT_LOG_QUEUE_SIZE);
    EXPECT_EQ(txt(config.log_layout_enter()), std::string(areg::DEFAULT_LAYOUT_SCOPE_ENTER));
    EXPECT_EQ(txt(config.log_layout_message()), std::string(areg::DEFAULT_LAYOUT_LOG_MESSAGE));
    EXPECT_EQ(txt(config.log_layout_exit()), std::string(areg::DEFAULT_LAYOUT_SCOPE_EXIT));
    EXPECT_TRUE(config.log_database_property(areg::String("name")).is_empty());

    EXPECT_TRUE(config.service_list().empty());
    EXPECT_TRUE(config.remote_service_connections(areg::String("router")).empty());
    EXPECT_TRUE(config.remote_service_name(areg::String("router"), areg::String("tcpip")).is_empty());
    EXPECT_EQ(config.remote_service_enable(areg::String("router"), areg::String("tcpip")), areg::DEFAULT_SERVICE_ENABLED);
    EXPECT_EQ(txt(config.remote_service_address(areg::String("router"), areg::String("tcpip"))), std::string(areg::LocalHost));
    EXPECT_EQ(config.remote_service_port(areg::String("router"), areg::String("tcpip")), areg::DEFAULT_ROUTER_PORT);
    EXPECT_EQ(txt(config.remote_service_name(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip)), std::string());
    EXPECT_EQ(config.remote_service_enable(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip), areg::DEFAULT_SERVICE_ENABLED);
    EXPECT_EQ(txt(config.remote_service_address(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip)), std::string(areg::LocalHost));
    EXPECT_EQ(config.remote_service_port(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip), areg::DEFAULT_ROUTER_PORT);

    EXPECT_EQ(config.buffer_block_size(), 0u);
    EXPECT_EQ(config.queue_capacity(), areg::QUEUE_DEFAULT_RING_CAPACITY);
    EXPECT_EQ(config.queue_wait_timeout(), areg::QUEUE_DEFAULT_FULL_WAIT_MS);
    EXPECT_FALSE(config.queue_drop_on_full());
    EXPECT_EQ(config.network_sndbuf(), areg::SOCKET_SEND_BUFFER_SIZE);
    EXPECT_EQ(config.network_rcvbuf(), areg::SOCKET_RECV_BUFFER_SIZE);
    EXPECT_EQ(config.network_sndbuf(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip), areg::SOCKET_SEND_BUFFER_SIZE);
    EXPECT_EQ(config.network_rcvbuf(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip), areg::SOCKET_RECV_BUFFER_SIZE);
    EXPECT_EQ(config.network_drain_limit(areg::String(), areg::String()), areg::DEFAULT_DRAIN_LIMIT);
    EXPECT_EQ(config.network_pool_pairs(areg::String(), areg::String()), areg::DEFAULT_POOL_PAIRS);
    EXPECT_EQ(config.network_timeout(areg::String(), areg::String()), 0u);
    EXPECT_EQ(config.network_cache(areg::String(), areg::String()), areg::DEFAULT_THREAD_CACHE);
    EXPECT_EQ(config.network_keepalive(areg::String(), areg::String()), areg::SOCKET_KEEPALIVE_SEC);

    EXPECT_EQ(config.module_log_scopes().size(), 0u);
    EXPECT_EQ(config.section_properties(areg::String("log")).size(), 0u);
    EXPECT_EQ(config.section_properties(areg::String()).size(), 0u);
    EXPECT_FALSE(config.exist(areg::String("log"), areg::String("file"), areg::String("location")));
    EXPECT_EQ(config.property(areg::String("log"), areg::String("file"), areg::String("location")), nullptr);
    EXPECT_EQ(config.module_property(areg::String("log"), areg::String("file"), areg::String("location")), nullptr);
}

/**
 * \brief   Every getter returns the value written for all modules in the shared configuration.
 **/
TEST(ConfigManagerTest, getters_return_shared_values)
{
    areg::FileBuffer text(BUFFER_MODE, "shared");
    fill(text, shared_lines());

    areg::ConfigManager config;
    ASSERT_TRUE(config.read_config(text));
    EXPECT_TRUE(config.is_configured());
    EXPECT_EQ(config.readonly_properties().size(), static_cast<uint32_t>(shared_lines().size()));
    EXPECT_EQ(config.module_properties().size(), 0u);

    EXPECT_TRUE(config.config_version() == areg::Version(3, 1, 4));
    EXPECT_TRUE(config.log_version() == areg::Version(2, 5, 0));
    ASSERT_EQ(config.log_targets().size(), 2u);
    EXPECT_EQ(config.log_targets()[0].value(), static_cast<uint32_t>(areg::LogTarget::File));
    EXPECT_EQ(config.log_targets()[1].value(), static_cast<uint32_t>(areg::LogTarget::Debug));
    EXPECT_TRUE(config.logging_status());
    EXPECT_TRUE(config.log_enabled(areg::String("file")));
    EXPECT_TRUE(config.log_enabled(areg::LogTarget::File));
    EXPECT_FALSE(config.log_enabled(areg::String("debug")));
    EXPECT_FALSE(config.log_enabled(areg::LogTarget::Debug));
    EXPECT_FALSE(config.log_enabled(areg::String("remote")));
    EXPECT_EQ(txt(config.log_file_location()), "./logs/shared.log");
    EXPECT_TRUE(config.log_file_append());
    EXPECT_EQ(config.remote_queue_size(), 77u);
    EXPECT_EQ(txt(config.log_layout_enter()), "ENTER");
    EXPECT_EQ(txt(config.log_layout_message()), "MESSAGE");
    EXPECT_EQ(txt(config.log_layout_exit()), "EXIT");
    EXPECT_EQ(txt(config.log_database_property(areg::String("name"))), "shared.db");
    EXPECT_TRUE(config.log_database_property(areg::String("engine")).is_empty());

    EXPECT_EQ(config.service_list().size(), 2u);
    ASSERT_EQ(config.remote_service_connections(areg::String("router")).size(), 2u);
    EXPECT_EQ(config.remote_service_connections(areg::String("router"))[1].value(), static_cast<uint32_t>(areg::ConnectionType::Udp));
    EXPECT_TRUE(config.remote_service_connections(areg::String("logger")).empty());
    EXPECT_FALSE(config.remote_service_enable(areg::String("router"), areg::String("tcpip")));
    EXPECT_FALSE(config.remote_service_enable(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip));
    EXPECT_TRUE(config.remote_service_enable(areg::String("router"), areg::String("udp")));
    EXPECT_EQ(txt(config.remote_service_address(areg::String("router"), areg::String("tcpip"))), "10.0.0.5");
    EXPECT_EQ(txt(config.remote_service_address(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip)), "10.0.0.5");
    EXPECT_EQ(config.remote_service_port(areg::String("router"), areg::String("tcpip")), 9191u);
    EXPECT_EQ(config.remote_service_port(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip), 9191u);
    EXPECT_EQ(config.remote_service_port(areg::String("router"), areg::String("udp")), areg::DEFAULT_ROUTER_PORT);

    EXPECT_EQ(config.buffer_block_size(), 64u);
    EXPECT_EQ(config.buffer_block_size(areg::String("other")), 0u);
    EXPECT_EQ(config.queue_capacity(), 2048u);
    EXPECT_EQ(config.queue_wait_timeout(), 500u);
    EXPECT_TRUE(config.queue_drop_on_full());

    const areg::String tcpip("tcpip");
    EXPECT_EQ(config.network_sndbuf(areg::String(), tcpip), 16u * 1024u);
    EXPECT_EQ(config.network_rcvbuf(areg::String(), tcpip), 32u * 1024u);
    EXPECT_EQ(config.network_sndbuf(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip), 16u * 1024u);
    EXPECT_EQ(config.network_rcvbuf(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip), 32u * 1024u);
    EXPECT_EQ(config.network_drain_limit(areg::String(), tcpip), 5u);
    EXPECT_EQ(config.network_pool_pairs(areg::String(), tcpip), 3u);
    EXPECT_EQ(config.network_timeout(areg::String(), tcpip), 1500u);
    EXPECT_EQ(config.network_cache(areg::String(), tcpip), 8u * areg::ONE_KILOBYTE);
    EXPECT_EQ(config.network_keepalive(areg::String(), tcpip), 40u);
    EXPECT_EQ(config.network_sndbuf(areg::String(), areg::String("udp")), areg::SOCKET_SEND_BUFFER_SIZE);
    EXPECT_EQ(config.network_keepalive(areg::String(), areg::String("udp")), areg::SOCKET_KEEPALIVE_SEC);
}

/**
 * \brief   A value for the module of the process wins over the value for all modules, for every
 *          network value and the queue policy; a value for another module does not apply.
 **/
TEST(ConfigManagerTest, module_value_wins_over_shared_value)
{
    std::vector<std::string> lines{ shared_lines() };
    for (const char* name : { "sndbuf", "rcvbuf", "drain", "pairs", "timeout", "cache", "keepalive" })
    {
        lines.push_back(own("net", std::string("tcpip::") + name + " = 3"));
        lines.push_back(std::string("net::foreign::tcpip::") + name + " = 99");
    }

    lines.push_back(own("config", "queue::drop = false"));
    areg::FileBuffer text(BUFFER_MODE, "module");
    fill(text, lines);

    areg::ConfigManager config;
    ASSERT_TRUE(config.read_config(text));

    const areg::String tcpip("tcpip");
    EXPECT_EQ(config.network_sndbuf(areg::String(), tcpip), 3u * 1024u);
    EXPECT_EQ(config.network_rcvbuf(areg::String(), tcpip), 3u * 1024u);
    EXPECT_EQ(config.network_drain_limit(areg::String(), tcpip), 3u);
    EXPECT_EQ(config.network_pool_pairs(areg::String(), tcpip), 3u);
    EXPECT_EQ(config.network_timeout(areg::String(), tcpip), 3u);
    EXPECT_EQ(config.network_cache(areg::String(), tcpip), 3u * areg::ONE_KILOBYTE);
    EXPECT_EQ(config.network_keepalive(areg::String(), tcpip), 3u);
    EXPECT_FALSE(config.queue_drop_on_full());
    EXPECT_TRUE(config.queue_drop_on_full(areg::String("not-the-process")));

    EXPECT_EQ(config.network_sndbuf(areg::String("foreign"), tcpip), 16u * 1024u);
    EXPECT_EQ(config.network_keepalive(areg::String("foreign"), tcpip), 40u);

    config.set_configuration(parse_list(lines), areg::ListProperties());
    EXPECT_EQ(config.network_sndbuf(areg::String("foreign"), tcpip), 99u * 1024u);
    EXPECT_EQ(config.network_keepalive(areg::String("foreign"), tcpip), 99u);
}

/**
 * \brief   A module value of zero for the timeout, the cache and the keep-alive is skipped: the
 *          value for all modules, then the default, is used.
 **/
TEST(ConfigManagerTest, zero_module_value_falls_through)
{
    areg::FileBuffer text(BUFFER_MODE, "zero");
    fill(text, { own("net", "tcpip::timeout = 0"), own("net", "tcpip::cache = 0"), own("net", "tcpip::keepalive = 0")
               , "net::*::tcpip::timeout = 7", "net::*::tcpip::cache = 9" });

    areg::ConfigManager config;
    ASSERT_TRUE(config.read_config(text));
    const areg::String tcpip("tcpip");
    EXPECT_EQ(config.network_timeout(areg::String(), tcpip), 7u);
    EXPECT_EQ(config.network_cache(areg::String(), tcpip), 9u * areg::ONE_KILOBYTE);
    EXPECT_EQ(config.network_keepalive(areg::String(), tcpip), areg::SOCKET_KEEPALIVE_SEC);
    EXPECT_EQ(config.network_timeout(areg::String("foreign"), tcpip), 7u);

    config.set_configuration(parse_list({ own("config", "queue::capacity = 0"), own("config", "queue::timeout = 0") }), areg::ListProperties());
    EXPECT_EQ(config.queue_capacity(), areg::QUEUE_DEFAULT_RING_CAPACITY);
    EXPECT_EQ(config.queue_wait_timeout(), areg::QUEUE_DEFAULT_FULL_WAIT_MS);
}

/**
 * \brief   The queue and block-size values are read for the module that is asked.
 **/
TEST(ConfigManagerTest, queue_values_of_a_module)
{
    areg::ConfigManager config;
    config.set_configuration(parse_list({ "config::*::queue::capacity = 100", "config::modx::queue::capacity = 200", "config::modx::queue::timeout = 300"
                                        , "config::modx::default::blocksize = 128", "config::modx::queue::drop = true" })
                           , areg::ListProperties());
    EXPECT_EQ(config.queue_capacity(), 100u);
    EXPECT_EQ(config.queue_capacity(areg::String("modx")), 200u);
    EXPECT_EQ(config.queue_wait_timeout(areg::String("modx")), 300u);
    EXPECT_EQ(config.queue_wait_timeout(), areg::QUEUE_DEFAULT_FULL_WAIT_MS);
    EXPECT_EQ(config.buffer_block_size(areg::String("modx")), 128u);
    EXPECT_TRUE(config.queue_drop_on_full(areg::String("modx")));
    EXPECT_FALSE(config.queue_drop_on_full());
}

/**
 * \brief   Logging is reported off for every target when the master switch is off, whatever the target says.
 **/
TEST(ConfigManagerTest, log_target_needs_master_switch)
{
    areg::FileBuffer text(BUFFER_MODE, "switch");
    fill(text, { "log::*::enable = false", "log::*::enable::file = true" });

    areg::ConfigManager config;
    ASSERT_TRUE(config.read_config(text));
    EXPECT_FALSE(config.logging_status());
    EXPECT_FALSE(config.log_enabled(areg::String("file")));

    config.set_logging_status(true);
    EXPECT_TRUE(config.logging_status());
    EXPECT_TRUE(config.log_enabled(areg::String("file")));
    EXPECT_FALSE(config.log_enabled(areg::String("debug")));
}

/**
 * \brief   Every setter stores a value for the process and the getter returns it; the shared
 *          configuration is not changed.
 **/
TEST(ConfigManagerTest, setters_store_process_values)
{
    areg::FileBuffer text(BUFFER_MODE, "setters");
    fill(text, shared_lines());
    areg::ConfigManager config;
    ASSERT_TRUE(config.read_config(text));
    const uint32_t sharedCount{ config.readonly_properties().size() };

    config.set_logging_status(false);
    EXPECT_FALSE(config.logging_status());
    config.set_logging_status(true);

    config.set_log_enabled(areg::String("debug"), true);
    EXPECT_TRUE(config.log_enabled(areg::String("debug")));
    config.set_log_enabled(areg::Identifier(static_cast<uint32_t>(areg::LogTarget::File), "file"), false);
    EXPECT_FALSE(config.log_enabled(areg::LogTarget::File));
    config.set_log_enabled(areg::LogTarget::Remote, true);
    EXPECT_TRUE(config.log_enabled(areg::LogTarget::Remote));
    EXPECT_TRUE(config.log_enabled(areg::String("remote")));

    config.set_file_location(areg::String("./logs/process.log"));
    EXPECT_EQ(txt(config.log_file_location()), "./logs/process.log");
    config.set_file_append(false);
    EXPECT_FALSE(config.log_file_append());
    config.set_remote_queue_size(5u);
    EXPECT_EQ(config.remote_queue_size(), 5u);
    config.set_layout_enter(areg::String("E2"));
    config.set_layout_message(areg::String("M2"));
    config.set_layout_exit(areg::String("X2"));
    EXPECT_EQ(txt(config.log_layout_enter()), "E2");
    EXPECT_EQ(txt(config.log_layout_message()), "M2");
    EXPECT_EQ(txt(config.log_layout_exit()), "X2");
    config.set_db_property(areg::String("name"), areg::String("process.db"));
    EXPECT_EQ(txt(config.log_database_property(areg::String("name"))), "process.db");

    config.set_service_enable(areg::String("router"), areg::String("tcpip"), true);
    EXPECT_TRUE(config.remote_service_enable(areg::String("router"), areg::String("tcpip")));
    config.set_service_address(areg::String("router"), areg::String("tcpip"), areg::String("10.1.1.1"));
    EXPECT_EQ(txt(config.remote_service_address(areg::String("router"), areg::String("tcpip"))), "10.1.1.1");
    config.set_service_port(areg::String("router"), areg::String("tcpip"), static_cast<uint16_t>(7000));
    EXPECT_EQ(config.remote_service_port(areg::String("router"), areg::String("tcpip")), 7000u);

    config.set_service_enable(areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip, false);
    EXPECT_FALSE(config.remote_service_enable(areg::String("logger"), areg::String("tcpip")));
    config.set_service_address(areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip, areg::String("10.2.2.2"));
    EXPECT_EQ(txt(config.remote_service_address(areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip)), "10.2.2.2");
    config.set_service_port(areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip, static_cast<uint16_t>(7001));
    EXPECT_EQ(config.remote_service_port(areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip), 7001u);
    EXPECT_EQ(config.remote_service_port(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip), 7000u);

    EXPECT_EQ(config.readonly_properties().size(), sharedCount);
    EXPECT_GT(config.module_properties().size(), 0u);
    EXPECT_FALSE(config.module_properties()[0].is_temporary());
}

/**
 * \brief   Setting a value equal to the shared one leaves no process value; setting another value
 *          adds one; setting the shared value again removes it.
 **/
TEST(ConfigManagerTest, setting_the_shared_value_removes_the_process_value)
{
    areg::FileBuffer text(BUFFER_MODE, "equal");
    fill(text, { "log::*::file::location = ./logs/shared.log" });
    areg::ConfigManager config;
    ASSERT_TRUE(config.read_config(text));

    config.set_file_location(areg::String("./logs/shared.log"));
    EXPECT_EQ(config.module_properties().size(), 0u);

    config.set_file_location(areg::String("./logs/other.log"));
    EXPECT_EQ(config.module_properties().size(), 1u);
    config.set_file_location(areg::String("./logs/another.log"));
    EXPECT_EQ(config.module_properties().size(), 1u);
    EXPECT_EQ(txt(config.log_file_location()), "./logs/another.log");

    config.set_file_location(areg::String("./logs/shared.log"));
    EXPECT_EQ(config.module_properties().size(), 0u);
    EXPECT_EQ(txt(config.log_file_location()), "./logs/shared.log");
}

/**
 * \brief   A temporary value is kept apart from the saved one: it wins while it differs, goes away
 *          when it equals the value below, and a saved set removes it.
 **/
TEST(ConfigManagerTest, temporary_value_lifecycle)
{
    areg::FileBuffer text(BUFFER_MODE, "temporary");
    fill(text, { "log::*::file::location = ./logs/shared.log" });
    areg::ConfigManager config;
    ASSERT_TRUE(config.read_config(text));

    config.set_file_location(areg::String("./logs/temp.log"), true);
    ASSERT_EQ(config.module_properties().size(), 1u);
    EXPECT_TRUE(config.module_properties()[0].is_temporary());
    EXPECT_EQ(txt(config.log_file_location()), "./logs/temp.log");

    config.set_file_location(areg::String("./logs/temp2.log"), true);
    EXPECT_EQ(config.module_properties().size(), 1u);
    EXPECT_EQ(txt(config.log_file_location()), "./logs/temp2.log");

    config.set_file_location(areg::String("./logs/shared.log"), true);
    EXPECT_EQ(config.module_properties().size(), 0u);
    EXPECT_EQ(txt(config.log_file_location()), "./logs/shared.log");

    config.set_file_location(areg::String("./logs/saved.log"));
    config.set_file_location(areg::String("./logs/temp.log"), true);
    ASSERT_EQ(config.module_properties().size(), 2u);
    config.set_file_location(areg::String("./logs/saved.log"), true);
    ASSERT_EQ(config.module_properties().size(), 1u);
    EXPECT_FALSE(config.module_properties()[0].is_temporary());

    config.set_file_location(areg::String("./logs/temp.log"), true);
    config.set_file_location(areg::String("./logs/saved2.log"));
    ASSERT_EQ(config.module_properties().size(), 1u);
    EXPECT_FALSE(config.module_properties()[0].is_temporary());
    EXPECT_EQ(txt(config.log_file_location()), "./logs/saved2.log");

    config.set_file_location(areg::String("./logs/shared.log"));
    EXPECT_EQ(config.module_properties().size(), 0u);
}

/**
 * \brief   A property is found for the module before the entry for all modules, and an entry for
 *          another module is never found.
 **/
TEST(ConfigManagerTest, property_lookup_order)
{
    areg::ConfigManager config;
    const areg::ListProperties shared{ parse_list({ "log::*::file::location = all", own("log", "file::location = shared-module")
                                                  , "log::foreign::file::location = foreign", "log::*::file::append = true" }) };
    config.set_configuration(shared, parse_list({ own("log", "file::location = process") }));

    const areg::String log("log"), file("file"), location("location"), append("append");
    ASSERT_NE(config.property(log, file, location), nullptr);
    EXPECT_EQ(txt(config.property(log, file, location)->value_string()), "process");
    EXPECT_TRUE(config.exist(log, file, location));
    ASSERT_NE(config.module_property(log, file, location), nullptr);
    EXPECT_EQ(config.module_property(log, file, append), nullptr);
    ASSERT_NE(config.property(log, file, append), nullptr);
    EXPECT_EQ(txt(config.property(log, file, append)->value_string()), "true");
    EXPECT_TRUE(config.exist(log, file, append));
    EXPECT_FALSE(config.exist(log, file, areg::String("missing")));
    EXPECT_FALSE(config.exist(areg::String("none"), file, location));

    EXPECT_EQ(txt(config.property_value(log, file, location)->value()), "process");
    EXPECT_EQ(config.property_value(log, file, areg::String("missing")), nullptr);
    EXPECT_EQ(config.module_property_value(log, file, append), nullptr);

    areg::PropertyValue* editable{ config.module_property_value(log, file, location) };
    ASSERT_NE(editable, nullptr);
    editable->set_value("edited");
    EXPECT_EQ(txt(config.log_file_location()), "edited");

    const areg::PropertyKey key(areg::String("log::*::file::location"));
    EXPECT_NE(config.property(key), nullptr);
    EXPECT_NE(config.property_value(key), nullptr);
    EXPECT_EQ(txt(config.module_property_value(key)->value()), "edited");
    EXPECT_NE(config.module_property(key), nullptr);

    config.set_configuration(shared, areg::ListProperties());
    EXPECT_EQ(txt(config.log_file_location()), "shared-module");
    config.set_configuration(parse_list({ "log::*::file::location = all" }), areg::ListProperties());
    EXPECT_EQ(txt(config.log_file_location()), "all");
}

/**
 * \brief   The constant manager gives the module property values, and the manager gives its lock.
 **/
TEST(ConfigManagerTest, constant_manager_gives_module_property_values_and_the_lock)
{
    areg::ConfigManager config;
    config.set_configuration(parse_list({ "log::*::file::location = all" })
                           , parse_list({ own("log", "file::location = process") }));
    const areg::ConfigManager& readonly{ config };

    const areg::String log("log"), file("file"), location("location"), append("append");
    const areg::PropertyValue* byName{ readonly.module_property_value(log, file, location) };
    ASSERT_NE(byName, nullptr);
    EXPECT_EQ(txt(byName->value()), "process");
    EXPECT_EQ(readonly.module_property_value(log, file, append), nullptr);

    const areg::PropertyKey key(areg::String("log::*::file::location"));
    EXPECT_EQ(readonly.module_property_value(key), byName);

    EXPECT_EQ(&config.lockable(), &readonly.lockable());
}

/**
 * \brief   The state of a log target given as identifier is the one of the target given as name.
 **/
TEST(ConfigManagerTest, log_target_by_identifier_is_the_target_by_name)
{
    areg::FileBuffer text(BUFFER_MODE, "identifier");
    fill(text, { "log::*::enable = true", "log::*::enable::file = true", "log::*::enable::debug = false" });

    areg::ConfigManager config;
    ASSERT_TRUE(config.read_config(text));
    const areg::Identifier fileTarget(static_cast<uint32_t>(areg::LogTarget::File), "file");
    const areg::Identifier debugTarget(static_cast<uint32_t>(areg::LogTarget::Debug), "debug");
    EXPECT_TRUE(config.log_enabled(fileTarget));
    EXPECT_FALSE(config.log_enabled(debugTarget));
}

/**
 * \brief   The list of a section holds the process values, then the shared values of the module
 *          that the process does not override.
 **/
TEST(ConfigManagerTest, section_properties)
{
    areg::ConfigManager config;
    config.set_configuration(parse_list({ own("log", "file::location = shared"), own("log", "file::append = true")
                                        , "log::foreign::file::location = foreign", own("net", "tcpip::sndbuf = 1") })
                           , parse_list({ own("log", "file::location = process") }));

    const areg::ListProperties logs{ config.section_properties(areg::String("log")) };
    ASSERT_EQ(logs.size(), 2u);
    EXPECT_EQ(txt(logs[0].value_string()), "process");
    EXPECT_EQ(txt(logs[1].value_string()), "true");
    EXPECT_EQ(config.section_properties(areg::String("net")).size(), 1u);
    EXPECT_EQ(config.section_properties(areg::String("absent")).size(), 0u);
    EXPECT_EQ(config.section_properties(areg::String()).size(), 0u);
}

/**
 * \brief   Module values are added only when missing, replaced as a list, removed by key, by
 *          property name and by section, and released.
 **/
TEST(ConfigManagerTest, module_property_list_operations)
{
    areg::ConfigManager config;
    config.set_configuration(parse_list({ "log::*::file::location = shared" }), areg::ListProperties());

    const areg::ListProperties added{ parse_list({ own("log", "file::location = a"), own("log", "file::append = true"), own("net", "tcpip::sndbuf = 4") }) };
    EXPECT_EQ(config.add_module_properties(added), 2);
    EXPECT_EQ(config.add_module_properties(added), 0);
    EXPECT_EQ(config.module_properties().size(), 2u);

    config.remove_module_property(areg::String("log"), areg::String("file"), areg::String("append"));
    EXPECT_EQ(config.module_properties().size(), 1u);
    config.remove_module_property(areg::String("log"), areg::String("file"), areg::String("append"));
    EXPECT_EQ(config.module_properties().size(), 1u);

    config.remove_module_property(areg::PropertyKey(areg::String("net"), areg::String(areg::Process::instance().app_name()), areg::String("tcpip"), areg::String("sndbuf")));
    EXPECT_EQ(config.module_properties().size(), 0u);

    config.replace_module_property(added);
    EXPECT_EQ(config.module_properties().size(), 3u);
    EXPECT_EQ(config.remove_module_properties(areg::String("log"), areg::String("file")), 2);
    EXPECT_EQ(config.remove_module_properties(areg::String("log"), areg::String("file")), 0);
    EXPECT_EQ(config.module_properties().size(), 1u);

    config.replace_module_property(added);
    EXPECT_EQ(config.remove_module_properties(areg::String("log"), areg::String("file"), areg::ConfigEntry::LogFileAppend), 1);
    EXPECT_EQ(config.module_properties().size(), 2u);

    config.remove_section_properties(areg::String("log"));
    EXPECT_EQ(config.module_properties().size(), 1u);
    config.remove_section_properties(areg::String("log"));
    EXPECT_EQ(config.module_properties().size(), 1u);

    config.release_module_properties();
    EXPECT_EQ(config.module_properties().size(), 0u);
    EXPECT_EQ(config.readonly_properties().size(), 1u);
    EXPECT_TRUE(config.is_configured());

    config.set_module_property(areg::PropertyKey(areg::String("log::*::file::location")), areg::String("viakey"));
    EXPECT_EQ(txt(config.log_file_location()), "viakey");

    config.release_properties();
    EXPECT_FALSE(config.is_configured());
    EXPECT_EQ(config.readonly_properties().size(), 0u);
    EXPECT_EQ(config.module_properties().size(), 0u);
}

/**
 * \brief   The scopes of the process are listed in three layers and the list returned is ordered
 *          shared, module, process.
 **/
TEST(ConfigManagerTest, scope_layers_and_lists)
{
    areg::ConfigManager config;
    config.set_configuration(parse_list({ "log::*::scope::sh_a = DEBUG", own("log", "scope::mod_a = WARN"), "log::*::file::append = true" })
                           , parse_list({ own("log", "scope::proc_a = INFO") }));

    std::vector<areg::Property> process, module, shared;
    config.log_scope_layers(process, module, shared);
    ASSERT_EQ(process.size(), 1u);
    ASSERT_EQ(module.size(), 1u);
    ASSERT_EQ(shared.size(), 1u);
    EXPECT_EQ(txt(process[0].key().position()), "proc_a");
    EXPECT_EQ(txt(module[0].key().position()), "mod_a");
    EXPECT_EQ(txt(shared[0].key().position()), "sh_a");

    std::vector<areg::Property> all;
    EXPECT_EQ(config.module_log_scopes(all), 3u);
    ASSERT_EQ(all.size(), 3u);
    EXPECT_EQ(txt(all[0].key().position()), "sh_a");
    EXPECT_EQ(txt(all[1].key().position()), "mod_a");
    EXPECT_EQ(txt(all[2].key().position()), "proc_a");
    EXPECT_EQ(config.module_log_scopes().size(), 3u);
}

/**
 * \brief   Scopes added by name and by priority mask are stored, replaced by the same name and
 *          removed one by one or all together.
 **/
TEST(ConfigManagerTest, add_and_remove_scopes)
{
    areg::ConfigManager config;
    config.set_configuration(parse_list({ "log::*::scope::shared_scope = DEBUG" }), areg::ListProperties());

    config.add_log_scope(areg::String("my_scope"), areg::String("WARN"));
    config.add_log_scope(areg::String("my_scope"), areg::String("ERROR"));
    config.add_log_scope(areg::String("other_scope"), static_cast<uint32_t>(areg::LogPriority::PrioInfo));
    EXPECT_EQ(config.module_properties().size(), 2u);

    const areg::Property* scope{ config.module_property(areg::String("log"), areg::String("scope"), areg::String("my_scope")) };
    ASSERT_NE(scope, nullptr);
    EXPECT_EQ(txt(scope->value_string()), "ERROR");
    EXPECT_FALSE(scope->value_string().is_empty());
    const areg::Property* masked{ config.module_property(areg::String("log"), areg::String("scope"), areg::String("other_scope")) };
    ASSERT_NE(masked, nullptr);
    EXPECT_FALSE(masked->value_string().is_empty());

    EXPECT_TRUE(config.remove_scope(areg::String("my_scope")));
    EXPECT_FALSE(config.remove_scope(areg::String("my_scope")));
    EXPECT_FALSE(config.remove_scope(areg::String("shared_scope")));
    EXPECT_EQ(config.module_properties().size(), 1u);

    config.add_log_scope(areg::String("third_scope"), areg::String("DEBUG"));
    EXPECT_EQ(config.remove_module_scopes(), 2);
    EXPECT_EQ(config.module_properties().size(), 0u);
    EXPECT_EQ(config.remove_module_scopes(), 0);
}

/**
 * \brief   A list of scopes added with the search flag replaces a scope of the same name; without
 *          it every scope is appended.
 **/
TEST(ConfigManagerTest, add_scope_list)
{
    areg::ConfigManager config;
    config.set_configuration(areg::ListProperties(), areg::ListProperties());

    const std::vector<areg::Property> first{ areg::Property(own("log", "scope::s1").c_str(), "WARN"), areg::Property(own("log", "scope::s2").c_str(), "INFO") };
    config.add_log_scopes(first, true);
    EXPECT_EQ(config.module_properties().size(), 2u);

    const std::vector<areg::Property> again{ areg::Property(own("log", "scope::s1").c_str(), "ERROR") };
    config.add_log_scopes(again, true);
    EXPECT_EQ(config.module_properties().size(), 2u);
    EXPECT_EQ(txt(config.module_property(areg::String("log"), areg::String("scope"), areg::String("s1"))->value_string()), "ERROR");

    config.add_log_scopes(again, false);
    EXPECT_EQ(config.module_properties().size(), 3u);
}

/**
 * \brief   The configuration set directly is used as given and the listener is told the sizes.
 **/
TEST(ConfigManagerTest, set_configuration_notifies)
{
    RecordingListener listener;
    areg::ConfigManager config;
    config.set_configuration(parse_list({ "log::*::file::append = true", "log::*::file::location = a" }), parse_list({ own("log", "file::append = false") }), &listener);
    EXPECT_TRUE(config.is_configured());
    EXPECT_FALSE(config.log_file_append());
    ASSERT_EQ(listener.events.size(), 1u);
    EXPECT_EQ(listener.events[0], "setup:2:1");

    config.set_configuration(areg::ListProperties(), areg::ListProperties());
    EXPECT_TRUE(config.is_configured());
    EXPECT_EQ(listener.events.size(), 1u);
    EXPECT_FALSE(config.log_file_append());
}

/**
 * \brief   Reading a memory file notifies before and after, ignores lines that are not properties
 *          and entries of other modules, and a second read keeps the first configuration.
 **/
TEST(ConfigManagerTest, read_from_text)
{
    areg::FileBuffer text(BUFFER_MODE, "text");
    fill(text, { "# a comment line", "", "this is garbage", "log::*::file::location = first", "log::foreign::file::location = foreign"
               , own("log", "file::append = true"), "log::*::file::location" });

    RecordingListener listener;
    areg::ConfigManager config;
    ASSERT_TRUE(config.read_config(text, &listener));
    EXPECT_EQ(listener.events, (std::vector<std::string>{ "prepare_read", "post_read" }));
    EXPECT_EQ(txt(config.log_file_location()), "first");
    EXPECT_TRUE(config.log_file_append());
    EXPECT_FALSE(config.exist(areg::String("log"), areg::String("file"), areg::String("missing")));

    areg::FileBuffer second(BUFFER_MODE, "second");
    fill(second, { "log::*::file::location = second" });
    EXPECT_TRUE(config.read_config(second, &listener));
    EXPECT_EQ(listener.events.size(), 2u);
    EXPECT_EQ(txt(config.log_file_location()), "first");
}

/**
 * \brief   A text without any property, an empty text and a file that is not open give no
 *          configuration; the listener is still told.
 **/
TEST(ConfigManagerTest, read_from_text_without_properties)
{
    areg::FileBuffer empty(BUFFER_MODE, "empty");
    ASSERT_TRUE(empty.open());
    areg::ConfigManager first;
    EXPECT_FALSE(first.read_config(empty));
    EXPECT_FALSE(first.is_configured());

    areg::FileBuffer garbage(BUFFER_MODE, "garbage");
    fill(garbage, { "# only comments", "   ", "no equal sign here", "foreign::other::x::y = 1" });
    RecordingListener listener;
    areg::ConfigManager second;
    EXPECT_FALSE(second.read_config(garbage, &listener));
    EXPECT_FALSE(second.is_configured());
    EXPECT_EQ(listener.events, (std::vector<std::string>{ "prepare_read", "post_read" }));
    EXPECT_EQ(txt(second.log_file_location()), std::string(areg::DEFAULT_LOG_FILE));

    areg::File closed;
    areg::ConfigManager third;
    EXPECT_FALSE(third.read_config(closed));
    EXPECT_FALSE(third.is_configured());
}

/**
 * \brief   A missing configuration file is not an error that throws: it reads nothing and the
 *          defaults stay; the listener is not called because no file was opened.
 **/
TEST(ConfigManagerTest, read_missing_file)
{
    const fs::path dir{ test_dir() };
    RecordingListener listener;
    areg::ConfigManager config;
    EXPECT_FALSE(config.read_config(areg::String((dir / "absent.init").string()), &listener));
    EXPECT_FALSE(config.is_configured());
    EXPECT_TRUE(listener.events.empty());
    EXPECT_TRUE(config.config_file().is_empty());
    EXPECT_FALSE(config.read_process_config());
}

/**
 * \brief   A file read once is not read again: a second call reports the existing configuration.
 **/
TEST(ConfigManagerTest, read_file_twice_keeps_first)
{
    const fs::path dir{ test_dir() };
    const fs::path shared{ dir / "areg.init" };
    write_file(shared, { "log::*::file::location = first" });
    areg::ConfigManager config;
    ASSERT_TRUE(config.read_config(areg::String(shared.string())));
    EXPECT_FALSE(config.config_file().is_empty());

    write_file(shared, { "log::*::file::location = second" });
    EXPECT_TRUE(config.read_config(areg::String(shared.string())));
    EXPECT_EQ(txt(config.log_file_location()), "first");

    EXPECT_FALSE(config.read_process_config());
    EXPECT_EQ(txt(config.log_file_location()), "first");
}

/**
 * \brief   Reproduces the defect: the release of the properties kept the path of the file read
 *          before, so the next read hit the assertion and a failed read kept the old path.
 **/
TEST(ConfigManagerTest, release_of_properties_forgets_the_file)
{
    const fs::path dir{ test_dir() };
    const fs::path first{ dir / "first.init" };
    const fs::path second{ dir / "second.init" };
    write_file(first, { "log::*::file::location = first" });
    write_file(second, { "log::*::file::location = second" });

    areg::ConfigManager config;
    ASSERT_TRUE(config.read_config(areg::String(first.string())));
    config.release_properties();
    ASSERT_TRUE(config.config_file().is_empty());

    EXPECT_FALSE(config.read_config(areg::String((dir / "absent.init").string())));
    EXPECT_TRUE(config.config_file().is_empty());

    ASSERT_TRUE(config.read_config(areg::String(second.string())));
    EXPECT_EQ(txt(config.log_file_location()), "second");
    EXPECT_EQ(fs::path(config.config_file().as_string()).filename(), second.filename());
}

/**
 * \brief   A save with all values writes the shared entries the process does not override and the
 *          saved process values, and never a temporary value.
 **/
TEST(ConfigManagerTest, save_all_writes_effective_values)
{
    areg::ListProperties shared{ parse_list({ "log::*::file::location = shared", "log::*::file::append = true", "log::*::version = 2.0.0"
                                            , own("log", "layout::exit = shared-module"), own("log", "layout::message = shared-message") }) };
    shared.add(areg::Property(areg::PropertyKey(areg::String("log::*::remote::queue")), areg::PropertyValue("9"), areg::String(), true));

    areg::ConfigManager config;
    config.set_configuration(shared, areg::ListProperties());
    config.set_file_location(areg::String("process"));
    config.set_file_append(false, true);
    config.set_layout_enter(areg::String("enter"));
    config.set_layout_message(areg::String("override"));

    areg::FileBuffer source(BUFFER_MODE, "src");
    ASSERT_TRUE(source.open());
    areg::FileBuffer target(BUFFER_MODE, "dst");
    ASSERT_TRUE(target.open());
    RecordingListener listener;
    ASSERT_TRUE(config.save_config(source, target, true, &listener));
    EXPECT_EQ(listener.events, (std::vector<std::string>{ "prepare_save", "post_save" }));

    const std::vector<std::string> lines{ lines_of(target) };
    ASSERT_EQ(lines.size(), 7u);
    EXPECT_EQ(lines[0], "log::*::file::location = shared;");
    EXPECT_EQ(lines[1], "log::*::file::append = true;");
    EXPECT_EQ(lines[2], "log::*::version = 2.0.0;");
    EXPECT_EQ(lines[3], own("log", "layout::exit = shared-module;"));
    EXPECT_EQ(lines[4], own("log", "file::location = process;"));
    EXPECT_EQ(lines[5], own("log", "layout::enter = enter;"));
    EXPECT_EQ(lines[6], own("log", "layout::message = override;"));
}

/**
 * \brief   A save that keeps the source copies lines of other modules and of comments, replaces
 *          the lines of the process with its saved values and keeps the line of a temporary value.
 **/
TEST(ConfigManagerTest, save_merges_into_source)
{
    areg::ConfigManager config;
    config.set_configuration(parse_list({ "log::*::file::location = shared" }), areg::ListProperties());
    config.set_file_location(areg::String("new"));
    config.set_file_append(true, true);

    areg::FileBuffer source(BUFFER_MODE, "src");
    fill(source, { "# header", "log::*::file::location = shared", "log::foreign::file::location = foreign"
                 , own("log", "file::location = old"), own("log", "file::append = false"), own("log", "layout::exit = gone"), "not a property" });
    areg::FileBuffer target(BUFFER_MODE, "dst");
    ASSERT_TRUE(target.open());
    ASSERT_TRUE(config.save_config(source, target, false));

    const std::vector<std::string> lines{ lines_of(target) };
    ASSERT_EQ(lines.size(), 6u);
    EXPECT_EQ(lines[0], "# header");
    EXPECT_EQ(lines[1], "log::*::file::location = shared");
    EXPECT_EQ(lines[2], "log::foreign::file::location = foreign");
    EXPECT_EQ(lines[3], own("log", "file::append = false"));
    EXPECT_EQ(lines[4], "not a property");
    EXPECT_EQ(lines[5], own("log", "file::location = new;"));
}

/**
 * \brief   A save without a source writes only the saved process values; a destination that is
 *          not open or cannot be written makes the save fail and writes nothing.
 **/
TEST(ConfigManagerTest, save_without_source_and_to_closed_destination)
{
    areg::ConfigManager config;
    config.set_configuration(parse_list({ "log::*::file::location = shared" }), areg::ListProperties());
    config.set_file_location(areg::String("new"));

    areg::File noSource;
    areg::FileBuffer target(BUFFER_MODE, "dst");
    ASSERT_TRUE(target.open());
    RecordingListener listener;
    ASSERT_TRUE(config.save_config(noSource, target, false, &listener));
    EXPECT_EQ(lines_of(target), (std::vector<std::string>{ own("log", "file::location = new;") }));
    EXPECT_EQ(listener.events, (std::vector<std::string>{ "prepare_save", "post_save" }));

    areg::File closed;
    areg::FileBuffer source(BUFFER_MODE, "src");
    ASSERT_TRUE(source.open());
    EXPECT_FALSE(config.save_config(source, closed, false));
    EXPECT_FALSE(config.save_config(source, closed, true));

    const fs::path dir{ test_dir() };
    write_file(dir / "ro.txt", { "x" });
    const std::string before{ read_file(dir / "ro.txt") };
    areg::File readOnly(areg::String((dir / "ro.txt").string()), static_cast<uint32_t>(areg::File::OpenMode::OnlyRead) | static_cast<uint32_t>(areg::File::OpenMode::Text) | static_cast<uint32_t>(areg::File::OpenMode::Exist));
    ASSERT_TRUE(readOnly.open());
    EXPECT_FALSE(config.save_config(source, readOnly, true));
    EXPECT_EQ(read_file(dir / "ro.txt"), before);
}

/**
 * \brief   A configuration saved to a file in a directory that does not exist is read back from it,
 *          and a second save replaces the first without leaving temporary files.
 **/
TEST(ConfigManagerTest, save_to_new_directory_and_read_back)
{
    const fs::path dir{ test_dir() };
    const fs::path shared{ dir / "areg.init" };
    write_file(shared, { "log::*::file::location = shared", "log::*::file::append = true" });
    areg::ConfigManager config;
    ASSERT_TRUE(config.read_config(areg::String(shared.string())));
    config.set_file_location(areg::String("saved"));

    const fs::path target{ dir / "nested" / "deeper" / "copy.init" };
    ASSERT_TRUE(config.save_config(areg::String(target.string())));
    ASSERT_TRUE(fs::exists(target));
    EXPECT_NE(read_file(target).find("file::location = saved;"), std::string::npos);

    config.set_file_location(areg::String("saved-again"));
    ASSERT_TRUE(config.save_config(areg::String(target.string())));
    const std::string content{ read_file(target) };
    EXPECT_NE(content.find("file::location = saved-again;"), std::string::npos);
    EXPECT_EQ(content.find("file::location = saved;"), std::string::npos);

    for (const auto& entry : fs::directory_iterator(target.parent_path()))
    {
        EXPECT_NE(entry.path().extension().string(), ".tmp");
    }

    areg::FileBuffer back(BUFFER_MODE, "back");
    fill(back, { "log::*::file::location = base" });
    areg::File copy(areg::String(target.string()), static_cast<uint32_t>(areg::File::OpenMode::Read) | static_cast<uint32_t>(areg::File::OpenMode::Text) | static_cast<uint32_t>(areg::File::OpenMode::Exist));
    ASSERT_TRUE(copy.open());
    areg::ConfigManager reader;
    ASSERT_TRUE(reader.read_config(copy));
    EXPECT_EQ(txt(reader.log_file_location()), "saved-again");
}

/**
 * \brief   A save with no value for the process writes the file of the process with the lines it
 *          had unchanged; the saved values of the process file are read at the next start.
 **/
TEST(ConfigManagerTest, process_file_round_trip)
{
    const fs::path dir{ test_dir() };
    const fs::path shared{ dir / "areg.init" };
    write_file(shared, { "log::*::file::location = shared" });

    areg::ConfigManager first;
    ASSERT_TRUE(first.read_config(areg::String(shared.string())));
    first.set_file_location(areg::String("persisted"));
    ASSERT_TRUE(first.save_config());
    ASSERT_TRUE(fs::exists(process_file(dir)));

    areg::ConfigManager second;
    ASSERT_TRUE(second.read_config(areg::String(shared.string())));
    EXPECT_EQ(txt(second.log_file_location()), "persisted");
    EXPECT_EQ(second.module_properties().size(), 1u);
    EXPECT_TRUE(second.read_process_config());
    EXPECT_EQ(second.module_properties().size(), 1u);

    second.set_file_location(areg::String("shared"));
    EXPECT_EQ(second.module_properties().size(), 0u);
    ASSERT_TRUE(second.save_config());

    areg::ConfigManager third;
    ASSERT_TRUE(third.read_config(areg::String(shared.string())));
    EXPECT_EQ(txt(third.log_file_location()), "shared");
    EXPECT_EQ(third.module_properties().size(), 0u);
}

/**
 * \brief   A process file that holds only lines that are not properties gives no process values and
 *          reports that nothing was read.
 **/
TEST(ConfigManagerTest, process_file_without_properties)
{
    const fs::path dir{ test_dir() };
    const fs::path shared{ dir / "areg.init" };
    write_file(shared, { "log::*::file::location = shared" });
    write_file(process_file(dir), { "# nothing here", "log::foreign::file::location = foreign" });

    areg::ConfigManager config;
    ASSERT_TRUE(config.read_config(areg::String(shared.string())));
    EXPECT_EQ(config.module_properties().size(), 0u);
    EXPECT_FALSE(config.read_process_config());
    EXPECT_EQ(txt(config.log_file_location()), "shared");
}

/**
 * \brief   The notifications of a read from a file and of a save to a file come in the documented
 *          order.
 **/
TEST(ConfigManagerTest, file_read_and_save_notify)
{
    const fs::path dir{ test_dir() };
    const fs::path shared{ dir / "areg.init" };
    write_file(shared, { "log::*::file::location = shared" });

    RecordingListener listener;
    areg::ConfigManager config;
    ASSERT_TRUE(config.read_config(areg::String(shared.string()), &listener));
    EXPECT_EQ(listener.events, (std::vector<std::string>{ "prepare_read", "post_read" }));

    listener.events.clear();
    config.set_file_location(areg::String("changed"));
    ASSERT_TRUE(config.save_config(areg::String(), &listener));
    EXPECT_EQ(listener.events, (std::vector<std::string>{ "prepare_save", "post_save" }));
}

/**
 * \brief   The process name of a remote service is read from the entry without position, for any
 *          connection type.
 **/
TEST(ConfigManagerTest, remote_service_name_is_one_per_service)
{
    areg::ConfigManager config;
    config.set_configuration(parse_list({ "router::*::service = mtrouter", "logger::*::service = logcollector ;" }), areg::ListProperties());
    EXPECT_EQ(txt(config.remote_service_name(areg::String("router"), areg::String("tcpip"))), "mtrouter");
    EXPECT_EQ(txt(config.remote_service_name(areg::String("router"), areg::String())), "mtrouter");
    EXPECT_EQ(txt(config.remote_service_name(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip)), "mtrouter");
    EXPECT_EQ(txt(config.remote_service_name(areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip)), "logcollector");
}

/**
 * \brief   A remote service without a process name entry has an empty name.
 **/
TEST(ConfigManagerTest, remote_service_name_of_unconfigured_service_is_empty)
{
    areg::ConfigManager config;
    config.set_configuration(parse_list({ "router::*::service = mtrouter" }), areg::ListProperties());
    EXPECT_TRUE(config.remote_service_name(areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip).is_empty());
    EXPECT_TRUE(config.remote_service_name(areg::String("unknown"), areg::String("tcpip")).is_empty());
}

/**
 * \brief   A temporary value set on a list that holds the saved value first still wins: the
 *          temporary entry is moved in front of the saved one.
 **/
TEST(ConfigManagerTest, temporary_value_wins_when_list_has_saved_first)
{
    areg::ConfigManager config;
    config.set_configuration(parse_list({ "log::*::file::location = shared" }), areg::ListProperties());

    areg::ListProperties list;
    list.add(areg::Property(areg::PropertyKey(areg::String(own("log", "file::location").c_str())), areg::PropertyValue("saved"), areg::String(), false));
    list.add(areg::Property(areg::PropertyKey(areg::String(own("log", "file::location").c_str())), areg::PropertyValue("temp"), areg::String(), true));
    config.replace_module_property(list);
    EXPECT_EQ(txt(config.log_file_location()), "saved");

    config.set_file_location(areg::String("temp2"), true);
    ASSERT_EQ(config.module_properties().size(), 2u);
    EXPECT_TRUE(config.module_properties()[0].is_temporary());
    EXPECT_FALSE(config.module_properties()[1].is_temporary());
    EXPECT_EQ(txt(config.log_file_location()), "temp2");
}

/**
 * \brief   A configuration set in memory is saved to the file that is asked for and read back.
 **/
TEST(ConfigManagerTest, save_in_memory_configuration_to_file)
{
    const fs::path dir{ test_dir() };
    areg::ConfigManager config;
    config.set_configuration(parse_list({ "log::*::file::location = shared" }), areg::ListProperties());
    config.set_file_location(areg::String("memory"));

    const fs::path target{ dir / "memory.init" };
    ASSERT_TRUE(config.save_config(areg::String(target.string())));

    areg::ConfigManager reader;
    ASSERT_TRUE(reader.read_config(areg::String(target.string())));
    EXPECT_EQ(txt(reader.log_file_location()), "memory");
}

/**
 * \brief   A file without any property is not a configuration and does not block a later read.
 **/
TEST(ConfigManagerTest, file_without_properties_is_not_a_configuration)
{
    const fs::path dir{ test_dir() };
    const fs::path shared{ dir / "areg.init" };
    write_file(shared, { "# only a comment", "garbage line" });

    areg::ConfigManager config;
    EXPECT_FALSE(config.read_config(areg::String(shared.string())));
    EXPECT_FALSE(config.is_configured());
    EXPECT_TRUE(config.config_file().is_empty());

    write_file(shared, { "log::*::file::location = later" });
    EXPECT_TRUE(config.read_config(areg::String(shared.string())));
    EXPECT_EQ(txt(config.log_file_location()), "later");
}

/**
 * \brief   Without a path the configuration is read from the default file in the working directory.
 **/
TEST(ConfigManagerTest, default_file_is_read_from_working_directory)
{
    struct WorkingDirectory
    {
        fs::path    previous{ fs::current_path() };

        ~WorkingDirectory()
        {
            std::error_code err;
            fs::current_path(previous, err);
        }
    };

    const fs::path dir{ fs::absolute(test_dir()) };
    std::error_code err;
    fs::create_directories(dir / "config", err);
    write_file(dir / "config" / "areg.init", { "log::*::file::location = from-default" });

    WorkingDirectory guard;
    fs::current_path(dir, err);
    ASSERT_FALSE(err);

    areg::ConfigManager config;
    ASSERT_TRUE(config.read_config());
    EXPECT_EQ(txt(config.log_file_location()), "from-default");
    EXPECT_FALSE(config.config_file().is_empty());
}

/**
 * \brief   A shared file that has the name of the process file is read as the shared file; the
 *          process file is never the shared file.
 **/
TEST(ConfigManagerTest, shared_file_named_like_process_file)
{
    const fs::path dir{ test_dir() };
    const fs::path shared{ process_file(dir) };
    write_file(shared, { "log::*::file::location = same-file" });

    areg::ConfigManager config;
    ASSERT_TRUE(config.read_config(areg::String(shared.string())));
    EXPECT_EQ(txt(config.log_file_location()), "same-file");
    EXPECT_EQ(config.module_properties().size(), 0u);
    EXPECT_FALSE(config.read_process_config());
    EXPECT_FALSE(config.save_config());
}

/**
 * \brief   A save fails and leaves the file as it was when the lock file cannot be taken.
 **/
TEST(ConfigManagerTest, save_fails_when_lock_cannot_be_taken)
{
    const fs::path dir{ test_dir() };
    const fs::path target{ dir / "locked.init" };
    write_file(target, { "log::*::file::location = untouched" });
    const std::string before{ read_file(target) };
    std::error_code err;
    fs::create_directories(dir / "locked.init.lock", err);
    ASSERT_FALSE(err);

    areg::ConfigManager config;
    config.set_configuration(parse_list({ "log::*::file::location = shared" }), areg::ListProperties());
    config.set_file_location(areg::String("changed"));
    EXPECT_FALSE(config.save_config(areg::String(target.string())));
    EXPECT_EQ(read_file(target), before);
}

/**
 * \brief   A process file is read even when its lock file cannot be created.
 **/
TEST(ConfigManagerTest, process_file_is_read_without_its_lock)
{
    const fs::path dir{ test_dir() };
    const fs::path shared{ dir / "areg.init" };
    write_file(shared, { "log::*::file::location = shared" });
    write_file(process_file(dir), { own("log", "file::location = process") });
    std::error_code err;
    fs::create_directories(process_file(dir).string() + ".lock", err);
    ASSERT_FALSE(err);

    areg::ConfigManager config;
    ASSERT_TRUE(config.read_config(areg::String(shared.string())));
    EXPECT_EQ(txt(config.log_file_location()), "process");
}

/**
 * \brief   The invalid key type asks for any key type; section "*" lists the entries of that section once.
 **/
TEST(ConfigManagerTest, invalid_key_type_means_any_and_star_section)
{
    areg::ConfigManager config;
    config.set_configuration(parse_list({ "log::*::file::location = shared", "*::*::service = mtrouter" }), areg::ListProperties());

    const areg::String log("log"), file("file"), location("location");
    ASSERT_NE(config.property(log, file, location, areg::ConfigEntry::Invalid), nullptr);
    EXPECT_EQ(config.module_property(log, file, location, areg::ConfigEntry::Invalid), nullptr);

    config.set_module_property(log, file, location, areg::String("typed"), areg::ConfigEntry::Invalid);
    ASSERT_NE(config.module_property(log, file, location, areg::ConfigEntry::Invalid), nullptr);
    EXPECT_EQ(txt(config.log_file_location()), "typed");

    const areg::ListProperties star{ config.section_properties(areg::String("*")) };
    EXPECT_GE(star.size(), 1u);
    for (uint32_t i = 0u; i < star.size(); ++i)
    {
        EXPECT_EQ(txt(star[i].key().section()), "*");
    }
}

/**
 * \brief   Removing by name removes only the entries of that section, name and key type.
 **/
TEST(ConfigManagerTest, remove_by_name_keeps_other_entries)
{
    areg::ConfigManager config;
    config.set_configuration(areg::ListProperties()
                           , parse_list({ own("log", "file::location = a"), own("log", "file::append = true"), own("log", "layout::enter = e")
                                        , own("net", "tcpip::sndbuf = 4") }));

    EXPECT_EQ(config.remove_module_properties(areg::String("log"), areg::String("missing")), 0);
    EXPECT_EQ(config.remove_module_properties(areg::String("none"), areg::String("file")), 0);
    EXPECT_EQ(config.remove_module_properties(areg::String("log"), areg::String("file"), areg::ConfigEntry::LogLayoutEnter), 0);
    EXPECT_EQ(config.module_properties().size(), 4u);

    EXPECT_EQ(config.remove_module_properties(areg::String("log"), areg::String("file"), areg::ConfigEntry::LogFileLocation), 1);
    EXPECT_EQ(config.module_properties().size(), 3u);
    EXPECT_EQ(config.remove_module_properties(areg::String("log"), areg::String("file")), 1);
    EXPECT_EQ(config.module_properties().size(), 2u);
}
