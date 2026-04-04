#ifndef AREG_COMPONENT_PRIVATE_WATCHDOGMANAGER_HPP
#define AREG_COMPONENT_PRIVATE_WATCHDOGMANAGER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/WatchdogManager.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Thread watchdog object declaration
 *
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/component/private/TimerManagerBase.hpp"
#include "areg/component/private/TimerManagerEvent.hpp"

#include "areg/component/private/Watchdog.hpp"


/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg::os {
    class TimerPosix;
}

namespace areg {

/**
 * \brief   Singleton that manages watchdog timers for monitoring thread health and restarting
 *          unresponsive components.
 **/
class WatchdogManager final : protected TimerManagerBase
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

    using MapWatchdogResource   = OrderedMap<Watchdog::GUARD_ID, Watchdog *>;
    using WatchdogResource      = ConcurrentResourceMap<Watchdog::GUARD_ID, Watchdog *, MapWatchdogResource>;

//////////////////////////////////////////////////////////////////////////
// Static members
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************/
// Private statics. Hidden
/************************************************************************/

    /**
     * \brief   Returns the watchdog manager singleton instance.
     **/
    static WatchdogManager& instance();

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// Public statics
/************************************************************************/

    /**
     * \brief   Creates and starts the watchdog manager and thread.
     *
     * \return  Returns true if started successfully.
     **/
    static bool start_watchdog_manager();

    /**
     * \brief   Stops the watchdog manager and cancels all timers.
     *
     * \param   waitComplete    If true, waits for completion; if false, triggers exit and returns
     *                          immediately.
     **/
    static void stop_watchdog_manager( bool waitComplete);

    /**
     * \brief   Blocks until watchdog manager completes and exits.
     **/
    static void wait_watchdog_manager();

    /**
     * \brief   Returns true if watchdog manager is started and ready.
     **/
    [[nodiscard]]
    static bool is_manager_started();

    /**
     * \brief   Starts a watchdog timer.
     *
     * \param   watchdog    The watchdog object to start.
     * \return  Returns true if timer was created successfully.
     **/
    static bool start_timer(Watchdog& watchdog);

    /**
     * \brief   Stops a watchdog timer.
     *
     * \param   watchdog    The watchdog object to stop.
     **/
    static void stop_timer(Watchdog& watchdog);

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
private:
    WatchdogManager();
    virtual ~WatchdogManager();

//////////////////////////////////////////////////////////////////////////
// Overrides.
//////////////////////////////////////////////////////////////////////////
protected:
/**
**********************************************************************/
// IETimerManagingEventConsumer overrides
/************************************************************************/

    /**
     * \brief   Processes timer events dispatched by the timer thread.
     *
     * \param   data    The timer event data.
     **/
    void process_event( const TimerManagerEventData & data) final;

/************************************************************************/
// DispatcherThread overrides
/************************************************************************/

    /**
     * \brief   Enables or disables event dispatching. Override to perform preparation.
     *
     * \param   is_ready    True to enable; false to disable.
     **/
    void ready_for_events( bool is_ready ) final;

//////////////////////////////////////////////////////////////////////////
// Hidden operations. Called from Watchdog Thread.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Processes expired watchdog timers.
     *
     * \param   watchdog        The watchdog that expired.
     * \param   watchdog_id     The watchdog ID.
     * \param   hiBytes         High bytes of expiration time.
     * \param   loBytes         Low bytes of expiration time.
     **/
    void _process_expired_timer(Watchdog* watchdog, Watchdog::WATCHDOG_ID watchdog_id, uint32_t hiBytes, uint32_t loBytes);

    /**
     * \brief   Stops and removes all watchdog timers.
     **/
    void _remove_all_watchdogs();

    /**
     * \brief   Creates system timer for watchdog and registers in resource map.
     *
     * \param   watchdog    The watchdog to register.
     **/
    inline void _register_watchdog( Watchdog & watchdog);

    /**
     * \brief   Stops watchdog timer and unregisters from resource map.
     *
     * \param   watchdog    The watchdog to unregister.
     **/
    inline void _unregister_watchdog( Watchdog & watchdog );

//////////////////////////////////////////////////////////////////////////
//  OS specific hidden methods
//////////////////////////////////////////////////////////////////////////
private:

#ifdef _WIN32

    /**
     * \brief   Windows timer callback when watchdog expires.
     *
     * \param   argPtr              Timer callback argument pointer.
     * \param   timerLowValue       Low value of expiration time.
     * \param   timerHighValue      High value of expiration time.
     **/
    static void _windows_watchdog_expired( void * argPtr, unsigned long timerLowValue, unsigned long timerHighValue ) noexcept;

#endif // _WIN32

#if defined(_POSIX) || defined(POSIX)

#ifdef __APPLE__
    /**
     * \brief   macOS GCD timer callback when watchdog expires.
     *
     * \param   timerPtr    Pointer to the expired timer.
     **/
    static void _posix_watchdog_expired( areg::os::TimerPosix* timerPtr ) noexcept;
#elif defined(__linux__)
    /**
     * \brief   Called by the epoll loop when the watchdog timerfd becomes readable.
     *          Looks up the Watchdog via context_id and dispatches the expiry.
     *
     * \param   handle  OS timer handle (TimerPosix*) that fired.
     **/
    void _on_timerfd_expired(TIMERHANDLE handle) final;
#elif defined(_POSIX) || defined(POSIX)
    /**
     * \brief   Generic POSIX SIGEV_THREAD callback triggered when a watchdog expires.
     *          Called with the TimerPosix pointer cast to void*.
     *
     * \param   timerPtr    Pointer to the expired areg::os::TimerPosix object (as void*).
     **/
    static void _posix_watchdog_expired( void * timerPtr ) noexcept;
#endif  // __APPLE__ / __linux__ / POSIX

#endif // defined(_POSIX) || defined(POSIX)

    /**
     * \brief   Starts OS system watchdog timer.
     *
     * \param   watchdog    The watchdog with timer information.
     * \return  Returns true if timer started successfully.
     **/
    static bool _os_timer_start( Watchdog & watchdog );

    /**
     * \brief   Stops OS system watchdog timer.
     *
     * \param   handle      The timer handle to stop.
     **/
    static void _os_timer_stop(TIMERHANDLE handle);

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
    AREG_NOCOPY_NOMOVE( WatchdogManager );

};

} // namespace areg
#endif  // AREG_COMPONENT_PRIVATE_WATCHDOGMANAGER_HPP
