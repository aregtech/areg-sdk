#ifndef AREG_COMPONENT_EVENT_HPP
#define AREG_COMPONENT_EVENT_HPP
/************************************************************************
 * \file        areg/component/Event.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
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
        static bool addListener(IEEventConsumer& eventConsumer, const char* whichThread = NULL);            \
        /*  Declare static function to add/register event consumer to start processing event.       */      \
        static bool addListener(IEEventConsumer& eventConsumer, DispatcherThread & dispThread);             \
        /*  Declare static function to remove/unregister event consumer to stop processing event.   */      \
        static bool removeListener(IEEventConsumer& eventConsumer, const char* whichThread = NULL);         \
        /*  Declare static function to remove/unregister event consumer to stop processing event.   */      \
        static bool removeListener(IEEventConsumer& eventConsumer, DispatcherThread & dispThread);

/**
 * \brief   MACRO, implements static functions to add and remove
 *          event consumer, which should be available in every Event class.
 *          Do not use them directly, instead use IMPLEMENT_RUNTIME_EVENT
 **/
#define IMPLEMENT_EVENT_STATIC_REGISTRATION(EventClass)                                                     \
    /*  Implementation of adding / registering event consumer.                                  */          \
    bool EventClass::addListener(IEEventConsumer& eventConsumer, const char* whichThread /*= NULL*/)        \
    {   return Event::addListener(EventClass::_getClassId(), eventConsumer, whichThread);      }            \
    /*  Implementation of adding / registering event consumer.                                  */          \
    bool EventClass::addListener(IEEventConsumer& eventConsumer, DispatcherThread & dispThread)             \
    {   return Event::addListener(EventClass::_getClassId(), eventConsumer, dispThread);      }             \
    /*  Implementation of removing / unregistering event consumer.                              */          \
    bool EventClass::removeListener(IEEventConsumer& eventConsumer, const char* whichThread /*= NULL*/)     \
    {   return Event::removeListener(EventClass::_getClassId(), eventConsumer, whichThread);   }            \
    /*  Implementation of removing / unregistering event consumer.                              */          \
    bool EventClass::removeListener(IEEventConsumer& eventConsumer, DispatcherThread & dispThread)          \
    {   return Event::removeListener(EventClass::_getClassId(), eventConsumer, dispThread);   }


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
 * \brief       This is Generic Base Event class.
 *              All events are instances of Event class.
 *              Because of this, do not create event of Event type,
 *              because it will be forwarded to all registered consumers.
 *              Instead, use unique custom or specific Event classes.
 *
 * \details     On the request to send Event, events are forwarded to
 *              processing dispatcher thread and queued either in 
 *              internal or external queue, depending on event type,
 *              which is described in the class. The events should be
 *              created by using 'new' operator. and should stay valid
 *              until are not processed. The caller should not delete
 *              event object, they will be automatically destroyed after
 *              processing is finished.
 *              Before sending Event, there should be appropriate Event
 *              Consumer registered in the concrete dispatcher thread.
 *              If no consumer is registered, processing event will fail.
 *              To register consumer for specific Event, use addListener()
 *              function (see above in MACRO definition).
 *              To unregister consumer for specific Event, call removeListener()
 *              function (see above in MACRO definition).
 *              There are several predefined event objects are provided,
 *              if a customer event is needed, use custom event declaration
 *              mechanism, described in TEEvent.hpp file.
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
    typedef enum E_EventType
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
     * \param	whichThread	    Thread name to register. If this is NULL or empty,
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
     * \param	whichThread	    Thread name to unregister. If this is NULL or empty,
     *                          it will use current thread to unregister consumer.
     * \return	Returns true if successfully unregistered.
     **/
    static bool removeListener(const RuntimeClassID & classId, IEEventConsumer & eventConsumer, const char* whichThread);

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
    virtual void destroy( void );

    /**
     * \brief	Dispatch event itself. Overwrite function if needed.
     * \param	consumer	Registered event consumer
     **/
    virtual void dispatchSelf( IEEventConsumer * consumer );

    /**
     * \brief   Sends the event to target thread. If target thread
     *          is NULL, it sends to current thread.
     **/
    virtual void sendEvent( void );

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
     *          from target thread. If target thread is NULL it will unregister
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
    bool registerForThread(ITEM_ID whichThread = 0);

    /**
     * \brief	Searches dispatcher thread by given name and sets as an event
     *          target thread.
     * \param	whichThread	    The unique name of event dispatcher thread.
     * \return  Returns true if dispatching thread found in system, the thread is
     *          running and ready to dispatch events.
     **/
    bool registerForThread( const char* whichThread );

    /**
     * \brief	Set or re-set target thread. If target thread is NULL,
     *          all events will be forwarded to current thread.
     * \param	dispatchThread	Target Dispatcher thread
     * \return	Returns true if target thread is not NULL and ready
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
     *          If NULL, no Event Consumer is set and the Event cannot be processed.
     **/
    inline IEEventConsumer * getEventConsumer( void );
    /**
     * \brief   Sets the Event Consumer object.
     * \param   consumer    The Event Consumer object, which should process event
     **/
    inline void setEventConsumer( IEEventConsumer * consumer );

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
// Hidden methods.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns reference to event object
     **/
    inline Event & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden method calls.
//////////////////////////////////////////////////////////////////////////
private:
    Event( const Event & /*src*/ );
    const Event & operator = ( const Event & /*src*/ );
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

inline bool Event::isInternal(void) const
{
    return (mEventType & Event::EventInternal) != 0;
}

inline bool Event::isExternal(void) const
{
    return (mEventType & Event::EventExternal) != 0;
}

inline bool Event::isLocal(void) const
{
    return (mEventType & Event::EventLocal) != 0;
}

inline bool Event::isRemote(void) const
{
    return (mEventType & Event::EventRemote) != 0;
}

inline Event& Event::self( void )
{
    return (*this);
}

inline const char* Event::getString(Event::eEventType eventType)
{

    switch ( eventType )
    {
    case Event::EventUnknown:
        return "Event::EventUnknown";

    case Event::EventInternal:
        return "Event::EventInternal";
    case Event::EventExternal:
        return "Event::EventExternal";

    case Event::EventLocal:
        return "Event::EventLocal";
    case Event::EventRemote:
        return "Event::EventRemote";

    case Event::EventNotify:
        return "Event::EventNotify";
    case Event::EventToStub:
        return "Event::EventToStub";
    case Event::EventToProxy:
        return "Event::EventToProxy";
    case Event::EventConnect:
        return "Event::EventConnect";

    case Event::EventNotifyClient:
        return "Event::EventNotifyClient";

    case Event::EventLocalServiceRequest:
        return "Event::EventLocalServiceRequest";
    case Event::EventRemoteServiceRequest:
        return "Event::EventRemoteServiceRequest";

    case Event::EventLocalNotifyRequest:
        return "Event::EventLocalNotifyRequest";
    case Event::EventRemoteNotifyRequest:
        return "Event::EventRemoteNotifyRequest";

    case Event::EventLocalServiceResponse:
        return "Event::EventLocalServiceResponse";
    case Event::EventRemoteServiceResponse:
        return "Event::EventRemoteServiceResponse";

    case Event::EventLocalStubConnect:
        return "Event::EventLocalStubConnect";
    case Event::EventRemoteStubConnect:
        return "Event::EventRemoteStubConnect";

    case Event::EventLocalProxyConnect:
        return "Event::EventLocalProxyConnect";
    case Event::EventRemoteProxyConnect:
        return "Event::EventRemoteProxyConnect";

    case Event::EventCustom:
        return "Event::EventCustom";
    case Event::EventCustomInternal:
        return "Event::EventCustomInternal";
    case Event::EventCustomExternal:
        return "Event::EventCustomExternal";

    default:
        return "ERR: Undefined Event::eEventType value!";    
    }
}

#endif  // AREG_COMPONENT_EVENT_HPP
