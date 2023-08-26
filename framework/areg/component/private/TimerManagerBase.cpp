/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/component/private/TimerManagerBase.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, The System Timer Manager Base class.
 *
 ************************************************************************/
#include "areg/component/private/TimerManagerBase.hpp"
#include "areg/component/private/ExitEvent.hpp"

//////////////////////////////////////////////////////////////////////////
// Implement Runtime
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME(TimerManagerBase, DispatcherThread)

TimerManagerBase::TimerManagerBase(const String& threadName)
    : DispatcherThread              (threadName)
    , IETimerManagerEventConsumer   ( )
{
}

bool TimerManagerBase::postEvent(Event& eventElem)
{
    bool result = false;
    if (RUNTIME_CAST(&eventElem, TimerManagerEvent) != nullptr)
    {
        result = EventDispatcher::postEvent(eventElem);
    }
    else
    {
        OUTPUT_ERR("Not a TimerManagerEvent type event, cannot Post. Destroying event type [ %s ]", eventElem.getRuntimeClassName().getString());
        eventElem.destroy();
    }

    return result;
}

bool TimerManagerBase::runDispatcher(void)
{
    readyForEvents( true );

    IESynchObject* synchObjects[] = { &mEventExit, &mEventQueue };
    MultiLock multiLock(synchObjects, 2, false);
    int whichEvent = static_cast<int>(EventDispatcherBase::eEventOrder::EventError);
    const ExitEvent& exitEvent = ExitEvent::getExitEvent();

    do
    {
        whichEvent = multiLock.lock(NECommon::WAIT_INFINITE, false, true);
        Event* eventElem = whichEvent == static_cast<int>(EventDispatcherBase::eEventOrder::EventQueue) ? pickEvent() : nullptr;
        if (static_cast<const Event*>(eventElem) != static_cast<const Event*>(&exitEvent))
        {
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

void TimerManagerBase::readyForEvents(bool isReady)
{
    if (isReady)
    {
        TimerManagerEvent::addListener(static_cast<IETimerManagerEventConsumer&>(self()), static_cast<DispatcherThread&>(self()));
    }
    else
    {
        TimerManagerEvent::removeListener(static_cast<IETimerManagerEventConsumer&>(self()), static_cast<DispatcherThread&>(self()));
    }

    DispatcherThread::readyForEvents( true );
}

bool TimerManagerBase::startTimerManagerThread(void)
{
    bool result = false;
    if (isReady() == false)
    {
        ASSERT(isRunning() == false);
        result = createThread(NECommon::WAIT_INFINITE) && waitForDispatcherStart(NECommon::WAIT_INFINITE);
#ifdef _DEBUG
        if (result == false)
        {
            OUTPUT_ERR("Failed to create [ %s ] System Timer Thread", this->getName().getString());
        }
#endif  // _DEBUG
    }
    else
    {
        result = true;
    }

    return result;
}

void TimerManagerBase::stopTimerManagerThread(void)
{
    destroyThread(NECommon::WAIT_INFINITE);
}
