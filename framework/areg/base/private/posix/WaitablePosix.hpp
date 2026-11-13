#ifndef AREG_BASE_PRIVATE_POSIX_WAITABLEPOSIX_HPP
#define AREG_BASE_PRIVATE_POSIX_WAITABLEPOSIX_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/base/private/posix/WaitablePosix.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, waitable base object.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/base/areg_global.h"

#if defined(_POSIX) || defined(POSIX)

#include "areg/base/private/posix/SyncObjectPosix.hpp"
#include "areg/base/private/WaitWord.hpp"
#include "areg/base/SyncPrimitives.hpp"
#include <atomic>
#include <pthread.h>
namespace areg::os {

//////////////////////////////////////////////////////////////////////////
// WaiterNode struct declaration
//
// Stack-allocated intrusive list node for the lock-free WaitAny path.
// One node is created per waitable in the wait set; all nodes for a
// single wait_multiple() call share the same mFiredWord.
//
// Supported on Linux (futex), macOS (__ulock), and Cygwin (WaitOnAddress).
// On other POSIX targets the old global-map + TLS-cond path is used.
//////////////////////////////////////////////////////////////////////////
#if defined(__linux__) || defined(__APPLE__) || defined(__CYGWIN__)

struct WaiterNode
{
    //! Per-call atomic fire word (stack-allocated by the waiting thread).
    std::atomic<uint32_t>*  mFiredWord;
    //! Value to CAS into mFiredWord when this waitable fires (waitable index).
    uint32_t                mFiredValue;
    //! Intrusive list linkage (per-waitable LIFO list).
    WaiterNode*             mNext;
};

#endif  // defined(__linux__) || defined(__APPLE__) || defined(__CYGWIN__)

class SignalLockPosix;

//////////////////////////////////////////////////////////////////////////
// SyncWaitable class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Base class for POSIX waitable synchronization objects. Supports single or group waiting.
 *          Cannot be instantiated directly; use derived classes.
 **/
class WaitablePosix : public SyncObjectPosix
{
    friend class SignalLockPosix;

//////////////////////////////////////////////////////////////////////////
// Constants
//////////////////////////////////////////////////////////////////////////
protected:
    //!< The bit of the wait state an event uses for its signaled state.
    static constexpr uint32_t   STATE_SIGNALED  { 0x00000001u };
    //!< The bit of the wait state set while a WaitAny waiter is registered.
    static constexpr uint32_t   STATE_WAIT_ANY  { 0x00000002u };
    //!< The bit of the wait state set while a WaitAll waiter is registered.
    static constexpr uint32_t   STATE_WAIT_ALL  { 0x00000004u };
    //!< The bits of the wait state set while a waiter of any kind is registered.
    static constexpr uint32_t   STATE_WAITERS   { STATE_WAIT_ANY | STATE_WAIT_ALL };

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Protected constructor. Sets synchronization type and name.
     *
     * \param   syncType    The type of the synchronization object.
     * \param   initState   The initial bits of the wait state owned by the derived class.
     **/
    WaitablePosix( areg::os::SyncKind syncType, uint32_t initState = 0u );

public:
    virtual ~WaitablePosix();

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns true if the object is signaled.
     *
     * \param   contextThread       The thread context where signaling is checked.
     **/
    [[nodiscard]]
    virtual bool check_signaled( pthread_t contextThread ) const = 0;

    /**
     * \brief   Callback when a waiting thread is released to continue execution.
     *
     * \param   ownerThread     The thread ID that was released from waiting.
     * \return  Returns true if ownership was successfully acquired.
     **/
    virtual bool notify_request_ownership( pthread_t ownerThread ) = 0;

    /**
     * \brief   Returns true if this object can signal multiple threads simultaneously (e.g.,
     *          Event). Returns false for single-threaded objects (e.g., Mutex).
     **/
    [[nodiscard]]
    virtual bool can_signal_threads() const noexcept = 0;

    /**
     * \brief   Callback to notify the object how many threads were released when signaled.
     *
     * \param   numThreads      The number of released threads; 0 if none.
     **/
    virtual void notify_released_threads( int32_t numThreads ) = 0;

    /**
     * \brief   Returns true if the synchronization object is valid.
     **/
    [[nodiscard]]
    bool is_valid() const noexcept override;

    /**
     * \brief   Called before deletion. Waits for a signal call that holds the signal lock, then
     *          releases all waiting threads with failure code.
     **/
    void free_resources() final;

    /**
     * \brief   Called by SyncLockAndWaitPosix under the WaitAll map lock when registering this
     *          waitable in the WaitAll global map, before the state of the waitable is checked.
     **/
    inline void add_wait_all_registration() noexcept;

    /**
     * \brief   Called by SyncLockAndWaitPosix under the WaitAll map lock when deregistering this
     *          waitable from the WaitAll global map.
     **/
    inline void remove_wait_all_registration() noexcept;

    /**
     * \brief   Returns true if at least one WaitAll waiter is currently registered for this
     *          waitable.  Used by event_signaled() to skip the global map lock on the common
     *          (WaitAny-only) path.
     **/
    [[nodiscard]]
    inline bool has_wait_all_waiters() const noexcept;

//////////////////////////////////////////////////////////////////////////
// WaitAny intrusive-list operations (Linux / macOS / Cygwin only)
//////////////////////////////////////////////////////////////////////////
#if defined(__linux__) || defined(__APPLE__) || defined(__CYGWIN__)

    /**
     * \brief   Prepends a WaiterNode to the per-object intrusive waiter list.
     *          Called by the waiting thread before entering OS sleep.
     *
     * \param   node    Stack-allocated node owned by the calling wait frame.
     **/
    void register_waiter( WaiterNode* node ) noexcept;

    /**
     * \brief   Removes a WaiterNode from the per-object intrusive waiter list.
     *          Called by the waiting thread on wakeup, timeout, or cancellation.
     *
     * \param   node    The node to remove.
     **/
    void unregister_waiter( WaiterNode* node ) noexcept;

    /**
     * \brief   Marks each eligible WaitAny waiter. The waiters are woken when the signal lock is released.
     *
     * \param   signal  The signal lock of this waitable, held by the caller.
     * \return  Returns the number of waiters marked.
     **/
    int32_t notify_any_waiters( SignalLockPosix & signal ) noexcept;

    /**
     * \brief   Hands a pulse to the WaitAny waiters parked now: each one marked returns owning
     *          this object without checking its state.
     *
     * \param   releaseAll  If true, marks every waiter; otherwise marks the first one only.
     * \param   signal      The signal lock of this waitable, held by the caller.
     * \return  Returns the number of waiters marked.
     **/
    int32_t notify_pulse_waiters( bool releaseAll, SignalLockPosix & signal ) noexcept;

    /**
     * \brief   Returns true if at least one waiter is currently parked on this object's intrusive list.
     *          Lets a signaler wake a parked waiter even after the false->true edge passed.
     **/
    [[nodiscard]]
    inline bool has_waiters() const noexcept;

#endif  // defined(__linux__) || defined(__APPLE__) || defined(__CYGWIN__)

protected:
    /**
     * \brief   Returns true if a waiter of any kind is registered. Called under the lock that
     *          guards the state of the derived class, which a waiter takes after registering.
     **/
    [[nodiscard]]
    inline bool has_registered_waiters() const noexcept;

    /**
     * \brief   Returns the wait state: the waiter bits and the bits owned by the derived class.
     **/
    inline std::atomic<uint32_t> & wait_state() noexcept;

    /**
     * \brief   Returns the wait state: the waiter bits and the bits owned by the derived class.
     **/
    inline const std::atomic<uint32_t> & wait_state() const noexcept;

private:
#if defined(__linux__) || defined(__APPLE__) || defined(__CYGWIN__)
    /**
     * \brief   Marks the waiters with their index and the given bits.
     *
     * \param   mark        The bits added to the index of each waiter.
     * \param   releaseAll  If true, marks every waiter; otherwise marks the first one only.
     * \param   signal      The signal lock of this waitable, held by the caller.
     * \return  Returns the number of waiters marked.
     **/
    int32_t _notify_waiters( uint32_t mark, bool releaseAll, SignalLockPosix & signal ) noexcept;

    //!< Head of the per-object intrusive WaiterNode list; nullptr when empty.
    std::atomic<WaiterNode*>    mWaiters;
#endif  // defined(__linux__) || defined(__APPLE__) || defined(__CYGWIN__)

    //!< Lock of the mWaiters list and of a signal call that may release a waiter.
    areg::PlainSpinLock         mWaitersLock;
    //!< The STATE_WAIT_ANY and STATE_WAIT_ALL bits, and the bits owned by the derived class.
    std::atomic<uint32_t>       mWaitState;
    //! Set to false by free_resources() to signal the object is no longer usable.
    std::atomic<bool>           mValid;
    //! Number of active WaitAll registrations, changed under the WaitAll map lock.
    uint32_t                    mWaitAllCount;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Default constructor is deleted.
     **/
    WaitablePosix() = delete;
    AREG_NOCOPY_NOMOVE( WaitablePosix );
};

//////////////////////////////////////////////////////////////////////////
// SignalLockPosix class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Holds the signal lock of a waitable for a signal call that may release a waiter.
 *          free_resources() of the waitable waits for it. The destructor releases the lock,
 *          then wakes the waiters marked under it, and reads nothing of the waitable.
 **/
class SignalLockPosix
{
    friend class WaitablePosix;

public:
    /**
     * \brief   Takes the signal lock of the waitable, before the call changes the state.
     *
     * \param   waitable    The waitable that is signaled.
     **/
    explicit inline SignalLockPosix( WaitablePosix & waitable ) noexcept;

    /**
     * \brief   Releases the signal lock and wakes the marked waiters.
     **/
    ~SignalLockPosix( ) noexcept;

private:
    //!< The signal lock of the waitable.
    areg::PlainSpinLock &   mLock;

#if defined(__linux__) || defined(__APPLE__) || defined(__CYGWIN__)
    /**
     * \brief   Wakes the marked waiters and empties the list.
     **/
    void _wake_marked() noexcept;

    //!< The fire words of the marked waiters.
    std::atomic<uint32_t> * mWake[areg::MAXIMUM_WAITING_OBJECTS];
    //!< The number of entries in mWake.
    int32_t                 mWakeCount;
#endif  // defined(__linux__) || defined(__APPLE__) || defined(__CYGWIN__)

    SignalLockPosix( ) = delete;
    AREG_NOCOPY_NOMOVE( SignalLockPosix );
};

//////////////////////////////////////////////////////////////////////////
// WaitablePosix class inline implementation
//////////////////////////////////////////////////////////////////////////

inline void WaitablePosix::add_wait_all_registration() noexcept
{
    if (mWaitAllCount ++ == 0u)
    {
        mWaitState.fetch_or(STATE_WAIT_ALL, std::memory_order_acq_rel);
    }
}

inline void WaitablePosix::remove_wait_all_registration() noexcept
{
    if (-- mWaitAllCount == 0u)
    {
        mWaitState.fetch_and(~STATE_WAIT_ALL, std::memory_order_release);
    }
}

inline bool WaitablePosix::has_wait_all_waiters() const noexcept
{
    return (mWaitState.load(std::memory_order_acquire) & STATE_WAIT_ALL) != 0u;
}

inline bool WaitablePosix::has_registered_waiters() const noexcept
{
    return (mWaitState.load(std::memory_order_relaxed) & STATE_WAITERS) != 0u;
}

inline std::atomic<uint32_t> & WaitablePosix::wait_state() noexcept
{
    return mWaitState;
}

inline const std::atomic<uint32_t> & WaitablePosix::wait_state() const noexcept
{
    return mWaitState;
}

#if defined(__linux__) || defined(__APPLE__) || defined(__CYGWIN__)

inline bool  WaitablePosix::has_waiters() const noexcept
{
    return (mWaiters.load(std::memory_order_acquire) != nullptr);
}

#endif  // defined(__linux__) || defined(__APPLE__) || defined(__CYGWIN__)

//////////////////////////////////////////////////////////////////////////
// SignalLockPosix class inline implementation
//////////////////////////////////////////////////////////////////////////

inline SignalLockPosix::SignalLockPosix( WaitablePosix & waitable ) noexcept
    : mLock     ( waitable.mWaitersLock )
#if defined(__linux__) || defined(__APPLE__) || defined(__CYGWIN__)
    , mWakeCount( 0 )
#endif  // defined(__linux__) || defined(__APPLE__) || defined(__CYGWIN__)
{
    mLock.lock();
}

} // namespace areg::os

#endif  // defined(_POSIX) || defined(POSIX)

#endif  // AREG_BASE_PRIVATE_POSIX_WAITABLEPOSIX_HPP
