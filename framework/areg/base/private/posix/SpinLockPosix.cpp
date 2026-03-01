/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/SpinLockPosix.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, OS specific spin-lock synchronization object.
 *
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/

#include "areg/base/private/posix/SpinLockPosix.hpp"

#if defined(_POSIX) || defined(POSIX)

#include "areg/base/Thread.hpp"
namespace areg::os {

//////////////////////////////////////////////////////////////////////////
// SpinLockPosix class, Methods
//////////////////////////////////////////////////////////////////////////


SpinLockPosix::SpinLockPosix()
    : mSpinLock     ( )
    , mInternLock   ( )
    , mSpinOwner    ( 0 )
    , mLockCount    ( 0 )
    , mIsValid      ( false )
{
#ifdef __APPLE__
    mSpinLock   = OS_UNFAIR_LOCK_INIT;
    mInternLock = OS_UNFAIR_LOCK_INIT;
    mIsValid    = true;
#else   // __APPLE__
    mIsValid =  (areg::RETURNED_OK == ::pthread_spin_init( &mSpinLock, PTHREAD_PROCESS_PRIVATE   ) ) &&
                (areg::RETURNED_OK == ::pthread_spin_init( &mInternLock, PTHREAD_PROCESS_PRIVATE ) );
#endif  // __APPLE__
}

SpinLockPosix::~SpinLockPosix()
{
    free_resources( );
}

bool SpinLockPosix::lock()
{
    bool result = false;

    if ( mIsValid.load() )
    {
        _lock_intern( );

        pthread_t curThread = ::pthread_self( );
        if ( mSpinOwner != curThread )
        {
            _unlock_intern( );

            if ( _lock_spin( ) )
            {
                _lock_intern( );

                result = true;
                mSpinOwner  = curThread;
                mLockCount  = 1;

                _unlock_intern( );
            }
        }
        else
        {
            result = true;
            mLockCount ++;
            _unlock_intern( );
        }
    }

    return result;
}

bool SpinLockPosix::unlock()
{
    bool result = false;

    if ( mIsValid.load() )
    {
        _lock_intern( );

        if ( mSpinOwner == ::pthread_self( ) )
        {
            ASSERT( mLockCount  != 0 );
            mLockCount --;

            if ( mLockCount == 0 )
            {
                mSpinOwner = 0;
                _unlock_spin( );
            }

            result = true;
        }

        _unlock_intern( );
    }

    return result;
}

bool SpinLockPosix::try_lock()
{
    bool result = false;

    if ( mIsValid.load() )
    {
        _lock_intern( );

        pthread_t curThread = ::pthread_self( );
        if ( mSpinOwner != curThread )
        {
            _unlock_intern( );

#ifdef __APPLE__
            if ( ::os_unfair_lock_trylock( &mSpinLock ) )
#else   // !__APPLE__
            if ( areg::RETURNED_OK == ::pthread_spin_trylock( &mSpinLock ) )
#endif  // __APPLE__
            {
                _lock_intern( );

                mSpinOwner  = curThread;
                mLockCount  = 1;
                result = true;

                _unlock_intern( );
            }
        }
        else
        {
            mLockCount ++;
            result = true;
            _unlock_intern( );
        }
    }

    return result;
}

void SpinLockPosix::free_resources()
{
    if ( mIsValid.load() )
    {
        mIsValid    = false;

#ifdef __APPLE__
        mSpinLock   = OS_UNFAIR_LOCK_INIT;
        mInternLock = OS_UNFAIR_LOCK_INIT;
#else   // !__APPLE__
        ::pthread_spin_destroy( &mSpinLock );
        ::pthread_spin_destroy( &mInternLock );
        mSpinLock   = 0;
        mInternLock = 0;
#endif  // __APPLE__
        mSpinOwner  = 0;
        mLockCount  = 0;
    }
}


inline bool SpinLockPosix::_lock_spin()
{
#ifdef __APPLE__
    ::os_unfair_lock_lock( &mSpinLock );
    return true;
#else   // !__APPLE__
    return (areg::RETURNED_OK == ::pthread_spin_lock( &mSpinLock ));
#endif  // __APPLE__
}

inline void SpinLockPosix::_unlock_spin()
{
#ifdef __APPLE__
    ::os_unfair_lock_unlock( &mSpinLock );
#else   // !__APPLE__
    ::pthread_spin_unlock( &mSpinLock );
#endif  // __APPLE__
}

inline void SpinLockPosix::_lock_intern()
{
#ifdef __APPLE__
    ::os_unfair_lock_lock( &mInternLock );
#else   // !__APPLE__
    ::pthread_spin_lock( &mInternLock );
#endif  // __APPLE__
}

inline void SpinLockPosix::_unlock_intern()
{
#ifdef __APPLE__
    ::os_unfair_lock_unlock( &mInternLock );
#else   // !__APPLE__
    ::pthread_spin_unlock( &mInternLock );
#endif  // __APPLE__
}

} // namespace areg::os
#endif // defined(_POSIX) || defined(POSIX)
