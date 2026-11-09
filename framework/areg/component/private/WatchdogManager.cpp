/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/component/private/WatchdogManager.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, The Thread Watchdog Manager.
 *              Controlling and stopping stuck threads.
 *
 ************************************************************************/
#include "areg/component/private/WatchdogManager.hpp"
#include "areg/component/private/ServiceManager.hpp"
#include "areg/component/ExitEvent.hpp"
#include "areg/component/ComponentThread.hpp"

#include "areg/logging/areg_log.h"

#include <new>

namespace areg {

DEF_LOG_SCOPE(areg_component_private_WatchdogManager, _process_expired_timer);

//////////////////////////////////////////////////////////////////////////
// WatchdogManager class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Static functions
//////////////////////////////////////////////////////////////////////////

WatchdogManager& WatchdogManager::instance()
{
    // Never destroyed: threads that end during process exit may still release their watchdogs.
    alignas(WatchdogManager) static unsigned char _theStorage[sizeof(WatchdogManager)];
    static WatchdogManager * const _theWatchdogManager{ new (_theStorage) WatchdogManager() };
    return *_theWatchdogManager;
}

bool WatchdogManager::start_watchdog_manager()
{
    return instance().start_manager_thread();
}

void WatchdogManager::stop_watchdog_manager(bool waitComplete)
{
    instance().stop_manager_thread(waitComplete);
}

void WatchdogManager::wait_watchdog_manager()
{
    instance().wait_completion();
}

bool WatchdogManager::is_manager_started()
{
    return instance().is_ready();
}

bool WatchdogManager::start_timer(Watchdog& watchdog)
{
    bool result = false;
    ASSERT(watchdog.handle() != nullptr);
    WatchdogManager& watchdogManager = instance();
    if (watchdogManager.is_ready())
    {
        watchdogManager._register_watchdog(watchdog);
        result = WatchdogManager::_post_arm_event(watchdog);
        if (!result)
        {
            watchdogManager._unregister_watchdog(watchdog);
        }
    }

    return result;
}

bool WatchdogManager::rearm_timer(Watchdog& watchdog, uint32_t msTimeout)
{
    WatchdogManager& watchdogManager = instance();
    if (watchdogManager.is_ready() == false)
        return false;

    watchdogManager.mWatchdogLock.lock();
    WatchdogManager::_os_timer_stop(watchdog.handle());
    watchdog.mTimeoutInMs.store(msTimeout, std::memory_order_relaxed);
    ++ watchdog.mSequence;
    watchdogManager._register_watchdog(watchdog);
    watchdogManager.mWatchdogLock.unlock();

    const bool result{ WatchdogManager::_post_arm_event(watchdog) };
    if (!result)
    {
        watchdogManager._unregister_watchdog(watchdog);
    }

    return result;
}

void WatchdogManager::prepare_arm_event(Watchdog& watchdog)
{
    if (watchdog.timeout() == areg::WATCHDOG_IGNORE)
        return;

    WatchdogManager& watchdogManager = instance();
    TimerManagerEvent event(TimerManagerEventData(&watchdog), static_cast<TimerManagerEventConsumer&>(watchdogManager));
    if (event.is_valid())
    {
        // The target is written once: the copies posted later share this header.
        event.set_target_dispatcher(static_cast<DispatcherThread*>(&watchdogManager));
        watchdog.mArmEvent = std::move(event);
    }
}

bool WatchdogManager::_post_arm_event(Watchdog& watchdog)
{
    Event event{ watchdog.mArmEvent };
    return event.is_valid() && event.deliver_event();
}

void WatchdogManager::stop_timer(Watchdog& watchdog)
{
    WatchdogManager& watchdogManager = instance();
    if (watchdogManager.is_ready())
    {
        watchdogManager._unregister_watchdog(watchdog);
    }
    else
    {
        // A destroyed watchdog never stays in the list.
        Lock lock(watchdogManager.mWatchdogLock);
        watchdogManager._unlink_watchdog(watchdog);
    }
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////

WatchdogManager::WatchdogManager()
    : TimerManagerBase(WatchdogManager::WATCHDOG_THREAD_NAME, areg::SYSTEM_THREAD_STACK_NORMAL, areg::QUEUE_DEFAULT_RING_CAPACITY)

    , mWatchdogs    ( nullptr )
    , mWatchdogLock ( )
{
}

WatchdogManager::~WatchdogManager()
{
    _remove_all_watchdogs();
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

inline void WatchdogManager::_register_watchdog(Watchdog & watchdog)
{
    Lock lock(mWatchdogLock);
    if (watchdog.mRegistered == false)
    {
        watchdog.mRegistered     = true;
        watchdog.mPrevRegistered = nullptr;
        watchdog.mNextRegistered = mWatchdogs;
        if (mWatchdogs != nullptr)
        {
            mWatchdogs->mPrevRegistered = &watchdog;
        }

        mWatchdogs = &watchdog;
    }
}

inline Watchdog * WatchdogManager::_find_registered(const TimerBase * timer) const
{
    for (Watchdog * entry = mWatchdogs; entry != nullptr; entry = entry->mNextRegistered)
    {
        if (static_cast<const TimerBase *>(entry) == timer)
            return entry;
    }

    return nullptr;
}

Watchdog * WatchdogManager::_find_by_handle(TIMERHANDLE handle) const
{
    for (Watchdog * entry = mWatchdogs; entry != nullptr; entry = entry->mNextRegistered)
    {
        if (entry->handle() == handle)
            return entry;
    }

    return nullptr;
}

inline bool WatchdogManager::_is_registered(const Watchdog * watchdog) const
{
    for (const Watchdog * entry = mWatchdogs; entry != nullptr; entry = entry->mNextRegistered)
    {
        if (entry == watchdog)
            return true;
    }

    return false;
}

inline void WatchdogManager::_unlink_watchdog(Watchdog & watchdog)
{
    if (watchdog.mRegistered)
    {
        if (watchdog.mPrevRegistered != nullptr)
        {
            watchdog.mPrevRegistered->mNextRegistered = watchdog.mNextRegistered;
        }
        else
        {
            mWatchdogs = watchdog.mNextRegistered;
        }

        if (watchdog.mNextRegistered != nullptr)
        {
            watchdog.mNextRegistered->mPrevRegistered = watchdog.mPrevRegistered;
        }

        watchdog.mRegistered     = false;
        watchdog.mNextRegistered = nullptr;
        watchdog.mPrevRegistered = nullptr;
    }
}

inline void WatchdogManager::_unregister_watchdog(Watchdog& watchdog)
{
    do
    {
        Lock lock(mWatchdogLock);
        _unlink_watchdog(watchdog);
    } while (false);

    WatchdogManager::_os_timer_stop(watchdog.handle());
}

void WatchdogManager::_remove_all_watchdogs()
{
    // Takes one watchdog at a time under the lock and stops its OS timer outside it.
    for (;;)
    {
        TIMERHANDLE handle{ nullptr };
        do
        {
            Lock lock(mWatchdogLock);
            if (mWatchdogs != nullptr)
            {
                handle = mWatchdogs->handle();
                _unlink_watchdog(*mWatchdogs);
            }
        } while (false);

        if (handle == nullptr)
            break;

        WatchdogManager::_os_timer_stop(handle);
    }
}

void WatchdogManager::process_event(const TimerManagerEventData & data)
{
    ASSERT(data.timer() != nullptr);
    // A watchdog unregisters under this lock before it is destroyed.
    mWatchdogLock.lock();
    Watchdog * watchdog = _find_registered(data.timer());
    if ((watchdog != nullptr) && (WatchdogManager::_os_timer_start(*watchdog) == false))
    {
        _unregister_watchdog(*watchdog);
    }

    mWatchdogLock.unlock();
}

void WatchdogManager::_process_expired_timer(Watchdog* watchdog, Watchdog::WATCHDOG_ID watchdog_id, uint32_t /* hiBytes */, uint32_t /* loBytes */)
{
    LOG_SCOPE( areg_component_private_WatchdogManager, _process_expired_timer);

    bool expired{ false };
    bool deadline{ false };
    bool isWorker{ false };
    uint64_t instance{ 0u };

    mWatchdogLock.lock();

    const Watchdog::SEQUENCE_ID sequence = Watchdog::make_sequence_id(watchdog_id);
    if ((watchdog != nullptr) && _is_registered(watchdog) && (watchdog->sequence() == sequence))
    {
        expired = true;
        deadline = watchdog->is_deadline_guard();
        if (deadline == false)
        {
            const ComponentThread & componentThread{ watchdog->component_thread() };
            isWorker    = (watchdog->name() != componentThread.name());
            instance    = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(&componentThread));
            LOG_WARN("The watchdog [ %s ] of component thread [ %s ] has expired", watchdog->name().as_string(), componentThread.name().as_string());
        }
    }

    mWatchdogLock.unlock();

    bool reported{ true };
    if (deadline)
    {
        reported = ServiceManager::report_deadline_expired();
    }
    else if (expired)
    {
        reported = ServiceManager::report_watchdog_expired(instance, isWorker);
    }

    if (reported == false)
    {
        // The report is retried after the timeout of the watchdog, unless it was stopped or re-armed.
        LOG_WARN("The expiry of a watchdog cannot be reported, it is retried");
        mWatchdogLock.lock();
        if (_is_registered(watchdog) && (watchdog->sequence() == sequence) && (WatchdogManager::_os_timer_start(*watchdog) == false))
        {
            _unregister_watchdog(*watchdog);
        }

        mWatchdogLock.unlock();
    }
}

void WatchdogManager::ready_for_events(bool is_ready)
{
    if (is_ready == false)
    {
        _remove_all_watchdogs();
    }

    TimerManagerBase::ready_for_events(is_ready);
}

} // namespace areg
