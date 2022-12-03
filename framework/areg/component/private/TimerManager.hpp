#ifndef AREG_COMPONENT_PRIVATE_TIMERMANAGER_HPP
#define AREG_COMPONENT_PRIVATE_TIMERMANAGER_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/component/private/TimerManager.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, The System Timer Manager.
 *              Controlling, triggering and stopping timer.
 *
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/component/private/TimerManagerBase.hpp"

#include "areg/base/SynchObjects.hpp"
#include "areg/base/TEResourceMap.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class Timer;

//////////////////////////////////////////////////////////////////////////
// TimerManager class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       The Time Manager object is starting and stopping timers.
 *              It generates Timer Events and forwards to Target Consumer.
 *              The Timer Manager is a singleton object and created
 *              only once on the first request until it is not requested
 *              to be stopped.
 * 
 *          When the timer is requested to be started, 
 *          the timer Manager generates Timer Managing event object
 *          and places in the queue of timer thread. If the timer thread
 *              is resumed because it received timer managing event, it 
 *              forwards to Timer Manager, which creates system timer.
 *              If timer thread is resumed because one of timers is
 *              fired, it triggers expired function of Timer Manager
 *              and the object is generating Timer Event and sends to
 *              the queue of Timer Consumer Thread.
 *
 **/
class TimerManager  : protected TimerManagerBase
{

//////////////////////////////////////////////////////////////////////////
// Predefined constants and types
//////////////////////////////////////////////////////////////////////////    
private:
    /**
     * \brief   TimerManager::TIMER_THREAD_NAME
     *          Timer Manager thread name
     **/
    static constexpr std::string_view TIMER_THREAD_NAME { "_AREG_TIMER_THREAD_NAME_" };

    using MapTimerResource  = TEHashMap<TIMERHANDLE, Timer*>;
    using TimerResource     = TELockResourceMap<TIMERHANDLE, Timer, MapTimerResource>;

//////////////////////////////////////////////////////////////////////////
// Static members
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************/
// Private statics. Hidden
/************************************************************************/

    /**
     * \brief   Returns reference to Timer Manager object.
     **/
    static TimerManager & getInstance( void );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// Public statics
/************************************************************************/

    /**
     * \brief   If needed, creates Time Manager object and Timer Thread.
     *          Returns reference to the object.
     **/
    static bool startTimerManager( void );

    /**
     * \brief   Stops Timer Manager and destroys Timer Thread.
     **/
    static void stopTimerManager( void );

    /**
     * \brief   Returns true if Timer Manager has been started and ready to process timers.
     **/
    static bool isTimerManagerStarted( void );

    /**
     * \brief   Starts the timer. If succeeds, returns true.
     *          When timer event is fired, it will be dispatched in the
     *          thread where it was started, i.e. in the current thread.
     * \param   timer   The timer object that should be started
     * \return  Returns true if timer was successfully created.
     **/
    static bool startTimer(Timer &timer);

    /**
     * \brief   Starts the timer. If succeeds, returns true.
     *          The timer event will be dispatched in the specified
     *          thread context.
     * \param   timer       The timer object that should be started
     * \param   whichThread The dispatcher thread where the timer 
     *                      event should be dispatched.
     * \return  Returns true if timer was successfully created.
     **/
    static bool startTimer(Timer &timer, const DispatcherThread & whichThread);

    /**
     * \brief   Stops the timer.
     * \param   timer   The timer object that should be stopped
     **/
    static void stopTimer(Timer &timer);

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Constructor
     **/
    TimerManager( void );
    /**
     * \brief   Destructor
     **/
    virtual ~TimerManager( void );

//////////////////////////////////////////////////////////////////////////
// Overrides.
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// IETimerManagingEventConsumer overrides
/************************************************************************/

    /**
     * \brief   Automatically triggered when event is dispatched by timer thread
     * \param   data    The data object passed in event.
     **/
    virtual void processEvent( const TimerManagerEventData & data) override;

/************************************************************************/
// DispatcherThread overrides
/************************************************************************/

    /**
     * \brief   Triggered before dispatcher starts to dispatch events and when event dispatching just finished.
     * \param   hasStarted  The flag to indicate whether the dispatcher is ready for events.
     **/
    virtual void readyForEvents( bool isReady ) override;

//////////////////////////////////////////////////////////////////////////
// Hidden operations. Called from Timer Thread.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Called when expired timers should be processed.
     **/
    void _processExpiredTimer(Timer * timer, TIMERHANDLE handle, uint32_t hiBytes, uint32_t loBytes);

    /**
     * \brief   Stops and removes all timers, i.e. unregisters all timers.
     **/
    void _removeAllTimers( void );

    /**
     * \brief   Registers timer in the timer resource map.
     *          Before registering timer, it creates system timer.
     *          if successfully created, stores in the resource map.
     *          The timers should be unique in the timer map.
     * \param   timer       The timer object that should be registered
     * \param   whichThread The dispatcher thread, where the timer event should be dispatched.
     * \return  Returns true if succeeded to register timer in the map.
     **/
    bool _registerTimer( Timer & timer, const DispatcherThread & whichThread );

    /**
     * \brief   Registers timer in the timer resource map.
     *          Before registering timer, it creates system timer.
     *          if successfully created, stores in the resource map.
     *          The timers should be unique in the timer map.
     * \param   timer           The timer object that should be registered
     * \param   whichThreadId   The dispatcher thread, where the timer event should be dispatched.
     * \return  Returns true if succeeded to register timer in the map.
     **/
    bool _registerTimer( Timer & timer, id_type whichThreadId );

    /**
     * \brief   Unregisters timer manager in the timer resource map.
     *          Before unregistering timer, it stops and closes system timer.
     * \param   timer   The pointer to timer object that should be unregistered.
     **/
    void _unregisterTimer( Timer & timer );

//////////////////////////////////////////////////////////////////////////
//  OS specific hidden methods
//////////////////////////////////////////////////////////////////////////
private:

#ifdef _WINDOWS

    /**
     * \brief   Windows OS specific timer routine function. Triggered, when one of timer is expired.
     * \param   argPtr          The pointer of argument passed to timer expired callback function
     * \param   timerLowValue   The low value of timer expiration
     * \param   timerHighValue  The high value of timer expiration.
     **/
    static void _windowsTimerExpiredRoutine( void * argPtr, unsigned long timerLowValue, unsigned long timerHighValue );

#endif // !_WINDOWS


#ifdef _POSIX

    /**
     * \brief   POSIX timer routine function. Triggered, when one of timer is expired.
     * \param   argSig          The value passed to thread signal when the timer was created.
     *                          This value is passed to routine callback.
     **/
    static void _posixTimerExpiredRoutine( union sigval argSig );

#endif  // !_POSIX

    /**
     * \brief   Starts system timer and returns true if timer started with success.
     * \param   timerInfo   The timer information object
     * \return  Returns true if system timer started with success.
     **/
    static bool _osSystemTimerStart( Timer& timer );

    /**
     * \brief   Stops previously started waitable timer.
     * \param   timerHandle The waitable timer handle to destroy.
     **/
    static void _osSsystemTimerStop( TIMERHANDLE timerHandle );

//////////////////////////////////////////////////////////////////////////
//  Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Timer resource handler; 
     **/
    TimerResource	mTimerResource;

//////////////////////////////////////////////////////////////////////////
//  Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( TimerManager );
};

#endif  // AREG_COMPONENT_PRIVATE_TIMERMANAGER_HPP
