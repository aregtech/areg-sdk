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

#include <vector>
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
    static WatchdogManager	_theWatchdogManager;
    return _theWatchdogManager;
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
        result = TimerManagerEvent::send_event( TimerManagerEventData(&watchdog)
                                             , static_cast<TimerManagerEventConsumer&>(watchdogManager)
                                             , static_cast<DispatcherThread&>(watchdogManager));
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

    watchdogManager.mWatchdogResource.lock();
    WatchdogManager::_os_timer_stop(watchdog.handle());
    watchdog.mTimeoutInMs = msTimeout;
    ++ watchdog.mSequence;
    watchdogManager._register_watchdog(watchdog);
    watchdogManager.mWatchdogResource.unlock();

    const bool result{ TimerManagerEvent::send_event( TimerManagerEventData(&watchdog)
                                                    , static_cast<TimerManagerEventConsumer&>(watchdogManager)
                                                    , static_cast<DispatcherThread&>(watchdogManager)) };
    if (!result)
    {
        watchdogManager._unregister_watchdog(watchdog);
    }

    return result;
}

void WatchdogManager::stop_timer(Watchdog& watchdog)
{
    WatchdogManager& watchdogManager = instance();
    if (watchdogManager.is_ready())
    {
        watchdogManager._unregister_watchdog(watchdog);
    }
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////

WatchdogManager::WatchdogManager()
    : TimerManagerBase(WatchdogManager::WATCHDOG_THREAD_NAME, areg::SYSTEM_THREAD_STACK_NORMAL, areg::QUEUE_DEFAULT_RING_CAPACITY)

    , mWatchdogResource()
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
    mWatchdogResource.register_resource_object(watchdog.id(), &watchdog);
}

inline bool WatchdogManager::_is_registered(const Watchdog * watchdog) const
{
    Watchdog::GUARD_ID key{ 0u };
    for (Watchdog * entry = mWatchdogResource.resource_first_key(key); entry != nullptr; entry = mWatchdogResource.resource_next_key(key))
    {
        if (entry == watchdog)
            return true;
    }

    return false;
}

inline void WatchdogManager::_unregister_watchdog(Watchdog& watchdog)
{
    mWatchdogResource.unregister_resource_object(watchdog.id());
    WatchdogManager::_os_timer_stop(watchdog.handle());
}

void WatchdogManager::_remove_all_watchdogs()
{
    // Drain the map under the lock, then stop OS timers outside it.
    std::vector<TIMERHANDLE> handles;

    mWatchdogResource.lock();
    std::pair<Watchdog::GUARD_ID, Watchdog*> elem;
    while (mWatchdogResource.is_empty() == false)
    {
        mWatchdogResource.remove_first_element(elem);
        ASSERT(elem.second != nullptr);
        handles.push_back(elem.second->handle());
    }
    mWatchdogResource.unlock();

    for (TIMERHANDLE handle : handles)
    {
        WatchdogManager::_os_timer_stop(handle);
    }
}

void WatchdogManager::process_event(const TimerManagerEventData & data)
{
    Watchdog* watchdog = static_cast<Watchdog*>(data.timer());
    ASSERT(watchdog != nullptr);
    // A watchdog unregisters under this lock before it is destroyed.
    mWatchdogResource.lock();
    if (_is_registered(watchdog) && (WatchdogManager::_os_timer_start(*watchdog) == false))
    {
        _unregister_watchdog(*watchdog);
    }

    mWatchdogResource.unlock();
}

void WatchdogManager::_process_expired_timer(Watchdog* watchdog, Watchdog::WATCHDOG_ID watchdog_id, uint32_t /* hiBytes */, uint32_t /* loBytes */)
{
    LOG_SCOPE( areg_component_private_WatchdogManager, _process_expired_timer);

    bool expired{ false };
    bool deadline{ false };
    String threadName;
    String guardedName;
    uint64_t instance{ 0u };

    mWatchdogResource.lock();

    const Watchdog::SEQUENCE_ID sequence = Watchdog::make_sequence_id(watchdog_id);
    if ((watchdog != nullptr) && mWatchdogResource.exist(watchdog->id()) && (watchdog->sequence() == sequence))
    {
        expired = true;
        deadline = watchdog->is_deadline_guard();
        if (deadline == false)
        {
            const ComponentThread & componentThread{ watchdog->component_thread() };
            threadName  = componentThread.name();
            guardedName = watchdog->name();
            instance    = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(&componentThread));
            LOG_WARN("The watchdog [ %s ] of component thread [ %s ] has expired", guardedName.as_string(), threadName.as_string());
        }
    }

    mWatchdogResource.unlock();

    if (deadline)
    {
        ServiceManager::report_deadline_expired();
    }
    else if (expired)
    {
        ServiceManager::report_watchdog_expired(threadName, instance, guardedName);
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
