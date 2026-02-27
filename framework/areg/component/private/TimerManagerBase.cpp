/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/TimerManagerBase.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, The System Timer Manager Base class.
 *
 ************************************************************************/
#include "areg/component/private/TimerManagerBase.hpp"
#include "areg/component/private/ExitEvent.hpp"

namespace areg
{

    //////////////////////////////////////////////////////////////////////////
    // Implement Runtime
    //////////////////////////////////////////////////////////////////////////
    AREG_IMPLEMENT_RUNTIME(TimerManagerBase, DispatcherThread)

    TimerManagerBase::TimerManagerBase(const String& threadName)
        : DispatcherThread              (threadName, STACK_SIZE_DEFAULT, QUEUE_SIZE_MAXIMUM)
        , TimerManagerEventConsumer   ( )
    {
    }

    bool TimerManagerBase::postEvent(Event& eventElem)
    {
        return (AREG_RUNTIME_CAST(&eventElem, TimerManagerEvent) != nullptr) && EventDispatcher::postEvent(eventElem);
    }

    bool TimerManagerBase::runDispatcher()
    {
        readyForEvents( true );

        SyncObject* syncObjects[] = { &mEventExit, &mEventQueue };
        MultiLock multiLock(syncObjects, 2, false);
        int32_t whichEvent = static_cast<int32_t>(EventDispatcherBase::EventSignal::Error);
        const ExitEvent& exitEvent = ExitEvent::getExitEvent();

        do
        {
            whichEvent = multiLock.lock(WAIT_INFINITE, false, true);
            Event* eventElem = whichEvent == static_cast<int32_t>(EventDispatcherBase::EventSignal::Queue) ? pickEvent() : nullptr;
            if (static_cast<const Event*>(eventElem) != static_cast<const Event*>(&exitEvent))
            {
                if (whichEvent == static_cast<int32_t>(EventDispatcherBase::EventSignal::Queue))
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
                whichEvent = static_cast<int32_t>(EventDispatcherBase::EventSignal::Exit);
            }

        } while (whichEvent == static_cast<int>(EventDispatcherBase::EventSignal::Queue) || (whichEvent == MultiLock::LOCK_INDEX_COMPLETION));

        readyForEvents(false);
        removeAllEvents();

        ASSERT(static_cast<EventQueue&>(mInternalEvents).isEmpty());

        return (whichEvent == static_cast<int32_t>(EventDispatcherBase::EventSignal::Exit));
    }

    void TimerManagerBase::readyForEvents(bool isReady)
    {
        if (isReady)
        {
            TimerManagerEvent::addListener(static_cast<TimerManagerEventConsumer&>(self()), static_cast<DispatcherThread&>(self()));
        }
        else
        {
            TimerManagerEvent::removeListener(static_cast<TimerManagerEventConsumer&>(self()), static_cast<DispatcherThread&>(self()));
        }

        DispatcherThread::readyForEvents( true );
    }

    bool TimerManagerBase::startTimerManagerThread()
    {
        ASSERT(isReady() || (isRunning() == false));
        return (isReady() || (createThread(WAIT_INFINITE) && waitForDispatcherStart(WAIT_INFINITE)));
    }

    void TimerManagerBase::stopTimerManagerThread(bool waitComplete)
    {
        if (waitComplete)
        {
            shutdownThread(WAIT_INFINITE);
        }
        else
        {
            triggerExit();
        }
    }

    void TimerManagerBase::waitCompletion()
    {
        shutdownThread(WAIT_INFINITE);
    }
} // namespace areg
