#ifndef AREG_COMPONENT_CEWORKERTHREAD_HPP
#define AREG_COMPONENT_CEWORKERTHREAD_HPP
/************************************************************************
 * \file        areg/component/CEWorkerThread.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Worker Thread class.
 *              Use to create component related Worker Thread .
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/CEDispatcherThread.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class CEComponent;
class CEComponentThread;
class IEWorkerThreadConsumer;

//////////////////////////////////////////////////////////////////////////
// CEWorkerThread class declaration
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
class AREG_API CEWorkerThread    : public CEDispatcherThread
{
//////////////////////////////////////////////////////////////////////////
// Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME(CEWorkerThread)

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
    CEWorkerThread( const char * threadName, CEComponent & bindingComponent, IEWorkerThreadConsumer & threadConsumer);

    /**
     * \brief   Destructor
     **/
    virtual ~CEWorkerThread( void );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns Binding (master) component object
     **/
    inline CEComponent & GetBindingComponent( void ) const;

    /**
     * \brief   Returns binding component thread object
     **/
    CEComponentThread & GetBindingComponentThread( void ) const;

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
    virtual bool PostEvent( CEEvent & eventElem );

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
    virtual bool RunDispatcher( void );

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
     *          Otherwise returns NULL
     **/
    virtual CEDispatcherThread * GetEventConsumerThread( const CERuntimeClassID & whichClass );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Binding (master) component object
     **/
    CEComponent &               mBindingComponent;

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
    inline CEWorkerThread & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    CEWorkerThread( void );
    CEWorkerThread(const CEWorkerThread & /*src*/ );
    const CEWorkerThread& operator = (const CEWorkerThread & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// CEWorkerThread class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline CEComponent& CEWorkerThread::GetBindingComponent( void ) const
{   return static_cast<CEComponent &>(mBindingComponent);    }

inline CEWorkerThread& CEWorkerThread::self( void )
{   return (*this); }

#endif  // AREG_COMPONENT_CEWORKERTHREAD_HPP
