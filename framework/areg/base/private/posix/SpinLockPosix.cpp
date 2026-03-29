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
 * \brief       Areg Platform, POSIX spin-lock implementation.
 *              Uses POSIX pthread_spinlock_t for all non-Apple POSIX platforms
 *              (Linux, Cygwin, FreeBSD, etc.).
 *              macOS uses os_unfair_lock — see macos/SpinLockMacOS.cpp.
 *
 ************************************************************************/
#include "areg/base/private/posix/SpinLockPosix.hpp"

#if defined(_POSIX) || defined(POSIX)

#include "areg/base/CommonDefs.hpp"

//////////////////////////////////////////////////////////////////////////
// Non-Apple POSIX: constructor and platform-specific helpers
// using POSIX pthread_spinlock_t (Linux, Cygwin, FreeBSD, etc.)
//////////////////////////////////////////////////////////////////////////

#ifndef __APPLE__

namespace areg::os {

SpinLockPosix::SpinLockPosix()
    : mSpinLock  ( )
    , mInternLock( )
    , mSpinOwner ( 0 )
    , mLockCount ( 0 )
    , mIsValid   ( false )
{
    mIsValid =  (areg::RETURNED_OK == ::pthread_spin_init(&mSpinLock,   PTHREAD_PROCESS_PRIVATE)) &&
                (areg::RETURNED_OK == ::pthread_spin_init(&mInternLock, PTHREAD_PROCESS_PRIVATE));
}

bool SpinLockPosix::try_lock() noexcept
{
    bool result = false;

    if (mIsValid.load())
    {
        _lock_intern();

        pthread_t curThread = ::pthread_self();
        if (mSpinOwner != curThread)
        {
            _unlock_intern();

            if (areg::RETURNED_OK == ::pthread_spin_trylock(&mSpinLock))
            {
                _lock_intern();
                mSpinOwner = curThread;
                mLockCount = 1;
                result     = true;
                _unlock_intern();
            }
        }
        else
        {
            mLockCount++;
            result = true;
            _unlock_intern();
        }
    }

    return result;
}

void SpinLockPosix::free_resources() noexcept
{
    if (mIsValid.load())
    {
        mIsValid = false;
        ::pthread_spin_destroy(&mSpinLock);
        ::pthread_spin_destroy(&mInternLock);
        mSpinLock   = 0;
        mInternLock = 0;
        mSpinOwner  = 0;
        mLockCount  = 0;
    }
}

bool SpinLockPosix::_lock_spin() noexcept
{
    return (areg::RETURNED_OK == ::pthread_spin_lock(&mSpinLock));
}

void SpinLockPosix::_unlock_spin() noexcept
{
    ::pthread_spin_unlock(&mSpinLock);
}

void SpinLockPosix::_lock_intern() noexcept
{
    ::pthread_spin_lock(&mInternLock);
}

void SpinLockPosix::_unlock_intern() noexcept
{
    ::pthread_spin_unlock(&mInternLock);
}

} // namespace areg::os

#endif  // !__APPLE__

//////////////////////////////////////////////////////////////////////////
// Common POSIX: lock() and unlock() — shared by all POSIX platforms
//////////////////////////////////////////////////////////////////////////

namespace areg::os {

SpinLockPosix::~SpinLockPosix() noexcept
{
    free_resources();
}

bool SpinLockPosix::lock() noexcept
{
    bool result = false;

    if (mIsValid.load())
    {
        _lock_intern();

        pthread_t curThread = ::pthread_self();
        if (mSpinOwner != curThread)
        {
            _unlock_intern();

            if (_lock_spin())
            {
                _lock_intern();
                result     = true;
                mSpinOwner = curThread;
                mLockCount = 1;
                _unlock_intern();
            }
        }
        else
        {
            result = true;
            mLockCount++;
            _unlock_intern();
        }
    }

    return result;
}

bool SpinLockPosix::unlock() noexcept
{
    bool result = false;

    if (mIsValid.load())
    {
        _lock_intern();

        if (mSpinOwner == ::pthread_self())
        {
            ASSERT(mLockCount != 0);
            mLockCount--;

            if (mLockCount == 0)
            {
                mSpinOwner = 0;
                _unlock_spin();
            }

            result = true;
        }

        _unlock_intern();
    }

    return result;
}

} // namespace areg::os

#endif  // defined(_POSIX) || defined(POSIX)
