#ifndef AREG_TESTS_UNITS_APPLICATIONSETUPTESTSUPPORT_HPP
#define AREG_TESTS_UNITS_APPLICATIONSETUPTESTSUPPORT_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial licenses. See LICENSE.txt for details.
 * Copyright (c) 2017-2026 Aregtech (Artak Avetyan)
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/ThreadAddress.hpp"
#include "areg/persist/ConfigListener.hpp"

#include <atomic>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

namespace
{
    constexpr const char* APP_TIMER_THREAD{ "_AREG_TIMER_THREAD_NAME_" };
    constexpr const char* APP_WATCHDOG_THREAD{ "_AREG_WATCHDOG_THREAD_NAME_" };
    constexpr const char* APP_SERVICE_THREAD{ "_AREG_SERVICE_MANAGER_THREAD_" };
    constexpr const char* APP_LOG_THREAD{ "_AREG_LOGGING_THREAD_" };

    areg::Thread* application_thread(const char* name)
    {
        return areg::Thread::find_by_address(areg::ThreadAddress(name));
    }

    void expect_application_threads_stopped()
    {
        for (const char* name : { APP_TIMER_THREAD, APP_WATCHDOG_THREAD, APP_SERVICE_THREAD, APP_LOG_THREAD })
        {
            EXPECT_EQ(application_thread(name), nullptr) << name;
        }
    }

    class ApplicationSetupListener final : public areg::ConfigListener
    {
    public:
        ApplicationSetupListener()
            : mEvents()
            , mStates()
            , mAvailable()
            , mReady()
            , mManagers()
        {
        }

        void prepare_save_configuration(areg::ConfigManager&) final
        {
            record("prepare_save");
        }

        void post_save_configuration(areg::ConfigManager&) final
        {
            record("post_save");
        }

        void prepare_read_configuration(areg::ConfigManager&) final
        {
            record("prepare_read");
        }

        void post_read_configuration(areg::ConfigManager&) final
        {
            record("post_read");
        }

        void on_setup_configuration(const areg::ListProperties&, const areg::ListProperties&, areg::ConfigManager&) final
        {
            record("setup_defaults");
        }

        std::vector<std::string> mEvents;
        std::vector<areg::AppState> mStates;
        std::vector<bool> mAvailable;
        std::vector<bool> mReady;
        std::vector<areg::ConfigManager*> mManagers;

    private:
        void record(const char* event)
        {
            mEvents.emplace_back(event);
            mStates.push_back(areg::Application::system_state());
            mAvailable.push_back(areg::Application::is_servicing_available());
            mReady.push_back(areg::Application::is_servicing_ready());
            mManagers.push_back(&areg::Application::config_manager());
        }
    };

    class ApplicationSetupTest : public testing::Test
    {
    protected:
        [[maybe_unused]] ApplicationSetupTest()
            : mDirectory()
            , mOriginalDirectory()
            , mConfigFile()
            , mOwnsDirectory(false)
            , mNeedsRelease(false)
            , mListener()
        {
        }

        void SetUp() override
        {
            ASSERT_EQ(areg::Application::system_state(), areg::AppState::Stopped);
            expect_application_threads_stopped();
            areg::Application::config_manager().release_properties();

            std::error_code error;
            mOriginalDirectory = std::filesystem::current_path(error);
            ASSERT_FALSE(error) << error.message();
            const auto temp{ std::filesystem::absolute(testing::TempDir(), error) };
            ASSERT_FALSE(error) << error.message();
            static std::atomic<uint32_t> _sequence{ 0u };
            const std::string prefix{ "areg_application_setup_" + std::to_string(areg::Process::instance().id()) + "_" };
            for (uint32_t attempt = 0u; attempt < 64u && !mOwnsDirectory; ++attempt)
            {
                mDirectory = temp / (prefix + std::to_string(_sequence.fetch_add(1u)));
                mOwnsDirectory = std::filesystem::create_directory(mDirectory, error);
                ASSERT_FALSE(error) << error.message();
            }
            ASSERT_TRUE(mOwnsDirectory);
            mConfigFile = mDirectory / "shared.init";
            write_file(mConfigFile,
                  "config::*::version = 2.0.0\n"
                  "log::*::enable = true\n"
                  "log::*::enable::file = false\n"
                  "log::*::enable::remote = false\n"
                  "log::*::enable::output = false\n"
                  "log::*::enable::db = false\n"
                  "log::*::scope::* = NOTSET\n"
                  "service::*::list = router | logger\n"
                  "router::*::enable::tcpip = false\n"
                  "router::*::address::tcpip = 127.0.0.1\n"
                  "logger::*::enable::tcpip = false\n");
            std::filesystem::current_path(mDirectory, error);
            ASSERT_FALSE(error) << error.message();
        }

        void TearDown() override
        {
            if (mNeedsRelease)
            {
                release_application();
            }
            expect_application_threads_stopped();
            areg::Application::config_manager().release_properties();
            std::error_code error;
            if (!mOriginalDirectory.empty())
            {
                std::filesystem::current_path(mOriginalDirectory, error);
                EXPECT_FALSE(error) << error.message();
            }
            if (mOwnsDirectory)
            {
                const auto removed{ std::filesystem::remove_all(mDirectory, error) };
                EXPECT_FALSE(error) << error.message();
                EXPECT_GT(removed, 0u);
            }
        }

        [[maybe_unused]] void setup_application(bool logging = false, bool servicing = false, bool routing = false
                             , bool timer = false, bool watchdog = false)
        {
            mNeedsRelease = true;
            const std::string config{ mConfigFile.string() };
            areg::Application::setup(logging, servicing, routing, timer, watchdog, config.c_str(), &mListener);
        }

        void release_application()
        {
            areg::Application::release();
            mNeedsRelease = false;
            EXPECT_EQ(areg::Application::system_state(), areg::AppState::Stopped);
            EXPECT_FALSE(areg::Application::is_servicing_ready());
            EXPECT_FALSE(areg::Application::is_servicing_available());
            expect_application_threads_stopped();
        }

        void write_file(const std::filesystem::path& path, const std::string& content)
        {
            std::ofstream out(path, std::ios::binary | std::ios::trunc);
            ASSERT_TRUE(out.is_open()) << path;
            out << content;
            out.close();
            ASSERT_FALSE(out.fail()) << path;
        }

        [[maybe_unused]] std::string read_file(const std::filesystem::path& path)
        {
            std::ifstream input(path, std::ios::binary);
            EXPECT_TRUE(input.is_open()) << path;
            return std::string(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
        }

        std::filesystem::path mDirectory;
        std::filesystem::path mOriginalDirectory;
        std::filesystem::path mConfigFile;
        bool mOwnsDirectory;
        bool mNeedsRelease;
        ApplicationSetupListener mListener;
    };
}

#endif // AREG_TESTS_UNITS_APPLICATIONSETUPTESTSUPPORT_HPP
