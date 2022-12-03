#ifndef AREG_COMPONENT_PRIVATE_WATCHDOGMANAGER_HPP
#define AREG_COMPONENT_PRIVATE_WATCHDOGMANAGER_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/component/private/WatchdogManager.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Thread watchdog object declaration
 *
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/private/TimerManagerBase.hpp"
#include "areg/component/private/TimerManagerEvent.hpp"

#include "areg/component/private/Watchdog.hpp"

class WatchdogManager   : protected TimerManagerBase
{
//////////////////////////////////////////////////////////////////////////
// Predefined constants and types
//////////////////////////////////////////////////////////////////////////    
private:
    /**
     * \brief   WatchdogManager::WATCHDOG_THREAD_NAME
     *          Watchdog Manager thread name
     **/
    static constexpr std::string_view WATCHDOG_THREAD_NAME { "_AREG_WATCHDOG_THREAD_NAME_" };

    using MapWatchdogResource   = TEMap<Watchdog::GUARD_ID, Watchdog*>;
    using WatchdogResource      = TELockResourceMap<Watchdog::GUARD_ID, Watchdog, MapWatchdogResource>;

//////////////////////////////////////////////////////////////////////////
// Static members
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************/
// Private statics. Hidden
/************************************************************************/

    /**
     * \brief   Returns reference to Watchdog Manager object.
     **/
    static WatchdogManager& getInstance( void );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// Public statics
/************************************************************************/

    /**
     * \brief   If needed, creates Watchdog Manager and Thread.
     *          Returns reference to the object.
     **/
    static bool startWatchdogManager( void );

    /**
     * \brief   Stops Watchdog Manager and destroys Watchdog Thread.
     **/
    static void stopWatchdogManager( void );

    /**
     * \brief   Returns true if Watchdog Manager has been started and ready to process Watchdogs.
     **/
    static bool isWatchdogManagerStarted( void );

    /**
     * \brief   Starts the timer. If succeeds, returns true.
     *          When timer event is fired, it will be dispatched in the
     *          thread where it was started, i.e. in the current thread.
     * \param   timer   The timer object that should be started
     * \return  Returns true if timer was successfully created.
     **/
    static bool startTimer(Watchdog& watchdog);

    /**
     * \brief   Stops the timer. Returns true if timer successfully was stopped.
     * \param   timer   The timer object that should be stopped
     * \return  Returns true if timer was successfully stopped.
     **/
    static void stopTimer(Watchdog& watchdog);

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Constructor
     **/
    WatchdogManager( void );
    /**
     * \brief   Destructor
     **/
    virtual ~WatchdogManager( void );

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
// Hidden operations. Called from Watchdog Thread.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Called when expired timers should be processed.
     **/
    void _processExpiredTimer(Watchdog* watchdog, Watchdog::WATCHDOG_ID watchdogId, uint32_t hiBytes, uint32_t loBytes);

    /**
     * \brief   Stops and removes all watchdog timers.
     **/
    void _removeAllWatchdogs( void );

    /**
     * \brief   Creates system timer for watchdog and registers it in the resource map.
     * \param   watchdog       The Watchdog object that should be registered.
     **/
    inline void _registerWatchdog( Watchdog & watchdog);

    /**
     * \brief   Stop watchdog timer and unregister from the resource map.
     * \param   watchdog   The instance of watchdog object to unregister.
     **/
    inline void _unregisterWatchdog( Watchdog & Watchdog );

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
    static void _windowsWatchdogExpiredRoutine( void * argPtr, unsigned long timerLowValue, unsigned long timerHighValue );

#endif // _WINDOWS

#ifdef _POSIX

    /**
     * \brief   POSIX timer routine function. Triggered, when one of timer is expired.
     * \param   argSig          The value passed to thread signal when the timer was created.
     *                          This value is passed to routine callback.
     **/
    static void _posixWatchdogExpiredRoutine( union sigval argSig );

#endif // _POSIX

    /**
     * \brief   Starts system Watchdog and returns true if Watchdog started with success.
     * \param   WatchdogInfo   The Watchdog information object
     * \return  Returns true if system Watchdog started with success.
     **/
    static bool _osSystemTimerStart( Watchdog & watchdog );

    /**
     * \brief   Stops previously started waitable timer.
     * \param   timerHandle The waitable timer handle to destroy.
     **/
    static void _osSystemTimerStop(TIMERHANDLE handle);

//////////////////////////////////////////////////////////////////////////
//  Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The Watchdog table object.
     **/
    WatchdogResource    mWatchdogResource;

//////////////////////////////////////////////////////////////////////////
//  Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( WatchdogManager );

};

#endif  // AREG_COMPONENT_PRIVATE_WATCHDOGMANAGER_HPP
