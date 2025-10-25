#ifndef AREG_COMPONENT_COMPONENTTHREAD_HPP
#define AREG_COMPONENT_COMPONENTTHREAD_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/component/ComponentThread.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Component Thread.
 *              All components are instantiated and run within
 *              component thread. The component thread is a 
 *              dispatcher of component events.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/DispatcherThread.hpp"

#include "areg/component/private/Watchdog.hpp"
#include "areg/base/TEResourceMap.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class Component;

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
class AREG_API ComponentThread  : public    DispatcherThread
{
//////////////////////////////////////////////////////////////////////////
// Local types and constants
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   ComponentThread::ListComponent;
     *          Linked List of instantiated components in the Component Thread.
     **/
    using ListComponent     = TELinkedList<Component*>;

//////////////////////////////////////////////////////////////////////////
// Declare as Runtime instance
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME(ComponentThread)

//////////////////////////////////////////////////////////////////////////
// Static functions
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns the current Component object of current Component Thread.
     *          The current should be Component Thread and there should be
     *          current Component set in the thread.
     *          If current thread is not a Component Thread and there is
     *          current Component set in the thread, it will return nullptr.
     * \return  Returns the current Component object of current Component Thread.
     **/
    static Component * getCurrentComponent( void );

    /**
     * \brief   Sets current Component object of current Component Thread.
     *          By passing nullptr, it will reset current Component in the Component Thread.
     *          The current Component is set automatically in every Component Thread
     *          before processing Event. And resets current Component when
     *          Event processing is completed.
     * \param   curComponent    The current Component to set in the current
     *                          Component Thread. If nullptr, it will reset current Component.
     * \return  The function returns true if current Thread is Component Thread.
     *          Otherwise, current Component is not set and function returns false.
     **/
    static bool setCurrentComponent( Component * curComponent );

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Initialization constructor.
     *          To create dispatcher thread, unique thread name required.
     * \param   threadName      The unique name of component thread.
     * \param   watchdogTimeout The watchdog timeout in milliseconds.
     *                          The watchdog is a guard to set the timeout to process and event.
     *                          If timeout is not zero and it expires before the thread processed
     *                          an event, it terminates and restarts the thread again.
     *                          There is no guarantee that terminated thread will make all cleanups properly.
     * \param   stackSizeKb     The stack size of thread in kilobytes (1 KB = 1024 Bytes). Pass `NECommon::STACK_SIZE_DEFAULT` (0)
     *                          to ignore changing stack size and use system default stack size.
     **/
    explicit ComponentThread( const String & threadName, uint32_t watchdogTimeout = NECommon::WATCHDOG_IGNORE, uint32_t stackSizeKb = NECommon::STACK_SIZE_DEFAULT);

    /**
     * \brief   Destructor
     **/
    virtual ~ComponentThread( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operations and overrides.
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Call to terminate the component thread and make cleanups.
     *          This method cleans up all components and worker threads bind
     *          with the component thread. After calling this method the component
     *          thread is not valid and not operable anymore.
     *          The method does not automatically delete the component thread object.
     **/
    void terminateSelf( void );

    /**
     * \brief   Returns the watchdog timeout value in milliseconds. The value 0
     *          (NECommon::WATCHDOG_IGNORE) means the watchdog is ignored by the worker thread.
     **/
    inline uint32_t getWatchdogTimeout(void) const;

/************************************************************************/
// Thread overrides
/************************************************************************/

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

    /**
     * \brief   Wait for thread completion. It will neither sent exit message, nor terminate thread.
     *          The function waits as long, until the thread is not completed.
     *          It will return true if thread has been completed or waiting timeout is NECommon::DO_NOT_WAIT.
     *          If thread exists normally, it will return true.
     * \param   waitForCompleteMs   The timeout to wait for completion.
     * \return  Returns true if either thread completed or the waiting timeout is NECommon::DO_NOT_WAIT.
     **/
    virtual bool completionWait( unsigned int waitForCompleteMs = NECommon::WAIT_INFINITE ) override;

/************************************************************************/
// IEEventRouter interface overrides
/************************************************************************/

    /**
     * \brief   Posts event. Push event in internal or external 
     *          event queue depending on event type.
     *          Thread should have registered consumer for
     *          specified event object.
     * \param   eventElem   The event object to push in the queue.
     * \return  Returns true if successfully pushed event in the queue.
     **/
    virtual bool postEvent( Event & eventElem ) override;

//////////////////////////////////////////////////////////////////////////
// Overrides. Protected
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// DispatcherThread overrides
/************************************************************************/

    /**
     * \brief	Triggered when dispatcher starts running. 
     *          In this function runs main dispatching loop.
     *          Events are picked and dispatched here.
     *          Override if logic should be changed.
     * \return	Returns true if Exit Event is signaled.
     **/
    virtual bool runDispatcher( void ) override;

    /**
     * \brief   Search for consumer thread that can dispatch event.
     *          It will check whether component thread has 
     *          registered consumer or not. If not found, will check in worker thread
     *          list of every registered component.
     * \param   whichClass  The runtime class ID to search registered component
     * \return  If found, returns valid pointer of dispatching thread. 
     *          Otherwise returns nullptr
     **/
    virtual DispatcherThread * getEventConsumerThread( const RuntimeClassID & whichClass ) override;

/************************************************************************/
// IEThreadConsumer interface overrides
/************************************************************************/

    /**
     * \brief   Function is called from Thread object when it is going to exit.
     *          This method is triggered after exiting from Run() function.
     * \return  Return thread exit error code.
     **/
    virtual int onThreadExit( void ) override;

/************************************************************************/
// ComponentThread overrides
/************************************************************************/

    /**
     * \brief	Called before component thread starts dispatching
     *          event messaged. By default, the list of registered
     *          component entries are taken from application model
     *          object. Overwrite for additional operations.
     * \return	Returns number of instantiated components.
     **/
    virtual int createComponents( void );

    /**
     * \brief	Called after components are created.
     *          For event component will trigger
     *          start call to start initialization.
     *          Overwrite for additional operations
     **/
    virtual void startComponents( void );

    /**
     * \brief	Triggered after getting exit notification.
     *          Before thread exits, for every instantiated
     *          component will trigger shutdown call to
     *          clean up component.
     *          Overwrite for additional operation.
     **/
    virtual void shutdownComponents( void );

    /**
     * \brief   Called after shutting down components.
     *          For every registered component it will call
     *          appropriate registered delete function
     *          to destroy components.
     **/
    virtual void destroyComponents( void );

/************************************************************************/
// IEEventDispatcher overrides
/************************************************************************/

    /**
     * \brief	The method is triggered to start dispatching valid event.
     *          Here dispatcher should forward message to appropriate 
     *          registered event consumer
     * \param	eventElem   Event element to dispatch	
     * \return	Returns true if at least one consumer processed event.
     *          Otherwise it returns false.
     **/
    virtual bool dispatchEvent( Event & eventElem ) override;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns reference to component thread.
     **/
    inline ComponentThread & self( void );

    /**
     * \brief   Returns pointer of current component thread.
     *          If returns nullptr, the current thread is not
     *          a Component Thread.
     **/
    static inline ComponentThread * _getCurrentComponentThread( void );

    /**
     * \brief   Called to shutdown proxies registered in the thread.
     **/
    inline void _shutdownProxies( void );

    /**
     * \brief   Called to shutdown components registered in the thread.
     **/
    inline void _shutdownComponents( void );

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
    ComponentThread( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( ComponentThread );
};

//////////////////////////////////////////////////////////////////////////
// ComponentThread inline methods.
//////////////////////////////////////////////////////////////////////////

inline uint32_t ComponentThread::getWatchdogTimeout(void) const
{
    return mWatchdog.getTimeout();
}

#endif  // AREG_COMPONENT_COMPONENTTHREAD_HPP
