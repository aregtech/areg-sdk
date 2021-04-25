/************************************************************************
 * \file        areg/src/base/private/posix/CEMutexIX.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, POSIX Mutex wrapper class.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/

#include "areg/src/base/private/posix/CEMutexIX.hpp"

#ifdef  _POSIX

//////////////////////////////////////////////////////////////////////////
// CEMutexIX class implementation
//////////////////////////////////////////////////////////////////////////

CEMutexIX::CEMutexIX( bool initLocked /*= false*/, const char * asciiName /* = NULL */)
    : IESynchObjectBaseIX   ( NESynchTypesIX::SoMutex, asciiName)

    , mPosixMutex           ( NULL )
    , mPosixMutexAttr       ( NULL )
{
    initPosixMutex( true );
    if (initLocked)
    {
        pthread_mutex_lock( &mPosixMutex );
    }
}

CEMutexIX::CEMutexIX( NESynchTypesIX::eSynchObject synchType, bool isRecursive, const char * asciiName /* = NULL */ )
    : IESynchObjectBaseIX   ( synchType, asciiName )

    , mPosixMutex           ( NULL )
    , mPosixMutexAttr       ( NULL )
{
    initPosixMutex( isRecursive );
}

CEMutexIX::~CEMutexIX( void )
{
    if (mPosixMutex != NULL)
        pthread_mutex_destroy(&mPosixMutex);
    if (mPosixMutexAttr != NULL)
        pthread_mutexattr_destroy(&mPosixMutexAttr);

    mPosixMutex     = NULL;
    mPosixMutexAttr = NULL;
}

inline void CEMutexIX::initPosixMutex( bool isRecursive )
{
    if ( RETURNED_OK == pthread_mutexattr_init( &mPosixMutexAttr ) )
    {
        if ( RETURNED_OK == pthread_mutexattr_settype( &mPosixMutexAttr, isRecursive ? PTHREAD_MUTEX_RECURSIVE : PTHREAD_MUTEX_DEFAULT ) )
        {
            if ( RETURNED_OK != pthread_mutex_init( &mPosixMutex, &mPosixMutexAttr ) )
            {
                pthread_mutexattr_destroy( &mPosixMutexAttr );
                mPosixMutexAttr = NULL;
                mPosixMutex     = NULL;
            }
        }
        else
        {
            pthread_mutexattr_destroy( &mPosixMutexAttr );
            mPosixMutexAttr   = NULL;
        }
    }
}

bool CEMutexIX::LockObject( unsigned int msTimeout /*= IESynchObject::WAIT_INFINITE*/ ) const
{
    bool result = false;
    if ( mPosixMutex != NULL)
    {
        if ( IESynchObject::WAIT_INFINITE == msTimeout )
        {
            result = RETURNED_OK == pthread_mutex_lock( &mPosixMutex );
        }
        else
        {
            timespec now;
            NESynchTypesIX::TimeoutFromNow(now, msTimeout);
            result = RETURNED_OK == pthread_mutex_timedlock( &mPosixMutex, &now );
        }
    }

    return result;
}

bool CEMutexIX::TryLockObject( void ) const
{
    return (RETURNED_OK == pthread_mutex_trylock( &mPosixMutex ) );
}

void CEMutexIX::UnlockObject( void ) const
{
    pthread_mutex_unlock( &mPosixMutex );
}

bool CEMutexIX::IsValid( void ) const
{
    return (mPosixMutex != NULL) && (mPosixMutexAttr != NULL);
}

void CEMutexIX::FreeResources(void)
{
    pthread_mutex_unlock( &mPosixMutex );
}

#endif // _POSIX
