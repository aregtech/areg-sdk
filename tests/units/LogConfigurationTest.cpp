/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial licenses. See LICENSE.txt for details.
 * Copyright (c) 2017-2026 Aregtech (Artak Avetyan)
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/base/File.hpp"
#include "areg/base/Process.hpp"
#include "areg/logging/LogConfiguration.hpp"
#include "areg/logging/LogScope.hpp"
#include "areg/logging/private/ScopeController.hpp"
#include "areg/persist/ConfigManager.hpp"
#include "areg/persist/Property.hpp"

#include <filesystem>
#include <string>
#include <vector>

namespace
{
    class LogConfigurationTest : public testing::Test
    {
    protected:
        LogConfigurationTest()
            : mManager(areg::Application::config_manager())
            , mWasConfigured(mManager.is_configured())
            , mReadonly(mManager.readonly_properties())
            , mWritable(mManager.module_properties())
            , mConfig()
            , mTempFile()
        {
        }

        void SetUp() override
        {
            mManager.set_configuration(areg::ListProperties(), areg::ListProperties());
        }

        void TearDown() override
        {
            if (mWasConfigured)
            {
                mManager.set_configuration(mReadonly, mWritable);
            }
            else
            {
                mManager.release_properties();
            }

            if (!mTempFile.empty())
            {
                std::error_code error;
                EXPECT_TRUE(std::filesystem::remove(mTempFile, error));
                EXPECT_FALSE(error) << error.message();
                EXPECT_TRUE(std::filesystem::remove(mTempFile.string() + ".lock", error));
                EXPECT_FALSE(error) << error.message();
                EXPECT_TRUE(std::filesystem::remove(mTempFile.parent_path(), error));
                EXPECT_FALSE(error) << error.message();
            }
        }

        areg::ConfigManager& mManager;
        const bool mWasConfigured;
        const areg::ListProperties mReadonly;
        const areg::ListProperties mWritable;
        areg::LogConfiguration mConfig;
        std::filesystem::path mTempFile;
    };

    areg::Property scope_property(const char* name, const char* priority)
    {
        const areg::String line{ areg::String("log::") + areg::Process::instance().app_name()
                                + "::scope::" + name + " = " + priority };
        areg::Property property;
        EXPECT_TRUE(property.parse(line));
        return property;
    }
}

// Each logging target can be switched on or off without changing the others.
TEST_F(LogConfigurationTest, each_target_can_be_enabled_and_disabled_independently)
{
    mConfig.set_status(true);
    using Query = bool (areg::LogConfiguration::*)() const noexcept;
    const struct
    {
        areg::LogTarget target;
        Query enabled;
    } targets[]
    {
          { areg::LogTarget::Remote, &areg::LogConfiguration::is_remote_logging_enabled }
        , { areg::LogTarget::File, &areg::LogConfiguration::is_file_logging_enabled }
        , { areg::LogTarget::Debug, &areg::LogConfiguration::is_debug_logging_enabled }
        , { areg::LogTarget::Database, &areg::LogConfiguration::is_db_logging_enabled }
    };

    for (const auto& target : targets)
    {
        mConfig.set_log_enabled(target.target, false);
    }
    mConfig.set_status(false);
    EXPECT_FALSE(mConfig.is_logging_enabled());
    EXPECT_FALSE(mConfig.status());
    for (const auto& target : targets)
    {
        mConfig.set_log_enabled(target.target, true);
        EXPECT_TRUE(mConfig.status());
        EXPECT_TRUE((mConfig.*target.enabled)());
        mConfig.set_log_enabled(target.target, true);
        EXPECT_TRUE((mConfig.*target.enabled)());
        for (const auto& other : targets)
        {
            if (other.target != target.target)
            {
                EXPECT_FALSE((mConfig.*other.enabled)());
            }
        }
        mConfig.set_log_enabled(target.target, false);
        mConfig.set_log_enabled(target.target, false);
        EXPECT_FALSE((mConfig.*target.enabled)());
    }
    mConfig.set_status(false);
    EXPECT_FALSE(mConfig.is_logging_enabled());
}

// Layout strings, file options, and stack size retain their configured values.
TEST_F(LogConfigurationTest, layouts_file_options_and_stack_size_round_trip)
{
    EXPECT_EQ(mConfig.version(), areg::Version(areg::String(areg::CONFIG_VERSION)));
    mConfig.set_layout_enter("enter:%z%n");
    mConfig.set_layout_message("[%p]%m%n");
    mConfig.set_layout_exit("exit:%z%n");
    EXPECT_EQ(mConfig.layout_enter(), "enter:%z%n");
    EXPECT_EQ(mConfig.layout_message(), "[%p]%m%n");
    EXPECT_EQ(mConfig.layout_exit(), "exit:%z%n");
    mConfig.set_layout_message("");
    EXPECT_TRUE(mConfig.layout_message().is_empty());
    for (uint32_t size : { 0u, 1u, 4096u })
    {
        mConfig.set_stack_size(size);
        EXPECT_EQ(mConfig.stack_size(), size);
    }
    for (bool append : { true, false })
    {
        mConfig.set_append_data(append);
        EXPECT_EQ(mConfig.append_data(), append);
    }
    mConfig.set_log_file("logs/custom_%time%.log");
    EXPECT_EQ(mConfig.log_file(), "logs/custom_%time%.log");
}

// Remote logging is enabled only when both the remote target and TCP switch are on.
TEST_F(LogConfigurationTest, remote_logging_requires_both_target_and_tcp_switch)
{
    mConfig.set_status(true);
    mConfig.set_log_enabled(areg::LogTarget::Remote, false);
    mConfig.set_tcp_enable(true);
    EXPECT_FALSE(mConfig.remote_tcp_enable());
    mConfig.set_log_enabled(areg::LogTarget::Remote, true);
    EXPECT_TRUE(mConfig.remote_tcp_enable());
    mConfig.set_tcp_enable(false);
    EXPECT_FALSE(mConfig.remote_tcp_enable());
    mConfig.set_tcp_enable(true, true);
    EXPECT_TRUE(mConfig.remote_tcp_enable());
    mConfig.set_tcp_address("127.0.0.2");
    mConfig.set_tcp_port(32123u);
    EXPECT_EQ(mConfig.remote_tcp_address(), "127.0.0.2");
    EXPECT_EQ(mConfig.remote_tcp_port(), 32123u);
    mConfig.set_tcp_address("127.0.0.3", true);
    mConfig.set_tcp_port(0u, true);
    EXPECT_EQ(mConfig.remote_tcp_address(), "127.0.0.3");
    EXPECT_EQ(mConfig.remote_tcp_port(), 0u);
}

// Database settings retain their paths, address, and credentials.
TEST_F(LogConfigurationTest, database_properties_paths_addresses_and_credentials_round_trip)
{
    mConfig.set_status(true);
    for (bool enabled : { false, true, false })
    {
        mConfig.set_database_enable(enabled);
        EXPECT_EQ(mConfig.database_enable(), enabled);
        EXPECT_EQ(mConfig.is_db_logging_enabled(), enabled);
    }
    mConfig.set_database_enable(true, true);
    EXPECT_TRUE(mConfig.database_enable());
    mConfig.set_database_engine("sqlite3");
    mConfig.set_database_name("rows.db");
    mConfig.set_database_location(testing::TempDir().c_str());
    mConfig.set_database_driver("driver");
    EXPECT_EQ(mConfig.database_engine(), "sqlite3");
    EXPECT_EQ(mConfig.database_name(), "rows.db");
    EXPECT_EQ(mConfig.database_location(), testing::TempDir().c_str());
    EXPECT_EQ(mConfig.database_driver(), "driver");
    const areg::String fullPath{ areg::File::make_full_path(mConfig.database_location(), "rows.db") };
    EXPECT_EQ(mConfig.database_full_path(), fullPath);
    mConfig.set_db_path(fullPath);
    EXPECT_EQ(mConfig.database_full_path(), fullPath);
    mConfig.set_database_address("127.0.0.1", 32124u);
    EXPECT_EQ(mConfig.database_address().host_address(), "127.0.0.1");
    EXPECT_EQ(mConfig.database_address().host_port(), 32124u);
    const areg::SocketAddress address{ "127.0.0.2", 32125u };
    mConfig.set_database_address(address, true);
    EXPECT_EQ(mConfig.database_address().host_address(), address.host_address());
    EXPECT_EQ(mConfig.database_address().host_port(), address.host_port());
    mConfig.set_database_user("reader", "local-test-value");
    EXPECT_EQ(mConfig.database_user().user(), "reader");
    EXPECT_EQ(mConfig.database_user().password(), "local-test-value");
    const areg::UserData user{ "writer", "another-test-value" };
    mConfig.set_database_user(user, true);
    EXPECT_EQ(mConfig.database_user().user(), user.user());
    EXPECT_EQ(mConfig.database_user().password(), user.password());
    mConfig.set_database_engine("temporary-engine", true);
    mConfig.set_database_name("temporary.db", true);
    mConfig.set_database_location("temporary-location", true);
    mConfig.set_database_driver("temporary-driver", true);
    EXPECT_EQ(mConfig.database_engine(), "temporary-engine");
    EXPECT_EQ(mConfig.database_name(), "temporary.db");
    EXPECT_EQ(mConfig.database_location(), "temporary-location");
    EXPECT_EQ(mConfig.database_driver(), "temporary-driver");
    mConfig.set_db_path(fullPath, true);
    EXPECT_EQ(mConfig.database_full_path(), fullPath);
}

// Replacing module scope settings removes the previous entries.
TEST_F(LogConfigurationTest, module_scopes_replace_previous_entries)
{
    const std::vector<areg::Property> initial
    {
          scope_property("config_first.method", "INFO")
        , scope_property("config_second.*", "WARN")
    };
    mConfig.set_module_scopes(initial);
    std::vector<areg::Property> scopes;
    ASSERT_EQ(mConfig.module_scopes(scopes), 2u);
    EXPECT_EQ(scopes[0].key().position(), initial[0].key().position());
    EXPECT_EQ(scopes[0].value_string(), "INFO");
    EXPECT_EQ(scopes[1].key().position(), initial[1].key().position());
    EXPECT_EQ(scopes[1].value_string(), "WARN");
    mConfig.set_module_scopes({ initial[1] });
    ASSERT_EQ(mConfig.module_scopes(scopes), 1u);
    EXPECT_EQ(scopes[0].key().position(), "config_second.*");
    mConfig.set_module_scopes({});
    EXPECT_EQ(mConfig.module_scopes(scopes), 0u);
    EXPECT_TRUE(scopes.empty());
}

// Enabling named scopes stores both scope and debug priority flags.
TEST_F(LogConfigurationTest, enabling_named_scopes_stores_scope_and_debug_flags)
{
    mConfig.enable_scopes({ "configuration_one.method", "configuration_two.*" });
    std::vector<areg::Property> scopes;
    ASSERT_EQ(mConfig.module_scopes(scopes), 2u);
    for (const auto& scope : scopes)
    {
        EXPECT_EQ(scope.value_string(), "SCOPE|DEBUG");
    }
    mConfig.enable_scopes({}, true);
    EXPECT_EQ(mConfig.module_scopes(scopes), 2u);
}

// Reloading a saved configuration restores persistent values but not temporary overrides.
TEST_F(LogConfigurationTest, saved_configuration_reloads_persistent_but_not_temporary_values)
{
    const std::string directory{ "logging_configuration_" + std::to_string(areg::Process::instance().id()) };
    const std::filesystem::path path{ std::filesystem::path(testing::TempDir()) / directory };
    std::error_code error;
    ASSERT_TRUE(std::filesystem::create_directory(path, error)) << error.message();
    mTempFile = path / "shared.init";
    mConfig.set_status(true);
    mConfig.set_layout_message("%p:%m%n");
    mConfig.set_log_file("saved.log");
    mConfig.set_append_data(false);
    mConfig.set_stack_size(345u);
    mConfig.set_tcp_address("127.0.0.2");
    mConfig.set_tcp_port(32126u);
    mConfig.set_database_engine("sqlite3");
    mConfig.set_tcp_address("127.0.0.3", true);
    ASSERT_TRUE(mManager.save_config(areg::String(mTempFile.string())));
    areg::ConfigManager loaded;
    ASSERT_TRUE(loaded.read_config(areg::String(mTempFile.string())));
    mManager.set_configuration(loaded.readonly_properties(), loaded.module_properties());
    EXPECT_TRUE(mConfig.status());
    EXPECT_EQ(mConfig.layout_message(), "%p:%m%n");
    EXPECT_EQ(mConfig.log_file(), "saved.log");
    EXPECT_FALSE(mConfig.append_data());
    EXPECT_EQ(mConfig.stack_size(), 345u);
    EXPECT_EQ(mConfig.remote_tcp_address(), "127.0.0.2");
    EXPECT_EQ(mConfig.remote_tcp_port(), 32126u);
    EXPECT_EQ(mConfig.database_engine(), "sqlite3");
}

#if AREG_LOGGING
// Saving registered scopes replaces the previous scope configuration.
TEST_F(LogConfigurationTest, scope_configuration_is_replaced_with_registered_scope_state)
{
    mConfig.set_module_scopes({ scope_property("obsolete", "DEBUG") });
    areg::LogScope first{ "configuration.first", areg::LogPriority::PrioInfo };
    areg::LogScope second{ "configuration.second", areg::LogPriority::PrioFatal };
    areg::ScopeController controller;
    controller.register_scope(first);
    controller.register_scope(second);
    mConfig.update_scope_configuration(controller);
    std::vector<areg::Property> scopes;
    ASSERT_EQ(mConfig.module_scopes(scopes), 2u);
    EXPECT_EQ(scopes[0].key().position(), first.name().data());
    EXPECT_EQ(scopes[0].value_string(), "INFO");
    EXPECT_EQ(scopes[1].key().position(), second.name().data());
    EXPECT_EQ(scopes[1].value_string(), "FATAL");
}
#endif

// Disabling scopes must store NOTSET rather than enabling fatal messages.
TEST_F(LogConfigurationTest, disabling_scopes_stores_notset_instead_of_fatal)
{
    mConfig.enable_scopes({ "configuration_disabled.method" }, false);
    std::vector<areg::Property> scopes;
    ASSERT_EQ(mConfig.module_scopes(scopes), 1u);
    EXPECT_EQ(scopes[0].value_string(), "NOTSET");
}

// A target must remain switchable while the master logging switch is off.
TEST_F(LogConfigurationTest, target_can_be_disabled_while_master_logging_is_off)
{
    mConfig.set_log_enabled(areg::LogTarget::File, true);
    mConfig.set_status(false);
    mConfig.set_log_enabled(areg::LogTarget::File, false);
    mConfig.set_status(true);
    EXPECT_FALSE(mConfig.is_file_logging_enabled());
}
// Disabling a target that is already off while master logging is on keeps it off.
TEST_F(LogConfigurationTest, target_disable_with_master_on_keeps_master_on)
{
    mConfig.set_status(true);
    mConfig.set_log_enabled(areg::LogTarget::File, false);
    EXPECT_FALSE(mConfig.is_file_logging_enabled());
    EXPECT_TRUE(mConfig.status());
    mConfig.set_log_enabled(areg::LogTarget::File, true);
    EXPECT_TRUE(mConfig.is_file_logging_enabled());
}
