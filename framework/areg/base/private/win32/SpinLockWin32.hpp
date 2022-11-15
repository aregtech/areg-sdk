#ifndef AREG_BASE_PRIVATE_WIN32_SPINLOCKWIN32_HPP
#define AREG_BASE_PRIVATE_WIN32_SPINLOCKWIN32_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/base/private/win32/SpinLockWin32.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, OS specific spin-lock synchronization object.
 * 
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/GEGlobal.h"

#ifdef _WINDOWS
#if defined (__cplusplus) && (__cplusplus > 201703L)

#include <atomic>

//////////////////////////////////////////////////////////////////////////
// SpinLockWin32 class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Windows specific recursive spin-lock with atomic operations.
 **/
class SpinLockWin32
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Sets the instance of CriticalSectionIX object, which is the 
     *          real POSIX spin-lock wrapper class.
     **/
    SpinLockWin32( void );

    /**
     * \brief   Destructor.
     **/
    ~SpinLockWin32( void );

//////////////////////////////////////////////////////////////////////////
// Operations.
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Override operations, IESynchObject interface
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Waits for ownership of spin-lock object. If the spin-lock
     *          object is currently owned by another thread, call of
     *          this function cause wait indefinitely for ownership.
     *          In contrast, when a mutex object is used for mutual exclusion,
     *          the lock() function accept a specified time-out interval.
     **/
    bool lock( void );

    /**
     * \brief   Releases ownership of the spin-lock object.
     * \return  Returns true if spin-lock owning thread called unlock.
     *          Otherwise, it returns false.
     **/
    bool unlock( void );

    /**
     * \brief   Attempts to take the spin-lock ownership without blocking thread.
     *          If the call is successful, the calling thread
     *          takes ownership of the spin-lock.
     * \return  If current thread successfully has taken the ownership or the thread
     *          already has the ownership of spin-lock, the return value is true.
     *          If another thread already owns the critical section,
     *          the return value is false.
     **/
    bool tryLock( void );

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
    DECLARE_NOCOPY_NOMOVE( SpinLockWin32 );
};

//////////////////////////////////////////////////////////////////////////
// SpinLock inline methods
//////////////////////////////////////////////////////////////////////////

#endif // defined (__cplusplus) && (__cplusplus > 201703L)

#endif // _WINDOWS

#endif  // AREG_BASE_PRIVATE_WIN32_SPINLOCKWIN32_HPP
