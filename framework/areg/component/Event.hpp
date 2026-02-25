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
#include "areg/base/GEGlobal.h"
#include "areg/base/RuntimeObject.hpp"

#include "areg/base/IOStream.hpp"

/************************************************************************
 * \brief   Predefined MACRO to use for event declaration and implementation
 ************************************************************************/

/**
 * \brief   MACRO, declares static functions to add and remove
 *          event consumer, which should be available in every Event class.
 *          Do not use them directly, instead use AREG_DECLARE_RUNTIME_EVENT
 **/
#define AREG_DECLARE_EVENT_REGISTRATION(EventClass)                                                                     \
public:                                                                                                                 \
    /*  Declare static function to add/register event consumer to start processing event.       */                      \
    static bool add_listener(EventConsumer& eventConsumer, const String & whichThread = String::empty_string());       \
    /*  Declare static function to add/register event consumer to start processing event.       */                      \
    static bool add_listener(EventConsumer& eventConsumer, id_type whichThread);                                         \
    /*  Declare static function to add/register event consumer to start processing event.       */                      \
    static bool add_listener(EventConsumer& eventConsumer, DispatcherThread & dispThread);                               \
    /*  Declare static function to remove/unregister event consumer to stop processing event.   */                      \
    static bool remove_listener(EventConsumer& eventConsumer, const String & whichThread = String::empty_string());    \
    /*  Declare static function to remove/unregister event consumer to stop processing event.   */                      \
    static bool remove_listener(EventConsumer& eventConsumer, id_type whichThread);                                      \
    /*  Declare static function to remove/unregister event consumer to stop processing event.   */                      \
    static bool remove_listener(EventConsumer& eventConsumer, DispatcherThread & dispThread);

/**
 * \brief   MACRO, implements static functions to add and remove
 *          event consumer, which should be available in every Event class.
 *          Do not use them directly, instead use AREG_IMPLEMENT_RUNTIME_EVENT
 **/
#define AREG_IMPLEMENT_EVENT_REGISTRATION(EventClass)                                                                  \
    bool EventClass::add_listener(EventConsumer& eventConsumer, const String & whichThread)                            \
    {   return Event::add_listener(EventClass::_class_id(), eventConsumer, whichThread);       }                       \
    bool EventClass::add_listener(EventConsumer& eventConsumer, id_type whichThread)                                   \
    {   return Event::add_listener(EventClass::_class_id(), eventConsumer, whichThread);       }                       \
    bool EventClass::add_listener(EventConsumer& eventConsumer, DispatcherThread & dispThread)                         \
    {   return Event::add_listener(EventClass::_class_id(), eventConsumer, dispThread);        }                       \
    bool EventClass::remove_listener(EventConsumer& eventConsumer, const String& whichThread)                          \
    {   return Event::remove_listener(EventClass::_class_id(), eventConsumer, whichThread);    }                       \
    bool EventClass::remove_listener(EventConsumer& eventConsumer, id_type whichThread)                                \
    {   return Event::remove_listener(EventClass::_class_id(), eventConsumer, whichThread);    }                       \
    bool EventClass::remove_listener(EventConsumer& eventConsumer, DispatcherThread & dispThread)                      \
    {   return Event::remove_listener(EventClass::_class_id(), eventConsumer, dispThread);     }


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
class EventConsumer;
class EventDispatcher;
class DispatcherThread;
class Thread;

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
     * \brief   Event::EventType
     *          Event types.
     **/
    enum class EventType    : uint32_t
    {
          EventUnknown              =    0  /*0x0000*/  //!< Unknown event type.      Bit set: 0000 0000 0000 0000

        , EventInternal             =    1  /*0x0001*/  //!< Internal event.          Bit set: 0000 0000 0000 0001
        , EventExternal             =    2  /*0x0002*/  //!< External event.          Bit set: 0000 0000 0000 0010

        , EventLocal                =   16  /*0x0010*/  //!< Local event.             Bit set: 0000 0000 0001 0000
        , EventRemote               =   32  /*0x0020*/  //!< Remote event.            Bit set: 0000 0000 0010 0000

        , EventNotify               =  256  /*0x0100*/  //!< Notification event.      Bit set: 0000 0001 0000 0000
        , EventToStub               =  512  /*0x0200*/  //!< Stub event.              Bit set: 0000 0010 0000 0000
        , EventToProxy              = 1024  /*0x0400*/  //!< Proxy event.             Bit set: 0000 0100 0000 0000
        , EventConnect              = 2048  /*0x0800*/  //!< Connection event.        Bit set: 0000 1000 0000 0000

        , EventNotifyClient         =  273  /*0x0111*/  //!< Client Notification.     Bit set: 0000 0001 0001 0001

        , EventLocalServiceRequest  =  530  /*0x0212*/  //!< Local Service Request.   Bit set: 0000 0010 0001 0010
        , EventRemoteServiceRequest =  546  /*0x0222*/  //!< Remote Service Request.  Bit set: 0000 0010 0010 0010

        , EventLocalNotifyRequest   =  786  /*0x0312*/  //!< Local Request notify.    Bit set: 0000 0011 0001 0010
        , EventRemoteNotifyRequest  =  802  /*0x0322*/  //!< Remote  Request notify.  Bit set: 0000 0011 0010 0010

        , EventLocalServiceResponse = 1042  /*0x0412*/  //!< Local Service Response.  Bit set: 0000 0100 0001 0010
        , EventRemoteServiceResponse= 1058  /*0x0422*/  //!< Remote Service Response. Bit set: 0000 0100 0010 0010

        , EventLocalStubConnect     = 2834  /*0x0B12*/  //!< Local Stub Connection.   Bit set: 0000 1011 0001 0010
        , EventRemoteStubConnect    = 2850  /*0x0B22*/  //!< Remote Stub Connection.  Bit set: 0000 1011 0010 0010

        , EventLocalProxyConnect    = 3346  /*0x0D12*/  //!< Local Proxy Connection.  Bit set: 0000 1101 0001 0010
        , EventRemoteProxyConnect   = 3362  /*0x0D22*/  //!< Remote Proxy Connection. Bit set: 0000 1101 0010 0010

        , EventCustom               =32784  /*0x8010*/  //!< Custom event.            Bit set: 1000 0000 0001 0000
        , EventCustomInternal       =32785  /*0x8011*/  //!< Custom Notification.     Bit set: 1000 0000 0001 0001
        , EventCustomExternal       =32786  /*0x8012*/  //!< Custom Thread event.     Bit set: 1000 0000 0001 0010

    };

    /**
     * \brief   Converts an event type to its string representation.
     *
     * \param   eventType       The event type to convert.
     * \return  String representation of the event type.
     **/
    static inline const char* as_string(Event::EventType eventType);

    /**
     * \brief   Event::EventPriority
     *          The priorities of the events.
     **/
    enum class EventPriority  : uint32_t
    {
          UndefinedPrio    //!< Undefined priority, should not be used.
        , LowPrio          //!< The priority of the event is low, should be processed only if there is no event in the queue.
        , NormalPrio       //!< The priority of the event is normal, should be processed in FIFO principle.
        , HighPrio         //!< The priority of the event is high, should be processed before any other events.
        , CriticalPrio     //!< The priority of the event is critical, should be processed nearly immediately.
        , IgnorePrio       //!< The priority of event should be ignored. Should be set in event.
        , ExitPrio         //!< The highest priority of the event. Should be processed as soon as possible and should not be removed from the stack.
    };

    /**
     * \brief   Event::DefaultPriority
     *          The default priority of the events.
     **/
    static constexpr EventPriority DefaultPriority  { EventPriority::NormalPrio };

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
     * \brief   Unregisters an event consumer for a specific event class from the specified thread
     *          by ID.
     *
     * \param   classId             The runtime class ID of the event.
     * \param   eventConsumer       The consumer to unregister.
     * \param   whichThread         The thread ID.
     * \return  True if unregistration succeeded; false otherwise.
     **/
    static bool remove_listener( const RuntimeClassID & classId, EventConsumer & eventConsumer, id_type whichThread );

    /**
     * \brief   Unregisters an event consumer for a specific event class from the specified
     *          dispatcher thread.
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
    /**
     * \brief   Default constructor. Creates an uninitialized event.
     **/
    Event();

    /**
     * \brief   Creates an event of the specified type.
     *
     * \param   eventType       The type of event to create.
     **/
    Event( Event::EventType eventType );

    /**
     * \brief   Destructor.
     **/
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
    bool is_event_registered() const;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Returns the type of this event.
     **/
    inline Event::EventType event_type() const;
    /**
     * \brief   Sets the type of this event.
     *
     * \param   eventType       The event type to set.
     **/
    inline void set_event_type( Event::EventType eventType );

    /**
     * \brief   Returns the priority of this event.
     **/
    inline EventPriority event_priority() const;

    /**
     * \brief   Sets the priority of this event.
     *
     * \param   eventPrio       The priority to set.
     **/
    inline void set_event_priority(EventPriority eventPrio);

    /**
     * \brief   Returns the consumer registered to process this event, or null if none is set.
     **/
    inline EventConsumer * event_consumer();
    /**
     * \brief   Sets the consumer to process this event.
     *
     * \param   consumer    The consumer to set.
     **/
    inline void set_event_consumer( EventConsumer * consumer );

    /**
     * \brief   Returns true if the event type is internal.
     *
     * \param   eventType       The event type to check.
     **/
    inline static bool is_internal( Event::EventType eventType );

    /**
     * \brief   Returns true if the event type is external.
     *
     * \param   eventType       The event type to check.
     **/
    inline static bool is_external( Event::EventType eventType );

    /**
     * \brief   Returns true if the event type is local.
     *
     * \param   eventType       The event type to check.
     **/
    inline static bool is_local( Event::EventType eventType );

    /**
     * \brief   Returns true if the event type is remote.
     *
     * \param   eventType       The event type to check.
     **/
    inline static bool is_remote( Event::EventType eventType );

    /**
     * \brief   Returns true if the event type is developer custom, false if system predefined.
     *
     * \param   eventType       The event type to check.
     **/
    inline static bool is_custom( Event::EventType eventType );

    /**
     * \brief   Returns true if this event is internal (queued in internal queue).
     **/
    inline bool is_internal() const;

    /**
     * \brief   Returns true if this event is external (queued in external queue).
     **/
    inline bool is_external() const;

    /**
     * \brief   Returns true if this event is local (cannot be processed in other processes).
     **/
    inline bool is_local() const;

    /**
     * \brief   Returns true if this event is remote (can be processed locally or in other
     *          processes).
     **/
    inline bool is_remote() const;

    /**
     * \brief   Returns true if this is a developer custom event, false if system predefined.
     **/
    inline bool is_custom() const;

//////////////////////////////////////////////////////////////////////////
// Protected members
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Returns the dispatcher of the target thread, or current thread if target is unknown.
     **/
    EventDispatcher & dispatcher() const;

//////////////////////////////////////////////////////////////////////////
// Hidden methods.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief
     **/
    inline Event & self();

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   The event type
     **/
    EventType          mEventType;
    /**
     * \brief   The event priority
     **/
    EventPriority      mEventPrio;
    /**
     * \brief   Event consumer.
     **/
    EventConsumer*    mConsumer;
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

AREG_IMPLEMENT_STREAMABLE(Event::EventType)

//////////////////////////////////////////////////////////////////////////
// Event class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline Event::EventType Event::event_type() const
{
    return mEventType;
}

inline void Event::set_event_type( Event::EventType eventType )
{
    mEventType = eventType;
}

inline EventConsumer * Event::event_consumer()
{
    return mConsumer;
}

inline void Event::set_event_consumer( EventConsumer * consumer )
{
    mConsumer = consumer;
}

inline bool Event::is_internal( Event::EventType eventType )
{
    return (static_cast<uint32_t>(eventType) & static_cast<uint32_t>(Event::EventType::EventInternal)) != 0;
}

inline bool Event::is_external( Event::EventType eventType )
{
    return (static_cast<uint32_t>(eventType) & static_cast<uint32_t>(Event::EventType::EventExternal)) != 0;
}

inline bool Event::is_local( Event::EventType eventType )
{
    return (static_cast<uint32_t>(eventType) & static_cast<uint32_t>(Event::EventType::EventLocal)) != 0;
}

inline bool Event::is_remote( Event::EventType eventType )
{
    return (static_cast<uint32_t>(eventType) & static_cast<uint32_t>(Event::EventType::EventRemote)) != 0;
}

inline bool Event::is_custom( Event::EventType eventType )
{
    return (static_cast<uint32_t>(eventType) & static_cast<uint32_t>(Event::EventType::EventCustom)) != 0;
}

inline bool Event::is_internal() const
{
    return Event::is_internal(mEventType);
}

inline bool Event::is_external() const
{
    return Event::is_external( mEventType );
}

inline bool Event::is_local() const
{
    return Event::is_local( mEventType );
}

inline bool Event::is_remote() const
{
    return Event::is_remote( mEventType );
}

inline bool Event::is_custom() const
{
    return Event::is_custom( mEventType );
}

inline Event::EventPriority Event::event_priority() const
{
    return mEventPrio;
}

inline void Event::set_event_priority(Event::EventPriority eventPrio)
{
    mEventPrio = eventPrio;
}

inline const char* Event::as_string(Event::EventType eventType)
{

    switch ( eventType )
    {
    case Event::EventType::EventUnknown:
        return "Event::EventType::EventUnknown";

    case Event::EventType::EventInternal:
        return "Event::EventType::EventInternal";
    case Event::EventType::EventExternal:
        return "Event::EventType::EventExternal";

    case Event::EventType::EventLocal:
        return "Event::EventType::EventLocal";
    case Event::EventType::EventRemote:
        return "Event::EventType::EventRemote";

    case Event::EventType::EventNotify:
        return "Event::EventType::EventNotify";
    case Event::EventType::EventToStub:
        return "Event::EventType::EventToStub";
    case Event::EventType::EventToProxy:
        return "Event::EventType::EventToProxy";
    case Event::EventType::EventConnect:
        return "Event::EventType::EventConnect";

    case Event::EventType::EventNotifyClient:
        return "Event::EventType::EventNotifyClient";

    case Event::EventType::EventLocalServiceRequest:
        return "Event::EventType::EventLocalServiceRequest";
    case Event::EventType::EventRemoteServiceRequest:
        return "Event::EventType::EventRemoteServiceRequest";

    case Event::EventType::EventLocalNotifyRequest:
        return "Event::EventType::EventLocalNotifyRequest";
    case Event::EventType::EventRemoteNotifyRequest:
        return "Event::EventType::EventRemoteNotifyRequest";

    case Event::EventType::EventLocalServiceResponse:
        return "Event::EventType::EventLocalServiceResponse";
    case Event::EventType::EventRemoteServiceResponse:
        return "Event::EventType::EventRemoteServiceResponse";

    case Event::EventType::EventLocalStubConnect:
        return "Event::EventType::EventLocalStubConnect";
    case Event::EventType::EventRemoteStubConnect:
        return "Event::EventType::EventRemoteStubConnect";

    case Event::EventType::EventLocalProxyConnect:
        return "Event::EventType::EventLocalProxyConnect";
    case Event::EventType::EventRemoteProxyConnect:
        return "Event::EventType::EventRemoteProxyConnect";

    case Event::EventType::EventCustom:
        return "Event::EventType::EventCustom";
    case Event::EventType::EventCustomInternal:
        return "Event::EventType::EventCustomInternal";
    case Event::EventType::EventCustomExternal:
        return "Event::EventType::EventCustomExternal";

    default:
        return "ERR: Undefined Event::EventType value!";
    }
}

#endif  // AREG_COMPONENT_EVENT_HPP
