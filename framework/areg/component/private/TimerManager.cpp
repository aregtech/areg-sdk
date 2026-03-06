/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/TimerManager.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, The System Timer Manager.
 *              Controlling, triggering and stopping timer.
 *
 ************************************************************************/
#include "areg/component/private/TimerManager.hpp"
#include "areg/component/private/ExitEvent.hpp"

#include "areg/component/Timer.hpp"
#include "areg/base/UtilityDefs.hpp"
#include "areg/logging/areg_log.h"
namespace areg {

DEF_LOG_SCOPE(areg_component_private_TimerManager_startTimer);
DEF_LOG_SCOPE(areg_component_private_TimerManager_processEvent);
DEF_LOG_SCOPE(areg_component_private_TimerManager__registerTimer);
DEF_LOG_SCOPE(areg_component_private_TimerManager__processExpiredTimers);

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
    LOG_SCOPE(areg_component_private_TimerManager_startTimer);

    TimerManager & timerManager = instance();

    bool result = false;
    if ( timerManager.is_manager_started() )
    {
        if ( timerManager._register_timer( timer, whichThread ) )
        {
            LOG_DBG( "Registered timer [ %s ] for thread [ %p ], sending event to start timer", timer.name( ).as_string( ), whichThread.id( ) );
            result = TimerManagerEvent::send_event( TimerManagerEventData(&timer)
                                                 , static_cast<TimerManagerEventConsumer &>(timerManager)
                                                 , static_cast<DispatcherThread &>(timerManager) );
        }
        else
        {
            LOG_ERR("Failed to register timer [ %s ]", timer.name().as_string());
        }
    }
    else
    {
        LOG_ERR("The Timer manager service is not running, cannot start and process the timer.");
    }

    return result;
}

void TimerManager::stop_timer( Timer &timer )
{
    instance()._unregister_timer( timer );
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////

TimerManager::TimerManager()
    : TimerManagerBase  ( TimerManager::TIMER_THREAD_NAME )

    , mTimerResource( )
{
}

TimerManager::~TimerManager()
{
    _remove_all_timers( );
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

bool TimerManager::_register_timer(Timer &timer, const DispatcherThread & whichThread)
{
    LOG_SCOPE(areg_component_private_TimerManager__registerTimer);

    bool result = false;
    if (timer.is_valid() && whichThread.is_valid() && whichThread.is_running() )
    {
        TIMERHANDLE timerHandle = timer.handle();
        ASSERT(timerHandle != nullptr);
        mTimerResource.register_resource_object(timerHandle, &timer);
        LOG_DBG("Registered timer [ %s ]", timer.name().as_string());
        result = true;
    }
#ifdef DEBUG
    else
    {
        LOG_ERR("Either times [ %s ] is not valid or thread [ %s ] is not running, cancel timer."
                        , timer.name().as_string()
                        , whichThread.name().as_string());
    }
#endif // DEBUG

    return result;
}

bool TimerManager::_register_timer(Timer &timer, id_type whichThreadId)
{
    Thread * thread = Thread::find_by_id(whichThreadId);
    DispatcherThread * disp = thread != nullptr ? AREG_RUNTIME_CAST(thread, DispatcherThread) : nullptr;
    return (disp != nullptr ? _register_timer(timer, *disp) : false);
}

void TimerManager::_unregister_timer( Timer & timer )
{
    TIMERHANDLE handle = timer.handle();
    if (handle != nullptr)
    {
        mTimerResource.unregister_resource_object(handle);
        TimerManager::_os_timer_stop(handle);
    }
}

void TimerManager::_remove_all_timers()
{
    mTimerResource.lock();

    std::pair<TIMERHANDLE, Timer*> elem{ nullptr, nullptr };
    while (mTimerResource.is_empty() == false)
    {
        mTimerResource.remove_first_element(elem);
        ASSERT(elem.second != nullptr);
        TimerManager::_os_timer_stop(elem.first);
    }

    mTimerResource.unlock();
}

void TimerManager::process_event( const TimerManagerEventData & data )
{
    LOG_SCOPE(areg_component_private_TimerManager_processEvent);
    Timer* timer = static_cast<Timer*>(data.timer());
    ASSERT(timer != nullptr);
    if ( mTimerResource.exist(timer->handle( )) )
    {
        LOG_DBG( "Starting timer [ %s ] with timeout [ %u ] ms.", timer->name( ).as_string( ), timer->timeout( ) );
        TimerManager::_os_timer_start( *timer );
    }
#ifdef DEBUG
    else
    {
        LOG_WARN("The timer [ %s ] is not registered, ignoring to start.", timer->name().as_string());
    }
#endif // DEBUG
}

void TimerManager::_process_expired_timer(Timer * timer, TIMERHANDLE handle, uint32_t hiBytes, uint32_t loBytes)
{
    LOG_SCOPE(areg_component_private_TimerManager__processExpiredTimers);

    mTimerResource.lock();
    if (mTimerResource.exist(handle))
    {
        if (timer->mDispatchThread != nullptr)
        {
            ASSERT(timer->handle() == handle);
            if (timer->timer_is_expired(hiBytes, loBytes, reinterpret_cast<ptr_type>(handle)) == false)
            {
                LOG_WARN("Either the Timer [ %s ] is not active or cannot send anymore. Going to unregister", timer->name().as_string());
                _unregister_timer(*timer);
                timer->mStarted = false;
            }
            else
            {
                LOG_DBG("Send timer [ %s ] event to target [ %u ], continuing timer"
                            , timer->name().as_string()
                            , static_cast<uint32_t>(timer->mDispatchThread->id()));
            }
        }
    }

    mTimerResource.unlock();
}

void TimerManager::ready_for_events( bool is_ready )
{
    if (is_ready == false)
    {
        _remove_all_timers( );
    }

    TimerManagerBase::ready_for_events(is_ready);
}

} // namespace areg
