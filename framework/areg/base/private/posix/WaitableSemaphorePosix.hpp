#ifndef AREG_BASE_PRIVATE_POSIX_WAITABLESEMAPHOREIX_HPP
#define AREG_BASE_PRIVATE_POSIX_WAITABLESEMAPHOREIX_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/WaitableSemaphorePosix.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, POSIX Waitable Semaphore class.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/base/GEGlobal.h"

#if defined(_POSIX) || defined(POSIX)

#include "areg/base/private/posix/WaitablePosix.hpp"

//////////////////////////////////////////////////////////////////////////
// WaitableSemaphorePosix class declaration.
//////////////////////////////////////////////////////////////////////////

namespace areg::os
{
    /**
     * \brief   Waitable Semaphore is a synchronization object that maintains 
     *          count between 0 and its given maximum number. Each time a semaphore
     *          waiting thread completes waiting and is released, the count is 
     *          decremented. When the count reaches zero, the semaphore get non-signaled
     *          and any thread that waits for semaphore gets locked and stopped. 
     *          The count is increased again when a thread releases semaphore. 
     *          Unlike waitable Mutex, the semaphore can be released from any thread.
     *          The semaphore cannot have owning thread, because several threads
     *          can get released until the count reaches zero.
     **/
    class WaitableSemaphorePosix : public areg::os::WaitablePosix
    {
    //////////////////////////////////////////////////////////////////////////
    // Constructor / Destructor.
    //////////////////////////////////////////////////////////////////////////
    public:

        /**
         * \brief   Initializes semaphore. Sets maximum and initial count. The maximum 
         *          count should be more than zero. The initial count can be zero or more,
         *          but not more than the specified maximum.
         * \param   maxCount    The maximum count of semaphore.
         * \param   initCount   The initial count of semaphore.
         * \param   asciiName   The name of semaphore.
         **/
        explicit WaitableSemaphorePosix( int32_t maxCount, int32_t initCount = 0, const char * asciiName = nullptr );

        /**
         * \brief   Destructor.
         **/
        virtual ~WaitableSemaphorePosix() = default;

    //////////////////////////////////////////////////////////////////////////
    // Attributes and operations.
    //////////////////////////////////////////////////////////////////////////
    public:

        /**
         * \brief   Call to release semaphore and increase the count. Semaphores can be released
         *          from any thread. If count is greater than zero, semaphore waiting thread is
         *          released and continue to run. The count is decreased when a thread waits for
         *          a semaphore.
         * \return  Return true if operation succeeded and the count increased.
         **/
        bool releaseSemaphore();

        /**
         * \brief   Returns maximum count of semaphore.
         **/
        inline int32_t getMaximumCount() const;

        /**
         * \brief   Returns current count of semaphore.
         **/
        inline int32_t getCurrentCount() const;

    /************************************************************************/
    // WaitablePosix callback overrides.
    /************************************************************************/

        /**
         * \brief   Returns true if the object is signaled. Otherwise, returns false.
         * \param   contextThread   The thread ID where the lock and wait is checked.
         **/
        bool checkSignaled( pthread_t contextThread ) const override;

        /**
         * \brief   This call is triggered by the system when a thread completed waiting and released by semaphore.
         *          Semaphores do not have owner thread, but with this callback they are notified that a thread
         *          completed waiting for semaphore signal state.
         * \param   ownerThread     The ID of POSIX thread that requests to take ownership.
         * \return  In case of sSemaphores this always returns true.
         **/
        bool notifyRequestOwnership( pthread_t ownerThread ) override;

        /**
         * \brief   This method returns true if the count is more than one. Otherwise, it returns false.
         **/
        bool checkCanSignalMultipleThreads() const override;

        /**
         * \brief   This callback is called to notify waitable the amount of threads that where released.
         * \param   numThreads  The amount of threads that where released.
         **/
        void notifyReleasedThreads( int32_t numThreads ) override;

    //////////////////////////////////////////////////////////////////////////
    // Member variables.
    //////////////////////////////////////////////////////////////////////////
    private:
        /**
         * \brief   Maximum count number. After setting cannot be changed.
         **/
        const int32_t   mMaxCount;

        /**
         * \brief   Current lock count of semaphore.
         **/
        int32_t         mCurCount;

    //////////////////////////////////////////////////////////////////////////
    // Forbidden calls.
    //////////////////////////////////////////////////////////////////////////
    private:
        WaitableSemaphorePosix() = delete;
        AREG_NOCOPY_NOMOVE( WaitableSemaphorePosix );
    };

    //////////////////////////////////////////////////////////////////////////
    // WaitableSemaphorePosix class declaration.
    //////////////////////////////////////////////////////////////////////////

    inline int32_t WaitableSemaphorePosix::getMaximumCount() const
    {
        return mMaxCount;
    }

    inline int32_t WaitableSemaphorePosix::getCurrentCount() const
    {
        ObjectLockPosix lock(*this); return mCurCount;
    }

} // namespace areg::os
#endif  // defined(_POSIX) || defined(POSIX)
#endif  // AREG_BASE_PRIVATE_POSIX_WAITABLESEMAPHOREIX_HPP
