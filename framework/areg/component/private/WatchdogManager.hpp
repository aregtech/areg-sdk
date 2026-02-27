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
#include "areg/base/GEGlobal.h"
#include "areg/component/private/TimerManagerBase.hpp"
#include "areg/component/private/TimerManagerEvent.hpp"

#include "areg/component/private/Watchdog.hpp"
#include <signal.h>

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg::os { class TimerPosix; }

namespace areg
{
    class WatchdogManager   : protected areg::TimerManagerBase
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

        using MapWatchdogResource   = areg::OrderedMap<areg::Watchdog::GUARD_ID, areg::Watchdog *>;
        using WatchdogResource      = areg::ConcurrentResourceMap<areg::Watchdog::GUARD_ID, areg::Watchdog *, MapWatchdogResource>;

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
        static WatchdogManager& getInstance();

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
        static bool startWatchdogManager();

        /**
         * \brief   Stops Watchdog Manager and the Thread. Cancels and stops all timers.
         *          If 'waitComplete' is set to true, the calling thread is
         *          blocked until Watchdog Manager completes jobs and cleans resources.
         *          Otherwise, this triggers stop and exit events, and immediately returns.
         * \param   waitComplete    If true, waits for Watchdog Manager to complete the jobs
         *                          and exit threads. Otherwise, it triggers exit and returns.
         **/
        static void stopWatchdogManager( bool waitComplete);

        /**
         * \brief   The calling thread is blocked until Watchdog Manager did not
         *          complete the jobs and exit. This should be called if previously
         *          it was requested to stop the Watchdog Manager without waiting for completion.
         **/
        static void waitWatchdogManager();

        /**
         * \brief   Returns true if Watchdog Manager has been started and ready to process Watchdogs.
         **/
        static bool isWatchdogManagerStarted();

        /**
         * \brief   Starts the watchdog timer. If succeeds, returns true.
         *          When timer event is fired, it will be dispatched in the
         *          thread where it was started, i.e. in the current thread.
         * \param   watchdog    The watchdog object that should be started.
         * \return  Returns true if timer was successfully created.
         **/
        static bool startTimer(areg::Watchdog& watchdog);

        /**
         * \brief   Stops the watchdog timer. Returns true if timer successfully was stopped.
         * \param   watchdog    The watchdog object that should be stopped.
         **/
        static void stopTimer(areg::Watchdog& watchdog);

    //////////////////////////////////////////////////////////////////////////
    // Constructor / Destructor
    //////////////////////////////////////////////////////////////////////////
    private:
        /**
         * \brief   Constructor
         **/
        WatchdogManager();
        /**
         * \brief   Destructor
         **/
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
         * \brief   Automatically triggered when event is dispatched by timer thread
         * \param   data    The data object passed in event.
         **/
        void processEvent( const areg::TimerManagerEventData & data) override;

    /************************************************************************/
    // DispatcherThread overrides
    /************************************************************************/

        /**
         * \brief   Call to enable or disable event dispatching threads to receive events.
         *          Override if need to make event dispatching preparation job.
         * \param   isReady     The flag to indicate whether the dispatcher is ready for events.
         **/
        void readyForEvents( bool isReady ) override;

    //////////////////////////////////////////////////////////////////////////
    // Hidden operations. Called from Watchdog Thread.
    //////////////////////////////////////////////////////////////////////////
    private:
        /**
         * \brief   Called when expired timers should be processed.
         **/
        void _processExpiredTimer(areg::Watchdog* watchdog, areg::Watchdog::WATCHDOG_ID watchdogId, uint32_t hiBytes, uint32_t loBytes);

        /**
         * \brief   Stops and removes all watchdog timers.
         **/
        void _removeAllWatchdogs();

        /**
         * \brief   Creates system timer for watchdog and registers it in the resource map.
         * \param   watchdog       The Watchdog object that should be registered.
         **/
        inline void _registerWatchdog( areg::Watchdog & watchdog);

        /**
         * \brief   Stop watchdog timer and unregister from the resource map.
         * \param   watchdog   The instance of watchdog object to unregister.
         **/
        inline void _unregisterWatchdog( areg::Watchdog & watchdog );

    //////////////////////////////////////////////////////////////////////////
    //  OS specific hidden methods
    //////////////////////////////////////////////////////////////////////////
    private:

    #ifdef _WIN32

        /**
         * \brief   Windows OS specific timer routine function. Triggered, when one of timer is expired.
         * \param   argPtr          The pointer of argument passed to timer expired callback function
         * \param   timerLowValue   The low value of timer expiration
         * \param   timerHighValue  The high value of timer expiration.
         **/
        static void _windowsWatchdogExpiredRoutine( void * argPtr, unsigned long timerLowValue, unsigned long timerHighValue );

    #endif // _WIN32

    #if defined(_POSIX) || defined(POSIX)

    #ifdef __APPLE__
        /**
         * \brief   macOS timer callback function. Triggered when one of watchdog timers is expired.
         * \param   timerPtr        The pointer to the TimerPosix object that expired.
         **/
        static void _posixWatchdogExpiredRoutine( areg::os::TimerPosix* timerPtr );
    #else   // !__APPLE__
        /**
         * \brief   POSIX timer routine function. Triggered, when one of timer is expired.
         * \param   argSig          The value passed to thread signal when the timer was created.
         *                          This value is passed to routine callback.
         **/
        static void _posixWatchdogExpiredRoutine( union ::sigval argSig );
    #endif  // __APPLE__

    #endif // defined(_POSIX) || defined(POSIX)

        /**
         * \brief   Starts system Watchdog and returns true if Watchdog started with success.
         * \param   watchdog    The Watchdog  object with timer information.
         * \return  Returns true if system Watchdog started with success.
         **/
        static bool _osSystemTimerStart( areg::Watchdog & watchdog );

        /**
         * \brief   Stops previously started waitable timer.
         * \param   handle      The waitable timer handle to destroy.
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
        AREG_NOCOPY_NOMOVE( WatchdogManager );

    };

} // namespace areg
#endif  // AREG_COMPONENT_PRIVATE_WATCHDOGMANAGER_HPP
