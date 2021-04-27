#ifndef AREG_COMPONENT_CECOMPONENTTHREAD_HPP
#define AREG_COMPONENT_CECOMPONENTTHREAD_HPP
/************************************************************************
 * \file        areg/component/CEComponentThread.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
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
#include "areg/component/CEDispatcherThread.hpp"

#include "areg/base/TEResourceMap.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class CEComponent;

//////////////////////////////////////////////////////////////////////////
// CEComponentThread class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       All components are instantiated and run in component thread.
 *              Events are dispatched and functions are triggered within
 *              same thread. This ensures that no component function call
 *              and no component data is shared between several threads.
 *              Every component thread can have several component objects.
 * 
 * \details     Component thread is an instance of Dispatcher thread,
 *              which means that every component will dispatch and
 *              receive events. On startup component threads start to 
 *              instantiate component object registered in component loader.
 *              For every instantiated component it triggers 
 *              StartComponent() function of component that component starts
 *              initialization (for example, setup listeners). Before
 *              this call, component will not receive any event message.
 *              When thread is going to be stopped, for every instance
 *              of registered component it will trigger ShutdownComponent()
 *              that component stops execution and starts cleanup.
 *
 **/
class AREG_API CEComponentThread    : public    CEDispatcherThread
{
//////////////////////////////////////////////////////////////////////////
// Local types and constants
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   CEComponentThread::ListComponent;
     *          Linked List of instantiated components in the Component Thread.
     **/
    typedef TELinkedList<CEComponent*, CEComponent*>  ListComponent;

//////////////////////////////////////////////////////////////////////////
// Declare as Runtime instance
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME(CEComponentThread)

//////////////////////////////////////////////////////////////////////////
// Static functions
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns the current Component object of current Component Thread.
     *          The current should be Component Thread and there should be
     *          current Component set in the thread.
     *          If current thread is not a Component Thread and there is
     *          current Component set in the thread, it will return NULL.
     * \return  Returns the current Component object of current Component Thread.
     **/
    static CEComponent * GetCurrentComponent( void );

    /**
     * \brief   Sets current Component object of current Component Thread.
     *          By passing NULL, it will reset current Component in the Component Thread.
     *          The current Component is set automatically in every Component Thread
     *          before processing Event. And resets current Component when
     *          Event processing is completed.
     * \param   curComponent    The current Component to set in the current
     *                          Component Thread. If NULL, it will reset current Component.
     * \return  The function returns true if current Thread is Component Thread.
     *          Otherwise, current Component is not set and function returns false.
     **/
    static bool SetCurrentComponent( CEComponent * curComponent );

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Initialization constructor.
     *          To create dispatcher thread, unique thread name required.
     * \param   threadName  The unique name of component thread.
     **/
    CEComponentThread( const char * threadName );

    /**
     * \brief   Destructor
     **/
    virtual ~CEComponentThread( void );

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
     *              CEThread::ThreadTerminated  -- The waiting timeout expired and thread was termited;
     *              CEThread::ThreadCompleted   -- The thread was valid and completed normaly;
     *              CEThread::ThreadInvalid     -- The thread was not valid and was not running, nothing was done.
     **/
    virtual CEThread::eCompletionStatus DestroyThread( unsigned int waitForStopMs = CEThread::DO_NOT_WAIT );

    /**
     * \brief   Stops Dispatcher and exists Dispatcher Thread without terminating
     **/
    virtual void ShutdownThread( void );

    /**
     * \brief   Wait for thread completion. It will neither sent exit message, nor terminate thread.
     *          The function waits as long, until the thread is not completed.
     *          It will return true if thread has been completed or waiting timeout is CEThread::DO_NOT_WAIT.
     *          If thread exists normally, it will return true.
     * \param   waitForCompleteMs   The timeout to wait for completion.
     * \return  Returns true if either thread completed or the waiting timeout is CEThread::DO_NOT_WAIT.
     **/
    virtual bool CompletionWait( unsigned int waitForCompleteMs = CEThread::WAIT_INFINITE );

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
    virtual bool PostEvent( CEEvent & eventElem );

//////////////////////////////////////////////////////////////////////////
// Overrides. Protected
//////////////////////////////////////////////////////////////////////////
protected:
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

/************************************************************************/
// CEComponentThread overrides
/************************************************************************/

    /**
     * \brief	Called before component thread starts dispatching
     *          event messaged. By default, the list of registered
     *          component entries are taken from application model
     *          object. Overwrite for additional operations.
     * \return	Returns number of instantiated components.
     **/
    virtual int CreateComponents( void );

    /**
     * \brief	Called after components are created.
     *          For event component will trigger
     *          start call to start initialization.
     *          Overwrite for additional operations
     **/
    virtual void StartComponents( void );

    /**
     * \brief	Triggered after getting exit notification.
     *          Before thread exits, for every instantiated
     *          component will trigger shutdown call to
     *          clean up component.
     *          Overwrite for additional operation.
     **/
    virtual void ShutdownComponents( void );

    /**
     * \brief   Called after shutting down components.
     *          For every registered component it will call
     *          appropriate registered delete function
     *          to destroy components.
     **/
    virtual void DestroyComponents( void );

    /**
     * \brief   Search for consumer thread that can dispatch event.
     *          It will check whether component thread has 
     *          registered consumer or not. If not found, will check in worker thread
     *          list of every registered component.
     * \param   whichClass  The runtime class ID to search registered component
     * \return  If found, returns valid pointer of dispatching thread. 
     *          Otherwise returns NULL
     **/
    virtual CEDispatcherThread * GetEventConsumerThread( const CERuntimeClassID & whichClass );

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Current component of Component Thread.
     **/
    CEComponent *   mCurrentComponent;

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
#if _MSC_VER
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
    /**
     * \brief   List of instantiated components in Component Thread.
     **/
    ListComponent   mListComponent;
#if _MSC_VER
    #pragma warning(default: 4251)
#endif  // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns reference to component thread.
     **/
    inline CEComponentThread & self( void );

    /**
     * \brief   Returns pointer of current component thread.
     *          If returns NULL, the current thread is not
     *          a Component Thread.
     **/
    static inline CEComponentThread * _getCurrentComponentThread( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden method calls.
//////////////////////////////////////////////////////////////////////////
private:
    CEComponentThread( void );
    CEComponentThread( const CEComponentThread & /*src*/ );
    const CEComponentThread & operator = ( const CEComponentThread & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// CEComponentThread class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline CEComponentThread & CEComponentThread::self( void )
{   return (*this);     }

inline CEComponentThread* CEComponentThread::_getCurrentComponentThread( void )
{   return RUNTIME_CAST(&(CEDispatcherThread::GetCurrentDispatcherThread()), CEComponentThread);    }

#endif  // AREG_COMPONENT_CECOMPONENTTHREAD_HPP
