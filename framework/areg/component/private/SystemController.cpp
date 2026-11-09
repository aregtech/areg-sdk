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
#include <cstring>
#include <new>
#include <string_view>
#include <type_traits>

namespace
{
    //!< The name of the deadline guard on the watchdog manager.
    constexpr std::string_view  DEADLINE_GUARD_NAME { "_AREG_DEADLINE_" };

    //!< The number of thread records of the first block.
    constexpr uint32_t          FIRST_RECORD_COUNT  { 8u };

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
DEF_LOG_SCOPE(areg_component_private_SystemController, _is_startable);
DEF_LOG_SCOPE(areg_component_private_SystemController, deadline_expired);

//////////////////////////////////////////////////////////////////////////
// SystemController class local helpers
//////////////////////////////////////////////////////////////////////////

Thread * SystemController::_hold_next_worker( const ComponentThread & owner )
{
    Thread::MapThreadNameResource & threads{ Thread::_map_thread_name() };
    Lock lock(threads.lockable());
    UniqueNumber key{ 0u };
    for (Thread * entry = threads.resource_first_key(key); entry != nullptr; entry = threads.resource_next_key(key))
    {
        WorkerThread * worker{ AREG_RUNTIME_CAST(entry, WorkerThread) };
        if ((worker != nullptr) && (&worker->binding_component_thread() == &owner))
        {
            const auto state{ worker->mLifeState.load(std::memory_order_relaxed) };
            if (((state & Thread::LIFE_SHARES) != 0u) && ((state & Thread::LIFE_REGISTERED) != 0u))
            {
                worker->mLifeState.fetch_add(Thread::LIFE_SHARE, std::memory_order_relaxed);
                return worker;
            }
        }
    }

    return nullptr;
}

//////////////////////////////////////////////////////////////////////////
// SystemController::Request class implementation
//////////////////////////////////////////////////////////////////////////

SystemController::Request::Request( bool waitComponents /*= false*/ )
    : mDone             ( true, false )
    , mResult           ( false )
    , mWaitComponents   ( waitComponents )
    , mLoad             ( false )
    , mShares           ( 2u )
    , mModels           ( SystemController::MODEL_NONE )
    , mLastModel        ( SystemController::MODEL_NONE )
    , mNext             ( nullptr )
{
}

SystemController::Request * SystemController::Request::create( bool waitComponents /*= false*/ )
{
    return new (std::nothrow) Request( waitComponents );
}

bool SystemController::Request::wait()
{
    mDone.lock(areg::WAIT_INFINITE);
    const bool result{ mResult };
    release();
    return result;
}

void SystemController::Request::complete( bool result )
{
    mResult = result;
    mDone.set_signaled();
    release();
}

void SystemController::Request::release()
{
    if (mShares.fetch_sub(1u, std::memory_order_acq_rel) == 1u)
    {
        delete this;
    }
}

//////////////////////////////////////////////////////////////////////////
// SystemController class implementation
//////////////////////////////////////////////////////////////////////////

SystemController::SystemController()
    : mRecords          ( nullptr )
    , mRecordCount      ( 0u )
    , mRecordCapacity   ( 0u )
    , mPending          ( nullptr )
    , mGuard            ( )
    , mExiting          ( false )
    , mExitDeadline     ( )
    , mLock             ( )
{
    static_assert(std::is_trivially_copyable<ThreadRecord>::value, "The records are copied as bytes");
}

SystemController::~SystemController()
{
    // Leaves behind a guard that stop() did not release.
    static_cast<void>(mGuard.release());
    delete[] mRecords;
}

bool SystemController::start()
{
    if (mGuard == nullptr)
    {
        mGuard.reset(new (std::nothrow) Watchdog(String(DEADLINE_GUARD_NAME)));
    }

    if ((mGuard == nullptr) || (mGuard->can_arm() == false))
    {
        mGuard.reset();
        return false;
    }

    return true;
}

void SystemController::load_model( uint32_t modelId, Request * request )
{
    LOG_SCOPE( areg_component_private_SystemController, load_model );

    bool accepted{ (mExiting == false) && ((modelId == MODEL_ALL) || _model_exists(modelId)) };
    for (uint32_t model = _next_model(modelId, 0u); accepted && (model != MODEL_NONE); model = _next_model(modelId, model))
    {
        switch (_model_state(model))
        {
        case areg::ModelState::Unloaded:
            _start_load(model);
            break;

        case areg::ModelState::Unloading:
            LOG_WARN("A model is unloading, the load is refused");
            accepted = false;
            break;

        case areg::ModelState::Loading:
        case areg::ModelState::Loaded:
        default:
            break;
        }
    }

    if ((request != nullptr) && accepted && (request->wait_components() == false))
    {
        for (uint32_t model = _next_model(modelId, 0u); model != MODEL_NONE; model = _next_model(modelId, model))
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
            _add_pending(*request, true, modelId);
        }
        else
        {
            request->complete(false);
        }
    }

    _check_models();
}

void SystemController::unload_model( uint32_t modelId, Request * request )
{
    LOG_SCOPE( areg_component_private_SystemController, unload_model );

    for (uint32_t model = _next_model(modelId, 0u); model != MODEL_NONE; model = _next_model(modelId, model))
    {
        switch (_model_state(model))
        {
        case areg::ModelState::Loaded:
            _start_unload(model);
            break;

        case areg::ModelState::Loading:
            _set_unload_after_load(model, true);
            break;

        case areg::ModelState::Unloaded:
        case areg::ModelState::Unloading:
        default:
            break;
        }
    }

    if (request != nullptr)
    {
        _add_pending(*request, false, modelId);
    }

    _check_models();
}

void SystemController::thread_running( uint64_t instance )
{
    const int32_t index{ _find_record(instance) };
    if ((index < 0) || (mRecords[static_cast<uint32_t>(index)].mState != areg::ThreadState::Starting))
        return;

    ThreadRecord & record{ mRecords[static_cast<uint32_t>(index)] };
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

void SystemController::thread_stopped( uint64_t instance )
{
    LOG_SCOPE( areg_component_private_SystemController, thread_stopped );

    const int32_t index{ _find_record(instance) };
    if (index < 0)
        return;

    const ThreadRecord record{ mRecords[static_cast<uint32_t>(index)] };
    if (record.mState == areg::ThreadState::Abandoned)
    {
        const bool deferred{ _take_deferred(record.mModel, record.mPosition) };
        LOG_WARN("The abandoned component thread [ %s ] returned and is released", record.mThread->name().as_string());
        _remove_record(static_cast<uint32_t>(index));
        _free_thread(record.mThread, record.mTimeout);
        if (deferred)
        {
            LOG_WARN("An instance of a component thread returned, starting the deferred new instance");
            _start_instance(record.mModel, record.mPosition);
        }
    }
    else
    {
        if (record.mState == areg::ThreadState::Starting)
        {
            LOG_ERR("The component thread [ %s ] ended before its components started", record.mThread->name().as_string());
        }
        else if (record.mRestart)
        {
            LOG_WARN("The component thread [ %s ] stopped, starting its new instance", record.mThread->name().as_string());
        }
        else if (record.mState == areg::ThreadState::Running)
        {
            LOG_WARN("The component thread [ %s ] ended without being asked to", record.mThread->name().as_string());
        }

        _remove_record(static_cast<uint32_t>(index));
        _free_thread(record.mThread, record.mTimeout);

        if (record.mState == areg::ThreadState::Starting)
        {
            if (_model_state(record.mModel) == areg::ModelState::Loading)
            {
                _start_unload(record.mModel);
            }
        }
        else if (record.mRestart)
        {
            _start_instance(record.mModel, record.mPosition);
        }
    }

    _check_models();
    _arm_deadline();
}

void SystemController::watchdog_expired( uint64_t instance, bool isWorker )
{
    LOG_SCOPE( areg_component_private_SystemController, watchdog_expired );

    const int32_t index{ _find_record(instance) };
    if ((index < 0) || (mRecords[static_cast<uint32_t>(index)].mState != areg::ThreadState::Running))
        return;

    const areg::FailurePolicy policy{ mRecords[static_cast<uint32_t>(index)].mPolicy };
    LOG_WARN("The watchdog of a %s of component thread [ %s ] expired, policy [ %s ]"
                , isWorker ? "worker" : "component thread"
                , mRecords[static_cast<uint32_t>(index)].mThread->name().as_string()
                , areg::as_string(policy));

    switch (policy)
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

    // An abandoned record stays at its index and has no deadline.
    for (uint32_t i = 0; i < mRecordCount; )
    {
        ThreadRecord & record{ mRecords[i] };
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
            _abandon_thread(i, areg::DisconnectReason::ProviderRestarting);
            _start_replacement(i);
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
    while (mPending != nullptr)
    {
        Request * const request{ mPending };
        mPending = request->mNext;
        request->complete(false);
    }

    mGuard.reset();
}

void SystemController::set_user_state( uint32_t userState )
{
    const uint32_t previous{ _userState.exchange(userState, std::memory_order_acq_rel) };
    if (previous == userState)
        return;

    for (uint32_t i = 0; i < mRecordCount; ++ i)
    {
        if (mRecords[i].mState != areg::ThreadState::Abandoned)
        {
            UserStateNotifier::announce(*mRecords[i].mThread, previous, userState);
        }
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
    areg::ThreadState result{ areg::ThreadState::Unknown };
    Lock lock(mLock);
    for (uint32_t i = 0; i < mRecordCount; ++ i)
    {
        if (mRecords[i].mThread->name() == threadName)
        {
            if (mRecords[i].mState != areg::ThreadState::Abandoned)
                return mRecords[i].mState;

            result = areg::ThreadState::Abandoned;
        }
    }

    return result;
}

uint32_t SystemController::find_model_id( const String & modelName )
{
    if (modelName.is_empty())
        return MODEL_ALL;

    ComponentLoader & loader{ ComponentLoader::instance() };
    Lock lock(loader.mLock);
    const areg::Model * model{ loader.find_model_named(modelName) };
    return (model != nullptr ? model->mModelId : MODEL_NONE);
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

void SystemController::_start_load( uint32_t modelId )
{
    LOG_SCOPE( areg_component_private_SystemController, _start_load );

    _set_model_state(modelId, areg::ModelState::Loading);

    bool started{ true };
    const uint32_t count{ _thread_count(modelId) };
    for (uint32_t position = 0u; started && (position < count); ++ position)
    {
        started = _is_startable(modelId, position) && _start_thread(modelId, position);
    }

    if (started == false)
    {
        LOG_ERR("Failed to load a model");
        _start_unload(modelId);
    }

    _arm_deadline();
}

void SystemController::_start_unload( uint32_t modelId )
{
    _set_model_state(modelId, areg::ModelState::Unloading);
    _set_unload_after_load(modelId, false);
    for (uint32_t i = 0; i < mRecordCount; ++ i)
    {
        ThreadRecord & record{ mRecords[i] };
        if (record.mModel != modelId)
            continue;

        if (record.mState == areg::ThreadState::Abandoned)
        {
            record.mDeferred = false;
        }
        else
        {
            _stop_thread(record);
        }
    }

    _arm_deadline();
}

bool SystemController::_is_startable( uint32_t modelId, uint32_t position ) const
{
    LOG_SCOPE( areg_component_private_SystemController, _is_startable );

    ComponentLoader & loader{ ComponentLoader::instance() };
    Lock lock(loader.mLock);
    const areg::Model * model{ _find_model(modelId) };
    if ((model == nullptr) || (position >= model->thread_list().mListThreads.size()))
        return false;

    const areg::ComponentThreadEntry & entry{ model->thread_list().mListThreads[position] };
    if (entry.is_valid() == false)
        return false;

    if ((_find_live(entry.mThreadName) >= 0) || (Thread::find_by_address(ThreadAddress(entry.mThreadName)) != nullptr))
    {
        LOG_ERR("A thread named [ %s ] already runs, the component thread cannot start", entry.mThreadName.as_string());
        return false;
    }

    return true;
}

bool SystemController::_start_thread( uint32_t modelId, uint32_t position )
{
    LOG_SCOPE( areg_component_private_SystemController, _start_thread );

    if (_secure_room() == false)
    {
        LOG_ERR("The record of a component thread cannot be allocated, the thread is not started");
        return false;
    }

    ComponentThread * thread{ nullptr };
    uint32_t timeout{ areg::LIFECYCLE_TIMEOUT_DEFAULT };
    areg::FailurePolicy policy{ areg::FailurePolicy::Exit };
    do
    {
        ComponentLoader & loader{ ComponentLoader::instance() };
        Lock lock(loader.mLock);
        const areg::Model * model{ _find_model(modelId) };
        if ((model == nullptr) || (position >= model->thread_list().mListThreads.size()))
            return false;

        const areg::ComponentThreadEntry & entry{ model->thread_list().mListThreads[position] };
        thread  = new (std::nothrow) ComponentThread(entry.mThreadName, entry.mWatchdogTimeout, entry.mStackSizeKB, entry.mMaxQueue, entry.mDropOnFull, entry.mQueueTimeout);
        timeout = entry.mLifecycleTimeout != 0u ? entry.mLifecycleTimeout : areg::LIFECYCLE_TIMEOUT_DEFAULT;
        policy  = entry.mFailurePolicy;
    } while (false);

    if (thread == nullptr)
        return false;

    thread->share_with_lookups();
    if ((thread->mWatchdog.can_arm() == false) ||
        (ServiceManager::build_thread_report(true, *thread, thread->mRunningReport) == false) ||
        (ServiceManager::build_thread_report(false, *thread, thread->mStoppedReport) == false))
    {
        LOG_ERR("The watchdog or the reports of the component thread [ %s ] cannot be built, the thread is not started", thread->name().as_string());
        static_cast<void>(thread->shutdown(areg::DO_NOT_WAIT));
        DispatcherThread::release_shared(thread);
        return false;
    }

    do
    {
        Lock lock(mLock);
        mRecords[mRecordCount] = ThreadRecord{ thread, _after(timeout), timeout, modelId, position, areg::ThreadState::Starting, policy, true, false, false, false };
        ++ mRecordCount;
    } while (false);

    if (thread->start(areg::DO_NOT_WAIT) == false)
    {
        LOG_ERR("Failed to start the component thread [ %s ]", thread->name().as_string());
        do
        {
            Lock lock(mLock);
            -- mRecordCount;
        } while (false);

        static_cast<void>(thread->shutdown(areg::DO_NOT_WAIT));
        DispatcherThread::release_shared(thread);
        return false;
    }

    return true;
}

void SystemController::_start_instance( uint32_t modelId, uint32_t position )
{
    LOG_SCOPE( areg_component_private_SystemController, _start_instance );

    if ((_model_state(modelId) != areg::ModelState::Loaded) || mExiting || (_find_live(modelId, position) >= 0))
        return;

    if (_start_thread(modelId, position) == false)
    {
        LOG_ERR("Failed to start the new instance of a component thread");
    }
}

void SystemController::_start_replacement( uint32_t index )
{
    LOG_SCOPE( areg_component_private_SystemController, _start_replacement );

    const uint32_t modelId{ mRecords[index].mModel };
    const uint32_t position{ mRecords[index].mPosition };
    if (_count_abandoned(modelId, position) >= MAX_HUNG_INSTANCES)
    {
        LOG_ERR("Two instances of the component thread [ %s ] hang, the new instance starts when one returns", mRecords[index].mThread->name().as_string());
        mRecords[index].mDeferred = true;
    }
    else
    {
        _start_instance(modelId, position);
    }
}

bool SystemController::_take_deferred( uint32_t modelId, uint32_t position )
{
    bool result{ false };
    for (uint32_t i = 0; i < mRecordCount; ++ i)
    {
        ThreadRecord & record{ mRecords[i] };
        if ((record.mState == areg::ThreadState::Abandoned) && (record.mModel == modelId) && (record.mPosition == position))
        {
            result = result || record.mDeferred;
            record.mDeferred = false;
        }
    }

    return result;
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
    ThreadRecord & record{ mRecords[index] };
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

    ThreadRecord & record{ mRecords[index] };
    ComponentThread * thread{ record.mThread };
    LOG_ERR("The component thread [ %s ] missed its deadline and is abandoned: it is unregistered, and released if it returns", thread->name().as_string());

    // From here on nothing the thread registers reaches the service registry.
    thread->mLifecycleState.store(areg::ThreadState::Abandoned, std::memory_order_release);
    do
    {
        Lock lock(mLock);
        record.mState       = areg::ThreadState::Abandoned;
        record.mHasDeadline = false;
    } while (false);

    thread->trigger_exit();
    thread->request_exit();

    ServiceManager & manager{ ServiceManager::instance() };

    // The worker threads of its components leave the thread registry with it.
    for (Thread * worker = _hold_next_worker(*thread); worker != nullptr; worker = _hold_next_worker(*thread))
    {
        worker->detach_from_registry();
        StubBase::_detach_thread_providers(worker->address());
        ProxyBase::_detach_thread_proxies(worker->address());
        manager.mEventProcessor.unregister_thread_services(worker->address(), reason, manager.service_register_provider());
        Thread::_drop_hold(worker);
    }

    thread->detach_from_registry();
    StubBase::_detach_thread_providers(thread->address());
    Component::_detach_thread_components(*thread);
    ProxyBase::_detach_thread_proxies(thread->address());
    manager.mEventProcessor.unregister_thread_services(thread->address(), reason, manager.service_register_provider());
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

    const ThreadRecord record{ mRecords[index] };
    LOG_ERR("The component thread [ %s ] missed its %s deadline of [ %u ] ms, policy [ %s ]"
                , record.mThread->name().as_string(), what, record.mTimeout, areg::as_string(record.mPolicy));

    if (record.mPolicy == areg::FailurePolicy::Exit)
    {
        _exit_process(static_cast<int32_t>(index), what);
    }
    else if ((record.mPolicy == areg::FailurePolicy::Restart) && (record.mState == areg::ThreadState::Starting) && (_model_state(record.mModel) == areg::ModelState::Loaded))
    {
        // A new instance of a loaded model that hangs in its start is replaced too.
        _abandon_thread(index, areg::DisconnectReason::ProviderRestarting);
        _start_replacement(index);
    }
    else if ((record.mPolicy == areg::FailurePolicy::Restart) && (record.mState == areg::ThreadState::Starting) && (_model_state(record.mModel) == areg::ModelState::Loading))
    {
        // A thread that hangs in its first start fails the load of its model.
        _abandon_thread(index, areg::DisconnectReason::ProviderDisconnected);
        LOG_ERR("Failed to load a model");
        _start_unload(record.mModel);
    }
    else
    {
        _abandon_thread(index, areg::DisconnectReason::ProviderDisconnected);
    }
}

void SystemController::_check_models()
{
    bool allUnloaded{ true };
    for (uint32_t model = _next_model(MODEL_ALL, 0u); model != MODEL_NONE; model = _next_model(MODEL_ALL, model))
    {
        const areg::ModelState state{ _model_state(model) };
        if (state == areg::ModelState::Loading)
        {
            bool starting{ false };
            for (uint32_t i = 0; i < mRecordCount; ++ i)
            {
                starting = starting || ((mRecords[i].mModel == model) && (mRecords[i].mState == areg::ThreadState::Starting));
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

    for (uint32_t model = _next_model(MODEL_ALL, 0u); model != MODEL_NONE; model = _next_model(MODEL_ALL, model))
    {
        if (_is_unload_due(model))
        {
            _start_unload(model);
            _check_models();
            return;
        }
    }
}

void SystemController::_add_pending( Request & request, bool load, uint32_t modelId )
{
    request.mLoad       = load;
    request.mModels     = modelId;
    request.mLastModel  = (modelId == MODEL_ALL) ? _last_model() : MODEL_ALL;
    request.mNext       = nullptr;

    Request ** tail{ &mPending };
    while (*tail != nullptr)
    {
        tail = &(*tail)->mNext;
    }

    *tail = &request;
}

void SystemController::_complete_pending()
{
    Request ** link{ &mPending };
    while (*link != nullptr)
    {
        Request * const request{ *link };
        bool done{ true };
        bool result{ true };
        for (uint32_t model = _next_model(request->mModels, 0u); (model != MODEL_NONE) && (model <= request->mLastModel); model = _next_model(request->mModels, model))
        {
            const areg::ModelState state{ _model_state(model) };
            if (request->mLoad)
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
            *link = request->mNext;
            request->complete(result);
        }
        else
        {
            link = &request->mNext;
        }
    }
}

void SystemController::_exit_process( int32_t culprit, [[maybe_unused]] const char * reason )
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
    for (uint32_t i = 0; i < mRecordCount; ++ i)
    {
        if (mRecords[i].mState != areg::ThreadState::Abandoned)
        {
            longest = std::max(longest, mRecords[i].mTimeout);
        }
    }

    mExitDeadline = _after(areg::SHUTDOWN_DRAIN_TIMEOUT + longest);

    for (uint32_t model = _next_model(MODEL_ALL, 0u); model != MODEL_NONE; model = _next_model(MODEL_ALL, model))
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
    for (uint32_t i = 0; i < mRecordCount; ++ i)
    {
        const ThreadRecord & record{ mRecords[i] };
        if (record.mHasDeadline && ((armed == false) || (record.mDeadline < earliest)))
        {
            earliest = record.mDeadline;
            armed = true;
        }
    }

    ASSERT(mGuard != nullptr);
    if (armed)
    {
        if (mGuard->arm(_ms_until(earliest)) == false)
        {
            LOG_WARN("The watchdog manager does not run: the lifecycle deadlines are not enforced");
        }
    }
    else
    {
        mGuard->stop_guard();
    }
}

bool SystemController::_secure_room()
{
    if (mRecordCount < mRecordCapacity)
        return true;

    const uint32_t capacity{ mRecordCapacity != 0u ? (mRecordCapacity * 2u) : FIRST_RECORD_COUNT };
    ThreadRecord * const records{ new (std::nothrow) ThreadRecord[capacity] };
    if (records == nullptr)
        return false;

    ThreadRecord * const previous{ mRecords };
    do
    {
        Lock lock(mLock);
        if (previous != nullptr)
        {
            std::memcpy(records, previous, sizeof(ThreadRecord) * mRecordCount);
        }

        mRecords        = records;
        mRecordCapacity = capacity;
    } while (false);

    delete[] previous;
    return true;
}

void SystemController::_remove_record( uint32_t index )
{
    Lock lock(mLock);
    -- mRecordCount;
    if (index != mRecordCount)
    {
        std::memmove(mRecords + index, mRecords + index + 1u, sizeof(ThreadRecord) * (mRecordCount - index));
    }
}

int32_t SystemController::_find_record( uint64_t instance ) const
{
    for (uint32_t i = 0; i < mRecordCount; ++ i)
    {
        if (static_cast<uint64_t>(reinterpret_cast<uintptr_t>(mRecords[i].mThread)) == instance)
            return static_cast<int32_t>(i);
    }

    return -1;
}

int32_t SystemController::_find_live( const String & threadName ) const
{
    for (uint32_t i = 0; i < mRecordCount; ++ i)
    {
        if ((mRecords[i].mState != areg::ThreadState::Abandoned) && (mRecords[i].mThread->name() == threadName))
            return static_cast<int32_t>(i);
    }

    return -1;
}

int32_t SystemController::_find_live( uint32_t modelId, uint32_t position ) const
{
    for (uint32_t i = 0; i < mRecordCount; ++ i)
    {
        if ((mRecords[i].mState != areg::ThreadState::Abandoned) && (mRecords[i].mModel == modelId) && (mRecords[i].mPosition == position))
            return static_cast<int32_t>(i);
    }

    return -1;
}

uint32_t SystemController::_count_abandoned( uint32_t modelId, uint32_t position ) const
{
    uint32_t result{ 0u };
    for (uint32_t i = 0; i < mRecordCount; ++ i)
    {
        result += ((mRecords[i].mState == areg::ThreadState::Abandoned) && (mRecords[i].mModel == modelId) && (mRecords[i].mPosition == position)) ? 1u : 0u;
    }

    return result;
}

uint32_t SystemController::_count_live( uint32_t modelId ) const
{
    uint32_t result{ 0u };
    for (uint32_t i = 0; i < mRecordCount; ++ i)
    {
        result += ((mRecords[i].mState != areg::ThreadState::Abandoned) && (mRecords[i].mModel == modelId)) ? 1u : 0u;
    }

    return result;
}

areg::Model * SystemController::_find_model( uint32_t modelId )
{
    ComponentLoader & loader{ ComponentLoader::instance() };
    for (uint32_t i = 0; i < loader.mModelList.size(); ++ i)
    {
        if (loader.mModelList[i].mModelId == modelId)
            return &loader.mModelList[i];
    }

    return nullptr;
}

uint32_t SystemController::_next_model( uint32_t selection, uint32_t previous )
{
    if (selection != MODEL_ALL)
        return (previous == MODEL_NONE ? selection : MODEL_NONE);

    uint32_t result{ MODEL_NONE };
    ComponentLoader & loader{ ComponentLoader::instance() };
    Lock lock(loader.mLock);
    for (uint32_t i = 0; i < loader.mModelList.size(); ++ i)
    {
        const uint32_t id{ loader.mModelList[i].mModelId };
        if ((id > previous) && ((result == MODEL_NONE) || (id < result)))
        {
            result = id;
        }
    }

    return result;
}

uint32_t SystemController::_last_model()
{
    uint32_t result{ MODEL_NONE };
    ComponentLoader & loader{ ComponentLoader::instance() };
    Lock lock(loader.mLock);
    for (uint32_t i = 0; i < loader.mModelList.size(); ++ i)
    {
        result = std::max(result, loader.mModelList[i].mModelId);
    }

    return result;
}

areg::ModelState SystemController::_model_state( uint32_t modelId )
{
    ComponentLoader & loader{ ComponentLoader::instance() };
    Lock lock(loader.mLock);
    const areg::Model * model{ _find_model(modelId) };
    return (model != nullptr ? model->model_state() : areg::ModelState::Unloaded);
}

void SystemController::_set_model_state( uint32_t modelId, areg::ModelState newState )
{
    ComponentLoader & loader{ ComponentLoader::instance() };
    Lock lock(loader.mLock);
    areg::Model * model{ _find_model(modelId) };
    if (model != nullptr)
    {
        model->set_model_state(newState);
    }
}

bool SystemController::_model_exists( uint32_t modelId )
{
    ComponentLoader & loader{ ComponentLoader::instance() };
    Lock lock(loader.mLock);
    return (_find_model(modelId) != nullptr);
}

void SystemController::_set_unload_after_load( uint32_t modelId, bool unload )
{
    ComponentLoader & loader{ ComponentLoader::instance() };
    Lock lock(loader.mLock);
    areg::Model * model{ _find_model(modelId) };
    if (model != nullptr)
    {
        model->mUnloadAfterLoad = unload;
    }
}

bool SystemController::_is_unload_due( uint32_t modelId )
{
    ComponentLoader & loader{ ComponentLoader::instance() };
    Lock lock(loader.mLock);
    const areg::Model * model{ _find_model(modelId) };
    return ((model != nullptr) && model->mUnloadAfterLoad && (model->model_state() == areg::ModelState::Loaded));
}

uint32_t SystemController::_thread_count( uint32_t modelId )
{
    ComponentLoader & loader{ ComponentLoader::instance() };
    Lock lock(loader.mLock);
    const areg::Model * model{ _find_model(modelId) };
    return (model != nullptr ? model->thread_list().mListThreads.size() : 0u);
}

} // namespace areg
