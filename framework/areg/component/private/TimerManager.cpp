/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/component/private/TimerManager.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, The System Timer Manager.
 *              Controlling, triggering and stopping timer.
 *
 ************************************************************************/
#include "areg/component/private/TimerManager.hpp"
#include "areg/component/ExitEvent.hpp"

#include "areg/component/Timer.hpp"
#include "areg/base/UtilityDefs.hpp"

#include <atomic>

namespace areg {

namespace
{
    //!< True while the timer manager singleton exists.
    std::atomic_bool _theManagerAlive{ false };
}

//////////////////////////////////////////////////////////////////////////
// TimerManager class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Static functions
//////////////////////////////////////////////////////////////////////////

TimerManager & TimerManager::instance()
{
    static TimerManager	_theTimerManager;
    return _theTimerManager;
}

bool TimerManager::start_timer_manager()
{
    return instance().start_manager_thread( );
}

void TimerManager::stop_timer_manager(bool waitComplete)
{
    instance().stop_manager_thread(waitComplete);
}

void TimerManager::wait_timer_manager()
{
    instance().wait_completion();
}

bool TimerManager::is_manager_started()
{
    return instance().is_ready();
}

bool TimerManager::start_timer( Timer &timer )
{
    return TimerManager::start_timer(timer, DispatcherThread::current_dispatcher_thread());
}

bool TimerManager::start_timer(Timer &timer, const DispatcherThread & whichThread)
{
    TimerManager & timerManager = instance();
    bool result = false;
    if ( timerManager.is_ready() )
    {
        if ( timerManager._register_timer( timer, whichThread ) )
        {
            result = TimerManagerEvent::send_event( TimerManagerEventData(&timer)
                                                 , static_cast<TimerManagerEventConsumer &>(timerManager)
                                                 , static_cast<DispatcherThread &>(timerManager) );
            if (!result)
            {
                timerManager._unregister_timer(timer);
            }
        }
    }

    return result;
}

void TimerManager::stop_timer( Timer &timer )
{
    // Does nothing when the timer manager singleton is already destroyed.
    if (_theManagerAlive.load(std::memory_order_acquire))
    {
        instance()._unregister_timer(timer);
    }
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////

TimerManager::TimerManager()
    : TimerManagerBase  ( TimerManager::TIMER_THREAD_NAME, areg::SYSTEM_THREAD_STACK_NORMAL, areg::QUEUE_TIMER_RING_CAPACITY )

    , mTimerResource( )
{
    _theManagerAlive.store(true, std::memory_order_release);
}

TimerManager::~TimerManager()
{
    _remove_all_timers( );
    _theManagerAlive.store(false, std::memory_order_release);
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

bool TimerManager::_register_timer(Timer &timer, const DispatcherThread & whichThread)
{
    Lock resourceLock(mTimerResource.lockable());
    Lock timerLock(timer.mLock);
    bool result = false;
    if (timer.is_valid() && whichThread.is_valid() && whichThread.is_running() )
    {
        TIMERHANDLE timerHandle = timer.handle();
        ASSERT(timerHandle != nullptr);
        mTimerResource.register_resource_object(timerHandle, &timer);
        result = true;
    }

    return result;
}

void TimerManager::_unregister_timer( Timer & timer )
{
    Lock resourceLock(mTimerResource.lockable());
    TIMERHANDLE handle = timer.handle();
    if (handle != nullptr)
    {
        mTimerResource.unregister_resource_object(handle);
        TimerManager::_os_timer_stop(handle);
    }
}

void TimerManager::_remove_all_timers()
{
    // Keep each OS handle alive until it has been disarmed.
    Lock resourceLock(mTimerResource.lockable());
    std::pair<TIMERHANDLE, Timer*> elem{ nullptr, nullptr };
    while (mTimerResource.is_empty() == false)
    {
        mTimerResource.remove_first_element(elem);
        ASSERT(elem.second != nullptr);
        TimerManager::_os_timer_stop(elem.first);
    }
}

void TimerManager::process_event( const TimerManagerEventData & data )
{
    Lock resourceLock(mTimerResource.lockable());
    Timer* timer = mTimerResource.find_resource_object(data.handle());
    if ((timer != nullptr) && (timer == data.timer()))
    {
        Lock timerLock(timer->mLock);
        if (TimerManager::_os_timer_start(*timer) == false)
        {
            _unregister_timer(*timer);

            timer->mStarted        = false;
            timer->mActive         = false;
            timer->mDispatchThread = nullptr;
        }
    }
}

void TimerManager::_process_expired_timer(Timer * timer, TIMERHANDLE handle, uint32_t hiBytes, uint32_t loBytes)
{
    bool    shouldStop{ false };    // the timer left the map and its OS timer must be disarmed

    Lock resourceLock(mTimerResource.lockable());

    if ((timer != nullptr) && (mTimerResource.find_resource_object(handle) == timer))
    {
        Lock timerLock(timer->mLock);
        ASSERT(timer->handle() == handle);
        const bool noTarget{ timer->mDispatchThread == nullptr };

        shouldStop = noTarget || (timer->timer_is_expired(hiBytes, loBytes, reinterpret_cast<ptr_type>(handle)) == false);

        if (shouldStop)
        {
            mTimerResource.unregister_resource_object(handle);
            timer->mStarted = false;
        }
        else
        {
            ASSERT(timer->mActive);
        }
    }

    if (shouldStop)
    {
        TimerManager::_os_timer_stop(handle);
    }
}

void TimerManager::ready_for_events( bool is_ready )
{
    if (!is_ready)
    {
        _remove_all_timers( );
    }

    TimerManagerBase::ready_for_events(is_ready);
}

} // namespace areg
