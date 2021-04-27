#ifndef AREG_COMPONENT_PRIVATE_CEEVENTQUEUE_HPP
#define AREG_COMPONENT_PRIVATE_CEEVENTQUEUE_HPP
/************************************************************************
 * \file        areg/component/private/CEEventQueue.hpp
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
class CEEvent;
class CERuntimeClassID;

//////////////////////////////////////////////////////////////////////////
// CEEventQueue class declaration
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
// CEEventQueue class declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Event queue class is a base class for external and 
 *          internal event queue classes. It contains basic methods to
 *          push and pop event objects from Stack.
 **/
class AREG_API CEEventQueue
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
    CEEventQueue( IEQueueListener & eventListener, TEStack<CEEvent *, CEEvent *> & eventQueue );

    /**
     * \brief   Destructor
     **/
    virtual ~CEEventQueue( void );

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
    inline bool LockQueue( void );

    /**
     * \brief   Unlocks previously locked Event Queue, letting other
     *          threads having data access
     **/
    inline void UnlockQueue( void );

    /**
     * \brief   Returns true, if Event Queue is empty.
     **/
    inline bool IsEmpty( void ) const;

    /**
     * \brief   Returns number of pending Events in the Queue.
     **/
    inline int GetSize( void ) const;

    /**
     * \brief   Pushes new Event in the Queue and notifies Event Listener
     *          about new Event element availability.
     **/
    void PushEvent( CEEvent & evendElem );

    /**
     * \brief   Pops Event object from Queue and notifies Event Listener if
     *          there is no more Event element in the Queue left.
     * \return  Returns Event object pending in FIFO Stack.
     *          If Queue was not empty, it will return valid pointer.
     *          If Queue was empty, it will return NULL.
     **/
    CEEvent * PopEvent( void );

    /**
     * \brief   Removes all Event elements from the Queue and if keepSpecials is true,
     *          it will not remove special predefined Exit Event (CEExitEvent) objects,
     *          which notify Dispatcher to complete job and exit.
     *          After Events are removed, it will signal Event Listener whether queue is 
     *          empty or not.
     *          When Event objects are removed from the Queue, Destroy() method of every
     *          Event is called to make cleanup
     * \param   keepSpecials    If true, it will remove all Event objects from the Queue,
     *                          except predefined special Exit Event (CEExitEvent).
     *                          Otherwise it will remove all elements.
     **/
    void RemoveEvents( bool keepSpecials );

    /**
     * \brief   Removes the specified Runtime Event objects from the Queue and returns
     *          the number of removed Events. For every Event the method Destroy() will
     *          be called to make cleanup
     *          After Events are removed, it will signal Event Listener whether queue is 
     *          empty or not.
     * \param   eventClassId    Runtime class ID of Event object to remove from the Queue.
     * \return  Returns number of removed Events from the Queue.
     **/
    int RemoveEvents( const CERuntimeClassID & eventClassId );

    /**
     * \brief   Removes all events. Makes event queue empty
     **/
    virtual void RemoveAllEvents( void );

//////////////////////////////////////////////////////////////////////////
// Protected methods
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Removes all Event elements from the Queue and if keepSpecials is true,
     *          it will not remove special predefined Exit Event (CEExitEvent) objects,
     *          which notify Dispatcher to complete job and exit.
     *          When Event objects are removed from the Queue, Destroy() method of every
     *          Event is called to make cleanup.
     *          The method does not send any signal to Event Listener object.
     * \param   keepSpecials    If true, it will remove all Event objects from the Queue,
     *                          except predefined special Exit Event (CEExitEvent).
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
    IEQueueListener &               mEventListener;

    /**
     * \brief   Event queue stack object, which stores event elements
     **/
    TEStack<CEEvent *, CEEvent *> & mEventQueue;

//////////////////////////////////////////////////////////////////////////
// Forbidden method calls.
//////////////////////////////////////////////////////////////////////////
private:
    CEEventQueue( void );
    CEEventQueue(const CEEventQueue &);
    const CEEventQueue& operator = (const CEEventQueue &);
};

//////////////////////////////////////////////////////////////////////////
// CEExternalEventQueue class declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   External event queue class declaration, which is accessed from many threads.
 *          Used to queue external types of event. 
 **/
class AREG_API CEExternalEventQueue : public    CEEventQueue
                                    , private   TELockStack<CEEvent *, CEEvent *>
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
    CEExternalEventQueue( IEQueueListener & eventListener );

    /**
     * \brief   Destructor
     **/
    virtual ~CEExternalEventQueue( void );

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns instance of CEExternalEventQueue object
     **/
    inline CEExternalEventQueue & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden method calls.
//////////////////////////////////////////////////////////////////////////
private:
    CEExternalEventQueue( void );
    CEExternalEventQueue( const CEExternalEventQueue & );
    const CEExternalEventQueue & operator = ( const CEExternalEventQueue & );
};

//////////////////////////////////////////////////////////////////////////
// CEInternalEventQueue class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Internal event queue class declaration, which is accessed only from one thread.
 *          Used to queue external types of event. 
 **/
class AREG_API CEInternalEventQueue : public    CEEventQueue
                                    , private   TENolockStack<CEEvent *, CEEvent *>
                                    , private   IEQueueListener
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor.
     **/
    CEInternalEventQueue( void );

    /**
     * \brief   Destructor
     **/
    virtual ~CEInternalEventQueue( void );

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
    virtual void SignalEvent( int eventCount );

private:
    /**
     * \brief   Returns instance of CEInternalEventQueue object
     **/
    inline CEInternalEventQueue & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden method calls.
//////////////////////////////////////////////////////////////////////////
private:
    CEInternalEventQueue( const CEInternalEventQueue & );
    const CEInternalEventQueue & operator = ( const CEInternalEventQueue & );
};

//////////////////////////////////////////////////////////////////////////
// CEEventQueue class inline functions implementation
//////////////////////////////////////////////////////////////////////////
inline bool CEEventQueue::LockQueue( void )
{   return mEventQueue.LockStack(); }

inline void CEEventQueue::UnlockQueue( void )
{   mEventQueue.UnlockStack();      }

inline bool CEEventQueue::IsEmpty( void ) const
{   return mEventQueue.IsEmpty();   }

inline int CEEventQueue::GetSize( void ) const
{   return mEventQueue.GetSize();   }

//////////////////////////////////////////////////////////////////////////
// CEExternalEventQueue class inline functions implementation
//////////////////////////////////////////////////////////////////////////
inline CEExternalEventQueue & CEExternalEventQueue::self( void )
{   return (*this);                 }

//////////////////////////////////////////////////////////////////////////
// CEInternalEventQueue class inline functions implementation
//////////////////////////////////////////////////////////////////////////
inline CEInternalEventQueue & CEInternalEventQueue::self( void )
{   return (*this);                 }

#endif  // AREG_COMPONENT_PRIVATE_CEEVENTQUEUE_HPP
