#ifndef AREG_COMPONENT_COMPONENTTHREAD_HPP
#define AREG_COMPONENT_COMPONENTTHREAD_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/component/ComponentThread.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Component Thread.
 *              All components are instantiated and run within
 *              component thread. The component thread is a 
 *              dispatcher of component events.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/component/DispatcherThread.hpp"

#include "areg/component/private/Watchdog.hpp"
#include "areg/base/ResourceMap.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {
    class Component;
} // namespace areg

namespace areg {

//////////////////////////////////////////////////////////////////////////
// ComponentThread class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   All components are instantiated and run in component thread.
 *          Events are dispatched and functions are triggered within
 *          same thread. This ensures that no component function call
 *          and no component data is shared between several threads.
 *          Every component thread can have several component objects.
 **/
class AREG_API ComponentThread final : public DispatcherThread
{
//////////////////////////////////////////////////////////////////////////
// Local types and constants
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   ComponentThread::ListComponent;
     *          Linked List of instantiated components in the Component Thread.
     **/
    using ListComponent     = LinkedList<Component*>;

//////////////////////////////////////////////////////////////////////////
// Declare as Runtime instance
//////////////////////////////////////////////////////////////////////////
    AREG_DECLARE_RUNTIME(ComponentThread)

//////////////////////////////////////////////////////////////////////////
// Static functions
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns the current Component object of current Component Thread. The current should
     *          be Component Thread and there should be current Component set in the thread. If
     *          current thread is not a Component Thread and there is current Component set in the
     *          thread, it will return nullptr.
     *
     * \return  Returns the current Component object of current Component Thread.
     **/
    [[nodiscard]]
    static Component * current_component() noexcept;

    /**
     * \brief   Sets current Component object of current Component Thread. By passing nullptr, it
     *          will reset current Component in the Component Thread. The current Component is set
     *          automatically in every Component Thread before processing Event. And resets current
     *          Component when Event processing is completed.
     *
     * \param   curComponent    The current Component to set in the current Component Thread. If
     *                          nullptr, it will reset current Component.
     * \return  The function returns true if current Thread is Component Thread. Otherwise, current
     *          Component is not set and function returns false.
     **/
    static bool set_current_component( Component * curComponent ) noexcept;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Initializes dispatcher thread. Requires unique thread name.
     *
     * \param   threadName          The unique name of component thread.
     * \param   watchdogTimeout     The watchdog timeout in milliseconds. The watchdog is a guard to
     *                              set the timeout to process and event. If timeout is not zero and
     *                              it expires before the thread processed an event, it terminates
     *                              and restarts the thread again. There is no guarantee that
     *                              terminated thread will make all cleanups properly.
     * \param   stackSizeKb         The stack size of thread in kilobytes (1 KB = 1024 Bytes). Pass
     *                              `areg::DEFAULT_STACK_SIZE` (0) to ignore changing stack size
     *                              and use system default stack size.
     * \param   maxQeueue           The event-queue ring capacity. Pass areg::IGNORE_VALUE (0) to read
     *                              the value from configuration, falling back to the built-in default.
     * \param   dropOnFull          The full-ring policy. areg::Bool::True drops the incoming event when
     *                              the queue is full; areg::Bool::False blocks the producer up to
     *                              \a waitMs; areg::Bool::Undefined reads the value from configuration.
     * \param   waitMs              The lossless full-ring block timeout in milliseconds (used only when
     *                              the resolved policy is "block"). 0 means do not wait; areg::WAIT_INFINITE
     *                              reads the value from configuration.
     **/
    explicit ComponentThread( const String & threadName
                            , uint32_t watchdogTimeout  = areg::WATCHDOG_IGNORE
                            , uint32_t stackSizeKb      = areg::DEFAULT_STACK_SIZE
                            , uint32_t maxQeueue        = areg::IGNORE_VALUE
                            , areg::Bool dropOnFull      = areg::Bool::Undefined
                            , uint32_t waitMs            = areg::WAIT_INFINITE );

    virtual ~ComponentThread() = default;

//////////////////////////////////////////////////////////////////////////
// Operations and overrides.
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Stops the component thread, releases its components and worker threads, and
     *          deletes the thread object. The thread is out of every registry in any case.
     *
     * \return  True if the thread stopped and the object was deleted. False if the thread
     *          could not be stopped: it keeps running and neither it nor anything it owns
     *          is released.
     **/
    bool terminate_self();

    /**
     * \brief   Returns true while this thread is being torn down in order to be recreated by
     *          the watchdog, as opposed to a plain shutdown. Its providers use it to announce
     *          DisconnectReason::ProviderRestarting, so that a consumer knows the same provider
     *          is expected back and can keep its state instead of giving up.
     **/
    [[nodiscard]]
    inline bool is_restarting() const noexcept;

    /**
     * \brief   Returns the watchdog timeout value in milliseconds. The value 0
     *          (areg::WATCHDOG_IGNORE) means the watchdog is ignored by the worker thread.
     **/
    inline uint32_t watchdog_timeout() const noexcept;

/************************************************************************/
// Thread overrides
/************************************************************************/

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
    Thread::ThreadCompletion shutdown( uint32_t waitForStopMs = areg::DO_NOT_WAIT ) final;

/************************************************************************/
// EventRouter interface overrides
/************************************************************************/

    /**
     * \brief   Posts event. Push event in internal or external event queue depending on event type.
     *          Thread should have registered consumer for specified event object.
     *
     * \param   eventElem       The event object to push in the queue.
     * \return  Returns true if successfully pushed event in the queue.
     **/
    bool post_event( Event & eventElem ) final;

//////////////////////////////////////////////////////////////////////////
// Overrides. Protected
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// DispatcherThread overrides
/************************************************************************/

    /**
     * \brief   Creates components, runs the dispatch loop, then tears down. Overrides the base
     *          dispatcher's run_dispatcher to sandwich the component lifecycle around the loop.
     *
     * \return  Returns true if the Exit event was the reason for stopping.
     **/
    bool run_dispatcher() final;

    /**
     * \brief   Finds the dispatcher thread whose consumer handles eventClassId.  Searches this
     *          thread first; if not found, searches each registered component's worker threads.
     *
     * \param   whichClass      Runtime class ID to search for.
     * \return  Valid pointer to the owning dispatcher thread, or nullptr if not found.
     **/
    DispatcherThread * event_consumer_thread( const uint32_t whichClass ) noexcept final;

/************************************************************************/
// ThreadConsumer interface overrides
/************************************************************************/

    /**
     * \brief   Called after the dispatcher loop exits. Shuts down and destroys all components.
     *
     * \return  Thread exit error code.
     **/
    int32_t on_exit() final;

/************************************************************************/
// ComponentThread lifecycle -- not virtual: ComponentThread is final
/************************************************************************/

    /**
     * \brief   Instantiates all components registered for this thread in the application model.
     *          Called once from run_dispatcher() before the dispatch loop starts.
     *
     * \return  Number of components successfully created.
     **/
    int32_t create_components();

    /**
     * \brief   Sends the startup notification to every instantiated component.
     *          Called once from run_dispatcher() after create_components() succeeds.
     **/
    void start_components();

    /**
     * \brief   Sends the shutdown notification to every component and stops all proxy objects.
     *          Called from on_exit() before destroy_components().
     **/
    void shutdown_components();

    /**
     * \brief   Gives every component the chance to wait for its worker threads to finish.
     *          Called from on_exit() between shutdown_components() and destroy_components(),
     *          so the components are still alive and run on the thread that owns them.
     **/
    void wait_components_completion();

    /**
     * \brief   Invokes the registered delete function for every component, or calls delete
     *          directly if no delete function is registered.
     *          Called from on_exit() after wait_components_completion().
     **/
    void destroy_components();

    /**
     * \brief   Moves every component off the shared list into \a result under the list lock,
     *          so that the caller can destroy them with no lock held.
     *
     * \param[out]  result  Receives the components taken off the list.
     **/
    inline void _detach_components( ListComponent & result );

/************************************************************************/
// EventDispatcherBase overrides
/************************************************************************/

    /**
     * \brief   Wraps the base dispatch with watchdog guard calls so the watchdog can detect a
     *          stuck event handler.
     *
     * \param   eventElem       Event element to dispatch.
     * \return  True if at least one consumer processed the event.
     **/
    bool dispatch_event( Event & eventElem ) final;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns reference to component thread.
     **/
    [[nodiscard]]
    inline ComponentThread & self() noexcept;

    /**
     * \brief   Returns pointer of current component thread. If returns nullptr, the current thread
     *          is not a Component Thread.
     **/
    [[nodiscard]]
    static inline ComponentThread * _current_component_thread() noexcept;

    /**
     * \brief   Called to shutdown proxies registered in the thread.
     **/
    inline void _shutdown_proxies();

    /**
     * \brief   Called to shutdown components registered in the thread.
     **/
    inline void _shutdown_components();

    /**
     * \brief   Releases the proxies and the components of this thread. Called only when the
     *          thread was killed by the OS and never ran its own exit sequence.
     **/
    inline void _release_abandoned_objects();

    /**
     * \brief   Removes the proxies of this thread from the proxy registries, so that none of
     *          them survives with a reference to this thread object after it is deleted.
     **/
    inline void _detach_thread_proxies();

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Current component of Component Thread.
     **/
    Component *     mCurrentComponent;

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The watchdog object to track the event processing.
     **/
    Watchdog        mWatchdog;

    /**
     * \brief   Set while terminate_self() tears the thread down for a watchdog restart.
     **/
    bool            mIsRestarting;

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

    /**
     * \brief   List of instantiated components in Component Thread.
     **/
    ListComponent   mListComponent;

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif  // _MSC_VER

    /**
     * \brief   Guards the component list. This thread fills it and empties it, while any
     *          thread that looks for an event consumer walks it. Held only for the walk
     *          and for the list operation itself, never across a thread shutdown.
     **/
    mutable SpinLock    mListLock;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    ComponentThread() = delete;
    AREG_NOCOPY_NOMOVE( ComponentThread );
};

//////////////////////////////////////////////////////////////////////////
// ComponentThread inline methods.
//////////////////////////////////////////////////////////////////////////

inline bool ComponentThread::is_restarting() const noexcept
{
    return mIsRestarting;
}

inline uint32_t ComponentThread::watchdog_timeout() const noexcept
{
    return mWatchdog.timeout();
}

} // namespace areg
#endif  // AREG_COMPONENT_COMPONENTTHREAD_HPP
