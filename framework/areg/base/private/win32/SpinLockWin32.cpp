/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/win32/SpinLockWin32.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, OS specific spin-lock synchronization object.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/


#ifdef _WIN32
/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/private/win32/SpinLockWin32.hpp"

#if defined (__cplusplus) && (__cplusplus >= 201703L)

#include "areg/base/Thread.hpp"
#include <thread>

namespace areg::os {

//////////////////////////////////////////////////////////////////////////
// SpinLockWin32 class, Methods
//////////////////////////////////////////////////////////////////////////

SpinLockWin32::SpinLockWin32() noexcept
    : mSpinLock     { false }
    , mOwnerThread  { 0 }
    , mLockCount    { 0 }
{
}

SpinLockWin32::~SpinLockWin32() noexcept
{
    unlock();
}

bool SpinLockWin32::lock() noexcept
{
    const id_type currThread = Thread::current_thread_id();

    if ( mOwnerThread != currThread )
    {
        // Test-and-set spin: attempt to acquire; if already locked, spin with a
        // non-modifying load to avoid hammering the cache line with writes
        while ( mSpinLock.exchange(true, std::memory_order_acquire) )
        {
            while ( mSpinLock.load(std::memory_order_relaxed) )
            {
                std::this_thread::yield();
            }
        }

        ASSERT( mLockCount == 0 );
        mLockCount  = 1;
        mOwnerThread= currThread;
    }
    else
    {
        ++mLockCount;
    }

    return true;
}

bool SpinLockWin32::unlock() noexcept
{
    bool result = false;

    const id_type currThread = Thread::current_thread_id();
    if ( mOwnerThread == currThread )
    {
        ASSERT( mLockCount != 0 );
        if ( --mLockCount == 0 )
        {
            mOwnerThread = 0;
            mSpinLock.store(false, std::memory_order_release);
        }

        result = true;
    }

    return result;
}

bool SpinLockWin32::try_lock() noexcept
{
    bool result = true;
    const id_type currThread = Thread::current_thread_id();
    if ( mOwnerThread != currThread )
    {
        result = false;
        if ( !mSpinLock.exchange(true, std::memory_order_acquire) )
        {
            ASSERT( mLockCount == 0 );
            mLockCount  = 1;
            mOwnerThread= currThread;

            result = true;
        }
    }

    return result;
}

} // namespace areg::os
#endif // defined (__cplusplus) && (__cplusplus >= 201703L)
#endif  // _WIN32
