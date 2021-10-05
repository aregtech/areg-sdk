/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/base/private/win32/SpinLockWin32.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, OS specific spin-lock synchronization object.
 *
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/

#include "areg/base/private/win32/SpinLockWin32.hpp"

#ifdef _WINDOWS
#if defined (__cplusplus) && (__cplusplus > 201703L)

#include "areg/base/Thread.hpp"
#include <thread>

//////////////////////////////////////////////////////////////////////////
// SpinLockWin32 class, Methods
//////////////////////////////////////////////////////////////////////////


SpinLockWin32::SpinLockWin32( void )
    : mSpinLock     ( )
    , mOwnerThread  ( 0 )
    , mLockCount    ( 0 )
{
    static_assert(std::atomic<id_type>::is_always_lock_free);
    static_assert(std::atomic<uint32_t>::is_always_lock_free);
}

SpinLockWin32::~SpinLockWin32( void )
{
    unlock();
}

bool SpinLockWin32::lock( void )
{
    id_type currThread = Thread::getCurrentThreadId( );

    if ( mOwnerThread != currThread )
    {
        while ( mSpinLock.test_and_set( std::memory_order_acquire ) )
        {
            while ( mSpinLock.test( std::memory_order_relaxed ) )
            {
                std::this_thread::yield();  // _YIELD_PROCESSOR(); // spin
            }
        }

        ASSERT( mLockCount == 0 );
        mLockCount  = 1;
        mOwnerThread= currThread;
    }
    else
    {
        mLockCount ++;
    }

    return true;
}

bool SpinLockWin32::unlock( void )
{
    bool result = false;

    id_type currThread = Thread::getCurrentThreadId( );
    if ( mOwnerThread == currThread )
    {
        ASSERT( mLockCount != 0 );
        if ( -- mLockCount == 0 )
        {
            mOwnerThread = 0;
            mSpinLock.clear( std::memory_order_release );   // release lock
        }

        result = true;
    }

    return result;
}

bool SpinLockWin32::tryLock( void )
{
    bool result = true;
    id_type currThread = Thread::getCurrentThreadId( );
    if ( mOwnerThread !=  currThread )
    {
        result = false;
        if ( mSpinLock.test_and_set( std::memory_order_acquire ) == false )
        {
            ASSERT( mLockCount == 0 );
            mLockCount  = 1;
            mOwnerThread= currThread;

            result = true;
        }
    }

    return result;
}

#endif // defined (__cplusplus) && (__cplusplus > 201703L)
#endif  // _WINDOWS
