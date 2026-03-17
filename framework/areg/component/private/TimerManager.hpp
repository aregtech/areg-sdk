#ifndef AREG_COMPONENT_PRIVATE_TIMERMANAGER_HPP
#define AREG_COMPONENT_PRIVATE_TIMERMANAGER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/TimerManager.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, The System Timer Manager.
 *              Controlling, triggering and stopping timer.
 *
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/component/private/TimerManagerBase.hpp"

#include "areg/base/SyncPrimitives.hpp"
#include "areg/base/ResourceMap.hpp"

#if defined(_POSIX) || defined(POSIX)
    #ifndef __APPLE__
        using signal_value = union sigval;
    #endif  // __APPLE__
#endif  // defined(_POSIX) || defined(POSIX)
   
/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {
    class Timer;
} // namespace areg

namespace areg::os {
    class TimerPosix;
} // namespace areg::os

namespace areg {

//////////////////////////////////////////////////////////////////////////
// TimerManager class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Manages timers and generates timer events to target consumers. Singleton pattern.
 *          Creates system timers and delivers timer events to the consumer's owner thread.
 **/
class TimerManager final    : protected TimerManagerBase
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

    using MapTimerResource  = HashMap<TIMERHANDLE, Timer *>;
    using TimerResource     = ConcurrentResourceMap<TIMERHANDLE, Timer *, MapTimerResource>;

//////////////////////////////////////////////////////////////////////////
// Static members
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************/
// Private statics. Hidden
/************************************************************************/

    /**
     * \brief   Returns a reference to the TimerManager singleton instance.
     **/
    static TimerManager & instance();

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// Public statics
/************************************************************************/

    /**
     * \brief   Creates the TimerManager singleton and its timer thread if not already running.
     *
     * \return  Returns true if the timer manager was started.
     **/
    static bool start_timer_manager();

    /**
     * \brief   Stops the TimerManager and cancels all timers.
     *
     * \param   waitComplete    If true, blocks until the timer manager completes and exits. If
     *                          false, triggers the exit and returns immediately.
     **/
    static void stop_timer_manager(bool waitComplete);

    /**
     * \brief   Blocks until the TimerManager exits (call after stop_timer_manager with
     *          waitComplete=false).
     **/
    static void wait_timer_manager();

    /**
     * \brief   Returns true if the TimerManager is running and ready.
     **/
    [[nodiscard]]
    static bool is_manager_started();

    /**
     * \brief   Starts a timer. The timer event will be dispatched in the calling thread.
     *
     * \param   timer       The timer object to start.
     * \return  Returns true if the timer was successfully created.
     **/
    static bool start_timer(Timer &timer);

    /**
     * \brief   Starts a timer and specifies the target thread for the timer event.
     *
     * \param   timer           The timer object to start.
     * \param   whichThread     The dispatcher thread where the timer event should be delivered.
     * \return  Returns true if the timer was successfully created.
     **/
    static bool start_timer(Timer &timer, const DispatcherThread & whichThread);

    /**
     * \brief   Stops a running timer.
     *
     * \param   timer       The timer object to stop.
     **/
    static void stop_timer(Timer &timer);

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
private:
    TimerManager();
    virtual ~TimerManager();

//////////////////////////////////////////////////////////////////////////
// Overrides.
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// IETimerManagingEventConsumer overrides
/************************************************************************/

    /**
     * \brief   Processes timer manager events dispatched by the timer thread.
     *
     * \param   data    The timer manager event data.
     **/
    void process_event( const TimerManagerEventData & data) final;

/************************************************************************/
// DispatcherThread overrides
/************************************************************************/

    /**
     * \brief   Enables or disables event dispatching to the timer manager.
     *
     * \param   is_ready    If true, the dispatcher is ready to receive events. If false, event
     *                      dispatching is disabled.
     **/
    void ready_for_events( bool is_ready ) final;

//////////////////////////////////////////////////////////////////////////
// Hidden operations. Called from Timer Thread.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Processes expired timers and generates timer events.
     *
     * \param   timer       The expired timer object.
     * \param   handle      The timer handle.
     * \param   hiBytes     The high 32 bits of the expiration time.
     * \param   loBytes     The low 32 bits of the expiration time.
     **/
    void _process_expired_timer(Timer * timer, TIMERHANDLE handle, uint32_t hiBytes, uint32_t loBytes);

    /**
     * \brief   Stops and removes all registered timers.
     **/
    void _remove_all_timers();

    /**
     * \brief   Registers a timer in the timer map after creating the system timer.
     *
     * \param   timer           The timer object to register.
     * \param   whichThread     The dispatcher thread where the timer event should be delivered.
     * \return  Returns true if the timer was successfully registered.
     * \note    Timers must be unique in the timer map.
     **/
    bool _register_timer( Timer & timer, const DispatcherThread & whichThread );

    /**
     * \brief   Unregisters and stops a timer, closing its system handle.
     *
     * \param   timer       The timer object to unregister.
     **/
    void _unregister_timer( Timer & timer );

//////////////////////////////////////////////////////////////////////////
//  OS specific hidden methods
//////////////////////////////////////////////////////////////////////////
private:

#ifdef _WIN32

    /**
     * \brief   Windows timer callback function triggered when a timer expires.
     *
     * \param   argPtr              Argument pointer passed to the timer callback.
     * \param   timerLowValue       Low 32 bits of the expiration time.
     * \param   timerHighValue      High 32 bits of the expiration time.
     **/
    static void _windows_timer_expired( void * argPtr, unsigned long timerLowValue, unsigned long timerHighValue ) noexcept;

#endif // !_WIN32


#if defined(_POSIX) || defined(POSIX)

#ifdef __APPLE__
    /**
     * \brief   macOS timer callback function triggered when a timer expires.
     *
     * \param   timerPtr    Pointer to the expired areg::os::TimerPosix object.
     **/
    static void _posix_timer_expired( areg::os::TimerPosix* timerPtr );
#else   // !__APPLE__
    /**
     * \brief   POSIX timer callback function triggered when a timer expires.
     *
     * \param   argSig      Signal value passed when the timer was created, containing the timer
     *                      pointer.
     **/
    static void _posix_timer_expired( signal_value argSig );
#endif  // __APPLE__

#endif  // defined(_POSIX) || defined(POSIX)

    /**
     * \brief   Starts a system-level timer and returns true if successful.
     *
     * \param   timer       The timer object to start at the OS level.
     * \return  Returns true if the system timer started successfully.
     **/
    static bool _os_timer_start( Timer& timer );

    /**
     * \brief   Stops a system timer and closes its handle.
     *
     * \param   timerHandle     The handle of the waitable timer to stop and destroy.
     **/
    static void _os_timer_stop( TIMERHANDLE timerHandle );

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
    AREG_NOCOPY_NOMOVE( TimerManager );
};

} // namespace areg
#endif  // AREG_COMPONENT_PRIVATE_TIMERMANAGER_HPP
