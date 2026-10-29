/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/base/private/posix/WaitableTimerPosix.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, POSIX Waitable Timer implementation.
 *              One timer thread per process expires every armed timer on all POSIX
 *              platforms (Linux, macOS, Cygwin, FreeBSD).
 *
 ************************************************************************/

#if defined(_POSIX) || defined(POSIX)

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/private/posix/WaitableTimerPosix.hpp"
#include "areg/base/private/posix/SyncDefsPosix.hpp"
#include "areg/base/private/posix/SyncLockAndWaitPosix.hpp"

#include <algorithm>
#include <limits.h>
#include <new>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

#ifdef __APPLE__
    #include <sys/event.h>
#endif  // __APPLE__

namespace areg::os {

//////////////////////////////////////////////////////////////////////////
// TimerServicePosix class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Expires every armed WaitableTimerPosix of the process on one thread. The thread
 *          sleeps until the earliest deadline, starts with the first armed timer and is
 *          joined when the last timer object is destroyed.
 **/
class TimerServicePosix
{
public:
    /**
     * \brief   Returns the process-wide timer service. It is never destroyed.
     **/
    static TimerServicePosix & instance() noexcept;

    /**
     * \brief   Counts a new timer object.
     **/
    void attach() noexcept;

    /**
     * \brief   Uncounts a destroyed timer object. The last one stops and joins the thread.
     **/
    void detach() noexcept;

    /**
     * \brief   Arms the timer to expire after the timeout, starting the thread if needed.
     *
     * \param   timer       The timer to arm. It is disarmed first if it is armed.
     * \param   msTimeout   The timeout in milliseconds, not 0.
     * \param   isPeriodic  If true, the timer is re-armed at every expiration.
     * \return  Returns true if the timer is armed.
     **/
    bool arm( WaitableTimerPosix & timer, uint32_t msTimeout, bool isPeriodic ) noexcept;

    /**
     * \brief   Disarms the timer and waits until a running expiration of it has returned.
     *
     * \param   timer       The timer to disarm.
     **/
    void disarm( WaitableTimerPosix & timer ) noexcept;

private:
    TimerServicePosix() noexcept;
    ~TimerServicePosix() = delete;

    /**
     * \brief   The thread routine. The argument is the generation the thread serves.
     **/
    static void * _thread_routine( void * arg ) noexcept;

    /**
     * \brief   Expires the timers while the generation is current.
     **/
    void _run( uint32_t generation ) noexcept;

    /**
     * \brief   Starts the thread of the current generation. Called with the lock held.
     **/
    bool _start_thread() noexcept;

    /**
     * \brief   Inserts the timer into the armed list in deadline order. Called with the lock held.
     **/
    void _link( WaitableTimerPosix & timer ) noexcept;

    /**
     * \brief   Removes the timer from the armed list. Called with the lock held.
     **/
    void _unlink( WaitableTimerPosix & timer ) noexcept;

    /**
     * \brief   Fork handlers: the child keeps a usable lock and no armed timer.
     **/
    static void _fork_prepare() noexcept;
    static void _fork_parent() noexcept;
    static void _fork_child() noexcept;

    /**
     * \brief   Returns the monotonic time in nanoseconds.
     **/
    static uint64_t _now_ns() noexcept;

#ifdef __APPLE__
    /**
     * \brief   Creates the kernel queue of the thread and adds its wake event to it.
     *
     * \return  Returns true if the queue is usable.
     **/
    bool _open_queue() noexcept;

    /**
     * \brief   Releases the lock, sleeps on the kernel queue and takes the lock again.
     *
     * \param   waitNs  The longest sleep in nanoseconds, 0 to sleep until woken.
     **/
    void _queue_sleep( uint64_t waitNs ) noexcept;

    /**
     * \brief   Wakes the thread sleeping on the kernel queue.
     **/
    void _queue_wake() noexcept;
#endif  // __APPLE__

private:
    //!< The stack size of the timer thread.
    static constexpr size_t     STACK_SIZE  { 64u * 1024u };

#ifdef __APPLE__
    //!< The identifier of the deadline timer in the kernel queue.
    static constexpr uint64_t   QUEUE_TIMER { 1u };
    //!< The identifier of the wake event in the kernel queue.
    static constexpr uint64_t   QUEUE_WAKE  { 2u };
#endif  // __APPLE__

    //!< Guards every field of the service and the armed fields of the timers.
    pthread_mutex_t         mLock;
#ifdef __APPLE__
    //!< The kernel queue the thread sleeps on until the earliest deadline. Its timer is not coalesced.
    int                     mQueue;
#else   // !__APPLE__
    //!< The thread sleeps on it until the earliest deadline. Uses the monotonic clock.
    pthread_cond_t          mWakeCond;
#endif  // __APPLE__
    //!< A disarming thread waits on it until the running expiration returns.
    pthread_cond_t          mDoneCond;
    //!< The armed timer with the earliest deadline.
    WaitableTimerPosix *    mHead;
    //!< The timer whose expiration runs outside the lock.
    WaitableTimerPosix *    mFiring;
    //!< The timer thread of the current generation.
    pthread_t               mThread;
    //!< The number of timer objects alive.
    uint32_t                mLiveCount;
    //!< The generation of the timer thread. A thread of an older generation exits.
    uint32_t                mGeneration;
    //!< The number of threads waiting on mDoneCond.
    uint32_t                mDoneWaiters;
    //!< Flag, indicates whether the thread of the current generation runs.
    bool                    mIsRunning;
    //!< Flag, indicates whether the condition variables were initialized.
    bool                    mIsValid;
};

//////////////////////////////////////////////////////////////////////////
// TimerServicePosix class implementation
//////////////////////////////////////////////////////////////////////////

TimerServicePosix & TimerServicePosix::instance() noexcept
{
    alignas(TimerServicePosix) static unsigned char _storage[sizeof(TimerServicePosix)];
    static TimerServicePosix * const _service{ new (_storage) TimerServicePosix() };
    return *_service;
}

TimerServicePosix::TimerServicePosix() noexcept
    : mLock         ( )
#ifdef __APPLE__
    , mQueue        ( -1 )
#else   // !__APPLE__
    , mWakeCond     ( )
#endif  // __APPLE__
    , mDoneCond     ( )
    , mHead         ( nullptr )
    , mFiring       ( nullptr )
    , mThread       ( )
    , mLiveCount    ( 0u )
    , mGeneration   ( 0u )
    , mDoneWaiters  ( 0u )
    , mIsRunning    ( false )
    , mIsValid      ( false )
{
    ::pthread_mutex_init(&mLock, nullptr);

#ifdef __APPLE__
    mIsValid =  (areg::RETURNED_OK == ::pthread_cond_init(&mDoneCond, nullptr)) && _open_queue();
#else   // !__APPLE__
    pthread_condattr_t attr;
    if (areg::RETURNED_OK == ::pthread_condattr_init(&attr))
    {
        mIsValid =  (areg::RETURNED_OK == ::pthread_condattr_setclock(&attr, CLOCK_MONOTONIC)) &&
                    (areg::RETURNED_OK == ::pthread_cond_init(&mWakeCond, &attr)) &&
                    (areg::RETURNED_OK == ::pthread_cond_init(&mDoneCond, nullptr));
        ::pthread_condattr_destroy(&attr);
    }
#endif  // __APPLE__

    ::pthread_atfork(&TimerServicePosix::_fork_prepare, &TimerServicePosix::_fork_parent, &TimerServicePosix::_fork_child);
}

void TimerServicePosix::attach() noexcept
{
    ::pthread_mutex_lock(&mLock);
    ++ mLiveCount;
    ::pthread_mutex_unlock(&mLock);
}

void TimerServicePosix::detach() noexcept
{
    bool join{ false };
    pthread_t thread{ };

    ::pthread_mutex_lock(&mLock);
    ASSERT(mLiveCount != 0u);
    if ((-- mLiveCount == 0u) && mIsRunning)
    {
        mIsRunning = false;
        ++ mGeneration;
        thread = mThread;
        join = true;
#ifdef __APPLE__
        _queue_wake();
#else   // !__APPLE__
        ::pthread_cond_broadcast(&mWakeCond);
#endif  // __APPLE__
    }

    ::pthread_mutex_unlock(&mLock);

    if (join)
    {
        if (::pthread_equal(thread, ::pthread_self()) == 0)
        {
            ::pthread_join(thread, nullptr);
        }
        else
        {
            ::pthread_detach(thread);
        }
    }
}

bool TimerServicePosix::arm(WaitableTimerPosix & timer, uint32_t msTimeout, bool isPeriodic) noexcept
{
    const uint64_t periodNs{ static_cast<uint64_t>(msTimeout) * static_cast<uint64_t>(areg::MILLISEC_TO_NS) };

    ::pthread_mutex_lock(&mLock);
    const bool result{ mIsValid && (mIsRunning || _start_thread()) };
    if (result)
    {
        if (timer.mIsArmed)
        {
            _unlink(timer);
        }

        timer.mDeadlineNs   = _now_ns() + periodNs;
        timer.mPeriodNs     = isPeriodic ? periodNs : 0u;
        _link(timer);
        if (mHead == &timer)
        {
#ifdef __APPLE__
            _queue_wake();
#else   // !__APPLE__
            ::pthread_cond_signal(&mWakeCond);
#endif  // __APPLE__
        }
    }

    ::pthread_mutex_unlock(&mLock);
    return result;
}

void TimerServicePosix::disarm(WaitableTimerPosix & timer) noexcept
{
    ::pthread_mutex_lock(&mLock);
    if (timer.mIsArmed)
    {
        _unlink(timer);
    }

    while ((mFiring == &timer) && (::pthread_equal(mThread, ::pthread_self()) == 0))
    {
        ++ mDoneWaiters;
        ::pthread_cond_wait(&mDoneCond, &mLock);
        -- mDoneWaiters;
    }

    ::pthread_mutex_unlock(&mLock);
}

void * TimerServicePosix::_thread_routine(void * arg) noexcept
{
#if defined(__APPLE__)
    ::pthread_setname_np("areg_timer");
#elif defined(__linux__)
    ::pthread_setname_np(::pthread_self(), "areg_timer");
#endif  // defined(__APPLE__)

    TimerServicePosix::instance()._run(static_cast<uint32_t>(reinterpret_cast<uintptr_t>(arg)));
    return nullptr;
}

void TimerServicePosix::_run(uint32_t generation) noexcept
{
    ::pthread_mutex_lock(&mLock);
    while (generation == mGeneration)
    {
        WaitableTimerPosix * timer{ mHead };
        if (timer == nullptr)
        {
#ifdef __APPLE__
            _queue_sleep(0u);
#else   // !__APPLE__
            ::pthread_cond_wait(&mWakeCond, &mLock);
#endif  // __APPLE__
            continue;
        }

        const uint64_t now{ _now_ns() };
        if (timer->mDeadlineNs > now)
        {
#ifdef __APPLE__
            _queue_sleep(timer->mDeadlineNs - now);
#else   // !__APPLE__
            const uint64_t waitNs{ timer->mDeadlineNs };
            struct timespec due;
            due.tv_sec  = static_cast<time_t>(waitNs / static_cast<uint64_t>(areg::SEC_TO_NS));
            due.tv_nsec = static_cast<long>(waitNs % static_cast<uint64_t>(areg::SEC_TO_NS));
            ::pthread_cond_timedwait(&mWakeCond, &mLock, &due);
#endif  // __APPLE__
            continue;
        }

        _unlink(*timer);
        uint32_t expirations{ 1u };
        if (timer->mPeriodNs != 0u)
        {
            // A periodic timer keeps its phase: a late expiration skips the missed periods.
            const uint64_t missed{ (now - timer->mDeadlineNs) / timer->mPeriodNs };
            expirations = static_cast<uint32_t>(std::min<uint64_t>(missed + 1u, UINT32_MAX));
            timer->mDeadlineNs += (missed + 1u) * timer->mPeriodNs;
            _link(*timer);
        }

        mFiring = timer;
        ::pthread_mutex_unlock(&mLock);

        timer->_timer_expired(expirations);

        ::pthread_mutex_lock(&mLock);
        mFiring = nullptr;
        if (mDoneWaiters != 0u)
        {
            ::pthread_cond_broadcast(&mDoneCond);
        }
    }

    ::pthread_mutex_unlock(&mLock);
}

bool TimerServicePosix::_start_thread() noexcept
{
    pthread_attr_t attr;
    if (areg::RETURNED_OK != ::pthread_attr_init(&attr))
        return false;

#if defined(_SC_THREAD_STACK_MIN)
    const long minStack{ ::sysconf(_SC_THREAD_STACK_MIN) };
#else   // !defined(_SC_THREAD_STACK_MIN)
    const long minStack{ static_cast<long>(PTHREAD_STACK_MIN) };
#endif  // defined(_SC_THREAD_STACK_MIN)
    const size_t stackSize{ std::max<size_t>(STACK_SIZE, minStack > 0 ? static_cast<size_t>(minStack) : 0u) };
    ::pthread_attr_setstacksize(&attr, stackSize);

    // The timer thread starts with every signal blocked, so no process signal is delivered to it.
    sigset_t allSignals;
    sigset_t oldSignals;
    sigfillset(&allSignals);
    ::pthread_sigmask(SIG_SETMASK, &allSignals, &oldSignals);

    ++ mGeneration;
    void * arg{ reinterpret_cast<void *>(static_cast<uintptr_t>(mGeneration)) };
    mIsRunning = (areg::RETURNED_OK == ::pthread_create(&mThread, &attr, &TimerServicePosix::_thread_routine, arg));

    ::pthread_sigmask(SIG_SETMASK, &oldSignals, nullptr);
    ::pthread_attr_destroy(&attr);
    return mIsRunning;
}

void TimerServicePosix::_link(WaitableTimerPosix & timer) noexcept
{
    ASSERT(timer.mIsArmed == false);
    WaitableTimerPosix * prev{ nullptr };
    WaitableTimerPosix * next{ mHead };
    while ((next != nullptr) && (next->mDeadlineNs <= timer.mDeadlineNs))
    {
        prev = next;
        next = next->mNextArmed;
    }

    timer.mPrevArmed = prev;
    timer.mNextArmed = next;
    timer.mIsArmed   = true;
    if (next != nullptr)
    {
        next->mPrevArmed = &timer;
    }

    if (prev != nullptr)
    {
        prev->mNextArmed = &timer;
    }
    else
    {
        mHead = &timer;
    }
}

void TimerServicePosix::_unlink(WaitableTimerPosix & timer) noexcept
{
    ASSERT(timer.mIsArmed);
    if (timer.mNextArmed != nullptr)
    {
        timer.mNextArmed->mPrevArmed = timer.mPrevArmed;
    }

    if (timer.mPrevArmed != nullptr)
    {
        timer.mPrevArmed->mNextArmed = timer.mNextArmed;
    }
    else
    {
        mHead = timer.mNextArmed;
    }

    timer.mNextArmed = nullptr;
    timer.mPrevArmed = nullptr;
    timer.mIsArmed   = false;
}

void TimerServicePosix::_fork_prepare() noexcept
{
    ::pthread_mutex_lock(&TimerServicePosix::instance().mLock);
}

void TimerServicePosix::_fork_parent() noexcept
{
    ::pthread_mutex_unlock(&TimerServicePosix::instance().mLock);
}

void TimerServicePosix::_fork_child() noexcept
{
    TimerServicePosix & service{ TimerServicePosix::instance() };
    while (service.mHead != nullptr)
    {
        service._unlink(*service.mHead);
    }

    service.mFiring      = nullptr;
    service.mDoneWaiters = 0u;
    service.mIsRunning   = false;
    ++ service.mGeneration;
#ifdef __APPLE__
    // A child process does not inherit the kernel queue.
    service.mIsValid     = service.mIsValid && service._open_queue();
#endif  // __APPLE__
    ::pthread_mutex_unlock(&service.mLock);
}

uint64_t TimerServicePosix::_now_ns() noexcept
{
    struct timespec now { 0, 0 };
    ::clock_gettime(CLOCK_MONOTONIC, &now);
    return (static_cast<uint64_t>(now.tv_sec) * static_cast<uint64_t>(areg::SEC_TO_NS) + static_cast<uint64_t>(now.tv_nsec));
}

#ifdef __APPLE__

bool TimerServicePosix::_open_queue() noexcept
{
    mQueue = ::kqueue();
    if (mQueue < 0)
        return false;

    struct kevent64_s change { };
    EV_SET64(&change, QUEUE_WAKE, EVFILT_USER, EV_ADD | EV_CLEAR, 0u, 0, 0u, 0u, 0u);
    return (::kevent64(mQueue, &change, 1, nullptr, 0, 0u, nullptr) == 0);
}

void TimerServicePosix::_queue_sleep(uint64_t waitNs) noexcept
{
    // A critical timer has no coalescing delay, also in a throttled process.
    struct kevent64_s change { };
    struct kevent64_s event { };
    EV_SET64(&change, QUEUE_TIMER, EVFILT_TIMER, EV_ADD | EV_ONESHOT, NOTE_NSECONDS | NOTE_CRITICAL, static_cast<int64_t>(waitNs), 0u, 0u, 0u);
    const int queue{ mQueue };
    ::pthread_mutex_unlock(&mLock);
    ::kevent64(queue, &change, waitNs != 0u ? 1 : 0, &event, 1, 0u, nullptr);
    ::pthread_mutex_lock(&mLock);
}

void TimerServicePosix::_queue_wake() noexcept
{
    struct kevent64_s change { };
    EV_SET64(&change, QUEUE_WAKE, EVFILT_USER, 0u, NOTE_TRIGGER, 0, 0u, 0u, 0u);
    ::kevent64(mQueue, &change, 1, nullptr, 0, 0u, nullptr);
}

#endif  // __APPLE__

//////////////////////////////////////////////////////////////////////////
// WaitableTimerPosix class implementation
//////////////////////////////////////////////////////////////////////////

WaitableTimerPosix::WaitableTimerPosix(bool is_auto_reset /*= false*/)
    : WaitablePosix  ( areg::os::SyncKind::SoWaitTimer )
    , mObjectLock    ( areg::os::SyncKind::SoMutex, false )
    , mResetInfo     ( is_auto_reset ? areg::os::ResetMode::Automatic : areg::os::ResetMode::Manual )
    , mNextArmed     ( nullptr )
    , mPrevArmed     ( nullptr )
    , mDeadlineNs    ( 0u )
    , mPeriodNs      ( 0u )
    , mIsArmed       ( false )
    , mTimeout       ( 0 )
    , mIsSignaled    ( false )
    , mFiredCount    ( 0 )
{
    TimerServicePosix::instance().attach();
}

WaitableTimerPosix::~WaitableTimerPosix()
{
    TimerServicePosix & service{ TimerServicePosix::instance() };
    service.disarm(*this);
    service.detach();
}

bool WaitableTimerPosix::set_timer(uint32_t msTimeout, bool is_periodic)
{
    TimerServicePosix & service{ TimerServicePosix::instance() };
    service.disarm(*this);

    do
    {
        ObjectLockPosix lock(mObjectLock);
        _stop_timer();
        if (msTimeout != 0)
        {
            mTimeout    = msTimeout;
            mIsSignaled = false;
        }
    } while (false);

    if ((msTimeout == 0) || (service.arm(*this, msTimeout, is_periodic) == false))
    {
        ObjectLockPosix lock(mObjectLock);
        _stop_timer();
        return false;
    }

    return true;
}

bool WaitableTimerPosix::cancel_timer() noexcept
{
    TimerServicePosix::instance().disarm(*this);

    do
    {
        ObjectLockPosix lock(mObjectLock);
        _stop_timer();
        mIsSignaled = true;
    } while (false);

    SyncLockAndWaitPosix::event_signaled(*this);
    return true;
}

bool WaitableTimerPosix::is_valid() const noexcept
{
    return WaitablePosix::is_valid();
}

bool WaitableTimerPosix::check_signaled(pthread_t /*contextThread*/) const
{
    ObjectLockPosix lock(mObjectLock);
    return mIsSignaled;
}

bool WaitableTimerPosix::notify_request_ownership(pthread_t /* ownerThread */)
{
    ObjectLockPosix lock(mObjectLock);
    if (mIsSignaled == false)
        return false;

    // An automatic reset timer is taken by one waiter.
    if (mResetInfo == areg::os::ResetMode::Automatic)
    {
        mIsSignaled = false;
    }

    return true;
}

bool WaitableTimerPosix::can_signal_threads() const noexcept
{
    return true;
}

void WaitableTimerPosix::notify_released_threads(int32_t /* numThreads */)
{
}

void WaitableTimerPosix::_stop_timer() noexcept
{
    mTimeout    = 0;
    mFiredCount = 0;
}

void WaitableTimerPosix::_timer_expired(uint32_t expirations) noexcept
{
    do
    {
        ObjectLockPosix lock(mObjectLock);
        mFiredCount += expirations;
        mIsSignaled  = true;
    } while (false);

    SyncLockAndWaitPosix::event_signaled(*this);
}

} // namespace areg::os

#endif  // defined(_POSIX) || defined(POSIX)
