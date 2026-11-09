/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Copyright (c) 2017-2026 Aregtech (Artak Avetyan)
 * Distributed under the licenses in LICENSE.txt.
 ************************************************************************/

#include "units/ServiceConsoleHarness.hpp"
#include "logcollector/app/LogCollector.hpp"
#include "areg/persist/ConfigManager.hpp"

namespace
{
    using namespace service_harness;

    constexpr const char* SHORT_OPTIONS[]{"-a", "-b", "-c", "-d", "-e", "-f", "-h", "-i", "-l", "-n", "-o", "-p", "-q", "-r", "-s", "-t", "-u", "-v"};
    constexpr const char* LONG_OPTIONS[]{"--save", "--unsave", "--console", "--log", "--query", "--config", "--help", "--install", "--load",
        "--instances", "--scope", "--pause", "--quit", "--restart", "--service", "--silent", "--uninstall", "--verbose"};

    areg::ext::ServiceApplicationBase& application()
    {
        return LogCollector::instance();
    }

    constexpr ServiceUnderTest COLLECTOR{ &application, "logger", areg::COOKIE_LOGGER, "logcollector_app_LogCollector", "" };

    //!< Returns the value an option needs on the command line.
    std::string option_value(const char* option)
    {
        for (const char* name : {"-a", "--save", "-d", "--log", "-e", "--query", "-f", "--config", "-l", "--load", "-o", "--scope"})
        {
            if (std::strcmp(option, name) == 0)
                return "=value";
        }

        return std::string();
    }
}

TEST(LogCollectorApplicationTest, metadata_is_available_through_public_base_interface)
{
    AREG_EXPECT_IN_CHILD(
        auto& app = application();
        AREG_CHILD_CHECK(std::string(app.service_name_a()) == areg::LOGGER_SERVICE_NAME_ASCII);
        AREG_CHILD_CHECK(std::wstring(app.service_name_w()) == areg::LOGGER_SERVICE_NAME_WIDE);
        AREG_CHILD_CHECK(std::strlen(app.service_display_name_a()) != 0u);
        AREG_CHILD_CHECK(std::wstring(app.service_description_w()) ==
            std::wstring(app.service_description_a(), app.service_description_a() + std::strlen(app.service_description_a())));
        AREG_CHILD_CHECK(app.service_type() == areg::RemoteServiceKind::Logger);
        AREG_CHILD_CHECK(app.connection_type() == areg::ConnectionType::Tcpip);
        AREG_CHILD_CHECK(app.state() == ServicePhase::Stopped);
        AREG_CHILD_CHECK(app.current_option() == ServiceOption::CMD_Console);
    );
}

TEST(LogCollectorApplicationTest, option_table_accepts_all_advertised_spellings)
{
    AREG_EXPECT_IN_CHILD(
        auto& app = application();
        const auto setup = app.app_options();
        AREG_CHILD_CHECK(setup.first != nullptr && setup.second == static_cast<int32_t>(std::size(SHORT_OPTIONS)));
        for (size_t i = 0u; i < std::size(SHORT_OPTIONS); ++i)
        {
            areg::ext::OptionParser parser(setup.first, static_cast<uint32_t>(setup.second));
            const std::string suffix{ option_value(SHORT_OPTIONS[i]) };
            AREG_CHILD_CHECK(parser.parse_option_line(areg::String((std::string(SHORT_OPTIONS[i]) + suffix).c_str())));
            AREG_CHILD_CHECK(parser.options().size() == 1u);
            const int32_t command{ parser.options()[0].inCommand };
            AREG_CHILD_CHECK(parser.parse_option_line(areg::String((std::string(LONG_OPTIONS[i]) + suffix).c_str())));
            AREG_CHILD_CHECK(parser.options().size() == 1u && parser.options()[0].inCommand == command);
        }

        areg::ext::OptionParser invalid(setup.first, static_cast<uint32_t>(setup.second));
        AREG_CHILD_CHECK(!invalid.parse_option_line("--not-a-collector-option"));
    );
}

TEST(LogCollectorApplicationTest, command_line_modes_preserve_stopped_state)
{
    AREG_EXPECT_IN_CHILD(
        auto& app = application();
        AREG_CHILD_CHECK(parse(COLLECTOR, {"--service"}) && app.current_option() == ServiceOption::CMD_Service);
        AREG_CHILD_CHECK(parse(COLLECTOR, {"--install"}) && app.current_option() == ServiceOption::CMD_Install);
        AREG_CHILD_CHECK(parse(COLLECTOR, {"--uninstall"}) && app.current_option() == ServiceOption::CMD_Uninstall);
        app.data_rate_helper().set_verbose(false);
        AREG_CHILD_CHECK(parse(COLLECTOR, {"--verbose"}) && app.data_rate_helper().is_verbose());
        AREG_CHILD_CHECK(app.current_option() == ServiceOption::CMD_Console);
        AREG_CHILD_CHECK(parse(COLLECTOR, {}) && app.current_option() == ServiceOption::CMD_Console);
        AREG_CHILD_CHECK(app.state() == ServicePhase::Stopped && !service_dispatcher().is_running());
    );
}

TEST(LogCollectorApplicationTest, console_only_options_are_refused_on_the_command_line)
{
    AREG_EXPECT_IN_CHILD(
        set_terminal();
        for (const char* option : {"--pause", "--restart", "--instances", "--silent", "--quit", "--unsave"})
        {
            testing::internal::CaptureStdout();
            const bool parsed{ parse(COLLECTOR, {option}) };
            const std::string usage{ testing::internal::GetCapturedStdout() };
            AREG_CHILD_CHECK(!parsed);
            AREG_CHILD_CHECK(usage.find("only on the console prompt") != std::string::npos);
            AREG_CHILD_CHECK(usage.find("Usage of Areg Log collector") != std::string::npos);
        }
    );
}

TEST(LogCollectorApplicationTest, log_target_option_accepts_only_known_targets)
{
    AREG_EXPECT_IN_CHILD(
        set_terminal();
        testing::internal::CaptureStdout();
        const bool unknown{ parse(COLLECTOR, {"--log=nowhere"}) };
        const std::string message{ testing::internal::GetCapturedStdout() };
        AREG_CHILD_CHECK(!unknown);
        AREG_CHILD_CHECK(message.find("Unknown target [ nowhere ]") != std::string::npos);
    );
}

TEST(LogCollectorApplicationTest, log_target_option_without_a_target_is_refused)
{
    AREG_EXPECT_IN_CHILD(
        set_terminal();
        testing::internal::CaptureStdout();
        const bool parsed{ parse(COLLECTOR, {"--log"}) };
        const std::string message{ testing::internal::GetCapturedStdout() };
        AREG_CHILD_CHECK(!parsed);
        AREG_CHILD_CHECK(message.find("requires a target") != std::string::npos);
        AREG_CHILD_CHECK(parse(COLLECTOR, {"--log=nodb"}));
    );
}

TEST(LogCollectorApplicationTest, resized_terminal_is_redrawn_with_the_title_and_prompt)
{
#if defined(_WIN32)
    GTEST_SKIP() << "the console of a Windows test is a pipe";
#else
    AREG_EXPECT_IN_CHILD(
        PrivateConfig config(COLLECTOR);
        ConsoleSession console(config);
        console.command("-h", "Usage of Areg Log collector");
        resize_terminal(TerminalSize{ 100, 30 });
        console.expect_gone("Usage of Areg Log collector");
        AREG_CHILD_CHECK(console.wait_screen([](const VtScreen& screen)
            { return screen.row(0).rfind("Areg Log Collector console application", 0) == 0; }));
        console.command("--instances", "There are no connected instances");
        console.quit();
    );
#endif
}

TEST(LogCollectorApplicationTest, command_line_help_is_plain_text)
{
    AREG_EXPECT_IN_CHILD(
        set_terminal();
        for (const char* option : {"--help", "--not-a-collector-option"})
        {
            testing::internal::CaptureStdout();
            const bool parsed{ parse(COLLECTOR, {option}) };
            const std::string usage{ testing::internal::GetCapturedStdout() };
            AREG_CHILD_CHECK(!parsed);
            AREG_CHILD_CHECK(usage.find("Usage of Areg Log collector") != std::string::npos);
            AREG_CHILD_CHECK(usage.find('\x1b') == std::string::npos);
        }
    );
}

TEST(LogCollectorApplicationTest, loaded_configuration_is_private_and_missing_load_is_rejected)
{
    AREG_EXPECT_IN_CHILD(
        PrivateConfig config(COLLECTOR);
        auto& app = application();
        AREG_CHILD_CHECK(parse(COLLECTOR, {"--console", "--load=" + config.path}));
        AREG_CHILD_CHECK(!parse(COLLECTOR, {"--load=" + config.path + ".missing"}));
        AREG_CHILD_CHECK(app.service_initialize(ServiceOption::CMD_Console, nullptr, nullptr));
        const auto& configuration = areg::Application::config_manager();
        AREG_CHILD_CHECK(configuration.remote_service_port(areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip) == config.port);
        AREG_CHILD_CHECK(configuration.remote_service_address(areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip) == "127.0.0.1");
        AREG_CHILD_CHECK(!service_dispatcher().is_running());
        app.service_release();
        config.check_unchanged();
    );
}

TEST(LogCollectorApplicationTest, public_status_outputs_only_in_console_mode)
{
    AREG_EXPECT_IN_CHILD(
        set_terminal();
        auto& app = application();
        testing::internal::CaptureStdout();
        redirect(descriptor(stdout), descriptor(stdout));
        app.set_current_option(ServiceOption::CMD_Service);
        LogCollector::print_status("hidden-status");
        const std::string hidden{ testing::internal::GetCapturedStdout() };
        redirect(descriptor(stdout), descriptor(stdout));
        AREG_CHILD_CHECK(hidden.find("hidden-status") == std::string::npos);
        testing::internal::CaptureStdout();
        redirect(descriptor(stdout), descriptor(stdout));
        app.set_current_option(ServiceOption::CMD_Console);
        LogCollector::print_status("visible-status");
        const std::string shown{ testing::internal::GetCapturedStdout() };
        redirect(descriptor(stdout), descriptor(stdout));
        AREG_CHILD_CHECK(shown.find("visible-status") != std::string::npos);
    );
}

TEST(LogCollectorApplicationTest, malformed_commands_report_errors_and_help_remains_usable)
{
    AREG_EXPECT_IN_CHILD(
        PrivateConfig config(COLLECTOR);
        ConsoleSession console(config);
        console.command("--not-a-collector-option", "unexpected command");
        console.command("--console --install --uninstall --service --load=\"" + config.path + "\"",
                        "This command should be used in command line");
        console.command("--log=db", "This command should be used in command line");
        console.command("--config=file", "The feature is not implemented yet");
        console.command("--help", "Usage of Areg Log collector");
        console.command("--instances", "There are no connected instances");
        console.quit();
        config.check_unchanged();
    );
}

// Reproduces the defect: the error row of a wrong command stayed on the screen after the next valid command.
TEST(LogCollectorApplicationTest, valid_command_clears_the_error_of_the_previous_one)
{
    AREG_EXPECT_IN_CHILD(
        PrivateConfig config(COLLECTOR);
        ConsoleSession console(config);
        console.command("--not-a-collector-option", "unexpected command");
        console.command("--instances", "There are no connected instances");
        console.expect_gone("unexpected command");
        console.quit();
    );
}

// Reproduces the defect: on a small terminal the help text wrapped and scrolled the screen, the title was lost.
TEST(LogCollectorApplicationTest, help_keeps_the_screen_in_place_on_small_terminals)
{
#if defined(_WIN32)
    GTEST_SKIP() << "the console of a Windows test is a pipe";
#else
    for (const TerminalSize size : { TerminalSize{ 80, 24 }, TerminalSize{ 120, 30 } })
    {
        AREG_EXPECT_IN_CHILD(
            PrivateConfig config(COLLECTOR);
            ConsoleSession console(config, size);
            console.command("-h", "Usage of Areg Log collector");
            console.command("--instances", "There are no connected instances");
            const int promptX{ areg::ext::COORD_USER_INPUT.posX + static_cast<int>(areg::ext::FORMAT_WAIT_QUIT.size()) };
            AREG_CHILD_CHECK(console.wait_screen([&](const VtScreen& screen)
                { return (screen.cursor_x() == promptX) && (screen.cursor_y() == areg::ext::COORD_USER_INPUT.posY); }));
            AREG_CHILD_CHECK(console.wait_screen([](const VtScreen& screen)
                { return (screen.scrolled() == 0) && (screen.wrapped() == 0) && (screen.row(0).rfind("Areg Log Collector console application", 0) == 0); }));
            console.quit();
        );
    }
#endif
}

TEST(LogCollectorApplicationTest, connected_instance_is_rendered_and_pause_restart_close_and_reopen_host)
{
    AREG_EXPECT_IN_CHILD(
        PrivateConfig config(COLLECTOR);
        ConsoleSession console(config);
        auto& app = application();
        ConnectedPeer client(COLLECTOR, config.port);
        const auto instances = app.communication_controller().instances();
        AREG_CHILD_CHECK(instances.size() == 1u);
        areg::ConnectedInstance instance;
        AREG_CHILD_CHECK(instances.find(client.reply.target, instance));
        AREG_CHILD_CHECK(instance.ciInstance == areg::Process::instance().name().as_string());
        const size_t rendered{ console.command("--instances", "Instance ID") };
        console.expect(instance_line(1u, client.reply.target, instance), rendered);

        console.command("--pause", "Log collector is paused");
        AREG_CHILD_CHECK(!service_dispatcher().is_running());
        AREG_CHILD_CHECK(!app.communication_controller().is_host_connected());
        AREG_CHILD_CHECK(app.communication_controller().instances().is_empty());
        client.socket.close();
        console.command("--instances", "There are no connected instances");

        console.command("--restart", "Log collector is restarted");
        check_listening(COLLECTOR);
        ConnectedPeer restored(COLLECTOR, config.port);
        ConnectedPeer second(COLLECTOR, config.port);
        const auto populated = app.communication_controller().instances();
        AREG_CHILD_CHECK(populated.size() == 2u);
        AREG_CHILD_CHECK(second.reply.target != restored.reply.target);
        const size_t multiple{ console.command("--instances", "Instance ID") };
        console.expect("    1. |", multiple);
        console.expect("    2. |", multiple);

        console.command("--pause", "Log collector is paused");
        restored.socket.close();
        second.socket.close();
        console.command("--restart", "Log collector is restarted");
        check_listening(COLLECTOR);
        AREG_CHILD_CHECK(app.communication_controller().is_host_connected());
        console.quit();
        config.check_unchanged();
    );
}

TEST(LogCollectorApplicationTest, silent_and_verbose_switch_the_rate_display)
{
    AREG_EXPECT_IN_CHILD(
        PrivateConfig config(COLLECTOR);
        ConsoleSession console(config);
        auto& helper = application().data_rate_helper();
        AREG_CHILD_CHECK(helper.output_data_rate());
        console.silent_command();
        AREG_CHILD_CHECK(!helper.output_data_rate());
        console.verbose_command();
        AREG_CHILD_CHECK(helper.output_data_rate());
        console.quit();
        config.check_unchanged();
    );
}

TEST(LogCollectorApplicationTest, service_helper_counts_a_client_and_survives_pause_and_continue)
{
    AREG_EXPECT_IN_CHILD(
        PrivateConfig config(COLLECTOR);
        auto& app = application();
        AREG_CHILD_CHECK(parse(COLLECTOR, {"--console", "--load=" + config.path}));
        AREG_CHILD_CHECK(app.service_initialize(ServiceOption::CMD_Console, nullptr, nullptr));
        AREG_CHILD_CHECK(app.service_start() && app.state() == ServicePhase::Running);
        check_listening(COLLECTOR);
        ConnectedPeer client(COLLECTOR, config.port);
        app.service_pause();
        AREG_CHILD_CHECK(app.state() == ServicePhase::Paused && !service_dispatcher().is_running());
        AREG_CHILD_CHECK(app.communication_controller().instances().is_empty());
        client.socket.close();
        auto& helper = app.data_rate_helper();
        uint64_t bytes{ 0u };
        uint32_t messages{ 0u };
        helper.query_data_received(bytes, messages);
        AREG_CHILD_CHECK(bytes == client.sentBytes && messages == 1u);
        AREG_CHILD_CHECK(app.service_continue() && app.state() == ServicePhase::Running);
        check_listening(COLLECTOR);
        ConnectedPeer again(COLLECTOR, config.port);
        app.service_stop();
        AREG_CHILD_CHECK(app.state() == ServicePhase::Stopping && !service_dispatcher().is_running());
        app.service_release();
        AREG_CHILD_CHECK(app.set_state(ServicePhase::Stopped));
        config.check_unchanged();
    );
}
