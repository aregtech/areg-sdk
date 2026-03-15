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
 *              This event class is generic and base class for all kind
 *              of events in system. This should be used for communication,
 *              but can be extended for custom events.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/IOStream.hpp"
#include "areg/base/RuntimeObject.hpp"

#include "areg/component/EventDefs.hpp"

/************************************************************************
 * \brief   Predefined MACRO to use for event declaration and implementation
 ************************************************************************/

/**
 * \brief   MACRO, declares static functions to add and remove
 *          event consumer, which should be available in every Event class.
 *          Do not use them directly, instead use AREG_DECLARE_RUNTIME_EVENT
 **/
#define AREG_DECLARE_EVENT_REGISTRATION(EventClass)                                                                                     \
public:                                                                                                                                 \
    /*  Declare static function to add/register event consumer to start processing event.       */                                      \
    static bool add_listener(areg::EventConsumer& eventConsumer, const areg::String & whichThread = areg::String::empty_string());      \
    /*  Declare static function to add/register event consumer to start processing event.       */                                      \
    static bool add_listener(areg::EventConsumer& eventConsumer, id_type whichThread);                                                  \
    /*  Declare static function to add/register event consumer to start processing event.       */                                      \
    static bool add_listener(areg::EventConsumer& eventConsumer, areg::DispatcherThread & dispThread);                                  \
    /*  Declare static function to remove/unregister event consumer to stop processing event.   */                                      \
    static bool remove_listener(areg::EventConsumer& eventConsumer, const areg::String & whichThread = areg::String::empty_string());   \
    /*  Declare static function to remove/unregister event consumer to stop processing event.   */                                      \
    static bool remove_listener(areg::EventConsumer& eventConsumer, id_type whichThread);                                               \
    /*  Declare static function to remove/unregister event consumer to stop processing event.   */                                      \
    static bool remove_listener(areg::EventConsumer& eventConsumer, areg::DispatcherThread & dispThread);

/**
 * \brief   MACRO, implements static functions to add and remove
 *          event consumer, which should be available in every Event class.
 *          Do not use them directly, instead use AREG_IMPLEMENT_RUNTIME_EVENT
 **/
#define AREG_IMPLEMENT_EVENT_REGISTRATION(EventClass)                                                                                   \
    bool EventClass::add_listener(areg::EventConsumer& eventConsumer, const areg::String & whichThread)                                 \
    {   return areg::Event::add_listener(EventClass::_class_id(), eventConsumer, whichThread);       }                                  \
    bool EventClass::add_listener(areg::EventConsumer& eventConsumer, id_type whichThread)                                              \
    {   return areg::Event::add_listener(EventClass::_class_id(), eventConsumer, whichThread);       }                                  \
    bool EventClass::add_listener(areg::EventConsumer& eventConsumer, areg::DispatcherThread & dispThread)                              \
    {   return areg::Event::add_listener(EventClass::_class_id(), eventConsumer, dispThread);        }                                  \
    bool EventClass::remove_listener(areg::EventConsumer& eventConsumer, const areg::String& whichThread)                               \
    {   return areg::Event::remove_listener(EventClass::_class_id(), eventConsumer, whichThread);    }                                  \
    bool EventClass::remove_listener(areg::EventConsumer& eventConsumer, id_type whichThread)                                           \
    {   return areg::Event::remove_listener(EventClass::_class_id(), eventConsumer, whichThread);    }                                  \
    bool EventClass::remove_listener(areg::EventConsumer& eventConsumer, areg::DispatcherThread & dispThread)                           \
    {   return areg::Event::remove_listener(EventClass::_class_id(), eventConsumer, dispThread);     }


/**
 * \brief   MACRO, to declare Event object as a runtime object
 *          to be able to check the type of event, find right consumer
 *          and trigger right event processing function call.
 *          Use this MACRO in every declared event class.
 *
 * \param   EventClass  Event class name.
 **/
#define AREG_DECLARE_RUNTIME_EVENT(EventClass)                                                                         \
    AREG_DECLARE_RUNTIME(EventClass)                                                                                    \
    AREG_DECLARE_EVENT_REGISTRATION(EventClass)

/**
 * \brief   MACRO, to implement appropriate runtime and event functions
 *          Use this MACRO in every event class source code.
 *
 * \param   EventClass          Event class name.
 * \param   EventBaseClass      The base (parent) class of Event
 *                              At least it should be Event.
 **/
#define AREG_IMPLEMENT_RUNTIME_EVENT(EventClass, EventBaseClass)                                                       \
    AREG_IMPLEMENT_RUNTIME(EventClass, EventBaseClass)                                                                  \
    AREG_IMPLEMENT_EVENT_REGISTRATION(EventClass)


/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {
    class EventConsumer;
    class EventDispatcher;
    class DispatcherThread;
    class Thread;
} // namespace areg

namespace areg {

//////////////////////////////////////////////////////////////////////////
// Event class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Base class for all events in the system. Events are forwarded to dispatcher threads for
 *          processing. Event consumers must be registered before sending events. Events are
 *          dynamically allocated and automatically destroyed after processing.
 **/
class AREG_API Event   : public RuntimeObject
{
//////////////////////////////////////////////////////////////////////////
// Defines and constants.
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Predefined invalid event object. It has Unknown type with ID 0.
     **/
    static const Event  BAD_EVENT;

//////////////////////////////////////////////////////////////////////////
// Event class
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Declare Event runtime information.
//////////////////////////////////////////////////////////////////////////
    AREG_DECLARE_RUNTIME_EVENT(Event)

//////////////////////////////////////////////////////////////////////////
// Event class statics
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Registers an event consumer for a specific event class in the specified thread.
     *
     * \param   classId             The runtime class ID of the event.
     * \param   eventConsumer       The consumer to register.
     * \param   whichThread         The thread name. If null or empty, uses the current thread.
     * \return  True if registration succeeded; false if failed or consumer already registered.
     **/
    static bool add_listener(const RuntimeClassID & classId, EventConsumer & eventConsumer, const String & whichThread);

    /**
     * \brief   Registers an event consumer for a specific event class in the specified thread by
     *          ID.
     *
     * \param   classId             The runtime class ID of the event.
     * \param   eventConsumer       The consumer to register.
     * \param   whichThread         The thread ID.
     * \return  True if registration succeeded; false if failed or consumer already registered.
     **/
    static bool add_listener( const RuntimeClassID & classId, EventConsumer & eventConsumer, id_type whichThread );

    /**
     * \brief   Registers an event consumer for a specific event class in the specified dispatcher
     *          thread.
     *
     * \param   classId             The runtime class ID of the event.
     * \param   eventConsumer       The consumer to register.
     * \param   dispThread          The dispatcher thread.
     * \return  True if registration succeeded; false if failed or consumer already registered.
     **/
    static bool add_listener(const RuntimeClassID & classId, EventConsumer & eventConsumer, DispatcherThread & dispThread);

    /**
     * \brief   Unregisters an event consumer for a specific event class from the specified thread.
     *
     * \param   classId             The runtime class ID of the event.
     * \param   eventConsumer       The consumer to unregister.
     * \param   whichThread         The thread name. If null or empty, uses the current thread.
     * \return  True if unregistration succeeded; false otherwise.
     **/
    static bool remove_listener(const RuntimeClassID & classId, EventConsumer & eventConsumer, const String & whichThread);

    /**
     * \brief   Unregisters an event consumer for a specific event class from the specified thread by ID.
     *
     * \param   classId             The runtime class ID of the event.
     * \param   eventConsumer       The consumer to unregister.
     * \param   whichThread         The thread ID.
     * \return  True if unregistration succeeded; false otherwise.
     **/
    static bool remove_listener( const RuntimeClassID & classId, EventConsumer & eventConsumer, id_type whichThread );

    /**
     * \brief   Unregisters an event consumer for a specific event class from the specified dispatcher thread.
     *
     * \param   classId             The runtime class ID of the event.
     * \param   eventConsumer       The consumer to unregister.
     * \param   dispThread          The dispatcher thread.
     * \return  True if unregistration succeeded; false otherwise.
     **/
    static bool remove_listener(const RuntimeClassID & classId, EventConsumer & eventConsumer, DispatcherThread & dispThread);

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
protected:
    Event();

    /**
     * \brief   Creates an event of the specified type.
     **/
    Event( areg::EventType eventType );

    virtual ~Event();

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// Event class overrides
/************************************************************************/

    /**
     * \brief   Destroys the event. Override this method to perform cleanup before destruction.
     **/
    void destroy() override;

    /**
     * \brief   Dispatches this event to the specified consumer. Override to implement custom
     *          dispatch logic.
     *
     * \param   consumer    The event consumer to dispatch to.
     **/
    virtual void dispatch_self( EventConsumer * consumer );

    /**
     * \brief   Delivers this event to its target thread. If target thread is null, delivers to
     *          current thread.
     **/
    virtual void deliver_event();

    /**
     * \brief   Registers a consumer for this event in the target thread. If target thread is not
     *          set, registers in current thread.
     *
     * \param   eventConsumer       The consumer to register.
     * \return  True if registration succeeded; false if failed or consumer already registered.
     **/
    virtual bool add_event_listener( EventConsumer & eventConsumer );

    /**
     * \brief   Unregisters a consumer from this event in the target thread. If target thread is
     *          null, unregisters from current thread.
     *
     * \param   eventConsumer       The consumer to unregister.
     * \return  True if unregistration succeeded; false otherwise.
     **/
    virtual bool remove_event_listener( EventConsumer & eventConsumer );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Registers this event with the specified thread by ID. If zero, uses current thread.
     *
     * \param   whichThread     The thread ID. Zero means current thread.
     * \return  True if the thread exists, is running, and ready; false otherwise.
     **/
    bool register_for_thread( id_type whichThread = 0);

    /**
     * \brief   Registers this event with the thread of the specified name.
     *
     * \param   whichThread     The name of the dispatcher thread.
     * \return  True if the thread exists, is running, and ready; false otherwise.
     **/
    bool register_for_thread( const char* whichThread );

    /**
     * \brief   Sets the target dispatcher thread for this event. If null, events are delivered to
     *          current thread.
     *
     * \param   dispatchThread      The target dispatcher thread. Null means current thread.
     * \return  True if the thread is not null and ready; false otherwise.
     **/
    bool register_for_thread( DispatcherThread * dispatchThread );

    /**
     * \brief   Returns true if the target thread has a consumer registered for this event.
     **/
    [[nodiscard]]
    bool is_event_registered() const noexcept;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Returns the type of this event.
     **/
    [[nodiscard]]
    inline areg::EventType event_type() const noexcept;
    /**
     * \brief   Sets the type of this event.
     * \param   eventType       The event type to set.
     **/
    inline void set_event_type( areg::EventType eventType ) noexcept;

    /**
     * \brief   Returns the priority of this event.
     **/
    [[nodiscard]]
    inline EventPriority event_priority() const noexcept;

    /**
     * \brief   Sets the priority of this event.
     * \param   eventPrio       The priority to set.
     **/
    inline void set_event_priority(EventPriority eventPrio) noexcept;

    /**
     * \brief   Returns the consumer registered to process this event, or null if none is set.
     **/
    [[nodiscard]]
    inline EventConsumer * event_consumer() noexcept;
    /**
     * \brief   Sets the consumer to process this event.
     * \param   consumer    The consumer to set.
     **/
    inline void set_event_consumer( EventConsumer * consumer ) noexcept;

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
     * \brief   Returns true if this event is remote (can be processed locally or in other
     *          processes).
     **/
    [[nodiscard]]
    inline bool is_remote() const noexcept;

    /**
     * \brief   Returns true if this is a developer custom event, false if system predefined.
     **/
    [[nodiscard]]
    inline bool is_custom() const noexcept;

//////////////////////////////////////////////////////////////////////////
// Protected members
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Returns the dispatcher of the target thread, or current thread if target is unknown.
     **/
    [[nodiscard]]
    EventDispatcher & dispatcher() const noexcept;

//////////////////////////////////////////////////////////////////////////
// Hidden methods.
//////////////////////////////////////////////////////////////////////////
private:
    [[nodiscard]]
    inline Event & self() noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   The event type
     **/
    EventType           mEventType;
    /**
     * \brief   The event priority
     **/
    EventPriority       mEventPrio;
    /**
     * \brief   Event consumer.
     **/
    EventConsumer*      mConsumer;
    /**
     * \brief   Target thread.
     **/
    DispatcherThread*   mTargetThread;

//////////////////////////////////////////////////////////////////////////
// Forbidden method calls.
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( Event );
};

//////////////////////////////////////////////////////////////////////////
// Event class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline areg::EventType Event::event_type() const noexcept
{
    return mEventType;
}

inline void Event::set_event_type( areg::EventType eventType ) noexcept
{
    mEventType = eventType;
}

inline EventConsumer * Event::event_consumer() noexcept
{
    return mConsumer;
}

inline void Event::set_event_consumer( EventConsumer * consumer ) noexcept
{
    mConsumer = consumer;
}

inline bool Event::is_internal() const noexcept
{
    return areg::is_internal(mEventType);
}

inline bool Event::is_external() const noexcept
{
    return areg::is_external( mEventType );
}

inline bool Event::is_local() const noexcept
{
    return areg::is_local( mEventType );
}

inline bool Event::is_remote() const noexcept
{
    return areg::is_remote( mEventType );
}

inline bool Event::is_custom() const noexcept
{
    return areg::is_custom( mEventType );
}

inline areg::EventPriority Event::event_priority() const noexcept
{
    return mEventPrio;
}

inline void Event::set_event_priority(areg::EventPriority eventPrio) noexcept
{
    mEventPrio = eventPrio;
}

} // namespace areg
#endif  // AREG_COMPONENT_EVENT_HPP
