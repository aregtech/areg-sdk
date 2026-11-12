/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/ComponentSelfLifecycleTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests of a component thread that shuts itself down, and of
 *              a component that loads or unloads models from its own thread.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "units/EventTestSupport.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/base/String.hpp"
#include "areg/base/SyncPrimitives.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/ThreadAddress.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/WorkerThread.hpp"
#include "areg/component/WorkerThreadConsumer.hpp"

#include <atomic>
#include <functional>

namespace
{
    constexpr uint32_t      WAIT_MS         { 30'000 };
    constexpr uint32_t      WATCHDOG_MS     { 60'000 };

    constexpr char const    _selfModel[]    { "CsSelfModel" };
    constexpr char const    _selfThread[]   { "CsSelfThread" };
    constexpr char const    _selfWorker[]   { "CsSelfWorker" };
    constexpr char const    _loadModel[]    { "CsLoadModel" };
    constexpr char const    _loadThread[]   { "CsLoadThread" };

    class CsHost;

    //!< The components the delete function destroyed.
    std::atomic_uint32_t    gDeleted        { 0 };
    //!< Signaled when the delete function destroyed a component.
    areg::SyncEvent         gDeletedEvent   { true, false };
    //!< The components the create function created.
    std::atomic_uint32_t    gCreated        { 0 };
    //!< The last host component that started.
    std::atomic<CsHost *>   gHost           { nullptr };
    //!< The worker thread of the last host component that started.
    std::atomic<areg::WorkerThread *> gWorker{ nullptr };
    //!< When set, the host loads its own model in its startup.
    std::atomic_bool        gLoadOwnModel   { false };
    //!< The result of the load the host made in its startup.
    std::atomic_bool        gOwnLoadResult  { false };

    //! A worker consumer that does nothing.
    class CsConsumer final : public areg::WorkerThreadConsumer
    {
    public:
        CsConsumer()
            : areg::WorkerThreadConsumer(areg::String(_selfWorker))
        { }
    };

    //! A component with a worker thread that runs the actions sent to its thread.
    class CsHost : public areg::Component
                 , public LifecycleActionConsumer
    {
    public:
        CsHost(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
            : areg::Component         ( entry, owner )
            , LifecycleActionConsumer ( )
            , mConsumer               ( )
        { }

    protected:
        void startup_component(areg::ComponentThread & comThread) override
        {
            areg::Component::startup_component(comThread);
            static_cast<void>(LifecycleActionEvent::add_listener(static_cast<LifecycleActionConsumer &>(*this), comThread));
            gWorker.store(create_worker_thread(areg::String(_selfWorker), mConsumer, comThread));
            if (gLoadOwnModel.load())
            {
                gOwnLoadResult.store(areg::Application::load_model(_loadModel, true));
            }

            gHost.store(this);
        }

        void shutdown_component(areg::ComponentThread & comThread) override
        {
            gHost.store(nullptr);
            LifecycleActionEvent::remove_listener(static_cast<LifecycleActionConsumer &>(*this), comThread);
            areg::Component::shutdown_component(comThread);
        }

        void process_event(const LifecycleActionData & data) override
        {
            data.action();
        }

    private:
        CsConsumer  mConsumer;
    };

    areg::Component * create_host(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    {
        gCreated.fetch_add(1u);
        return new CsHost(entry, owner);
    }

    void delete_host(areg::Component & comp, const areg::ComponentEntry & /*entry*/)
    {
        delete &comp;
        gDeleted.fetch_add(1u);
        gDeletedEvent.set_signaled();
    }

    //! Registers the models of this file once.
    void register_models()
    {
        static const bool _registered{ [] {
            const areg::String self(_selfThread), load(_loadThread);
            areg::Model selfModel(_selfModel), loadModel(_loadModel);
            areg::ComponentThreadEntry selfEntry(self, WATCHDOG_MS);
            selfEntry.add_component(areg::ComponentEntry(self, areg::String("CsSelfHost"), &create_host, &delete_host));
            areg::ComponentThreadEntry loadEntry(load);
            loadEntry.add_component(areg::ComponentEntry(load, areg::String("CsLoadHost"), &create_host, &delete_host));
            selfModel.add_thread(selfEntry);
            loadModel.add_thread(loadEntry);
            return areg::ComponentLoader::add_model_unique(selfModel) && areg::ComponentLoader::add_model_unique(loadModel);
        }() };
        ASSERT_TRUE(_registered);
    }

    //! Starts the application without logging and routing, and stops it.
    class CsSystem
    {
    public:
        CsSystem()
        {
            register_models();
            gDeleted.store(0u);
            gCreated.store(0u);
            gDeletedEvent.reset();
            gHost.store(nullptr);
            gWorker.store(nullptr);
            gOwnLoadResult.store(false);
            areg::Application::setup(false, true, false, true, false);
        }

        ~CsSystem()
        {
            gLoadOwnModel.store(false);
            areg::Application::unload_model(nullptr);
            areg::Application::release();
        }
    };

    //! Returns the component thread of the name, or null.
    areg::ComponentThread * component_thread(const char * name)
    {
        areg::Thread * thread{ areg::Thread::find_by_address(areg::ThreadAddress(areg::String(name))) };
        return AREG_RUNTIME_CAST(thread, areg::ComponentThread);
    }

    //! Runs the action on the thread of the last host and waits until it ran.
    bool run_on_host(std::function<void()> action)
    {
        CsHost * host{ gHost.load() };
        if (host == nullptr)
            return false;

        areg::SyncEvent done{ true, false };
        const bool sent{ LifecycleActionEvent::send_event(LifecycleActionData{ [&action, &done]() { action(); done.set_signaled(); } }
                                                         , static_cast<LifecycleActionConsumer &>(*host)
                                                         , host->master_thread()) };
        return sent && done.lock(WAIT_MS);
    }
}

/**
 * \brief   A component thread knows the watchdog timeout of its model entry and reports the
 *          state Running once its components started.
 **/
TEST(ComponentSelfLifecycleTest, component_thread_reports_its_watchdog_timeout_and_running_state)
{
    CsSystem system;
    ASSERT_TRUE(areg::Application::load_model(_selfModel, true));
    areg::ComponentThread * thread{ component_thread(_selfThread) };
    ASSERT_NE(thread, nullptr);
    EXPECT_EQ(thread->watchdog_timeout(), WATCHDOG_MS);
    EXPECT_EQ(thread->lifecycle_state(), areg::ThreadState::Running);
    EXPECT_EQ(areg::Application::thread_state(_selfThread), areg::ThreadState::Running);
}

/**
 * \brief   A component thread that shuts itself down stops the worker threads of its components
 *          before the call returns, then leaves and destroys its components. Its model can be
 *          unloaded, and loads again with a new thread.
 **/
TEST(ComponentSelfLifecycleTest, component_thread_that_shuts_itself_down_stops_its_workers_and_its_model_loads_again)
{
    CsSystem system;
    ASSERT_TRUE(areg::Application::load_model(_selfModel, true));
    ASSERT_NE(gWorker.load(), nullptr);
    ASSERT_TRUE(gWorker.load()->is_running());

    std::atomic_bool workerRanAfter{ true };
    std::atomic_bool selfThread{ false };
    ASSERT_TRUE(run_on_host([&workerRanAfter, &selfThread]() {
        areg::ComponentThread & thread{ gHost.load()->master_thread() };
        selfThread.store(areg::Thread::current_thread() == static_cast<areg::Thread *>(&thread));
        static_cast<void>(thread.shutdown(areg::DO_NOT_WAIT));
        workerRanAfter.store(gWorker.load()->is_running());
    }));

    EXPECT_TRUE(selfThread.load());
    EXPECT_FALSE(workerRanAfter.load());
    ASSERT_TRUE(gDeletedEvent.lock(WAIT_MS));
    EXPECT_EQ(gDeleted.load(), 1u);

    areg::Application::unload_model(_selfModel);
    EXPECT_EQ(areg::Application::model_state(_selfModel), areg::ModelState::Unloaded);
    EXPECT_EQ(gDeleted.load(), 1u);

    ASSERT_TRUE(areg::Application::load_model(_selfModel, true));
    EXPECT_EQ(gCreated.load(), 2u);
    ASSERT_NE(gWorker.load(), nullptr);
    EXPECT_TRUE(gWorker.load()->is_running());
    areg::Application::unload_model(_selfModel);
    EXPECT_EQ(gDeleted.load(), 2u);
}

/**
 * \brief   A component that loads its own model in its startup does not wait for itself: the
 *          call returns true, the waiting load of the caller completes, and the model has one
 *          instance of its component.
 **/
TEST(ComponentSelfLifecycleTest, component_that_loads_its_own_model_in_its_startup_does_not_wait_for_itself)
{
    CsSystem system;
    gLoadOwnModel.store(true);
    ASSERT_TRUE(areg::Application::load_model(_loadModel, true));
    EXPECT_TRUE(gOwnLoadResult.load());
    EXPECT_TRUE(areg::Application::is_model_loaded(_loadModel));
    EXPECT_EQ(gCreated.load(), 1u);

    areg::Application::unload_model(_loadModel);
    EXPECT_EQ(areg::Application::model_state(_loadModel), areg::ModelState::Unloaded);
    EXPECT_EQ(gDeleted.load(), 1u);
}

/**
 * \brief   A component that unloads all models from its own thread does not wait for itself:
 *          the call returns true before the component is destroyed, and then every loaded
 *          model unloads.
 **/
TEST(ComponentSelfLifecycleTest, component_that_unloads_all_models_from_its_thread_does_not_wait_for_itself)
{
    CsSystem system;
    ASSERT_TRUE(areg::Application::load_model(_selfModel, true));
    ASSERT_TRUE(areg::Application::load_model(_loadModel, true));

    std::atomic_bool unloaded{ false };
    std::atomic_uint32_t deletedAtReturn{ 99u };
    ASSERT_TRUE(run_on_host([&unloaded, &deletedAtReturn]() {
        unloaded.store(areg::Application::unload_model(nullptr));
        deletedAtReturn.store(gDeleted.load());
    }));

    EXPECT_TRUE(unloaded.load());
    EXPECT_EQ(deletedAtReturn.load(), 0u);
    ASSERT_TRUE(areg::ComponentLoader::wait_model_unload(areg::String(_selfModel)));
    ASSERT_TRUE(areg::ComponentLoader::wait_model_unload(areg::String(_loadModel)));
    EXPECT_EQ(areg::Application::model_state(_selfModel), areg::ModelState::Unloaded);
    EXPECT_EQ(areg::Application::model_state(_loadModel), areg::ModelState::Unloaded);
    EXPECT_EQ(gDeleted.load(), 2u);
}
