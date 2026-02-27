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
#include "areg/base/GEGlobal.h"

#if defined(_POSIX) || defined(POSIX)

#include <pthread.h>
#include <atomic>

#ifdef __APPLE__
    #include <os/lock.h>
    typedef os_unfair_lock      pthread_spinlock_ix;
#else
    typedef pthread_spinlock_t  pthread_spinlock_ix;
#endif  // __APPLE__

namespace areg::os
{
    //////////////////////////////////////////////////////////////////////////
    // SpinLockPosix class declaration.
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   POSIX specific recursive spin-lock with atomic operations.
     **/
    class SpinLockPosix
    {
    //////////////////////////////////////////////////////////////////////////
    // Constructor / Destructor.
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Sets the instance of CriticalSectionPosix object, which is the 
         *          real POSIX spin-lock wrapper class.
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
         * \brief   Waits for ownership of spin-lock object. If the spin-lock
         *          object is currently owned by another thread, call of
         *          this function cause wait indefinitely for ownership.
         *          In contrast, when a mutex object is used for mutual exclusion,
         *          the lock() function accept a specified time-out interval.
         **/
        bool lock();

        /**
         * \brief   Releases ownership of the spin-lock object.
         * \return  Returns true if spin-lock owning thread called unlock.
         *          Otherwise, it returns false.
         **/
        bool unlock();

        /**
         * \brief   Attempts to take the spin-lock ownership without blocking thread.
         *          If the call is successful, the calling thread
         *          takes ownership of the spin-lock.
         * \return  If current thread successfully has taken the ownership or the thread
         *          already has the ownership of spin-lock, the return value is true.
         *          If another thread already owns the critical section,
         *          the return value is false.
         **/
        bool tryLock();

        /**
         * \brief   Returns true if spin lock is valid.
         **/
        inline bool isValid() const;

        /**
         * \brief   Free the spin-lock resources. Cannot run anymore
         **/
        void freeResources();

    //////////////////////////////////////////////////////////////////////////
    // Hidden calls
    //////////////////////////////////////////////////////////////////////////
    private:
        /**
         * \brief   Takes the ownership of critical section spin lock.
         * \return  Returns true if operation succeeded.
         */
        inline bool _lockSpin();
        /**
         * \brief   Releases the ownership of critical section spin lock.
         */
        inline void _unlockSpin();
        /**
         * \brief   Takes the ownership of spin lock to access resources of critical section.
         */
        inline void _lockIntern();
        /**
         * \brief   Releases the ownership of spin lock to access resources of critical section.
         */
        inline void _unlockIntern();

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
     * \brief   The automatic lock of spin-lock It is not used for any other objects.
     *          Use the spin lock for quick operations only.
     **/
    class SpinAutolockPosix
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
         * \brief   Manually locks the spin lock and returns the result.
         * \return  Returns true if operation succeeded.
         **/
        inline bool lock()
        {
            return mSpinLock.lock();
        }

        /**
         * \brief   Manually unlocks the spin lock and returns the result.
         * \return  Returns true if operation succeeded.
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

    inline bool SpinLockPosix::isValid() const
    {
        return mIsValid.load();
    }

} // namespace areg::os
#endif  // defined(_POSIX) || defined(POSIX)
#endif  // AREG_BASE_PRIVATE_POSIX_SPINLOCKPOSIX_HPP