#ifndef AREG_COMPONENT_CEEVENTDISPATCHER_HPP
#define AREG_COMPONENT_CEEVENTDISPATCHER_HPP
/************************************************************************
 * \file        areg/src/component/CEEventDispatcher.hpp
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
#include "areg/src/base/GEGlobal.h"
#include "areg/src/base/IEThreadConsumer.hpp"
#include "areg/src/component/IEEventRouter.hpp"
#include "areg/src/component/private/CEEventDispatcherBase.hpp"

//////////////////////////////////////////////////////////////////////////
// CEEventDispatcher class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       Event dispatcher class. It derives from thread consumer 
 *              plays role of event poster and  protects some methods 
 *              of dispatcher base class.
 *
 * \details     Every Dispatcher Thread has a dispatcher. All events
 *              are queued in dispatcher, which are picked up and
 *              processed one by one. The dispatcher is responsible
 *              to push appropriate event in the event queue and
 *              trigger appropriate event processing function.
 *              The dispatcher will run in loop as long until did not
 *              get Exit event notification, which will cause event queue
 *              cleanup and stop dispatcher running.
 *              Do not make long processing and run function in the long
 *              loop, because this will delay processing of queued events
 *              and/or will delay dispatcher stop. The Dispatching Thread
 *              will remain active as long, until the dispatcher is running.
 **/
class AREG_API CEEventDispatcher    : public    IEThreadConsumer
                                    , public    IEEventRouter
                                    , protected CEEventDispatcherBase
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initialization constructor.
     * \param   name    The name of Event Dispatcher
     **/
    CEEventDispatcher( const char* name );
    /**
     * \brief   Destructor.
     **/
    virtual ~CEEventDispatcher( void );

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
     *          than function Run() is called.
     * \param	threadObj	The new created Thread object, 
     *                      which contains this consumer.
     * \return	Return true if thread should run. Return false, it it should not run.
     **/
    virtual bool ThreadRegistered( CEThread * threadObj );

    /**
     * \brief   Function is triggered from thread object when it is going to be destroyed.
     **/
    virtual void ThreadUnregistering( void );

    /**
     * \brief   Function is called from Thread object, when it is running and fully operable.
     **/
    virtual void ThreadRuns( void );

    /**
     * \brief   Function is called from Thread object when it is going to exit.
     * \return  Return thread exit error code.
     **/
    virtual int ThreadExit( void );

/************************************************************************/
// IEEventRouter interface overrides
/************************************************************************/

    /**
     * \brief	Posts event and delivers to its target thread / process.
     * \param	eventElem	Event object to post.
     * \return	Returns true if target was found and the event
     *          delivered with success. Otherwise it returns false.
     **/
    virtual bool PostEvent(CEEvent& eventElem);

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

/************************************************************************/
// CEEventDispatcherBase overrides.
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

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Return pointer to Dispatcher Thread where current dispatcher
     *          is registered.
     **/
    inline CEDispatcherThread * GetDispatcherThread( void ) const;

protected:
    /**
     * \brief   Returns true, if dispatcher has more queued events.
     *          Here, only external events are counted, since
     *          internal events are proceed immediately after external event.
     **/
    inline bool HasMoreEvents( void ) const;

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Pointer to dispatcher thread, which is holding dispatcher.
     *          The pointer is set after thread has been created and reset
     *          when it is destroyed.
     **/
    CEDispatcherThread *    mDispatcherThread;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls.
//////////////////////////////////////////////////////////////////////////
private:
    CEEventDispatcher( void );
    CEEventDispatcher( const CEEventDispatcher & /*src*/ );
    const CEEventDispatcher & operator = ( const CEEventDispatcher & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// CEDispatcherThread class inline functions implementation
//////////////////////////////////////////////////////////////////////////
inline CEDispatcherThread * CEEventDispatcher::GetDispatcherThread( void ) const
{   return mDispatcherThread;                                               }

inline bool CEEventDispatcher::HasMoreEvents( void ) const
{   return static_cast<const CEEventQueue &>(mExternaEvents).GetSize() > 0; }

#endif  // AREG_COMPONENT_CEEVENTDISPATCHER_HPP
