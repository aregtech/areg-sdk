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
 *              macOS uses os_unfair_lock -- see macos/SpinLockMacOS.cpp.
 *
 ************************************************************************/

#if defined(_POSIX) || defined(POSIX)

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/private/posix/SpinLockPosix.hpp"
#include "areg/base/CommonDefs.hpp"

//////////////////////////////////////////////////////////////////////////
// Non-Apple POSIX -- uses pthread_spinlock_t
//////////////////////////////////////////////////////////////////////////

#ifndef __APPLE__

namespace areg::os {

SpinLockPosix::SpinLockPosix()
    : mSpinLock  ( )
    , mInternLock( )
    , mSpinOwner ( 0 )
    , mLockCount ( 0u )
    , mIsValid   ( false )
{
    mIsValid = ( areg::RETURNED_OK == ::pthread_spin_init( &mSpinLock, PTHREAD_PROCESS_PRIVATE ) );
    if ( mIsValid.load() )
    {
        ::pthread_spin_init( &mInternLock, PTHREAD_PROCESS_PRIVATE );
    }
}

bool SpinLockPosix::try_lock() noexcept
{
    if ( !mIsValid.load( std::memory_order_relaxed ) )
        return false;

    const pthread_t self = ::pthread_self();

    if ( mSpinOwner.load( std::memory_order_relaxed ) == self )
    {
        mLockCount.fetch_add( 1u, std::memory_order_relaxed );
        return true;
    }

    // Non-blocking try to acquire the underlying POSIX spinlock.
    if ( areg::RETURNED_OK != ::pthread_spin_trylock( &mSpinLock ) )
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
        ::pthread_spin_destroy( &mSpinLock );
        ::pthread_spin_destroy( &mInternLock );
        mSpinOwner.store( 0,  std::memory_order_relaxed );
        mLockCount.store( 0u, std::memory_order_relaxed );
    }
}

bool SpinLockPosix::_lock_spin() noexcept
{
    return ( areg::RETURNED_OK == ::pthread_spin_lock( &mSpinLock ) );
}

void SpinLockPosix::_unlock_spin() noexcept
{
    ::pthread_spin_unlock( &mSpinLock );
}

void SpinLockPosix::_lock_intern() noexcept
{
}

void SpinLockPosix::_unlock_intern() noexcept
{
}

} // namespace areg::os

#endif  // !__APPLE__

//////////////////////////////////////////////////////////////////////////
// Common POSIX -- lock() and unlock() -- shared by all POSIX platforms
// (non-Apple uses pthread_spinlock_t; Apple uses os_unfair_lock via
// macOS/SpinLockMacOS.cpp which overrides _lock_spin/_unlock_spin).
//////////////////////////////////////////////////////////////////////////

namespace areg::os {

SpinLockPosix::~SpinLockPosix() noexcept
{
    free_resources();
}

bool SpinLockPosix::lock() noexcept
{
    if ( !mIsValid.load( std::memory_order_relaxed ) )
        return false;

    const pthread_t self = ::pthread_self();

    // Recursive fast path -- no spinning needed.
    if ( mSpinOwner.load( std::memory_order_relaxed ) == self )
    {
        mLockCount.fetch_add( 1u, std::memory_order_relaxed );
        return true;
    }

    // Blocking acquire -- spin until the POSIX spinlock is available.
    if ( !_lock_spin() )
        return false;

    // Record ownership after acquiring.
    mSpinOwner.store( self, std::memory_order_relaxed );
    mLockCount.store( 1u,   std::memory_order_relaxed );
    return true;
}

bool SpinLockPosix::unlock() noexcept
{
    if ( !mIsValid.load( std::memory_order_relaxed ) )
        return false;

    if ( mSpinOwner.load( std::memory_order_relaxed ) != ::pthread_self() )
        return false;   // Not the owner.

    ASSERT( mLockCount.load() != 0u );
    if ( mLockCount.fetch_sub( 1u, std::memory_order_relaxed ) == 1u )
    {
        // Last recursion level -- clear ownership then release the lock.
        mSpinOwner.store( 0, std::memory_order_relaxed );
        _unlock_spin();
    }

    return true;
}

} // namespace areg::os

#endif  // defined(_POSIX) || defined(POSIX)