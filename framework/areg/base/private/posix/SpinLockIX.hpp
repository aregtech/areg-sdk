#ifndef AREG_BASE_PRIVATE_POSIX_SPINLOCKIX_HPP
#define AREG_BASE_PRIVATE_POSIX_SPINLOCKIX_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/SpinLockIX.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, OS specific spin-lock synchronization object.
 * 
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/GEGlobal.h"

#if defined(_POSIX) || defined(POSIX)

#include <pthread.h>
#include <atomic>

//////////////////////////////////////////////////////////////////////////
// SpinLockIX class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   POSIX specific recursive spin-lock with atomic operations.
 **/
class SpinLockIX
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Sets the instance of CriticalSectionIX object, which is the 
     *          real POSIX spin-lock wrapper class.
     **/
    SpinLockIX( void );

    /**
     * \brief   Destructor.
     **/
    ~SpinLockIX( void );

//////////////////////////////////////////////////////////////////////////
// Operations.
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Override operations, IESynchObject interface
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Waits for ownership of spin-lock object. If the spin-lock
     *          object is currently owned by another thread, call of
     *          this function cause wait indefinitely for ownership.
     *          In contrast, when a mutex object is used for mutual exclusion,
     *          the lock() function accept a specified time-out interval.
     **/
    bool lock( void );

    /**
     * \brief   Releases ownership of the spin-lock object.
     * \return  Returns true if spin-lock owning thread called unlock.
     *          Otherwise, it returns false.
     **/
    bool unlock( void );

    /**
     * \brief   Attempts to take the spin-lock ownership without blocking thread.
     *          If the call is successful, the calling thread
     *          takes ownership of the spin-lock.
     * \return  If current thread successfully has taken the ownership or the thread
     *          already has the ownership of spin-lock, the return value is true.
     *          If another thread already owns the critical section,
     *          the return value is false.
     **/
    bool tryLock( void );

    /**
     * \brief   Returns true if spin lock is valid.
     **/
    inline bool isValid( void ) const;

    /**
     * \brief   Free the spin-lock resources. Cannot run anymore
     **/
    void freeResources( void );

//////////////////////////////////////////////////////////////////////////
// Hidden calls
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Takes the ownership of critical section spin lock.
     * \return  Returns true if operation succeeded.
     */
    inline bool _lockSpin( void );
    /**
     * \brief   Releases the ownership of critical section spin lock.
     */
    inline void _unlockSpin( void );
    /**
     * \brief   Takes the ownership of spin lock to access resources of critical section.
     */
    inline void _lockIntern( void );
    /**
     * \brief   Releases the ownership of spin lock to access resources of critical section.
     */
    inline void _unlockIntern( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:

    pthread_spinlock_t      mSpinLock;  //!< The POSIX spin lock to synchronize multi-threading access of critical section.
    pthread_spinlock_t      mInternLock;//!< The POSIX spin lock to synchronize internal structure resources.
    std::atomic<pthread_t>  mSpinOwner; //!< The spin-lock owner POSIX thread
    std::atomic<uint32_t>   mLockCount; //!< The lock counter to release spin lock when counter reaches zero.
    std::atomic<bool>       mIsValid;   //!< Flag, indicating whether the elements are initialized or not.

//////////////////////////////////////////////////////////////////////////
// Forbidden method calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( SpinLockIX );
};

//////////////////////////////////////////////////////////////////////////
// SpinAutolockIX class declaration to make recursive automatic lock of spin
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   The automatic lock of spin-lock It is not used for any other objects.
 *          Use the spin lock for quick operations only.
 **/
class SpinAutolockIX
{
//////////////////////////////////////////////////////////////////////////
// COnstructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Initializes the SpinLock object and immediately locks it.
     *          Since the lock can be done recursive, it will not block
     *          the spin owning thread.
     **/
    inline SpinAutolockIX( SpinLockIX & spinLock )
        : mSpinLock ( spinLock )
    {
        mSpinLock.lock();
    }

    /**
     * \brief   Automatically unlocks the spin lock
     **/
    inline ~SpinAutolockIX( void )
    {
        mSpinLock.unlock();
    }

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Manually locks the spin lock and returns the result.
     * \return  Returns true if operation succeeded.
     **/
    inline bool lock( void )
    {
        return mSpinLock.lock();
    }

    /**
     * \brief   Manually unlocks the spin lock and returns the result.
     * \return  Returns true if operation succeeded.
     **/
    inline bool unlock( void )
    {
        return mSpinLock.unlock();
    }

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:

    SpinLockIX &    mSpinLock;  //!< The valid instance of SpinLock object.

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    SpinAutolockIX( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( SpinAutolockIX );
};

//////////////////////////////////////////////////////////////////////////
// SpinLockIX inline  methods
//////////////////////////////////////////////////////////////////////////

inline bool SpinLockIX::isValid( void ) const
{
    return mIsValid.load();
}

#endif  // defined(_POSIX) || defined(POSIX)

#endif  // AREG_BASE_PRIVATE_POSIX_SPINLOCKIX_HPP
