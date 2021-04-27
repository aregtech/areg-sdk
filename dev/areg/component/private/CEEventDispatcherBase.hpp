#ifndef AREG_COMPONENT_PRIVATE_CEEVENTDISPATCHERBASE_HPP
#define AREG_COMPONENT_PRIVATE_CEEVENTDISPATCHERBASE_HPP

/************************************************************************
 * \file        areg/component/private/CEEventDispatcherBase.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Global AREG Platform, Event Dispatcher base class.
 *              The class is providing basic dispatching functionalities,
 *              keeps track of queued events and registered event consumers, 
 *              starts and stops event dispatcher, triggers Event Processing.
 *              The Event Dispatcher is Thread blocker, it runs and pick up
 *              event objects from queue in the loop, until do not get request 
 *              to stop dispatcher. To stop Dispatcher and release Thread,
 *              call StopDispatcher() function or it will be automatically
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

#include "areg/component/private/CEEventConsumerMap.hpp"
#include "areg/component/private/CEEventQueue.hpp"
#include "areg/base/CEString.hpp"
#include "areg/base/ESynchObjects.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class CEDispatcherThread;

//////////////////////////////////////////////////////////////////////////
// CEEventDispatcherBase class declaration
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
class AREG_API CEEventDispatcherBase : public IEQueueListener
                                     , public IEEventDispatcher
{
//////////////////////////////////////////////////////////////////////////
// Internal defines and constants.
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   CEEventDispatcherBase::eEventOrder
     *          Used in main loop to identify event signal.
     **/
    typedef enum E_EventOrder
    {
          EVENT_ERROR   = -1    //!< Error happened during waiting for event
        , EVENT_EXIT    =  0    //!< Exit event has been signaled.
        , EVENT_QUEUE   =  1    //!< Queue event has been signaled.
    } eEventOrder;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected.
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initialization constructor. Initialized Dispatcher and assigns name.
     * \param   name            The name of Dispatcher.
     **/
    CEEventDispatcherBase( const char* name );
    /**
     * \brief   Destructor
     **/
    virtual ~CEEventDispatcherBase( void );

//////////////////////////////////////////////////////////////////////////
// Public overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// CEEventDispatcherBase overrides
/************************************************************************/

    /**
     * \brief   Returns true if dispatcher is ready to receive events.
     *          Otherwise return false.
     *          Override method if logic should be changed.
     **/
    virtual bool IsReady( void ) const;

    /**
     * \brief   Removes all internal events, removes all external events,
     *          except exit event, and if keep special flag is set to true.
     *          it will keep special response events like connect, disconnect and 
     *          implemented service interface version number.
     * \param   keepSpecials    If true, it will keep special reserved events
     *                          like version, connect / disconnect and exit events.
     *                          If false, removes all events, except exit event.
     **/
    virtual void RemoveEvents( bool keepSpecials );

    /**
     * \brief   Removes all events. Makes event queue empty
     **/
    virtual void RemoveAllEvents( void );

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
    virtual int RemoveExternalEventType(const CERuntimeClassID & eventClassId);

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
    virtual void SignalEvent(int eventCount);

/************************************************************************/
// IEEventDispatcher overrides
/************************************************************************/

    /**
     * \brief   Call to start dispatcher. Returns true if successfully started.
     **/
    virtual bool StartDispatcher( void );

    /**
     * \brief   Call to stop running dispatcher.
     **/
    virtual void StopDispatcher( void );
    
    /**
     * \brief   Notifies exit event to shutdown dispatcher.
     *          No element will be removed.
     **/
    virtual void ShutdownDispatcher( void );
    
    /**
     * \brief   Call to queue event object in the event queue of dispatcher.
     **/
    virtual bool QueueEvent( CEEvent & eventElem );
    
    /**
     * \brief   Call to register specified event consumer for the specified
     *          event class type. One event class type can have several
     *          registered consumers and one consumer can be registered for
     *          several class types.
     * \param	whichClass	    The runtime class ID object of an event 
     *                          to register consumer.
     * \param	whichConsumer	Reference to event consumer object to register
     *                          as an event listener. The ProcessEvent() function
     *                          of consumer will be automatically triggered by
     *                          event dispatcher every time when appropriate
     *                          event class type has been picked from event queue.
     * \return	Return true if consumer has been registered with success.
     *          If specified consumer is already registered for specified
     *          event class type, it returns false.
     **/
    virtual bool RegisterEventConsumer( const CERuntimeClassID & whichClass, IEEventConsumer & whichConsumer );

    /**
     * \brief	Call to unregister specified event consumer previously registered
     *          for specified event class type.
     * \param	whichClass	    The runtime class ID of event object.
     * \param	whichConsumer	Reference to consumer that should be unregistered.
     * \return	Returns true if successfully unregistered event consumer.
     **/
    virtual bool UnregisterEventConsumer( const CERuntimeClassID & whichClass, IEEventConsumer & whichConsumer );

    /**
     * \brief	Call to remove specified consumer for all registered event class types,
     *          previously registered in dispatcher.
     * \param	whichConsumer	Reference to consumer object to unregister.
     * \return	Returns unregister count. If zero, consumer is not registered for any event.
     **/
    virtual int  RemoveConsumer( IEEventConsumer & whichConsumer );

    /**
     * \brief	Call to check whether specified event class type has any registered consumer.
     * \param	whichClass	Runtime class ID to be checked.
     * \return	Returns true if dispatcher has at least one registered consumer for
     *          specified runtime class ID.
     **/
    virtual bool HasRegisteredConsumer( const CERuntimeClassID & whichClass ) const;

//////////////////////////////////////////////////////////////////////////
// Protected overrides
//////////////////////////////////////////////////////////////////////////
protected:
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
    virtual bool DispatchEvent( CEEvent & eventElem );
    /**
     * \brief   The method is triggered after picking up event from event queue.
     *          Before starting dispatching, this function is called and if it
     *          returns false, the event will not be dispatched.
     * \param   eventElem   Pointer to the event element to be dispatched.
     * \return  Return true if event should be forwarded for dispatching.
     *          Return false if event should be ignored / dropped.
     **/
    virtual bool PrepareDispatchEvent( CEEvent * eventElem );
    /**
     * \brief	All events after being processed are forwarded
     *          to this method. All cleanup operations should be provided
     *          in this method.
     * \param	eventElem	Pointer to Event element, which has been finished
     *                      to be dispatched.
     * \return	
     **/
    virtual void PostDispatchEvent( CEEvent * eventElem );

/************************************************************************/
// CEEventDispatcherBase overrides
/************************************************************************/

    /**
     * \brief	Triggered when dispatcher starts running. 
     *          In this function runs main dispatching loop.
     *          Events are picked and dispatched here.
     *          Override if logic should be changed.
     * \return	Returns true if Exit Event is signaled.
     **/
    virtual bool RunDispatcher( void );
    /**
     * \brief	Picks up single Event element from the event queue
     *          and forwards to be dispatched.
     * \return	Return pointer to event element to be dispatched.
     **/
    virtual CEEvent * PickEvent( void );

    /**
     * \brief   Call if need to set exit event in the dispatcher
     *          but without blocking anything. This might be
     *          if need to complete own thread. In this case the
     *          exit event will be set and as soon as the loop
     *          is completed, the thread will complete job and exit.
     * \return  Returns true if could fire event.
     **/
    virtual bool PulseExit( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   The name of dispatcher
     **/
    CEString                mDispatcherName;

    /**
     * \brief   External Event Queue element. One External queue per one dispatcher.
     *          It is locking queue. Any thread can queue elements
     **/
    CEExternalEventQueue    mExternaEvents;

    /**
     * \brief   Internal Event Queue element. One Internal queue per one dispatcher.
     *          It is non-locking queue, only current owning thread can queue elements.
     **/
    CEInternalEventQueue    mInternalEvents;

    /**
     * \brief   Map of registered consumers.
     **/
    CEEventConsumerMap      mConsumerMap;

    /**
     * \brief   Exit Synchronization Event. 
     *          Signaled, when dispatcher should be stopped and exit from loop.
     **/
    CESynchEvent            mEventExit;
    /**
     * \brief   Queue Synchronization Event.
     *          Signaled when new event is pushed into the queue and 
     *          reset (not signaled) when queue is empty.
     **/
    CESynchEvent            mEventQueue;

    /**
     * \brief   Flag, which is indicating whether dispatcher is started or not.
     *          Dispatcher is started, when it is in dispatching loop.
     **/
    bool                    mHasStarted;

//////////////////////////////////////////////////////////////////////////
// Hidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns reference to CEEventDispatcherBase object
     **/
    inline CEEventDispatcherBase & self( void );
    /**
     * \brief   Called when needs to make cleanup after Dispatcher completed job.
     *          This will remove Event Consumers.
     **/
    void _clean();

//////////////////////////////////////////////////////////////////////////
// Forbidden method calls
//////////////////////////////////////////////////////////////////////////
private:
    CEEventDispatcherBase( void );
    CEEventDispatcherBase(const CEEventDispatcherBase &);
    const CEEventDispatcherBase& operator = (const CEEventDispatcherBase &);
};

//////////////////////////////////////////////////////////////////////////
// CEEventDispatcherBase class inline implementation
//////////////////////////////////////////////////////////////////////////

inline CEEventDispatcherBase& CEEventDispatcherBase::self( void )
{   return (*this); }

#endif  // AREG_COMPONENT_PRIVATE_CEEVENTDISPATCHERBASE_HPP
