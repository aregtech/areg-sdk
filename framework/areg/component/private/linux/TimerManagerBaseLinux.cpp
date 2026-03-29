/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/linux/TimerManagerBaseLinux.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Linux-specific TimerManagerBase implementation.
 *              epoll-based dispatcher loop (post_event, run_dispatcher,
 *              stop_manager_thread).
 *
 ************************************************************************/
#ifdef __linux__

#include "areg/component/private/TimerManagerBase.hpp"
#include "areg/component/private/ExitEvent.hpp"
#include "areg/component/private/posix/TimerPosix.hpp"
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include <errno.h>
#include <cstdint>

namespace areg {

bool TimerManagerBase::post_event(Event& eventElem)
{
    bool result = (AREG_RUNTIME_CAST(&eventElem, TimerManagerEvent) != nullptr) && EventDispatcher::post_event(eventElem);

    // Wake the epoll loop in run_dispatcher() so it can drain the event queue.
    if (result && (mCommandFd >= 0))
    {
        const uint64_t one { 1u };
        (void)::write(mCommandFd, &one, sizeof(one));
    }

    return result;
}

bool TimerManagerBase::run_dispatcher()
{
    static constexpr void * COMMAND_PTR { nullptr };                        // epoll data.ptr for mCommandFd
    void * const             EXIT_PTR   { reinterpret_cast<void*>(-1) };    // epoll data.ptr for mExitFd

    mEpollFd   = ::epoll_create1(EPOLL_CLOEXEC);
    mCommandFd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    mExitFd    = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);

    if ((mEpollFd < 0) || (mCommandFd < 0) || (mExitFd < 0))
    {
        if (mEpollFd   >= 0) { ::close(mEpollFd);   mEpollFd   = -1; }
        if (mCommandFd >= 0) { ::close(mCommandFd); mCommandFd = -1; }
        if (mExitFd    >= 0) { ::close(mExitFd);    mExitFd    = -1; }
        return false;
    }

    struct epoll_event evCmd{};
    evCmd.events   = EPOLLIN;
    evCmd.data.ptr = COMMAND_PTR;
    ::epoll_ctl(mEpollFd, EPOLL_CTL_ADD, mCommandFd, &evCmd);

    struct epoll_event evExit{};
    evExit.events   = EPOLLIN;
    evExit.data.ptr = EXIT_PTR;
    ::epoll_ctl(mEpollFd, EPOLL_CTL_ADD, mExitFd, &evExit);

    ready_for_events(true);

    const ExitEvent& exitEvent = ExitEvent::exit_event();
    bool exitRequested = false;

    while (!exitRequested)
    {
        struct epoll_event events[32];
        const int n = ::epoll_wait(mEpollFd, events, 32, -1);
        if (n < 0)
        {
            if (errno == EINTR)
                continue;   // interrupted by signal — retry

            break;          // unexpected error — treat as exit
        }

        for (int i = 0; (i < n) && !exitRequested; ++i)
        {
            void* ptr = events[i].data.ptr;

            if (ptr == EXIT_PTR)
            {
                // Exit signal from stop_manager_thread().
                exitRequested = true;
            }
            else if (ptr == COMMAND_PTR)
            {
                // Management event(s) queued (start / stop timer).
                // Drain the eventfd counter, then process all pending events.
                uint64_t counter { 0u };
                (void)::read(mCommandFd, &counter, sizeof(counter));

                Event* evt{ nullptr };
                while ((evt = pick_event()) != nullptr)
                {
                    if (static_cast<const Event*>(evt) == static_cast<const Event*>(&exitEvent))
                    {
                        exitRequested = true;
                        break;
                    }

                    if (prepare_dispatch_event(evt))
                    {
                        dispatch_event(*evt);
                    }

                    post_dispatch_event(evt);
                    ASSERT(static_cast<EventQueue&>(mInternalEvents).is_empty());
                }
            }
            else
            {
                // A timerfd became readable — dispatch to subclass.
                TIMERHANDLE handle = reinterpret_cast<TIMERHANDLE>(ptr);
                areg::os::TimerPosix* posixTimer = reinterpret_cast<areg::os::TimerPosix*>(handle);

                // Read and discard the expiration count (clears the readable state).
                uint64_t expirations { 0u };
                (void)::read(posixTimer->timer_fd(), &expirations, sizeof(expirations));

                _on_timerfd_expired(handle);
            }
        }
    }

    ready_for_events(false);
    remove_all_events();

    ::close(mExitFd);    mExitFd    = -1;
    ::close(mCommandFd); mCommandFd = -1;
    ::close(mEpollFd);   mEpollFd   = -1;

    return true;
}

void TimerManagerBase::stop_manager_thread(bool waitComplete)
{
    // Signal the epoll loop to exit before calling the base implementation.
    if (mExitFd >= 0)
    {
        const uint64_t one { 1u };
        (void)::write(mExitFd, &one, sizeof(one));
    }

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

#endif  // __linux__
