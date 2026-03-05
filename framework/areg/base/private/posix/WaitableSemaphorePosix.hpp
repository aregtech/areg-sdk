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
#include "areg/base/areg_global.h"

#if defined(_POSIX) || defined(POSIX)

#include "areg/base/private/posix/WaitablePosix.hpp"
namespace areg::os {

//////////////////////////////////////////////////////////////////////////
// WaitableSemaphorePosix class declaration.
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   POSIX synchronization semaphore. Maintains a count between 0 and a maximum. Threads
 *          decrement the count on wait and increment it when released. Semaphores can be released
 *          from any thread and support multi-thread signaling.
 **/
class WaitableSemaphorePosix : public WaitablePosix
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor.
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Initializes semaphore with maximum and initial counts. Initial count must not exceed
     *          maximum.
     *
     * \param   maxCount        Maximum count; must be greater than zero.
     * \param   initCount       Initial count; must not exceed maximum.
     * \param   asciiName       Semaphore name (optional).
     **/
    explicit WaitableSemaphorePosix( int32_t maxCount, int32_t initCount = 0, const char * asciiName = nullptr );

    virtual ~WaitableSemaphorePosix() = default;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations.
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Releases the semaphore and increments the count. If count becomes greater than zero,
     *          a waiting thread is released. Can be called from any thread.
     *
     * \return  Returns true if the count was incremented successfully.
     **/
    bool release_semaphore();

    /**
     * \brief   Returns the maximum count of the semaphore.
     **/
    inline int32_t maximum_count() const;

    /**
     * \brief   Returns the current count of the semaphore.
     **/
    inline int32_t current_count() const;

/************************************************************************/
// WaitablePosix callback overrides.
/************************************************************************/

    /**
     * \brief   Returns true if the semaphore is signaled.
     *
     * \param   contextThread       The thread ID where the signaled state is checked.
     **/
    bool check_signaled( pthread_t contextThread ) const override;

    /**
     * \brief   Callback when a waiting thread is released. Semaphores do not have owning threads
     *          but are notified of release events.
     *
     * \param   ownerThread     The thread ID that was released.
     * \return  Always returns true for semaphores.
     **/
    bool notify_request_ownership( pthread_t ownerThread ) override;

    /**
     * \brief   Returns true if the count is greater than one, indicating multiple threads can be
     *          signaled.
     **/
    bool can_signal_threads() const override;

    /**
     * \brief   Called to notify the semaphore how many threads were released.
     *
     * \param   numThreads      The number of threads that were released.
     **/
    void notify_released_threads( int32_t numThreads ) override;

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
    /**
     * \brief   Default constructor is deleted.
     **/
    WaitableSemaphorePosix() = delete;
    AREG_NOCOPY_NOMOVE( WaitableSemaphorePosix );
};

//////////////////////////////////////////////////////////////////////////
// WaitableSemaphorePosix class declaration.
//////////////////////////////////////////////////////////////////////////

inline int32_t WaitableSemaphorePosix::maximum_count() const
{
    return mMaxCount;
}

inline int32_t WaitableSemaphorePosix::current_count() const
{
    ObjectLockPosix lock(*this); return mCurCount;
}

} // namespace areg::os

#endif  // defined(_POSIX) || defined(POSIX)

#endif  // AREG_BASE_PRIVATE_POSIX_WAITABLESEMAPHOREIX_HPP
