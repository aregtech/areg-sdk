/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/ServiceApplicationBaseTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests of the service application base: the run modes of
 *              the service main, the service controls and the states they report. The service
 *              never connects to a host, the communication object has another service kind.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/appbase/AppDefs.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/ipc/RemoteServiceDefs.hpp"
#include "areg/persist/ConfigManager.hpp"
#include "aregextend/service/ServiceApplicationBase.hpp"

#include <algorithm>
#include <iterator>
#include <string>
#include <utility>

#if defined(_WIN32)
    #include <Windows.h>

    //! The service entry points every service application defines.
    VOID WINAPI _win32_service_main(DWORD /*argc*/, LPTSTR * /*argv*/)
    {
    }

    VOID WINAPI _win32_service_ctrl_handler(DWORD /*control*/)
    {
    }
#endif  // defined(_WIN32)

namespace
{
    using areg::ext::ServiceOption;
    using areg::ext::ServicePhase;
    using areg::ext::SystemServiceBase;

    //!< The communication object that never starts a thread or a socket.
    class TestComm : public areg::ext::ServiceCommunicationBase
    {
    public:
        TestComm()
            : areg::ext::ServiceCommunicationBase   ( areg::COOKIE_ROUTER
                                                    , areg::RemoteServiceKind::Router
                                                    , static_cast<uint32_t>(areg::ConnectionType::Tcpip)
                                                    , areg::SYSTEM_THREAD_STACK_BIG
                                                    , areg::String("ServiceApplicationBaseTestDispatcher") )
        {
        }

        void failed_process_message(const areg::MessageEnvelope &) override { }
        void on_connection_started() override { }
        void on_connection_stopped() override { }
        void on_connection_lost() override { }
        void on_message_received(const areg::MessageEnvelope &) override { }
    };

    //!< The service application that counts its console runs and configuration hooks.
    class TestApp : public areg::ext::ServiceApplicationBase
    {
    public:
        explicit TestApp(areg::ext::ServiceCommunicationBase & comm)
            : areg::ext::ServiceApplicationBase(comm)
        {
        }

        //!< The service kind that service_start() asks the communication object for.
        areg::RemoteServiceKind kind            { areg::RemoteServiceKind::Logger };
        int32_t                 helpCount       { 0 };
        int32_t                 consoleRuns     { 0 };
        ServicePhase            phaseInConsole  { ServicePhase::Stopped };
        bool                    quitInConsole   { false };
        int32_t                 hookDelta[5]    { 0, 0, 0, 0, 0 };

        void * service_handle() const { return mSvcHandle; }

        //!< Sets the service name, shorter than the name buffers.
        void set_name(const wchar_t * name)
        {
            for (size_t i = 0u; i < 47u; ++i)
            {
                mNameW[i] = name[i];
                mNameA[i] = static_cast<char>(name[i]);
                if (name[i] == L'\0')
                    break;
            }
        }
        [[maybe_unused]] void set_service_handle(void * handle) { mSvcHandle = handle; }

        std::pair<const areg::ext::OptionParser::OptionSetup *, int32_t> app_options() const override
        {
            return { areg::ext::ServiceOptionSetup, static_cast<int32_t>(std::size(areg::ext::ServiceOptionSetup)) };
        }

        wchar_t * service_name_w() const override { return mNameW; }
        char * service_name_a() const override { return mNameA; }
        wchar_t * service_display_name_w() const override { return mNameW; }
        char * service_display_name_a() const override { return mNameA; }
        wchar_t * service_description_w() const override { return mNameW; }
        char * service_description_a() const override { return mNameA; }
        areg::RemoteServiceKind service_type() const override { return kind; }
        areg::ConnectionType connection_type() const override { return areg::ConnectionType::Tcpip; }

        void print_help(bool) override { ++helpCount; }
        areg::ext::Console::CallBack option_check_callback() const override { return nullptr; }

        void run_console_io() override
        {
            ++consoleRuns;
            phaseInConsole = state();
            quitInConsole = areg::Application::wait_quit(areg::DO_NOT_WAIT);
            call_hooks(areg::Application::config_manager());
        }

    private:
        void call_hooks(areg::ConfigManager & config)
        {
            const areg::ListProperties none;
            int32_t before[5];
            std::copy(std::begin(mHooks), std::end(mHooks), before);
            prepare_save_configuration(config);
            post_save_configuration(config);
            prepare_read_configuration(config);
            post_read_configuration(config);
            on_setup_configuration(none, none, config);
            for (size_t i = 0; i < 5u; ++i)
            {
                hookDelta[i] = mHooks[i] - before[i];
            }
        }

        void prepare_save_configuration(areg::ConfigManager & config) override
        {
            ++mHooks[0];
            ServiceApplicationBase::prepare_save_configuration(config);
        }

        void post_save_configuration(areg::ConfigManager & config) override
        {
            ++mHooks[1];
            ServiceApplicationBase::post_save_configuration(config);
        }

        void prepare_read_configuration(areg::ConfigManager & config) override
        {
            ++mHooks[2];
            ServiceApplicationBase::prepare_read_configuration(config);
        }

        void post_read_configuration(areg::ConfigManager & config) override
        {
            ++mHooks[3];
            ServiceApplicationBase::post_read_configuration(config);
        }

        void on_setup_configuration(const areg::ListProperties & ro, const areg::ListProperties & rw, areg::ConfigManager & config) override
        {
            ++mHooks[4];
            ServiceApplicationBase::on_setup_configuration(ro, rw, config);
        }

    private:
        mutable char    mNameA[48]  { 't', 'e', 's', 't', '\0' };
        mutable wchar_t mNameW[48]  { L't', L'e', L's', L't', L'\0' };
        int32_t     mHooks[5]   { 0, 0, 0, 0, 0 };
    };

    //!< The communication object and the service application that uses it.
    struct Harness
    {
        TestComm    comm;
        TestApp     app{ comm };
    };
}

TEST(ServiceApplicationBaseTest, options_come_from_the_application_table)
{
    Harness h;
    char program[] = "service";
    char option[] = "--service";
    char * argv[] = { program, option };

    EXPECT_TRUE(h.app.parse_options(2, argv));
    EXPECT_EQ(h.app.current_option(), ServiceOption::CMD_Service);
    EXPECT_EQ(h.app.helpCount, 0);
}

TEST(ServiceApplicationBaseTest, help_option_fails_through_the_application_table)
{
    Harness h;
    char program[] = "service";
    char option[] = "-h";
    char * argv[] = { program, option };

    EXPECT_FALSE(h.app.parse_options(2, argv));
    EXPECT_EQ(h.app.helpCount, 1);
}

TEST(ServiceApplicationBaseTest, program_name_only_selects_the_console)
{
    Harness h;
    char program[] = "service";
    char * argv[] = { program };
    h.app.set_current_option(ServiceOption::CMD_Service);

    EXPECT_TRUE(h.app.parse_options(1, argv));
    EXPECT_EQ(h.app.current_option(), ServiceOption::CMD_Console);
}

TEST(ServiceApplicationBaseTest, a_new_application_is_stopped)
{
    Harness h;
    EXPECT_EQ(h.app.state(), ServicePhase::Stopped);
    EXPECT_EQ(h.app.current_option(), areg::ext::DEFAULT_OPTION);
    EXPECT_EQ(h.app.service_handle(), nullptr);
}

TEST(ServiceApplicationBaseTest, help_and_verbose_startups_do_nothing)
{
    Harness h;

    EXPECT_EQ(h.app.service_main(ServiceOption::CMD_Help, nullptr), SystemServiceBase::RESULT_SUCCEEDED);
    EXPECT_EQ(h.app.service_main(ServiceOption::CMD_Verbose, nullptr), SystemServiceBase::RESULT_SUCCEEDED);

    EXPECT_EQ(h.app.state(), ServicePhase::Stopped);
    EXPECT_EQ(h.app.consoleRuns, 0);
}

#if defined(_POSIX) || defined(POSIX)

TEST(ServiceApplicationBaseTest, install_succeeds_on_posix)
{
    Harness h;
    EXPECT_EQ(h.app.service_main(ServiceOption::CMD_Install, nullptr), SystemServiceBase::RESULT_SUCCEEDED);
    EXPECT_EQ(h.app.current_option(), ServiceOption::CMD_Install);
    EXPECT_EQ(h.app.consoleRuns, 0);
}

TEST(ServiceApplicationBaseTest, uninstall_frees_the_service_handles)
{
    Harness h;
    int handle{ 0 };
    h.app.set_service_handle(&handle);

    EXPECT_EQ(h.app.service_main(ServiceOption::CMD_Uninstall, nullptr), SystemServiceBase::RESULT_SUCCEEDED);

    EXPECT_EQ(h.app.service_handle(), nullptr);
    EXPECT_EQ(h.app.current_option(), ServiceOption::CMD_Uninstall);
}

TEST(ServiceApplicationBaseTest, posix_service_dispatcher_is_ignored)
{
    Harness h;
    EXPECT_EQ(h.app.start_service_dispatcher(), SystemServiceBase::RESULT_IGNORED);
    EXPECT_EQ(h.app.start_service_dispatcher(), SystemServiceBase::RESULT_IGNORED);
}

TEST(ServiceApplicationBaseTest, posix_registration_and_opening_succeed)
{
    Harness h;
    EXPECT_TRUE(h.app.register_service());
    EXPECT_TRUE(h.app.service_open());
}

TEST(ServiceApplicationBaseTest, service_main_runs_until_quit)
{
    Harness h;

    EXPECT_EQ(h.app.service_main(ServiceOption::CMD_Service, nullptr), SystemServiceBase::RESULT_SUCCEEDED);

    EXPECT_EQ(h.app.consoleRuns, 0);
    EXPECT_EQ(h.app.state(), ServicePhase::Stopped);
    EXPECT_EQ(h.app.current_option(), ServiceOption::CMD_Service);
}

#endif  // defined(_POSIX) || defined(POSIX)

TEST(ServiceApplicationBaseTest, console_main_runs_the_console_and_stops)
{
    Harness h;

    EXPECT_EQ(h.app.service_main(ServiceOption::CMD_Console, nullptr), SystemServiceBase::RESULT_SUCCEEDED);

    EXPECT_EQ(h.app.consoleRuns, 1);
    EXPECT_EQ(h.app.phaseInConsole, ServicePhase::Starting);
    EXPECT_TRUE(h.app.quitInConsole);
    EXPECT_EQ(h.app.state(), ServicePhase::Stopped);
    EXPECT_EQ(h.app.current_option(), ServiceOption::CMD_Console);
}

TEST(ServiceApplicationBaseTest, configuration_hooks_of_the_base_are_callable_while_running)
{
    Harness h;

    EXPECT_EQ(h.app.service_main(ServiceOption::CMD_Console, nullptr), SystemServiceBase::RESULT_SUCCEEDED);

    for (int32_t delta : h.app.hookDelta)
    {
        EXPECT_EQ(delta, 1);
    }
}

TEST(ServiceApplicationBaseTest, load_startup_does_not_enter_the_console)
{
    Harness h;

    EXPECT_EQ(h.app.service_main(ServiceOption::CMD_Load, nullptr), SystemServiceBase::RESULT_SUCCEEDED);

    EXPECT_EQ(h.app.consoleRuns, 0);
    EXPECT_EQ(h.app.state(), ServicePhase::Stopped);
    EXPECT_EQ(h.app.current_option(), ServiceOption::CMD_Load);
}

TEST(ServiceApplicationBaseTest, service_controls_report_their_phases)
{
    areg::Application::setup(false, false, false, false, false);
    {
        Harness h;

        h.app.kind = areg::RemoteServiceKind::Unknown;
        EXPECT_FALSE(h.app.service_start());
        EXPECT_FALSE(areg::Application::wait_quit(areg::DO_NOT_WAIT));
        EXPECT_EQ(h.app.state(), ServicePhase::Stopped);

        h.app.kind = areg::RemoteServiceKind::Logger;
        EXPECT_FALSE(h.app.service_start());
        EXPECT_TRUE(areg::Application::wait_quit(areg::DO_NOT_WAIT));
        EXPECT_EQ(h.app.state(), ServicePhase::Stopped);

        h.app.service_pause();
        EXPECT_EQ(h.app.state(), ServicePhase::Paused);

        EXPECT_FALSE(h.app.service_continue());
        EXPECT_EQ(h.app.state(), ServicePhase::Continuing);

        EXPECT_TRUE(h.app.set_state(ServicePhase::Running));
        EXPECT_EQ(h.app.state(), ServicePhase::Running);
        h.app.service_stop();
        EXPECT_EQ(h.app.state(), ServicePhase::Stopping);

        EXPECT_TRUE(h.app.set_state(ServicePhase::Running));
        h.app.service_shutdown();
        EXPECT_EQ(h.app.state(), ServicePhase::Stopping);

        EXPECT_TRUE(h.app.set_state(ServicePhase::Running));
        h.app.control_service(SystemServiceBase::ServiceControl::ServicePause);
        EXPECT_EQ(h.app.state(), ServicePhase::Paused);
    }
    areg::Application::release();
}

#if defined(_WIN32)

TEST(ServiceApplicationBaseTest, windows_service_dispatcher_fails_outside_the_service_manager_and_can_be_tried_again)
{
    Harness h;
    EXPECT_EQ(h.app.start_service_dispatcher(), SystemServiceBase::RESULT_FAILED_INIT);
    EXPECT_EQ(h.app.start_service_dispatcher(), SystemServiceBase::RESULT_FAILED_INIT);
}

TEST(ServiceApplicationBaseTest, windows_service_startup_fails_outside_the_service_manager_without_running)
{
    Harness h;

    EXPECT_EQ(h.app.service_main(ServiceOption::CMD_Service, nullptr), SystemServiceBase::RESULT_FAILED_INIT);

    EXPECT_EQ(h.app.consoleRuns, 0);
    EXPECT_EQ(h.app.state(), ServicePhase::Stopped);
    EXPECT_EQ(h.app.current_option(), ServiceOption::CMD_Service);
}

TEST(ServiceApplicationBaseTest, windows_registration_of_a_console_startup_fails)
{
    Harness h;
    h.app.set_current_option(ServiceOption::CMD_Console);
    EXPECT_FALSE(h.app.register_service());
}

TEST(ServiceApplicationBaseTest, windows_service_that_is_not_installed_cannot_be_opened_and_uninstall_frees_the_handles)
{
    Harness h;
    h.app.set_name(L"areg_unit_test_no_such_service");
    EXPECT_FALSE(h.app.service_open());
    EXPECT_EQ(h.app.service_handle(), nullptr);

    EXPECT_EQ(h.app.service_main(ServiceOption::CMD_Uninstall, nullptr), SystemServiceBase::RESULT_SUCCEEDED);
    EXPECT_EQ(h.app.service_handle(), nullptr);
    EXPECT_EQ(h.app.current_option(), ServiceOption::CMD_Uninstall);
}

#endif  // defined(_WIN32)

