#ifndef AREG_COMPONENT_WORKERTHREAD_HPP
#define AREG_COMPONENT_WORKERTHREAD_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/component/WorkerThread.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Worker Thread class.
 *              Use to create component related Worker Thread .
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/DispatcherThread.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class Component;
class ComponentThread;
class IEWorkerThreadConsumer;

//////////////////////////////////////////////////////////////////////////
// WorkerThread class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       The Worker Thread is used as a helper thread for component
 *              to perform certain tasks and should be instantiated when 
 *              component has been started. The Worker Thread requires
 *              component object as a master object, which controls
 *              Worker Thread. Specify and pass Worker Thread Consumer
 *              object when creating object to be able to react on
 *              thread start and stop calls. The communication between 
 *              worker threads and component thread should be via custom 
 *              events. Set and remove event listeners when Worker Thread
 *              starts and stops.
 * 
 * \details     Some components may require additional thread to perform
 *              time consuming computation, which may block thread
 *              for longer time. In such case all other components
 *              will not get any event from dispatcher. In such cases,
 *              it is recommended to define Worker Thread and make all
 *              those tasks there. The Worker Thread should be specified
 *              in Model object. As an instance of Dispatcher Thread,
 *              Worker Thread is able to receive and dispatch events.
 *              Use event objects as main instrument for communication.
 *              The Worker Thread may send event only to its master component
 *              and other Worker Threads, related with component. For example,
 *              if a component has 2 worker threads, these 2 threads still
 *              may communicate directly. The events sent by Worker Thread
 *              by default are defined as External and processed in external
 *              event queue. Worker Threads require Consumer object to 
 *              call start and stop functions to set and remove certain
 *              type of event listeners. For every Thread use separate
 *              event objects. Worker Thread is a Runtime Class and
 *              the type of thread is identified during Runtime.
 *
 **/
class AREG_API WorkerThread    : public DispatcherThread
{
//////////////////////////////////////////////////////////////////////////
// Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME(WorkerThread)

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes Worker Thread object, but does not start yet.
     *          Call appropriate create method to start thread.
     *          Constructor needs to know master component object and
     *          thread consumer object.
     * \param   threadName          The name of Worker Thread. Should be
     *                              unique in application.
     * \param   bindingComponent    The master (binding) component
     * \param   threadConsumer      The Consumer object.
     **/
    WorkerThread( const String & threadName, Component & bindingComponent, IEWorkerThreadConsumer & threadConsumer);

    /**
     * \brief   Destructor
     **/
    virtual ~WorkerThread( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns Binding (master) component object
     **/
    inline Component & getBindingComponent( void ) const;

    /**
     * \brief   Returns binding component thread object
     **/
    ComponentThread & getBindingComponentThread( void ) const;

//////////////////////////////////////////////////////////////////////////
// overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// IEEventRouter interface overrides.
/************************************************************************/
    /**
     * \brief   Posts given event object. If the event addressee is
     *          not binded with master component or it is an internal
     *          event, the event will not be delivered and will be
     *          destroyed.
     * \param   eventElem   The event object to post.
     **/
    virtual bool postEvent( Event & eventElem ) override;

protected:
/************************************************************************/
// Dispatcher overrides
/************************************************************************/
    /**
     * \brief	Triggered when dispatcher starts running. 
     *          In this function runs main dispatching loop.
     *          Events are picked and dispatched here.
     *          Override if logic should be changed.
     * \return	Returns true if Exit Event is signaled.
     **/
    virtual bool runDispatcher( void ) override;

/************************************************************************/
// Dispatcher Thread overrides
/************************************************************************/

    /**
     * \brief   Search for consumer thread that can dispatch event.
     *          It will  lookup whether it has consumer or not
     *          registered in worker thread. If does not find,  it will search
     *          in worker thread list of binded component. 
     * \param   whichClass  The runtime class ID to search registered component
     * \return  If found, returns valid pointer of dispatching thread. 
     *          Otherwise returns nullptr
     **/
    virtual DispatcherThread * getEventConsumerThread( const RuntimeClassID & whichClass ) override;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Binding (master) component object
     **/
    Component &               mBindingComponent;

    /**
     * \brief   Worker Thread Consumer object
     **/
    IEWorkerThreadConsumer &   mWorkerThreadConsumer;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns reference of Worker Thread
     **/
    inline WorkerThread & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    WorkerThread( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( WorkerThread );
};

//////////////////////////////////////////////////////////////////////////
// WorkerThread class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline Component& WorkerThread::getBindingComponent( void ) const
{
    return static_cast<Component &>(mBindingComponent);
}

inline WorkerThread& WorkerThread::self( void )
{
    return (*this);
}

#endif  // AREG_COMPONENT_WORKERTHREAD_HPP
