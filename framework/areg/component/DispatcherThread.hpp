#ifndef AREG_COMPONENT_DISPATCHERTHREAD_HPP
#define AREG_COMPONENT_DISPATCHERTHREAD_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/component/DispatcherThread.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \brief       AREG Platform, Dispatcher thread.
 *              Generic thread to dispatch events and trigger event processing
 *              functions on registered consumer side.
 *
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/Thread.hpp"
#include "areg/component/EventDispatcher.hpp"

/************************************************************************
 * Declared classes.
 * NullDispatcherThread is declared and implemented in DispatcherThread.cpp
 ************************************************************************/
class DispatcherThread;
class NullDispatcherThread;

//////////////////////////////////////////////////////////////////////////
// DispatcherThread declarations
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * \brief   Global type.
 ************************************************************************/
/**
 * \brief   Generic event dispatching thread.
 *          It is derived from generic thread and event dispatcher classes.
 *          It also contains NullDispatcher object used in case if 
 *          by request to dispatch event no appropriate dispatcher was found
 *          in system. The event dispatching thread is a base class for
 *          Worker thread and Component thread.
 **/
class AREG_API DispatcherThread : public Thread
                                , public EventDispatcher
{
    /**
     * \brief   EventDispatcher needs this to access NullDispatcher.
     **/
    friend class EventDispatcher;

    /**
     * \brief   DispatcherList
     *          List of Dispatcher Thread type.
     **/
    using DispatcherList    = TELinkedList<DispatcherThread *>;

//////////////////////////////////////////////////////////////////////////
// Internal types, constants, etc.
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// Declare this to make runtime information available for dispatcher thread.
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME(DispatcherThread)

//////////////////////////////////////////////////////////////////////////
// Static functions
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief	By given thread name searches registered Event Dispatcher thread and returns object.
     *          If no thread by give name was found, it returns NullDispatcher Thread, which will ignore (destroy) 
     *          any event passed to thread.
     * \param	threadName	The unique name of dispatching thread.
     * \return	If found, returns valid Dispatcher thread. Otherwise, returns NullDispather object, which destroys any event passed to thread.
     **/
    static inline DispatcherThread & getDispatcherThread(const String & threadName);

    /**
     * \brief	By given thread ID searches registered Event Dispatcher thread and returns object.
     *          If no thread by give name was found, it returns NullDispatcher Thread, which will ignore (destroy) 
     *          any event passed to thread.
     * \param   threadId    The unique thread ID.
     * \return	If found, returns valid Dispatcher thread. Otherwise, returns NullDispather object, which destroys any event passed to thread.
     **/
    static inline DispatcherThread & getDispatcherThread( id_type threadId);

    /**
     * \brief	By given thread address searches registered Event Dispatcher thread and returns object.
     *          If no thread by give name was found, it returns NullDispatcher Thread, which will ignore (destroy) 
     *          any event passed to thread.
     * \param   threadAddr  The unique thread address to search. Should be local address
     * \return	If found, returns valid Dispatcher thread. Otherwise, returns NullDispather object, which destroys any event passed to thread.
     **/
    static inline DispatcherThread & getDispatcherThread(const ThreadAddress & threadAddr );

    /**
     * \brief   Static method to get reference to the current 
     *          Event Dispatcher Thread object. If current thread is not 
     *          registered in resource map or it is not a dispatcher thread,
     *          the NullDispatcher will be returned.
     **/
    static inline DispatcherThread & getCurrentDispatcherThread( void );

    /**
     * \brief   Static method to get reference to the current Event Dispatcher
     *          object, i.e. get a dispatcher object of current dispatcher thread.
     *          If current thread is not registered in resource map or is not a 
     *          Dispatcher thread, the Event Dispatcher (invalid dispatcher) of 
     *          NullDispatcher will be returned.
     **/
    static inline EventDispatcher & getCurrentDispatcher( void );

    /**
     * \brief   For specified event class ID it searches the appropriate dispatcher thread.
     *          Should be called when custom events are sent to communicate between threads
     *          The searching is done by priority. At first, it checks the current thread.
     *          If does not find, searches all threads. The event is properly sent to the
     *          target thread if there is only one event consumer for the particular event.
     *          Otherwise, when send an event, specify the target thread to send the event.
     * \return  Returns valid dispatcher thread, which contains consumer to dispatch
     *          event of specified class ID.
     *
     * \see     getEventConsumerThread()
     **/
    static DispatcherThread * findEventConsumerThread(const RuntimeClassID & whichClass);

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	The only constructor to create Dispatcher Thread. The dispatcher thread should have unique
     *          name, which is registered in system (in resource mapping). The Dispatcher Thread can be accessed by its unique name.
     * \param	threadName	The unique name of dispatcher. If this parameter is nullptr or empty, system will unique thread name.
     **/
    explicit DispatcherThread( const String & threadName );
    /**
     * \brief   Destructor.
     **/
    virtual ~DispatcherThread( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Returns reference to Event Dispatcher object of the thread.
     *          Every Dispatching Thread has one event dispatcher object.
     **/
    inline EventDispatcher & getEventDispatcher( void );

    /**
     * \brief   Returns true if specified event is special exit event.
     **/
    bool isExitEvent( const Event * checkEvent ) const;

//////////////////////////////////////////////////////////////////////////
// Operations and overrides.
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Locks current thread and unlocks it when dispatcher is started and ready to dispatch
     * \param   waitTimeout     The waiting timeout in milliseconds
     * \return  Returns true, if dispatcher is started with ready to dispatch. Otherwise it returns false.
     **/
    virtual bool waitForDispatcherStart( unsigned int waitTimeout = NECommon::WAIT_INFINITE );

/************************************************************************/
// Thread overrides
/************************************************************************/

    /**
     * \brief   This call does not stop dispatcher, but sets exit event in the queue
     *          and when all messages are dispatched, the dispatcher will be stopped and exit loop.
     **/
    virtual void triggerExit( void ) override;

    /**
     * \brief	Shuts down the thread and frees resources. If waiting timeout is not 'DO_NOT_WAIT and it expires,
     *          the function terminates the thread. The shutdown thread can be re-created again.
     *          The calling thread (current thread) may be blocked until target thread completes the job.
     * \param	waitForStopMs	Waiting time out in milliseconds until target thread is finis run.
     *                          -   Set DO_NOT_WAIT to trigger exit and immediately return
     *                              without waiting for thread to complete the job.
     *                          -   Set WAIT_INFINITE to trigger exit and wait until thread completes the job.
     *                          -   Set any other value in milliseconds to specify waiting time
     *                              until thread completes the job or timeout expires.
     * \return	Returns the thread completion status. The following statuses are defined:
     *              Thread::ThreadTerminated  -- The waiting timeout expired and thread was terminated;
     *              Thread::ThreadCompleted   -- The thread was valid and completed normally;
     *              Thread::ThreadInvalid     -- The thread was not valid and was not running, nothing was done.
     **/
    virtual Thread::eCompletionStatus shutdownThread( unsigned int waitForStopMs = NECommon::DO_NOT_WAIT ) override;

protected:
/************************************************************************/
// IEEventRouter interface overrides
/************************************************************************/

    /**
     * \brief	Posts event and delivers to its target.
     *          Since the Dispatcher Thread is a Base object for
     *          Worker and Component threads, it does nothing
     *          and only destroys event object without processing.
     *          Override this method or use Worker / Component thread.
     * \param	eventElem	Event object to post
     * \return	In this class it always returns true.
     **/
    virtual bool postEvent( Event & eventElem ) override;

/************************************************************************/
// DispatcherThread overrides
/************************************************************************/

    /**
     * \brief   Call to enable or disable event dispatching threads to receive events.
     *          Override if need to make event dispatching preparation job.
     * \param   isReady     The flag to indicate whether the dispatcher is ready for events.
     **/
    virtual void readyForEvents( bool isReady ) override;

    /**
     * \brief   Search for consumer thread that can dispatch event.
     *          The worker and component threads override this method to get
     *          proper address. If current thread is worker thread, it will 
     *          lookup whether it has consumer or not. If does not find, 
     *          it will search in worker thread list of binded component. 
     *          If the current thread is component thread, it will check whether
     *          it has consumer or not. If not found, will check in worker thread
     *          list of every registered component.
     * \param   whichClass  The runtime class ID to search registered component
     * \return  If found, returns valid pointer of dispatching thread. 
     *          Otherwise returns nullptr
     **/
    virtual DispatcherThread * getEventConsumerThread( const RuntimeClassID & whichClass );

//////////////////////////////////////////////////////////////////////////
// Hidden members
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   DispatcherThread::_getNullDispatherThread()
     *          Returns predefined invalid Null Dispatcher Thread.
     *          Null Dispatch Thread is not running and it is
     *          not dispatching events.
     *          The object is required for error cases.
     **/
    static DispatcherThread & _getNullDispatherThread( void );

    /**
     * \brief   Return reference to self object.
     **/
    inline DispatcherThread & self( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Event, indicating whether Dispatcher started or not.
     *          When Dispatcher is started, this event is signaled.
     *          Otherwise it is not signaled.
     **/
    SynchEvent    mEventStarted;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls.
//////////////////////////////////////////////////////////////////////////
private:
    DispatcherThread( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( DispatcherThread );
};

//////////////////////////////////////////////////////////////////////////
// DispatcherThread class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline DispatcherThread & DispatcherThread::getDispatcherThread( const String & threadName )
{
    DispatcherThread * dispThread = RUNTIME_CAST(threadName.isEmpty() == false ? Thread::findThreadByName(threadName) : Thread::getCurrentThread(), DispatcherThread);
    return ( dispThread != nullptr ? *dispThread : DispatcherThread::_getNullDispatherThread() );
}

inline DispatcherThread & DispatcherThread::getDispatcherThread( id_type threadId )
{
    DispatcherThread* dispThread = RUNTIME_CAST(threadId != 0 ? Thread::findThreadById(threadId) : Thread::getCurrentThread(), DispatcherThread);
    return ( dispThread != nullptr ? *dispThread : DispatcherThread::_getNullDispatherThread() );
}

inline DispatcherThread & DispatcherThread::getDispatcherThread(const ThreadAddress & threadAddr )
{
    DispatcherThread* dispThread = RUNTIME_CAST(Thread::findThreadByAddress(threadAddr), DispatcherThread);
    return ( dispThread != nullptr ? *dispThread : DispatcherThread::_getNullDispatherThread() );
}

inline DispatcherThread & DispatcherThread::getCurrentDispatcherThread( void )
{
    DispatcherThread* currThread = RUNTIME_CAST(Thread::getCurrentThread(), DispatcherThread);
    return ( currThread != nullptr ? *currThread : DispatcherThread::_getNullDispatherThread() );
}

inline EventDispatcher & DispatcherThread::getCurrentDispatcher( void )
{
    return getCurrentDispatcherThread().getEventDispatcher();
}

inline EventDispatcher & DispatcherThread::getEventDispatcher( void )
{
    return static_cast<EventDispatcher &>(self());
}

inline DispatcherThread & DispatcherThread::self( void )
{
    return (*this);
}

#endif  // AREG_COMPONENT_DISPATCHERTHREAD_HPP
