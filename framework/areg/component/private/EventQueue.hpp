#ifndef AREG_COMPONENT_PRIVATE_EVENTQUEUE_HPP
#define AREG_COMPONENT_PRIVATE_EVENTQUEUE_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/component/private/EventQueue.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Event queue class declaration
 *
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/private/SortedEventStack.hpp"
#include "areg/component/private/IEQueueListener.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class Event;
class RuntimeClassID;

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
    EventQueue( IEQueueListener & eventListener, SortedEventStack & messageQueue );

    /**
     * \brief   Destructor
     **/
    virtual ~EventQueue( void ) = default;

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
     * \brief   Pushes new Event in the Queue and notifies Event Listener
     *          about new Event element availability.
     **/
    void pushEvent( Event & evendElem );

    /**
     * \brief   Pops Event object from Queue and notifies Event Listener if
     *          there is no more Event element in the Queue left.
     * \return  Returns Event object pending in FIFO Stack.
     *          If Queue was not empty, it will return valid pointer.
     *          If Queue was empty, it will return nullptr.
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
     **/
    void removeEvents( const RuntimeClassID & eventClassId );

    /**
     * \brief   Removes all events. Makes event queue empty and resets the signal.
     **/
    void removeAllEvents( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Queue Listener object, which is signaled every time 
     *          new Event is pushed or removed.
     **/
    IEQueueListener &   mEventListener;
    /**
     * \brief   Event queue stack object, which stores event elements
     **/
    SortedEventStack &  mEventQueue;

//////////////////////////////////////////////////////////////////////////
// Forbidden method calls.
//////////////////////////////////////////////////////////////////////////
private:
    EventQueue( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( EventQueue );
};

//////////////////////////////////////////////////////////////////////////
// ExternalEventQueue class declaration
//////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
/**
 * \brief   External event queue class declaration, which is accessed from many threads.
 *          Used to queue external types of event. 
 **/
class AREG_API ExternalEventQueue   : public    EventQueue
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
// members
//////////////////////////////////////////////////////////////////////////
private:
    //! The stack to store queued elements.
    SortedEventStack    mStack;

//////////////////////////////////////////////////////////////////////////
// Forbidden method calls.
//////////////////////////////////////////////////////////////////////////
private:
    ExternalEventQueue( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( ExternalEventQueue );
};

//////////////////////////////////////////////////////////////////////////
// InternalEventQueue class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Internal event queue class declaration, which is accessed only from one thread.
 *          Used to queue external types of event. 
 **/
class AREG_API InternalEventQueue   : public    EventQueue
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
    virtual void signalEvent(uint32_t eventCount ) override;

private:
    //! The stack to store queued elements.
    SortedEventStack   mStack;

    inline InternalEventQueue& self(void);

//////////////////////////////////////////////////////////////////////////
// Forbidden method calls.
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( InternalEventQueue );
};

//////////////////////////////////////////////////////////////////////////
// EventQueue class inline functions implementation
//////////////////////////////////////////////////////////////////////////
inline bool EventQueue::lockQueue( void )
{
    return mEventQueue.lockStack();
}

inline void EventQueue::unlockQueue( void )
{
    mEventQueue.unlockStack();
}

inline bool EventQueue::isEmpty( void ) const
{
    return mEventQueue.isEmpty();
}

#endif  // AREG_COMPONENT_PRIVATE_EVENTQUEUE_HPP
