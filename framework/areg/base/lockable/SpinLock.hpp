#ifndef AREG_BASE_SPINLOCK_HPP
#define AREG_BASE_SPINLOCK_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/lockable/SpinLock.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Synchronization objects
 *              Declared following synchronization objects:
 *              SpinLock            - Spin-Lock synchronization object.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/lockable/lockable.h"

#include <atomic>

namespace areg {
//////////////////////////////////////////////////////////////////////////
// class SpinLock declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Recursive spin-lock for fast synchronization.
 *
 *          Uses an atomic owner-thread ID and a recursion counter to support
 *          same-thread re-entry without deadlocking. No OS primitives are
 *          required -- implemented entirely with C++17 standard atomics and
 *          portable CPU-pause hints.
 *
 *          Use only for short critical sections. Spinning wastes CPU cycles
 *          while waiting; prefer CriticalSection or Mutex for longer holds.
 *
 *          Compatible with all platforms: Windows (x86/x86-64), Linux, macOS,
 *          Cygwin, MinGW, ARM (32-bit and 64-bit). Works with MSVC, GCC, Clang.
 **/
class AREG_API SpinLock final   : public Lockable
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    SpinLock();

    virtual ~SpinLock() = default;

//////////////////////////////////////////////////////////////////////////
// Override operations, SyncObject interface
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Acquires spin-lock ownership. If called again from the same
     *          thread, increments the recursion counter and returns immediately.
     *          Spins until the lock is available when called from another thread.
     *
     * \return  Always returns true.
     **/
    inline bool lock();

    /**
     * \brief   Acquires spin-lock ownership (timeout parameter is ignored).
     *          Identical to lock().
     *
     * \return  Always returns true.
     **/
    bool lock( uint32_t /*timeout = areg::WAIT_INFINITE*/ ) final;

    /**
     * \brief   Releases one recursion level of the spin-lock.
     *          The underlying lock is released only when the recursion counter
     *          reaches zero.
     *
     * \return  Returns true if the calling thread owns the lock; false otherwise.
     **/
    bool unlock() final;

    /**
     * \brief   Attempts to acquire the spin-lock without spinning.
     *          If called from the owning thread, increments the recursion counter
     *          and returns true immediately.
     *          If the lock is held by another thread, returns false immediately.
     *
     * \return  Returns true if acquired or already owned; false if busy.
     **/
    bool try_lock() final;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:


#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

    std::atomic<id_type>    mOwner;     //!< Thread ID of the current owner; 0 = unlocked.
    std::atomic<uint32_t>   mCount;     //!< Recursion depth; 0 when unlocked.

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif  // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( SpinLock );
};

//////////////////////////////////////////////////////////////////////////
// SpinLock class inline functions
//////////////////////////////////////////////////////////////////////////

inline bool SpinLock::lock()
{
    return lock(areg::WAIT_INFINITE);
}

} // namespace areq

#endif // AREG_BASE_SPINLOCK_HPP

