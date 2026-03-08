#ifndef AREG_BASE_PRIVATE_POSIX_MUTEXPOSIX_HPP
#define AREG_BASE_PRIVATE_POSIX_MUTEXPOSIX_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/MutexPosix.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, POSIX Mutex wrapper class.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/base/areg_global.h"

#if defined(_POSIX) || defined(POSIX)

#include "areg/base/private/posix/SyncObjectPosix.hpp"
#include <pthread.h>
namespace areg::os {

//////////////////////////////////////////////////////////////////////////
// MutexPosix class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Synchronization mutex wrapper for POSIX mutexes. Optionally recursive. Used to protect
 *          shared resources and as a base class for other waitable synchronization objects.
 **/
class MutexPosix   : public SyncObjectPosix
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes a recursive POSIX mutex. If initLocked is true, the mutex is owned by
     *          the calling thread.
     *
     * \param   initLocked      If true, the mutex is initially locked and owned by the calling
     *                          thread. Other threads are blocked from acquiring it.
     * \param   asciiName       The name of the synchronization object.
     **/
    explicit MutexPosix(bool initLocked = false, const char* asciiName = nullptr);

    virtual ~MutexPosix();

protected:
    /**
     * \brief   Protected constructor for derived classes. Sets the synchronization type, recursion
     *          flag, and name.
     *
     * \param   syncType        The type of synchronization object.
     * \param   isRecursive     If true, the owning thread can lock the object multiple times
     *                          without deadlock. If false, recursive locks block the thread.
     * \param   asciiName       The name of the synchronization object.
     **/
    MutexPosix( areg::os::SyncKind syncType, bool isRecursive, const char * asciiName = nullptr );

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// MutexPosix operations.
/************************************************************************/

    /**
     * \brief   Locks the mutex. Blocks if the mutex is already locked, unless it is recursive and
     *          locked by the same thread.
     *
     * \param   msTimeout       The timeout in milliseconds. Use areg::DO_NOT_WAIT for
     *                          non-blocking lock, or areg::WAIT_INFINITE to wait indefinitely.
     * \return  Returns true if successfully locked. Returns false if timeout expired or lock
     *          failed.
     **/
    bool lock(uint32_t msTimeout = areg::WAIT_INFINITE) const;

    /**
     * \brief   Attempts to lock the mutex without blocking. Returns immediately regardless of
     *          success.
     *
     * \return  Returns true if successfully acquired the lock; false otherwise.
     **/
    bool try_lock() const;

    /**
     * \brief   Releases the mutex. Only the owning thread can unlock. Calls from other threads are
     *          ignored.
     **/
    void unlock() const;

/************************************************************************/
// SyncObjectPosix overrides.
/************************************************************************/
    /**
     * \brief   Returns true if the synchronization object is valid.
     **/
    [[nodiscard]]
    bool is_valid() const noexcept override;

    /**
     * \brief   Releases all resources held by the synchronization object. Called when the object is
     *          being destroyed.
     **/
    void free_resources() override;

//////////////////////////////////////////////////////////////////////////
// MutexPosix class implementation
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Initializes the POSIX mutex with the specified recursion flag. If recursive, the
     *          owning thread can lock multiple times without deadlock.
     *
     * \param   isRecursive     If true, creates a recursive mutex. If false, recursive locks from
     *                          the same thread cause deadlock.
     **/
    inline void _init_posix_mutex( bool isRecursive );

//////////////////////////////////////////////////////////////////////////
// MutexPosix class implementation
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   POSIX mutex.
     **/
    mutable pthread_mutex_t     mPosixMutex;
    /**
     * \brief   Mutex validity flag.
     **/
    mutable bool                mMutexValid;
    /**
     * \brief   POSIX mutex attribute.
     **/
    mutable pthread_mutexattr_t mPosixMutexAttr;
    /**
     * \brief   Mutex Attribute validity flag.
     **/
    mutable bool                mMutexAttrValid;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( MutexPosix );
};

//////////////////////////////////////////////////////////////////////////
// ObjectLockPosix class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   RAII wrapper for automatic scoped locking of a POSIX mutex.
 **/
class ObjectLockPosix
{
//////////////////////////////////////////////////////////////////////////
// Constructor / destructor.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes the lock with the specified mutex. If autoLock is true, immediately
     *          acquires the mutex.
     *
     * \param   mutex       Reference to the POSIX mutex wrapper object.
     * \param   autoLock    If true, automatically locks on construction.
     **/
    inline ObjectLockPosix( const MutexPosix & mutex, bool autoLock = true );

    inline ~ObjectLockPosix();

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Acquires the mutex. Returns immediately if successful, or after the timeout expires.
     *
     * \param   msTimeout       Timeout in milliseconds. WAIT_INFINITE waits indefinitely.
     * \return  Returns true if mutex ownership was acquired; false if timeout expired or error
     *          occurred.
     **/
    inline bool lock( uint32_t msTimeout = areg::WAIT_INFINITE ) const;

    /**
     * \brief   Releases the mutex so other threads can acquire it.
     **/
    inline void unlock() const;

//////////////////////////////////////////////////////////////////////////
// Constructor / destructor.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Instance of POSIX mutex wrapper object.
     **/
    MutexPosix & mMutex;
    /**
     * \brief   Flag, indicating whether mutex automatically locked / unlocked or not.
     *          The automatic locking mutexes are automatically released in destructor.
     **/
    const bool  mAutolock;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    ObjectLockPosix() = delete;
    AREG_NOCOPY_NOMOVE( ObjectLockPosix );
};

//////////////////////////////////////////////////////////////////////////
// ObjectLockPosix inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline ObjectLockPosix::ObjectLockPosix( const MutexPosix & mutex, bool autoLock /*= true*/ )
    : mMutex    (const_cast<MutexPosix &>(mutex))
    , mAutolock (autoLock)
{
    if (autoLock)
    {
        mMutex.lock(areg::WAIT_INFINITE);
    }
}

inline ObjectLockPosix::~ObjectLockPosix()
{
    if (mAutolock)
    {
        mMutex.unlock();
    }
}

inline bool ObjectLockPosix::lock( uint32_t msTimeout /*= areg::WAIT_INFINITE*/ ) const
{
    return mMutex.lock(msTimeout);
}

inline void ObjectLockPosix::unlock() const
{
    mMutex.unlock();
}

} // namespace areg::os

#endif  // defined(_POSIX) || defined(POSIX)

#endif  // AREG_BASE_PRIVATE_POSIX_MUTEXPOSIX_HPP
