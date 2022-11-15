#ifndef AREG_COMPONENT_PRIVATE_TIMERMANAGERBASE_HPP
#define AREG_COMPONENT_PRIVATE_TIMERMANAGERBASE_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/component/private/TimerManagerBase.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, The System Timer Manager Base class.
 *
 ************************************************************************/

 /************************************************************************
  * Include files
  ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/DispatcherThread.hpp"
#include "areg/component/private/TimerManagerEvent.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class TimerBase;

//////////////////////////////////////////////////////////////////////////
// TimerManager class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The Time Manager Base class to start and stop system timers,
 *          and process event messages. The base class is extended by
 *          Timer Manager and Watchdog Manager to execute specific tasks.
 *          It requires to run as separate thread.
 **/
class TimerManagerBase  : protected DispatcherThread
                        , protected IETimerManagerEventConsumer
{
//////////////////////////////////////////////////////////////////////////
// Runtime declaration
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME(TimerManagerBase)

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   protected Constructor / destructor
     **/
    TimerManagerBase( const String & threadName );
    virtual ~TimerManagerBase( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Overrides.
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// DispatcherThread overrides
/************************************************************************/

    /**
     * \brief	Posts event and delivers to its target thread / process.
     * \param	eventElem	Event object to post.
     * \return	Returns true if target was found and the event
     *          delivered with success. Otherwise it returns false.
     **/
    virtual bool postEvent( Event & eventElem ) override;

    /**
     * \brief	Triggered when dispatcher starts running. 
     *          In this function runs main dispatching loop.
     *          Events are picked and dispatched here.
     *          Override if logic should be changed.
     * \return	Returns true if Exit Event is signaled.
     **/
    virtual bool runDispatcher( void ) override;

    /**
     * \brief   Triggered before dispatcher starts to dispatch events and when event dispatching just finished.
     * \param   hasStarted  The flag to indicate whether the dispatcher is ready for events.
     **/
    virtual void readyForEvents( bool isReady ) override;

    /**
     * \brief   Starts Timer Manager Thread it is not started yet.
     * \return  Returns true if Timer Manager Thread is started and ready to process events.
     **/
    bool startTimerManagerThread( void );

    /**
     * \brief   Stops Timer Manager Thread and waits until it exists.
     **/
    void stopTimerManagerThread( void );

//////////////////////////////////////////////////////////////////////////
// Hidden operations. Called from Timer Thread.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns TimerManager object. for internal calls.
     **/
    inline TimerManagerBase & self( void );

//////////////////////////////////////////////////////////////////////////
//  Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    TimerManagerBase(void) = delete;
    DECLARE_NOCOPY_NOMOVE( TimerManagerBase );
};

//////////////////////////////////////////////////////////////////////////
// TimerManager class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline TimerManagerBase& TimerManagerBase::self( void )
{
    return (*this);
}

#endif  // AREG_COMPONENT_PRIVATE_TIMERMANAGERBASE_HPP
