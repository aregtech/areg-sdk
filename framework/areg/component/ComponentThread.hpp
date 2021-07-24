#ifndef AREG_COMPONENT_COMPONENTTHREAD_HPP
#define AREG_COMPONENT_COMPONENTTHREAD_HPP
/************************************************************************
 * \file        areg/component/ComponentThread.hpp
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
#include "areg/component/DispatcherThread.hpp"

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
    typedef TELinkedList<Component*, Component*>  ListComponent;

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
     *          current Component set in the thread, it will return NULL.
     * \return  Returns the current Component object of current Component Thread.
     **/
    static Component * getCurrentComponent( void );

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
    static bool setCurrentComponent( Component * curComponent );

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Initialization constructor.
     *          To create dispatcher thread, unique thread name required.
     * \param   threadName  The unique name of component thread.
     **/
    ComponentThread( const char * threadName );

    /**
     * \brief   Destructor
     **/
    virtual ~ComponentThread( void );

//////////////////////////////////////////////////////////////////////////
// Operations and overrides.
//////////////////////////////////////////////////////////////////////////
/************************************************************************/
// Thread overrides
/************************************************************************/

    /**
     * \brief   Stops Dispatcher and exists Dispatcher Thread without terminating
     **/
    virtual void shutdownThread( void );

    /**
     * \brief   Wait for thread completion. It will neither sent exit message, nor terminate thread.
     *          The function waits as long, until the thread is not completed.
     *          It will return true if thread has been completed or waiting timeout is Thread::DO_NOT_WAIT.
     *          If thread exists normally, it will return true.
     * \param   waitForCompleteMs   The timeout to wait for completion.
     * \return  Returns true if either thread completed or the waiting timeout is Thread::DO_NOT_WAIT.
     **/
    virtual bool completionWait( unsigned int waitForCompleteMs = Thread::WAIT_INFINITE );

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
    virtual bool postEvent( Event & eventElem );

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
    virtual bool runDispatcher( void );

/************************************************************************/
// IEThreadConsumer interface overrides
/************************************************************************/

    /**
     * \brief   Function is called from Thread object when it is going to exit.
     *          This method is triggered after exiting from Run() function.
     * \return  Return thread exit error code.
     **/
    virtual int onThreadExit( void );

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

    /**
     * \brief   Search for consumer thread that can dispatch event.
     *          It will check whether component thread has 
     *          registered consumer or not. If not found, will check in worker thread
     *          list of every registered component.
     * \param   whichClass  The runtime class ID to search registered component
     * \return  If found, returns valid pointer of dispatching thread. 
     *          Otherwise returns NULL
     **/
    virtual DispatcherThread * getEventConsumerThread( const RuntimeClassID & whichClass );

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Current component of Component Thread.
     **/
    Component *   mCurrentComponent;

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
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
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns reference to component thread.
     **/
    inline ComponentThread & self( void );

    /**
     * \brief   Returns pointer of current component thread.
     *          If returns NULL, the current thread is not
     *          a Component Thread.
     **/
    static inline ComponentThread * _getCurrentComponentThread( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden method calls.
//////////////////////////////////////////////////////////////////////////
private:
    ComponentThread( void );
    ComponentThread( const ComponentThread & /*src*/ );
    const ComponentThread & operator = ( const ComponentThread & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// ComponentThread class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline ComponentThread & ComponentThread::self( void )
{
    return (*this);
}

inline ComponentThread* ComponentThread::_getCurrentComponentThread( void )
{
    return RUNTIME_CAST(&(DispatcherThread::getCurrentDispatcherThread()), ComponentThread);
}

#endif  // AREG_COMPONENT_COMPONENTTHREAD_HPP
