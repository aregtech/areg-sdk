/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/WatchdogManager.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, The Thread Watchdog Manager.
 *              Controlling and stopping stuck threads.
 *
 ************************************************************************/
#include "areg/component/private/WatchdogManager.hpp"

#include "areg/component/private/ExitEvent.hpp"
#include "areg/component/private/ServiceManager.hpp"
#include "areg/component/ComponentThread.hpp"

#include "areg/logging/areg_log.h"
namespace areg {

DEF_LOG_SCOPE(areg_component_private_WatchdogManager__processExpiredTimers);

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
    return instance().stop_manager_thread(waitComplete);
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
    }

    return result;
}

void WatchdogManager::stop_timer(Watchdog& watchdog)
{
    WatchdogManager& watchdogManager = instance();
    if (watchdogManager.is_ready())
    {
        instance()._unregister_watchdog(watchdog);
        WatchdogManager::_os_timer_stop(watchdog.handle());
    }
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////

WatchdogManager::WatchdogManager()
    : TimerManagerBase(WatchdogManager::WATCHDOG_THREAD_NAME)

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

inline void WatchdogManager::_unregister_watchdog(Watchdog& watchdog)
{
    mWatchdogResource.unregister_resource_object(watchdog.id());
    WatchdogManager::_os_timer_stop(watchdog.handle());
}

void WatchdogManager::_remove_all_watchdogs()
{
    mWatchdogResource.lock();

    std::pair< Watchdog::GUARD_ID, Watchdog*> elem;
    while (mWatchdogResource.is_empty() == false)
    {
        mWatchdogResource.remove_first_element(elem);
        ASSERT(elem.second != nullptr);
        WatchdogManager::_os_timer_stop(elem.second->handle());
    }

    mWatchdogResource.unlock();
}

void WatchdogManager::process_event(const TimerManagerEventData & data)
{
    Watchdog* watchdog = static_cast<Watchdog*>(data.timer());
    ASSERT(watchdog != nullptr);
    if (mWatchdogResource.exist(watchdog->id()))
    {
        WatchdogManager::_os_timer_start(*watchdog);
    }
}

void WatchdogManager::_process_expired_timer(Watchdog* watchdog, Watchdog::WATCHDOG_ID watchdog_id, uint32_t /* hiBytes */, uint32_t /* loBytes */)
{
    LOG_SCOPE(areg_component_private_WatchdogManager__processExpiredTimers);

    mWatchdogResource.lock();

    Watchdog::SEQUENCE_ID sequence  = Watchdog::make_sequence_id(watchdog_id);
    if ((watchdog != nullptr) && (watchdog->sequence() == sequence))
    {
        LOG_WARN("The watchdog [ %s ] has expired, terminating component thread [ %s ]"
                        , watchdog->name().as_string()
                        , watchdog->component_thread().name().as_string());

        ServiceManager::request_recreate_thread(watchdog->component_thread());
    }

    mWatchdogResource.unlock();
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
