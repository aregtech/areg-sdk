#ifndef AREG_COMPONENT_DISPATCHERTHREAD_HPP
#define AREG_COMPONENT_DISPATCHERTHREAD_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/DispatcherThread.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \brief       Areg Platform, Dispatcher thread.
 *              Generic thread to dispatch events and trigger event processing
 *              functions on registered consumer side.
 *
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/Thread.hpp"
#include "areg/component/EventDispatcher.hpp"
namespace areg {

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
 * Global type.
 ************************************************************************/
/**
 * \brief   Generic event dispatching thread derived from generic thread and event dispatcher
 *          classes. It also contains NullDispatcher object used in case if by request to dispatch
 *          event no appropriate dispatcher was found in system. The event dispatching thread is a
 *          base class for Worker thread and Component thread.
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
    using DispatcherList    = LinkedList<DispatcherThread *>;

//////////////////////////////////////////////////////////////////////////
// Internal types, constants, etc.
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// Declare this to make runtime information available for dispatcher thread.
//////////////////////////////////////////////////////////////////////////
    AREG_DECLARE_RUNTIME(DispatcherThread)

//////////////////////////////////////////////////////////////////////////
// Static functions
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   By given thread name searches registered Event Dispatcher thread and returns object.
     *          If no thread by give name was found, it returns NullDispatcher Thread, which will
     *          ignore (destroy) any event passed to thread.
     *
     * \param   threadName      The unique name of dispatching thread.
     * \return  If found, returns valid Dispatcher thread. Otherwise, returns NullDispather object,
     *          which destroys any event passed to thread.
     **/
    [[nodiscard]]
    static inline DispatcherThread & dispatcher_thread(const String & threadName) noexcept;

    /**
     * \brief   By given thread ID searches registered Event Dispatcher thread and returns object.
     *          If no thread by give name was found, it returns NullDispatcher Thread, which will
     *          ignore (destroy) any event passed to thread.
     *
     * \param   threadId    The unique thread ID.
     * \return  If found, returns valid Dispatcher thread. Otherwise, returns NullDispather object,
     *          which destroys any event passed to thread.
     **/
    [[nodiscard]]
    static inline DispatcherThread & dispatcher_thread( id_type threadId) noexcept;

    /**
     * \brief   By given thread address searches registered Event Dispatcher thread and returns
     *          object. If no thread by give name was found, it returns NullDispatcher Thread, which
     *          will ignore (destroy) any event passed to thread.
     *
     * \param   threadAddr      The unique thread address to search. Should be local address
     * \return  If found, returns valid Dispatcher thread. Otherwise, returns NullDispather object,
     *          which destroys any event passed to thread.
     **/
    [[nodiscard]]
    static inline DispatcherThread & dispatcher_thread(const ThreadAddress & threadAddr ) noexcept;

    /**
     * \brief   Returns reference to the current Event Dispatcher Thread object. If current thread
     *          is not registered in resource map or it is not a dispatcher thread, the
     *          NullDispatcher will be returned.
     **/
    [[nodiscard]]
    static inline DispatcherThread & current_dispatcher_thread() noexcept;

    /**
     * \brief   Returns reference to the current Event Dispatcher object, i.e. get a dispatcher
     *          object of current dispatcher thread. If current thread is not registered in resource
     *          map or is not a Dispatcher thread, the Event Dispatcher (invalid dispatcher) of
     *          NullDispatcher will be returned.
     **/
    [[nodiscard]]
    static inline EventDispatcher & current_dispatcher() noexcept;

    /**
     * \brief   For specified event class ID it searches the appropriate dispatcher thread. Should
     *          be called when custom events are sent to communicate between threads The searching
     *          is done by priority. At first, it checks the current thread. If does not find,
     *          searches all threads. The event is properly sent to the target thread if there is
     *          only one event consumer for the particular event. Otherwise, when send an event,
     *          specify the target thread to send the event.
     *
     * \return  Returns valid dispatcher thread, which contains consumer to dispatch event of
     *          specified class ID.
     **/
    [[nodiscard]]
    static DispatcherThread * find_consumer_thread(const RuntimeClassID & whichClass) noexcept;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates Dispatcher Thread with unique name, which is registered in system (in
     *          resource mapping). The Dispatcher Thread can be accessed by its unique name.
     *
     * \param   threadName      The unique name of dispatcher. If this parameter is nullptr or
     *                          empty, system will unique thread name.
     * \param   stackSizeKb     The stack size of the thread in kilobytes (1 KB = 1024 Bytes). Pass
     *                          `areg::STACK_SIZE_DEFAULT` (0) to ignore changing stack size and
     *                          use system default stack size.
     * \param   maxQeueue       The maximum number of queued external events. Pass
     *                          `areg::IGNORE_VALUE` to use default value set in configuration
     *                          or ignore the parameter if not configured. The configuration is set
     *                          in `areg.init` file under key "config::*::default::messagequeue".
     **/
    explicit DispatcherThread( const String & threadName, uint32_t stackSizeKb, uint32_t maxQeueue);
    virtual ~DispatcherThread() = default;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Returns reference to Event Dispatcher object of the thread. Every Dispatching Thread
     *          has one event dispatcher object.
     **/
    [[nodiscard]]
    inline EventDispatcher & event_dispatcher() noexcept;

    /**
     * \brief   Returns true if specified event is special exit event.
     **/
    [[nodiscard]]
    bool is_exit_event( const Event * checkEvent ) const noexcept;

//////////////////////////////////////////////////////////////////////////
// Operations and overrides.
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Locks current thread and unlocks it when dispatcher is started and ready to
     *          dispatch.
     *
     * \param   waitTimeout     The waiting timeout in milliseconds
     * \return  Returns true, if dispatcher is started with ready to dispatch. Otherwise it returns
     *          false.
     **/
    virtual bool wait_start( uint32_t waitTimeout = areg::WAIT_INFINITE );

/************************************************************************/
// Thread overrides
/************************************************************************/

    /**
     * \brief   Sets exit event in the queue. When all messages are dispatched, the dispatcher will
     *          be stopped and exit loop.
     **/
    void trigger_exit() override;

    /**
     * \brief   Shuts down the thread and frees resources. If waiting timeout is not 'DO_NOT_WAIT
     *          and it expires, the function terminates the thread. The shutdown thread can be
     *          re-created again. The calling thread (current thread) may be blocked until target
     *          thread completes the job.
     *
     * \param   waitForStopMs       Waiting time out in milliseconds until target thread is finis
     *                              run. - Set DO_NOT_WAIT to trigger exit and immediately return
     *                              without waiting for thread to complete the job. - Set
     *                              WAIT_INFINITE to trigger exit and wait until thread completes
     *                              the job. - Set any other value in milliseconds to specify
     *                              waiting time until thread completes the job or timeout expires.
     * \return  Returns the thread completion status. The following statuses are defined:
     *          Thread::Terminated -- The waiting timeout expired and thread was terminated;
     *          Thread::Completed -- The thread was valid and completed normally; Thread::Invalid --
     *          The thread was not valid and was not running, nothing was done.
     **/
    Thread::ThreadCompletion shutdown( uint32_t waitForStopMs = areg::DO_NOT_WAIT ) override;

protected:
/************************************************************************/
// EventRouter interface overrides
/************************************************************************/

    /**
     * \brief   Posts event and delivers to its target. Since the Dispatcher Thread is a Base object
     *          for Worker and Component threads, it does nothing and only destroys event object
     *          without processing. Override this method or use Worker / Component thread.
     *
     * \param   eventElem       Event object to post
     * \return  In this class it always returns true.
     **/
    bool post_event( Event & eventElem ) override;

/************************************************************************/
// DispatcherThread overrides
/************************************************************************/

    /**
     * \brief   Call to enable or disable event dispatching threads to receive events. Override if
     *          need to make event dispatching preparation job.
     *
     * \param   is_ready    The flag to indicate whether the dispatcher is ready for events.
     **/
    void ready_for_events( bool is_ready ) override;

    /**
     * \brief   Search for consumer thread that can dispatch event. The worker and component threads
     *          override this method to get proper address. If current thread is worker thread, it
     *          will lookup whether it has consumer or not. If does not find, it will search in
     *          worker thread list of binded component. If the current thread is component thread,
     *          it will check whether it has consumer or not. If not found, will check in worker
     *          thread list of every registered component.
     *
     * \param   whichClass      The runtime class ID to search registered component
     * \return  If found, returns valid pointer of dispatching thread. Otherwise returns nullptr
     **/
    virtual DispatcherThread * event_consumer_thread( const RuntimeClassID & whichClass );

//////////////////////////////////////////////////////////////////////////
// Hidden members
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Returns predefined invalid Null Dispatcher Thread. Null Dispatch Thread is not
     *          running and it is not dispatching events. The object is required for error cases.
     **/
    [[nodiscard]]
    static DispatcherThread & _null_dispather_thread() noexcept;

    /**
     * \brief   Returns reference to self object.
     **/
    [[nodiscard]]
    inline DispatcherThread & self() noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Event, indicating whether Dispatcher started or not.
     *          When Dispatcher is started, this event is signaled.
     *          Otherwise it is not signaled.
     **/
    SyncEvent    mEventStarted;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls.
//////////////////////////////////////////////////////////////////////////
private:
    DispatcherThread() = delete;
    AREG_NOCOPY_NOMOVE( DispatcherThread );
};

//////////////////////////////////////////////////////////////////////////
// DispatcherThread class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline DispatcherThread & DispatcherThread::dispatcher_thread( const String & threadName ) noexcept
{
    DispatcherThread * dispThread = AREG_RUNTIME_CAST(threadName.is_empty() == false ? Thread::find_by_name(threadName) : Thread::current_thread(), DispatcherThread);
    return ( dispThread != nullptr ? *dispThread : DispatcherThread::_null_dispather_thread() );
}

inline DispatcherThread & DispatcherThread::dispatcher_thread( id_type threadId ) noexcept
{
    DispatcherThread* dispThread = AREG_RUNTIME_CAST(threadId != 0 ? Thread::find_by_id(threadId) : Thread::current_thread(), DispatcherThread);
    return ( dispThread != nullptr ? *dispThread : DispatcherThread::_null_dispather_thread() );
}

inline DispatcherThread & DispatcherThread::dispatcher_thread(const ThreadAddress & threadAddr ) noexcept
{
    DispatcherThread* dispThread = AREG_RUNTIME_CAST(Thread::find_by_address(threadAddr), DispatcherThread);
    return ( dispThread != nullptr ? *dispThread : DispatcherThread::_null_dispather_thread() );
}

inline DispatcherThread & DispatcherThread::current_dispatcher_thread() noexcept
{
    DispatcherThread* currThread = AREG_RUNTIME_CAST(Thread::current_thread(), DispatcherThread);
    return ( currThread != nullptr ? *currThread : DispatcherThread::_null_dispather_thread() );
}

inline EventDispatcher & DispatcherThread::current_dispatcher() noexcept
{
    return current_dispatcher_thread().event_dispatcher();
}

inline EventDispatcher & DispatcherThread::event_dispatcher() noexcept
{
    return static_cast<EventDispatcher &>(self());
}

inline DispatcherThread & DispatcherThread::self() noexcept
{
    return (*this);
}

} // namespace areg
#endif  // AREG_COMPONENT_DISPATCHERTHREAD_HPP
