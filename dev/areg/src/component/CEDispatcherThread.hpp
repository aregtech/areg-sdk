#ifndef AREG_COMPONENT_CEDISPATCHERTHREAD_HPP
#define AREG_COMPONENT_CEDISPATCHERTHREAD_HPP
/************************************************************************
 * \file        areg/src/component/CEDispatcherThread.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \brief       AREG Platform, Dispatcher thread.
 *              Generic thread to dispatch events and trigger event processing
 *              functions on registered consumer side.
 *
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "areg/src/base/CEThread.hpp"
#include "areg/src/component/CEEventDispatcher.hpp"

/************************************************************************
 * Declared classes.
 * CENullDispatcherThread is declared and implemented in CEDispatcherThread.cpp
 ************************************************************************/
class CEDispatcherThread;
class CENullDispatcherThread;

//////////////////////////////////////////////////////////////////////////
// CEDispatcherThread declarations
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * \brief   Global type.
 ************************************************************************/
/**
 * \brief   CEDispatcherList
 *          List of Dispatcher Thread type.
 **/
typedef TELinkedList<CEDispatcherThread *, CEDispatcherThread *>   CEDispatcherList;

/**
 * \brief   Generic event dispatching thread.
 *          It is derived from generic thread and event dispatcher classes.
 *          It also contains NullDispatcher object used in case if 
 *          by request to dispatch event no appropriate dispatcher was found
 *          in system. The event dispatching thread is a base class for
 *          Worker thread and Component thread.
 **/
class AREG_API CEDispatcherThread   : public    CEThread
                                    , public    CEEventDispatcher
{
    /**
     * \brief   CEEventDispatcher needs this to access NullDispatcher.
     **/
    friend class CEEventDispatcher;

//////////////////////////////////////////////////////////////////////////
// Internal types, constants, etc.
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// Declare this to make runtime information available for dispatcher thread.
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME(CEDispatcherThread)

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
    static inline CEDispatcherThread & GetDispatcherThread(const char * threadName);

    /**
     * \brief	By given thread ID searches registered Event Dispatcher thread and returns object.
     *          If no thread by give name was found, it returns NullDispatcher Thread, which will ignore (destroy) 
     *          any event passed to thread.
     * \param   threadId    The unique thread ID.
     * \return	If found, returns valid Dispatcher thread. Otherwise, returns NullDispather object, which destroys any event passed to thread.
     **/
    static inline CEDispatcherThread & GetDispatcherThread(ITEM_ID threadId);

    /**
     * \brief	By given thread address searches registered Event Dispatcher thread and returns object.
     *          If no thread by give name was found, it returns NullDispatcher Thread, which will ignore (destroy) 
     *          any event passed to thread.
     * \param   threadAddr  The unique thread address to search. Should be local address
     * \return	If found, returns valid Dispatcher thread. Otherwise, returns NullDispather object, which destroys any event passed to thread.
     **/
    static inline CEDispatcherThread & GetDispatcherThread(const CEThreadAddress & threadAddr );

    /**
     * \brief   Static method to get reference to the current 
     *          Event Dispatcher Thread object. If current thread is not 
     *          registered in resource map or it is not a dispatcher thread,
     *          the NullDispatcher will be returned.
     **/
    static inline CEDispatcherThread & GetCurrentDispatcherThread( void );

    /**
     * \brief   Static method to get reference to the current Event Dispatcher
     *          object, i.e. get a dispatcher object of current dispatcher thread.
     *          If current thread is not registered in resource map or is not a 
     *          Dispatcher thread, the Event Dispatcher (invalid dispatcher) of 
     *          NullDispatcher will be returned.
     **/
    static inline CEEventDispatcher & GetCurrentDispatcher( void );

    /**
     * \brief   For specified event class ID it searches the appropriate dispatcher thread.
     *          Should be called when custom events are sent to communication between worker 
     *          threads or with component master thread. The searching will be done according
     *          of current thread type. Every worker thread should be able certain event object,
     *          which means that do not assign same events to be dispatched by different consumers
     *          registered within one set of bind threads. For more details, see GetEventConsumerThread()
     * \param   whichClass  The runtime class ID of event object to search dispatching thread.
     * \return  Returns valid dispatcher thread, which contains consumer to dispatch
     *          event of specified class ID.
     *
     * \see     GetEventConsumerThread()
     **/
    static inline CEDispatcherThread * FindEventConsumerThread(const CERuntimeClassID & whichClass);

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	The only constructor to create Dispatcher Thread. The dispatcher thread should have unique
     *          name, which is registered in system (in resource mapping). The Dispatcher Thread can be accessed by its unique name.
     * \param	threadName	The unique name of dispatcher. If this parameter is NULL or empty, system will unique thread name.
     **/
    CEDispatcherThread( const char * threadName );
    /**
     * \brief   Destructor.
     **/
    virtual ~CEDispatcherThread( void );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns reference to Event Dispatcher object of the thread.
     *          Every Dispatching Thread has one event dispatcher object.
     **/
    inline CEEventDispatcher & GetEventDispatcher( void );

    /**
     * \brief   This call does not stop dispatcher, but sets exit event in the queue
     *          and when all messages are dispatched, the dispatcher will be stopped and exit loop.
     **/
    void SetExitEvent( void );

    /**
     * \brief   Returns true if specified event is special exit event.
     **/
    bool IsExitEvent( const CEEvent * checkEvent ) const;

//////////////////////////////////////////////////////////////////////////
// Operations and overrides.
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// CEThread overrides
/************************************************************************/

    /**
     * \brief	Destroys thread and free resources. Once thread is destroyed,
     *          it can be re-created again. The calling thread (current thread)
     *          may be blocked until target thread is not destroyed.
     * \param	waitForStopMs	Waiting time out in milliseconds until
     *                          target thread is not finished running.
     *                          If DO_NOT_WAIT, calling thread (current thread)
     *                          will not be blocked and will not wait until target
     *                          thread finished running. It will immediately
     *                          close thread handle and free resources.
     *                          If WAIT_INFINITE, calling (current) thread will wait
     *                          until target thread completes running.
     *                          Any other value may cause thread termination if 
     *                          waiting time is expired and the target thread still runs.
     * \return	Returns the thread completion status. The following statuses are defined:
     *              CEThread::ThreadTerminated  -- The waiting timeout expired and thread was terminated;
     *              CEThread::ThreadCompleted   -- The thread was valid and completed normally;
     *              CEThread::ThreadInvalid     -- The thread was not valid and was not running, nothing was done.
     **/
    virtual CEThread::eCompletionStatus DestroyThread(unsigned int waitForStopMs = CEThread::DO_NOT_WAIT);

    /**
     * \brief   Stops Dispatcher and exists Dispatcher Thread without terminating
     **/
    virtual void ShutdownThread( void );

    /**
     * \brief   Locks current thread and unlocks it when dispatcher is started and ready to dispatch
     * \param   waitTimeout     The waiting timeout in milliseconds
     * \return  Returns true, if dispatcher is started with ready to dispatch. Otherwise it returns false.
     **/
    virtual bool WaitForDispatcherStart( unsigned int waitTimeout = IESynchObject::WAIT_INFINITE );

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
    virtual bool PostEvent( CEEvent & eventElem );

/************************************************************************/
// CEDispatcherThread overrides
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
     *          Otherwise returns NULL
     **/
    virtual CEDispatcherThread * GetEventConsumerThread( const CERuntimeClassID & whichClass );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Event, indicating whether Dispatcher started or not.
     *          When Dispatcher is started, this event is signaled.
     *          Otherwise it is not signaled.
     **/
    CESynchEvent    mEventStarted;

//////////////////////////////////////////////////////////////////////////
// Hidden members
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   CEDispatcherThread::_getNullDispatherThread()
     *          Returns predefined invalid Null Dispatcher Thread.
     *          Null Dispatch Thread is not running and it is
     *          not dispatching events.
     *          The object is required for error cases.
     **/
    static CEDispatcherThread & _getNullDispatherThread( void );

    /**
     * \brief   Return reference to self object.
     **/
    inline CEDispatcherThread & self( void );
//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls.
//////////////////////////////////////////////////////////////////////////
private:
    CEDispatcherThread( void );
    CEDispatcherThread( const CEDispatcherThread & /*src*/ );
    const CEDispatcherThread & operator = ( const CEDispatcherThread & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// CEDispatcherThread class inline functions implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// Inline static methods
/************************************************************************/

inline CEDispatcherThread & CEDispatcherThread::GetDispatcherThread( const char * threadName )
{
    CEDispatcherThread * dispThread = RUNTIME_CAST(threadName != NULL ? CEThread::FindThreadByName(threadName) : CEThread::GetCurrentThread(), CEDispatcherThread);
    return ( dispThread != NULL ? *dispThread : CEDispatcherThread::_getNullDispatherThread() );
}

inline CEDispatcherThread & CEDispatcherThread::GetDispatcherThread( ITEM_ID threadId )
{
    CEDispatcherThread* dispThread = RUNTIME_CAST(CEThread::FindThreadById(threadId), CEDispatcherThread);
    return ( dispThread != NULL ? *dispThread : CEDispatcherThread::_getNullDispatherThread() );
}

inline CEDispatcherThread & CEDispatcherThread::GetDispatcherThread(const CEThreadAddress & threadAddr )
{
    CEDispatcherThread* dispThread = RUNTIME_CAST(CEThread::FindThreadByAddress(threadAddr), CEDispatcherThread);
    return ( dispThread != NULL ? *dispThread : CEDispatcherThread::_getNullDispatherThread() );
}

inline CEDispatcherThread & CEDispatcherThread::GetCurrentDispatcherThread( void )
{
    CEDispatcherThread* currThread = RUNTIME_CAST(CEThread::GetCurrentThread(), CEDispatcherThread);
    return ( currThread != NULL ? *currThread : CEDispatcherThread::_getNullDispatherThread() );
}

inline CEEventDispatcher & CEDispatcherThread::GetCurrentDispatcher( void )
{
    return GetCurrentDispatcherThread().GetEventDispatcher();
}

inline CEDispatcherThread * CEDispatcherThread::FindEventConsumerThread( const CERuntimeClassID & whichClass )
{
    CEDispatcherThread* thisThread = RUNTIME_CAST(CEThread::GetCurrentThread(), CEDispatcherThread);
    return (thisThread != NULL ? thisThread->GetEventConsumerThread(whichClass) : NULL);
}

/************************************************************************/
// Inline methods
/************************************************************************/

inline CEEventDispatcher & CEDispatcherThread::GetEventDispatcher( void )
{   return static_cast<CEEventDispatcher &>(self());    }

inline CEDispatcherThread & CEDispatcherThread::self( void )
{   return (*this);                                     }

#endif  // AREG_COMPONENT_CEDISPATCHERTHREAD_HPP
