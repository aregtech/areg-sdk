/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial licenses. See LICENSE.txt for details.
 * Copyright (c) 2017-2026 Aregtech (Artak Avetyan)
 ************************************************************************/
#include "units/ApplicationSetupTestSupport.hpp"
#include "areg/base/File.hpp"
#include "areg/base/MemoryDefs.hpp"
#include "areg/base/Version.hpp"
#include "areg/component/DispatcherThread.hpp"
#include "areg/component/Model.hpp"
#include "areg/component/Timer.hpp"
#include "areg/component/TimerConsumer.hpp"
#include "areg/logging/LoggingDefs.hpp"
#include "areg/persist/Property.hpp"

#include <algorithm>
#include <array>
#include <chrono>
#include <condition_variable>
#include <future>
#include <mutex>
#include <thread>

namespace
{
    constexpr uint32_t APP_WAIT_MS{ 30'000u };
    constexpr std::chrono::seconds APP_WAIT{ 30 };

    class ApplicationTimerProbe final : public areg::DispatcherThread, public areg::TimerConsumer
    {
    public:
        ApplicationTimerProbe()
            : areg::DispatcherThread("ApplicationSetupTimerProbe", areg::DEFAULT_STACK_SIZE, 16u)
            , areg::TimerConsumer()
            , mTimer(*this, "ApplicationSetupTimer")
            , mMutex()
            , mChanged()
            , mCalls(0u)
            , mCorrectThread(true)
        {
        }

        ~ApplicationTimerProbe() override
        {
            mTimer.stop_timer();
            if (is_valid())
            {
                EXPECT_EQ(shutdown(areg::WAIT_INFINITE), areg::Thread::ThreadCompletion::Completed);
            }
        }

        bool start_ready()
        {
            return start(APP_WAIT_MS) && wait_start(APP_WAIT_MS);
        }

        bool arm(uint32_t timeout)
        {
            return mTimer.start_timer(timeout, *this, areg::TimerBase::ONE_TIME);
        }

        bool wait_for_expiry()
        {
            std::unique_lock<std::mutex> guard(mMutex);
            return mChanged.wait_for(guard, APP_WAIT, [this]() { return mCalls == 1u; });
        }

        uint32_t calls()
        {
            std::lock_guard<std::mutex> guard(mMutex);
            return mCalls;
        }

        bool correct_thread()
        {
            std::lock_guard<std::mutex> guard(mMutex);
            return mCorrectThread;
        }

        bool post_event(areg::Event& event) final
        {
            return areg::EventDispatcher::post_event(event);
        }

    private:
        void process_timer(areg::Timer& timer) final
        {
            {
                std::lock_guard<std::mutex> guard(mMutex);
                ++mCalls;
                mCorrectThread = mCorrectThread && (&timer == &mTimer)
                               && (&areg::DispatcherThread::current_dispatcher_thread() == this);
            }
            mChanged.notify_all();
        }

        areg::Timer mTimer;
        std::mutex mMutex;
        std::condition_variable mChanged;
        uint32_t mCalls;
        bool mCorrectThread;
    };

    class ApplicationSetupOptionsTest : public ApplicationSetupTest, public testing::WithParamInterface<uint32_t>
    {
    };

    class ApplicationSetupRoutingDependencyTest : public ApplicationSetupTest, public testing::WithParamInterface<uint32_t>
    {
    };

    std::vector<uint32_t> valid_setup_options()
    {
        std::vector<uint32_t> result;
        for (uint32_t mask = 0u; mask < 32u; ++mask)
        {
            result.push_back(mask);
        }
        return result;
    }

    areg::Primitive storage_value(uint64_t value)
    {
        areg::Primitive result{};
        result.valUInt64.mElement = value;
        return result;
    }

    void expect_default_configuration(const areg::ConfigManager& manager = areg::Application::config_manager())
    {
        EXPECT_TRUE(areg::Application::is_configured());
        EXPECT_EQ(manager.config_version(), areg::Version(areg::String(areg::CONFIG_VERSION)));
        EXPECT_EQ(manager.log_version(), areg::Version(areg::String(areg::LOG_VERSION)));
        const auto services{ manager.service_list() };
        ASSERT_EQ(services.size(), 2u);
        EXPECT_EQ(services[0].name(), "router");
        EXPECT_EQ(services[1].name(), "logger");
        EXPECT_TRUE(manager.remote_service_enable(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip));
        EXPECT_TRUE(manager.remote_service_enable(areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip));
        EXPECT_EQ(manager.remote_service_address(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip)
                , areg::DEFAULT_ROUTER_HOST);
        EXPECT_EQ(manager.remote_service_port(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip), 8181u);
        EXPECT_EQ(manager.remote_service_port(areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip), 8282u);
        EXPECT_TRUE(manager.logging_status());
        EXPECT_TRUE(manager.log_enabled(areg::LogTarget::File));
        EXPECT_TRUE(manager.log_enabled(areg::LogTarget::Remote));
        EXPECT_FALSE(manager.log_enabled(areg::LogTarget::Debug));
        EXPECT_FALSE(manager.log_enabled(areg::LogTarget::Database));
    }
}

TEST_P(ApplicationSetupOptionsTest, selected_services_are_ready_and_release_joins_every_manager)
{
    const uint32_t options{ GetParam() };
    const bool logging{ (options & 1u) != 0u };
    const bool servicing{ (options & 2u) != 0u };
    const bool routing{ (options & 4u) != 0u };
    const bool timer{ (options & 8u) != 0u };
    const bool watchdog{ (options & 16u) != 0u };
    setup_application(logging, servicing, routing, timer, watchdog);

    const bool manager{ servicing || routing };
    EXPECT_EQ(areg::Application::system_state(), areg::AppState::Ready);
    EXPECT_TRUE(areg::Application::is_servicing_available());
    EXPECT_EQ(application_thread(APP_SERVICE_THREAD) != nullptr, manager);
    EXPECT_EQ(application_thread(APP_TIMER_THREAD) != nullptr, timer || manager);
    EXPECT_EQ(application_thread(APP_WATCHDOG_THREAD) != nullptr, watchdog || manager);
#if AREG_LOGGING
    EXPECT_EQ(areg::is_started(), logging);
    EXPECT_EQ(application_thread(APP_LOG_THREAD) != nullptr, logging);
#endif
    EXPECT_EQ(areg::Application::router_state(), areg::RouterState::NotConfigured);
    EXPECT_FALSE(areg::Application::connect_router());
    EXPECT_EQ(mListener.mEvents, (std::vector<std::string>{ "prepare_read", "post_read" }));
    EXPECT_EQ(mListener.mStates, (std::vector<areg::AppState>{ areg::AppState::Initializing, areg::AppState::Initializing }));
    EXPECT_EQ(mListener.mAvailable, (std::vector<bool>{ true, true }));
    EXPECT_EQ(mListener.mReady, (std::vector<bool>{ false, false }));
    for (auto* manager : mListener.mManagers)
    {
        EXPECT_EQ(manager, &areg::Application::config_manager());
    }
    release_application();
}

INSTANTIATE_TEST_SUITE_P(ServiceFlags, ApplicationSetupOptionsTest, testing::ValuesIn(valid_setup_options())
    , [](const testing::TestParamInfo<uint32_t>& info) { return "Flags" + std::to_string(info.param); });

TEST_P(ApplicationSetupRoutingDependencyTest, routing_only_setup_starts_its_service_and_timer_dependencies)
{
    const uint32_t options{ GetParam() };
    setup_application((options & 1u) != 0u, false, true, (options & 8u) != 0u, (options & 16u) != 0u);
    EXPECT_EQ(areg::Application::system_state(), areg::AppState::Ready);
    EXPECT_NE(application_thread(APP_SERVICE_THREAD), nullptr);
    EXPECT_NE(application_thread(APP_TIMER_THREAD), nullptr);
    EXPECT_NE(application_thread(APP_WATCHDOG_THREAD), nullptr);
}

INSTANTIATE_TEST_SUITE_P(RoutingDependency, ApplicationSetupRoutingDependencyTest
    , testing::Values(4u, 5u, 12u, 13u, 20u, 21u, 28u, 29u)
    , [](const testing::TestParamInfo<uint32_t>& info) { return "Flags" + std::to_string(info.param); });

TEST_F(ApplicationSetupTest, configuration_only_setup_transitions_from_stopped_to_ready)
{
    EXPECT_FALSE(areg::Application::is_servicing_ready());
    EXPECT_FALSE(areg::Application::is_servicing_available());
    setup_application();
    EXPECT_FALSE(areg::Application::wait_quit(0u));
    EXPECT_EQ(areg::Application::system_state(), areg::AppState::Ready);
    EXPECT_TRUE(areg::Application::is_servicing_ready());
    EXPECT_TRUE(areg::Application::is_servicing_available());
    expect_application_threads_stopped();
    release_application();
}

TEST_F(ApplicationSetupTest, repeated_setup_keeps_running_managers_and_reads_configuration_once)
{
    setup_application(true, true, false, true, true);
    std::array<areg::Thread*, 4> before
    {
          application_thread(APP_SERVICE_THREAD), application_thread(APP_TIMER_THREAD)
        , application_thread(APP_WATCHDOG_THREAD), application_thread(APP_LOG_THREAD)
    };
    setup_application(true, true, false, true, true);
    EXPECT_EQ(areg::Application::system_state(), areg::AppState::Ready);
    EXPECT_EQ(mListener.mEvents, (std::vector<std::string>{ "prepare_read", "post_read" }));
    EXPECT_EQ(before[0], application_thread(APP_SERVICE_THREAD));
    EXPECT_EQ(before[1], application_thread(APP_TIMER_THREAD));
    EXPECT_EQ(before[2], application_thread(APP_WATCHDOG_THREAD));
    EXPECT_EQ(before[3], application_thread(APP_LOG_THREAD));
    release_application();
}

TEST_F(ApplicationSetupTest, repeated_release_is_safe_and_does_not_restart_any_manager)
{
    setup_application(true, true, false, true, true);
    release_application();
    release_application();
    EXPECT_TRUE(areg::Application::is_configured());
}

TEST_F(ApplicationSetupTest, release_before_setup_leaves_the_system_stopped)
{
    release_application();
    EXPECT_FALSE(areg::Application::is_configured());
}

TEST_F(ApplicationSetupTest, timer_only_setup_delivers_one_callback_on_its_dispatcher)
{
    setup_application(false, false, false, true, false);
    {
        ApplicationTimerProbe probe;
        ASSERT_TRUE(probe.start_ready());
        ASSERT_TRUE(probe.arm(1u));
        ASSERT_TRUE(probe.wait_for_expiry());
        EXPECT_TRUE(probe.correct_thread());
        EXPECT_EQ(probe.calls(), 1u);
    }
    release_application();
}

TEST_F(ApplicationSetupTest, servicing_enables_timer_delivery_even_when_timer_flag_is_false)
{
    setup_application(false, true, false, false, false);
    {
        ApplicationTimerProbe probe;
        ASSERT_TRUE(probe.start_ready());
        ASSERT_TRUE(probe.arm(1u));
        ASSERT_TRUE(probe.wait_for_expiry());
        EXPECT_EQ(probe.calls(), 1u);
        EXPECT_TRUE(probe.correct_thread());
    }
    release_application();
}

TEST_F(ApplicationSetupTest, release_cancels_an_armed_timer_before_its_dispatcher_is_destroyed)
{
    setup_application(false, false, false, true, false);
    ApplicationTimerProbe probe;
    ASSERT_TRUE(probe.start_ready());
    ASSERT_TRUE(probe.arm(3'600'000u));
    release_application();
    EXPECT_EQ(probe.calls(), 0u);
}

TEST_F(ApplicationSetupTest, watchdog_can_start_stop_and_restart_without_other_services)
{
    setup_application();
    areg::Application::stop_watchdog_manager();
    for (uint32_t cycle = 0u; cycle < 3u; ++cycle)
    {
        ASSERT_TRUE(areg::Application::start_watchdog_manager());
        auto* watchdog{ application_thread(APP_WATCHDOG_THREAD) };
        ASSERT_NE(watchdog, nullptr);
        EXPECT_TRUE(watchdog->is_running());
        EXPECT_TRUE(areg::Application::start_watchdog_manager());
        EXPECT_EQ(application_thread(APP_WATCHDOG_THREAD), watchdog);
        EXPECT_EQ(application_thread(APP_TIMER_THREAD), nullptr);
        EXPECT_EQ(application_thread(APP_SERVICE_THREAD), nullptr);
        areg::Application::stop_watchdog_manager();
        EXPECT_EQ(application_thread(APP_WATCHDOG_THREAD), nullptr);
        areg::Application::stop_watchdog_manager();
        EXPECT_EQ(areg::Application::system_state(), areg::AppState::Ready);
    }
}

TEST_F(ApplicationSetupTest, stopping_watchdog_does_not_stop_timer_or_servicing)
{
    setup_application(false, true, false, false, false);
    auto* timer{ application_thread(APP_TIMER_THREAD) };
    auto* service{ application_thread(APP_SERVICE_THREAD) };
    ASSERT_NE(timer, nullptr);
    ASSERT_NE(service, nullptr);
    areg::Application::stop_watchdog_manager();
    EXPECT_EQ(application_thread(APP_WATCHDOG_THREAD), nullptr);
    EXPECT_EQ(application_thread(APP_TIMER_THREAD), timer);
    EXPECT_EQ(application_thread(APP_SERVICE_THREAD), service);
    EXPECT_TRUE(areg::Application::is_servicing_available());
    EXPECT_TRUE(areg::Application::start_watchdog_manager());
}

TEST_F(ApplicationSetupTest, logging_can_start_stop_and_restart_without_starting_internal_services)
{
    setup_application();
    areg::Application::stop_logging();
    for (uint32_t cycle = 0u; cycle < 3u; ++cycle)
    {
        ASSERT_TRUE(areg::Application::start_logging());
#if AREG_LOGGING
        EXPECT_TRUE(areg::is_started());
        auto* logger{ application_thread(APP_LOG_THREAD) };
        ASSERT_NE(logger, nullptr);
        EXPECT_TRUE(logger->is_running());
#endif
        EXPECT_TRUE(areg::Application::start_logging(true));
#if AREG_LOGGING
        EXPECT_EQ(application_thread(APP_LOG_THREAD), logger);
#endif
        EXPECT_EQ(application_thread(APP_SERVICE_THREAD), nullptr);
        EXPECT_EQ(application_thread(APP_TIMER_THREAD), nullptr);
        EXPECT_EQ(application_thread(APP_WATCHDOG_THREAD), nullptr);
        areg::Application::stop_logging();
#if AREG_LOGGING
        EXPECT_FALSE(areg::is_started());
#endif
        EXPECT_EQ(application_thread(APP_LOG_THREAD), nullptr);
        areg::Application::stop_logging();
    }
}

TEST_F(ApplicationSetupTest, logging_started_before_setup_is_kept_until_release)
{
    const std::string path{ mConfigFile.string() };
    ASSERT_TRUE(areg::Application::load_configuration(path.c_str()));
    mNeedsRelease = true;
    ASSERT_TRUE(areg::Application::start_logging());
    auto* logger{ application_thread(APP_LOG_THREAD) };
#if AREG_LOGGING
    ASSERT_NE(logger, nullptr);
#endif
    EXPECT_EQ(areg::Application::system_state(), areg::AppState::Stopped);
    EXPECT_EQ(application_thread(APP_TIMER_THREAD), nullptr);
    setup_application(false, false, false, true, false);
    EXPECT_EQ(application_thread(APP_LOG_THREAD), logger);
#if AREG_LOGGING
    EXPECT_TRUE(areg::is_started());
#endif
    EXPECT_NE(application_thread(APP_TIMER_THREAD), nullptr);
    release_application();
}

TEST_F(ApplicationSetupTest, router_requires_servicing_and_rejects_disabled_or_empty_addresses)
{
    setup_application();
    auto& config{ areg::Application::config_manager() };
    EXPECT_EQ(areg::Application::router_state(), areg::RouterState::NotConfigured);
    EXPECT_FALSE(areg::Application::connect_router());
    config.set_service_enable(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip, true, true);
    config.set_service_address(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip, "127.0.0.1", true);
    EXPECT_EQ(areg::Application::router_state(), areg::RouterState::Disconnected);
    EXPECT_FALSE(areg::Application::connect_router());
    config.set_service_address(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip, "", true);
    EXPECT_EQ(areg::Application::router_state(), areg::RouterState::NotConfigured);
    EXPECT_FALSE(areg::Application::connect_router());
}

TEST_F(ApplicationSetupTest, servicing_rejects_router_without_configuration_and_keeps_local_services_running)
{
    setup_application(false, true, false, false, false);
    EXPECT_FALSE(areg::Application::connect_router());
    EXPECT_EQ(areg::Application::router_state(), areg::RouterState::NotConfigured);
    auto& config{ areg::Application::config_manager() };
    config.set_service_enable(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip, true, true);
    config.set_service_address(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip, "", true);
    EXPECT_FALSE(areg::Application::connect_router());
    EXPECT_NE(application_thread(APP_SERVICE_THREAD), nullptr);
    EXPECT_NE(application_thread(APP_TIMER_THREAD), nullptr);
    EXPECT_EQ(areg::Application::system_state(), areg::AppState::Ready);
}

TEST_F(ApplicationSetupTest, inactive_router_counters_are_overwritten_and_reset_to_zero)
{
    setup_application(false, true, false, false, false);
    for (bool enabled : { false, true, true, false })
    {
        areg::Application::enable_data_rate(enabled);
        for (uint32_t query = 0u; query < 2u; ++query)
        {
            uint64_t bytesSent{ UINT64_MAX };
            uint64_t bytesReceived{ UINT64_MAX };
            uint32_t messagesSent{ UINT32_MAX };
            uint32_t messagesReceived{ UINT32_MAX };
            areg::Application::query_data_sent(bytesSent, messagesSent);
            areg::Application::query_data_received(bytesReceived, messagesReceived);
            EXPECT_EQ(bytesSent, 0u);
            EXPECT_EQ(bytesReceived, 0u);
            EXPECT_EQ(messagesSent, 0u);
            EXPECT_EQ(messagesReceived, 0u);
        }
    }
}

TEST_F(ApplicationSetupTest, setup_resets_an_earlier_quit_signal_and_signal_stays_set_until_exit)
{
    areg::Application::signal_quit();
    EXPECT_TRUE(areg::Application::wait_quit(0u));
    setup_application();
    EXPECT_FALSE(areg::Application::wait_quit(0u));
    areg::Application::signal_quit();
    for (uint32_t query = 0u; query < 4u; ++query)
    {
        EXPECT_TRUE(areg::Application::wait_quit(0u));
    }
    EXPECT_EQ(areg::Application::system_state(), areg::AppState::Ready);
}

TEST_F(ApplicationSetupTest, one_quit_signal_releases_all_waiters_without_consuming_the_signal)
{
    setup_application();
    std::array<std::promise<void>, 4> entered;
    std::array<std::future<void>, 4> ready;
    std::array<std::future<bool>, 4> returned;
    std::array<std::thread, 4> waiters;
    for (uint32_t index = 0u; index < waiters.size(); ++index)
    {
        ready[index] = entered[index].get_future();
        std::promise<bool> result;
        returned[index] = result.get_future();
        waiters[index] = std::thread([&, index, result = std::move(result)]() mutable
        {
            entered[index].set_value();
            result.set_value(areg::Application::wait_quit(APP_WAIT_MS));
        });
    }
    for (auto& future : ready)
    {
        EXPECT_EQ(future.wait_for(APP_WAIT), std::future_status::ready);
    }
    areg::Application::signal_quit();
    for (uint32_t index = 0u; index < waiters.size(); ++index)
    {
        const auto status{ returned[index].wait_for(APP_WAIT) };
        EXPECT_EQ(status, std::future_status::ready);
        waiters[index].join();
        EXPECT_TRUE(returned[index].get());
    }
    EXPECT_TRUE(areg::Application::wait_quit(0u));
}

TEST_F(ApplicationSetupTest, process_and_machine_identity_are_stable_public_values)
{
    const auto& process{ areg::Process::instance() };
    EXPECT_EQ(areg::Application::application_name(), process.app_name());
    EXPECT_EQ(&areg::Application::application_name(), &areg::Application::application_name());
    EXPECT_FALSE(areg::Application::machine_name().is_empty());
    EXPECT_EQ(&areg::Application::machine_name(), &areg::Application::machine_name());
    EXPECT_EQ(&areg::Application::config_manager(), &areg::Application::config_manager());
}

TEST_F(ApplicationSetupTest, unknown_model_and_thread_queries_do_not_create_services)
{
    constexpr const char* MISSING_MODEL{ "ApplicationSetupUnregisteredModel" };
    EXPECT_FALSE(areg::Application::is_model_loaded(MISSING_MODEL));
    EXPECT_EQ(areg::Application::model_state(MISSING_MODEL), areg::ModelState::Unloaded);
    EXPECT_EQ(areg::Application::thread_state("ApplicationSetupUnregisteredThread"), areg::ThreadState::Unknown);
    EXPECT_EQ(areg::Application::system_state(), areg::AppState::Stopped);
    expect_application_threads_stopped();
}

TEST_F(ApplicationSetupTest, working_directory_accepts_explicit_empty_and_null_paths)
{
    const std::string directory{ mDirectory.string() };
    areg::Application::set_working_directory(directory.c_str());
    EXPECT_EQ(std::filesystem::path(areg::File::current_dir().as_string()), mDirectory);
    areg::Application::set_working_directory("");
    const std::filesystem::path executable{ areg::Process::instance().path().as_string() };
    EXPECT_EQ(std::filesystem::path(areg::File::current_dir().as_string()), executable);
    areg::Application::set_working_directory(directory.c_str());
    areg::Application::set_working_directory(nullptr);
    EXPECT_EQ(std::filesystem::path(areg::File::current_dir().as_string()), executable);
}

TEST_F(ApplicationSetupTest, setup_changes_the_working_directory_to_the_executable)
{
    const std::string directory{ mDirectory.string() };
    areg::Application::set_working_directory(directory.c_str());
    setup_application();
    EXPECT_EQ(std::filesystem::path(areg::File::current_dir().as_string())
            , std::filesystem::path(areg::Process::instance().path().as_string()));
}

TEST_F(ApplicationSetupTest, storage_missing_queries_do_not_insert_and_replacement_returns_the_previous_value)
{
    const areg::String key{ "ApplicationSetupStoragePrevious" };
    EXPECT_FALSE(areg::Application::is_element_stored(key));
    EXPECT_EQ(areg::Application::stored_element(key), areg::InvalidElement);
    EXPECT_FALSE(areg::Application::is_element_stored(key));
    EXPECT_EQ(areg::Application::store_element(key, storage_value(17u)), areg::InvalidElement);
    EXPECT_TRUE(areg::Application::is_element_stored(key));
    EXPECT_EQ(areg::Application::stored_element(key), storage_value(17u));
    EXPECT_EQ(areg::Application::store_element(key, storage_value(99u)), storage_value(17u));
    EXPECT_EQ(areg::Application::store_element(key, areg::InvalidElement), storage_value(99u));
    EXPECT_TRUE(areg::Application::is_element_stored(key));
    EXPECT_EQ(areg::Application::stored_element(key), areg::InvalidElement);
}

TEST_F(ApplicationSetupTest, storage_preserves_empty_case_distinct_and_many_independent_names)
{
    const std::array<areg::String, 3> names{ areg::String(), areg::String("ApplicationSetupKey"), areg::String("applicationsetupkey") };
    for (uint32_t index = 0u; index < names.size(); ++index)
    {
        EXPECT_EQ(areg::Application::store_element(names[index], storage_value(index + 1u)), areg::InvalidElement);
    }
    for (uint32_t index = 0u; index < 256u; ++index)
    {
        const areg::String name{ std::string("ApplicationSetupMany_") + std::to_string(index) };
        EXPECT_EQ(areg::Application::store_element(name, storage_value(index + 100u)), areg::InvalidElement);
    }
    for (uint32_t index = 0u; index < 256u; ++index)
    {
        const areg::String name{ std::string("ApplicationSetupMany_") + std::to_string(index) };
        EXPECT_TRUE(areg::Application::is_element_stored(name));
        EXPECT_EQ(areg::Application::stored_element(name), storage_value(index + 100u));
    }
    for (uint32_t index = 0u; index < names.size(); ++index)
    {
        EXPECT_EQ(areg::Application::stored_element(names[index]), storage_value(index + 1u));
    }
}

TEST_F(ApplicationSetupTest, storage_retains_caller_owned_pointer_identity)
{
    int32_t first{ 7 };
    int32_t second{ 11 };
    areg::Primitive value{};
    value.valPtr.mElement = &first;
    const areg::String key{ "ApplicationSetupCallerOwned" };
    EXPECT_EQ(areg::Application::store_element(key, value), areg::InvalidElement);
    EXPECT_EQ(areg::Application::stored_element(key).valPtr.mElement, &first);
    value.valPtr.mElement = &second;
    EXPECT_EQ(areg::Application::store_element(key, value).valPtr.mElement, &first);
    EXPECT_EQ(areg::Application::store_element(key, areg::InvalidElement).valPtr.mElement, &second);
    EXPECT_EQ(first, 7);
    EXPECT_EQ(second, 11);
}

TEST_F(ApplicationSetupTest, concurrent_storage_replacements_form_one_complete_lossless_history)
{
    constexpr uint32_t WRITERS{ 4u };
    constexpr uint32_t PER_WRITER{ 128u };
    const areg::String key{ "ApplicationSetupConcurrentHistory" };
    EXPECT_EQ(areg::Application::store_element(key, storage_value(0u)), areg::InvalidElement);
    std::promise<void> release;
    const std::shared_future<void> start{ release.get_future().share() };
    std::array<std::thread, WRITERS> writers;
    std::array<std::vector<uint64_t>, WRITERS> previous;
    for (uint32_t index = 0u; index < WRITERS; ++index)
    {
        writers[index] = std::thread([&, index]()
        {
            previous[index].reserve(PER_WRITER);
            start.wait();
            for (uint32_t item = 0u; item < PER_WRITER; ++item)
            {
                const uint64_t value{ 1u + index * PER_WRITER + item };
                previous[index].push_back(areg::Application::store_element(key, storage_value(value)).valUInt64.mElement);
            }
        });
    }
    release.set_value();
    for (auto& writer : writers)
    {
        writer.join();
    }
    std::vector<uint64_t> history;
    for (uint32_t index = 0u; index < WRITERS; ++index)
    {
        ASSERT_EQ(previous[index].size(), PER_WRITER);
        history.insert(history.end(), previous[index].begin(), previous[index].end());
    }
    history.push_back(areg::Application::stored_element(key).valUInt64.mElement);
    std::sort(history.begin(), history.end());
    ASSERT_EQ(history.size(), WRITERS * PER_WRITER + 1u);
    for (uint32_t index = 0u; index < history.size(); ++index)
    {
        EXPECT_EQ(history[index], index) << index;
    }
}

TEST_F(ApplicationSetupTest, explicit_configuration_load_and_save_forward_the_same_listener_in_order)
{
    const std::string config{ mConfigFile.string() };
    const std::string original{ read_file(mConfigFile) };
    EXPECT_FALSE(areg::Application::is_configured());
    ASSERT_TRUE(areg::Application::load_configuration(config.c_str(), &mListener));
    EXPECT_EQ(mListener.mEvents, (std::vector<std::string>{ "prepare_read", "post_read" }));
    areg::Application::config_manager().set_file_location("logs/application-custom.log");
    const std::string destination{ (mDirectory / "saved.init").string() };
    ASSERT_TRUE(areg::Application::save_configuration(destination.c_str(), &mListener));
    EXPECT_EQ(mListener.mEvents, (std::vector<std::string>{ "prepare_read", "post_read", "prepare_save", "post_save" }));
    EXPECT_EQ(read_file(mConfigFile), original);
    EXPECT_NE(read_file(destination).find("logs/application-custom.log"), std::string::npos);
    for (auto* manager : mListener.mManagers)
    {
        EXPECT_EQ(manager, &areg::Application::config_manager());
    }
    EXPECT_EQ(mListener.mStates, std::vector<areg::AppState>(4u, areg::AppState::Stopped));
}

TEST_F(ApplicationSetupTest, saving_the_shared_configuration_is_rejected_without_listener_notifications)
{
    const std::string config{ mConfigFile.string() };
    ASSERT_TRUE(areg::Application::load_configuration(config.c_str(), &mListener));
    const std::string original{ read_file(mConfigFile) };
    ASSERT_FALSE(areg::Application::save_configuration(config.c_str(), &mListener));
    EXPECT_EQ(mListener.mEvents, (std::vector<std::string>{ "prepare_read", "post_read" }));
    EXPECT_EQ(read_file(mConfigFile), original);
}

TEST_F(ApplicationSetupTest, null_save_path_writes_only_the_process_configuration)
{
    const std::string config{ mConfigFile.string() };
    ASSERT_TRUE(areg::Application::load_configuration(config.c_str()));
    const std::string original{ read_file(mConfigFile) };
    areg::Application::config_manager().set_file_location("logs/process-only.log");
    ASSERT_TRUE(areg::Application::save_configuration(nullptr, &mListener));
    EXPECT_EQ(mListener.mEvents, (std::vector<std::string>{ "prepare_save", "post_save" }));
    const auto processFile{ mDirectory / (std::string(areg::Process::instance().app_name().as_string()) + ".init") };
    EXPECT_NE(read_file(processFile).find("logs/process-only.log"), std::string::npos);
    EXPECT_EQ(read_file(mConfigFile), original);
}

TEST_F(ApplicationSetupTest, missing_configuration_reports_failure_but_installs_complete_defaults)
{
    const std::string missing{ (mDirectory / "missing.init").string() };
    EXPECT_FALSE(areg::Application::load_configuration(missing.c_str(), &mListener));
    EXPECT_EQ(mListener.mEvents, (std::vector<std::string>{ "setup_defaults" }));
    expect_default_configuration();
    EXPECT_EQ(areg::Application::system_state(), areg::AppState::Stopped);
    expect_application_threads_stopped();
}

TEST_F(ApplicationSetupTest, empty_configuration_falls_back_after_read_notifications)
{
    write_file(mConfigFile, "");
    const std::string config{ mConfigFile.string() };
    EXPECT_FALSE(areg::Application::load_configuration(config.c_str(), &mListener));
    EXPECT_EQ(mListener.mEvents, (std::vector<std::string>{ "prepare_read", "post_read", "setup_defaults" }));
    expect_default_configuration();
}

TEST_F(ApplicationSetupTest, missing_file_setup_notifies_defaults_while_initializing_and_still_becomes_ready)
{
    mConfigFile = mDirectory / "not-present.init";
    setup_application(false, false, false, true, false);
    EXPECT_EQ(mListener.mEvents, (std::vector<std::string>{ "setup_defaults" }));
    EXPECT_EQ(mListener.mStates, (std::vector<areg::AppState>{ areg::AppState::Initializing }));
    EXPECT_EQ(mListener.mAvailable, (std::vector<bool>{ true }));
    EXPECT_EQ(mListener.mReady, (std::vector<bool>{ false }));
    EXPECT_EQ(areg::Application::system_state(), areg::AppState::Ready);
    areg::ConfigManager defaults;
    defaults.set_configuration(areg::Application::config_manager().readonly_properties(), areg::ListProperties());
    expect_default_configuration(defaults);
    EXPECT_NE(application_thread(APP_TIMER_THREAD), nullptr);
}

TEST_F(ApplicationSetupTest, default_configuration_discards_overrides_and_notifies_once_per_reset)
{
    const std::string config{ mConfigFile.string() };
    ASSERT_TRUE(areg::Application::load_configuration(config.c_str()));
    auto& manager{ areg::Application::config_manager() };
    manager.set_service_port(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip, 123u, true);
    for (uint32_t reset = 0u; reset < 2u; ++reset)
    {
        areg::Application::setup_default_configuration(&mListener);
        expect_default_configuration();
    }
    EXPECT_EQ(mListener.mEvents, (std::vector<std::string>{ "setup_defaults", "setup_defaults" }));
}

TEST_F(ApplicationSetupTest, configured_application_keeps_first_file_and_does_not_notify_a_second_listener)
{
    const std::string config{ mConfigFile.string() };
    ASSERT_TRUE(areg::Application::load_configuration(config.c_str(), &mListener));
    const std::string missing{ (mDirectory / "not-loaded.init").string() };
    ApplicationSetupListener second;
    EXPECT_TRUE(areg::Application::load_configuration(missing.c_str(), &second));
    EXPECT_TRUE(second.mEvents.empty());
    EXPECT_EQ(mListener.mEvents, (std::vector<std::string>{ "prepare_read", "post_read" }));
    EXPECT_EQ(std::filesystem::path(areg::Application::config_manager().config_file().as_string()), mConfigFile);
    EXPECT_FALSE(areg::Application::config_manager().remote_service_enable(
        areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip));
}

TEST_F(ApplicationSetupTest, null_configuration_path_uses_defaults_or_the_executable_shared_file)
{
    mNeedsRelease = true;
    areg::Application::setup(false, false, false, false, false, nullptr, &mListener);
    EXPECT_TRUE(areg::Application::is_configured());
    EXPECT_EQ(areg::Application::system_state(), areg::AppState::Ready);
    EXPECT_FALSE(mListener.mEvents.empty());
    expect_application_threads_stopped();
}

TEST_F(ApplicationSetupTest, empty_configuration_path_uses_defaults_or_the_executable_shared_file)
{
    mNeedsRelease = true;
    areg::Application::setup(false, false, false, false, false, "", &mListener);
    EXPECT_TRUE(areg::Application::is_configured());
    EXPECT_EQ(areg::Application::system_state(), areg::AppState::Ready);
    EXPECT_FALSE(mListener.mEvents.empty());
    expect_application_threads_stopped();
}
