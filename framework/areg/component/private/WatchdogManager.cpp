/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/component/private/WatchdogManager.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, The Thread Watchdog Manager.
 *              Controlling and stopping stuck threads.
 *
 ************************************************************************/
#include "areg/component/private/WatchdogManager.hpp"

#include "areg/component/private/ExitEvent.hpp"
#include "areg/base/Thread.hpp"
#include "areg/trace/GETrace.h"

DEF_TRACE_SCOPE(areg_component_private_WatchdogManager_runDispatcher);
DEF_TRACE_SCOPE(areg_component_private_WatchdogManager_startTimer);
DEF_TRACE_SCOPE(areg_component_private_WatchdogManager_processEvent);
DEF_TRACE_SCOPE(areg_component_private_WatchdogManager__registerWatchdog);
DEF_TRACE_SCOPE(areg_component_private_WatchdogManager__processExpiredTimers);
DEF_TRACE_SCOPE(areg_component_private_WatchdogManager__startSystemTimer);

//////////////////////////////////////////////////////////////////////////
// WatchdogManager class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Implement Runtime
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME(WatchdogManager, DispatcherThread)

//////////////////////////////////////////////////////////////////////////
// Static functions
//////////////////////////////////////////////////////////////////////////

WatchdogManager& WatchdogManager::getInstance(void)
{
    static WatchdogManager	_theWatchdogManager;
    return _theWatchdogManager;
}

bool WatchdogManager::startWatchdogManager(void)
{
    return getInstance()._startWatchdogManagerThread();
}

void WatchdogManager::stopWatchdogManager(void)
{
    return getInstance()._stopWatchdogManagerThread();
}

bool WatchdogManager::isWatchdogManagerStarted(void)
{
    return getInstance().isReady();
}

bool WatchdogManager::startTimer(Watchdog& watchdog)
{
    TRACE_SCOPE(areg_component_private_WatchdogManager_startTimer);

    bool result = false;
    ASSERT(watchdog.getHandle() != nullptr);
    WatchdogManager& watchdogManager = getInstance();

    if (watchdogManager.isWatchdogManagerStarted())
    {
        watchdogManager._registerWatchdog(watchdog);
        TRACE_DBG("Registered timer [ %s ] for thread [ %p ], sending event to start timer", watchdog.getName().getString(), watchdog.getThread().getId());
        WatchdogManagerEventData data(WatchdogManagerEventData::eTimerAction::TimerStart, watchdog);
        result = WatchdogManagerEvent::sendEvent(data, static_cast<IEWatchdogManagerEventConsumer&>(watchdogManager), static_cast<DispatcherThread&>(watchdogManager));
    }
    else
    {
        TRACE_ERR("The Timer manager service is not running, cannot start and process the timer.");
    }

    return result;
}

void WatchdogManager::stopTimer(Watchdog& watchdog)
{
    getInstance()._unregisterWatchdog(watchdog);
    WatchdogManager::_systemTimerStop(watchdog);
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////

WatchdogManager::WatchdogManager(void)
    : DispatcherThread(WatchdogManager::WATCHDOG_THREAD_NAME)
    , IEWatchdogManagerEventConsumer()

    , mWatchdogResource()
    , mLock(false)
{
}

WatchdogManager::~WatchdogManager(void)
{
    _removeAllWatchdogs();
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

void WatchdogManager::_registerWatchdog(Watchdog & watchdog)
{
    TRACE_SCOPE(areg_component_private_WatchdogManager__registerWatchdog);

    ASSERT(watchdog.mTimer != nullptr);
    mWatchdogResource.registerResourceObject(watchdog.getId(), &watchdog);
    TRACE_DBG("Registered watchdog [ %s ]", watchdog.getName().getString());
}

void WatchdogManager::_unregisterWatchdog(Watchdog& watchdog)
{
    mWatchdogResource.unregisterResourceObject(watchdog.getId());
}

void WatchdogManager::_removeAllWatchdogs(void)
{
    mWatchdogResource.lock();
    Watchdog::GUARD_ID guardId{ 0 };
    for (Watchdog* watchdog = mWatchdogResource.resourceFirstKey(guardId); watchdog != nullptr; watchdog = mWatchdogResource.resourceNextKey(guardId))
    {
        ASSERT(watchdog->getHandle() != nullptr);
        WatchdogManager::_systemTimerStop(*watchdog);
    }

    mWatchdogResource.removeAllResources();
    mWatchdogResource.unlock();
}

void WatchdogManager::processEvent(const WatchdogManagerEventData& data)
{
    switch (data.getAction())
    {
    case WatchdogManagerEventData::eTimerAction::TimerStart:
    {
        TRACE_SCOPE(areg_component_private_WatchdogManager_processEvent);
        Watchdog* watchdog = data.getWatchdog();
        if (watchdog->isActive())
        {
            TRACE_DBG("Starting timer [ %s ] with timeout [ %u ] ms.", watchdog->getName().getString(), watchdog->getTimeout());
            _systemTimerStart(*watchdog);
        }
    }
    break;

    case WatchdogManagerEventData::eTimerAction::TimerExpired:
    {
        TRACE_SCOPE(areg_component_private_WatchdogManager_processEvent);
        Watchdog* watchdog = data.getWatchdog();
        ASSERT(watchdog != nullptr);
        TRACE_DBG("Watchdog [ %s ] timeout [ %u ] ms expired, checking the state.", watchdog->getName().getString(), watchdog->getTimeout());
        watchdog->timeoutExpired(data.getWatchdogId());
    }
    break;

    case WatchdogManagerEventData::eTimerAction::TimerIgnore: // fall through
    case WatchdogManagerEventData::eTimerAction::TimerStop:   // fall through
    case WatchdogManagerEventData::eTimerAction::TimerCancel: // fall through
    default:
        break; // ignore
    }
}

void WatchdogManager::_timerExpired(Watchdog::WATCHDOG_ID watchdogId, unsigned int highValue, unsigned int lowValue)
{
    Watchdog::GUARD_ID guardId      = Watchdog::makeGuardId(watchdogId);
    Watchdog* watchdog = mWatchdogResource.findResourceObject(guardId);
    if ((watchdog != nullptr) && (watchdog->getSequence() == Watchdog::makeSequenceId(watchdogId)))
    {
        WatchdogManagerEventData data(WatchdogManagerEventData::eTimerAction::TimerExpired, *watchdog, watchdogId);
        WatchdogManagerEvent::sendEvent(data, static_cast<IEWatchdogManagerEventConsumer&>(self()), static_cast<DispatcherThread&>(self()));
    }

}

bool WatchdogManager::postEvent(Event& eventElem)
{
    bool result = false;
    if (RUNTIME_CAST(&eventElem, WatchdogManagerEvent) != nullptr)
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

bool WatchdogManager::runDispatcher(void)
{
    IESynchObject* synchObjects[] = { &mEventExit, &mEventQueue };
    MultiLock multiLock(synchObjects, 2, false);

    int whichEvent = static_cast<int>(EventDispatcherBase::eEventOrder::EventError);
    const ExitEvent& exitEvent = ExitEvent::getExitEvent();

    readyForEvents(true);
    do
    {
        whichEvent = multiLock.lock(NECommon::WAIT_INFINITE, false, true);
        Event* eventElem = whichEvent == static_cast<int>(EventDispatcherBase::eEventOrder::EventQueue) ? pickEvent() : nullptr;
        if (static_cast<const Event*>(eventElem) != static_cast<const Event*>(&exitEvent))
        {
            TRACE_SCOPE(areg_component_private_WatchdogManager_runDispatcher);
            if (whichEvent == static_cast<int>(EventDispatcherBase::eEventOrder::EventQueue))
            {
                // proceed one external event.
                if (prepareDispatchEvent(eventElem))
                {
                    dispatchEvent(*eventElem);
                }

                postDispatchEvent(eventElem);

                ASSERT(static_cast<EventQueue&>(mInternalEvents).isEmpty());
            }
        }
        else
        {
            OUTPUT_DBG("Received exit event. Going to exit System Timer Thread!");
            whichEvent = static_cast<int>(EventDispatcherBase::eEventOrder::EventExit);
        }

    } while (whichEvent == static_cast<int>(EventDispatcherBase::eEventOrder::EventQueue) || (whichEvent == MultiLock::LOCK_INDEX_COMPLETION));

    readyForEvents(false);
    removeAllEvents();

    ASSERT(static_cast<EventQueue&>(mInternalEvents).isEmpty());

    return (whichEvent == static_cast<int>(EventDispatcherBase::eEventOrder::EventExit));
}

void WatchdogManager::readyForEvents(bool isReady)
{
    if (isReady)
    {
        WatchdogManagerEvent::addListener(static_cast<IEWatchdogManagerEventConsumer&>(self()), static_cast<DispatcherThread&>(self()));

        mHasStarted = true;
        mEventStarted.setEvent();
    }
    else
    {
        WatchdogManagerEvent::removeListener(static_cast<IEWatchdogManagerEventConsumer&>(self()), static_cast<DispatcherThread&>(self()));

        _removeAllWatchdogs();
        mHasStarted = false;
        mEventStarted.resetEvent();
    }
}

bool WatchdogManager::_startWatchdogManagerThread(void)
{
    bool result = false;
    if (isReady() == false)
    {
        ASSERT(isRunning() == false);
        result = createThread(NECommon::WAIT_INFINITE) && waitForDispatcherStart(NECommon::WAIT_INFINITE);
#ifdef _DEBUG
        if (result == false)
        {
            OUTPUT_ERR("Failed to create [ %s ] System Timer Thread", WatchdogManager::TIMER_THREAD_NAME.data());
        }
#endif  // _DEBUG
    }
    else
    {
        result = true;
    }

    return result;
}

void WatchdogManager::_stopWatchdogManagerThread(void)
{
    destroyThread(NECommon::WAIT_INFINITE);
}
