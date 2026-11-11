/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/ComponentLifecycleFailureTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests of a model whose component is not created,
 *              of the components that do not handle the user state, of a worker
 *              thread asked for twice, and of the runtime identity of the threads
 *              and components the system runs.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "units/EventTestSupport.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/base/RuntimeObject.hpp"
#include "areg/base/String.hpp"
#include "areg/base/SyncPrimitives.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/ThreadAddress.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/DispatcherThread.hpp"
#include "areg/component/WorkerThread.hpp"
#include "areg/component/WorkerThreadConsumer.hpp"

#include <atomic>
#include <initializer_list>
#include <string_view>

namespace
{
    constexpr uint32_t          WAIT_MS         { 30'000 };

    constexpr char const        _refusedModel[] { "ClRefusedModel" };
    constexpr char const        _stateModel[]   { "ClStateModel" };
    constexpr char const        _workerModel[]  { "ClWorkerModel" };
    constexpr char const        _workerName[]   { "ClWorker" };
    constexpr char const        _takenModel[]   { "ClTakenModel" };
    constexpr char const        _takenThread[]  { "ClTakenThread" };

    //!< When set, the create function of the refused role returns no component.
    std::atomic_bool            gRefuse         { true };
    //!< The calls of the create function of the refused role.
    std::atomic_uint32_t        gCreateCalls    { 0 };
    //!< The components the delete function of the refused role destroyed.
    std::atomic_uint32_t        gDeleted        { 0 };
    //!< Signaled when the delete function of the refused role destroyed a component.
    areg::SyncEvent             gDeletedEvent   { true, false };
    //!< The user states the recording component got, previous and current.
    std::atomic_uint32_t        gPrevious       { 0 };
    std::atomic_uint32_t        gCurrent        { 0 };
    //!< Signaled when the recording component got a user state.
    areg::SyncEvent             gStateSeen      { true, false };
    //!< The workers the second call of create_worker_thread returned, and the first.
    std::atomic<areg::WorkerThread *> gFirstWorker  { nullptr };
    std::atomic<areg::WorkerThread *> gSecondWorker { nullptr };
    //!< The workers that were running when the component stopped.
    std::atomic_bool            gWorkerRan      { false };
    //!< The runtime identity checks a component did of itself on its thread.
    std::atomic_bool            gSelfIdentity   { false };
    std::atomic_bool            gThreadIdentity { false };

    //! A runtime class unrelated to every class of the system.
    class ClStranger : public areg::RuntimeObject
    {
    public:
        AREG_DECLARE_RUNTIME(ClStranger)
    };

    AREG_IMPLEMENT_RUNTIME(ClStranger, areg::RuntimeObject)

    //! True if the object knows its own class and its base in every form, and not a stranger.
    bool knows_its_identity(const areg::RuntimeObject & obj)
    {
        const areg::RuntimeClassID & own{ obj.class_id() };
        const std::string_view name{ own.name() };
        const areg::String text(name);
        const ClStranger stranger;
        const areg::RuntimeClassID & other{ stranger.class_id() };
        return (obj.class_number() == own.magic())
            && (obj.class_name() == name)
            && (std::string_view(obj.class_string()) == name)
            && obj.is_runtime(own) && obj.is_runtime(own.magic())
            && obj.is_runtime(text.as_string()) && obj.is_runtime(name) && obj.is_runtime(text)
            && obj.is_runtime(areg::RuntimeObject::CLASS_ID)
            && obj.is_runtime(areg::RuntimeObject::CLASS_ID.magic())
            && obj.is_runtime(areg::RuntimeObject::CLASS_ID.name())
            && obj.is_runtime("RuntimeObject")
            && obj.is_runtime(areg::String("RuntimeObject"))
            && (obj.is_runtime(other) == false) && (obj.is_runtime(other.magic()) == false)
            && (obj.is_runtime(other.name()) == false) && (obj.is_runtime("ClStranger") == false)
            && (obj.is_runtime(areg::String("ClStranger")) == false);
    }

    //! A component that does nothing of its own.
    class ClPlain : public areg::Component
    {
    public:
        ClPlain(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
            : areg::Component(entry, owner)
        { }
    };

    //! Creates a plain component, or none while gRefuse is set.
    areg::Component * create_refused(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    {
        gCreateCalls.fetch_add(1u);
        return (gRefuse.load() ? nullptr : new ClPlain(entry, owner));
    }

    //! Deletes the component and counts it.
    void delete_refused(areg::Component & comp, const areg::ComponentEntry & /*entry*/)
    {
        delete &comp;
        gDeleted.fetch_add(1u);
        gDeletedEvent.set_signaled();
    }

    //! A component that records the user state it gets.
    class ClRecorder : public areg::Component
    {
    public:
        ClRecorder(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
            : areg::Component(entry, owner)
        { }

    protected:
        void on_user_state(uint32_t previous, uint32_t current) override
        {
            gPrevious.store(previous);
            gCurrent.store(current);
            gStateSeen.set_signaled();
        }
    };

    //! A worker consumer that does nothing.
    class ClConsumer final : public areg::WorkerThreadConsumer
    {
    public:
        ClConsumer()
            : areg::WorkerThreadConsumer(areg::String(_workerName))
        { }
    };

    //! A component that asks twice for the same worker thread on its startup.
    class ClWorkerHost : public areg::Component
    {
    public:
        ClWorkerHost(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
            : areg::Component(entry, owner)
        { }

    protected:
        void startup_component(areg::ComponentThread & comThread) override
        {
            areg::Component::startup_component(comThread);
            gSelfIdentity.store(knows_its_identity(*this));
            gThreadIdentity.store(knows_its_identity(comThread));
            const areg::String name(_workerName);
            gFirstWorker.store(create_worker_thread(name, mConsumer, comThread));
            gSecondWorker.store(create_worker_thread(name, mConsumer, comThread));
        }

        void shutdown_component(areg::ComponentThread & comThread) override
        {
            areg::WorkerThread * worker{ gFirstWorker.load() };
            gWorkerRan.store((worker != nullptr) && worker->is_running() && knows_its_identity(*worker));
            delete_worker_thread(areg::String(_workerName));
            areg::Component::shutdown_component(comThread);
        }

    private:
        ClConsumer  mConsumer;
    };

    //! Creates a component of the class.
    template <class Comp>
    areg::Component * create_of(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    {
        return new Comp(entry, owner);
    }

    //! Deletes a component.
    void delete_of(areg::Component & comp, const areg::ComponentEntry & /*entry*/)
    {
        delete &comp;
    }

    //! Returns a thread entry with the components of the roles.
    areg::ComponentThreadEntry thread_of(const char * thread, std::initializer_list<areg::ComponentEntry> components)
    {
        areg::ComponentThreadEntry entry{ areg::String(thread) };
        for (const areg::ComponentEntry & comp : components)
        {
            entry.add_component(comp);
        }

        return entry;
    }

    //! Registers the models of this file once.
    void register_models()
    {
        static const bool _registered{ [] {
            const areg::String refused("ClRefusedThread"), state("ClStateThread"), worker("ClWorkerThread"), taken(_takenThread);
            areg::Model refusedModel(_refusedModel), stateModel(_stateModel), workerModel(_workerModel), takenModel(_takenModel);
            takenModel.add_thread(thread_of(_takenThread,
                { areg::ComponentEntry(taken, areg::String("ClTakenPlain"), &create_of<ClPlain>, &delete_of) }));
            refusedModel.add_thread(thread_of("ClRefusedThread",
                { areg::ComponentEntry(refused, areg::String("ClRefused"), &create_refused, &delete_refused) }));
            stateModel.add_thread(thread_of("ClStateThread",
                { areg::ComponentEntry(state, areg::String("ClStatePlain"), &create_of<ClPlain>, &delete_of)
                , areg::ComponentEntry(state, areg::String("ClStateRecorder"), &create_of<ClRecorder>, &delete_of) }));
            workerModel.add_thread(thread_of("ClWorkerThread",
                { areg::ComponentEntry(worker, areg::String("ClWorkerHost"), &create_of<ClWorkerHost>, &delete_of) }));
            return areg::ComponentLoader::add_model_unique(refusedModel)
                && areg::ComponentLoader::add_model_unique(stateModel)
                && areg::ComponentLoader::add_model_unique(workerModel)
                && areg::ComponentLoader::add_model_unique(takenModel);
        }() };
        ASSERT_TRUE(_registered);
    }

    //! Registers the models, starts the application without logging and routing, and stops it.
    class ClSystem
    {
    public:
        ClSystem()
        {
            register_models();
            areg::Application::setup(false, true, false, true, false);
        }

        ~ClSystem()
        {
            areg::Application::unload_model(nullptr);
            areg::Application::release();
        }
    };
}

/**
 * \brief   A model whose only component is not created fails its waiting load and is left
 *          unloaded; once the component is created, the same model loads, and its unload
 *          destroys the component.
 **/
TEST(ComponentLifecycleFailureTest, model_whose_component_is_not_created_fails_to_load_and_loads_once_it_is)
{
    ClSystem system;
    gRefuse.store(true);
    gCreateCalls.store(0u);
    gDeleted.store(0u);

    EXPECT_FALSE(areg::Application::load_model(_refusedModel, true));
    EXPECT_EQ(gCreateCalls.load(), 1u);
    EXPECT_FALSE(areg::Application::is_model_loaded(_refusedModel));
    EXPECT_EQ(areg::Application::model_state(_refusedModel), areg::ModelState::Unloaded);
    EXPECT_EQ(areg::Thread::find_by_address(areg::ThreadAddress(areg::String("ClRefusedThread"))), nullptr);

    gRefuse.store(false);
    ASSERT_TRUE(areg::Application::load_model(_refusedModel, true));
    EXPECT_EQ(gCreateCalls.load(), 2u);
    EXPECT_TRUE(areg::Application::is_model_loaded(_refusedModel));

    areg::Application::unload_model(_refusedModel);
    EXPECT_EQ(areg::Application::model_state(_refusedModel), areg::ModelState::Unloaded);
    EXPECT_EQ(gDeleted.load(), 1u);
}

/**
 * \brief   A user state change reaches every component of a thread: a component that does not
 *          handle it does not stop the one after it from getting the previous and the new state.
 **/
TEST(ComponentLifecycleFailureTest, user_state_reaches_the_handling_component_after_one_that_does_not_handle_it)
{
    ClSystem system;
    ASSERT_TRUE(areg::Application::load_model(_stateModel, true));
    const uint32_t previous{ areg::Application::user_state() };
    gStateSeen.reset();

    areg::Application::set_user_state(previous + 7u);
    ASSERT_TRUE(gStateSeen.lock(WAIT_MS));
    EXPECT_EQ(gPrevious.load(), previous);
    EXPECT_EQ(gCurrent.load(), previous + 7u);
    EXPECT_EQ(areg::Application::user_state(), previous + 7u);

    areg::Application::set_user_state(previous);
}

/**
 * \brief   A worker thread asked for twice by name is created once: the second call returns the
 *          running worker. The component, its thread and the worker know their runtime class
 *          and its base, and not a stranger.
 **/
TEST(ComponentLifecycleFailureTest, worker_asked_for_twice_is_the_same_running_worker)
{
    gFirstWorker.store(nullptr);
    gSecondWorker.store(nullptr);
    gWorkerRan.store(false);
    {
        ClSystem system;
        ASSERT_TRUE(areg::Application::load_model(_workerModel, true));
        ASSERT_NE(gFirstWorker.load(), nullptr);
        EXPECT_EQ(gSecondWorker.load(), gFirstWorker.load());
        EXPECT_TRUE(gSelfIdentity.load());
        EXPECT_TRUE(gThreadIdentity.load());
        areg::Application::unload_model(_workerModel);
    }

    EXPECT_TRUE(gWorkerRan.load());
}

/**
 * \brief   The system threads know their runtime class and its base, and not a stranger, and
 *          the current component outside a component thread is none.
 **/
TEST(ComponentLifecycleFailureTest, system_threads_know_their_runtime_identity)
{
    ClSystem system;
    EXPECT_EQ(areg::ComponentThread::current_component(), nullptr);

    const char * const names[]{ "_AREG_SERVICE_MANAGER_THREAD_", "_AREG_TIMER_THREAD_NAME_" };
    for (const char * name : names)
    {
        areg::Thread * thread{ areg::Thread::find_by_address(areg::ThreadAddress(areg::String(name))) };
        ASSERT_NE(thread, nullptr) << name;
        EXPECT_TRUE(knows_its_identity(*thread)) << name;
        EXPECT_TRUE(thread->is_runtime(areg::DispatcherThread::CLASS_ID)) << name;
    }

    const areg::DispatcherThread & none{ areg::DispatcherThread::current_dispatcher_thread() };
    EXPECT_TRUE(knows_its_identity(none));
}

/**
 * \brief   A user state set while the system does not run is kept, and the system reports it.
 **/
TEST(ComponentLifecycleFailureTest, user_state_set_while_the_system_does_not_run_is_kept)
{
    const uint32_t previous{ areg::Application::user_state() };
    areg::Application::set_user_state(previous + 11u);
    EXPECT_EQ(areg::Application::user_state(), previous + 11u);
    areg::Application::set_user_state(previous);
    EXPECT_EQ(areg::Application::user_state(), previous);
}

/**
 * \brief   Reproduces the defect: a model whose thread name was taken by a running thread
 *          reported a successful load without its components. Now the load fails, and the
 *          thread that runs is left running.
 **/
TEST(ComponentLifecycleFailureTest, model_whose_thread_name_is_taken_fails_to_load)
{
    ClSystem system;
    EventTestDispatcher other{ areg::String(_takenThread) };
    ASSERT_TRUE(other.start_ready());

    EXPECT_FALSE(areg::Application::load_model(_takenModel, true));
    EXPECT_FALSE(areg::Application::is_model_loaded(_takenModel));
    EXPECT_TRUE(other.is_running());
    EXPECT_TRUE(other.barrier());
}

/**
 * \brief   A component thread that ends without being asked destroys its components; its model
 *          can still be unloaded, and loads again with a new thread.
 **/
TEST(ComponentLifecycleFailureTest, thread_that_ends_unasked_leaves_its_model_unloadable_and_loadable)
{
    ClSystem system;
    gRefuse.store(false);
    gDeleted.store(0u);
    gDeletedEvent.reset();
    ASSERT_TRUE(areg::Application::load_model(_refusedModel, true));

    areg::DispatcherThread & thread{ areg::DispatcherThread::dispatcher_thread(areg::ThreadAddress(areg::String("ClRefusedThread"))) };
    ASSERT_TRUE(thread.is_valid());
    thread.trigger_exit();
    ASSERT_TRUE(gDeletedEvent.lock(WAIT_MS));
    EXPECT_EQ(gDeleted.load(), 1u);

    areg::Application::unload_model(_refusedModel);
    EXPECT_EQ(areg::Application::model_state(_refusedModel), areg::ModelState::Unloaded);
    EXPECT_EQ(gDeleted.load(), 1u);

    ASSERT_TRUE(areg::Application::load_model(_refusedModel, true));
    EXPECT_TRUE(areg::Application::is_model_loaded(_refusedModel));
    EXPECT_NE(areg::Thread::find_by_address(areg::ThreadAddress(areg::String("ClRefusedThread"))), nullptr);
    areg::Application::unload_model(_refusedModel);
    EXPECT_EQ(gDeleted.load(), 2u);
}
