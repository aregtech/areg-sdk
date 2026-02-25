/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/WorkerThread.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Worker Thread implementation.
 *
 ************************************************************************/
#include "areg/component/WorkerThread.hpp"

#include "areg/component/WorkerThreadConsumer.hpp"
#include "areg/component/Event.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/Component.hpp"
#include "areg/base/UtilityDefs.hpp"

//////////////////////////////////////////////////////////////////////////
// WorkerThread class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Implement Runtime
//////////////////////////////////////////////////////////////////////////
AREG_IMPLEMENT_RUNTIME(WorkerThread, DispatcherThread)

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
WorkerThread::WorkerThread( const areg::String & threadName
                          , Component & bindingComponent
                          , WorkerThreadConsumer & threadConsumer
                          , uint32_t watchdogTimeout/* = areg::WATCHDOG_IGNORE    */
                          , uint32_t stackSizeKb    /* = areg::STACK_SIZE_DEFAULT */
                          , uint32_t maxQueue       /* = areg::IGNORE_VALUE */ )
    : DispatcherThread      ( threadName, stackSizeKb, maxQueue )

    , mBindingComponent     ( bindingComponent )
    , mWorkerThreadConsumer ( threadConsumer )
    , mWatchdog             ( self(), watchdogTimeout )
{
    ASSERT(areg::isEmpty<char>(threadName) == false);
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////
bool WorkerThread::postEvent( Event& eventElem )
{
    return Event::isCustom(eventElem.getEventType()) && EventDispatcher::postEvent(eventElem);
}

void WorkerThread::readyForEvents( bool isReady )
{
    if ( isReady )
    {
        mWorkerThreadConsumer.registerEventConsumers( self( ), mBindingComponent.getMasterThread( ) );
    }
    else
    {
        mWorkerThreadConsumer.unregisterEventConsumers( self( ) );
    }

    DispatcherThread::readyForEvents(isReady);
}

DispatcherThread* WorkerThread::getEventConsumerThread( const RuntimeClassID& whichClass )
{
    return (hasRegisteredConsumer(whichClass) ? static_cast<DispatcherThread *>(this) : getBindingComponent().findEventConsumer(whichClass));
}

bool WorkerThread::dispatchEvent(Event& eventElem)
{
    mWatchdog.startGuard();
    bool result = DispatcherThread::dispatchEvent(eventElem);
    mWatchdog.stopGuard();

    return result;
}

ComponentThread & WorkerThread::getBindingComponentThread() const
{
    return mBindingComponent.getMasterThread();
}

void WorkerThread::terminateSelf()
{
    mHasStarted = false;
    removeAllEvents();
    mEventExit.setEvent();
    areg::Thread::shutdownThread(areg::TIMEOUT_10_MS);

    delete this;
}
