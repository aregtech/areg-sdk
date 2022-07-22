/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/component/private/TimerManager.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, The System Timer Manager.
 *              Controlling, triggering and stopping timer.
 *
 ************************************************************************/
#include "areg/component/private/TimerManager.hpp"

#include "areg/component/IETimerConsumer.hpp"
#include "areg/component/private/TimerEventData.hpp"
#include "areg/component/private/ExitEvent.hpp"

#include "areg/component/Timer.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/NEUtilities.hpp"
#include "areg/trace/GETrace.h"

DEF_TRACE_SCOPE(areg_component_private_TimerManager_runDispatcher);
DEF_TRACE_SCOPE(areg_component_private_TimerManager_startTimer);
DEF_TRACE_SCOPE(areg_component_private_TimerManager_processEvent);
DEF_TRACE_SCOPE(areg_component_private_TimerManager__registerTimer);
DEF_TRACE_SCOPE(areg_component_private_TimerManager__processExpiredTimers);
DEF_TRACE_SCOPE(areg_component_private_TimerManager__startSystemTimer);

//////////////////////////////////////////////////////////////////////////
// TimerManager class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Implement Runtime
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME(TimerManager, DispatcherThread)

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
    return getInstance()._startTimerManagerThread( );
}

void TimerManager::stopTimerManager( void )
{
    return getInstance()._stopTimerManagerThread();
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
            TimerManagingEventData data( TimerManagingEventData::eTimerAction::TimerStart, &timer, nullptr );
            result = TimerManagingEvent::sendEvent( data, static_cast<IETimerManagingEventConsumer &>(timerManager), static_cast<DispatcherThread &>(timerManager) );
        }
    }
    else
    {
        TRACE_ERR("The Timer manager service is not running, cannot start and process the timer.");
    }

    return result;
}

bool TimerManager::stopTimer( Timer &timer )
{
    return getInstance()._unregisterTimer( &timer );
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////

TimerManager::TimerManager( void )
    : DispatcherThread            ( TimerManager::TIMER_THREAD_NAME.data() )
    , IETimerManagingEventConsumer ( )

    , mTimerTable   ( )
    , mExpiredTimers( )
    , mTimerResource( )
    , mLock         ( false )
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
        TIMERHANDLE timerHandle = TimerManager::_createWaitableTimer(timer.hasName() ? timer.getName().getString() : nullptr);
        if (timerHandle != nullptr)
        {
            TimerManager::getInstance().mTimerResource.registerResourceObject(timerHandle, &timer);

            TRACE_DBG("Succeeded to create waitable object for timer [ %s ], going to register", timer.getName().getString());

            do
            {
                Lock lock(mLock);

                result = true;
                mTimerTable.registerObject(&timer, TimerInfo(timer, timerHandle, whichThread.getId()) );
            } while (false);
            
            TRACE_DBG("Registered timer [ %s ]", timer.getName().getString());
        }
        else
        {
            TRACE_ERR("Failed to create waitable object for timer [ %s ], going to register", timer.getName().getString());
        }
    }
    else
    {
        TRACE_ERR("Either times [ %s ] is not valid or thread [ %s ] is not running, cancel timer.", timer.getName().getString(), whichThread.getName().getString());
    }

    return result;
}

bool TimerManager::_registerTimer(Timer &timer, id_type whichThreadId)
{
    Thread * thread = Thread::findThreadById(whichThreadId);
    DispatcherThread * disp = thread != nullptr ? RUNTIME_CAST(thread, DispatcherThread) : nullptr;
    return (disp != nullptr ? _registerTimer(timer, *disp) : false);
}

bool TimerManager::_unregisterTimer( Timer * timer )
{
    bool result = false;

    if (timer != nullptr)
    {
        Lock lock(mLock);

        TimerInfo timerInfo;
        mExpiredTimers.removeAllTimers(timer);
        if ( mTimerTable.unregisterObject(timer, timerInfo) )
        {
            ASSERT(timerInfo.getHandle() != nullptr);
            TimerManager::getInstance().mTimerResource.unregisterResourceObject(timerInfo.getHandle());
            TimerManager::_stopSystemTimer(timerInfo.getHandle());

            TimerManagingEventData data(TimerManagingEventData::eTimerAction::TimerRemove, timer, timerInfo.getHandle());
            TimerManagingEvent::sendEvent(data, static_cast<IETimerManagingEventConsumer &>(self()), static_cast<DispatcherThread &>(self()));
            result = true;

            OUTPUT_DBG("Successfully unregistered timer [ %s ].", timer->getName().getString());
        }
    }

    return result;
}

void TimerManager::_removeAllTimers( void )
{
    getInstance().mTimerResource.removeAllResources();

    do
    {
        Lock lock(mLock);

        Timer*    timer  = nullptr;
        TimerInfo timerInfo;
        while ( mTimerTable.isEmpty() == false )
        {
            VERIFY(mTimerTable.unregisterFirstObject(timer, timerInfo));
            ASSERT((timer != nullptr) && (timerInfo.getHandle() != nullptr));
            TimerManager::_destroyWaitableTimer( timerInfo.getHandle( ), true );
        }

        mTimerTable.clear();
        mExpiredTimers.clear();
    } while (false);
    
}

void TimerManager::processEvent( const TimerManagingEventData & data )
{
    switch(data.getAction())
    {
    case TimerManagingEventData::eTimerAction::TimerStart:
    {
    	TRACE_SCOPE(areg_component_private_TimerManager_processEvent);
    	Timer *timer = data.getTimer();
    	ASSERT(timer != nullptr);
    	TRACE_DBG("Starting timer [ %s ] with timeout [ %u ] ms.", timer->getName().getString(), timer->getTimeout());
        _startSystemTimer(timer);
    }
    break;

    case TimerManagingEventData::eTimerAction::TimerExpired:
        _processExpiredTimers();
        break;

    case TimerManagingEventData::eTimerAction::TimerRemove: // fall through
        TimerManager::_destroyWaitableTimer(data.getHandle(), false);
        break;

    case TimerManagingEventData::eTimerAction::TimerIgnore: // fall through
    case TimerManagingEventData::eTimerAction::TimerStop:   // fall through
    case TimerManagingEventData::eTimerAction::TimerCancel: // fall through
    default:
        break; // ignore
    }
}

void TimerManager::_processExpiredTimers( void )
{
    do 
    {
        TRACE_SCOPE(areg_component_private_TimerManager__processExpiredTimers);
        Thread::switchThread();
        Lock lock(mLock);

        TRACE_DBG("There are [ %d ] expired timers in the list to process", mExpiredTimers.getSize());

        if (mExpiredTimers.isEmpty())
        {
            TRACE_INFO("No more expired timer, finishing the job.");
            break; // exit loop!!!
        }

        ExpiredTimerInfo expiredTime = mExpiredTimers.popTimer();
        Timer * currTimer     = expiredTime.mTimer;
        TimerInfo * timerInfo = currTimer != nullptr ? mTimerTable.findObject(currTimer) : nullptr;

        if (timerInfo != nullptr)
        {
            ASSERT(currTimer != nullptr);
            id_type threadId = timerInfo->mOwnThreadId;

            if ( timerInfo->canContinueTimer(expiredTime))
            {
                TRACE_DBG("Send timer [ %s ] event to target [ %u ], continuing timer", currTimer->getName().getString(), static_cast<unsigned int>(timerInfo->mOwnThreadId));
            }
            else
            {
                TRACE_WARN("Either the Timer [ %s ] is not active or cannot send anymore. Going to unregister", currTimer->getName().getString());
                _unregisterTimer(currTimer);
            }

            TimerEvent::sendEvent( *currTimer, threadId );
        }
        else
        {
            TRACE_WARN("Invalid timer in the list of [%d ] expired timers. Cannot get timer [ %p ] information from map of size [ %d ] entries"
                            , mExpiredTimers.getSize()
                            , currTimer
                            , mTimerTable.getSize());
            break;
        }
    } while (true);
}

void TimerManager::_timerExpired( Timer* whichTimer, unsigned int highValue, unsigned int lowValue )
{
    do 
    {
        Lock lock(mLock);

        ExpiredTimerInfo expiredTimer(whichTimer, highValue, lowValue);
        mExpiredTimers.pushTimer( expiredTimer );
    } while (false);

    TimerManagingEventData data(TimerManagingEventData::eTimerAction::TimerExpired, nullptr, nullptr);
    TimerManagingEvent::sendEvent(data, static_cast<IETimerManagingEventConsumer &>(self()), static_cast<DispatcherThread &>(self()));
}

void TimerManager::_startSystemTimer( Timer* whichTimer )
{
	TRACE_SCOPE(areg_component_private_TimerManager__startSystemTimer);

    if (whichTimer != nullptr)
    {
        Lock lock(mLock);

        TimerInfo * timerInfo = mTimerTable.findObject(whichTimer);
        if ( timerInfo != nullptr )
        {
            ASSERT(timerInfo->mTimer == whichTimer);
            ASSERT(timerInfo->mHandle != nullptr);
            if (_startSystemTimer(*timerInfo))
            {
                TRACE_DBG("Succeeded starting timer [ %s ]", whichTimer->getName().getString());
            }
            else
            {
                TRACE_ERR("Failed starting timer [ %s ]", whichTimer->getName().getString());
            }
        }
        else
        {
            TRACE_WARN("The timer object is not found in the table. Ignoring request to start timer");
        }
    }
    else
    {
        TRACE_ERR("Invalid nullptr pointer to Timer object. Ignoring starting timer.");
    }
}

bool TimerManager::_startSystemTimer( TimerInfo &timerInfo )
{
    bool result = false;
    if (timerInfo.canStartTimer())
    {
        result = TimerManager::_createSystemTimer(timerInfo, mTimerTable);
    }
    else
    {
    	TRACE_SCOPE(areg_component_private_TimerManager__startSystemTimer);
        TRACE_ERR("Ignoring firing timer because it is not in Idle state. The current state of timer [ %s ] is [ %s ]"
                    , timerInfo.getTimer()->getName().getString()
                    , TimerInfo::getString(timerInfo.mTimerState));
    }

    return result;
}

bool TimerManager::postEvent( Event& eventElem )
{
    bool result = false;
    if ( RUNTIME_CAST(&eventElem, TimerManagingEvent) != nullptr)
    {
        result = EventDispatcher::postEvent(eventElem);
    }
    else
    {
        OUTPUT_ERR("Not a TimerManagingEvent type event, cannot Post. Destroying event type [ %s ]", eventElem.getRuntimeClassName().getString());
        eventElem.destroy();
    }

    return result;
}

bool TimerManager::runDispatcher( void )
{
    IESynchObject* synchObjects[] = {&mEventExit, &mEventQueue};
    MultiLock multiLock(synchObjects, 2, false);

    int whichEvent  = static_cast<int>(EventDispatcherBase::eEventOrder::EventError);
    const ExitEvent& exitEvent = ExitEvent::getExitEvent();

    readyForEvents(true);
    do 
    {
        whichEvent = multiLock.lock(NECommon::WAIT_INFINITE, false, true);
        Event* eventElem = whichEvent == static_cast<int>(EventDispatcherBase::eEventOrder::EventQueue) ? pickEvent() : nullptr;
        if ( static_cast<const Event *>(eventElem) != static_cast<const Event *>(&exitEvent) )
        {
            TRACE_SCOPE(areg_component_private_TimerManager_runDispatcher);
            if ( whichEvent == static_cast<int>(EventDispatcherBase::eEventOrder::EventQueue) )
            {
                // proceed one external event.
                if (prepareDispatchEvent(eventElem))
                {
                    dispatchEvent(*eventElem);
                }
                 
                postDispatchEvent(eventElem);

                ASSERT(static_cast<EventQueue &>(mInternalEvents).isEmpty());
            }
        }
        else
        {
            OUTPUT_DBG("Received exit event. Going to exit System Timer Thread!");
            whichEvent = static_cast<int>(EventDispatcherBase::eEventOrder::EventExit);
        }

    } while (whichEvent == static_cast<int>(EventDispatcherBase::eEventOrder::EventQueue) || (whichEvent == MultiLock::LOCK_INDEX_COMPLETION));

    readyForEvents(false);
    removeAllEvents( );

    ASSERT(static_cast<EventQueue &>(mInternalEvents).isEmpty());

    return (whichEvent == static_cast<int>(EventDispatcherBase::eEventOrder::EventExit));
}

void TimerManager::readyForEvents( bool isReady )
{
    if (isReady)
    {
        TimerManagingEvent::addListener(static_cast<IETimerManagingEventConsumer &>(self()), static_cast<DispatcherThread &>(self()));

        mHasStarted = true;
        mEventStarted.setEvent();
    }
    else
    {
        TimerManagingEvent::removeListener(static_cast<IETimerManagingEventConsumer &>(self()), static_cast<DispatcherThread &>(self()));

        _removeAllTimers( );
        mHasStarted = false;
        mEventStarted.resetEvent();
    }
}

bool TimerManager::_startTimerManagerThread( void )
{
    bool result = false;
    if ( isReady() == false )
    {
        ASSERT(isRunning() == false);
        result = createThread(NECommon::WAIT_INFINITE) && waitForDispatcherStart(NECommon::WAIT_INFINITE);
#ifdef _DEBUG
        if ( result == false )
        {
            OUTPUT_ERR("Failed to create [ %s ] System Timer Thread", TimerManager::TIMER_THREAD_NAME.data());
        }
#endif  // _DEBUG
    }
    else
    {
        result = true;
    }

    return result;
}

void TimerManager::_stopTimerManagerThread( void )
{
    destroyThread(NECommon::WAIT_INFINITE);
}
