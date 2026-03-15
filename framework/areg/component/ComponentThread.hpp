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
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
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
     *                              `areg::STACK_SIZE_DEFAULT` (0) to ignore changing stack size
     *                              and use system default stack size.
     * \param   maxQeueue           The maximum number of events in the internal event queue. Pass
     *                              areg::IGNORE_VALUE to use default value set in configuration
     *                              or ignore the parameter if not configured.
     **/
    explicit ComponentThread( const String & threadName
                            , uint32_t watchdogTimeout  = areg::WATCHDOG_IGNORE
                            , uint32_t stackSizeKb      = areg::STACK_SIZE_DEFAULT
                            , uint32_t maxQeueue        = areg::IGNORE_VALUE);

    virtual ~ComponentThread() = default;

//////////////////////////////////////////////////////////////////////////
// Operations and overrides.
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Terminates the component thread and makes cleanups. This method cleans up all
     *          components and worker threads bind with the component thread. After calling this
     *          method the component thread is not valid and not operable anymore. The method does
     *          not automatically delete the component thread object.
     **/
    void terminate_self();

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

    /**
     * \brief   Wait for thread completion. It will neither sent exit message, nor terminate thread.
     *          The function waits as long, until the thread is not completed. It will return true
     *          if thread has been completed or waiting timeout is areg::DO_NOT_WAIT. If thread
     *          exists normally, it will return true.
     *
     * \param   waitForCompleteMs       The timeout to wait for completion.
     * \return  Returns true if either thread completed or the waiting timeout is
     *          areg::DO_NOT_WAIT.
     **/
    bool wait_completion( uint32_t waitForCompleteMs = areg::WAIT_INFINITE ) final;

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
    DispatcherThread * event_consumer_thread( const RuntimeClassID & whichClass ) final;

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
// ComponentThread lifecycle — not virtual: ComponentThread is final
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
     * \brief   Invokes the registered delete function for every component, or calls delete
     *          directly if no delete function is registered.
     *          Called from on_exit() after shutdown_components().
     **/
    void destroy_components();

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

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

    /**
     * \brief   List of instantiated components in Component Thread.
     **/
    ListComponent   mListComponent;

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(default: 4251)
#endif  // _MSC_VER

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

inline uint32_t ComponentThread::watchdog_timeout() const noexcept
{
    return mWatchdog.timeout();
}

} // namespace areg
#endif  // AREG_COMPONENT_COMPONENTTHREAD_HPP
