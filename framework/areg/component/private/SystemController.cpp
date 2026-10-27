/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/component/private/SystemController.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, the owner of the models and of the component threads.
 *
 ************************************************************************/
#include "areg/component/private/SystemController.hpp"

#include "areg/base/CommonDefs.hpp"
#include "areg/base/Thread.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/ProxyBase.hpp"
#include "areg/component/StubBase.hpp"
#include "areg/component/WorkerThread.hpp"
#include "areg/component/private/ServiceManager.hpp"
#include "areg/component/private/Watchdog.hpp"
#include "areg/component/private/UserStateEvent.hpp"
#include "areg/logging/areg_log.h"

#include <algorithm>
#include <atomic>
#include <cstdlib>
#include <string_view>

namespace
{
    //!< The name of the deadline guard on the watchdog manager.
    constexpr std::string_view  DEADLINE_GUARD_NAME { "_AREG_LIFECYCLE_DEADLINE_" };

    //!< The time a thread whose watchdog expired with the policy Restart has to stop by itself.
    constexpr uint32_t          RESTART_GRACE_MS    { areg::TIMEOUT_500_MS };

    //!< The number of hung instances of one thread at which its next instance waits for one to return.
    constexpr uint32_t          MAX_HUNG_INSTANCES  { 2u };

    //!< The user state; areg defines no value.
    std::atomic<uint32_t>       _userState          { 0u };

    //!< Milliseconds from now until the time point, at least 1.
    inline uint32_t _ms_until( const areg::SystemController::Clock::time_point & when )
    {
        const auto now{ areg::SystemController::Clock::now() };
        if (when <= now)
            return 1u;

        const auto ms{ std::chrono::duration_cast<std::chrono::milliseconds>(when - now).count() + 1 };
        return static_cast<uint32_t>(std::min<int64_t>(ms, static_cast<int64_t>(areg::WAIT_INFINITE - 1u)));
    }

    inline areg::SystemController::Clock::time_point _after( uint32_t ms )
    {
        return (areg::SystemController::Clock::now() + std::chrono::milliseconds(ms));
    }
}

namespace areg {

DEF_LOG_SCOPE(areg_component_private_SystemController, load_model);
DEF_LOG_SCOPE(areg_component_private_SystemController, unload_model);
DEF_LOG_SCOPE(areg_component_private_SystemController, thread_stopped);
DEF_LOG_SCOPE(areg_component_private_SystemController, watchdog_expired);
DEF_LOG_SCOPE(areg_component_private_SystemController, _abandon_thread);
DEF_LOG_SCOPE(areg_component_private_SystemController, _exit_process);
DEF_LOG_SCOPE(areg_component_private_SystemController, _arm_deadline);
DEF_LOG_SCOPE(areg_component_private_SystemController, _finish_exit);
DEF_LOG_SCOPE(areg_component_private_SystemController, _apply_policy);
DEF_LOG_SCOPE(areg_component_private_SystemController, _free_thread);
DEF_LOG_SCOPE(areg_component_private_SystemController, _start_thread);
DEF_LOG_SCOPE(areg_component_private_SystemController, _start_load);
DEF_LOG_SCOPE(areg_component_private_SystemController, _start_instance);
DEF_LOG_SCOPE(areg_component_private_SystemController, _start_replacement);
DEF_LOG_SCOPE(areg_component_private_SystemController, _start_deferred);
DEF_LOG_SCOPE(areg_component_private_SystemController, deadline_expired);

//////////////////////////////////////////////////////////////////////////
// SystemController::Request class implementation
//////////////////////////////////////////////////////////////////////////

SystemController::Request::Request( bool waitComponents /*= false*/ )
    : mDone             ( true, false )
    , mResult           ( false )
    , mWaitComponents   ( waitComponents )
{
}

bool SystemController::Request::wait()
{
    mDone.lock(areg::WAIT_INFINITE);
    return mResult;
}

void SystemController::Request::complete( bool result )
{
    mResult = result;
    mDone.set_signaled();
}

//////////////////////////////////////////////////////////////////////////
// SystemController class implementation
//////////////////////////////////////////////////////////////////////////

SystemController::SystemController()
    : mThreads          ( )
    , mAbandoned        ( )
    , mPending          ( )
    , mFailedLoads      ( )
    , mUnloadAfterLoad  ( )
    , mDeferred         ( )
    , mGuard            ( )
    , mExiting          ( false )
    , mExitDeadline     ( )
    , mLock             ( )
{
}

SystemController::~SystemController()
{
    // Leaves behind a guard that stop() did not release.
    static_cast<void>(mGuard.release());
}

void SystemController::load_model( const String & modelName, const std::shared_ptr<Request> & request )
{
    LOG_SCOPE( areg_component_private_SystemController, load_model );

    std::vector<String> models;
    if (modelName.is_empty())
    {
        _model_names(models);
    }
    else if (_model_exists(modelName))
    {
        models.push_back(modelName);
    }

    bool accepted{ (mExiting == false) && ((models.empty() == false) || modelName.is_empty()) };
    for (const String & model : models)
    {
        if (accepted == false)
            break;

        switch (_model_state(model))
        {
        case areg::ModelState::Unloaded:
            _start_load(model);
            break;

        case areg::ModelState::Unloading:
            LOG_WARN("The model [ %s ] is unloading, the load is refused", model.as_string());
            accepted = false;
            break;

        default:
            break;
        }
    }

    if ((request != nullptr) && accepted && (request->wait_components() == false))
    {
        for (const String & model : models)
        {
            const areg::ModelState state{ _model_state(model) };
            accepted = accepted && ((state == areg::ModelState::Loading) || (state == areg::ModelState::Loaded));
        }

        request->complete(accepted);
    }
    else if (request != nullptr)
    {
        if (accepted)
        {
            mPending.push_back(Pending{ request, true, models });
        }
        else
        {
            request->complete(false);
        }
    }

    _check_models();
}

void SystemController::unload_model( const String & modelName, const std::shared_ptr<Request> & request )
{
    LOG_SCOPE( areg_component_private_SystemController, unload_model );

    std::vector<String> models;
    if (modelName.is_empty())
    {
        _model_names(models);
    }
    else if (_model_exists(modelName))
    {
        models.push_back(modelName);
    }

    for (const String & model : models)
    {
        switch (_model_state(model))
        {
        case areg::ModelState::Loaded:
            _start_unload(model);
            break;

        case areg::ModelState::Loading:
            if (std::find(mUnloadAfterLoad.begin(), mUnloadAfterLoad.end(), model) == mUnloadAfterLoad.end())
            {
                mUnloadAfterLoad.push_back(model);
            }
            break;

        default:
            break;
        }
    }

    if (request != nullptr)
    {
        mPending.push_back(Pending{ request, false, models });
    }

    _check_models();
}

void SystemController::thread_running( const String & threadName, uint64_t instance )
{
    const int32_t index{ _find_live(threadName, instance) };
    if ((index < 0) || (mThreads[static_cast<uint32_t>(index)].mState != areg::ThreadState::Starting))
        return;

    ThreadRecord & record{ mThreads[static_cast<uint32_t>(index)] };
    do
    {
        Lock lock(mLock);
        record.mState       = areg::ThreadState::Running;
        record.mHasDeadline = false;
    } while (false);

    record.mThread->mLifecycleState.store(areg::ThreadState::Running, std::memory_order_release);
    _check_models();
    _arm_deadline();
}

void SystemController::thread_stopped( const String & threadName, uint64_t instance )
{
    LOG_SCOPE( areg_component_private_SystemController, thread_stopped );

    const int32_t index{ _find_live(threadName, instance) };
    if (index >= 0)
    {
        const ThreadRecord record{ mThreads[static_cast<uint32_t>(index)] };
        do
        {
            Lock lock(mLock);
            mThreads.erase(mThreads.begin() + index);
        } while (false);

        _free_thread(record.mThread, record.mTimeout);

        const areg::ModelState modelState{ _model_state(record.mModel) };
        if (record.mState == areg::ThreadState::Starting)
        {
            LOG_ERR("The component thread [ %s ] ended before its components started", record.mName.as_string());
            if (modelState == areg::ModelState::Loading)
            {
                mFailedLoads.push_back(record.mModel);
                _start_unload(record.mModel);
            }
        }
        else if (record.mRestart)
        {
            LOG_WARN("The component thread [ %s ] stopped, starting its new instance", record.mName.as_string());
            _start_instance(record.mName, record.mModel);
        }
        else if (record.mState == areg::ThreadState::Running)
        {
            LOG_WARN("The component thread [ %s ] ended without being asked to", record.mName.as_string());
        }

        _check_models();
        _arm_deadline();
        return;
    }

    for (uint32_t i = 0; i < mAbandoned.size(); ++ i)
    {
        if ((mAbandoned[i].mName == threadName) && (static_cast<uint64_t>(reinterpret_cast<uintptr_t>(mAbandoned[i].mThread)) == instance))
        {
            ComponentThread * thread{ mAbandoned[i].mThread };
            const uint32_t timeout{ mAbandoned[i].mTimeout };
            do
            {
                Lock lock(mLock);
                mAbandoned.erase(mAbandoned.begin() + i);
            } while (false);

            LOG_WARN("The abandoned component thread [ %s ] returned and is released", threadName.as_string());
            _free_thread(thread, timeout);
            _start_deferred(threadName);
            _check_models();
            _arm_deadline();
            break;
        }
    }
}

void SystemController::watchdog_expired( const String & threadName, uint64_t instance, const String & guardedName )
{
    LOG_SCOPE( areg_component_private_SystemController, watchdog_expired );

    const int32_t index{ _find_live(threadName, instance) };
    if ((index < 0) || (mThreads[static_cast<uint32_t>(index)].mState != areg::ThreadState::Running))
        return;

    ThreadRecord & record{ mThreads[static_cast<uint32_t>(index)] };
    const bool isWorker{ guardedName != threadName };
    LOG_WARN("The watchdog of the thread [ %s ] of component thread [ %s ] expired, policy [ %s ]"
                , guardedName.as_string(), threadName.as_string(), areg::as_string(record.mPolicy));

    switch (record.mPolicy)
    {
    case areg::FailurePolicy::Restart:
        _restart_thread(static_cast<uint32_t>(index));
        break;

    case areg::FailurePolicy::Ignore:
        if (isWorker == false)
        {
            _abandon_thread(static_cast<uint32_t>(index), areg::DisconnectReason::ProviderDisconnected);
        }
        break;

    case areg::FailurePolicy::Exit:
    default:
        _exit_process(isWorker ? -1 : index, "a watchdog expired");
        break;
    }

    _check_models();
    _arm_deadline();
}

void SystemController::deadline_expired()
{
    LOG_SCOPE( areg_component_private_SystemController, deadline_expired );

    const Clock::time_point now{ Clock::now() };
    if (mExiting && (now >= mExitDeadline))
    {
        _finish_exit();
    }

    for (uint32_t i = 0; i < mThreads.size(); )
    {
        ThreadRecord & record{ mThreads[i] };
        if ((record.mHasDeadline == false) || (record.mDeadline > now))
        {
            ++ i;
        }
        else if ((record.mState == areg::ThreadState::Stopping) && record.mDrain)
        {
            // The drain is over: the rest of the queue is not delivered.
            record.mDrain       = false;
            record.mDeadline    = _after(record.mTimeout);
            record.mThread->trigger_exit();
            record.mThread->request_exit();
            ++ i;
        }
        else if ((record.mState == areg::ThreadState::Stopping) && record.mRestart)
        {
            const String name{ record.mName };
            const String model{ record.mModel };
            _abandon_thread(i, areg::DisconnectReason::ProviderRestarting);
            _start_replacement(name, model);
        }
        else
        {
            _apply_policy(i, record.mState == areg::ThreadState::Starting ? "start" : "stop");
        }
    }

    _check_models();
    _arm_deadline();
}

void SystemController::stop()
{
    for (const Pending & pending : mPending)
    {
        pending.mRequest->complete(false);
    }

    mPending.clear();
    mGuard.reset();
}

void SystemController::set_user_state( uint32_t userState )
{
    const uint32_t previous{ _userState.exchange(userState, std::memory_order_acq_rel) };
    if (previous == userState)
        return;

    for (const ThreadRecord & record : mThreads)
    {
        UserStateNotifier::announce(*record.mThread, previous, userState);
    }
}

uint32_t SystemController::user_state() noexcept
{
    return _userState.load(std::memory_order_acquire);
}

void SystemController::store_user_state( uint32_t userState ) noexcept
{
    _userState.store(userState, std::memory_order_release);
}

areg::ThreadState SystemController::thread_state( const String & threadName ) const
{
    Lock lock(mLock);
    for (const ThreadRecord & record : mThreads)
    {
        if (record.mName == threadName)
            return record.mState;
    }

    for (const ThreadRecord & record : mAbandoned)
    {
        if (record.mName == threadName)
            return areg::ThreadState::Abandoned;
    }

    return areg::ThreadState::Unknown;
}

bool SystemController::is_caller_abandoned() noexcept
{
    // Reads the object of the calling thread, also when it is no longer registered.
    Thread * self{ Thread::_self_thread() };
    const ComponentThread * compThread{ AREG_RUNTIME_CAST(self, ComponentThread) };
    if (compThread != nullptr)
        return compThread->is_abandoned();

    const WorkerThread * workThread{ AREG_RUNTIME_CAST(self, WorkerThread) };
    return ((workThread != nullptr) && workThread->binding_component_thread().is_abandoned());
}

bool SystemController::is_caller_in_model( const String & modelName )
{
    Thread * self{ Thread::_self_thread() };
    const ComponentThread * compThread{ AREG_RUNTIME_CAST(self, ComponentThread) };
    if (compThread == nullptr)
    {
        const WorkerThread * workThread{ AREG_RUNTIME_CAST(self, WorkerThread) };
        compThread = (workThread != nullptr ? &workThread->binding_component_thread() : nullptr);
    }

    if (compThread == nullptr)
        return false;
    else if (modelName.is_empty())
        return true;

    ComponentLoader & loader{ ComponentLoader::instance() };
    Lock lock(loader.mLock);
    const areg::Model * model{ loader.find_model_named(modelName) };
    return ((model != nullptr) && (model->find_thread(compThread->name()) >= 0));
}

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////

void SystemController::_start_load( const String & modelName )
{
    LOG_SCOPE( areg_component_private_SystemController, _start_load );

    std::vector<areg::ComponentThreadEntry> entries;
    do
    {
        ComponentLoader & loader{ ComponentLoader::instance() };
        Lock lock(loader.mLock);
        const areg::Model * model{ loader.find_model_named(modelName) };
        if (model == nullptr)
            return;

        const areg::ComponentThreadList & threads{ model->thread_list() };
        for (uint32_t i = 0; i < threads.mListThreads.size(); ++ i)
        {
            entries.push_back(threads.mListThreads[i]);
        }
    } while (false);

    _set_model_state(modelName, areg::ModelState::Loading);

    bool started{ true };
    for (const areg::ComponentThreadEntry & entry : entries)
    {
        if (entry.is_valid() == false)
        {
            started = false;
            break;
        }

        if ((_find_live(entry.mThreadName) >= 0) || (Thread::find_by_address(ThreadAddress(entry.mThreadName)) != nullptr))
        {
            LOG_WARN("A thread named [ %s ] already runs, it is not started again", entry.mThreadName.as_string());
            continue;
        }

        if (_start_thread(entry, modelName) == false)
        {
            started = false;
            break;
        }
    }

    if (started == false)
    {
        LOG_ERR("Failed to load the model [ %s ]", modelName.as_string());
        mFailedLoads.push_back(modelName);
        _start_unload(modelName);
    }

    _arm_deadline();
}

void SystemController::_start_unload( const String & modelName )
{
    _set_model_state(modelName, areg::ModelState::Unloading);
    mUnloadAfterLoad.erase(std::remove(mUnloadAfterLoad.begin(), mUnloadAfterLoad.end(), modelName), mUnloadAfterLoad.end());
    mDeferred.erase(std::remove_if(mDeferred.begin(), mDeferred.end(), [&modelName](const Deferred & deferred) { return (deferred.mModel == modelName); }), mDeferred.end());
    for (ThreadRecord & record : mThreads)
    {
        if (record.mModel == modelName)
        {
            _stop_thread(record);
        }
    }

    _arm_deadline();
}

bool SystemController::_start_thread( const areg::ComponentThreadEntry & entry, const String & modelName )
{
    LOG_SCOPE( areg_component_private_SystemController, _start_thread );

    ComponentThread * thread{ new ComponentThread(entry.mThreadName, entry.mWatchdogTimeout, entry.mStackSizeKB, entry.mMaxQueue, entry.mDropOnFull, entry.mQueueTimeout) };
    if (thread == nullptr)
        return false;

    thread->share_with_lookups();
    const uint32_t timeout{ entry.mLifecycleTimeout != 0u ? entry.mLifecycleTimeout : areg::LIFECYCLE_TIMEOUT_DEFAULT };
    do
    {
        Lock lock(mLock);
        mThreads.push_back(ThreadRecord{ entry.mThreadName, modelName, thread, areg::ThreadState::Starting, _after(timeout), true, false, false, timeout, entry.mFailurePolicy });
    } while (false);

    if (thread->start(areg::DO_NOT_WAIT) == false)
    {
        LOG_ERR("Failed to start the component thread [ %s ]", entry.mThreadName.as_string());
        do
        {
            Lock lock(mLock);
            mThreads.pop_back();
        } while (false);

        static_cast<void>(thread->shutdown(areg::DO_NOT_WAIT));
        DispatcherThread::release_shared(thread);
        return false;
    }

    return true;
}

void SystemController::_start_instance( const String & threadName, const String & modelName )
{
    LOG_SCOPE( areg_component_private_SystemController, _start_instance );

    if ((_model_state(modelName) != areg::ModelState::Loaded) || mExiting || (_find_live(threadName) >= 0))
        return;

    const areg::ComponentThreadEntry entry{ ComponentLoader::find_thread_entry(threadName) };
    if ((entry.is_valid() == false) || (_start_thread(entry, modelName) == false))
    {
        LOG_ERR("Failed to start the new instance of the component thread [ %s ]", threadName.as_string());
    }
}

void SystemController::_start_replacement( const String & threadName, const String & modelName )
{
    LOG_SCOPE( areg_component_private_SystemController, _start_replacement );

    if (_count_abandoned(threadName) >= MAX_HUNG_INSTANCES)
    {
        LOG_ERR("Two instances of the component thread [ %s ] hang, the new instance starts when one returns", threadName.as_string());
        mDeferred.push_back(Deferred{ threadName, modelName });
    }
    else
    {
        _start_instance(threadName, modelName);
    }
}

void SystemController::_start_deferred( const String & threadName )
{
    LOG_SCOPE( areg_component_private_SystemController, _start_deferred );

    for (auto pos = mDeferred.begin(); pos != mDeferred.end(); ++ pos)
    {
        if (pos->mThread == threadName)
        {
            const String model{ pos->mModel };
            mDeferred.erase(pos);
            LOG_WARN("An instance of the component thread [ %s ] returned, starting the deferred new instance", threadName.as_string());
            _start_instance(threadName, model);
            break;
        }
    }
}

void SystemController::_stop_thread( ThreadRecord & record )
{
    if (record.mState == areg::ThreadState::Stopping)
    {
        // A restart in progress: the old instance stops and no new one is started.
        record.mRestart = false;
        return;
    }

    do
    {
        Lock lock(mLock);
        record.mState       = areg::ThreadState::Stopping;
        record.mDrain       = true;
        record.mRestart     = false;
        record.mHasDeadline = true;
        record.mDeadline    = _after(areg::SHUTDOWN_DRAIN_TIMEOUT);
    } while (false);

    record.mThread->mLifecycleState.store(areg::ThreadState::Stopping, std::memory_order_release);
    record.mThread->trigger_exit_drained();
}

void SystemController::_restart_thread( uint32_t index )
{
    ThreadRecord & record{ mThreads[index] };
    do
    {
        Lock lock(mLock);
        record.mState       = areg::ThreadState::Stopping;
        record.mDrain       = false;
        record.mRestart     = true;
        record.mHasDeadline = true;
        record.mDeadline    = _after(RESTART_GRACE_MS);
    } while (false);

    record.mThread->mIsRestarting.store(true, std::memory_order_release);
    record.mThread->mLifecycleState.store(areg::ThreadState::Stopping, std::memory_order_release);
    record.mThread->trigger_exit();
    record.mThread->request_exit();
}

void SystemController::_abandon_thread( uint32_t index, areg::DisconnectReason reason )
{
    LOG_SCOPE( areg_component_private_SystemController, _abandon_thread );

    ThreadRecord record{ mThreads[index] };
    ComponentThread * thread{ record.mThread };
    LOG_ERR("The component thread [ %s ] missed its deadline and is abandoned: it is unregistered, and released if it returns", record.mName.as_string());

    // From here on nothing the thread registers reaches the service registry.
    thread->mLifecycleState.store(areg::ThreadState::Abandoned, std::memory_order_release);
    record.mState       = areg::ThreadState::Abandoned;
    record.mHasDeadline = false;
    do
    {
        Lock lock(mLock);
        mThreads.erase(mThreads.begin() + index);
        mAbandoned.push_back(record);
    } while (false);

    thread->trigger_exit();
    thread->request_exit();

    // The worker threads of its components leave the thread registry with it.
    ArrayList<String> names;
    names.add(thread->name());
    std::vector<Thread *> workers;
    do
    {
        Thread::MapThreadNameResource & threads{ Thread::_map_thread_name() };
        Lock lock(threads.lockable());
        UniqueNumber key{ 0u };
        for (Thread * entry = threads.resource_first_key(key); entry != nullptr; entry = threads.resource_next_key(key))
        {
            WorkerThread * worker{ AREG_RUNTIME_CAST(entry, WorkerThread) };
            if ((worker != nullptr) && (&worker->binding_component_thread() == thread) && ((worker->mLifeState.load(std::memory_order_relaxed) & Thread::LIFE_SHARES) != 0u))
            {
                worker->mLifeState.fetch_add(Thread::LIFE_SHARE, std::memory_order_relaxed);
                workers.push_back(worker);
            }
        }
    } while (false);

    for (Thread * worker : workers)
    {
        names.add(worker->name());
        worker->detach_from_registry();
        Thread::_drop_hold(worker);
    }

    thread->detach_from_registry();
    StubBase::_detach_thread_providers(names);
    Component::_detach_thread_components(*thread);
    ProxyBase::_detach_thread_proxies(names);

    ServiceManager & manager{ ServiceManager::instance() };
    manager.mEventProcessor.unregister_thread_services(names, reason, manager.service_register_provider());
    thread->mWatchdog.disarm();
}

void SystemController::_free_thread( ComponentThread * thread, uint32_t timeout )
{
    LOG_SCOPE( areg_component_private_SystemController, _free_thread );

    if (thread->shutdown(timeout) == Thread::ThreadCompletion::Stuck)
    {
        LOG_ERR("The component thread [ %s ] did not leave after its exit, it is not released", thread->name().as_string());
        return;
    }

    DispatcherThread::release_shared(thread);
}

void SystemController::_apply_policy( uint32_t index, const char * what )
{
    LOG_SCOPE( areg_component_private_SystemController, _apply_policy );

    const ThreadRecord & record{ mThreads[index] };
    LOG_ERR("The component thread [ %s ] missed its %s deadline of [ %u ] ms, policy [ %s ]"
                , record.mName.as_string(), what, record.mTimeout, areg::as_string(record.mPolicy));

    if (record.mPolicy == areg::FailurePolicy::Exit)
    {
        _exit_process(static_cast<int32_t>(index), what);
    }
    else if ((record.mPolicy == areg::FailurePolicy::Restart) && (record.mState == areg::ThreadState::Starting) && (_model_state(record.mModel) == areg::ModelState::Loaded))
    {
        // A new instance of a loaded model that hangs in its start is replaced too.
        const String name{ record.mName };
        const String model{ record.mModel };
        _abandon_thread(index, areg::DisconnectReason::ProviderRestarting);
        _start_replacement(name, model);
    }
    else if ((record.mPolicy == areg::FailurePolicy::Restart) && (record.mState == areg::ThreadState::Starting) && (_model_state(record.mModel) == areg::ModelState::Loading))
    {
        // A thread that hangs in its first start fails the load of its model.
        const String model{ record.mModel };
        _abandon_thread(index, areg::DisconnectReason::ProviderDisconnected);
        LOG_ERR("Failed to load the model [ %s ]", model.as_string());
        mFailedLoads.push_back(model);
        _start_unload(model);
    }
    else
    {
        _abandon_thread(index, areg::DisconnectReason::ProviderDisconnected);
    }
}

void SystemController::_check_models()
{
    std::vector<String> models;
    _model_names(models);
    bool allUnloaded{ true };
    for (const String & model : models)
    {
        const areg::ModelState state{ _model_state(model) };
        if (state == areg::ModelState::Loading)
        {
            bool starting{ false };
            for (const ThreadRecord & record : mThreads)
            {
                starting = starting || ((record.mModel == model) && (record.mState == areg::ThreadState::Starting));
            }

            if (starting == false)
            {
                _set_model_state(model, areg::ModelState::Loaded);
            }

            allUnloaded = false;
        }
        else if (state == areg::ModelState::Unloading)
        {
            if (_count_live(model) == 0u)
            {
                _set_model_state(model, areg::ModelState::Unloaded);
                mFailedLoads.erase(std::remove(mFailedLoads.begin(), mFailedLoads.end(), model), mFailedLoads.end());
            }
            else
            {
                allUnloaded = false;
            }
        }
        else if (state == areg::ModelState::Loaded)
        {
            allUnloaded = false;
        }
    }

    _complete_pending();

    if (mExiting && allUnloaded)
    {
        _finish_exit();
    }

    for (const String & model : models)
    {
        if ((std::find(mUnloadAfterLoad.begin(), mUnloadAfterLoad.end(), model) != mUnloadAfterLoad.end()) && (_model_state(model) == areg::ModelState::Loaded))
        {
            _start_unload(model);
            _check_models();
            return;
        }
    }
}

void SystemController::_complete_pending()
{
    for (uint32_t i = 0; i < mPending.size(); )
    {
        const Pending & pending{ mPending[i] };
        bool done{ true };
        bool result{ true };
        for (const String & model : pending.mModels)
        {
            const areg::ModelState state{ _model_state(model) };
            if (pending.mLoad)
            {
                done    = done && ((state == areg::ModelState::Loaded) || (state == areg::ModelState::Unloaded));
                result  = result && (state == areg::ModelState::Loaded);
            }
            else
            {
                done    = done && (state == areg::ModelState::Unloaded);
            }
        }

        if (done)
        {
            const std::shared_ptr<Request> request{ pending.mRequest };
            mPending.erase(mPending.begin() + i);
            request->complete(result);
        }
        else
        {
            ++ i;
        }
    }
}

void SystemController::_exit_process( int32_t culprit, const char * reason )
{
    LOG_SCOPE( areg_component_private_SystemController, _exit_process );
    LOG_ERR("The failure policy Exit ends the process: %s", reason);

    if (culprit >= 0)
    {
        // The thread that failed is not waited for.
        _abandon_thread(static_cast<uint32_t>(culprit), areg::DisconnectReason::ProviderDisconnected);
    }

    if (mExiting)
    {
        _finish_exit();
    }

    mExiting = true;
    uint32_t longest{ 0u };
    for (const ThreadRecord & record : mThreads)
    {
        longest = std::max(longest, record.mTimeout);
    }

    mExitDeadline = _after(areg::SHUTDOWN_DRAIN_TIMEOUT + longest);

    std::vector<String> models;
    _model_names(models);
    for (const String & model : models)
    {
        const areg::ModelState state{ _model_state(model) };
        if ((state == areg::ModelState::Loaded) || (state == areg::ModelState::Loading))
        {
            _start_unload(model);
        }
    }

    _check_models();
    _arm_deadline();
}

void SystemController::_finish_exit()
{
    LOG_SCOPE( areg_component_private_SystemController, _finish_exit );

    LOG_ERR("The process exits with the code [ %d ]", areg::LIFECYCLE_FAILURE_EXIT_CODE);
    areg::stop_logging(true);
    std::_Exit(areg::LIFECYCLE_FAILURE_EXIT_CODE);
}

void SystemController::_arm_deadline()
{
    LOG_SCOPE( areg_component_private_SystemController, _arm_deadline );

    bool armed{ mExiting };
    Clock::time_point earliest{ mExitDeadline };
    for (const ThreadRecord & record : mThreads)
    {
        if (record.mHasDeadline && ((armed == false) || (record.mDeadline < earliest)))
        {
            earliest = record.mDeadline;
            armed = true;
        }
    }

    if (armed)
    {
        if (mGuard == nullptr)
        {
            mGuard.reset(new Watchdog(String(DEADLINE_GUARD_NAME)));
        }

        if ((mGuard != nullptr) && (mGuard->arm(_ms_until(earliest)) == false))
        {
            LOG_WARN("The watchdog manager does not run: the lifecycle deadlines are not enforced");
        }
    }
    else if (mGuard != nullptr)
    {
        mGuard->stop_guard();
    }
}

int32_t SystemController::_find_live( const String & threadName, uint64_t instance ) const
{
    for (uint32_t i = 0; i < mThreads.size(); ++ i)
    {
        if ((mThreads[i].mName == threadName) && (static_cast<uint64_t>(reinterpret_cast<uintptr_t>(mThreads[i].mThread)) == instance))
            return static_cast<int32_t>(i);
    }

    return -1;
}

int32_t SystemController::_find_live( const String & threadName ) const
{
    for (uint32_t i = 0; i < mThreads.size(); ++ i)
    {
        if (mThreads[i].mName == threadName)
            return static_cast<int32_t>(i);
    }

    return -1;
}

uint32_t SystemController::_count_abandoned( const String & threadName ) const
{
    uint32_t result{ 0u };
    for (const ThreadRecord & record : mAbandoned)
    {
        result += (record.mName == threadName) ? 1u : 0u;
    }

    return result;
}

uint32_t SystemController::_count_live( const String & modelName ) const
{
    uint32_t result{ 0u };
    for (const ThreadRecord & record : mThreads)
    {
        result += (record.mModel == modelName) ? 1u : 0u;
    }

    return result;
}

areg::ModelState SystemController::_model_state( const String & modelName )
{
    ComponentLoader & loader{ ComponentLoader::instance() };
    Lock lock(loader.mLock);
    const areg::Model * model{ loader.find_model_named(modelName) };
    return (model != nullptr ? model->model_state() : areg::ModelState::Unloaded);
}

void SystemController::_set_model_state( const String & modelName, areg::ModelState newState )
{
    ComponentLoader & loader{ ComponentLoader::instance() };
    Lock lock(loader.mLock);
    for (uint32_t i = 0; i < loader.mModelList.size(); ++ i)
    {
        areg::Model & model{ loader.mModelList[i] };
        if (model.model_name() == modelName)
        {
            model.set_model_state(newState);
            break;
        }
    }
}

bool SystemController::_model_exists( const String & modelName )
{
    ComponentLoader & loader{ ComponentLoader::instance() };
    Lock lock(loader.mLock);
    return (loader.find_model_named(modelName) != nullptr);
}

void SystemController::_model_names( std::vector<String> & names )
{
    ComponentLoader & loader{ ComponentLoader::instance() };
    Lock lock(loader.mLock);
    for (uint32_t i = 0; i < loader.mModelList.size(); ++ i)
    {
        names.push_back(loader.mModelList[i].model_name());
    }
}

} // namespace areg
