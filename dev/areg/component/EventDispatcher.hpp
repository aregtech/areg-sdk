#ifndef AREG_COMPONENT_EVENTDISPATCHER_HPP
#define AREG_COMPONENT_EVENTDISPATCHER_HPP
/************************************************************************
 * \file        areg/component/EventDispatcher.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Event Dispatcher class
 *              Posts, receives dispatcher event objects, registers
 *              event consumers in dispatcher registry and triggers
 *              appropriate event processing function of event consumer.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/IEThreadConsumer.hpp"
#include "areg/component/IEEventRouter.hpp"
#include "areg/component/private/EventDispatcherBase.hpp"

//////////////////////////////////////////////////////////////////////////
// EventDispatcher class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Event Dispatcher is a component of Dispatcher Thread to queue, 
 *          dispatch and process events in the thread. The dispatcher thread runs
 *          in the loop until gets exit event in the queue of Event Dispather.
 *          Do not make long processing and run methods in the long loop, 
 *          because this will delay event processing.
 **/
class AREG_API EventDispatcher  : public    IEThreadConsumer
                                , public    IEEventRouter
                                , protected EventDispatcherBase
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initialization constructor.
     * \param   name    The name of Event Dispatcher
     **/
    EventDispatcher( const char* name );
    /**
     * \brief   Destructor.
     **/
    virtual ~EventDispatcher( void );

//////////////////////////////////////////////////////////////////////////
// Override operations
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// IEThreadConsumer interface overrides
/************************************************************************/

    /**
     * \brief	Function triggered with thread object has been created.
     *          If this function returns true, thread will continue running.
     *          If this function returns false, the thread will not run.
     *          This function might be triggered from different thread
     *          than function run() is called.
     * \param	threadObj	The new created Thread object, 
     *                      which contains this consumer.
     * \return	Return true if thread should run. Return false, it it should not run.
     **/
    virtual bool onThreadRegistered( Thread * threadObj );

    /**
     * \brief   Function is triggered from thread object when it is going to be destroyed.
     **/
    virtual void onThreadUnregistering( void );

    /**
     * \brief   Function is called from Thread object, when it is running and fully operable.
     **/
    virtual void onThreadRuns( void );

    /**
     * \brief   Function is called from Thread object when it is going to exit.
     * \return  Return thread exit error code.
     **/
    virtual int onThreadExit( void );

/************************************************************************/
// IEEventRouter interface overrides
/************************************************************************/

    /**
     * \brief	Posts event and delivers to its target thread / process.
     * \param	eventElem	Event object to post.
     * \return	Returns true if target was found and the event
     *          delivered with success. Otherwise it returns false.
     **/
    virtual bool postEvent(Event& eventElem);

/************************************************************************/
// IEEventDispatcher Interface overrides.
/************************************************************************/

    /**
     * \brief   Call to register specified event consumer for the specified
     *          event class type. One event class type can have several
     *          registered consumers and one consumer can be registered for
     *          several class types.
     * \param	whichClass	    The runtime class ID object of an event 
     *                          to register consumer.
     * \param	whichConsumer	Reference to event consumer object to register
     *                          as an event listener. The processEvent() function
     *                          of consumer will be automatically triggered by
     *                          event dispatcher every time when appropriate
     *                          event class type has been picked from event queue.
     * \return	Return true if consumer has been registered with success.
     *          If specified consumer is already registered for specified
     *          event class type, it returns false.
     **/
    virtual bool registerEventConsumer( const RuntimeClassID & whichClass, IEEventConsumer & whichConsumer );

    /**
     * \brief	Call to unregister specified event consumer previously registered
     *          for specified event class type.
     * \param	whichClass	    The runtime class ID of event object.
     * \param	whichConsumer	Reference to consumer that should be unregistered.
     * \return	Returns true if successfully unregistered event consumer.
     **/
    virtual bool unregisterEventConsumer( const RuntimeClassID & whichClass, IEEventConsumer & whichConsumer );
    /**
     * \brief	Call to remove specified consumer for all registered event class types,
     *          previously registered in dispatcher.
     * \param	whichConsumer	Reference to consumer object to unregister.
     * \return	Returns unregister count. If zero, consumer is not registered for any event.
     **/
    virtual int  removeConsumer( IEEventConsumer & whichConsumer );
    /**
     * \brief	Call to check whether specified event class type has any registered consumer.
     * \param	whichClass	Runtime class ID to be checked.
     * \return	Returns true if dispatcher has at least one registered consumer for
     *          specified runtime class ID.
     **/
    virtual bool hasRegisteredConsumer( const RuntimeClassID & whichClass ) const;

/************************************************************************/
// EventDispatcherBase overrides.
/************************************************************************/

    /**
     * \brief   Returns true if dispatcher is ready to receive events.
     *          Otherwise return false.
     *          Override method if logic should be changed.
     **/
    virtual bool isReady( void ) const;

    /**
     * \brief   Removes all internal events, removes all external events,
     *          except exit event, and if keep special flag is set to true.
     *          it will keep special response events like connect, disconnect and 
     *          implemented service interface version number.
     * \param   keepSpecials    If true, it will keep special reserved events
     *                          like version, connect / disconnect and exit events.
     *                          If false, removes all events, except exit event.
     **/
    virtual void removeEvents( bool keepSpecials );

    /**
     * \brief   Removes all events. Makes event queue empty
     **/
    virtual void removeAllEvents( void );

    /**
     * \brief   Removes specified event type from external event queue
     *          and returns the amount of removed events.
     *          If after removing events the queue is empty, the event
     *          listener object will be notified.
     * \param   eventClassId    The class ID of external event object.
     *                          All events having specified class ID
     *                          will be removed.
     * \return  Returns amount of removed events.
     **/
    virtual int removeExternalEventType(const RuntimeClassID & eventClassId);

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Return pointer to Dispatcher Thread where current dispatcher
     *          is registered.
     **/
    inline DispatcherThread * getDispatcherThread( void ) const;

protected:
    /**
     * \brief   Returns true, if dispatcher has more queued events.
     *          Here, only external events are counted, since
     *          internal events are proceed immediately after external event.
     **/
    inline bool hasMoreEvents( void ) const;

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Pointer to dispatcher thread, which is holding dispatcher.
     *          The pointer is set after thread has been created and reset
     *          when it is destroyed.
     **/
    DispatcherThread *    mDispatcherThread;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls.
//////////////////////////////////////////////////////////////////////////
private:
    EventDispatcher( void );
    EventDispatcher( const EventDispatcher & /*src*/ );
    const EventDispatcher & operator = ( const EventDispatcher & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// DispatcherThread class inline functions implementation
//////////////////////////////////////////////////////////////////////////
inline DispatcherThread * EventDispatcher::getDispatcherThread( void ) const
{
    return mDispatcherThread;
}

inline bool EventDispatcher::hasMoreEvents( void ) const
{
    return (static_cast<const EventQueue &>(mExternaEvents).getSize() > 0);
}

#endif  // AREG_COMPONENT_EVENTDISPATCHER_HPP
