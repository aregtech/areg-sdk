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
    AREG_IMPLEMENT_RUNTIME(TimerManagerBase, areg::DispatcherThread)

    TimerManagerBase::TimerManagerBase(const areg::String& threadName)
        : areg::DispatcherThread              (threadName, areg::STACK_SIZE_DEFAULT, areg::QUEUE_SIZE_MAXIMUM)
        , areg::TimerManagerEventConsumer   ( )
    {
    }

    bool TimerManagerBase::postEvent(areg::Event& eventElem)
    {
        return (AREG_RUNTIME_CAST(&eventElem, areg::TimerManagerEvent) != nullptr) && areg::EventDispatcher::postEvent(eventElem);
    }

    bool TimerManagerBase::runDispatcher()
    {
        readyForEvents( true );

        areg::SyncObject* syncObjects[] = { &mEventExit, &mEventQueue };
        areg::MultiLock multiLock(syncObjects, 2, false);
        int32_t whichEvent = static_cast<int32_t>(EventDispatcherBase::EventSignal::Error);
        const areg::ExitEvent& exitEvent = areg::ExitEvent::getExitEvent();

        do
        {
            whichEvent = multiLock.lock(areg::WAIT_INFINITE, false, true);
            areg::Event* eventElem = whichEvent == static_cast<int32_t>(EventDispatcherBase::EventSignal::Queue) ? pickEvent() : nullptr;
            if (static_cast<const areg::Event*>(eventElem) != static_cast<const areg::Event*>(&exitEvent))
            {
                if (whichEvent == static_cast<int32_t>(EventDispatcherBase::EventSignal::Queue))
                {
                    // proceed one external event.
                    if (prepareDispatchEvent(eventElem))
                    {
                        dispatchEvent(*eventElem);
                    }

                    postDispatchEvent(eventElem);

                    ASSERT(static_cast<areg::EventQueue&>(mInternalEvents).isEmpty());
                }
            }
            else
            {
                whichEvent = static_cast<int32_t>(EventDispatcherBase::EventSignal::Exit);
            }

        } while (whichEvent == static_cast<int>(EventDispatcherBase::EventSignal::Queue) || (whichEvent == areg::MultiLock::LOCK_INDEX_COMPLETION));

        readyForEvents(false);
        removeAllEvents();

        ASSERT(static_cast<areg::EventQueue&>(mInternalEvents).isEmpty());

        return (whichEvent == static_cast<int32_t>(EventDispatcherBase::EventSignal::Exit));
    }

    void TimerManagerBase::readyForEvents(bool isReady)
    {
        if (isReady)
        {
            areg::TimerManagerEvent::addListener(static_cast<areg::TimerManagerEventConsumer&>(self()), static_cast<areg::DispatcherThread&>(self()));
        }
        else
        {
            areg::TimerManagerEvent::removeListener(static_cast<areg::TimerManagerEventConsumer&>(self()), static_cast<areg::DispatcherThread&>(self()));
        }

        areg::DispatcherThread::readyForEvents( true );
    }

    bool TimerManagerBase::startTimerManagerThread()
    {
        ASSERT(isReady() || (isRunning() == false));
        return (isReady() || (createThread(areg::WAIT_INFINITE) && waitForDispatcherStart(areg::WAIT_INFINITE)));
    }

    void TimerManagerBase::stopTimerManagerThread(bool waitComplete)
    {
        if (waitComplete)
        {
            shutdownThread(areg::WAIT_INFINITE);
        }
        else
        {
            triggerExit();
        }
    }

    void TimerManagerBase::waitCompletion()
    {
        shutdownThread(areg::WAIT_INFINITE);
    }
} // namespace areg
