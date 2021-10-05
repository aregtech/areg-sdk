#pragma once
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/WaitableSemaphoreIX.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, POSIX Waitable Semaphore class.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/base/GEGlobal.h"

#if defined(_POSIX) || defined(POSIX)

#include "areg/base/private/posix/IEWaitableBaseIX.hpp"

//////////////////////////////////////////////////////////////////////////
// WaitableSemaphoreIX class declaration.
//////////////////////////////////////////////////////////////////////////

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
class WaitableSemaphoreIX : public IEWaitableBaseIX
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
    explicit WaitableSemaphoreIX( int maxCount, int initCount = 0, const char * asciiName = nullptr );

    /**
     * \brief   Destructor.
     **/
    virtual ~WaitableSemaphoreIX( void ) = default;

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
    bool releaseSemaphore( void );

    /**
     * \brief   Returns maximum count of semaphore.
     **/
    inline int getMaximumCount( void ) const;

    /**
     * \brief   Returns current count of semaphore.
     **/
    inline int getCurrentCount( void ) const;

/************************************************************************/
// IEWaitableBaseIX callback overrides.
/************************************************************************/

    /**
     * \brief   Returns true if the object is signaled. Otherwise, returns false.
     * \param   contextThread   The thread ID where the lock and wait is checked.
     **/
    virtual bool checkSignaled( pthread_t contextThread ) const override;

    /**
     * \brief   This call is triggered by the system when a thread completed waiting and released by semaphore.
     *          Semaphores do not have owner thread, but with this callback they are notified that a thread
     *          completed waiting for semaphore signal state.
     * \param   ownerThread     The ID of POSIX thread that requests to take ownership.
     * \return  In case of sSemaphores this always returns true.
     **/
    virtual bool notifyRequestOwnership( pthread_t ownerThread ) override;

    /**
     * \brief   This method returns true if the count is more than one. Otherwise, it returns false.
     **/
    virtual bool checkCanSignalMultipleThreads( void ) const override;

    /**
     * \brief   This callback is called to notify waitable the amount of threads that where released.
     * \param   numThreads  The amount of threads that where released.
     **/
    virtual void notifyReleasedThreads( int numThreads ) override;

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Maximum count number. After setting cannot be changed.
     **/
    const int   mMaxCount;

    /**
     * \brief   Current lock count of semaphore.
     **/
    int         mCurCount;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    WaitableSemaphoreIX( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( WaitableSemaphoreIX );
};

//////////////////////////////////////////////////////////////////////////
// WaitableSemaphoreIX class declaration.
//////////////////////////////////////////////////////////////////////////

inline int WaitableSemaphoreIX::getMaximumCount(void) const
{
    return mMaxCount;
}

inline int WaitableSemaphoreIX::getCurrentCount(void) const
{
    ObjectLockIX lock(*this); return mCurCount;
}

#endif  // defined(_POSIX) || defined(POSIX)
