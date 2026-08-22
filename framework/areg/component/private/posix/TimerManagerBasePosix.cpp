/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/component/private/posix/TimerManagerBasePosix.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, the dispatching loop of the timer and the watchdog manager
 *              on a generic POSIX platform (Cygwin, FreeBSD, and any other POSIX that is
 *              neither Linux nor macOS). The loop keeps the deadlines itself, waits for the
 *              nearest one and processes the expiry on the manager thread.
 *
 *              Windows and macOS: TimerManagerBase.cpp
 *              Linux:             linux/TimerManagerBaseLinux.cpp
 ************************************************************************/

#if !defined(__linux__) && !defined(__APPLE__)
#if defined(_POSIX) || defined(POSIX)

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/component/private/TimerManagerBase.hpp"
#include "areg/component/private/posix/TimerPosix.hpp"
#include "areg/component/ExitEvent.hpp"

namespace areg {

bool TimerManagerBase::post_event(Event& eventElem)
{
    return (eventElem.event_id() == TimerManagerEvent::CLASS_ID) && EventDispatcher::post_event(eventElem);
}

bool TimerManagerBase::run_dispatcher()
{
    ready_for_events(true);

    bool isExit{ false };   // true once the ExitEvent is dequeued -> leave the loop

    do
    {
        // Drain all queued management events (start / stop timer) without waiting between them.
        for (;;)
        {
            Event eventElem = pick_event();

            if (eventElem.is_exit_prio())
            {
                isExit = true;
                break;
            }

            if (!eventElem.is_valid())
            {
                break;
            }

            if (prepare_dispatch_event(eventElem))
            {
                dispatch_event(eventElem);
            }

            post_dispatch_event(eventElem);

            ASSERT(mInternalEvents.is_empty());
        }

        if (isExit)
            break;

        timespec now{};
        areg::os::deadline_now(now);

        timespec nextDue{};
        const bool armed{ _check_deadlines(now, nextDue) };

        uint32_t waitMs{ areg::WAIT_INFINITE };
        if (armed)
        {
            const uint32_t remain{ areg::os::deadline_remaining_ms(now, nextDue) };
            waitMs = remain != 0u ? remain : 1u;
        }

        // Returns on a posted event, on the exit request, or when the deadline is reached.
        mExternalEvents.wait_event(waitMs);

    } while (true);

    ready_for_events(false);
    remove_all_events();

    ASSERT(mInternalEvents.is_empty());

    return isExit;
}

void TimerManagerBase::stop_manager_thread(bool waitComplete)
{
    if (waitComplete)
    {
        shutdown(areg::WAIT_INFINITE);
    }
    else
    {
        trigger_exit();
    }
}

} // namespace areg

#endif  // defined(_POSIX) || defined(POSIX)
#endif  // !defined(__linux__) && !defined(__APPLE__)
