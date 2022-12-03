/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/component/private/TimerManager.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, The System Timer Manager.
 *              Controlling, triggering and stopping timer.
 *
 ************************************************************************/
#include "areg/component/private/TimerManager.hpp"
#include "areg/component/private/ExitEvent.hpp"

#include "areg/component/Timer.hpp"
#include "areg/base/NEUtilities.hpp"
#include "areg/trace/GETrace.h"

DEF_TRACE_SCOPE(areg_component_private_TimerManager_startTimer);
DEF_TRACE_SCOPE(areg_component_private_TimerManager_processEvent);
DEF_TRACE_SCOPE(areg_component_private_TimerManager__registerTimer);
DEF_TRACE_SCOPE(areg_component_private_TimerManager__processExpiredTimers);

//////////////////////////////////////////////////////////////////////////
// TimerManager class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Static functions
//////////////////////////////////////////////////////////////////////////

TimerManager & TimerManager::getInstance( void )
{
    static TimerManager	_theTimerManager;
    return _theTimerManager;
}

bool TimerManager::startTimerManager( void )
{
    return getInstance().startTimerManagerThread( );
}

void TimerManager::stopTimerManager( void )
{
    getInstance().stopTimerManagerThread();
}

bool TimerManager::isTimerManagerStarted( void )
{
    return getInstance().isReady();
}

bool TimerManager::startTimer( Timer &timer )
{
    return TimerManager::startTimer(timer, DispatcherThread::getCurrentDispatcherThread());
}

bool TimerManager::startTimer(Timer &timer, const DispatcherThread & whichThread)
{
    TRACE_SCOPE(areg_component_private_TimerManager_startTimer);

    TimerManager & timerManager = getInstance();

    bool result = false;
    if ( timerManager.isTimerManagerStarted() )
    {
        if ( timerManager._registerTimer( timer, whichThread ) )
        {
            TRACE_DBG( "Registered timer [ %s ] for thread [ %p ], sending event to start timer", timer.getName( ).getString( ), whichThread.getId( ) );
            result = TimerManagerEvent::sendEvent( TimerManagerEventData(&timer)
                                                 , static_cast<IETimerManagerEventConsumer &>(timerManager)
                                                 , static_cast<DispatcherThread &>(timerManager) );
        }
        else
        {
            TRACE_ERR("Failed to register timer [ %s ]", timer.getName().getString());
        }
    }
    else
    {
        TRACE_ERR("The Timer manager service is not running, cannot start and process the timer.");
    }

    return result;
}

void TimerManager::stopTimer( Timer &timer )
{
    getInstance()._unregisterTimer( timer );
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////

TimerManager::TimerManager( void )
    : TimerManagerBase  ( TimerManager::TIMER_THREAD_NAME )

    , mTimerResource( )
{
}

TimerManager::~TimerManager( void )
{
    _removeAllTimers( );
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

bool TimerManager::_registerTimer(Timer &timer, const DispatcherThread & whichThread)
{
    TRACE_SCOPE(areg_component_private_TimerManager__registerTimer);

    bool result = false;
    if (timer.isValid() && whichThread.isValid() && whichThread.isRunning() )
    {
        TIMERHANDLE timerHandle = timer.getHandle();
        ASSERT(timerHandle != nullptr);
        mTimerResource.registerResourceObject(timerHandle, &timer);
        TRACE_DBG("Registered timer [ %s ]", timer.getName().getString());
        result = true;
    }
#ifdef DEBUG
    else
    {
        TRACE_ERR("Either times [ %s ] is not valid or thread [ %s ] is not running, cancel timer."
                        , timer.getName().getString()
                        , whichThread.getName().getString());
    }
#endif // DEBUG

    return result;
}

bool TimerManager::_registerTimer(Timer &timer, id_type whichThreadId)
{
    Thread * thread = Thread::findThreadById(whichThreadId);
    DispatcherThread * disp = thread != nullptr ? RUNTIME_CAST(thread, DispatcherThread) : nullptr;
    return (disp != nullptr ? _registerTimer(timer, *disp) : false);
}

void TimerManager::_unregisterTimer( Timer & timer )
{
    TIMERHANDLE handle = timer.getHandle();
    if (handle != nullptr)
    {
        mTimerResource.unregisterResourceObject(handle);
        TimerManager::_osSsystemTimerStop(handle);
    }
}

void TimerManager::_removeAllTimers( void )
{
    mTimerResource.lock();

    std::pair<TIMERHANDLE, Timer*> elem{ nullptr, nullptr };
    while (mTimerResource.isEmpty() == false)
    {
        mTimerResource.removeResourceFirstElement(elem);
        ASSERT(elem.second != nullptr);
        TimerManager::_osSsystemTimerStop(elem.first);
    }

    mTimerResource.unlock();
}

void TimerManager::processEvent( const TimerManagerEventData & data )
{
    TRACE_SCOPE(areg_component_private_TimerManager_processEvent);
    Timer* timer = static_cast<Timer*>(data.getTimer());
    ASSERT(timer != nullptr);
    if ( mTimerResource.existResource(timer->getHandle( )) )
    {
        TRACE_DBG( "Starting timer [ %s ] with timeout [ %u ] ms.", timer->getName( ).getString( ), timer->getTimeout( ) );
        TimerManager::_osSystemTimerStart( *timer );
    }
#ifdef DEBUG
    else
    {
        TRACE_WARN("The timer [ %s ] is not registered, ignoring to start.", timer->getName().getString());
    }
#endif // DEBUG
}

void TimerManager::_processExpiredTimer(Timer * timer, TIMERHANDLE handle, uint32_t hiBytes, uint32_t loBytes)
{
    TRACE_SCOPE(areg_component_private_TimerManager__processExpiredTimers);

    mTimerResource.lock();
    if (mTimerResource.existResource(handle))
    {
        if (timer->mDispatchThread != nullptr)
        {
            ASSERT(timer->getHandle() == handle);
            if (timer->timerIsExpired(hiBytes, loBytes, reinterpret_cast<ptr_type>(handle)) == false)
            {
                TRACE_WARN("Either the Timer [ %s ] is not active or cannot send anymore. Going to unregister", timer->getName().getString());
                _unregisterTimer(*timer);
                timer->mStarted = false;
            }
            else
            {
                TRACE_DBG("Send timer [ %s ] event to target [ %u ], continuing timer"
                            , timer->getName().getString()
                            , static_cast<uint32_t>(timer->mDispatchThread->getId()));
            }
        }
    }

    mTimerResource.unlock();
}

void TimerManager::readyForEvents( bool isReady )
{
    if (isReady == false)
    {
        _removeAllTimers( );
    }

    TimerManagerBase::readyForEvents(isReady);
}
