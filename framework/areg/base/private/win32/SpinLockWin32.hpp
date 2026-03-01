#ifndef AREG_BASE_PRIVATE_WIN32_SPINLOCKWIN32_HPP
#define AREG_BASE_PRIVATE_WIN32_SPINLOCKWIN32_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/win32/SpinLockWin32.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, OS specific spin-lock synchronization object.
 * 
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/areg_global.h"

#ifdef _WIN32
#if defined (__cplusplus) && (__cplusplus > 201703L)

#include <atomic>
namespace areg::os {

//////////////////////////////////////////////////////////////////////////
// SpinLockWin32 class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Windows-specific recursive spin-lock with atomic operations.
 **/
class SpinLockWin32
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates and initializes the Windows spin-lock.
     **/
    SpinLockWin32();

    /**
     * \brief   Destructor.
     **/
    ~SpinLockWin32();

//////////////////////////////////////////////////////////////////////////
// Operations.
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Override operations, SyncObject interface
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Acquires spin-lock ownership, waiting indefinitely if the lock is held by another
     *          thread.
     **/
    bool lock();

    /**
     * \brief   Releases ownership of the spin-lock.
     *
     * \return  Returns true if the spin-lock owning thread called unlock; false otherwise.
     **/
    bool unlock();

    /**
     * \brief   Attempts to acquire spin-lock ownership without blocking.
     *
     * \return  Returns true if the current thread acquired or already owns the spin-lock; false if
     *          another thread owns it.
     **/
    bool try_lock();

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:

    std::atomic_flag        mSpinLock;      //!< Atomic flag to acquire
    std::atomic<id_type>    mOwnerThread;   //!< Atomic owner thread ID
    std::atomic<uint32_t>   mLockCount;     //!< The number of recursive locks

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( SpinLockWin32 );
};

//////////////////////////////////////////////////////////////////////////
// SpinLock inline methods
//////////////////////////////////////////////////////////////////////////

} // namespace areg::os

#endif // defined (__cplusplus) && (__cplusplus > 201703L)

#endif // _WIN32

#endif  // AREG_BASE_PRIVATE_WIN32_SPINLOCKWIN32_HPP
