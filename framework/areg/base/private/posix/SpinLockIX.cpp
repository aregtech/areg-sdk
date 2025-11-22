/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/SpinLockIX.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, OS specific spin-lock synchronization object.
 *
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/

#include "areg/base/private/posix/SpinLockIX.hpp"

#if defined(_POSIX) || defined(POSIX)

#include "areg/base/Thread.hpp"

//////////////////////////////////////////////////////////////////////////
// SpinLockIX class, Methods
//////////////////////////////////////////////////////////////////////////


SpinLockIX::SpinLockIX( void )
    : mSpinLock     ( )
    , mInternLock   ( )
    , mSpinOwner    ( 0 )
    , mLockCount    ( 0 )
    , mIsValid      ( false )
{
    mIsValid =  (RETURNED_OK == ::pthread_spin_init( &mSpinLock, PTHREAD_PROCESS_PRIVATE   ) ) && 
                (RETURNED_OK == ::pthread_spin_init( &mInternLock, PTHREAD_PROCESS_PRIVATE ) );
}

SpinLockIX::~SpinLockIX( void )
{
    freeResources( );
}

bool SpinLockIX::lock( void )
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

bool SpinLockIX::unlock( void )
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

bool SpinLockIX::tryLock( void )
{
    bool result = false;

    if ( mIsValid.load() )
    {
        _lockIntern( );

        pthread_t curThread = ::pthread_self( );
        if ( mSpinOwner != curThread )
        {
            _unlockIntern( );

            if ( RETURNED_OK == ::pthread_spin_trylock( &mSpinLock ) )
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

void SpinLockIX::freeResources( void )
{
    if ( mIsValid.load() )
    {
        mIsValid    = false;

        ::pthread_spin_destroy( &mSpinLock );
        ::pthread_spin_destroy( &mInternLock );

        mSpinLock   = 0;
        mInternLock = 0;
        mSpinOwner  = 0;
        mLockCount  = 0;
    }
}


inline bool SpinLockIX::_lockSpin( void )
{
    return (RETURNED_OK == ::pthread_spin_lock( &mSpinLock ));
}

inline void SpinLockIX::_unlockSpin( void )
{
    ::pthread_spin_unlock( &mSpinLock );
}

inline void SpinLockIX::_lockIntern( void )
{
    ::pthread_spin_lock( &mInternLock );
}

inline void SpinLockIX::_unlockIntern( void )
{
    ::pthread_spin_unlock( &mInternLock );
}

#endif // defined(_POSIX) || defined(POSIX)
