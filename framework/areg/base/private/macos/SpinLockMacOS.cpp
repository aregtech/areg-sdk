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
    , mLockCount ( 0u )
    , mIsValid   ( false )
{
    mSpinLock   = OS_UNFAIR_LOCK_INIT;
    mInternLock = OS_UNFAIR_LOCK_INIT;  // kept for ABI layout; not used on hot path.
    mIsValid    = true;
}

bool SpinLockPosix::try_lock() noexcept
{
    if ( !mIsValid.load( std::memory_order_relaxed ) )
        return false;

    const pthread_t self = ::pthread_self();

    // Recursive fast path.
    if ( mSpinOwner.load( std::memory_order_relaxed ) == self )
    {
        mLockCount.fetch_add( 1u, std::memory_order_relaxed );
        return true;
    }

    if ( !::os_unfair_lock_trylock( &mSpinLock ) )
        return false;

    mSpinOwner.store( self, std::memory_order_relaxed );
    mLockCount.store( 1u,   std::memory_order_relaxed );
    return true;
}

void SpinLockPosix::free_resources() noexcept
{
    if ( mIsValid.load() )
    {
        mIsValid.store( false, std::memory_order_release );
        mSpinLock   = OS_UNFAIR_LOCK_INIT;
        mInternLock = OS_UNFAIR_LOCK_INIT;
        mSpinOwner.store( 0,  std::memory_order_relaxed );
        mLockCount.store( 0u, std::memory_order_relaxed );
    }
}

bool SpinLockPosix::_lock_spin() noexcept
{
    ::os_unfair_lock_lock( &mSpinLock );
    return true;
}

void SpinLockPosix::_unlock_spin() noexcept
{
    ::os_unfair_lock_unlock( &mSpinLock );
}

void SpinLockPosix::_lock_intern() noexcept
{
}

void SpinLockPosix::_unlock_intern() noexcept
{
}

} // namespace areg::os

#endif  // __APPLE__