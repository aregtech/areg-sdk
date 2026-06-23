#ifndef AREG_COMPONENT_WORKERTHREAD_HPP
#define AREG_COMPONENT_WORKERTHREAD_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/WorkerThread.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Worker Thread class.
 *              Use to create component related Worker Thread .
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/component/DispatcherThread.hpp"

#include "areg/component/private/Watchdog.hpp"
namespace areg {

/************************************************************************
 * Dependencies
 ************************************************************************/
class Component;
class ComponentThread;
class WorkerThreadConsumer;

//////////////////////////////////////////////////////////////////////////
// WorkerThread class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Helper thread that performs component tasks under control of a binding component;
 *          communicates via custom events and requires a WorkerThreadConsumer callback handler.
 **/
class AREG_API WorkerThread final : public DispatcherThread
{
//////////////////////////////////////////////////////////////////////////
// Runtime
//////////////////////////////////////////////////////////////////////////
    AREG_DECLARE_RUNTIME(WorkerThread)

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes a worker thread but does not start it; call appropriate create method to
     *          start.
     *
     * \param   threadName          Unique thread name.
     * \param   bindingComponent    Master component that owns this worker thread.
     * \param   threadConsumer      Callback handler for thread lifecycle events.
     * \param   watchdogTimeout     Watchdog timeout in milliseconds; 0 (WATCHDOG_IGNORE) disables
     *                              the watchdog.
     * \param   stackSizeKb         Stack size in kilobytes; 0 (DEFAULT_STACK_SIZE) uses system
     *                              default.
     * \param   maxQueue            Maximum message queue size; IGNORE_VALUE uses configured or
     *                              default value.
     **/
    WorkerThread( const String & threadName
                , Component & bindingComponent
                , WorkerThreadConsumer & threadConsumer
                , uint32_t watchdogTimeout  = areg::WATCHDOG_IGNORE
                , uint32_t stackSizeKb      = areg::DEFAULT_STACK_SIZE
                , uint32_t maxQueue         = areg::IGNORE_VALUE);

    virtual ~WorkerThread() = default;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the master component.
     **/
    [[nodiscard]]
    inline Component & binding_component() const noexcept;

    /**
     * \brief   Returns the component thread of the master component.
     **/
    [[nodiscard]]
    ComponentThread & binding_component_thread() const;

    /**
     * \brief   Terminates the worker thread in an emergency; use only when necessary.
     **/
    void terminate_self();

    /**
     * \brief   Returns the watchdog timeout in milliseconds; 0 means watchdog is disabled.
     **/
    [[nodiscard]]
    inline uint32_t watchdog_timeout() const noexcept;

//////////////////////////////////////////////////////////////////////////
// overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// EventRouter interface overrides.
/************************************************************************/
    /**
     * \brief   Posts an event for delivery; fails and destroys the event if the addressee is not
     *          the master component or if it is internal.
     *
     * \param   eventElem       Event to post.
     **/
    bool post_event( Event & eventElem ) final;

protected:
/************************************************************************/
// Dispatcher overrides
/************************************************************************/

    /**
     * \brief   On enable: registers event consumers via the WorkerThreadConsumer callback.
     *          On disable: unregisters them. Then delegates to the base to update mHasStarted.
     *
     * \param   is_ready    True to enable event dispatching, false to disable.
     **/
    void ready_for_events( bool is_ready ) final;

/************************************************************************/
// DispatcherThread overrides
/************************************************************************/

    /**
     * \brief   Finds the consumer thread for a given class; searches this thread first, then the
     *          master component's dispatcher.
     *
     * \param   whichClass      Runtime class ID of the component to find.
     * \return  Valid dispatcher thread pointer if found; nullptr otherwise.
     **/
    [[nodiscard]]
    DispatcherThread * event_consumer_thread( const uint32_t whichClass ) noexcept final;

/************************************************************************/
// EventDispatcherBase overrides
/************************************************************************/

    /**
     * \brief   Wraps the base dispatch with watchdog guard calls so the watchdog can detect a
     *          stuck event handler.
     *
     * \param   eventElem       Event to dispatch.
     * \return  True if at least one consumer processed the event; false otherwise.
     **/
    bool dispatch_event( Event & eventElem ) final;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Binding (master) component object
     **/
    Component &             mBindingComponent;

    /**
     * \brief   Worker Thread Consumer object
     **/
    WorkerThreadConsumer &  mWorkerThreadConsumer;

    /**
     * \brief   The watchdog object to track the event processing.
     **/
    Watchdog                mWatchdog;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns a reference to this worker thread.
     **/
    inline WorkerThread & self() noexcept;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    WorkerThread() = delete;
    AREG_NOCOPY_NOMOVE( WorkerThread );
};

//////////////////////////////////////////////////////////////////////////
// WorkerThread class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline Component& WorkerThread::binding_component() const noexcept
{
    return mBindingComponent;
}

inline WorkerThread& WorkerThread::self() noexcept
{
    return (*this);
}

inline uint32_t WorkerThread::watchdog_timeout() const noexcept
{
    return mWatchdog.timeout();
}

} // namespace areg
#endif  // AREG_COMPONENT_WORKERTHREAD_HPP
