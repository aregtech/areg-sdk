#ifndef AREG_BASE_PRIVATE_POSIX_CEWAITABLESEMAPHOREIX_HPP
#define AREG_BASE_PRIVATE_POSIX_CEWAITABLESEMAPHOREIX_HPP
/************************************************************************
 * \file        areg/base/private/posix/CEWaitableSemaphoreIX.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, POSIX Waitable Semaphore class.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/base/GEGlobal.h"

#ifdef _POSIX
#include "areg/base/private/posix/IEWaitableBaseIX.hpp"

//////////////////////////////////////////////////////////////////////////
// CEWaitableSemaphoreIX class declaration.
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
class CEWaitableSemaphoreIX : public IEWaitableBaseIX
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
    CEWaitableSemaphoreIX( int maxCount, int initCount = 0, const char * asciiName = NULL );

    /**
     * \brief   Destructor.
     **/
    virtual ~CEWaitableSemaphoreIX( void );

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
    bool ReleaseSemaphore( void );

    /**
     * \brief   Returns maximum count of semaphore.
     **/
    inline int GetMaximumCount( void ) const;

    /**
     * \brief   Returns current count of semaphore.
     **/
    inline int GetCurrentCount( void ) const;

/************************************************************************/
// IEWaitableBaseIX callback overrides.
/************************************************************************/

    /**
     * \brief   Returns true if the object is signaled. Otherwise, returns false.
     **/
    virtual bool IsSignaled( void ) const;

    /**
     * \brief   This call is triggered by the system when a thread completed waiting and released by semaphore.
     *          Semaphores do not have owner thread, but with this callback they are notified that a thread
     *          completed waiting for semaphore signal state.
     * \param   ownerThread     The ID of POSIX thread that requests to take ownership.
     * \return  In case of sSemaphores this always returns true.
     **/
    virtual bool RequestsOwnership( pthread_t ownerThread );

    /**
     * \brief   This method returns true if the count is more than one. Otherwise, it returns false.
     **/
    virtual bool CanSignalMultipleThreads( void ) const;

    /**
     * \brief   This callback is called to notify waitable the amount of threads that where released.
     * \param   numThreads  The amount of threads that where released.
     **/
    virtual void ThreadsReleased( int numThreads );

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Maximum count number. After setting cannot be changed.
     **/
    const int   mMaxCount;

    int         mCurCount;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden calls.
//////////////////////////////////////////////////////////////////////////
    CEWaitableSemaphoreIX( void );
    CEWaitableSemaphoreIX( const CEWaitableSemaphoreIX & /*src*/ );
    const CEWaitableSemaphoreIX & operator = ( const CEWaitableSemaphoreIX & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// CEWaitableSemaphoreIX class declaration.
//////////////////////////////////////////////////////////////////////////

inline int CEWaitableSemaphoreIX::GetMaximumCount(void) const
{   return mMaxCount;                               }

inline int CEWaitableSemaphoreIX::GetCurrentCount(void) const
{   CEObjectLockIX lock(*this); return mCurCount;   }

#endif  // _POSIX
#endif  // AREG_BASE_PRIVATE_POSIX_CEWAITABLESEMAPHOREIX_HPP
