#ifndef AREG_COMPONENT_PRIVATE_TIMERMANAGERBASE_HPP
#define AREG_COMPONENT_PRIVATE_TIMERMANAGERBASE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/component/private/TimerManagerBase.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, The System Timer Manager Base class.
 *
 ************************************************************************/

 /************************************************************************
  * Include files
  ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/CommonDefs.hpp"
#include "areg/component/DispatcherThread.hpp"
#include "areg/component/private/TimerManagerEvent.hpp"

#if !defined(__linux__) && !defined(__APPLE__) && (defined(_POSIX) || defined(POSIX))
    #include <time.h>
#endif  // generic POSIX

namespace areg {

/************************************************************************
 * Dependencies
 ************************************************************************/
class TimerBase;

//////////////////////////////////////////////////////////////////////////
// TimerManager class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Base class to manage system timers and process event messages. Extended by Timer Manager
 *          and Watchdog Manager. Must run as separate thread.
 **/
class TimerManagerBase  : protected DispatcherThread
                        , protected TimerManagerEventConsumer
{
//////////////////////////////////////////////////////////////////////////
// Runtime declaration
//////////////////////////////////////////////////////////////////////////
    AREG_DECLARE_RUNTIME(TimerManagerBase)

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Creates Timer Manager Base with specified thread name.
     *
     * \param   threadName      The name of the timer manager thread.
     * \param   stackSizeKb     The stack size for the timer manager thread in kilobytes.
     **/
    explicit TimerManagerBase( const String & threadName, uint32_t stackSizeKb );
    virtual ~TimerManagerBase() = default;

//////////////////////////////////////////////////////////////////////////
// Overrides.
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// DispatcherThread overrides
/************************************************************************/

    /**
     * \brief   Posts event and delivers to its target thread or process.
     *
     * \param   eventElem       Event object to post.
     * \return  Returns true if target was found and the event delivered successfully. Otherwise
     *          returns false.
     **/
    bool post_event( Event & eventElem ) override;

    /**
     * \brief   Enables or disables event dispatching threads to receive events. Override if event
     *          dispatching preparation is needed.
     *
     * \param   is_ready    The flag to indicate whether the dispatcher is ready for events.
     **/
    void ready_for_events( bool is_ready ) override;

    /**
     * \brief   Runs main dispatching loop to pick and dispatch events. Override if logic should be
     *          changed.
     *
     * \return  Returns true if Exit Event is signaled.
     **/
    bool run_dispatcher() final;

    /**
     * \brief   Starts Timer Manager Thread if not already started.
     *
     * \return  Returns true if Timer Manager Thread is started and ready to process events.
     **/
    bool start_manager_thread();

    /**
     * \brief   Stops Timer Manager Thread and cancels all timers. Optionally waits for completion.
     *
     * \param   waitComplete    If true, waits for Timer Manager Thread to complete jobs and exit.
     *                          Otherwise triggers exit and returns immediately.
     **/
    void stop_manager_thread( bool waitComplete );

    /**
     * \brief   Waits for thread completion without sending exit message or terminating the thread.
     *          Returns true if thread completes normally or if waiting timeout is DO_NOT_WAIT.
     *
     * \param   waitForCompleteMs       The timeout in milliseconds to wait for completion.
     * \return  Returns true if either thread completed or the waiting timeout is
     *          areg::DO_NOT_WAIT.
     **/
    bool wait_completion( uint32_t waitForCompleteMs = areg::WAIT_INFINITE ) override;

//////////////////////////////////////////////////////////////////////////
// Hidden operations. Called from Timer Thread.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns reference to this Timer Manager object.
     **/
    [[nodiscard]]
    inline TimerManagerBase & self() noexcept;

//////////////////////////////////////////////////////////////////////////
// Linux timerfd / epoll support (non-Apple POSIX only)
//////////////////////////////////////////////////////////////////////////
#if defined(__linux__)

    /**
     * \brief   Called from the epoll loop when the timerfd associated with
     *          handle becomes readable (i.e., the timer has fired). Subclasses
     *          implement this to look up the corresponding timer/watchdog object
     *          and call the appropriate expired-timer handler.
     *
     * \param   handle  OS timer handle (pointer to TimerPosix object) that fired.
     **/
    virtual void _on_timerfd_expired(TIMERHANDLE handle) = 0;

protected:
    /**
     * \brief   epoll file descriptor that watches all timerfd handles plus the
     *          command and exit eventfd handles. Value -1 when not initialized.
     **/
    int     mEpollFd;

    /**
     * \brief   eventfd written by post_event() to wake the epoll loop when a
     *          timer-management event (start / stop) has been queued.
     *          Value -1 when not initialized.
     **/
    int     mCommandFd;

    /**
     * \brief   eventfd written by stop_manager_thread() to signal the epoll loop
     *          to exit cleanly.  Value -1 when not initialized.
     **/
    int     mExitFd;

#endif  // defined(__linux__)

//////////////////////////////////////////////////////////////////////////
// Generic POSIX deadline support (every POSIX platform but Linux and macOS)
//////////////////////////////////////////////////////////////////////////
#if !defined(__linux__) && !defined(__APPLE__) && (defined(_POSIX) || defined(POSIX))

private:
    /**
     * \brief   Fires every armed timer that has reached its due time and reports when the
     *          dispatching loop has to wake up next. Called on the manager thread only.
     *
     *          This platform keeps no OS timer object at all: the deadlines live in the
     *          manager and the loop waits for the nearest one, the same way Linux waits on
     *          its timerfd set. Nothing outside areg runs a timer callback and nothing
     *          outside areg takes a timer lock, so no foreign thread can be destroyed while
     *          holding one. See posix/TimerManagerBasePosix.cpp.
     *
     * \param   now             The current time, read from the deadline clock.
     * \param   out_nextDue     On return, the earliest due time that is still armed.
     * \return  Returns true when at least one timer stays armed, so out_nextDue is set.
     **/
    virtual bool _check_deadlines( const timespec & now, timespec & out_nextDue ) = 0;

#endif  // generic POSIX

//////////////////////////////////////////////////////////////////////////
//  Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    TimerManagerBase() = delete;
    AREG_NOCOPY_NOMOVE( TimerManagerBase );
};

//////////////////////////////////////////////////////////////////////////
// TimerManager class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline TimerManagerBase& TimerManagerBase::self() noexcept
{
    return (*this);
}

} // namespace areg
#endif  // AREG_COMPONENT_PRIVATE_TIMERMANAGERBASE_HPP
