/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/macos/SpinLockMacOS.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, macOS spin-lock using Apple os_unfair_lock.
 *
 ************************************************************************/
#ifdef __APPLE__

#include "areg/base/private/posix/SpinLockPosix.hpp"

namespace areg::os {

SpinLockPosix::SpinLockPosix()
    : mSpinLock  ( )
    , mInternLock( )
    , mSpinOwner ( 0 )
    , mLockCount ( 0 )
    , mIsValid   ( false )
{
    mSpinLock   = OS_UNFAIR_LOCK_INIT;
    mInternLock = OS_UNFAIR_LOCK_INIT;
    mIsValid    = true;
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

            if (::os_unfair_lock_trylock(&mSpinLock))
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
        mIsValid    = false;
        mSpinLock   = OS_UNFAIR_LOCK_INIT;
        mInternLock = OS_UNFAIR_LOCK_INIT;
        mSpinOwner  = 0;
        mLockCount  = 0;
    }
}

bool SpinLockPosix::_lock_spin() noexcept
{
    ::os_unfair_lock_lock(&mSpinLock);
    return true;
}

void SpinLockPosix::_unlock_spin() noexcept
{
    ::os_unfair_lock_unlock(&mSpinLock);
}

void SpinLockPosix::_lock_intern() noexcept
{
    ::os_unfair_lock_lock(&mInternLock);
}

void SpinLockPosix::_unlock_intern() noexcept
{
    ::os_unfair_lock_unlock(&mInternLock);
}

} // namespace areg::os

#endif  // __APPLE__
