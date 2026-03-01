#ifndef AREG_BASE_PRIVATE_POSIX_SPINLOCKPOSIX_HPP
#define AREG_BASE_PRIVATE_POSIX_SPINLOCKPOSIX_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/SpinLockPosix.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, OS specific spin-lock synchronization object.
 * 
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/areg_global.h"

#if defined(_POSIX) || defined(POSIX)

#include <pthread.h>
#include <atomic>

#ifdef __APPLE__
    #include <os/lock.h>
    typedef os_unfair_lock      pthread_spinlock_ix;
#else
    typedef pthread_spinlock_t  pthread_spinlock_ix;
#endif  // __APPLE__

namespace areg::os {

//////////////////////////////////////////////////////////////////////////
// SpinLockPosix class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   POSIX-specific recursive spin-lock with atomic operations.
 **/
class SpinLockPosix
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates and initializes the POSIX spin-lock.
     **/
    SpinLockPosix();

    /**
     * \brief   Destructor.
     **/
    ~SpinLockPosix();

//////////////////////////////////////////////////////////////////////////
// Operations.
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Override operations, SyncObject interface
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Acquires spin-lock ownership, waiting indefinitely if the lock is held by another
     *          thread.
     **/
    bool lock();

    /**
     * \brief   Releases ownership of the spin-lock.
     *
     * \return  Returns true if the spin-lock owning thread called unlock; false otherwise.
     **/
    bool unlock();

    /**
     * \brief   Attempts to acquire spin-lock ownership without blocking. Returns immediately.
     *
     * \return  Returns true if the current thread acquired ownership or already owns the spin-lock;
     *          false if another thread owns it.
     **/
    bool try_lock();

    /**
     * \brief   Returns true if the spin-lock is valid.
     **/
    inline bool is_valid() const;

    /**
     * \brief   Releases spin-lock resources. The lock cannot be used after this call.
     **/
    void free_resources();

//////////////////////////////////////////////////////////////////////////
// Hidden calls
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Acquires critical section spin-lock ownership.
     *
     * \return  Returns true if operation succeeded.
     **/
    inline bool _lock_spin();
    /**
     * \brief   Releases critical section spin-lock ownership.
     **/
    inline void _unlock_spin();
    /**
     * \brief   Acquires spin-lock for accessing critical section resources.
     **/
    inline void _lock_intern();
    /**
     * \brief   Releases spin-lock for accessing critical section resources.
     **/
    inline void _unlock_intern();

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:

    pthread_spinlock_ix     mSpinLock;  //!< The POSIX spin lock to synchronize multi-threading access of critical section.
    pthread_spinlock_ix     mInternLock;//!< The POSIX spin lock to synchronize internal structure resources.
    std::atomic<pthread_t>  mSpinOwner; //!< The spin-lock owner POSIX thread
    std::atomic<uint32_t>   mLockCount; //!< The lock counter to release spin lock when counter reaches zero.
    std::atomic<bool>       mIsValid;   //!< Flag, indicating whether the elements are initialized or not.

//////////////////////////////////////////////////////////////////////////
// Forbidden method calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( SpinLockPosix );
};

//////////////////////////////////////////////////////////////////////////
// SpinAutolockPosix class declaration to make recursive automatic lock of spin
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   RAII wrapper for automatic scoped locking of a POSIX spin-lock.
 **/
class SpinAutolockPosix
{
//////////////////////////////////////////////////////////////////////////
// COnstructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Acquires the spin-lock immediately upon construction. The lock is recursive and will
     *          not block the owning thread.
     *
     * \param   spinLock    Reference to the SpinLockPosix to acquire.
     **/
    inline SpinAutolockPosix( SpinLockPosix & spinLock )
        : mSpinLock ( spinLock )
    {
        mSpinLock.lock();
    }

    /**
     * \brief   Automatically unlocks the spin lock
     **/
    inline ~SpinAutolockPosix()
    {
        mSpinLock.unlock();
    }

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Manually acquires the spin-lock.
     *
     * \return  Returns true if the operation succeeded.
     **/
    inline bool lock()
    {
        return mSpinLock.lock();
    }

    /**
     * \brief   Manually releases the spin-lock.
     *
     * \return  Returns true if the operation succeeded.
     **/
    inline bool unlock()
    {
        return mSpinLock.unlock();
    }

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:

    SpinLockPosix &    mSpinLock;  //!< The valid instance of SpinLock object.

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    SpinAutolockPosix() = delete;
    AREG_NOCOPY_NOMOVE( SpinAutolockPosix );
};

//////////////////////////////////////////////////////////////////////////
// SpinLockPosix inline  methods
//////////////////////////////////////////////////////////////////////////

inline bool SpinLockPosix::is_valid() const
{
    return mIsValid.load();
}

} // namespace areg::os

#endif  // defined(_POSIX) || defined(POSIX)

#endif  // AREG_BASE_PRIVATE_POSIX_SPINLOCKPOSIX_HPP
