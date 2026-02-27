#ifndef AREG_COMPONENT_PRIVATE_EVENTQUEUE_HPP
#define AREG_COMPONENT_PRIVATE_EVENTQUEUE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/EventQueue.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Event queue class declaration
 *
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/private/SortedEventStack.hpp"
#include "areg/component/private/QueueListener.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class Event;
class RuntimeClassID;

//////////////////////////////////////////////////////////////////////////
// EventQueue class declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Base class for event queues; provides FIFO stack operations with listener notification
 *          on insert/remove.
 **/
class AREG_API EventQueue
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes the queue with a listener and message stack.
     *
     * \param   eventListener       Listener to signal on insert/remove operations.
     * \param   messageQueue        Stack container to hold event elements.
     **/
    EventQueue( QueueListener & eventListener, SortedEventStack & messageQueue );

    /**
     * \brief   Destructor
     **/
    virtual ~EventQueue() = default;

//////////////////////////////////////////////////////////////////////////
// OPerations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Locks the queue for exclusive access; blocks if already locked by another thread.
     *
     * \return  True if successfully locked.
     **/
    inline bool lock_queue();

    /**
     * \brief   Unlocks the queue, allowing other threads to access it.
     **/
    inline void unlock_queue();

    /**
     * \brief   Returns true if the queue is empty.
     **/
    inline bool is_empty() const;

    /**
     * \brief   Pushes an event into the queue and signals the listener; may evict an old event.
     *
     * \param   evendElem       Event to push.
     * \param[out] removedEvent    If an event was evicted and this is not nullptr, receives the
     *                             evicted event.
     **/
    void push_event( Event & evendElem, Event** removedEvent);

    /**
     * \brief   Pops an event from the queue and signals the listener if empty.
     *
     * \return  Valid event pointer from FIFO stack; nullptr if queue was empty.
     **/
    Event * pop_event();

    /**
     * \brief   Removes events from the queue and calls Destroy on each; optionally preserves
     *          ExitEvents.
     *
     * \param   keepSpecials    If true, preserves ExitEvent objects; if false, removes all events.
     **/
    void remove_events( bool keepSpecials );

    /**
     * \brief   Removes events of a specific runtime class from the queue.
     *
     * \param   eventClassId    Runtime class ID of events to remove.
     **/
    void remove_events( const RuntimeClassID & eventClassId );

    /**
     * \brief   Removes all events from the queue and resets the signal.
     **/
    void remove_all_events();

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Queue Listener object, which is signaled every time 
     *          new Event is pushed or removed.
     **/
    QueueListener &   mEventListener;
    /**
     * \brief   Event queue stack object, which stores event elements
     **/
    SortedEventStack &  mEventQueue;

//////////////////////////////////////////////////////////////////////////
// Forbidden method calls.
//////////////////////////////////////////////////////////////////////////
private:
    EventQueue() = delete;
    AREG_NOCOPY_NOMOVE( EventQueue );
};

//////////////////////////////////////////////////////////////////////////
// ExternalEventQueue class declaration
//////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
/**
 * \brief   Event queue accessed from multiple threads. Used to queue external event types. Notifies
 *          a listener when events are added or queue becomes empty.
 **/
class AREG_API ExternalEventQueue   : public    EventQueue
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes the external event queue with the specified listener and maximum size.
     *
     * \param   eventListener       The listener to notify when events are added or the queue
     *                              becomes empty.
     * \param   maxQueue            The maximum number of event elements. NECommon::IGNORE_VALUE (0)
     *                              means no limit.
     **/
    ExternalEventQueue( QueueListener & eventListener, uint32_t maxQueue );

    /**
     * \brief   Destructor
     **/
    virtual ~ExternalEventQueue();

//////////////////////////////////////////////////////////////////////////
// members
//////////////////////////////////////////////////////////////////////////
private:
    //! The stack to store queued elements.
    SortedEventStack    mStack;

//////////////////////////////////////////////////////////////////////////
// Forbidden method calls.
//////////////////////////////////////////////////////////////////////////
private:
    ExternalEventQueue() = delete;
    AREG_NOCOPY_NOMOVE( ExternalEventQueue );
};

//////////////////////////////////////////////////////////////////////////
// InternalEventQueue class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Event queue accessed from a single thread. Used to queue external event types.
 **/
class AREG_API InternalEventQueue   : public    EventQueue
                                    , private   QueueListener
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes the internal event queue with the specified maximum size.
     *
     * \param   maxQueue    The maximum number of event elements. NECommon::IGNORE_VALUE (0) means
     *                      no limit.
     **/
    InternalEventQueue( uint32_t maxQueue);

    /**
     * \brief   Destructor
     **/
    virtual ~InternalEventQueue();

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************/
// QueueListener interface overrides
/************************************************************************/
    /**
     * \brief   Triggered whenever an event is pushed into the queue or the queue becomes empty.
     *          Override to implement queue signaling logic.
     *
     * \param   eventCount      The number of events in the queue. Zero means the queue is empty and
     *                          the dispatcher can be suspended.
     **/
    void signal_event(uint32_t eventCount ) override;

private:
    //! The stack to store queued elements.
    SortedEventStack   mStack;

    /**
     * \brief   Returns a reference to this queue object.
     **/
    inline InternalEventQueue& self();

//////////////////////////////////////////////////////////////////////////
// Forbidden method calls.
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( InternalEventQueue );
};

//////////////////////////////////////////////////////////////////////////
// EventQueue class inline functions implementation
//////////////////////////////////////////////////////////////////////////
inline bool EventQueue::lock_queue()
{
    return mEventQueue.lock_stack();
}

inline void EventQueue::unlock_queue()
{
    mEventQueue.unlock_stack();
}

inline bool EventQueue::is_empty() const
{
    return mEventQueue.is_empty();
}

#endif  // AREG_COMPONENT_PRIVATE_EVENTQUEUE_HPP
