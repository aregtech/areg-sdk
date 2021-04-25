#ifndef AREG_COMPONENT_PRIVATE_IEEVENTDISPATCHER_HPP
#define AREG_COMPONENT_PRIVATE_IEEVENTDISPATCHER_HPP

/************************************************************************
 * \file        areg/src/component/private/IEEventDispatcher.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Event Dispatcher interface.
 *              The Event Dispatcher should implement these methods to
 *              start dispatching events.
 *
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"

/************************************************************************
 * Dependencies
 ************************************************************************/
class CEEvent;
class CERuntimeClassID;
class IEEventConsumer;

//////////////////////////////////////////////////////////////////////////
// IEEventDispatcher class declarations
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Event Dispatcher pure virtual class interface declaration.
 *          The interface provides possibility to start and stop dispatcher,
 *          to run dispatcher and dispatch events. To be able to
 *          dispatch events, the classes should implement pure virtual
 *          methods. The interface is declared not for global usage.
 *          It is declared to implement different dispatching
 *          mechanisms, which might differ in different threads.
 *          The dispatcher interface mainly used in Dispatcher Threads.
 **/
class AREG_API IEEventDispatcher
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Default constructor
     *          Protected, cannot be instantiated.
     **/
    IEEventDispatcher( void );

    /**
     * \brief   Destructor
     **/
    virtual ~IEEventDispatcher( void );

//////////////////////////////////////////////////////////////////////////
// Override operations.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Call to start dispatcher. Returns true if successfully started.
     **/
    virtual bool StartDispatcher( void ) = 0;

    /**
     * \brief   Call to stop running dispatcher.
     **/
    virtual void StopDispatcher( void ) = 0;

    /**
     * \brief   Call to queue event object in the event queue of dispatcher.
     *          The passed event parameter should be allocated in memory and
     *          should be globally accessed (for example, via new operator).
     * \param   eventElem   Event object to queue in event stack of dispatcher.
     * \return  Returns true, if Event was queued. Otherwise, it was not queued
     *          and the object should be deleted.
     **/
    virtual bool QueueEvent( CEEvent & eventElem ) = 0;

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
    virtual bool RegisterEventConsumer( const CERuntimeClassID & whichClass, IEEventConsumer & whichConsumer ) = 0;

    /**
     * \brief	Call to unregister specified event consumer previously registered
     *          for specified event class type.
     * \param	whichClass	    The runtime class ID of event object.
     * \param	whichConsumer	Reference to consumer that should be unregistered.
     * \return	Returns true if successfully unregistered event consumer.
     **/
    virtual bool UnregisterEventConsumer( const CERuntimeClassID & whichClass, IEEventConsumer & whichConsumer ) = 0;

    /**
     * \brief	Call to remove specified consumer for all registered event class types,
     *          previously registered in dispatcher.
     * \param	whichConsumer	Reference to consumer object to unregister.
     * \return	Returns unregister count. If zero, consumer is not registered for any event.
     **/
    virtual int  RemoveConsumer( IEEventConsumer & whichConsumer ) = 0;

    /**
     * \brief	Call to check whether specified event class type has any registered consumer.
     * \param	whichClass	Runtime class ID to be checked.
     * \return	Returns true if dispatcher has at least one registered consumer for
     *          specified runtime class ID.
     **/
    virtual bool HasRegisteredConsumer( const CERuntimeClassID & whichClass ) const = 0;

protected:
    /**
     * \brief	The method is triggered to start dispatching valid event.
     *          Here dispatcher should forward message to appropriate 
     *          registered event consumer
     * \param	eventElem   Event element to dispatch	
     * \return	Returns true if at least one consumer processed event.
     *          Otherwise it returns false.
     **/
    virtual bool DispatchEvent( CEEvent & eventElem ) = 0;

    /**
     * \brief   The method is triggered after picking up event from event queue.
     *          Before starting dispatching, this function is called and if it
     *          returns false, the event will not be dispatched.
     * \param   eventElem   Pointer to the event element to be dispatched.
     * \return  Return true if event should be forwarded for dispatching.
     *          Return false if event should be ignored / dropped.
     **/
    virtual bool PrepareDispatchEvent( CEEvent * eventElem ) = 0;

    /**
     * \brief	All events after being processed are forwarded
     *          to this method. All cleanup operations should be provided
     *          in this method.
     * \param	eventElem	Pointer to Event element, which has been finished
     *                      to be dispatched.
     * \return	
     **/
    virtual void PostDispatchEvent( CEEvent * eventElem ) = 0;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls.
//////////////////////////////////////////////////////////////////////////
private:
    IEEventDispatcher( const IEEventDispatcher & /*src*/ );
    const IEEventDispatcher & operator = ( const IEEventDispatcher & /*src*/ );
};

#endif  // AREG_COMPONENT_PRIVATE_IEEVENTDISPATCHER_HPP
