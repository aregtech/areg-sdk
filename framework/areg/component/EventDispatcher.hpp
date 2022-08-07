#ifndef AREG_COMPONENT_EVENTDISPATCHER_HPP
#define AREG_COMPONENT_EVENTDISPATCHER_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/component/EventDispatcher.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Event Dispatcher class
 *              Posts, receives dispatcher event objects, registers
 *              event consumers in dispatcher registry and triggers
 *              appropriate event processing function of event consumer.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/IEThreadConsumer.hpp"
#include "areg/component/IEEventRouter.hpp"
#include "areg/component/private/EventDispatcherBase.hpp"

//////////////////////////////////////////////////////////////////////////
// EventDispatcher class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Event Dispatcher is a component of Dispatcher Thread to queue, 
 *          dispatch and process events in the thread. The dispatcher thread runs
 *          in the loop until gets exit event in the queue of Event Dispatcher.
 *          Do not make long processing and run methods in the long loop, 
 *          because this will delay event processing.
 **/
class AREG_API EventDispatcher  : public    EventDispatcherBase
                                , public    IEThreadConsumer
                                , public    IEEventRouter
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initialization constructor.
     * \param   name    The name of Event Dispatcher
     **/
    explicit EventDispatcher( const String & name );
    /**
     * \brief   Destructor.
     **/
    virtual ~EventDispatcher( void );

//////////////////////////////////////////////////////////////////////////
// Override operations
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// IEThreadConsumer interface overrides
/************************************************************************/

    /**
     * \brief	Function triggered with thread object has been created.
     *          If this function returns true, thread will continue running.
     *          If this function returns false, the thread will not run.
     *          This function might be triggered from different thread
     *          than function run() is called.
     * \param	threadObj	The new created Thread object, 
     *                      which contains this consumer.
     * \return	Return true if thread should run. Return false, it should not run.
     **/
    virtual bool onThreadRegistered( Thread * threadObj ) override;

    /**
     * \brief   Function is triggered from thread object when it is going to be destroyed.
     **/
    virtual void onThreadUnregistering( void ) override;

    /**
     * \brief   Function is called from Thread object, when it is running and fully operable.
     **/
    virtual void onThreadRuns( void ) override;

    /**
     * \brief   Function is called from Thread object when it is going to exit.
     * \return  Return thread exit error code.
     **/
    virtual int onThreadExit( void ) override;

/************************************************************************/
// IEEventRouter interface overrides
/************************************************************************/

    /**
     * \brief	Posts event and delivers to its target thread / process.
     * \param	eventElem	Event object to post.
     * \return	Returns true if target was found and the event
     *          delivered with success. Otherwise it returns false.
     **/
    virtual bool postEvent(Event& eventElem) override;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Return pointer to Dispatcher Thread where current dispatcher
     *          is registered.
     **/
    inline DispatcherThread * getDispatcherThread( void ) const;

protected:
    /**
     * \brief   Returns true, if dispatcher has more queued events.
     *          Here, only external events are counted, since
     *          internal events are proceed immediately after external event.
     **/
    inline bool hasMoreEvents( void ) const;

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Pointer to dispatcher thread, which is holding dispatcher.
     *          The pointer is set after thread has been created and reset
     *          when it is destroyed.
     **/
    DispatcherThread *  mDispatcherThread;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls.
//////////////////////////////////////////////////////////////////////////
private:
    EventDispatcher( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( EventDispatcher );
};

//////////////////////////////////////////////////////////////////////////
// DispatcherThread class inline functions implementation
//////////////////////////////////////////////////////////////////////////
inline DispatcherThread * EventDispatcher::getDispatcherThread( void ) const
{
    return mDispatcherThread;
}

inline bool EventDispatcher::hasMoreEvents( void ) const
{
    return (static_cast<const EventQueue &>(mExternaEvents).getSize() > 0);
}

#endif  // AREG_COMPONENT_EVENTDISPATCHER_HPP
