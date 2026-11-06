/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/SystemServiceBaseTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests of the system service base: command line options,
 *              dispatching of the options, the order of the service main and the controls.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/appbase/AppDefs.hpp"
#include "areg/base/String.hpp"
#include "areg/ipc/RemoteServiceDefs.hpp"
#include "aregextend/service/SystemServiceBase.hpp"

#include <cstdio>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

namespace
{
    using areg::ext::ServiceOption;
    using areg::ext::ServicePhase;

    //!< The communication object that never starts a thread or a socket.
    class TestComm : public areg::ext::ServiceCommunicationBase
    {
    public:
        TestComm()
            : areg::ext::ServiceCommunicationBase   ( areg::COOKIE_ROUTER
                                                    , areg::RemoteServiceKind::Router
                                                    , static_cast<uint32_t>(areg::ConnectionType::Tcpip)
                                                    , areg::SYSTEM_THREAD_STACK_BIG
                                                    , areg::String("SystemServiceBaseTestDispatcher") )
        {
        }

        void on_service_channel_connected(const areg::Channel &) override  { }
        void on_service_channel_disconnected(const areg::Channel &) override { }
        void on_service_channel_lost(const areg::Channel &) override { }
        void failed_process_message(const areg::MessageEnvelope &) override { }
        void on_connection_started() override { }
        void on_connection_stopped() override { }
        void on_connection_lost() override { }
        void on_message_received(const areg::MessageEnvelope &) override { }
    };

    //!< The service that records every call of the base.
    class FakeService : public areg::ext::SystemServiceBase
    {
    public:
        explicit FakeService(areg::ext::ServiceCommunicationBase & comm)
            : areg::ext::SystemServiceBase(comm)
        {
        }

        bool    initializeResult    { true };
        bool    registerResult      { true };
        int32_t helpCount           { 0 };
        bool    helpOnCmdLine       { false };

        std::vector<std::string>    calls;

        const char * file_config() const { return mFileConfig.as_string(); }
        void set_file_config(const char * path) { mFileConfig = areg::String(path); }

        bool service_initialize(ServiceOption, const char *, const char *) override
        {
            calls.push_back("initialize");
            return initializeResult;
        }

        void service_release() override { calls.push_back("release"); }
        bool service_install() override { calls.push_back("install"); return true; }
        void service_uninstall() override { calls.push_back("uninstall"); }
        bool register_service() override { calls.push_back("register"); return registerResult; }
        bool service_open() override { calls.push_back("open"); return true; }
        bool service_start() override { calls.push_back("start"); return true; }
        void service_pause() override { calls.push_back("pause"); }
        bool service_continue() override { calls.push_back("continue"); return true; }
        void service_stop() override { calls.push_back("stop"); }
        void service_shutdown() override { calls.push_back("shutdown"); }
        int32_t start_service_dispatcher() override { return RESULT_IGNORED; }

        bool set_state(ServicePhase newState) override
        {
            calls.push_back(std::string("state:") + areg::ext::as_string(newState));
            mSystemServiceState = newState;
            return true;
        }

        void print_help(bool isCmdLine) override
        {
            ++helpCount;
            helpOnCmdLine = isCmdLine;
        }

        areg::ext::Console::CallBack option_check_callback() const override { return nullptr; }
        void run_console_io() override { calls.push_back("console"); }
        void run_service() override { calls.push_back("run"); }
    };

    //!< The communication object and the service that uses it.
    struct Harness
    {
        TestComm    comm;
        FakeService service{ comm };
    };

    //!< Names a state in the list of calls.
    std::string state_call(ServicePhase phase)
    {
        return std::string("state:") + areg::ext::as_string(phase);
    }

    //!< Writes a small file and removes it on destruction.
    class TempFile
    {
    public:
        explicit TempFile(const char * name)
            : mPath(testing::TempDir() + name)
        {
            std::ofstream out(mPath.c_str());
            out << "x\n";
        }

        ~TempFile() { std::remove(mPath.c_str()); }

        const char * path() const { return mPath.c_str(); }

    private:
        std::string mPath;
    };

    constexpr uint32_t SETUP_COUNT{ static_cast<uint32_t>(std::size(areg::ext::ServiceOptionSetup)) };
}

TEST(SystemServiceBaseTest, a_new_service_is_stopped_in_console_mode)
{
    Harness h;
    EXPECT_EQ(h.service.state(), ServicePhase::Stopped);
    EXPECT_EQ(h.service.current_option(), areg::ext::DEFAULT_OPTION);
    EXPECT_EQ(&h.service.communication_controller(), &h.comm);
    EXPECT_EQ(&h.service.data_rate_helper(), &h.comm.data_rate_helper());
}

TEST(SystemServiceBaseTest, the_current_option_can_be_changed)
{
    Harness h;
    h.service.set_current_option(ServiceOption::CMD_Service);
    EXPECT_EQ(h.service.current_option(), ServiceOption::CMD_Service);
}

TEST(SystemServiceBaseTest, program_name_only_restores_the_defaults)
{
    Harness h;
    char program[] = "service";
    char * argv[] = { program };
    h.service.set_current_option(ServiceOption::CMD_Service);
    h.service.set_file_config("./other.init");
    h.comm.enable_data_rate(false);

    EXPECT_TRUE(h.service.parse_options(1, argv, areg::ext::ServiceOptionSetup, SETUP_COUNT));

    EXPECT_EQ(h.service.current_option(), areg::ext::DEFAULT_OPTION);
    EXPECT_STREQ(h.service.file_config(), areg::DEFAULT_CONFIG_FILE.data());
    EXPECT_TRUE(h.comm.is_data_rate_enabled());
    EXPECT_EQ(h.service.helpCount, 0);
}

TEST(SystemServiceBaseTest, no_arguments_fail_and_change_nothing)
{
    Harness h;
    h.service.set_current_option(ServiceOption::CMD_Service);

    EXPECT_FALSE(h.service.parse_options(0, static_cast<char **>(nullptr), areg::ext::ServiceOptionSetup, SETUP_COUNT));
    EXPECT_FALSE(h.service.parse_options(0, static_cast<const char **>(nullptr), areg::ext::ServiceOptionSetup, SETUP_COUNT));

    EXPECT_EQ(h.service.current_option(), ServiceOption::CMD_Service);
    EXPECT_EQ(h.service.helpCount, 0);
}

TEST(SystemServiceBaseTest, run_mode_options_select_the_current_option)
{
    struct Row
    {
        const char *    arg;
        ServiceOption   expected;
    };

    static const Row rows[] =
    {
          { "-c", ServiceOption::CMD_Console }
        , { "--console", ServiceOption::CMD_Console }
        , { "-i", ServiceOption::CMD_Install }
        , { "--install", ServiceOption::CMD_Install }
        , { "-u", ServiceOption::CMD_Uninstall }
        , { "--uninstall", ServiceOption::CMD_Uninstall }
        , { "-s", ServiceOption::CMD_Service }
        , { "--service", ServiceOption::CMD_Service }
    };

    for (const Row & row : rows)
    {
        Harness h;
        h.service.set_current_option(ServiceOption::CMD_Custom);
        char program[] = "service";
        std::string arg(row.arg);
        char * argv[] = { program, &arg[0] };

        EXPECT_TRUE(h.service.parse_options(2, argv, areg::ext::ServiceOptionSetup, SETUP_COUNT)) << row.arg;
        EXPECT_EQ(h.service.current_option(), row.expected) << row.arg;
        EXPECT_EQ(h.service.helpCount, 0) << row.arg;
    }
}

TEST(SystemServiceBaseTest, the_constant_argument_list_is_parsed_like_the_mutable_one)
{
    Harness h;
    const char * argv[] = { "service", "--service" };

    EXPECT_TRUE(h.service.parse_options(2, argv, areg::ext::ServiceOptionSetup, SETUP_COUNT));
    EXPECT_EQ(h.service.current_option(), ServiceOption::CMD_Service);

    const char * program[] = { "service" };
    h.comm.enable_data_rate(false);
    EXPECT_TRUE(h.service.parse_options(1, program, areg::ext::ServiceOptionSetup, SETUP_COUNT));
    EXPECT_EQ(h.service.current_option(), areg::ext::DEFAULT_OPTION);
    EXPECT_TRUE(h.comm.is_data_rate_enabled());
}

TEST(SystemServiceBaseTest, help_option_prints_help_and_fails)
{
    Harness h;
    char program[] = "service";
    char option[] = "--help";
    char * argv[] = { program, option };

    EXPECT_FALSE(h.service.parse_options(2, argv, areg::ext::ServiceOptionSetup, SETUP_COUNT));
    EXPECT_EQ(h.service.helpCount, 1);
    EXPECT_TRUE(h.service.helpOnCmdLine);
}

TEST(SystemServiceBaseTest, unknown_option_without_default_setup_prints_help_and_fails)
{
    const areg::ext::OptionParser::OptionSetup setup[]
    {
        { "-c", "--console", static_cast<int32_t>(ServiceOption::CMD_Console), areg::ext::OptionParser::NO_DATA, {}, {}, {} }
    };

    Harness h;
    char program[] = "service";
    char option[] = "--bogus";
    char * argv[] = { program, option };
    const char * constArgv[] = { "service", "--bogus" };

    EXPECT_FALSE(h.service.parse_options(2, argv, setup, static_cast<uint32_t>(std::size(setup))));
    EXPECT_EQ(h.service.helpCount, 1);
    EXPECT_FALSE(h.service.parse_options(2, constArgv, setup, static_cast<uint32_t>(std::size(setup))));
    EXPECT_EQ(h.service.helpCount, 2);
    EXPECT_TRUE(h.service.helpOnCmdLine);
}

TEST(SystemServiceBaseTest, verbose_option_enables_data_rate_in_console_mode)
{
    Harness h;
    h.comm.enable_data_rate(false);
    h.service.set_current_option(ServiceOption::CMD_Service);
    char program[] = "service";
    char option[] = "--verbose";
    char * argv[] = { program, option };

    EXPECT_FALSE(h.comm.is_data_rate_enabled());
    EXPECT_TRUE(h.service.parse_options(2, argv, areg::ext::ServiceOptionSetup, SETUP_COUNT));
    EXPECT_TRUE(h.comm.is_data_rate_enabled());
    EXPECT_EQ(h.service.current_option(), ServiceOption::CMD_Console);
}

TEST(SystemServiceBaseTest, load_option_takes_an_existing_file)
{
    TempFile file("SystemServiceBaseTest_load.init");
    Harness h;
    char program[] = "service";
    char option[] = "--load";
    std::string path(file.path());
    char * argv[] = { program, option, &path[0] };

    EXPECT_TRUE(h.service.parse_options(3, argv, areg::ext::ServiceOptionSetup, SETUP_COUNT));
    EXPECT_STREQ(h.service.file_config(), file.path());
    EXPECT_EQ(h.service.helpCount, 0);
}

TEST(SystemServiceBaseTest, load_option_rejects_a_missing_file)
{
    Harness h;
    const std::string missing = testing::TempDir() + "SystemServiceBaseTest_missing.init";
    std::remove(missing.c_str());
    char program[] = "service";
    char option[] = "-l";
    std::string path(missing);
    char * argv[] = { program, option, &path[0] };

    EXPECT_FALSE(h.service.parse_options(3, argv, areg::ext::ServiceOptionSetup, SETUP_COUNT));
    EXPECT_STREQ(h.service.file_config(), areg::DEFAULT_CONFIG_FILE.data());
}

TEST(SystemServiceBaseTest, one_failing_option_fails_the_whole_line)
{
    Harness h;
    const std::string missing = testing::TempDir() + "SystemServiceBaseTest_missing2.init";
    std::remove(missing.c_str());
    char program[] = "service";
    char verbose[] = "--verbose";
    char load[] = "--load";
    std::string path(missing);
    char * argv[] = { program, verbose, load, &path[0] };

    EXPECT_FALSE(h.service.parse_options(4, argv, areg::ext::ServiceOptionSetup, SETUP_COUNT));
}

TEST(SystemServiceBaseTest, an_empty_option_list_is_accepted)
{
    Harness h;
    areg::ext::OptionParser::InputOptionList none;
    EXPECT_TRUE(h.service.prepare_options(none));
}

TEST(SystemServiceBaseTest, undefined_and_custom_commands_reset_the_option_and_print_help)
{
    const ServiceOption commands[] = { ServiceOption::CMD_Undefined, ServiceOption::CMD_Custom };
    for (ServiceOption command : commands)
    {
        Harness h;
        h.service.set_current_option(ServiceOption::CMD_Service);
        areg::ext::OptionParser::InputOption opt;
        opt.inCommand = static_cast<int32_t>(command);

        EXPECT_FALSE(h.service.dispatch_option(opt));
        EXPECT_EQ(h.service.current_option(), ServiceOption::CMD_Undefined);
        EXPECT_EQ(h.service.helpCount, 1);
        EXPECT_TRUE(h.service.helpOnCmdLine);
    }
}

TEST(SystemServiceBaseTest, a_command_out_of_range_is_treated_as_undefined)
{
    Harness h;
    areg::ext::OptionParser::InputOption opt;
    opt.inCommand = 1000;

    EXPECT_FALSE(h.service.dispatch_option(opt));
    EXPECT_EQ(h.service.current_option(), ServiceOption::CMD_Undefined);
    EXPECT_EQ(h.service.helpCount, 1);
}

TEST(SystemServiceBaseTest, reset_restores_option_file_and_data_rate)
{
    Harness h;
    h.service.set_current_option(ServiceOption::CMD_Service);
    h.service.set_file_config("./other.init");
    h.comm.enable_data_rate(false);

    h.service.reset();

    EXPECT_EQ(h.service.current_option(), areg::ext::DEFAULT_OPTION);
    EXPECT_STREQ(h.service.file_config(), areg::DEFAULT_CONFIG_FILE.data());
    EXPECT_TRUE(h.comm.is_data_rate_enabled());
}

TEST(SystemServiceBaseTest, console_main_runs_the_console_between_start_and_stop)
{
    Harness h;
    h.service.set_current_option(ServiceOption::CMD_Console);

    EXPECT_EQ(h.service.service_main(ServiceOption::CMD_Console, nullptr), areg::ext::SystemServiceBase::RESULT_SUCCEEDED);

    const std::vector<std::string> expected
    {
          "initialize"
        , state_call(ServicePhase::Starting)
        , "register"
        , "start"
        , "console"
        , "stop"
        , "release"
        , state_call(ServicePhase::Stopped)
    };
    EXPECT_EQ(h.service.calls, expected);
    EXPECT_EQ(h.service.state(), ServicePhase::Stopped);
}

TEST(SystemServiceBaseTest, service_main_runs_the_service_when_registered)
{
    Harness h;
    h.service.set_current_option(ServiceOption::CMD_Service);

    EXPECT_EQ(h.service.service_main(ServiceOption::CMD_Service, nullptr), areg::ext::SystemServiceBase::RESULT_SUCCEEDED);

    const std::vector<std::string> expected
    {
          "initialize"
        , state_call(ServicePhase::Starting)
        , "register"
        , "start"
        , "run"
        , "stop"
        , "release"
        , state_call(ServicePhase::Stopped)
    };
    EXPECT_EQ(h.service.calls, expected);
}

TEST(SystemServiceBaseTest, unregistered_console_still_starts_the_service)
{
    Harness h;
    h.service.registerResult = false;
    h.service.set_current_option(ServiceOption::CMD_Console);

    EXPECT_EQ(h.service.service_main(ServiceOption::CMD_Console, nullptr), areg::ext::SystemServiceBase::RESULT_SUCCEEDED);

    const std::vector<std::string> expected
    {
          "initialize"
        , state_call(ServicePhase::Starting)
        , "register"
        , "start"
        , "console"
        , "stop"
        , "release"
        , state_call(ServicePhase::Stopped)
    };
    EXPECT_EQ(h.service.calls, expected);
}

TEST(SystemServiceBaseTest, unregistered_service_is_not_started)
{
    Harness h;
    h.service.registerResult = false;
    h.service.set_current_option(ServiceOption::CMD_Service);

    EXPECT_EQ(h.service.service_main(ServiceOption::CMD_Service, nullptr), areg::ext::SystemServiceBase::RESULT_SUCCEEDED);

    const std::vector<std::string> expected
    {
          "initialize"
        , state_call(ServicePhase::Starting)
        , "register"
        , "run"
        , "stop"
        , "release"
        , state_call(ServicePhase::Stopped)
    };
    EXPECT_EQ(h.service.calls, expected);
}

TEST(SystemServiceBaseTest, other_options_neither_run_the_service_nor_the_console)
{
    Harness h;
    h.service.set_current_option(ServiceOption::CMD_Load);

    EXPECT_EQ(h.service.service_main(ServiceOption::CMD_Load, nullptr), areg::ext::SystemServiceBase::RESULT_SUCCEEDED);

    const std::vector<std::string> expected
    {
          "initialize"
        , state_call(ServicePhase::Starting)
        , "register"
        , "start"
        , "stop"
        , "release"
        , state_call(ServicePhase::Stopped)
    };
    EXPECT_EQ(h.service.calls, expected);
}

TEST(SystemServiceBaseTest, failed_initialization_releases_and_reports)
{
    Harness h;
    h.service.initializeResult = false;

    EXPECT_EQ(h.service.service_main(ServiceOption::CMD_Console, nullptr), areg::ext::SystemServiceBase::RESULT_FAILED_INIT);

    const std::vector<std::string> expected
    {
          "initialize"
        , "release"
        , state_call(ServicePhase::Stopped)
    };
    EXPECT_EQ(h.service.calls, expected);
}

TEST(SystemServiceBaseTest, controls_are_routed_to_the_service_calls)
{
    struct Row
    {
        areg::ext::SystemServiceBase::ServiceControl    control;
        const char *                                    call;
    };

    static const Row rows[] =
    {
          { areg::ext::SystemServiceBase::ServiceControl::ServiceStop    , "stop" }
        , { areg::ext::SystemServiceBase::ServiceControl::ServiceShutdown, "stop" }
        , { areg::ext::SystemServiceBase::ServiceControl::ServicePause   , "pause" }
        , { areg::ext::SystemServiceBase::ServiceControl::ServiceContinue, "continue" }
    };

    for (const Row & row : rows)
    {
        Harness h;
        h.service.control_service(row.control);
        ASSERT_EQ(h.service.calls.size(), 1u);
        EXPECT_EQ(h.service.calls[0], row.call);
    }
}
