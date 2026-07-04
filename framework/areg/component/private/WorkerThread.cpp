/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
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
namespace areg {

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
WorkerThread::WorkerThread( const String & threadName
                          , Component & bindingComponent
                          , WorkerThreadConsumer & threadConsumer
                          , uint32_t watchdogTimeout/* = areg::WATCHDOG_IGNORE    */
                          , uint32_t stackSizeKb    /* = areg::DEFAULT_STACK_SIZE */
                          , uint32_t maxQueue       /* = areg::IGNORE_VALUE */
                          , areg::Bool dropOnFull    /* = areg::Bool::Undefined */
                          , uint32_t waitMs          /* = areg::WAIT_INFINITE */ )
    : DispatcherThread      ( threadName, stackSizeKb, maxQueue, dropOnFull, waitMs )

    , mBindingComponent     ( bindingComponent )
    , mWorkerThreadConsumer ( threadConsumer )
    , mWatchdog             ( self(), watchdogTimeout )
{
    ASSERT(!threadName.is_empty());
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////
bool WorkerThread::post_event( Event& eventElem )
{
    return areg::is_custom(eventElem.event_type()) && EventDispatcher::post_event(eventElem);
}

void WorkerThread::ready_for_events( bool is_ready )
{
    if ( is_ready )
    {
        mWorkerThreadConsumer.register_event_consumers( self( ), mBindingComponent.master_thread( ) );
    }
    else
    {
        mWorkerThreadConsumer.unregister_event_consumers( self( ) );
    }

    DispatcherThread::ready_for_events(is_ready);
}

DispatcherThread* WorkerThread::event_consumer_thread( const uint32_t whichClass ) noexcept
{
    return (has_registered_consumer(whichClass) ? static_cast<DispatcherThread *>(this) : binding_component().find_event_consumer(whichClass));
}

bool WorkerThread::dispatch_event(Event& eventElem)
{
    mWatchdog.start_guard();
    bool result = DispatcherThread::dispatch_event(eventElem);
    mWatchdog.stop_guard();

    return result;
}

ComponentThread & WorkerThread::binding_component_thread() const
{
    return mBindingComponent.master_thread();
}

void WorkerThread::terminate_self()
{
    mHasStarted = false;
    remove_all_events();
    signal_exit_event();
    Thread::shutdown(areg::TIMEOUT_10_MS);

    delete this;
}

} // namespace areg
