/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/Event.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Event Base class implementation
 *
 ************************************************************************/
#include "areg/component/Event.hpp"

#include "areg/component/DispatcherThread.hpp"
#include "areg/component/EventConsumer.hpp"
namespace areg {

//////////////////////////////////////////////////////////////////////////
// Event class declaration
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Event class, Runtime implementation
//////////////////////////////////////////////////////////////////////////
AREG_IMPLEMENT_RUNTIME_EVENT(Event, RuntimeObject)

//////////////////////////////////////////////////////////////////////////
// Event class, static members
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Predefined Bad Event object
 **/
const Event Event::BAD_EVENT(Event::EventType::EventUnknown);

bool Event::add_listener( const RuntimeClassID & classId, EventConsumer & eventConsumer, const String & whichThread )
{
    return Event::add_listener(classId, eventConsumer, DispatcherThread::dispatcher_thread(whichThread));
}

bool Event::add_listener( const RuntimeClassID & classId, EventConsumer & eventConsumer, id_type whichThread )
{
    return Event::add_listener( classId, eventConsumer, DispatcherThread::dispatcher_thread( whichThread ) );
}

bool Event::add_listener( const RuntimeClassID & classId, EventConsumer & eventConsumer, DispatcherThread & dispThread )
{
    return ( dispThread.is_running() ? dispThread.register_event_consumer(classId, eventConsumer) : false );
}

bool Event::remove_listener( const RuntimeClassID & classId, EventConsumer & eventConsumer, const String & whichThread )
{
    return Event::remove_listener(classId, eventConsumer, DispatcherThread::dispatcher_thread(whichThread));
}

bool Event::remove_listener( const RuntimeClassID & classId, EventConsumer & eventConsumer, id_type whichThread )
{
    return Event::remove_listener( classId, eventConsumer, DispatcherThread::dispatcher_thread( whichThread ) );
}

bool Event::remove_listener( const RuntimeClassID & classId, EventConsumer & eventConsumer, DispatcherThread & dispThread )
{
    return dispThread.is_running() ? dispThread.unregister_event_consumer(classId, eventConsumer) : false;
}

//////////////////////////////////////////////////////////////////////////
// Event class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

Event::Event()
    : RuntimeObject ( )
    , mEventType    ( Event::EventType::EventUnknown )
    , mEventPrio    ( DefaultPriority )
    , mConsumer     ( nullptr )
    , mTargetThread ( nullptr )
{
}

Event::Event( Event::EventType eventType )
    : RuntimeObject ( )
    , mEventType    ( eventType )
    , mEventPrio    ( DefaultPriority )
    , mConsumer     ( nullptr )
    , mTargetThread ( nullptr )
{
}

Event::~Event()
{
    mConsumer       = nullptr;
    mTargetThread   = nullptr;
}

inline Event & Event::self()
{
    return (*this);
}

//////////////////////////////////////////////////////////////////////////
// Event class, methods
//////////////////////////////////////////////////////////////////////////

void Event::destroy()
{
    delete this;
}

EventDispatcher& Event::dispatcher() const
{
    return (mTargetThread != nullptr ? mTargetThread->event_dispatcher() : DispatcherThread::current_dispatcher());
}

void Event::deliver_event()
{
    EventDispatcher * dispatcher = mTargetThread != nullptr ? &mTargetThread->event_dispatcher( ) : nullptr;
    if ((dispatcher == nullptr) || (dispatcher->post_event(*this) == false))
    {
        destroy();
    }
}

bool Event::register_for_thread( id_type whichThread /*= 0*/ )
{
    return register_for_thread(whichThread != 0 ? AREG_RUNTIME_CAST(Thread::find_by_id(whichThread), DispatcherThread)
                                              : AREG_RUNTIME_CAST(Thread::current_thread(), DispatcherThread));
}

bool Event::register_for_thread( const char* whichThread )
{
    return register_for_thread(whichThread != nullptr ? AREG_RUNTIME_CAST(Thread::find_by_name(whichThread), DispatcherThread) : nullptr);
}

bool Event::register_for_thread( DispatcherThread * dispatchThread )
{
    if ((dispatchThread != nullptr) && dispatchThread->is_valid())
    {
        mTargetThread = dispatchThread->is_ready() ? dispatchThread : nullptr;
    }

    return (mTargetThread != nullptr);
}

bool Event::is_event_registered() const
{
    return dispatcher().has_registered_consumer(class_id());
}

bool Event::add_event_listener( EventConsumer& eventConsumer )
{
    return dispatcher().register_event_consumer(class_id(), eventConsumer);
}

bool Event::remove_event_listener( EventConsumer& eventConsumer )
{
    return dispatcher().unregister_event_consumer(class_id(), eventConsumer);
}

void Event::dispatch_self( EventConsumer* consumer )
{
    consumer = consumer != nullptr ? consumer : this->mConsumer;
    if ((consumer != nullptr) && consumer->preprocess_event(*this) )
    {
        consumer->start_event_processing(*this);
    }
}

} // namespace areg
