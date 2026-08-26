/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/component/private/EventDispatcher.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Event Dispatcher class
 *
 ************************************************************************/
#include "areg/component/EventDispatcher.hpp"

#include "areg/component/DispatcherThread.hpp"
#include "areg/component/Event.hpp"
namespace areg {

//////////////////////////////////////////////////////////////////////////
// EventDispatcher class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// EventDispatcher class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
EventDispatcher::EventDispatcher( const String & name, uint32_t maxQeueue, areg::Bool dropOnFull, uint32_t waitMs )
    : EventDispatcherBase( name, maxQeueue, dropOnFull, waitMs )
    , ThreadConsumer     (  )
    , EventRouter        (  )
{
}

EventDispatcher::EventDispatcher( areg::NullTag ) noexcept
    : EventDispatcherBase( areg::NullTag{} )
    , ThreadConsumer     (  )
    , EventRouter        (  )
{
}

EventDispatcher::~EventDispatcher()
{
}

//////////////////////////////////////////////////////////////////////////
// EventDispatcher class, methods
//////////////////////////////////////////////////////////////////////////
bool EventDispatcher::on_thread_registered( Thread * threadObj )
{
    ASSERT(AREG_RUNTIME_CAST(threadObj, DispatcherThread) == static_cast<DispatcherThread *>(this));
    static_cast<void>(threadObj);

    EventDispatcherBase::remove_all_events( );
    EventDispatcherBase::mExternalEvents.reset_exit();
    return true;
}

void EventDispatcher::on_thread_unregistering()
{
    stop_dispatcher();
}

void EventDispatcher::on_run()
{
    start_dispatcher();
}

int32_t EventDispatcher::on_exit()
{
    exit_dispatcher( );
    return static_cast<int32_t>(ThreadConsumer::ExitCode::Normal);
}

DispatcherThread & EventDispatcher::dispatcher_thread() const noexcept
{
    return static_cast<DispatcherThread &>(const_cast<EventDispatcher &>(*this));
}

bool EventDispatcher::post_event( Event& eventElem )
{
    return queue_event(eventElem);
}

} // namespace areg
