#ifndef AREG_BASE_PRIVATE_POSIX_MUTEXIX_HPP
#define AREG_BASE_PRIVATE_POSIX_MUTEXIX_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/MutexIX.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, POSIX Mutex wrapper class.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/base/GEGlobal.h"

#if defined(_POSIX) || defined(POSIX)

#include "areg/base/private/posix/IESynchObjectBaseIX.hpp"
#include <pthread.h>

//////////////////////////////////////////////////////////////////////////
// MutexIX class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A simple synchronization object to synchronize resource access.
 *          This is a wrapper class of POSIX mutex. It is used either
 *          to synchronize simple objects or is a base class for other
 *          waitable synchronization objects that can trigger event to
 *          a waiting thread.
 **/
class MutexIX   : public IESynchObjectBaseIX
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes MutexIX. If the object is initialized
     *          via this constructor, it is specified as recursive.
     *          If a thread owns the synchronization object, any
     *          further request to lock the object within the same
     *          thread context will not block the thread.
     *          Synchronization object initialized via this constructor
     *          cannot be used in event waiting operations.
     * \param   initLocked  If set true, the object is immediately
     *                      owned by calling thread, so that all other
     *                      threads are automatically blocked. Only
     *                      owning thread can unlock the object.
     * \param   asciiName   The name of synchronization object.
     **/
    explicit MutexIX(bool initLocked = false, const char* asciiName = nullptr);

    /**
     * \brief   Destructor.
     **/
    virtual ~MutexIX(void);

protected:
    /**
     * \brief   Protected constructor called by child classes to set the synchronization object type,
     *          recursion flag and the name.If set as recursive, the owning thread is not blocked 
     *          if locks object again. Only owning thread can unlock locked object.
     *          Objects created with this constructor can be used event waiting operations.
     *          This constructor is used by child objects.
     * \param   synchType       The type of synchronization object.
     * \param   isRecursive     If true, the object can be recursively locked
     *                          by the same thread. If set fails, once the
     *                          object is locked, second call to lock the object
     *                          will block the thread even if same thread
     *                          locked the object.
     * \param   asciiName       The name of synchronization object.
     **/
    MutexIX( NESynchTypesIX::eSynchObject synchType, bool isRecursive, const char * asciiName = nullptr );

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// MutexIX operations.
/************************************************************************/

    /**
     * \brief   Locks the object. Depending on type and the sate of object,
     *          and the recursive flag, the thread can be blocked or not.
     *          In recursive objects the call to request is not blocked by
     *          the same thread if method is called within same thread context.
     *          If timeout specified, it will be locked until either object
     *          is not released or timeout expired.
     * \param   msTimeout   The timeout is milliseconds to stay locked.
     *                      If specified NECommon::DO_NOT_WAIT the
     *                      function returned immediately independent whether
     *                      it is locked or not. If timeout specified
     *                      NECommon::WAIT_INFINITE it will remain
     *                      blocked until object is not released.
     * \return  Returns true if succeeded to lock the object. Returns false
     *          if either failed to lock or the timeout expired.
     **/
    bool lock(unsigned int msTimeout = NECommon::WAIT_INFINITE) const;

    /**
     * \brief   This function returns immediately without blocking.
     *          If the object is released and the owner ship is taken
     *          by current thread, the function returns true. Otherwise,
     *          it returns false.
     **/
    bool tryLock( void ) const;

    /**
     * \brief   Call to unlock / release the ownership of the object.
     *          Only the thread that locked the object can unlock it.
     *          In all other cases the call is ignored.
     **/
    void unlock( void ) const;

/************************************************************************/
// IESynchObjectBaseIX overrides.
/************************************************************************/
    /**
     * \brief   Returns true if synchronization object is valid.
     **/
    virtual bool isValid( void ) const override;

    /**
     * \brief   Triggered when synchronization object is going to be deleted.
     *          This should free all resources.
     **/
    virtual void freeResources( void ) override;

//////////////////////////////////////////////////////////////////////////
// MutexIX class implementation
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Initializes POSIX mutex.
     * \param   isRecursive     If true, the POSIX mutex is created with permission to lock
     *                          recursively within the same thread context. In this case
     *                          the ownership by thread is taken only once and the further calls
     *                          will not lock the thread. Otherwise, the locking should be
     *                          carefully called within same context, because if the ownership
     *                          already is taken, the second call will blocked and this will cause
     *                          deadlock situation.
     **/
    inline void _initPosixMutex( bool isRecursive );

//////////////////////////////////////////////////////////////////////////
// MutexIX class implementation
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
    DECLARE_NOCOPY_NOMOVE( MutexIX );
};

//////////////////////////////////////////////////////////////////////////
// ObjectLockIX class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Simple auto-locking object for single synchronization object.
 *          The object can lock synchronization object when instantiated
 *          and release when the destructor is called. Other helper methods
 *          can help to lock and unlock synchronization object during run.
 **/
class ObjectLockIX
{
//////////////////////////////////////////////////////////////////////////
// Constructor / destructor.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes the instance of POSIX mutex wrapper object and
     *          if needed automatically locks.
     * \param   mutex       The instance of POSIX mutex wrapper object.
     * \param   autoLock    If true, the mutex is automatically locked when
     *                      this object is instantiated.
     **/
    inline ObjectLockIX( const MutexIX & mutex, bool autoLock = true );

    /**
     * \brief   If previously the object is locked, it will be unlocked in destructor.
     **/
    inline ~ObjectLockIX( void );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Locks the mutex. this call either returns immediately because
     *          the mutex was release and it succeeded to take ownership,
     *          or returns when timeout expired.
     * \param   msTimeout   Timeout in milliseconds to wait when requesting mutex
     *                      mutex ownership. If value is equal to
     *                      NECommon::WAIT_INFINITE the thread will be locked until
     *                      does not get the ownership.
     * \return  Returns true if succeeded to take mutex ownership.
     *          Otherwise, if timeout expired or error happened, it returns false.
     **/
    inline bool lock( unsigned int msTimeout = NECommon::WAIT_INFINITE ) const;

    /**
     * \brief   Call to unlock mutex to let other threads to take the ownership.
     **/
    inline void unlock( void ) const;

//////////////////////////////////////////////////////////////////////////
// Constructor / destructor.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Instance of POSIX mutex wrapper object.
     **/
    MutexIX & mMutex;
    /**
     * \brief   Flag, indicating whether mutex automatically locked / unlocked or not.
     *          The automatic locking mutexes are automatically released in destructor.
     **/
    const bool  mAutolock;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    ObjectLockIX( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( ObjectLockIX );
};

//////////////////////////////////////////////////////////////////////////
// ObjectLockIX inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline ObjectLockIX::ObjectLockIX( const MutexIX & mutex, bool autoLock /*= true*/ )
    : mMutex    (const_cast<MutexIX &>(mutex))
    , mAutolock (autoLock)
{
    if (autoLock)
    {
        mMutex.lock(NECommon::WAIT_INFINITE);
    }
}

inline ObjectLockIX::~ObjectLockIX( void )
{
    if (mAutolock)
    {
        mMutex.unlock();
    }
}

inline bool ObjectLockIX::lock( unsigned int msTimeout /*= NECommon::WAIT_INFINITE*/ ) const
{
    return mMutex.lock(msTimeout);
}

inline void ObjectLockIX::unlock( void ) const
{
    mMutex.unlock();
}

#endif  // defined(_POSIX) || defined(POSIX)

#endif  // AREG_BASE_PRIVATE_POSIX_MUTEXIX_HPP
