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
    static bool addListener(EventConsumer& eventConsumer, const areg::String & whichThread = areg::String::getEmptyString());       \
    /*  Declare static function to add/register event consumer to start processing event.       */                      \
    static bool addListener(EventConsumer& eventConsumer, id_type whichThread);                                         \
    /*  Declare static function to add/register event consumer to start processing event.       */                      \
    static bool addListener(EventConsumer& eventConsumer, DispatcherThread & dispThread);                               \
    /*  Declare static function to remove/unregister event consumer to stop processing event.   */                      \
    static bool removeListener(EventConsumer& eventConsumer, const areg::String & whichThread = areg::String::getEmptyString());    \
    /*  Declare static function to remove/unregister event consumer to stop processing event.   */                      \
    static bool removeListener(EventConsumer& eventConsumer, id_type whichThread);                                      \
    /*  Declare static function to remove/unregister event consumer to stop processing event.   */                      \
    static bool removeListener(EventConsumer& eventConsumer, DispatcherThread & dispThread);

/**
 * \brief   MACRO, implements static functions to add and remove
 *          event consumer, which should be available in every Event class.
 *          Do not use them directly, instead use AREG_IMPLEMENT_RUNTIME_EVENT
 **/
#define AREG_IMPLEMENT_EVENT_REGISTRATION(EventClass)                                                                  \
    bool EventClass::addListener(EventConsumer& eventConsumer, const areg::String & whichThread)                            \
    {   return Event::addListener(EventClass::_getClassId(), eventConsumer, whichThread);       }                       \
    bool EventClass::addListener(EventConsumer& eventConsumer, id_type whichThread)                                   \
    {   return Event::addListener(EventClass::_getClassId(), eventConsumer, whichThread);       }                       \
    bool EventClass::addListener(EventConsumer& eventConsumer, DispatcherThread & dispThread)                         \
    {   return Event::addListener(EventClass::_getClassId(), eventConsumer, dispThread);        }                       \
    bool EventClass::removeListener(EventConsumer& eventConsumer, const areg::String& whichThread)                          \
    {   return Event::removeListener(EventClass::_getClassId(), eventConsumer, whichThread);    }                       \
    bool EventClass::removeListener(EventConsumer& eventConsumer, id_type whichThread)                                \
    {   return Event::removeListener(EventClass::_getClassId(), eventConsumer, whichThread);    }                       \
    bool EventClass::removeListener(EventConsumer& eventConsumer, DispatcherThread & dispThread)                      \
    {   return Event::removeListener(EventClass::_getClassId(), eventConsumer, dispThread);     }


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
namespace areg { class Thread; }

//////////////////////////////////////////////////////////////////////////
// Event class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A generic Base Event class. All events are instances of Event
 *          class. Use unique custom or specific Event classes.
 *
 *          The events are forwarded to be processed in dispatcher thread
 *          and depending on even type, they are queued either in internal
 *          or external queue. The events are created dynamically in the heap
 *          to remain valid as long until they are processed. After processing
 *          the events are automatically destroyed.
 *
 *          Before sending Event to the target dispatcher thread, the thread
 *          should have registered event consumer object which processes
 *          the events. The event consumers are registered and unregistered in
 *          the dispatcher thread by calling addListener() and removeListener()
 *          methods. User AREG_DECLARE_RUNTIME_EVENT() and AREG_IMPLEMENT_RUNTIME_EVENT()
 *          macros to have appropriate method definition in the event object.
 *
 *          In addition, the system contains several predefined event objects,
 *          which are used by the system.
 *
 **/
class AREG_API Event   : public areg::RuntimeObject
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
     * \return Returns string value of Event::EventType
     **/
    static inline const char* getString(Event::EventType eventType);

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
     * \brief	Static method to add the listener to specified thread,
     *          i.e. registers consumer for specified event class.
     * \param	classId	        Runtime ID of event class.
     * \param	eventConsumer	Consumer to register.
     * \param	whichThread	    The registered thread name to add the
     *                          event consumer. If this is nullptr or empty,
     *                          it will use current thread for registering consumer.
     * \return	Returns true if successfully registered.
     *          Returns false, if failed or specified thread already had specified
     *          consumer registered for specified event class type.
     **/
    static bool addListener(const RuntimeClassID & classId, EventConsumer & eventConsumer, const areg::String & whichThread);

    /**
     * \brief	Static method to add the listener to specified thread,
     *          i.e. registers consumer for specified event class.
     * \param	classId	        Runtime ID of event class.
     * \param	eventConsumer	Consumer to register.
     * \param	whichThread	    The valid registered thread ID to add listener.
     * \return	Returns true if successfully registered.
     *          Returns false, if failed or specified thread already had specified
     *          consumer registered for specified event class type.
     **/
    static bool addListener( const RuntimeClassID & classId, EventConsumer & eventConsumer, id_type whichThread );

    /**
     * \brief	Static method to add the listener to specified thread,
     *          i.e. registers consumer for specified event class.
     * \param	classId	        Runtime ID of event class.
     * \param	eventConsumer	Consumer to register.
     * \param	dispThread	    The dispatcher thread, which dispatches messages
     * \return	Returns true if successfully registered.
     *          Returns false, if failed or specified thread already had specified
     *          consumer registered for specified event class type.
     **/
    static bool addListener(const RuntimeClassID & classId, EventConsumer & eventConsumer, DispatcherThread & dispThread);

    /**
     * \brief	Static method to remove listener from specified thread,
     *          i.e. unregister consumer in specified thread.
     * \param	classId	        Runtime ID of event class.
     * \param	eventConsumer	Consumer to unregister.
     * \param	whichThread	    Thread name to unregister. If this is nullptr or empty,
     *                          it will use current thread to unregister consumer.
     * \return	Returns true if successfully unregistered.
     **/
    static bool removeListener(const RuntimeClassID & classId, EventConsumer & eventConsumer, const areg::String & whichThread);

    /**
     * \brief	Static method to remove listener from specified thread,
     *          i.e. unregister consumer in specified thread.
     * \param	classId	        Runtime ID of event class.
     * \param	eventConsumer	Consumer to unregister.
     * \param	whichThread	    The valid registered thread ID to remove listener.
     * \return	Returns true if successfully unregistered.
     **/
    static bool removeListener( const RuntimeClassID & classId, EventConsumer & eventConsumer, id_type whichThread );

    /**
     * \brief	Static method to remove listener from specified thread,
     *          i.e. unregister consumer in specified thread.
     * \param	classId	        Runtime ID of event class.
     * \param	eventConsumer	Consumer to unregister.
     * \param	dispThread	    The dispatcher thread, which dispatches messages
     * \return	Returns true if successfully unregistered.
     **/
    static bool removeListener(const RuntimeClassID & classId, EventConsumer & eventConsumer, DispatcherThread & dispThread);

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Default constructor.
     **/
    Event();

    /**
     * \brief   Initialization constructor.
     *          Creates event object of specified type.
     * \param   eventType   The type of Event.
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
     * \brief   Call to destroy Event object.
     *          Overwrite if there is any special action should be performed
     *          before destroying event object.
     **/
    void destroy() override;

    /**
     * \brief	Dispatch event itself. Overwrite function if needed.
     * \param	consumer	Registered event consumer
     **/
    virtual void dispatchSelf( EventConsumer * consumer );

    /**
     * \brief   Delivers the event to target thread. If target thread
     *          is nullptr, it delivers to current thread.
     **/
    virtual void deliverEvent();

    /**
     * \brief	Adds the listener to target thread, i.e. registers
     *          consumer for the event. It uses runtime information
     *          as an event identifier. If target thread is not
     *          specified, it will register consumer in current thread.
     * \param	eventConsumer	Event consumer to add
     * \return	Returns true if could register consumer in target thread.
     *          Returns false if failed or target thread already had consumer
     *          registered for current event class.
     **/
    virtual bool addEventListener( EventConsumer & eventConsumer );

    /**
     * \brief	Removes listener from target thread, i.e. unregisters consumer
     *          from target thread. If target thread is nullptr it will unregister
     *          consumer in current thread.
     * \param	eventConsumer	The consumer object to unregister for current
     *                          event class.
     * \return	Returns true if successfully unregistered consumer.
     **/
    virtual bool removeEventListener( EventConsumer & eventConsumer );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Registers event for specified thread, i.e. sets target thread.
     *          By specified thread ID it will search dispatching thread and
     *          will set target thread object.
     * \param   whichThread     The ID of target thread. If this parameter is null
     *                          it will get current thread as an event target.
     * \return  Returns true if dispatching thread found in system, the thread is
     *          running and ready to dispatch events.
     **/
    bool registerForThread( id_type whichThread = 0);

    /**
     * \brief	Searches dispatcher thread by given name and sets as an event
     *          target thread.
     * \param	whichThread	    The unique name of event dispatcher thread.
     * \return  Returns true if dispatching thread found in system, the thread is
     *          running and ready to dispatch events.
     **/
    bool registerForThread( const char* whichThread );

    /**
     * \brief	Set or re-set target thread. If target thread is nullptr,
     *          all events will be forwarded to current thread.
     * \param	dispatchThread	Target Dispatcher thread
     * \return	Returns true if target thread is not nullptr and ready
     *          to dispatch events.
     **/
    bool registerForThread( DispatcherThread * dispatchThread );

    /**
     * \brief   Returns true if the target thread has a consumer
     *          registered for event. It uses runtime information
     *          as an event identifier.
     **/
    bool isEventRegistered() const;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Returns the type of Event.
     *          For more information see description of Event::EventType
     * \see Event::EventType
     **/
    inline Event::EventType getEventType() const;
    /**
     * \brief   Sets the type of Event.
     *          For more information see description of Event::EventType
     * \param   eventType   The type of Event.
     * \see Event::EventType
     **/
    inline void setEventType( Event::EventType eventType );

    /**
     * \brief   Returns the priority of the event.
     **/
    inline EventPriority getEventPriority() const;

    /**
     * \brief   Sets new priority of the event.
     **/
    inline void setEventPriority(EventPriority eventPrio);

    /**
     * \brief   Returns pointer of Event Consumer object.
     *          If nullptr, no Event Consumer is set and the Event cannot be processed.
     **/
    inline EventConsumer * getEventConsumer();
    /**
     * \brief   Sets the Event Consumer object.
     * \param   consumer    The Event Consumer object, which should process event
     **/
    inline void setEventConsumer( EventConsumer * consumer );

    /**
     * \brief   Checks whether the given event type is internal or not.
     * \param   eventType   The event type to check.
     **/
    inline static bool isInternal( Event::EventType eventType );

    /**
     * \brief   Checks whether the given event type is external or not.
     * \param   eventType   The event type to check.
     **/
    inline static bool isExternal( Event::EventType eventType );

    /**
     * \brief   Checks whether the given event type is local or not.
     * \param   eventType   The event type to check.
     **/
    inline static bool isLocal( Event::EventType eventType );

    /**
     * \brief   Checks whether the given event type is remote or not.
     * \param   eventType   The event type to check.
     **/
    inline static bool isRemote( Event::EventType eventType );

    /**
     * \brief   Checks whether the given event type is developer custom or system predefined.
     * \param   eventType   The event type to check.
     **/
    inline static bool isCustom( Event::EventType eventType );

    /**
     * \brief   Returns true, if event is internal, i.e. should be queued in internal event queue
     **/
    inline bool isInternal() const;

    /**
     * \brief   Returns true, if event is external, i.e. should be queued in external event queue
     **/
    inline bool isExternal() const;

    /**
     * \brief   Returns true, if event is local, i.e. cannot be processed in other process
     **/
    inline bool isLocal() const;

    /**
     * \brief   Returns true, if event is remote, i.e. can be processed local and in other process
     **/
    inline bool isRemote() const;

    /**
     * \brief   Returns true, if event is developer custom to communicate with worker thread or system predefined.
     **/
    inline bool isCustom() const;

//////////////////////////////////////////////////////////////////////////
// Protected members
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Returns reference to dispatcher of target or current thread.
     *          If target thread is unknown, this will return dispatcher of
     *          current thread.
     **/
    EventDispatcher & getDispatcher() const;

//////////////////////////////////////////////////////////////////////////
// Hidden methods.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns reference to event object
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

inline Event::EventType Event::getEventType() const
{
    return mEventType;
}

inline void Event::setEventType( Event::EventType eventType )
{
    mEventType = eventType;
}

inline EventConsumer * Event::getEventConsumer()
{
    return mConsumer;
}

inline void Event::setEventConsumer( EventConsumer * consumer )
{
    mConsumer = consumer;
}

inline bool Event::isInternal( Event::EventType eventType )
{
    return (static_cast<uint32_t>(eventType) & static_cast<uint32_t>(Event::EventType::EventInternal)) != 0;
}

inline bool Event::isExternal( Event::EventType eventType )
{
    return (static_cast<uint32_t>(eventType) & static_cast<uint32_t>(Event::EventType::EventExternal)) != 0;
}

inline bool Event::isLocal( Event::EventType eventType )
{
    return (static_cast<uint32_t>(eventType) & static_cast<uint32_t>(Event::EventType::EventLocal)) != 0;
}

inline bool Event::isRemote( Event::EventType eventType )
{
    return (static_cast<uint32_t>(eventType) & static_cast<uint32_t>(Event::EventType::EventRemote)) != 0;
}

inline bool Event::isCustom( Event::EventType eventType )
{
    return (static_cast<uint32_t>(eventType) & static_cast<uint32_t>(Event::EventType::EventCustom)) != 0;
}

inline bool Event::isInternal() const
{
    return Event::isInternal(mEventType);
}

inline bool Event::isExternal() const
{
    return Event::isExternal( mEventType );
}

inline bool Event::isLocal() const
{
    return Event::isLocal( mEventType );
}

inline bool Event::isRemote() const
{
    return Event::isRemote( mEventType );
}

inline bool Event::isCustom() const
{
    return Event::isCustom( mEventType );
}

inline Event::EventPriority Event::getEventPriority() const
{
    return mEventPrio;
}

inline void Event::setEventPriority(Event::EventPriority eventPrio)
{
    mEventPrio = eventPrio;
}

inline const char* Event::getString(Event::EventType eventType)
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
