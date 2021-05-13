#ifndef AREG_COMPONENT_PRIVATE_EVENTQUEUE_HPP
#define AREG_COMPONENT_PRIVATE_EVENTQUEUE_HPP
/************************************************************************
 * \file        areg/component/private/EventQueue.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Event queue class decration
 *
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/TEStack.hpp"
#include "areg/component/private/IEQueueListener.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class Event;
class RuntimeClassID;

//////////////////////////////////////////////////////////////////////////
// EventQueue class declaration
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * \brief   The Event Queue is a FIFO Stack, containing list of Events.
 *          Every Dispatching Thread has an Event Queue object and
 *          the Event objects before processing, they are placed in
 *          this queue. There are external and internal event queues in
 *          every dispatcher thread. 
 *          1.  External event queue is used in every dispatcher thread.
 *              It contains synchronization object and it is thread safe.
 *              All events received from components and other threads are 
 *              placed in external event queue. The dispatcher is starting
 *              to dispatch events only when external event contain at least
 *              one event object.
 *
 *          2.  The internal event queue is used to store events, which are
 *              dispatched within same thread context. The events in internal
 *              event queue are normally placed by Proxies when they need to
 *              send notification messages to every client object within
 *              same thread. The internal event queue is empty, when external
 *              event queue is empty. This means that every dispatcher should
 *              first dispatch external event queue, then check whether internal
 *              event queue contains any element, then start to dispatch events
 *              in internal event queue.
 *
 *          Every Event object, which is pushed in the Queue, must be
 *          allocated globally (for example, via new operator). When
 *          Event is popped from queue and dispatched, it is manually deleted.
 *          If it is needed, certain event types might be removed from event queue
 *          by specifying runtime class ID of event type. All other events
 *          will remain untouched.
 *          When event queue is changing size, it is signaling event queue
 *          listener object by passing number of un-dispatched events in
 *          the queue. When event queue is empty, it is notifying listener
 *          event queue size is zero.
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// EventQueue class declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Event queue class is a base class for external and 
 *          internal event queue classes. It contains basic methods to
 *          push and pop event objects from Stack.
 **/
class AREG_API EventQueue
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initialization constructor.
     *          Event Listener object, which is signaled when Queue is receiving 
     *          new Event object or when Queue is empty.
     *          The caller should define queue stack object, which will hold event elements
     * \param   eventListener   The Event Listener object, which should
     *                          be signaled when receive new Event or when
     *                          the Queue is empty.
     * \param   eventQueue      The instance of event queue object, which will keep event elements.
     **/
    EventQueue( IEQueueListener & eventListener, TEStack<Event *, Event *> & eventQueue );

    /**
     * \brief   Destructor
     **/
    virtual ~EventQueue( void );

//////////////////////////////////////////////////////////////////////////
// OPerations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Locks Queue, prevent accessing from other threads.
     *          If Queue was locked by other thread, the current thread will
     *          wait, until it is not unlocked.
     * \return  Returns true, if Event Queue is locked with success.
     **/
    inline bool lockQueue( void );

    /**
     * \brief   Unlocks previously locked Event Queue, letting other
     *          threads having data access
     **/
    inline void unlockQueue( void );

    /**
     * \brief   Returns true, if Event Queue is empty.
     **/
    inline bool isEmpty( void ) const;

    /**
     * \brief   Returns number of pending Events in the Queue.
     **/
    inline int getSize( void ) const;

    /**
     * \brief   Pushes new Event in the Queue and notifies Event Listener
     *          about new Event element availability.
     **/
    void pushEvent( Event & evendElem );

    /**
     * \brief   Pops Event object from Queue and notifies Event Listener if
     *          there is no more Event element in the Queue left.
     * \return  Returns Event object pending in FIFO Stack.
     *          If Queue was not empty, it will return valid pointer.
     *          If Queue was empty, it will return NULL.
     **/
    Event * popEvent( void );

    /**
     * \brief   Removes all Event elements from the Queue and if keepSpecials is true,
     *          it will not remove special predefined Exit Event (ExitEvent) objects,
     *          which notify Dispatcher to complete job and exit.
     *          After Events are removed, it will signal Event Listener whether queue is 
     *          empty or not.
     *          When Event objects are removed from the Queue, Destroy() method of every
     *          Event is called to make cleanup
     * \param   keepSpecials    If true, it will remove all Event objects from the Queue,
     *                          except predefined special Exit Event (ExitEvent).
     *                          Otherwise it will remove all elements.
     **/
    void removeEvents( bool keepSpecials );

    /**
     * \brief   Removes the specified Runtime Event objects from the Queue and returns
     *          the number of removed Events. For every Event the method Destroy() will
     *          be called to make cleanup
     *          After Events are removed, it will signal Event Listener whether queue is 
     *          empty or not.
     * \param   eventClassId    Runtime class ID of Event object to remove from the Queue.
     * \return  Returns number of removed Events from the Queue.
     **/
    int removeEvents( const RuntimeClassID & eventClassId );

    /**
     * \brief   Removes all events. Makes event queue empty
     **/
    virtual void removeAllEvents( void );

//////////////////////////////////////////////////////////////////////////
// Protected methods
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Removes all Event elements from the Queue and if keepSpecials is true,
     *          it will not remove special predefined Exit Event (ExitEvent) objects,
     *          which notify Dispatcher to complete job and exit.
     *          When Event objects are removed from the Queue, Destroy() method of every
     *          Event is called to make cleanup.
     *          The method does not send any signal to Event Listener object.
     * \param   keepSpecials    If true, it will remove all Event objects from the Queue,
     *                          except predefined special Exit Event (ExitEvent).
     *                          Otherwise it will remove all elements.
     **/
    bool removePendingEvents( bool keepSpecials );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Queue Listener object, which is signaled every time 
     *          new Event is pushed or removed.
     **/
    IEQueueListener &           mEventListener;
    /**
     * \brief   Event queue stack object, which stores event elements
     **/
    TEStack<Event *, Event *> & mEventQueue;

//////////////////////////////////////////////////////////////////////////
// Forbidden method calls.
//////////////////////////////////////////////////////////////////////////
private:
    EventQueue( void );
    EventQueue(const EventQueue &);
    const EventQueue& operator = (const EventQueue &);
};

//////////////////////////////////////////////////////////////////////////
// ExternalEventQueue class declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   External event queue class declaration, which is accessed from many threads.
 *          Used to queue external types of event. 
 **/
class AREG_API ExternalEventQueue   : public    EventQueue
                                    , private   TELockStack<Event *, Event *>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initialization constructor.
     *          Event Listener object, which is signaled when Queue is receiving 
     *          new Event object or when Queue is empty.
     * \param   eventListener   The Event Listener object, which should
     *                          be signaled when receive new Event or when
     *                          the Queue is empty.
     **/
    ExternalEventQueue( IEQueueListener & eventListener );

    /**
     * \brief   Destructor
     **/
    virtual ~ExternalEventQueue( void );

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns instance of ExternalEventQueue object
     **/
    inline ExternalEventQueue & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden method calls.
//////////////////////////////////////////////////////////////////////////
private:
    ExternalEventQueue( void );
    ExternalEventQueue( const ExternalEventQueue & );
    const ExternalEventQueue & operator = ( const ExternalEventQueue & );
};

//////////////////////////////////////////////////////////////////////////
// InternalEventQueue class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Internal event queue class declaration, which is accessed only from one thread.
 *          Used to queue external types of event. 
 **/
class AREG_API InternalEventQueue   : public    EventQueue
                                    , private   TENolockStack<Event *, Event *>
                                    , private   IEQueueListener
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor.
     **/
    InternalEventQueue( void );

    /**
     * \brief   Destructor
     **/
    virtual ~InternalEventQueue( void );

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************/
// IEQueueListener interface overrides
/************************************************************************/
    /**
     * \brief	Triggered from Event Queue object every time when new event
     *          element is pushed into queue or when queue is empty.
     *          Override method to provide queuing logic.
     * \param	eventCount	The number of event elements currently in the queue.
     *                      If zero, queue is empty, dispatcher can be suspended.
     * \return  
     **/
    virtual void signalEvent( int eventCount );

private:
    /**
     * \brief   Returns instance of InternalEventQueue object
     **/
    inline InternalEventQueue & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden method calls.
//////////////////////////////////////////////////////////////////////////
private:
    InternalEventQueue( const InternalEventQueue & );
    const InternalEventQueue & operator = ( const InternalEventQueue & );
};

//////////////////////////////////////////////////////////////////////////
// EventQueue class inline functions implementation
//////////////////////////////////////////////////////////////////////////
inline bool EventQueue::lockQueue( void )
{
    return mEventQueue.lock();
}

inline void EventQueue::unlockQueue( void )
{
    mEventQueue.unlock();
}

inline bool EventQueue::isEmpty( void ) const
{
    return mEventQueue.isEmpty();
}

inline int EventQueue::getSize( void ) const
{
    return mEventQueue.getSize();
}

//////////////////////////////////////////////////////////////////////////
// ExternalEventQueue class inline functions implementation
//////////////////////////////////////////////////////////////////////////
inline ExternalEventQueue & ExternalEventQueue::self( void )
{
    return (*this);
}

//////////////////////////////////////////////////////////////////////////
// InternalEventQueue class inline functions implementation
//////////////////////////////////////////////////////////////////////////
inline InternalEventQueue & InternalEventQueue::self( void )
{
    return (*this);
}

#endif  // AREG_COMPONENT_PRIVATE_EVENTQUEUE_HPP
