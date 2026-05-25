#ifndef AREG_BASE_SPINSYNCEVENT_HPP
#define AREG_BASE_SPINSYNCEVENT_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/SpinSyncEvent.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, high-performance hybrid spin+sleep synchronization event.
 *
 *              SpinSyncEvent:
 *
 *              1. Spin phase: before entering OS sleep, the waiting thread busy-waits
 *                 for up to mSpinCount iterations.
 *
 *              2. Sleeper count: set_signaled() only calls the OS wake primitive when
 *                 mSleeperCount > 0 (a thread has exited the spin phase and is sleeping).
 *
 *              3. Sleeping flag: wait_any() sets a per-call atomic<bool> flag immediately
 *                 before entering OS sleep.
 *
 *              Platform support:
 *                - Linux:   futex (FUTEX_WAIT_PRIVATE / FUTEX_WAKE_PRIVATE)
 *                - Windows: WaitOnAddress / WakeByAddress (Windows 8+ / Server 2012+)
 *                - macOS:   __ulock_wait / __ulock_wake (macOS 10.12+)
 *
 *              API mirrors:
 *                - lock()         — wait until signaled (spin then sleep)
 *                - unlock()       — signal (alias of set_signaled)
 *                - set_signaled() — signal the event; only wakes OS if sleepers exist
 *                - reset()        — return to non-signaled state
 *                - pulse_event()  — brief signal (best-effort)
 *                - is_signaled()  — non-blocking state query
 *                - wait_any()     — wait for any one of up to MAX_WAIT_ANY events
 *
 * \note        SpinSyncEvent does NOT derive from SyncObject and CANNOT be
 *              used with areg::MultiLock. Use wait_any() for multi-event waits.
 *
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/CommonDefs.hpp"

#if defined(_MSC_VER)
    #include <intrin.h>     // _mm_pause() — MSVC x86/x64/ARM spin hint
#endif  // _MSC_VER

#include <atomic>
#include <cstdint>

namespace areg {

//////////////////////////////////////////////////////////////////////////
// SpinSyncEvent class declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Hybrid spin+sleep synchronization event for low-latency inter-thread signaling.
 *
 *          SpinSyncEvent eliminates OS wake syscalls on the no-waiter fast path and reduces wake latency.
 *          The sleeping-flag optimization further eliminates the ~150–250 ns futex syscall on Linux in the
 *          wait_any() path when the signal arrives while the waiter is still spinning.
 *
 *          Use case: EventDispatcherBase event queue signaling, where the dispatching
 *          thread processes a burst and the next signal arrives within a few microseconds
 *          of the thread entering its wait.
 **/
class AREG_API SpinSyncEvent
{
//////////////////////////////////////////////////////////////////////////
// Internal types
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Intrusive list node linking one event to a wait_any() call.
     *          Instances live on the stack frame of the wait_any() caller — no heap allocation.
     **/
    struct WaiterNode
    {
        std::atomic<uint32_t>*  mAggWord    { nullptr }; //!< Shared aggregator for this wait batch
        uint32_t                mBitMask    { 0u };      //!< Bit index assigned to this event
        WaiterNode*             mNext       { nullptr }; //!< Next node in per-event list
        std::atomic<bool>*      mSleeping   { nullptr }; //!< Points to caller's sleeping flag; non-null when the waiter has entered OS sleep
    };

//////////////////////////////////////////////////////////////////////////
// Constants
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Maximum number of events that wait_any() accepts.
     **/
    static constexpr int32_t  MAX_WAIT_ANY      { 32 };

    /**
     * \brief   Return value of wait_any() when the call times out with no event signaled.
     **/
    static constexpr int32_t  WAIT_INDEX_TIMEOUT { -1 };

    /**
     * \brief   Default spin iteration count before falling back to OS sleep.
     *              DEFAULT_SPIN_COUNT=0   — disables spinning; embedded / power-sensitive targets.
     *              DEFAULT_SPIN_COUNT=200  — best throughput on server / desktop.
     **/
    static constexpr uint32_t DEFAULT_SPIN_COUNT{ 0u };

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Constructs a hybrid spin+sleep synchronization event.
     *
     * \param   initLock    If true, the event starts non-signaled (threads block on lock()).
     *                      If false, the event starts signaled. Default: true.
     * \param   autoReset   If true, the event resets automatically after a successful lock().
     *                      If false (manual-reset), stays signaled until reset() is called.
     *                      Default: true.
     * \param   spinCount   Number of spin iterations before falling back to OS sleep.
     *                      Default: DEFAULT_SPIN_COUNT. Pass 0 to disable spinning
     *                      (degrades to FastSyncEvent behavior while retaining the
     *                      conditional OS wake optimization).
     **/
    explicit SpinSyncEvent( bool     initLock  = true
                          , bool     autoReset = true
                          , uint32_t spinCount = DEFAULT_SPIN_COUNT ) noexcept;

    ~SpinSyncEvent() noexcept = default;

//////////////////////////////////////////////////////////////////////////
// Public API
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Waits until the event is signaled or the timeout expires.
     *          Spins for up to mSpinCount iterations, then falls back to OS sleep.
     *          For auto-reset events, atomically consumes the signal on success.
     *
     * \param   timeout     Timeout in milliseconds. areg::WAIT_INFINITE waits forever.
     *                      areg::DO_NOT_WAIT checks state without blocking or spinning.
     * \return  Returns true if the event was signaled; false if the timeout expired.
     **/
    inline bool lock(uint32_t timeout = areg::WAIT_INFINITE) noexcept;

    /**
     * \brief   Sets the event to signaled state. Equivalent to set_signaled().
     * \return  Always returns true.
     **/
    inline bool unlock() noexcept;

    /**
     * \brief   Sets the event to signaled state.
     *          Only calls the OS wake primitive if at least one thread is in OS sleep
     *          (i.e., has exited the spin phase). Threads still spinning observe the
     *          state change on their next load without any OS involvement.
     * \return  Always returns true.
     **/
    bool set_signaled() noexcept;

    /**
     * \brief   Resets the event to non-signaled state.
     * \return  Always returns true.
     **/
    bool reset() noexcept;

    /**
     * \brief   Briefly signals the event, wakes waiting threads, then immediately resets.
     *          Inherently racy — prefer set_signaled() with explicit synchronization.
     **/
    void pulse_event() noexcept;

    /**
     * \brief   Returns true if the event is currently in signaled state.
     **/
    [[nodiscard]]
    inline bool is_signaled() const noexcept;

    /**
     * \brief   Returns true if the event uses auto-reset; false for manual-reset.
     **/
    [[nodiscard]]
    inline bool is_auto_reset() const noexcept;

    /**
     * \brief   Waits until at least one of the specified events is signaled, or the timeout expires.
     *
     *          Spins on the shared aggregator word for spin_count iterations before entering
     *          OS sleep. Signals that arrive within the spin window are observed without OS
     *          involvement.
     *
     *          Does NOT auto-reset events. Call reset() on the winning event if needed.
     *          At most MAX_WAIT_ANY (32) events may be passed.
     *
     * \param   events      Non-null array of SpinSyncEvent pointers.
     * \param   count       Number of events (must be in [1, MAX_WAIT_ANY]).
     * \param   timeout     Timeout in milliseconds. areg::WAIT_INFINITE waits forever.
     * \param   spin_count  Spin iterations on the aggregator word before OS sleep.
     *                      Default: DEFAULT_SPIN_COUNT.
     * \return  Zero-based index of the first signaled event, or WAIT_INDEX_TIMEOUT on timeout.
     **/
    static int32_t wait_any( SpinSyncEvent** events
                           , int32_t         count
                           , uint32_t        timeout    = areg::WAIT_INFINITE
                           , uint32_t        spin_count = DEFAULT_SPIN_COUNT ) noexcept;

//////////////////////////////////////////////////////////////////////////
// OS-specific primitives (defined per-platform)
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Waits until mState becomes non-zero or the timeout expires.
     *          Retries internally on spurious wakeups.
     *          For auto-reset events, atomically consumes the signal on success.
     *          Does NOT perform the spin phase — that is handled by _do_wait().
     **/
    bool _os_wait(uint32_t timeout_ms) noexcept;

    /**
     * \brief   Wakes one waiter (auto-reset) or all waiters (manual-reset) sleeping on mState.
     **/
    void _os_wake() noexcept;

    /**
     * \brief   Wakes all waiters sleeping on mState regardless of reset mode.
     **/
    void _os_wake_all() noexcept;

    /**
     * \brief   Waits until *word becomes non-zero or the timeout expires.
     * \return  Returns true if *word != 0; false on timeout.
     **/
    static bool _os_wait_on(std::atomic<uint32_t>* word, uint32_t timeout_ms) noexcept;

    /**
     * \brief   Wakes all waiters sleeping on *word.
     **/
    static void _os_wake_on(std::atomic<uint32_t>* word) noexcept;

//////////////////////////////////////////////////////////////////////////
// Private helpers
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Spin phase + sleeper-count tracking + OS sleep fallback.
     *          Called by lock(). Handles DO_NOT_WAIT, spin, and sleep.
     **/
    bool _do_wait(uint32_t timeout_ms) noexcept;

    /**
     * \brief   Attempts to consume the signal.
     *          Auto-reset: CAS(1 -> 0). Manual-reset: read mState != 0.
     **/
    inline bool _try_consume() noexcept;

    void _register_waiter(WaiterNode* node) noexcept;
    void _unregister_waiter(WaiterNode* node) noexcept;
    void _notify_waiters() noexcept;

    /**
     * \brief   CPU spin hint — reduces pipeline pressure and power during busy-wait.
     *          Maps to PAUSE (x86), YIELD (ARM), or a compiler fence (generic).
     **/
    static inline void _spin_hint() noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

    /**
     * \brief   Atomic event state: 0 = non-signaled, 1 = signaled.
     *          Cache-line aligned to prevent false sharing with adjacent objects.
     **/
    alignas(64) std::atomic<uint32_t>   mState;

    /**
     * \brief   Number of threads currently blocked in OS sleep (past the spin phase).
     *          set_signaled() skips _os_wake() when this is zero.
     **/
    std::atomic<int32_t>                mSleeperCount;

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif  // _MSC_VER

    /**
     * \brief   True for auto-reset mode; false for manual-reset mode.
     **/
    const bool                          mAutoReset;

    /**
     * \brief   Number of busy-spin iterations before falling back to OS sleep.
     **/
    const uint32_t                      mSpinCount;

    /**
     * \brief   Head of the intrusive linked list of wait_any() waiters.
     **/
    WaiterNode*                         mWaiters;

    /**
     * \brief   Spinlock protecting the mWaiters list.
     **/
    std::atomic_flag                    mWaitersLock;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( SpinSyncEvent );
};

//////////////////////////////////////////////////////////////////////////
// SpinSyncEvent inline methods
//////////////////////////////////////////////////////////////////////////

inline bool SpinSyncEvent::lock(uint32_t timeout /* = areg::WAIT_INFINITE */) noexcept
{
    return _do_wait(timeout);
}

inline bool SpinSyncEvent::unlock() noexcept
{
    return set_signaled();
}

inline bool SpinSyncEvent::is_signaled() const noexcept
{
    return (mState.load(std::memory_order_acquire) != 0u);
}

inline bool SpinSyncEvent::is_auto_reset() const noexcept
{
    return mAutoReset;
}

inline bool SpinSyncEvent::_try_consume() noexcept
{
    if (mAutoReset)
    {
        uint32_t expected { 1u };
        return mState.compare_exchange_strong( expected, 0u
                                             , std::memory_order_acq_rel
                                             , std::memory_order_relaxed );
    }
    else
    {
        return (mState.load(std::memory_order_acquire) != 0u);
    }
}

inline void SpinSyncEvent::_spin_hint() noexcept
{
#if defined(_MSC_VER)
    _mm_pause();
#elif defined(__i386__) || defined(__x86_64__)
    __asm__ volatile("pause" ::: "memory");
#elif defined(__arm__) || defined(__aarch64__)
    __asm__ volatile("yield" ::: "memory");
#else
    std::atomic_signal_fence(std::memory_order_seq_cst);
#endif  // _MSC_VER
}

} // namespace areg

#endif  // AREG_BASE_SPINSYNCEVENT_HPP
