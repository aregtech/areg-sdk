#ifndef AREG_COMPONENT_PRIVATE_IEEVENTDISPATCHER_HPP
#define AREG_COMPONENT_PRIVATE_IEEVENTDISPATCHER_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/component/private/IEEventDispatcher.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Event Dispatcher interface.
 *              The Event Dispatcher should implement these methods to
 *              start dispatching events.
 *
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

/************************************************************************
 * Dependencies
 ************************************************************************/
class Event;
class RuntimeClassID;
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
    IEEventDispatcher( void ) = default;

    /**
     * \brief   Destructor
     **/
    virtual ~IEEventDispatcher( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Override operations.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Call to start dispatcher. Returns true if successfully started.
     **/
    virtual bool startDispatcher( void ) = 0;

    /**
     * \brief   Call to stop running dispatcher.
     **/
    virtual void stopDispatcher( void ) = 0;

    /**
     * \brief   Called when dispatcher completed the job and exit.
     *          The cleanups should be done here.
     **/
    virtual void exitDispatcher( void ) = 0;

    /**
     * \brief   Call to queue event object in the event queue of dispatcher.
     *          The passed event parameter should be allocated in memory and
     *          should be globally accessed (for example, via new operator).
     * \param   eventElem   Event object to queue in event stack of dispatcher.
     * \return  Returns true, if Event was queued. Otherwise, it was not queued
     *          and the object should be deleted.
     **/
    virtual bool queueEvent( Event & eventElem ) = 0;

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
    virtual bool registerEventConsumer( const RuntimeClassID & whichClass, IEEventConsumer & whichConsumer ) = 0;

    /**
     * \brief	Call to unregister specified event consumer previously registered
     *          for specified event class type.
     * \param	whichClass	    The runtime class ID of event object.
     * \param	whichConsumer	Reference to consumer that should be unregistered.
     * \return	Returns true if successfully unregistered event consumer.
     **/
    virtual bool unregisterEventConsumer( const RuntimeClassID & whichClass, IEEventConsumer & whichConsumer ) = 0;

    /**
     * \brief	Call to remove specified consumer for all registered event class types,
     *          previously registered in dispatcher.
     * \param	whichConsumer	Reference to consumer object to unregister.
     * \return	Returns unregister count. If zero, consumer is not registered for any event.
     **/
    virtual int  removeConsumer( IEEventConsumer & whichConsumer ) = 0;

    /**
     * \brief	Call to check whether specified event class type has any registered consumer.
     * \param	whichClass	Runtime class ID to be checked.
     * \return	Returns true if dispatcher has at least one registered consumer for
     *          specified runtime class ID.
     **/
    virtual bool hasRegisteredConsumer( const RuntimeClassID & whichClass ) const = 0;

protected:
    /**
     * \brief	The method is triggered to start dispatching valid event.
     *          Here dispatcher should forward message to appropriate 
     *          registered event consumer
     * \param	eventElem   Event element to dispatch	
     * \return	Returns true if at least one consumer processed event.
     *          Otherwise it returns false.
     **/
    virtual bool dispatchEvent( Event & eventElem ) = 0;

    /**
     * \brief   The method is triggered after picking up event from event queue.
     *          Before starting dispatching, this function is called and if it
     *          returns false, the event will not be dispatched.
     * \param   eventElem   Pointer to the event element to be dispatched.
     * \return  Return true if event should be forwarded for dispatching.
     *          Return false if event should be ignored / dropped.
     **/
    virtual bool prepareDispatchEvent( Event * eventElem ) = 0;

    /**
     * \brief	All events after being processed are forwarded
     *          to this method. All cleanup operations should be provided
     *          in this method.
     * \param	eventElem	Pointer to Event element, which has been finished
     *                      to be dispatched.
     **/
    virtual void postDispatchEvent( Event * eventElem ) = 0;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls.
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( IEEventDispatcher );
};

#endif  // AREG_COMPONENT_PRIVATE_IEEVENTDISPATCHER_HPP
