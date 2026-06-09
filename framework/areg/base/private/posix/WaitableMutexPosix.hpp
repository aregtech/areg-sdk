#ifndef AREG_BASE_PRIVATE_POSIX_WAITABLEMUTEXIX_HPP
#define AREG_BASE_PRIVATE_POSIX_WAITABLEMUTEXIX_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/WaitableMutexPosix.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, POSIX Waitable Event class.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/base/areg_global.h"

#if defined(_POSIX) || defined(POSIX)

#include "areg/base/private/posix/WaitablePosix.hpp"
#include "areg/base/private/posix/MutexPosix.hpp"

namespace areg::os {

//////////////////////////////////////////////////////////////////////////
// WaitableMutexPosix class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Recursive synchronization mutex for protecting data access across multiple threads. Only
 *          the owning thread can release the mutex. Threads waiting for an owned mutex are
 *          automatically blocked until the owner releases it or timeout expires.
 **/
class WaitableMutexPosix final  : public WaitablePosix
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes the synchronization mutex. If initOwned is true, the mutex is owned by
     *          the calling thread and other threads are blocked from acquiring it.
     *
     * \param   initOwned       If true, the mutex is initially owned by the current thread. The
     *                          mutex is recursive, so the owning thread can lock it multiple times
     *                          without deadlock. Other threads are blocked until the owner releases it.
     **/
    explicit WaitableMutexPosix(bool initOwned = false);

    virtual ~WaitableMutexPosix() = default;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations.
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Releases the mutex ownership. Only the owning thread can release it. When released,
     *          the first waiting thread acquires ownership.
     *
     * \return  Returns true if successfully released. Returns false if the calling thread does not
     *          own the mutex or the mutex is already in signaled state.
     **/
    bool release_mutex() noexcept;

    /**
     * \brief   Returns the POSIX thread ID that currently owns the mutex, or null if no thread owns it.
     **/
    [[nodiscard]]
    inline pthread_t owning_thread_id() const noexcept;

/************************************************************************/
// WaitablePosix callback overrides.
/************************************************************************/

    /**
     * \brief   Returns true if the mutex is signaled (not owned) or owned by the calling thread.
     *
     * \param   contextThread       The thread ID to check. The mutex is signaled if it has no owner
     *                              or the owner matches this thread.
     * \return  Returns true if the mutex is available to the calling thread.
     **/
    bool check_signaled( pthread_t contextThread ) const final;

    /**
     * \brief   Callback invoked when a waiting thread is released to acquire mutex ownership.
     *
     * \param   ownerThread     The POSIX thread ID that completed waiting.
     * \return  Returns true if the thread successfully acquired ownership. Returns false if the
     *          mutex already has an owner.
     **/
    bool notify_request_ownership( pthread_t ownerThread ) final;

    /**
     * \brief   Returns false to indicate that the mutex can signal only one thread at a time.
     **/
    bool can_signal_threads() const noexcept final;

    /**
     * \brief   Notifies the mutex that one or zero threads were released when it was in signaled state.
     *
     * \param   numThreads      The number of threads released. Always 0 or 1 for mutex.
     **/
    void notify_released_threads( int32_t numThreads ) final;

    /**
     * \brief   Returns true if the internal object lock is valid.
     **/
    [[nodiscard]]
    bool is_valid() const noexcept final;

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    //!< Internal mutex protecting mOwnerThread and mLockCount.
    MutexPosix  mObjectLock;

    //!< The owner thread. The waitable Mutex is released thread ID is invalid.
    pthread_t   mOwnerThread;

    //!< The number of locks recursively called.
    int32_t     mLockCount;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( WaitableMutexPosix );
};

//////////////////////////////////////////////////////////////////////////
// WaitableMutexPosix class inline functions
//////////////////////////////////////////////////////////////////////////

inline bool WaitableMutexPosix::can_signal_threads() const noexcept
{
    return false;
}

#ifdef  DEBUG
inline void WaitableMutexPosix::notify_released_threads(int32_t numThreads)
{
    ASSERT((numThreads == 1) || (numThreads == 0));
}
#else   // DEBUG
inline void WaitableMutexPosix::notify_released_threads(int32_t /*numThreads*/)
{
}
#endif  // DEBUG

inline bool WaitableMutexPosix::check_signaled(pthread_t contextThread) const
{
    ObjectLockPosix lock(mObjectLock);
    return (mOwnerThread == static_cast<pthread_t>(0)) || (mOwnerThread == contextThread);
}

inline bool WaitableMutexPosix::is_valid() const noexcept
{
    return mObjectLock.is_valid();
}

inline pthread_t WaitableMutexPosix::owning_thread_id() const noexcept
{
    ObjectLockPosix lock(mObjectLock);
    return mOwnerThread;
}

} // namespace areg::os

#endif  // defined(_POSIX) || defined(POSIX)

#endif  // AREG_BASE_PRIVATE_POSIX_WAITABLEMUTEXIX_HPP
