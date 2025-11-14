#ifndef AREG_COMPONENT_PRIVATE_EVENTDISPATCHERBASE_HPP
#define AREG_COMPONENT_PRIVATE_EVENTDISPATCHERBASE_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/component/private/EventDispatcherBase.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Global AREG Platform, Event Dispatcher base class.
 *              The class is providing basic dispatching functionalities,
 *              keeps track of queued events and registered event consumers, 
 *              starts and stops event dispatcher, triggers Event Processing.
 *              The Event Dispatcher is Thread blocker, it runs and pick up
 *              event objects from queue in the loop, until do not get request 
 *              to stop dispatcher. To stop Dispatcher and release Thread,
 *              call stopDispatcher() function or it will be automatically
 *              triggered in Dispatcher Thread when request to stop thread
 *              is received.
 *
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/private/IEQueueListener.hpp"
#include "areg/component/private/IEEventDispatcher.hpp"

#include "areg/component/private/EventConsumerMap.hpp"
#include "areg/component/private/EventQueue.hpp"
#include "areg/base/String.hpp"
#include "areg/base/SynchObjects.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class DispatcherThread;

//////////////////////////////////////////////////////////////////////////
// EventDispatcherBase class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This class is providing event dispatching basic functionalities.
 *          It is an implementation of Event Queue Listener and
 *          Event Dispatcher interfaces. Class tracks Event queue to be able
 *          to pickup events from queue and finish executing when receives 
 *          Quit request. Every Dispatcher thread should contain one Event
 *          Dispatcher running in Loop. Class contains map or registered
 *          Event Consumers to trigger Event Processing.
 *          
 **/
class AREG_API EventDispatcherBase  : public    IEEventDispatcher
                                    , protected IEQueueListener
{
//////////////////////////////////////////////////////////////////////////
// Internal defines and constants.
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   EventDispatcherBase::eEventOrder
     *          Used in main loop to identify event signal.
     **/
    typedef enum class E_EventOrder : int8_t
    {
          EventError    = -1    //!< Error happened during waiting for event
        , EventExit     =  0    //!< Exit event has been signaled.
        , EventQueue    =  1    //!< Queue event has been signaled.
    } eEventOrder;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected.
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initialization constructor. Initialized Dispatcher and assigns name.
     * \param   name        The name of Dispatcher.
     * \param   maxQeueue   The maximum number of event elements in the queue.
     **/
    EventDispatcherBase( const String & name, uint32_t maxQeueue );
    /**
     * \brief   Destructor
     **/
    virtual ~EventDispatcherBase( void );

//////////////////////////////////////////////////////////////////////////
// Public overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// EventDispatcherBase overrides
/************************************************************************/

    /**
     * \brief   Returns true if dispatcher is ready to receive events.
     *          Otherwise return false.
     *          Override method if logic should be changed.
     **/
    inline bool isReady( void ) const;

    /**
     * \brief   Removes all internal events, removes all external events,
     *          except exit event, and if keep special flag is set to true.
     *          it will keep special response events like connect, disconnect and 
     *          implemented service interface version number.
     * \param   keepSpecials    If true, it will keep special reserved events
     *                          like version, connect / disconnect and exit events.
     *                          If false, removes all events, except exit event.
     **/
    inline void removeEvents( bool keepSpecials );

    /**
     * \brief   Removes all events. Makes event queue empty
     **/
    inline void removeAllEvents( void );

    /**
     * \brief   Removes specified event type from external event queue
     *          and returns the amount of removed events.
     *          If after removing events the queue is empty, the event
     *          listener object will be notified.
     * \param   eventClassId    The class ID of external event object.
     *                          All events having specified class ID
     *                          will be removed.
     **/
    inline void removeExternalEventType(const RuntimeClassID & eventClassId);

    /**
     * \brief   Returns true if the specified event object is a special reserved event indicating to exit the thread.
     * \param   anEvent     A pointer to the event object to check.
     * \return  Returns true, if dispatcher should complete the job and exit the thread.
     **/
    bool isExitEvent( const Event * anEvent ) const;

/************************************************************************/
// IEEventDispatcher overrides
/************************************************************************/

    /**
     * \brief   Call to start dispatcher. Returns true if successfully started.
     **/
    virtual bool startDispatcher( void ) override;

    /**
     * \brief   Call to stop running dispatcher.
     **/
    virtual void stopDispatcher( void ) override;

    /**
     * \brief   Called when dispatcher completed the job and exit.
     *          The cleanups should be done here.
     **/
    virtual void exitDispatcher(void) override;

    /**
     * \brief   Call to queue event object in the event queue of dispatcher.
     *          The passed event parameter should be allocated in memory and
     *          should be globally accessed (for example, via new operator).
     * \param   eventElem           Event object to queue in event stack of dispatcher.
     * \return  Returns true, if Event was queued. Otherwise, it was not queued
     *          and the object should be deleted.
     **/
    virtual bool queueEvent( Event & eventElem ) override;
    
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
    virtual bool registerEventConsumer( const RuntimeClassID & whichClass, IEEventConsumer & whichConsumer ) override;

    /**
     * \brief	Call to unregister specified event consumer previously registered
     *          for specified event class type.
     * \param	whichClass	    The runtime class ID of event object.
     * \param	whichConsumer	Reference to consumer that should be unregistered.
     * \return	Returns true if successfully unregistered event consumer.
     **/
    virtual bool unregisterEventConsumer( const RuntimeClassID & whichClass, IEEventConsumer & whichConsumer ) override;

    /**
     * \brief	Call to remove specified consumer for all registered event class types,
     *          previously registered in dispatcher.
     * \param	whichConsumer	Reference to consumer object to unregister.
     * \return	Returns unregister count. If zero, consumer is not registered for any event.
     **/
    virtual int  removeConsumer( IEEventConsumer & whichConsumer ) override;

    /**
     * \brief	Call to check whether specified event class type has any registered consumer.
     * \param	whichClass	Runtime class ID to be checked.
     * \return	Returns true if dispatcher has at least one registered consumer for
     *          specified runtime class ID.
     **/
    virtual bool hasRegisteredConsumer( const RuntimeClassID & whichClass ) const override;

//////////////////////////////////////////////////////////////////////////
// Protected overrides
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// IEQueueListener overrides
/************************************************************************/

    /**
     * \brief	Triggered from Event Queue object every time when new event
     *          element is pushed into queue or when queue is empty.
     *          It notifies dispatcher about update in event queue.
     * \param	eventCount	The number of event elements currently in the queue.
     *                      If zero, queue is empty, dispatcher can be suspended.
     **/
    virtual void signalEvent(uint32_t eventCount) override;

/************************************************************************/
// IEEventDispatcher overrides
/************************************************************************/

    /**
     * \brief	The method is triggered to start dispatching valid event.
     *          Here dispatcher should forward message to appropriate 
     *          registered event consumer
     * \param	eventElem   Event element to dispatch	
     * \return	Returns true if at least one consumer processed event.
     *          Otherwise it returns false.
     **/
    virtual bool dispatchEvent( Event & eventElem ) override;
    /**
     * \brief   The method is triggered after picking up event from event queue.
     *          Before starting dispatching, this function is called and if it
     *          returns false, the event will not be dispatched.
     * \param   eventElem   Pointer to the event element to be dispatched.
     * \return  Return true if event should be forwarded for dispatching.
     *          Return false if event should be ignored / dropped.
     **/
    virtual bool prepareDispatchEvent( Event * eventElem ) override;
    /**
     * \brief	All events after being processed are forwarded
     *          to this method. All cleanup operations should be provided
     *          in this method.
     * \param	eventElem	Pointer to Event element, which has been finished
     *                      to be dispatched.
     **/
    virtual void postDispatchEvent( Event * eventElem ) override;

/************************************************************************/
// EventDispatcherBase overrides
/************************************************************************/

    /**
     * \brief	Triggered when dispatcher starts running. 
     *          In this function runs main dispatching loop.
     *          Events are picked and dispatched here.
     *          Override if logic should be changed.
     * \return	Returns true if Exit Event is signaled.
     **/
    virtual bool runDispatcher( void );

    /**
     * \brief   Notifies exit event to shutdown dispatcher.
     *          No element will be removed.
     **/
    virtual void shutdownDispatcher( void );

    /**
     * \brief	Picks up single Event element from the event queue
     *          and forwards to be dispatched.
     * \return	Return pointer to event element to be dispatched.
     **/
    virtual Event * pickEvent( void );

    /**
     * \brief   Call if need to set exit event in the dispatcher
     *          but without blocking anything. This might be
     *          if need to complete own thread. In this case the
     *          exit event will be set and as soon as the loop
     *          is completed, the thread will complete job and exit.
     * \return  Returns true if could fire event.
     **/
    virtual bool pulseExit( void );

    /**
     * \brief   Call to enable or disable event dispatching threads to receive events.
     *          Override if need to make event dispatching preparation job.
     * \param   isReady     The flag to indicate whether the dispatcher is ready for events.
     **/
    virtual void readyForEvents( bool isReady );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   The name of dispatcher
     **/
    String              mDispatcherName;

    /**
     * \brief   External Event Queue element. One External queue per one dispatcher.
     *          It is locking queue. Any thread can queue elements
     **/
    ExternalEventQueue  mExternaEvents;

    /**
     * \brief   Internal Event Queue element. One Internal queue per one dispatcher.
     *          It is non-locking queue, only current owning thread can queue elements.
     **/
    InternalEventQueue  mInternalEvents;

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

    /**
     * \brief   Map of registered consumers.
     **/
    EventConsumerMap    mConsumerMap;

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(default: 4251)
#endif  // _MSC_VER

    /**
     * \brief   Exit Synchronization Event. 
     *          Signaled, when dispatcher should be stopped and exit from loop.
     **/
    SynchEvent          mEventExit;
    /**
     * \brief   Queue Synchronization Event.
     *          Signaled when new event is pushed into the queue and 
     *          reset (not signaled) when queue is empty.
     **/
    SynchEvent          mEventQueue;

    /**
     * \brief   Flag, which is indicating whether dispatcher is started or not.
     *          Dispatcher is started, when it is in dispatching loop.
     **/
    bool                mHasStarted;

//////////////////////////////////////////////////////////////////////////
// Hidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns reference to EventDispatcherBase object
     **/
    inline EventDispatcherBase & self( void );
    /**
     * \brief   Called when needs to make cleanup after Dispatcher completed job.
     *          This will remove Event Consumers.
     **/
    void _clean();

//////////////////////////////////////////////////////////////////////////
// Forbidden method calls
//////////////////////////////////////////////////////////////////////////
private:
    EventDispatcherBase( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( EventDispatcherBase );
};

//////////////////////////////////////////////////////////////////////////
// EventDispatcherBase class inline implementation
//////////////////////////////////////////////////////////////////////////

inline bool EventDispatcherBase::isReady( void ) const
{
    return mHasStarted;
}

inline void EventDispatcherBase::removeEvents(bool keepSpecials)
{
    mExternaEvents.lockQueue();
    mInternalEvents.removeEvents( false );
    mExternaEvents.removeEvents( keepSpecials );
    mExternaEvents.unlockQueue();
}

inline void EventDispatcherBase::removeAllEvents(void)
{
    mExternaEvents.lockQueue();
    mInternalEvents.removeAllEvents( );
    mExternaEvents.removeAllEvents( );
    mExternaEvents.unlockQueue();
}

inline void EventDispatcherBase::removeExternalEventType( const RuntimeClassID & eventClassId )
{
    mExternaEvents.removeEvents(eventClassId);
}

inline EventDispatcherBase& EventDispatcherBase::self( void )
{
    return (*this);
}

#endif  // AREG_COMPONENT_PRIVATE_EVENTDISPATCHERBASE_HPP
