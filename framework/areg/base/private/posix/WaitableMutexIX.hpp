#ifndef AREG_BASE_PRIVATE_POSIX_WAITABLEMUTEXIX_HPP
#define AREG_BASE_PRIVATE_POSIX_WAITABLEMUTEXIX_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/WaitableMutexIX.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, POSIX Waitable Event class.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/base/GEGlobal.h"

#if defined(_POSIX) || defined(POSIX)

#include "areg/base/private/posix/IEWaitableBaseIX.hpp"

//////////////////////////////////////////////////////////////////////////
// WaitableMutexIX class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The synchronization waitable Mutex object is used to synchronize data access
 *          between multiple threads. If a Mutex is owned by a thread any attempts
 *          of other threads to take Mutex ownership will be automatically blocked and
 *          the threads will be stopped until Mutex is not released or the waiting timeout
 *          is not expired. There can be only one  thread at a time owning waitable Mutex
 *          and only owning thread can release the waitable Mutex to put to signaled
 *          state. The released state of Mute is considered as signaled and the thread
 *          owning state is non-signaled if it has mutex owning thread.
 **/
class WaitableMutexIX : public IEWaitableBaseIX
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes the synchronization waitable Mutex object, sets the Mutex owned flag.
     * \param   initOwned   If true, the Mutex is created non-signaled and owned by the 
     *                      current thread. The waitable Mutex gets ownership by calling one of
     *                      wait methods defined in SyncLockAndWaitIX. Once the thread gets 
     *                      ownership, any further waiting functions calls of the same thread
     *                      will not be blocked and stopped, so that the waiting Mutex can be
     *                      waited recursive. Any other thread that tries to get the ownership
     *                      of the waitable Mutex will be automatically locked and stopped by
     *                      the system. Once Mutex ownership is taken, it is in non-signaled state.
     *                      Only Mutex owning thread can unblock and release waitable Mutex.
     *                      The requests of other threads to release (signal) waitable Mutex
     *                      is ignored by the system.
     * \param   asciiName   The name of synchronization Event.
     **/
    explicit WaitableMutexIX(bool initOwned = false, const char * asciiName = nullptr);

    /**
     * \brief   Destructor.
     **/
    virtual ~WaitableMutexIX( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations.
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Call to release waitable Mutex and set to signaled state. Only owning thread
     *          can release waitable Mutex. The call from any other threads is ignored.
     *          When Mutex is signaled, the first released thread gets waitable Mutex ownership
     *          and immediately set to non-signaled state, so that no other thread can be
     *          released until mutex is not signaled again. Only released thread (owning thread)
     *          can release Mutex to set to signaled state again. This call is ignored if the 
     *          waitable Mutex is already in signaled state or method is from not owning
     *          thread context.
     * \return Returns true if operation succeeded.
     **/
    bool releaseMutex( void );

    /**
     * \brief   Returns waitable mutex owner thread ID, if there is any.
     **/
    inline pthread_t getOwningThreadId( void ) const;

/************************************************************************/
// IEWaitableBaseIX callback overrides.
/************************************************************************/

    /**
     * \brief   Returns true if the object is signaled. Otherwise, returns false.
     * \param   contextThread   The ID of thread where locking happened.
     *                          The mutex is signaled if owner thread is nullptr or the thread context
     *                          and owner threads are same.
     **/
    virtual bool checkSignaled( pthread_t contextThread ) const override;

    /**
     * \brief   This callback is triggered when a waiting thread is released to continue to run.
     *          Waitable Event always return true.
     * \param   ownerThread     Indicates the POSIX thread ID that completed to wait.
     * \return  Returns true if waitable Mutex did not have owner thread and successfully set 
     *          new owner thread. Only signaled waitable Mutex can assign thread ownership.
     *          Returns false, if waitable Mutex already has ownership and cannot take new.
     **/
    virtual bool notifyRequestOwnership( pthread_t ownerThread ) override;

    /**
     * \brief   This callback is triggered to when a system needs to know whether waitable
     *          can signal multiple threads. Returned 'true' value indicates that there can be
     *          multiple threads can get waitable signaled state. For example, waitable Mutex 
     *          signals only one thread, when waitable Event can signal multiple threads.
     * \return  Waitable Mutex always returns false.
     **/
    virtual bool checkCanSignalMultipleThreads( void ) const override;

    /**
     * \brief   This callback is called to notify the object the amount of
     *          threads that were leased when the object is in signaled state.
     *          The number of threads for waitable Mutex are 1 or 0.
     * \param   numThreads  The number of threads that where released when the
     *                      object is in signaled state. 0 means that no thread
     *                      was released by the object.
     **/
    virtual void notifyReleasedThreads( int numThreads ) override;

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The owner thread. The waitable Mutex is released thread ID is invalid.
     **/
    pthread_t       mOwnerThread;

    /**
     * \brief   The number of locks recursively called.
     **/
    int             mLockCount;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( WaitableMutexIX );
};

//////////////////////////////////////////////////////////////////////////
// WaitableMutexIX class inline functions
//////////////////////////////////////////////////////////////////////////

inline pthread_t WaitableMutexIX::getOwningThreadId(void) const
{
    ObjectLockIX lock(*this);
    return mOwnerThread;
}

#endif  // defined(_POSIX) || defined(POSIX)

#endif  // AREG_BASE_PRIVATE_POSIX_WAITABLEMUTEXIX_HPP
