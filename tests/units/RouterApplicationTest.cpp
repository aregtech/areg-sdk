/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Copyright (c) 2017-2026 Aregtech (Artak Avetyan)
 * Distributed under the licenses in LICENSE.txt.
 ************************************************************************/

#include "units/ServiceConsoleHarness.hpp"
#include "mtrouter/app/MultitargetRouter.hpp"
#include "areg/ipc/DataRateStats.hpp"
#include "areg/persist/ConfigManager.hpp"

namespace
{
    using service_harness::ConsoleSession;
    using service_harness::DEADLINE;
    using service_harness::ServiceOption;
    using service_harness::ServicePhase;
    using service_harness::ServiceUnderTest;
    using service_harness::TerminalCapture;
    using service_harness::descriptor;
    using service_harness::instance_line;
    using service_harness::rate_line;
    using service_harness::redirect;
    using service_harness::service_dispatcher;
    using service_harness::set_terminal;

    constexpr const char* SHORT_OPTIONS[]{"-c", "-h", "-i", "-l", "-n", "-p", "-q", "-r", "-s", "-t", "-u", "-v", "-w"};
    constexpr const char* LONG_OPTIONS[]{"--console", "--help", "--install", "--load", "--instances", "--pause", "--quit",
        "--restart", "--service", "--silent", "--uninstall", "--verbose", "--threads"};

    areg::ext::ServiceApplicationBase& application()
    {
        return MultitargetRouter::instance();
    }

    areg::DispatcherThread& router_dispatcher()
    {
        return service_dispatcher();
    }

    constexpr ServiceUnderTest ROUTER{ &application, "router", areg::COOKIE_ROUTER, "mtrouter_app_MultitargetRouter",
                                       "Found option [ MultitargetRouter::RouterOption::CMD_Router" };

    void check_console_verbose(bool expected)
    {
        service_harness::check_console_verbose(ROUTER, expected);
    }

    void check_listening()
    {
        service_harness::check_listening(ROUTER);
    }

    bool parse(const std::vector<std::string>& options)
    {
        return service_harness::parse(ROUTER, options);
    }

    struct PrivateConfig : service_harness::PrivateConfig
    {
        explicit PrivateConfig(bool enableLogging = false, uint32_t pairs = 0u)
            : service_harness::PrivateConfig(ROUTER, enableLogging, pairs)
        {
        }
    };

    struct ConnectedPeer : service_harness::ConnectedPeer
    {
        explicit ConnectedPeer(uint16_t port)
            : service_harness::ConnectedPeer(ROUTER, port)
        {
        }
    };

    void send_to_router(ConnectedPeer& peer, areg::MessageEnvelope&& message)
    {
        AREG_CHILD_CHECK(message.is_valid());
        const areg::EventHeader* header{ message.header() };
        const int32_t size{ static_cast<int32_t>(areg::wire_size(*header)) };
        AREG_CHILD_CHECK(peer.socket.send(areg::wire_data(*header), size) == size);
    }

    void expect_registration(ConnectedPeer& peer, areg::RegistrationAction action, areg::DisconnectReason reason)
    {
        areg::EventHeader received{};
        AREG_CHILD_CHECK(peer.socket.receive(areg::wire_header(received), areg::WIRE_HEADER_SIZE) == areg::WIRE_HEADER_SIZE);
        AREG_CHILD_CHECK(received.messageId == static_cast<uint32_t>(areg::FuncIdRange::SystemServiceNotifyRegister));
        AREG_CHILD_CHECK(received.callType == static_cast<uint8_t>(action));
        AREG_CHILD_CHECK(received.result == static_cast<uint32_t>(reason));
        AREG_CHILD_CHECK(received.bufHeader.biUsed == 0u);
    }
}

TEST(RouterApplicationTest, metadata_is_available_through_public_base_interface)
{
    AREG_EXPECT_IN_CHILD(
        auto& app = application();
        AREG_CHILD_CHECK(&app == &application());
        AREG_CHILD_CHECK(std::string(app.service_name_a()) == areg::ROUTER_SERVICE_NAME_ASCII);
        AREG_CHILD_CHECK(std::wstring(app.service_name_w()) == areg::ROUTER_SERVICE_NAME_WIDE);
        AREG_CHILD_CHECK(std::string(app.service_display_name_a()) == "Areg Multitarget Router Service");
        AREG_CHILD_CHECK(std::wstring(app.service_display_name_w()) == L"Areg Multitarget Router Service");
        AREG_CHILD_CHECK(std::wstring(app.service_description_w()) ==
            std::wstring(app.service_description_a(), app.service_description_a() + std::strlen(app.service_description_a())));
        AREG_CHILD_CHECK(std::string(app.service_description_a()).find("TCP / IP Multitarget Router") != std::string::npos);
        AREG_CHILD_CHECK(app.service_type() == areg::RemoteServiceKind::Router);
        AREG_CHILD_CHECK(app.connection_type() == areg::ConnectionType::Tcpip);
        AREG_CHILD_CHECK(app.state() == ServicePhase::Stopped);
        AREG_CHILD_CHECK(app.current_option() == ServiceOption::CMD_Console);
    );
}

TEST(RouterApplicationTest, option_table_accepts_all_advertised_spellings)
{
    AREG_EXPECT_IN_CHILD(
        auto& app = application();
        const auto setup = app.app_options();
        AREG_CHILD_CHECK(setup.first != nullptr && setup.second == 13);
        for (size_t i = 0u; i < std::size(SHORT_OPTIONS); ++i)
        {
            areg::ext::OptionParser parser(setup.first, static_cast<uint32_t>(setup.second));
            const std::string suffix{ i == 3u ? "=private.init" : "" };
            AREG_CHILD_CHECK(parser.parse_option_line(areg::String((std::string(SHORT_OPTIONS[i]) + suffix).c_str())));
            AREG_CHILD_CHECK(parser.options().size() == 1u);
            const int32_t command{ parser.options()[0].inCommand };
            AREG_CHILD_CHECK(parser.parse_option_line(areg::String((std::string(LONG_OPTIONS[i]) + suffix).c_str())));
            AREG_CHILD_CHECK(parser.options().size() == 1u && parser.options()[0].inCommand == command);
        }
        areg::ext::OptionParser invalid(setup.first, static_cast<uint32_t>(setup.second));
        AREG_CHILD_CHECK(!invalid.parse_option_line("--not-a-router-option"));
        AREG_CHILD_CHECK(!invalid.parse_option_line("--save=private.init"));
    );
}

TEST(RouterApplicationTest, command_line_modes_and_reset_preserve_stopped_state)
{
    AREG_EXPECT_IN_CHILD(
        auto& app = application();
        AREG_CHILD_CHECK(parse({"--service"}) && app.current_option() == ServiceOption::CMD_Service);
        AREG_CHILD_CHECK(parse({"--install"}) && app.current_option() == ServiceOption::CMD_Install);
        AREG_CHILD_CHECK(parse({"--uninstall"}) && app.current_option() == ServiceOption::CMD_Uninstall);
        app.data_rate_helper().set_verbose(false);
        AREG_CHILD_CHECK(parse({"--verbose"}) && app.data_rate_helper().is_verbose());
        AREG_CHILD_CHECK(app.current_option() == ServiceOption::CMD_Console);
        AREG_CHILD_CHECK(parse({}) && app.current_option() == ServiceOption::CMD_Console);
        AREG_CHILD_CHECK(app.state() == ServicePhase::Stopped && !router_dispatcher().is_running());
    );
}

TEST(RouterApplicationTest, loaded_configuration_is_private_and_missing_load_is_rejected)
{
    AREG_EXPECT_IN_CHILD(
        PrivateConfig config;
        auto& app = application();
        AREG_CHILD_CHECK(parse({"--console", "--load=" + config.path}));
        AREG_CHILD_CHECK(!parse({"--load=" + config.path + ".missing"}));
        AREG_CHILD_CHECK(app.service_initialize(ServiceOption::CMD_Console, nullptr, nullptr));
        const auto& configuration = areg::Application::config_manager();
        AREG_CHILD_CHECK(configuration.remote_service_port(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip) == config.port);
        AREG_CHILD_CHECK(configuration.remote_service_address(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip) == "127.0.0.1");
        AREG_CHILD_CHECK(configuration.remote_service_enable(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip));
        AREG_CHILD_CHECK(app.communication_controller().setup_connection_data(
            areg::RemoteServiceKind::Router, static_cast<uint32_t>(areg::ConnectionType::Tcpip)));
        AREG_CHILD_CHECK(app.communication_controller().is_host_setup());
        AREG_CHILD_CHECK(!router_dispatcher().is_running());
        app.service_release();
        config.check_unchanged();
    );
}

TEST(RouterApplicationTest, public_status_outputs_only_in_console_mode)
{
    AREG_EXPECT_IN_CHILD(
        set_terminal();
        auto& app = application();
        testing::internal::CaptureStdout();
        redirect(descriptor(stdout), descriptor(stdout));
        app.set_current_option(ServiceOption::CMD_Service);
        MultitargetRouter::print_status("C2-hidden-status");
        const std::string hidden{ testing::internal::GetCapturedStdout() };
        redirect(descriptor(stdout), descriptor(stdout));
        AREG_CHILD_CHECK(hidden.find("C2-hidden-status") == std::string::npos);
        testing::internal::CaptureStdout();
        redirect(descriptor(stdout), descriptor(stdout));
        app.set_current_option(ServiceOption::CMD_Console);
        MultitargetRouter::print_status("C2-visible-status");
        const std::string shown{ testing::internal::GetCapturedStdout() };
        redirect(descriptor(stdout), descriptor(stdout));
        AREG_CHILD_CHECK(shown.find("C2-visible-status") != std::string::npos);
        AREG_CHILD_CHECK(app.current_option() == ServiceOption::CMD_Console);
    );
}

TEST(RouterApplicationTest, malformed_commands_report_errors_and_help_remains_usable)
{
    AREG_EXPECT_IN_CHILD(
        PrivateConfig config(true);
        ConsoleSession console(config);
        console.command("--not-a-router-option", "unexpected command");
        console.command("--console --install --uninstall --service --load=\"" + config.path + "\"",
                        "This command should be used in command line");
        console.command("--help", "Usage of Areg Multi-target Message Router");
        console.command("--instances", "There are no connected instances");
        console.command("--threads", "Active per-client thread pairs: 0");
        console.quit();
        config.check_unchanged();
        config.check_options_logged({"Console", "Install", "Uninstall", "Service", "Load",
            "PrintHelp", "Instances", "Threads", "Quit"});
    );
}

// Reproduces the defect: a scrolled screen published its old rows again, so a test matched the text of an earlier command.
TEST(RouterApplicationTest, scrolled_terminal_publishes_no_old_rows)
{
    TerminalCapture terminal;
    const std::string filler(38u, '\n');
    terminal.feed(filler.data(), filler.size());
    const std::string marker{ "earlier-command-output\n" };
    terminal.feed(marker.data(), marker.size());
    const size_t offset{ terminal.transcript.size() };
    const std::string scroll{ "first-new-row\nsecond-new-row\nthird-new-row\n" };
    terminal.feed(scroll.data(), scroll.size());

    EXPECT_NE(std::string::npos, terminal.transcript.find("third-new-row", offset));
    EXPECT_EQ(std::string::npos, terminal.transcript.find("earlier-command-output", offset));
}

// Reproduces the defect: a row written again with the same text was not published, so a test waited for it until the deadline.
TEST(RouterApplicationTest, terminal_publishes_a_row_written_again_with_the_same_text)
{
    TerminalCapture terminal;
    const std::string row{ "\x1b[5;1Hsame-row-text" };
    terminal.feed(row.data(), row.size());
    const size_t offset{ terminal.transcript.size() };
    terminal.feed(row.data(), row.size());

    EXPECT_NE(std::string::npos, terminal.transcript.find("same-row-text", offset));
}

// Reproduces the defect: ncurses discarded the usage text at exit, so a failed command line printed nothing.
TEST(RouterApplicationTest, command_line_help_is_plain_text)
{
    AREG_EXPECT_IN_CHILD(
        set_terminal();
        for (const char* option : {"--help", "--not-a-router-option"})
        {
            testing::internal::CaptureStdout();
            const bool parsed{ parse({option}) };
            const std::string usage{ testing::internal::GetCapturedStdout() };
            AREG_CHILD_CHECK(!parsed);
            AREG_CHILD_CHECK(usage.find("Usage of Areg Multi-target Message Router") != std::string::npos);
            AREG_CHILD_CHECK(usage.find("--threads") != std::string::npos);
            AREG_CHILD_CHECK(usage.find('\x1b') == std::string::npos);
        }
    );
}

// Reproduces the defect: the router took the cookie to disconnect from the payload, so any client could drop another one.
TEST(RouterApplicationTest, disconnect_request_releases_only_its_sender)
{
    AREG_EXPECT_IN_CHILD(
        PrivateConfig config;
        ConsoleSession console(config);
        ConnectedPeer owner(config.port);
        ConnectedPeer other(config.port);
        const ITEM_ID ownerCookie{ owner.reply.target };
        const ITEM_ID otherCookie{ other.reply.target };

        areg::StubAddress provider(0x4321u, areg::Version(1u, 2u, 3u), areg::ServiceType::Public, 0x1234u, 0x1111u, 0x2222u);
        provider.set_cookie(ownerCookie);
        provider.set_source(ownerCookie);
        areg::ProxyAddress consumer(0x4321u, areg::Version(1u, 2u, 3u), areg::ServiceType::Public, 0x1234u, 0x3333u, 0x4444u);
        consumer.set_cookie(otherCookie);
        consumer.set_source(otherCookie);
        send_to_router(owner, areg::router_register_service(provider, ownerCookie, areg::COOKIE_ROUTER));
        send_to_router(other, areg::router_register_consumer(consumer, otherCookie, areg::COOKIE_ROUTER));
        expect_registration(owner, areg::RegistrationAction::RegisterConsumer, areg::DisconnectReason::UndefinedReason);
        expect_registration(other, areg::RegistrationAction::RegisterProvider, areg::DisconnectReason::UndefinedReason);

        areg::MessageEnvelope bye{ areg::create_disconnect_request(otherCookie, areg::COOKIE_ROUTER) };
        AREG_CHILD_CHECK(bye.header() != nullptr);
        bye.header()->source = static_cast<uint32_t>(ownerCookie);
        send_to_router(owner, std::move(bye));

        expect_registration(other, areg::RegistrationAction::UnregisterProvider, areg::DisconnectReason::ProviderDisconnected);
        const auto instances = application().communication_controller().instances();
        areg::ConnectedInstance survivor;
        AREG_CHILD_CHECK(instances.size() == 1u && instances.find(otherCookie, survivor));
        console.quit();
    );
}

TEST(RouterApplicationTest, connected_instance_is_rendered_and_pause_restart_close_and_reopen_host)
{
    AREG_EXPECT_IN_CHILD(
        PrivateConfig config(true);
        ConsoleSession console(config);
        auto& app = application();
        ConnectedPeer client(config.port);
        const auto instances = app.communication_controller().instances();
        AREG_CHILD_CHECK(instances.size() == 1u);
        areg::ConnectedInstance instance;
        AREG_CHILD_CHECK(instances.find(client.reply.target, instance));
        AREG_CHILD_CHECK(instance.ciInstance == areg::Process::instance().name().as_string());
        AREG_CHILD_CHECK(static_cast<uint32_t>(instance.ciBitness) == sizeof(void*) * 8u);
        const size_t rendered{ console.command("--instances", "Instance ID") };
        console.expect(instance_line(1u, client.reply.target, instance), rendered);
        console.expect(rate_line(areg::ext::FORMAT_SEND_DATA, client.received_bytes()));
        console.expect(rate_line(areg::ext::FORMAT_RECV_DATA, client.sentBytes));
        console.expect("Send messages ....:         1 msgs/sec");
        console.expect("Recv messages ....:         1 msgs/sec");
        console.command("--pause", "paused");
        AREG_CHILD_CHECK(!router_dispatcher().is_running());
        AREG_CHILD_CHECK(!app.communication_controller().is_host_connected());
        AREG_CHILD_CHECK(app.communication_controller().instances().is_empty());
        client.socket.close();
        console.silent_command();
        check_console_verbose(AREG_EXTENDED == 0);
        console.command("--help", "Usage of Areg Multi-target Message Router");
        console.command("--silent --threads", "Active per-client thread pairs: 0");
        console.verbose_command();
        check_console_verbose(true);
        console.command("--help", "Usage of Areg Multi-target Message Router");
        console.command("--verbose --threads", "Active per-client thread pairs: 0");
        check_console_verbose(true);
        console.command("--restart", "restarted");
        check_listening();
        ConnectedPeer restored(config.port);
        ConnectedPeer second(config.port);
        const auto populated = app.communication_controller().instances();
        AREG_CHILD_CHECK(populated.size() == 2u);
        AREG_CHILD_CHECK(second.reply.target != restored.reply.target);
        const size_t multiple{ console.command("--instances", "Instance ID") };
        console.expect("    1. |", multiple);
        console.expect("    2. |", multiple);
        for (auto position = populated.first_position(); populated.is_valid_position(position); position = populated.next_position(position))
        {
            ITEM_ID cookie{ 0u };
            areg::ConnectedInstance connected;
            populated.at_position(position, cookie, connected);
            console.expect_any({instance_line(1u, cookie, connected), instance_line(2u, cookie, connected)}, multiple);
        }
        console.command("--pause", "paused");
        AREG_CHILD_CHECK(!router_dispatcher().is_running());
        AREG_CHILD_CHECK(!app.communication_controller().is_host_connected());
        AREG_CHILD_CHECK(app.communication_controller().instances().is_empty());
        restored.socket.close();
        second.socket.close();
        console.command("--instances", "There are no connected instances");
        console.command("--restart", "restarted");
        check_listening();
        AREG_CHILD_CHECK(app.communication_controller().is_host_connected());
        console.quit();
        config.check_unchanged();
        config.check_options_logged({"Instances", "Silent", "Verbose", "Pause", "Restart", "Quit"});
    );
}

// Reproduces the defect: the console read the pool list while the service thread filled it. It needs TSan to fail.
TEST(RouterApplicationTest, thread_pair_display_matches_pool_configuration_and_pause_restart)
{
    AREG_EXPECT_IN_CHILD(
        PrivateConfig config(false, 2u);
        ConsoleSession console(config);
        auto& communication = application().communication_controller();
        AREG_CHILD_CHECK(communication.active_client_pair_count() == 2u);
        console.command("--threads", "Active per-client thread pairs: 2");
        console.command("--pause", "paused");
        AREG_CHILD_CHECK(communication.active_client_pair_count() == 0u);
        console.command("--threads", "Active per-client thread pairs: 0");
        console.command("--restart", "restarted");
        check_listening();
        AREG_CHILD_CHECK(communication.active_client_pair_count() == 2u);
        console.command("--threads", "Active per-client thread pairs: 2");
        console.quit();
        AREG_CHILD_CHECK(communication.active_client_pair_count() == 0u);
        config.check_unchanged();
    );
}

TEST(RouterApplicationRateTest, service_helper_queries_exact_wire_totals_and_mute_resets_interval)
{
    AREG_EXPECT_IN_CHILD(
        PrivateConfig config;
        auto& app = application();
        AREG_CHILD_CHECK(parse({"--console", "--load=" + config.path}));
        AREG_CHILD_CHECK(app.service_initialize(ServiceOption::CMD_Console, nullptr, nullptr));
        AREG_CHILD_CHECK(app.service_start() && app.state() == ServicePhase::Running);
        check_listening();
        ConnectedPeer client(config.port);
        app.service_pause();
        AREG_CHILD_CHECK(app.state() == ServicePhase::Paused && !router_dispatcher().is_running());
        AREG_CHILD_CHECK(app.communication_controller().instances().is_empty());
        client.socket.close();
        auto& helper = app.data_rate_helper();
        uint64_t bytes{ 999u };
        uint32_t messages{ 999u };
        helper.query_data_received(bytes, messages);
        AREG_CHILD_CHECK(bytes == client.sentBytes && messages == 1u);
        helper.query_data_sent(bytes, messages);
        AREG_CHILD_CHECK(bytes == client.received_bytes() && messages == 1u);
        helper.query_data_received(bytes, messages);
        AREG_CHILD_CHECK(bytes == 0u && messages == 0u);
        helper.query_data_sent(bytes, messages);
        AREG_CHILD_CHECK(bytes == 0u && messages == 0u);
        helper.set_verbose(false);
        AREG_CHILD_CHECK(!helper.is_verbose());
        helper.query_data_received(bytes, messages);
        AREG_CHILD_CHECK(bytes == 0u && messages == 0u);
        helper.query_data_sent(bytes, messages);
        AREG_CHILD_CHECK(bytes == 0u && messages == 0u);
        helper.set_verbose(true);
        AREG_CHILD_CHECK(helper.is_verbose());
        AREG_CHILD_CHECK(app.service_continue() && app.state() == ServicePhase::Running);
        check_listening();
        app.service_stop();
        AREG_CHILD_CHECK(app.state() == ServicePhase::Stopping && !router_dispatcher().is_running());
        app.service_release();
        AREG_CHILD_CHECK(app.set_state(ServicePhase::Stopped));
        config.check_unchanged();
    );
}

// Reproduces the defect: the rate thread redrew the four rows that silent mode had cleared.
TEST(RouterApplicationRateTest, silent_mode_outputs_no_rate_rows)
{
    AREG_EXPECT_IN_CHILD(
        PrivateConfig config;
        ConsoleSession console(config);
        auto& helper = application().data_rate_helper();
        AREG_CHILD_CHECK(helper.output_data_rate());
        console.silent_command();
        const size_t silent{ console.mark() };
        AREG_CHILD_CHECK(helper.output_data_rate() == (AREG_EXTENDED == 0));
        console.command("--threads", "Active per-client thread pairs: 0");
        AREG_CHILD_CHECK((AREG_EXTENDED == 0) || !console.shown_since("Send data rate", silent));
        console.verbose_command();
        AREG_CHILD_CHECK(helper.output_data_rate());
        console.quit();
        config.check_unchanged();
    );
}

// Reproduces the defect: the enabled flag was a plain bool that one thread wrote while another read it. It needs TSan to fail.
TEST(RouterApplicationRateTest, live_enabled_changes_are_thread_safe)
{
    AREG_EXPECT_IN_CHILD(
        areg::DataRateStats stats;
        stats.set_enabled(true);
        std::promise<void> started;
        auto ready = started.get_future();
        std::thread producer([&]()
        {
            started.set_value();
            for (uint32_t i = 0u; i < 10'000u; ++i)
                stats.accumulate(7u, 1u);
        });
        AREG_CHILD_CHECK(ready.wait_for(DEADLINE) == std::future_status::ready);
        for (uint32_t i = 0u; i < 10'000u; ++i)
            stats.set_enabled((i & 1u) != 0u);
        producer.join();
        stats.set_enabled(false);
        stats.accumulate(999u, 999u);
        AREG_CHILD_CHECK(stats.extract_bytes() == 0u && stats.extract_msgs() == 0u);
        stats.set_enabled(true);
        stats.accumulate(7u * 13u, 13u);
        AREG_CHILD_CHECK(stats.extract_bytes() == 91u && stats.extract_msgs() == 13u);
    );
}
