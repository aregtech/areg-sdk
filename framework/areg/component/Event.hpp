#ifndef AREG_COMPONENT_EVENT_HPP
#define AREG_COMPONENT_EVENT_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/Event.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Event Base class declaration.
 *              Event IS an EventEnvelope: the binary header and payload live in
 *              the same allocation. Local custom events carry a DataClass via
 *              placement-new in the payload tail. IPC events carry serialized bytes.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/EventEnvelope.hpp"
#include "areg/base/RuntimeClassID.hpp"
#include "areg/component/EventDefs.hpp"
#include "areg/base/UtilityDefs.hpp"

/************************************************************************
 * \brief   Predefined MACRO to use for event declaration and implementation
 ************************************************************************/

/**
 * \brief   MACRO, declares static functions to add and remove
 *          event consumer, which should be available in every Event class.
 *          Do not use them directly, instead use AREG_DECLARE_RUNTIME_EVENT
 **/
#define AREG_DECLARE_EVENT_REGISTRATION(EventClass)                                                                         \
public:                                                                                                                     \
    /*  Declare static function to add/register event consumer to start processing event.       */                          \
    static bool add_listener(areg::EventConsumer& eventConsumer, const UniqueNumber whichThread = areg::IGNORE_VALUE);      \
    /*  Declare static function to add/register event consumer to start processing event.       */                          \
    static bool add_listener(areg::EventConsumer& eventConsumer, const id_type whichThread);                                \
    /*  Declare static function to add/register event consumer to start processing event.       */                          \
    static bool add_listener(areg::EventConsumer& eventConsumer, areg::DispatcherThread & dispThread);                      \
    /*  Declare static function to remove/unregister event consumer to stop processing event.   */                          \
    static bool remove_listener(areg::EventConsumer& eventConsumer, const UniqueNumber whichThread = areg::IGNORE_VALUE);   \
    /*  Declare static function to remove/unregister event consumer to stop processing event.   */                          \
    static bool remove_listener(areg::EventConsumer& eventConsumer, const id_type whichThread);                             \
    /*  Declare static function to remove/unregister event consumer to stop processing event.   */                          \
    static bool remove_listener(areg::EventConsumer& eventConsumer, areg::DispatcherThread & dispThread);

/**
 * \brief   MACRO, implements static functions to add and remove
 *          event consumer, which should be available in every Event class.
 *          Do not use them directly, instead use AREG_IMPLEMENT_RUNTIME_EVENT
 **/
#define AREG_IMPLEMENT_EVENT_REGISTRATION(EventClass)                                                                       \
    bool EventClass::add_listener(areg::EventConsumer& eventConsumer, const UniqueNumber whichThread)                       \
    {   return areg::Event::add_listener(EventClass::CLASS_ID, eventConsumer, whichThread);       }                         \
    bool EventClass::add_listener(areg::EventConsumer& eventConsumer, const id_type whichThread)                            \
    {   return areg::Event::add_listener(EventClass::CLASS_ID, eventConsumer, whichThread);       }                         \
    bool EventClass::add_listener(areg::EventConsumer& eventConsumer, areg::DispatcherThread & dispThread)                  \
    {   return areg::Event::add_listener(EventClass::CLASS_ID, eventConsumer, dispThread);        }                         \
    bool EventClass::remove_listener(areg::EventConsumer& eventConsumer, const UniqueNumber whichThread)                    \
    {   return areg::Event::remove_listener(EventClass::CLASS_ID, eventConsumer, whichThread);    }                         \
    bool EventClass::remove_listener(areg::EventConsumer& eventConsumer, const id_type whichThread)                         \
    {   return areg::Event::remove_listener(EventClass::CLASS_ID, eventConsumer, whichThread);    }                         \
    bool EventClass::remove_listener(areg::EventConsumer& eventConsumer, areg::DispatcherThread & dispThread)               \
    {   return areg::Event::remove_listener(EventClass::CLASS_ID, eventConsumer, dispThread);     }


/**
 * \brief   MACRO, declares an Event class for runtime dispatch and consumer registration.
 *          CLASS_ID is a compile-time CRC32 of the class name (uint32_t), stored in
 *          EventHeader::classId by the constructor of the final class. No RuntimeObject
 *          inheritance is required.
 *
 * \param   EventClass  Event class name.
 **/
#define AREG_DECLARE_RUNTIME_EVENT(EventClass)                                                                              \
public:                                                                                                                     \
    static constexpr uint32_t CLASS_ID { AREG_RUNTIME_CLASS_ID(EventClass) };                                               \
public:                                                                                                                     \
    AREG_DECLARE_EVENT_REGISTRATION(EventClass)

/**
 * \brief   MACRO, implements static event-registration helper functions.
 *          EventBaseClass is accepted for backward compatibility but is unused;
 *          all registration is keyed by CLASS_ID (uint32_t).
 *
 * \param   EventClass          Event class name.
 * \param   EventBaseClass      Ignored; kept for source compatibility.
 **/
#define AREG_IMPLEMENT_RUNTIME_EVENT(EventClass, EventBaseClass)                                                            \
    AREG_IMPLEMENT_EVENT_REGISTRATION(EventClass)

/**
 * \brief   Template variant of AREG_IMPLEMENT_EVENT_REGISTRATION.
 *          Use this instead of AREG_IMPLEMENT_RUNTIME_TEMPLATE for Event-derived template classes.
 *
 * \param   Template    Full template parameter list, e.g. template<class DATA_CLASS>
 * \param   ClassName   Fully-qualified template class name, e.g. AregImpl_MyEvent<DATA_CLASS>
 **/
#define AREG_IMPLEMENT_EVENT_REGISTRATION_TEMPLATE(Template, ClassName)                                                     \
    Template inline bool ClassName::add_listener(areg::EventConsumer& eventConsumer, const UniqueNumber whichThread)        \
    {   return areg::Event::add_listener(ClassName::CLASS_ID, eventConsumer, whichThread);       }                          \
    Template inline bool ClassName::add_listener(areg::EventConsumer& eventConsumer, const id_type whichThread)             \
    {   return areg::Event::add_listener(ClassName::CLASS_ID, eventConsumer, whichThread);       }                          \
    Template inline bool ClassName::add_listener(areg::EventConsumer& eventConsumer, areg::DispatcherThread & dispThread)   \
    {   return areg::Event::add_listener(ClassName::CLASS_ID, eventConsumer, dispThread);        }                          \
    Template inline bool ClassName::remove_listener(areg::EventConsumer& eventConsumer, const UniqueNumber whichThread)     \
    {   return areg::Event::remove_listener(ClassName::CLASS_ID, eventConsumer, whichThread);    }                          \
    Template inline bool ClassName::remove_listener(areg::EventConsumer& eventConsumer, const id_type whichThread)          \
    {   return areg::Event::remove_listener(ClassName::CLASS_ID, eventConsumer, whichThread);    }                          \
    Template inline bool ClassName::remove_listener(areg::EventConsumer& eventConsumer, areg::DispatcherThread & dispThread)\
    {   return areg::Event::remove_listener(ClassName::CLASS_ID, eventConsumer, dispThread);     }


/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {
    class EventConsumer;
    class EventDispatcher;
    class DispatcherThread;
    class RuntimeClassID;
} // namespace areg

namespace areg {

//////////////////////////////////////////////////////////////////////////
// Event class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Base class for all events. Event IS an EventEnvelope: a single heap allocation
 *          carries the 128-byte EventHeader followed by an optional payload.
 *
 *          Heap-allocated events are freed via `delete`; destroy() invalidates the buffer only.
 *          Internal events stored by value in EventStack are cleaned up automatically by the destructor.
 **/
class AREG_API Event   : protected EventEnvelope
{
//////////////////////////////////////////////////////////////////////////
// Defines and constants.
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Predefined invalid event object.
     **/
    static const Event  BAD_EVENT;

//////////////////////////////////////////////////////////////////////////
// Event class statics
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Registers an event consumer for a specific event class in the specified thread.
     **/
    static bool add_listener(const uint32_t classId, EventConsumer & eventConsumer, const UniqueNumber whichThread);

    /**
     * \brief   Registers an event consumer for a specific event class in the specified thread by ID.
     **/
    static bool add_listener( const uint32_t classId, EventConsumer & eventConsumer, const id_type whichThread );

    /**
     * \brief   Registers an event consumer for a specific event class in the specified dispatcher thread.
     **/
    static bool add_listener(const uint32_t classId, EventConsumer & eventConsumer, DispatcherThread & dispThread);

    /**
     * \brief   Unregisters an event consumer for a specific event class from the specified thread.
     **/
    static bool remove_listener(const uint32_t classId, EventConsumer & eventConsumer, const UniqueNumber whichThread);

    /**
     * \brief   Unregisters an event consumer for a specific event class from the specified thread by ID.
     **/
    static bool remove_listener( const uint32_t classId, EventConsumer & eventConsumer, id_type whichThread );

    /**
     * \brief   Unregisters an event consumer for a specific event class from the specified dispatcher thread.
     **/
    static bool remove_listener(const uint32_t classId, EventConsumer & eventConsumer, DispatcherThread & dispThread);

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:

    Event();

    /**
     * \brief   Creates an event of the specified type with no payload. Allocates the EventEnvelope.
     *
     * \param   eventType   The event type to store in the header.
     * \param   prio        The event priority to store in the header.
     **/
    explicit Event( areg::EventType eventType, areg::EventPriority prio = areg::DefaultPriority ) noexcept;

    /**
     * \brief   Creates an event of the specified type and reserves payload space for a DataClass.
     *          Use this constructor when storing a DataClass via placement-new at payload_ptr().
     *
     * \param   eventType   The event type to store in the header.
     * \param   reserve     Bytes to reserve after the 128-byte header for the DataClass.
     * \param   prio        The event priority to store in the header.
     **/
    explicit Event( areg::EventType eventType, uint32_t reserve, areg::EventPriority prio = areg::DefaultPriority ) noexcept;

    /**
     * \brief   Constructs from an existing envelope (IPC receive path). Shares the buffer.
     **/
    Event(const EventEnvelope& envelope) noexcept;

    /**
     * \brief   Constructs from a moved envelope (IPC receive path). Takes ownership.
     **/
    Event(EventEnvelope&& envelope) noexcept;

    Event(const Event& src) = default;

    Event(Event&& src) noexcept = default;

    virtual ~Event();

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    Event& operator = (const Event& src) noexcept = default;

    Event& operator = (Event&& src) noexcept = default;

    [[nodiscard]]
    bool operator == (const Event& other) const noexcept;

    [[nodiscard]]
    bool operator != (const Event& other) const noexcept;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Dispatches this event to the specified consumer.
     **/
    void dispatch_self( EventConsumer * consumer );

    /**
     * \brief   Delivers this event to its target thread.
     **/
    void deliver_event();

    /**
     * \brief   Registers a consumer for this event in the target thread.
     **/
    bool add_event_listener( EventConsumer & eventConsumer );

    /**
     * \brief   Unregisters a consumer from this event in the target thread.
     **/
    bool remove_event_listener( EventConsumer & eventConsumer );

    /**
     * \brief   Registers this event with the specified thread by ID.
     **/
    bool register_for_thread_id( id_type whichThread = 0);

    /**
     * \brief   Registers this event with the thread of the specified name.
     **/
    bool register_for_thread_number( uint32_t whichThread );

    /**
     * \brief   Sets the target dispatcher thread for this event.
     **/
    bool register_for_thread( DispatcherThread * dispatchThread );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns true if the target thread has a consumer registered for this event.
     **/
    [[nodiscard]]
    bool is_event_registered() const noexcept;

    [[nodiscard]]
    inline bool is_exit_prio() const noexcept;

    [[nodiscard]]
    inline bool is_critical_prio() const noexcept;

    [[nodiscard]]
    inline bool is_high_prio() const noexcept;

    [[nodiscard]]
    inline bool is_normal_prio() const noexcept;

    [[nodiscard]]
    inline bool is_event_id(const RuntimeClassID& classId) const noexcept;

    [[nodiscard]]
    inline bool is_event_id(const uint32_t eventId) const noexcept;

    [[nodiscard]]
    inline bool is_request_call() const noexcept;

    [[nodiscard]]
    inline bool is_response_call() const noexcept;

    [[nodiscard]]
    inline bool is_broadcast_call() const noexcept;

    [[nodiscard]]
    inline bool is_attribute_call() const noexcept;

    [[nodiscard]]
    inline bool is_request_failed_call() const noexcept;

    [[nodiscard]]
    inline bool is_notify_call() const noexcept;

    [[nodiscard]]
    inline bool is_notify_response() const noexcept;

    [[nodiscard]]
    inline bool is_notify_broadcast() const noexcept;

    [[nodiscard]]
    inline bool is_notify_attribute() const noexcept;

    [[nodiscard]]
    inline bool is_connect_call() const noexcept;

    [[nodiscard]]
    inline MSG_ID message_id() const noexcept;

    /**
     * \brief   Returns the type of this event from the EventHeader.
     **/
    [[nodiscard]]
    inline areg::EventType event_type() const noexcept;

    /**
     * \brief   Sets the event type in the EventHeader.
     **/
    inline void set_event_type( areg::EventType eventType ) noexcept;

    /**
     * \brief   Returns the priority of this event from the EventHeader.
     **/
    [[nodiscard]]
    inline areg::EventPriority event_priority() const noexcept;

    /**
     * \brief   Sets the event priority in the EventHeader.
     **/
    inline void set_event_priority( areg::EventPriority eventPrio ) noexcept;

    /**
     * \brief   Returns the consumer registered to process this event, or null if none is set.
     *          Stored in EventHeader::internal2 (LOCAL-ONLY); zeroed on IPC wire.
     **/
    [[nodiscard]]
    inline EventConsumer * event_consumer() const noexcept;

    /**
     * \brief   Sets the consumer to process this event.
     *          Stored in EventHeader::internal2 (LOCAL-ONLY); zeroed on IPC wire.
     **/
    inline void set_event_consumer( EventConsumer * consumer ) noexcept;

    /**
     * \brief   Returns the target dispatcher thread, or null if none is set.
     *          Stored in EventHeader::internal1 (LOCAL-ONLY); zeroed on IPC wire.
     **/
    [[nodiscard]]
    inline DispatcherThread * target_dispatcher() const noexcept;

    /**
     * \brief   Sets the target dispatcher thread.
     *          Stored in EventHeader::internal1 (LOCAL-ONLY); zeroed on IPC wire.
     **/
    inline void set_target_dispatcher( DispatcherThread * thread ) noexcept;

    /**
     * \brief   Returns true if this event is internal (queued in internal queue).
     **/
    [[nodiscard]]
    inline bool is_internal() const noexcept;

    /**
     * \brief   Returns true if this event is external (queued in external queue).
     **/
    [[nodiscard]]
    inline bool is_external() const noexcept;

    /**
     * \brief   Returns true if this event is local (cannot be processed in other processes).
     **/
    [[nodiscard]]
    inline bool is_local() const noexcept;

    /**
     * \brief   Returns true if this event is remote (can be processed locally or in other processes).
     **/
    [[nodiscard]]
    inline bool is_remote() const noexcept;

    /**
     * \brief   Returns true if this is a developer custom event, false if system predefined.
     **/
    [[nodiscard]]
    inline bool is_custom() const noexcept;

    /**
     * \brief   Returns this event cast to InStream for reading serialized payload.
     **/
    [[nodiscard]]
    inline const InStream & read_stream( ) const noexcept;

    /**
     * \brief   Returns this event cast to OutStream for writing serialized payload.
     **/
    [[nodiscard]]
    inline OutStream & write_stream( ) const noexcept;

    [[nodiscard]]
    inline const EventEnvelope& envelope() const noexcept;

    /**
     * \brief   Returns the class ID stored in the EventHeader (set by final-class constructor).
     **/
    [[nodiscard]]
    inline uint32_t event_id() const noexcept;

    /**
     * \brief   Returns true if the Event has an allocated buffer (non-empty/valid state).
     *          Returns false for default-constructed Events (queue-empty sentinel) and for
     *          moved-from Events.
     **/
    [[nodiscard]]
    inline bool is_valid() const noexcept;

    /**
     * \brief   Returns a mutable pointer to the EventHeader at the start of the buffer,
     *          or nullptr if the buffer is not allocated.
     **/
    [[nodiscard]]
    inline areg::EventHeader* header() noexcept;

    /**
     * \brief   Returns a const pointer to the EventHeader at the start of the buffer,
     *          or nullptr if the buffer is not allocated.
     **/
    [[nodiscard]]
    inline const areg::EventHeader* header() const noexcept;

    /**
     * \brief   Invalidates the internal buffer without deleting the Event object.
     *          Used by specialized events that manage their own lifetime.
     **/
    inline void destroy_event() noexcept;

//////////////////////////////////////////////////////////////////////////
// Protected members
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Returns the dispatcher of the target thread, or current thread if target is unknown.
     **/
    [[nodiscard]]
    EventDispatcher & dispatcher() const noexcept;
};

//////////////////////////////////////////////////////////////////////////
// Event class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline bool Event::operator == (const Event& other) const noexcept
{
    return mByteBuffer.get() == other.mByteBuffer.get();
}

inline bool Event::operator != (const Event& other) const noexcept
{
    return mByteBuffer.get() != other.mByteBuffer.get();
}

inline areg::EventType Event::event_type() const noexcept
{
    return static_cast<areg::EventType>(EventEnvelope::event_type());
}

inline void Event::set_event_type( areg::EventType eventType ) noexcept
{
    EventEnvelope::set_event_type(static_cast<uint16_t>(eventType));
}

inline areg::EventPriority Event::event_priority() const noexcept
{
    return static_cast<areg::EventPriority>(EventEnvelope::priority());
}

inline void Event::set_event_priority( areg::EventPriority eventPrio ) noexcept
{
    EventEnvelope::set_priority(static_cast<uint8_t>(eventPrio));
}

inline EventConsumer * Event::event_consumer() const noexcept
{
    const areg::EventHeader* hdr{ EventEnvelope::header() };
    return (hdr != nullptr ? areg::to_ptr<EventConsumer*, uint64_t>(hdr->internal2) : nullptr);
}

inline void Event::set_event_consumer( EventConsumer * consumer ) noexcept
{
    areg::EventHeader* hdr{ EventEnvelope::header() };
    if (hdr != nullptr)
        hdr->internal2 = areg::to_num<uint64_t, EventConsumer*>(consumer);
}

inline DispatcherThread * Event::target_dispatcher() const noexcept
{
    const areg::EventHeader* hdr{ EventEnvelope::header() };
    return (hdr != nullptr ? areg::to_ptr<DispatcherThread*, uint64_t>(hdr->internal1) : nullptr);
}

inline void Event::set_target_dispatcher( DispatcherThread * thread ) noexcept
{
    areg::EventHeader* hdr{ EventEnvelope::header() };
    if (hdr != nullptr)
        hdr->internal1 = areg::to_num<uint64_t, DispatcherThread*>(thread);
}

inline bool Event::is_internal() const noexcept
{
    return areg::is_internal(event_type());
}

inline bool Event::is_external() const noexcept
{
    return areg::is_external(event_type());
}

inline bool Event::is_local() const noexcept
{
    return areg::is_local(event_type());
}

inline bool Event::is_remote() const noexcept
{
    return areg::is_remote(event_type());
}

inline bool Event::is_custom() const noexcept
{
    return areg::is_custom(event_type());
}

inline const InStream& Event::read_stream() const noexcept
{
    return static_cast<const InStream&>(*this);
}

inline OutStream& Event::write_stream() const noexcept
{
    return static_cast<OutStream&>(const_cast<Event&>(*this));
}

inline const EventEnvelope& Event::envelope() const noexcept
{
    static const EventEnvelope _envelope{};
    return (is_valid() ? static_cast<const EventEnvelope&>(*this) : _envelope);
}

inline uint32_t Event::event_id() const noexcept
{
    return EventEnvelope::event_id();
}

inline bool Event::is_valid() const noexcept
{
    return EventEnvelope::is_valid();
}

inline areg::EventHeader* Event::header() noexcept
{
    return EventEnvelope::header();
}

inline const areg::EventHeader* Event::header() const noexcept
{
    return EventEnvelope::header();
}

inline void Event::destroy_event() noexcept
{
    EventEnvelope::destroy_event();
}

inline bool Event::is_exit_prio() const noexcept
{
    const areg::RawEnvelope* env{ raw_envelope() };
    return ((env != nullptr) && (env->envHeader.priority == static_cast<uint8_t>(areg::EventPriority::ExitPrio)));
}

inline bool Event::is_critical_prio() const noexcept
{
    const areg::RawEnvelope* env{ raw_envelope() };
    return ((env != nullptr) && (env->envHeader.priority == static_cast<uint8_t>(areg::EventPriority::CriticalPrio)));
}

inline bool Event::is_high_prio() const noexcept
{
    const areg::RawEnvelope* env{ raw_envelope() };
    return ((env != nullptr) && (env->envHeader.priority == static_cast<uint8_t>(areg::EventPriority::HighPrio)));
}

inline bool Event::is_normal_prio() const noexcept
{
    const areg::RawEnvelope* env{ raw_envelope() };
    return ((env != nullptr) && (env->envHeader.priority == static_cast<uint8_t>(areg::EventPriority::NormalPrio)));
}

inline bool Event::is_event_id(const RuntimeClassID& classId) const noexcept
{
    return is_event_id(static_cast<uint32_t>(classId));
}

inline bool Event::is_event_id(const uint32_t eventId) const noexcept
{
    const areg::RawEnvelope* env{ raw_envelope() };
    return ((env != nullptr) && (env->envHeader.eventId == eventId));
}

inline bool Event::is_request_call() const noexcept
{
    const areg::RawEnvelope* env{ raw_envelope() };
    return ((env != nullptr) && areg::is_request_call(static_cast<areg::EventType>(env->envHeader.eventType)));
}

inline bool Event::is_response_call() const noexcept
{
    const areg::RawEnvelope* env{ raw_envelope() };
    return ((env != nullptr) && areg::is_response_call(static_cast<areg::EventType>(env->envHeader.eventType)));
}

inline bool Event::is_broadcast_call() const noexcept
{
    const areg::RawEnvelope* env{ raw_envelope() };
    return ((env != nullptr) && areg::is_broadcast_call(static_cast<areg::EventType>(env->envHeader.eventType)));
}

inline bool Event::is_attribute_call() const noexcept
{
    const areg::RawEnvelope* env{ raw_envelope() };
    return ((env != nullptr) && areg::is_attribute_call(static_cast<areg::EventType>(env->envHeader.eventType)));
}

inline bool Event::is_request_failed_call() const noexcept
{
    const areg::RawEnvelope* env{ raw_envelope() };
    return ((env != nullptr) && areg::is_request_failure(static_cast<areg::EventType>(env->envHeader.eventType)));
}

inline bool Event::is_notify_call() const noexcept
{
    const areg::RawEnvelope* env{ raw_envelope() };
    return ((env != nullptr) && areg::is_notify(static_cast<areg::EventType>(env->envHeader.eventType)));
}

inline bool Event::is_notify_response() const noexcept
{
    const areg::RawEnvelope* env{ raw_envelope() };
    return ((env != nullptr) && areg::is_notify_response(static_cast<areg::EventType>(env->envHeader.eventType)));
}

inline bool Event::is_notify_broadcast() const noexcept
{
    const areg::RawEnvelope* env{ raw_envelope() };
    return ((env != nullptr) && areg::is_notify_broadcast(static_cast<areg::EventType>(env->envHeader.eventType)));
}

inline bool Event::is_notify_attribute() const noexcept
{
    const areg::RawEnvelope* env{ raw_envelope() };
    return ((env != nullptr) && areg::is_notify_attribute(static_cast<areg::EventType>(env->envHeader.eventType)));
}

inline bool Event::is_connect_call() const noexcept
{
    const areg::RawEnvelope* env{ raw_envelope() };
    return ((env != nullptr) && areg::is_connect(static_cast<areg::EventType>(env->envHeader.eventType)));
}

inline MSG_ID Event::message_id() const noexcept
{
    const areg::RawEnvelope* env{ raw_envelope() };
    return (env != nullptr ? static_cast<MSG_ID>(env->envHeader.messageId) : areg::INVALID_MESSAGE_ID);
}

} // namespace areg

#endif  // AREG_COMPONENT_EVENT_HPP
