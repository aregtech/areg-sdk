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

#include "areg/base/RuntimeClassID.hpp"
#include "areg/component/DispatcherThread.hpp"
#include "areg/component/EventConsumer.hpp"

#include <cstring>

namespace areg {

//////////////////////////////////////////////////////////////////////////
// Event class, static members
//////////////////////////////////////////////////////////////////////////

const Event Event::BAD_EVENT(areg::EventType::EventUnknown);

bool Event::add_listener(const uint32_t classId, EventConsumer& eventConsumer, const UniqueNumber whichThread)
{
    return Event::add_listener(classId, eventConsumer, DispatcherThread::dispatcher_thread(whichThread));
}

bool Event::add_listener(const uint32_t classId, EventConsumer& eventConsumer, const id_type whichThread)
{
    return Event::add_listener(classId, eventConsumer, DispatcherThread::dispatcher_thread(whichThread));
}

bool Event::add_listener(const uint32_t classId, EventConsumer& eventConsumer, DispatcherThread& dispThread)
{
    return (dispThread.is_running() ? dispThread.register_event_consumer(classId, eventConsumer) : false);
}

bool Event::remove_listener(const uint32_t classId, EventConsumer& eventConsumer, const UniqueNumber whichThread)
{
    return Event::remove_listener(classId, eventConsumer, DispatcherThread::dispatcher_thread(whichThread));
}

bool Event::remove_listener(const uint32_t classId, EventConsumer& eventConsumer, id_type whichThread)
{
    return Event::remove_listener(classId, eventConsumer, DispatcherThread::dispatcher_thread(whichThread));
}

bool Event::remove_listener(const uint32_t classId, EventConsumer& eventConsumer, DispatcherThread& dispThread)
{
    return dispThread.is_running() ? dispThread.unregister_event_consumer(classId, eventConsumer) : false;
}

//////////////////////////////////////////////////////////////////////////
// Event class, Constructors
//////////////////////////////////////////////////////////////////////////

Event::~Event()
{
    if (!is_unique())
        return;

    areg::EventHeader* hdr{ MessageEnvelope::header() };
    if ((hdr != nullptr) && (hdr->custom != 0u))
    {
        using CleanupFn = void(*)(void*) noexcept;
        CleanupFn fn{ nullptr };
        static_assert(sizeof(fn) <= sizeof(uint64_t), "cleanup fn ptr does not fit in uint64_t");
        std::memcpy(&fn, &hdr->custom, sizeof(fn));
        hdr->custom = 0u;   // clear before call — prevent re-entrant double-call
        fn(payload_ptr());
    }
}

//////////////////////////////////////////////////////////////////////////
// Event class, methods
//////////////////////////////////////////////////////////////////////////

EventDispatcher& Event::dispatcher() const noexcept
{
    DispatcherThread* dt{ target_dispatcher() };
    return (dt != nullptr ? dt->event_dispatcher() : DispatcherThread::current_dispatcher());
}

void Event::deliver_event()
{
    DispatcherThread* dt{ target_dispatcher() };
    if (dt == nullptr)
    {
        DispatcherThread& thread = DispatcherThread::dispatcher_thread(static_cast<UniqueNumber>(consumer_thread()));
        if (thread.is_running())
        {
            register_for_thread(&thread);
            dt = &thread;
        }
    }

    if ((dt == nullptr) || !dt->event_dispatcher().post_event(*this))
    {
        destroy_event();
    }
}

bool Event::register_for_thread_id( id_type whichThread /*= 0*/ )
{
    return register_for_thread(&DispatcherThread::dispatcher_thread(whichThread));
}

bool Event::register_for_thread_number( uint32_t whichThread )
{
    return register_for_thread(&DispatcherThread::dispatcher_thread(whichThread));
}

bool Event::register_for_thread( DispatcherThread * dispatchThread )
{
    if ((dispatchThread != nullptr) && dispatchThread->is_valid())
    {
        set_target_dispatcher(dispatchThread->is_ready() ? dispatchThread : nullptr);
    }

    return (target_dispatcher() != nullptr);
}

bool Event::is_event_registered() const noexcept
{
    return dispatcher().has_registered_consumer(event_id());
}

bool Event::add_event_listener( EventConsumer& eventConsumer )
{
    return dispatcher().register_event_consumer(event_id(), eventConsumer);
}

bool Event::remove_event_listener( EventConsumer& eventConsumer )
{
    return dispatcher().unregister_event_consumer(event_id(), eventConsumer);
}

void Event::dispatch_self( EventConsumer* consumer )
{
    consumer = consumer != nullptr ? consumer : event_consumer();
    if (consumer != nullptr)
    {
        if (consumer->preprocess_event(*this))
            consumer->start_event_processing(*this);
    }
}

} // namespace areg
