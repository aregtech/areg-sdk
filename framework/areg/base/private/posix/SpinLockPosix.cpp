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
    freeResources( );
}

bool SpinLockPosix::lock()
{
    bool result = false;

    if ( mIsValid.load() )
    {
        _lockIntern( );

        pthread_t curThread = ::pthread_self( );
        if ( mSpinOwner != curThread )
        {
            _unlockIntern( );

            if ( _lockSpin( ) )
            {
                _lockIntern( );

                result = true;
                mSpinOwner  = curThread;
                mLockCount  = 1;

                _unlockIntern( );
            }
        }
        else
        {
            result = true;
            mLockCount ++;
            _unlockIntern( );
        }
    }

    return result;
}

bool SpinLockPosix::unlock()
{
    bool result = false;

    if ( mIsValid.load() )
    {
        _lockIntern( );

        if ( mSpinOwner == ::pthread_self( ) )
        {
            ASSERT( mLockCount  != 0 );
            mLockCount --;

            if ( mLockCount == 0 )
            {
                mSpinOwner = 0;
                _unlockSpin( );
            }

            result = true;
        }

        _unlockIntern( );
    }

    return result;
}

bool SpinLockPosix::tryLock()
{
    bool result = false;

    if ( mIsValid.load() )
    {
        _lockIntern( );

        pthread_t curThread = ::pthread_self( );
        if ( mSpinOwner != curThread )
        {
            _unlockIntern( );

#ifdef __APPLE__
            if ( ::os_unfair_lock_trylock( &mSpinLock ) )
#else   // !__APPLE__
            if ( areg::RETURNED_OK == ::pthread_spin_trylock( &mSpinLock ) )
#endif  // __APPLE__
            {
                _lockIntern( );

                mSpinOwner  = curThread;
                mLockCount  = 1;
                result = true;

                _unlockIntern( );
            }
        }
        else
        {
            mLockCount ++;
            result = true;
            _unlockIntern( );
        }
    }

    return result;
}

void SpinLockPosix::freeResources()
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


inline bool SpinLockPosix::_lockSpin()
{
#ifdef __APPLE__
    ::os_unfair_lock_lock( &mSpinLock );
    return true;
#else   // !__APPLE__
    return (areg::RETURNED_OK == ::pthread_spin_lock( &mSpinLock ));
#endif  // __APPLE__
}

inline void SpinLockPosix::_unlockSpin()
{
#ifdef __APPLE__
    ::os_unfair_lock_unlock( &mSpinLock );
#else   // !__APPLE__
    ::pthread_spin_unlock( &mSpinLock );
#endif  // __APPLE__
}

inline void SpinLockPosix::_lockIntern()
{
#ifdef __APPLE__
    ::os_unfair_lock_lock( &mInternLock );
#else   // !__APPLE__
    ::pthread_spin_lock( &mInternLock );
#endif  // __APPLE__
}

inline void SpinLockPosix::_unlockIntern()
{
#ifdef __APPLE__
    ::os_unfair_lock_unlock( &mInternLock );
#else   // !__APPLE__
    ::pthread_spin_unlock( &mInternLock );
#endif  // __APPLE__
}

#endif // defined(_POSIX) || defined(POSIX)
