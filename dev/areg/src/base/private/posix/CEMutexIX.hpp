#ifndef AREG_BASE_PRIVATE_POSIX_CEMUTEXIX_HPP
#define AREG_BASE_PRIVATE_POSIX_CEMUTEXIX_HPP
/************************************************************************
 * \file        areg/src/base/private/posix/CEMutexIX.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, POSIX Mutex wrapper class.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/src/base/GEGlobal.h"

#ifdef _POSIX
#include "areg/src/base/private/posix/IESynchObjectBaseIX.hpp"
#include <pthread.h>

//////////////////////////////////////////////////////////////////////////
// CEMutexIX class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A simple synchronization object to synchronize resource access.
 *          This is a wrapper class of POSIX mutex. It is used either
 *          to synchronize simple objects or is a base class for other
 *          waitable synchronization objects that can trigger event to
 *          a waiting thread.
 **/
class CEMutexIX       : public IESynchObjectBaseIX
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes CEMutexIX. If the object is initialized
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
    CEMutexIX(bool initLocked = false, const char* asciiName = NULL);

    /**
     * \brief   Destructor.
     **/
    virtual ~CEMutexIX(void);

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
    CEMutexIX( NESynchTypesIX::eSynchObject synchType, bool isRecursive, const char * asciiName = NULL );

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// CEMutexIX operations.
/************************************************************************/

    /**
     * \brief   Locks the object. Depending on type and the sate of object,
     *          and the recursive flag, the thread can be blocked or not.
     *          In recursive objects the call to request is not blocked by
     *          the same thread if method is called within same thread context.
     *          If timeout specified, it will be locked until either object
     *          is not released or timeout expired.
     * \param   msTimeout   The timeout is milliseconds to stay locked.
     *                      If specified IESynchObject::DO_NOT_WAIT the
     *                      function returned immediately independent whether
     *                      it is locked or not. If timeout specified
     *                      IESynchObject::WAIT_INFINITE it will remain
     *                      blocked until object is not released.
     * \return  Returns true if succeeded to lock the object. Returns false
     *          if either failed to lock or the timeout expired.
     **/
    bool LockObject(unsigned int msTimeout = IESynchObject::WAIT_INFINITE) const;

    /**
     * \brief   This function returns immediately without blocking.
     *          If the object is released and the owner ship is taken
     *          by current thread, the function returns true. Otherwise,
     *          it returns false.
     **/
    bool TryLockObject( void ) const;

    /**
     * \brief   Call to unlock / release the ownership of the object.
     *          Only the thread that locked the object can unlock it.
     *          In all other cases the call is ignored.
     **/
    void UnlockObject( void ) const;

/************************************************************************/
// IESynchObjectBaseIX overrides.
/************************************************************************/
    /**
     * \brief   Returns true if synchronization object is valid.
     **/
    virtual bool IsValid( void ) const;

    /**
     * \brief   Triggered when synchronization object is going to be deleted.
     *          This should free all resources.
     **/
    virtual void FreeResources( void );

//////////////////////////////////////////////////////////////////////////
// CEMutexIX class implementation
//////////////////////////////////////////////////////////////////////////
private:
    inline void initPosixMutex( bool isRecursive );

//////////////////////////////////////////////////////////////////////////
// CEMutexIX class implementation
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   POSIX mutex.
     **/
    mutable pthread_mutex_t     mPosixMutex;
    /**
     * \brief   POSIX mutex attribute.
     **/
    mutable pthread_mutexattr_t mPosixMutexAttr;

//////////////////////////////////////////////////////////////////////////
// CEMutexIX class implementation
//////////////////////////////////////////////////////////////////////////
private:
    CEMutexIX( const CEMutexIX & );
    const CEMutexIX & operator = ( const CEMutexIX & );
};

class CEObjectLockIX
{
public:
    inline CEObjectLockIX( const CEMutexIX & mutex, bool autoLock = true )
        : mMutex    (const_cast<CEMutexIX &>(mutex))
        , mAutolock (autoLock)
    {
        if (autoLock)
        {
            mMutex.LockObject(IESynchObject::WAIT_INFINITE);
        }
    }

    inline ~CEObjectLockIX( void )
    {
        if (mAutolock)
        {
            mMutex.UnlockObject();
        }
    }

    inline bool Lock( unsigned int msTimeout = IESynchObject::WAIT_INFINITE ) const
    {
        return mMutex.LockObject(msTimeout);
    }

    inline void Unlock( void ) const
    {
        mMutex.UnlockObject();
    }

private:
    CEMutexIX & mMutex;

    const bool  mAutolock;

private:
    CEObjectLockIX( void );
    CEObjectLockIX( const CEObjectLockIX & /*src*/ );
    const CEObjectLockIX & operator = ( const CEObjectLockIX & /*src*/ );
};

#endif  // _POSIX

#endif  // AREG_BASE_PRIVATE_POSIX_CEMUTEXIX_HPP
