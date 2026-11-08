/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial licenses. See LICENSE.txt for details.
 * Copyright (c) 2017-2026 Aregtech (Artak Avetyan)
 ************************************************************************/
#include "units/ApplicationSetupTestSupport.hpp"

#if defined(POSIX) || defined(_POSIX)

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <string>

#include <signal.h>
#include <spawn.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

namespace
{
    volatile sig_atomic_t _application_signal_calls{ 0 };

    void application_pipe_handler(int)
    {
        _application_signal_calls = 1;
    }

    void application_pipe_info_handler(int, siginfo_t*, void*)
    {
        _application_signal_calls = 1;
    }

    class ApplicationSignalGuard
    {
    public:
        explicit ApplicationSignalGuard(int signal)
            : mSignal(signal)
            , mOriginal()
            , mValid(::sigaction(mSignal, nullptr, &mOriginal) == 0)
        {
        }

        ~ApplicationSignalGuard()
        {
            if (areg::Application::system_state() != areg::AppState::Stopped)
            {
                areg::Application::release();
            }
            if (mValid)
            {
                EXPECT_EQ(::sigaction(mSignal, &mOriginal, nullptr), 0);
            }
        }

        bool is_valid() const
        {
            return mValid;
        }

    private:
        const int mSignal;
        struct sigaction mOriginal;
        const bool mValid;
    };

    void expect_same_disposition(int owner, const struct sigaction& actual, const struct sigaction& expected)
    {
        EXPECT_EQ(actual.sa_flags & (SA_SIGINFO | SA_RESTART), expected.sa_flags & (SA_SIGINFO | SA_RESTART));
        if ((expected.sa_flags & SA_SIGINFO) != 0)
        {
            EXPECT_EQ(actual.sa_sigaction, expected.sa_sigaction);
        }
        else
        {
            EXPECT_EQ(actual.sa_handler, expected.sa_handler);
        }
        for (int signal : { SIGPIPE, SIGSEGV, SIGUSR1 })
        {
            // Without SA_NODEFER the signal blocks itself, and a system may store it in the mask.
            if ((signal == owner) && ((expected.sa_flags & SA_NODEFER) == 0))
            {
                continue;
            }
            EXPECT_EQ(sigismember(&actual.sa_mask, signal), sigismember(&expected.sa_mask, signal)) << "mask bit of signal " << signal;
        }
    }

    class ApplicationSetupSignalsTest : public ApplicationSetupTest, public testing::WithParamInterface<uint32_t>
    {
    };

    class ApplicationSetupPosixTest : public ApplicationSetupTest
    {
    };

    std::string application_signal_mode_name(const testing::TestParamInfo<uint32_t>& info)
    {
        const char* names[]{ "Default", "Ignored", "HostHandler", "HostSiginfoHandler" };
        return names[info.param];
    }

    class ApplicationEnvironmentGuard
    {
    public:
        explicit ApplicationEnvironmentGuard(const char* name)
            : mName(name)
            , mExisted(std::getenv(name) != nullptr)
            , mValue(mExisted ? std::getenv(name) : "")
        {
        }

        ~ApplicationEnvironmentGuard()
        {
            EXPECT_EQ(mExisted ? ::setenv(mName.c_str(), mValue.c_str(), 1) : ::unsetenv(mName.c_str()), 0);
        }

    private:
        const std::string mName;
        const bool mExisted;
        const std::string mValue;
    };

    class ApplicationSetupLocalServiceTest : public ApplicationSetupTest, public testing::WithParamInterface<bool>
    {
    protected:
        //!< The executable base name of the service under test.
        std::string service_base() const
        {
            return std::string(GetParam() ? areg::DEFAULT_LOGGER_SERVICE_NAME : areg::DEFAULT_ROUTER_SERVICE_NAME);
        }

        //!< The arguments the service manager receives, one per line.
        std::string expected_arguments() const
        {
#ifdef __APPLE__
            return "kickstart\nsystem/tech.areg." + service_base() + "\n";
#else   // __APPLE__
            return "--no-ask-password\nstart\n" + service_base() + ".service\n";
#endif  // __APPLE__
        }

        //!< True if the real service process runs on this host; the call then starts nothing.
        bool host_service_runs() const
        {
            const std::string base{ service_base() };
            FILE* const list{ ::popen("ps -A -o comm=", "r") };
            if (list == nullptr)
                return false;

            bool result{ false };
            char line[512]{ };
            while ((result == false) && (std::fgets(line, sizeof(line), list) != nullptr))
            {
                std::string name{ line };
                name.erase(name.find_last_not_of(" \r\n") + 1u);
                const std::string::size_type slash{ name.rfind('/') };
                name = (slash == std::string::npos) ? name : name.substr(slash + 1u);
                result = (name.compare(0u, base.size(), base) == 0)
                      && ((name.size() == base.size()) || (name[base.size()] == '.'));
            }

            ::pclose(list);
            return result;
        }

        //!< Calls the start method with only a recording service manager on PATH, or none.
        void run_service_call(bool withTool, uint32_t exitStatus, bool& result, std::string& arguments)
        {
            const auto capture{ mDirectory / "service.arguments" };
            if (withTool)
            {
                for (const char* tool : { "systemctl", "launchctl" })
                {
                    const auto script{ mDirectory / tool };
                    write_file(script, "#!/bin/sh\nprintf '%s\\n' \"$@\" > \"$AREG_APPLICATION_TEST_CAPTURE\"\nexit "
                                      + std::to_string(exitStatus) + "\n");
                    ASSERT_FALSE(HasFatalFailure());
                    std::error_code error;
                    std::filesystem::permissions(script, std::filesystem::perms::owner_all, std::filesystem::perm_options::replace, error);
                    ASSERT_FALSE(error) << error.message();
                }
            }

            ApplicationEnvironmentGuard path("PATH");
            ApplicationEnvironmentGuard output("AREG_APPLICATION_TEST_CAPTURE");
            ASSERT_EQ(::setenv("PATH", mDirectory.c_str(), 1), 0);
            ASSERT_EQ(::setenv("AREG_APPLICATION_TEST_CAPTURE", capture.c_str(), 1), 0);
            result = GetParam() ? areg::Application::start_logging_service() : areg::Application::start_router_service();
            std::error_code error;
            arguments = std::filesystem::exists(capture, error) ? read_file(capture) : std::string{ };
            ASSERT_FALSE(error) << error.message();
        }
    };
}

TEST_P(ApplicationSetupSignalsTest, setup_respects_signal_ownership_and_release_restores_the_original_disposition)
{
    ApplicationSignalGuard guard(SIGPIPE);
    ASSERT_TRUE(guard.is_valid());
    struct sigaction wanted{};
    sigemptyset(&wanted.sa_mask);
    ASSERT_EQ(sigaddset(&wanted.sa_mask, SIGUSR1), 0);
    const uint32_t mode{ GetParam() };
    wanted.sa_handler = mode == 0u ? SIG_DFL : mode == 1u ? SIG_IGN : &application_pipe_handler;
    if (mode == 3u)
    {
        wanted.sa_flags = SA_SIGINFO;
        wanted.sa_sigaction = &application_pipe_info_handler;
    }
    ASSERT_EQ(::sigaction(SIGPIPE, &wanted, nullptr), 0);
    struct sigaction before{};
    struct sigaction segvBefore{};
    ASSERT_EQ(::sigaction(SIGPIPE, nullptr, &before), 0);
    ASSERT_EQ(::sigaction(SIGSEGV, nullptr, &segvBefore), 0);

    setup_application(false, false, false, true, false);
    struct sigaction installed{};
    ASSERT_EQ(::sigaction(SIGPIPE, nullptr, &installed), 0);
    if (mode < 2u)
    {
        EXPECT_NE(installed.sa_handler, SIG_DFL);
        EXPECT_NE(installed.sa_handler, SIG_IGN);
        EXPECT_NE(installed.sa_flags & SA_RESTART, 0);
    }
    else
    {
        expect_same_disposition(SIGPIPE, installed, before);
    }
    _application_signal_calls = 0;
    ASSERT_EQ(::raise(SIGPIPE), 0);
    EXPECT_EQ(_application_signal_calls, mode < 2u ? 0 : 1);
    EXPECT_EQ(areg::Application::system_state(), areg::AppState::Ready);

    struct sigaction segvInstalled{};
    ASSERT_EQ(::sigaction(SIGSEGV, nullptr, &segvInstalled), 0);
    const bool segvOwned{ (segvBefore.sa_flags & SA_SIGINFO) != 0
                       || (segvBefore.sa_handler != SIG_DFL && segvBefore.sa_handler != SIG_IGN) };
    if (segvOwned)
    {
        expect_same_disposition(SIGSEGV, segvInstalled, segvBefore);
    }

    release_application();
    struct sigaction restored{};
    struct sigaction segvRestored{};
    ASSERT_EQ(::sigaction(SIGPIPE, nullptr, &restored), 0);
    ASSERT_EQ(::sigaction(SIGSEGV, nullptr, &segvRestored), 0);
    expect_same_disposition(SIGPIPE, restored, before);
    expect_same_disposition(SIGSEGV, segvRestored, segvBefore);
    release_application();
    ASSERT_EQ(::sigaction(SIGPIPE, nullptr, &restored), 0);
    expect_same_disposition(SIGPIPE, restored, before);
}

INSTANTIATE_TEST_SUITE_P(PipeOwnership, ApplicationSetupSignalsTest, testing::Values(0u, 1u, 2u, 3u)
    , application_signal_mode_name);

TEST_F(ApplicationSetupPosixTest, repeated_setup_restores_the_original_signal_instead_of_its_own_handler)
{
    ApplicationSignalGuard guard(SIGPIPE);
    ASSERT_TRUE(guard.is_valid());
    struct sigaction ignored{};
    sigemptyset(&ignored.sa_mask);
    ignored.sa_handler = SIG_IGN;
    ASSERT_EQ(::sigaction(SIGPIPE, &ignored, nullptr), 0);
    setup_application(false, false, false, true, false);
    struct sigaction first{};
    ASSERT_EQ(::sigaction(SIGPIPE, nullptr, &first), 0);
    setup_application(false, false, false, true, false);
    struct sigaction second{};
    ASSERT_EQ(::sigaction(SIGPIPE, nullptr, &second), 0);
    expect_same_disposition(SIGPIPE, second, first);
    release_application();
    struct sigaction restored{};
    ASSERT_EQ(::sigaction(SIGPIPE, nullptr, &restored), 0);
    EXPECT_EQ(restored.sa_handler, SIG_IGN);
}

TEST_P(ApplicationSetupLocalServiceTest, service_manager_starts_the_installed_service)
{
    if (host_service_runs())
    {
        GTEST_SKIP() << "The " << service_base() << " process runs on this host";
    }

    bool result{ false };
    std::string arguments;
    run_service_call(true, 0u, result, arguments);
    ASSERT_FALSE(HasFatalFailure());
    EXPECT_TRUE(result);
    EXPECT_EQ(arguments, expected_arguments());
}

TEST_P(ApplicationSetupLocalServiceTest, failed_service_manager_command_returns_false)
{
    if (host_service_runs())
    {
        GTEST_SKIP() << "The " << service_base() << " process runs on this host";
    }

    bool result{ true };
    std::string arguments;
    run_service_call(true, 7u, result, arguments);
    ASSERT_FALSE(HasFatalFailure());
    EXPECT_FALSE(result);
    EXPECT_EQ(arguments, expected_arguments());
}

TEST_P(ApplicationSetupLocalServiceTest, missing_service_manager_returns_false)
{
    if (host_service_runs())
    {
        GTEST_SKIP() << "The " << service_base() << " process runs on this host";
    }

    bool result{ true };
    std::string arguments;
    run_service_call(false, 0u, result, arguments);
    ASSERT_FALSE(HasFatalFailure());
    EXPECT_FALSE(result);
    EXPECT_TRUE(arguments.empty());
}

TEST_P(ApplicationSetupLocalServiceTest, running_service_process_is_not_started_again)
{
    if (host_service_runs())
    {
        GTEST_SKIP() << "The " << service_base() << " process runs on this host";
    }

    const auto executable{ mDirectory / (service_base() + ".elf") };
    std::error_code error;
#if defined(__linux__)
    // A shell runs under any name, writes a line once it runs, and waits for a line of its standard input.
    ASSERT_TRUE(std::filesystem::copy_file("/bin/sh", executable, error)) << error.message();
    const std::string program{ executable.string() };
    char flag[]{ "-c" };
    char command[]{ "echo ready; read line" };
    char* const argv[]{ const_cast<char*>(program.c_str()), flag, command, nullptr };
#else   // defined(__linux__)
    ASSERT_TRUE(std::filesystem::copy_file("/bin/sleep", executable, error)) << error.message();
    const std::string program{ executable.string() };
    char duration[]{ "60" };
    char* const argv[]{ const_cast<char*>(program.c_str()), duration, nullptr };
#endif  // defined(__linux__)

    int input[2]{ -1, -1 };
    int output[2]{ -1, -1 };
    ASSERT_EQ(::pipe(input), 0);
    ASSERT_EQ(::pipe(output), 0);
    posix_spawn_file_actions_t actions;
    ASSERT_EQ(::posix_spawn_file_actions_init(&actions), 0);
    ASSERT_EQ(::posix_spawn_file_actions_adddup2(&actions, input[0], STDIN_FILENO), 0);
    ASSERT_EQ(::posix_spawn_file_actions_adddup2(&actions, output[1], STDOUT_FILENO), 0);
    pid_t child{ -1 };
    const int spawned{ ::posix_spawn(&child, program.c_str(), &actions, nullptr, argv, nullptr) };
    ::posix_spawn_file_actions_destroy(&actions);
    ASSERT_EQ(spawned, 0);
    ::close(output[1]);
#if defined(__linux__)
    char ready{ 0 };
    ASSERT_EQ(::read(output[0], &ready, 1u), 1);
#endif  // defined(__linux__)

    bool result{ false };
    std::string arguments;
    run_service_call(true, 7u, result, arguments);
    EXPECT_EQ(::kill(child, SIGKILL), 0);
    int status{ 0 };
    EXPECT_EQ(::waitpid(child, &status, 0), child);
    ::close(input[0]);
    ::close(input[1]);
    ::close(output[0]);
    ASSERT_FALSE(HasFatalFailure());
    EXPECT_TRUE(result);
    EXPECT_TRUE(arguments.empty());
}

INSTANTIATE_TEST_SUITE_P(ServiceKind, ApplicationSetupLocalServiceTest, testing::Values(false, true)
    , [](const testing::TestParamInfo<bool>& info) { return info.param ? "Logger" : "Router"; });

#endif // defined(POSIX) || defined(_POSIX)
