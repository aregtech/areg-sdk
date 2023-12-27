/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/component/private/WorkerThread.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Worker Thread implementation.
 *
 ************************************************************************/
#include "areg/component/WorkerThread.hpp"

#include "areg/component/IEWorkerThreadConsumer.hpp"
#include "areg/component/Event.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/Component.hpp"
#include "areg/base/NEUtilities.hpp"

//////////////////////////////////////////////////////////////////////////
// WorkerThread class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Implement Runtime
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME(WorkerThread, DispatcherThread)

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
WorkerThread::WorkerThread( const String & threadName
                          , Component & bindingComponent
                          , IEWorkerThreadConsumer & threadConsumer
                          , uint32_t watchdogTimeout /*= NECommon::WATCHDOG_IGNORE*/)
    : DispatcherThread      ( threadName )

    , mBindingComponent     ( bindingComponent )
    , mWorkerThreadConsumer ( threadConsumer )
    , mWatchdog             ( self(), watchdogTimeout )
{
    ASSERT(NEString::isEmpty<char>(threadName) == false);
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////
bool WorkerThread::postEvent( Event& eventElem )
{
    bool result = false;
    if ( Event::isCustom(eventElem.getEventType()) )
    {
        EventDispatcher::postEvent(eventElem);
    }
    else
    {
        OUTPUT_ERR("Wrong event to post, event type [ %s ], category [ %d ]", eventElem.getRuntimeClassName().getString(), static_cast<int>(eventElem.getEventType()));
        eventElem.destroy();
        ASSERT(false);
    }

    return result;
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

ComponentThread & WorkerThread::getBindingComponentThread( void ) const
{
    return mBindingComponent.getMasterThread();
}

void WorkerThread::terminateSelf(void)
{
    mHasStarted = false;
    removeAllEvents();
    mEventExit.setEvent();
    Thread::shutdownThread(NECommon::TIMEOUT_10_MS);

    delete this;
}
