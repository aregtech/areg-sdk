#ifndef AREG_COMPONENT_EVENT_HPP
#define AREG_COMPONENT_EVENT_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/component/Event.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Event Base class declaration.
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

#include "areg/base/IEIOStream.hpp"

/************************************************************************
 * \brief   Predefined MACRO to use for event declaration and implementation
 ************************************************************************/

/**
 * \brief   MACRO, declares static functions to add and remove
 *          event consumer, which should be available in every Event class.
 *          Do not use them directly, instead use DECLARE_RUNTIME_EVENT
 **/
#define DECLARE_EVENT_STATIC_REGISTRATION(EventClass)                                                       \
    public:                                                                                                 \
        /*  Declare static function to add/register event consumer to start processing event.       */      \
        static bool addListener(IEEventConsumer& eventConsumer, const char* whichThread = nullptr);         \
        /*  Declare static function to add/register event consumer to start processing event.       */      \
        static bool addListener(IEEventConsumer& eventConsumer, id_type whichThread);                       \
        /*  Declare static function to add/register event consumer to start processing event.       */      \
        static bool addListener(IEEventConsumer& eventConsumer, DispatcherThread & dispThread);             \
        /*  Declare static function to remove/unregister event consumer to stop processing event.   */      \
        static bool removeListener(IEEventConsumer& eventConsumer, const char* whichThread = nullptr);      \
        /*  Declare static function to remove/unregister event consumer to stop processing event.   */      \
        static bool removeListener(IEEventConsumer& eventConsumer, id_type whichThread);                    \
        /*  Declare static function to remove/unregister event consumer to stop processing event.   */      \
        static bool removeListener(IEEventConsumer& eventConsumer, DispatcherThread & dispThread);

/**
 * \brief   MACRO, implements static functions to add and remove
 *          event consumer, which should be available in every Event class.
 *          Do not use them directly, instead use IMPLEMENT_RUNTIME_EVENT
 **/
#define IMPLEMENT_EVENT_STATIC_REGISTRATION(EventClass)                                                     \
    /*  Implementation of adding / registering event consumer.                                  */          \
    bool EventClass::addListener(IEEventConsumer& eventConsumer, const char* whichThread /*= nullptr*/)     \
    {   return Event::addListener(EventClass::_getClassId(), eventConsumer, whichThread);       }           \
    /*  Implementation of adding / registering event consumer.                                  */          \
    bool EventClass::addListener(IEEventConsumer& eventConsumer, id_type whichThread)                       \
    {   return Event::addListener(EventClass::_getClassId(), eventConsumer, whichThread);       }           \
    /*  Implementation of adding / registering event consumer.                                  */          \
    bool EventClass::addListener(IEEventConsumer& eventConsumer, DispatcherThread & dispThread)             \
    {   return Event::addListener(EventClass::_getClassId(), eventConsumer, dispThread);        }           \
    /*  Implementation of removing / unregistering event consumer.                              */          \
    bool EventClass::removeListener(IEEventConsumer& eventConsumer, const char* whichThread /*= nullptr*/)  \
    {   return Event::removeListener(EventClass::_getClassId(), eventConsumer, whichThread);    }           \
    /*  Implementation of removing / unregistering event consumer.                              */          \
    bool EventClass::removeListener(IEEventConsumer& eventConsumer, id_type whichThread)                    \
    {   return Event::removeListener(EventClass::_getClassId(), eventConsumer, whichThread);    }           \
    /*  Implementation of removing / unregistering event consumer.                              */          \
    bool EventClass::removeListener(IEEventConsumer& eventConsumer, DispatcherThread & dispThread)          \
    {   return Event::removeListener(EventClass::_getClassId(), eventConsumer, dispThread);     }


/**
 * \brief   MACRO, to declare Event object as a runtime object
 *          to be able to check the type of event, find right consumer
 *          and trigger right event processing function call.
 *          Use this MACRO in every declared event class.
 *
 * \param   EventClass  Event class name.
 **/
#define DECLARE_RUNTIME_EVENT(EventClass)                                                                   \
    /*  Declare runtime functions and objects.                                                  */          \
    DECLARE_RUNTIME(EventClass)                                                                             \
    /*  Declare static functions to add and remove  event consumer.                             */          \
    DECLARE_EVENT_STATIC_REGISTRATION(EventClass)

/**
 * \brief   MACRO, to implement appropriate runtime and event functions
 *          Use this MACRO in every event class source code.
 *
 * \param   EventClass          Event class name.
 * \param   EventBaseClass      The base (parent) class of Event
 *                              At least it should be Event.
 **/
#define IMPLEMENT_RUNTIME_EVENT(EventClass, EventBaseClass)                                                 \
    /*  Implement event runtime functions.                                                      */          \
    IMPLEMENT_RUNTIME(EventClass, EventBaseClass)                                                           \
    /*  Implement event static functions.                                                       */          \
    IMPLEMENT_EVENT_STATIC_REGISTRATION(EventClass)


/************************************************************************
 * Dependencies
 ************************************************************************/
class IEEventConsumer;
class EventDispatcher;
class DispatcherThread;
class Thread;

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
 *          methods. User DECLARE_RUNTIME_EVENT() and IMPLEMENT_RUNTIME_EVENT()
 *          macros to have appropriate method definition in the event object.
 * 
 *          In addition, the system contains several predefined event objects,
 *          which are used by the system.
 *
 **/
class AREG_API Event   : public RuntimeObject
{
//////////////////////////////////////////////////////////////////////////
// Defines and constants.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Event::eEventType
     *          Event types.
     **/
    typedef enum class E_EventType  : unsigned int
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

    } eEventType;

    /**
     * \return Returns string value of Event::eEventType
     **/
    static inline const char* getString( Event::eEventType eventType );

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
    DECLARE_RUNTIME_EVENT(Event)

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
    static bool addListener(const RuntimeClassID & classId, IEEventConsumer & eventConsumer, const char* whichThread);

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
    static bool addListener( const RuntimeClassID & classId, IEEventConsumer & eventConsumer, id_type whichThread );

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
    static bool addListener(const RuntimeClassID & classId, IEEventConsumer & eventConsumer, DispatcherThread & dispThread);

    /**
     * \brief	Static method to remove listener from specified thread,
     *          i.e. unregister consumer in specified thread.
     * \param	classId	        Runtime ID of event class.
     * \param	eventConsumer	Consumer to unregister.
     * \param	whichThread	    Thread name to unregister. If this is nullptr or empty,
     *                          it will use current thread to unregister consumer.
     * \return	Returns true if successfully unregistered.
     **/
    static bool removeListener(const RuntimeClassID & classId, IEEventConsumer & eventConsumer, const char* whichThread);

    /**
     * \brief	Static method to remove listener from specified thread,
     *          i.e. unregister consumer in specified thread.
     * \param	classId	        Runtime ID of event class.
     * \param	eventConsumer	Consumer to unregister.
     * \param	whichThread	    The valid registered thread ID to remove listener.
     * \return	Returns true if successfully unregistered.
     **/
    static bool removeListener( const RuntimeClassID & classId, IEEventConsumer & eventConsumer, id_type whichThread );

    /**
     * \brief	Static method to remove listener from specified thread,
     *          i.e. unregister consumer in specified thread.
     * \param	classId	        Runtime ID of event class.
     * \param	eventConsumer	Consumer to unregister.
     * \param	dispThread	    The dispatcher thread, which dispatches messages
     * \return	Returns true if successfully unregistered.
     **/
    static bool removeListener(const RuntimeClassID & classId, IEEventConsumer & eventConsumer, DispatcherThread & dispThread);

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Default constructor.
     **/
    Event( void );

    /**
     * \brief   Initialization constructor. 
     *          Creates event object of specified type.
     * \param   eventType   The type of Event.
     **/
    Event( Event::eEventType eventType );
    
    /**
     * \brief   Destructor.
     **/
    virtual ~Event( void );

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
    virtual void destroy( void ) override;

    /**
     * \brief	Dispatch event itself. Overwrite function if needed.
     * \param	consumer	Registered event consumer
     **/
    virtual void dispatchSelf( IEEventConsumer * consumer );

    /**
     * \brief   Delivers the event to target thread. If target thread
     *          is nullptr, it delivers to current thread.
     **/
    virtual void deliverEvent( void );

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
    virtual bool addEventListener( IEEventConsumer & eventConsumer );

    /**
     * \brief	Removes listener from target thread, i.e. unregisters consumer
     *          from target thread. If target thread is nullptr it will unregister
     *          consumer in current thread.
     * \param	eventConsumer	The consumer object to unregister for current
     *                          event class.
     * \return	Returns true if successfully unregistered consumer.
     **/
    virtual bool removeEventListener( IEEventConsumer & eventConsumer );

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
    bool isEventRegistered( void ) const;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Returns the type of Event.
     *          For more information see description of Event::eEventType
     * \see Event::eEventType
     **/
    inline Event::eEventType getEventType( void ) const;
    /**
     * \brief   Sets the type of Event.
     *          For more information see description of Event::eEventType
     * \param   eventType   The type of Event.
     * \see Event::eEventType
     **/
    inline void setEventType( Event::eEventType eventType );

    /**
     * \brief   Returns pointer of Event Consumer object.
     *          If nullptr, no Event Consumer is set and the Event cannot be processed.
     **/
    inline IEEventConsumer * getEventConsumer( void );
    /**
     * \brief   Sets the Event Consumer object.
     * \param   consumer    The Event Consumer object, which should process event
     **/
    inline void setEventConsumer( IEEventConsumer * consumer );

    /**
     * \brief   Checks whether the given event type is internal or not.
     * \param   eventType   The event type to check.
     **/
    inline static bool isInternal( Event::eEventType eventType );

    /**
     * \brief   Checks whether the given event type is external or not.
     * \param   eventType   The event type to check.
     **/
    inline static bool isExternal( Event::eEventType eventType );

    /**
     * \brief   Checks whether the given event type is local or not.
     * \param   eventType   The event type to check.
     **/
    inline static bool isLocal( Event::eEventType eventType );

    /**
     * \brief   Checks whether the given event type is remote or not.
     * \param   eventType   The event type to check.
     **/
    inline static bool isRemote( Event::eEventType eventType );

    /**
     * \brief   Checks whether the given event type is developer custom or system predefined.
     * \param   eventType   The event type to check.
     **/
    inline static bool isCustom( Event::eEventType eventType );

    /**
     * \brief   Returns true, if event is internal, i.e. should be queued in internal event queue
     **/
    inline bool isInternal( void ) const;

    /**
     * \brief   Returns true, if event is external, i.e. should be queued in external event queue
     **/
    inline bool isExternal( void ) const;

    /**
     * \brief   Returns true, if event is local, i.e. cannot be processed in other process
     **/
    inline bool isLocal( void ) const;

    /**
     * \brief   Returns true, if event is remote, i.e. can be processed local and in other process
     **/
    inline bool isRemote( void ) const;

    /**
     * \brief   Returns true, if event is developer custom to communicate with worker thread or system predefined.
     **/
    inline bool isCustom( void ) const;

//////////////////////////////////////////////////////////////////////////
// Protected members
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Returns reference to dispatcher of target or current thread.
     *          If target thread is unknown, this will return dispatcher of
     *          current thread.
     **/
    EventDispatcher & getDispatcher( void ) const;

//////////////////////////////////////////////////////////////////////////
// Hidden methods.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns reference to event object
     **/
    inline Event & self( void );

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   The event type
     **/
    eEventType          mEventType;
    /**
     * \brief   Event consumer.
     **/
    IEEventConsumer*    mConsumer;
    /**
     * \brief   Target thread.
     **/
    DispatcherThread*   mTargetThread;

//////////////////////////////////////////////////////////////////////////
// Forbidden method calls.
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( Event );
};

IMPLEMENT_STREAMABLE(Event::eEventType)

//////////////////////////////////////////////////////////////////////////
// Event class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline Event::eEventType Event::getEventType( void ) const
{
    return mEventType;
}

inline void Event::setEventType( Event::eEventType eventType )
{
    mEventType = eventType;
}

inline IEEventConsumer * Event::getEventConsumer( void )
{
    return mConsumer;
}

inline void Event::setEventConsumer( IEEventConsumer * consumer )
{
    mConsumer = consumer;
}

inline bool Event::isInternal( Event::eEventType eventType )
{
    return (static_cast<unsigned int>(eventType) & static_cast<unsigned int>(Event::eEventType::EventInternal)) != 0;
}

inline bool Event::isExternal( Event::eEventType eventType )
{
    return (static_cast<unsigned int>(eventType) & static_cast<unsigned int>(Event::eEventType::EventExternal)) != 0;
}

inline bool Event::isLocal( Event::eEventType eventType )
{
    return (static_cast<unsigned int>(eventType) & static_cast<unsigned int>(Event::eEventType::EventLocal)) != 0;
}

inline bool Event::isRemote( Event::eEventType eventType )
{
    return (static_cast<unsigned int>(eventType) & static_cast<unsigned int>(Event::eEventType::EventRemote)) != 0;
}

inline bool Event::isCustom( Event::eEventType eventType )
{
    return (static_cast<unsigned int>(eventType) & static_cast<unsigned int>(Event::eEventType::EventCustom)) != 0;
}

inline bool Event::isInternal(void) const
{
    return Event::isInternal(mEventType);
}

inline bool Event::isExternal(void) const
{
    return Event::isExternal( mEventType );
}

inline bool Event::isLocal(void) const
{
    return Event::isLocal( mEventType );
}

inline bool Event::isRemote(void) const
{
    return Event::isRemote( mEventType );
}

inline bool Event::isCustom( void ) const
{
    return Event::isCustom( mEventType );
}

inline const char* Event::getString(Event::eEventType eventType)
{

    switch ( eventType )
    {
    case Event::eEventType::EventUnknown:
        return "Event::eEventType::EventUnknown";

    case Event::eEventType::EventInternal:
        return "Event::eEventType::EventInternal";
    case Event::eEventType::EventExternal:
        return "Event::eEventType::EventExternal";

    case Event::eEventType::EventLocal:
        return "Event::eEventType::EventLocal";
    case Event::eEventType::EventRemote:
        return "Event::eEventType::EventRemote";

    case Event::eEventType::EventNotify:
        return "Event::eEventType::EventNotify";
    case Event::eEventType::EventToStub:
        return "Event::eEventType::EventToStub";
    case Event::eEventType::EventToProxy:
        return "Event::eEventType::EventToProxy";
    case Event::eEventType::EventConnect:
        return "Event::eEventType::EventConnect";

    case Event::eEventType::EventNotifyClient:
        return "Event::eEventType::EventNotifyClient";

    case Event::eEventType::EventLocalServiceRequest:
        return "Event::eEventType::EventLocalServiceRequest";
    case Event::eEventType::EventRemoteServiceRequest:
        return "Event::eEventType::EventRemoteServiceRequest";

    case Event::eEventType::EventLocalNotifyRequest:
        return "Event::eEventType::EventLocalNotifyRequest";
    case Event::eEventType::EventRemoteNotifyRequest:
        return "Event::eEventType::EventRemoteNotifyRequest";

    case Event::eEventType::EventLocalServiceResponse:
        return "Event::eEventType::EventLocalServiceResponse";
    case Event::eEventType::EventRemoteServiceResponse:
        return "Event::eEventType::EventRemoteServiceResponse";

    case Event::eEventType::EventLocalStubConnect:
        return "Event::eEventType::EventLocalStubConnect";
    case Event::eEventType::EventRemoteStubConnect:
        return "Event::eEventType::EventRemoteStubConnect";

    case Event::eEventType::EventLocalProxyConnect:
        return "Event::eEventType::EventLocalProxyConnect";
    case Event::eEventType::EventRemoteProxyConnect:
        return "Event::eEventType::EventRemoteProxyConnect";

    case Event::eEventType::EventCustom:
        return "Event::eEventType::EventCustom";
    case Event::eEventType::EventCustomInternal:
        return "Event::eEventType::EventCustomInternal";
    case Event::eEventType::EventCustomExternal:
        return "Event::eEventType::EventCustomExternal";

    default:
        return "ERR: Undefined Event::eEventType value!";    
    }
}

#endif  // AREG_COMPONENT_EVENT_HPP
