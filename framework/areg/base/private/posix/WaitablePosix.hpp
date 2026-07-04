#ifndef AREG_BASE_PRIVATE_POSIX_WAITABLEPOSIX_HPP
#define AREG_BASE_PRIVATE_POSIX_WAITABLEPOSIX_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
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

//////////////////////////////////////////////////////////////////////////
// SyncWaitable class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Base class for POSIX waitable synchronization objects. Supports single or group waiting.
 *          Cannot be instantiated directly; use derived classes.
 **/
class WaitablePosix : public SyncObjectPosix
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Protected constructor. Sets synchronization type and name.
     **/
    WaitablePosix( areg::os::SyncKind syncType);

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
    inline bool is_valid() const noexcept override;

    /**
     * \brief   Called before deletion. Releases all waiting threads with failure code.
     **/
    void free_resources() final;

    /**
     * \brief   Called by SyncLockAndWaitPosix when registering this waitable in the WaitAll
     *          global map. Tracks the count of active WaitAll waiters so event_signaled() can
     *          skip the global map lock entirely when no WaitAll waiter is present.
     **/
    inline void add_wait_all_registration() noexcept;

    /**
     * \brief   Called by SyncLockAndWaitPosix when deregistering this waitable from the WaitAll global map.
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
     * \brief   Walks the intrusive waiter list and fires each eligible WaitAny waiter.
     *
     * \return  Returns the number of waiters woken.
     **/
    int32_t notify_any_waiters() noexcept;

    /**
     * \brief   Returns true if at least one waiter is currently parked on this object's intrusive list.
     *          Lets a signaler wake a parked waiter even after the false->true edge passed.
     **/
    [[nodiscard]]
    inline bool has_waiters() const noexcept;

private:
    //!< Head of the per-object intrusive WaiterNode list; nullptr when empty.
    std::atomic<WaiterNode*>    mWaiters;
    //!< Spinlock protecting mWaiters list mutations.
    std::atomic_flag            mWaitersLock;

#endif  // defined(__linux__) || defined(__APPLE__) || defined(__CYGWIN__)

private:
    //! Set to false by free_resources() to signal the object is no longer usable.
    std::atomic<bool>        mValid;
    //! Number of active WaitAll (SyncLockAndWaitPosix) registrations.
    std::atomic<uint32_t>    mWaitAllCount;

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
// WaitablePosix class inline implementation
//////////////////////////////////////////////////////////////////////////

inline void WaitablePosix::add_wait_all_registration() noexcept
{
    mWaitAllCount.fetch_add(1u, std::memory_order_relaxed);
}

inline void WaitablePosix::remove_wait_all_registration() noexcept
{
    mWaitAllCount.fetch_sub(1u, std::memory_order_relaxed);
}

inline bool WaitablePosix::has_wait_all_waiters() const noexcept
{
    return mWaitAllCount.load(std::memory_order_relaxed) > 0u;
}

inline bool WaitablePosix::is_valid() const noexcept
{
    return mValid.load(std::memory_order_relaxed);
}

inline bool  WaitablePosix::has_waiters() const noexcept
{
    return (mWaiters.load(std::memory_order_acquire) != nullptr);
}

} // namespace areg::os

#endif  // defined(_POSIX) || defined(POSIX)

#endif  // AREG_BASE_PRIVATE_POSIX_WAITABLEPOSIX_HPP
