#ifndef AREG_BASE_PRIVATE_POSIX_CRITICALSECTIONPOSIX_HPP
#define AREG_BASE_PRIVATE_POSIX_CRITICALSECTIONPOSIX_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/CriticalSectionPosix.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, POSIX Spin Lock wrapper class.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/base/GEGlobal.h"

#if  defined(_POSIX) || defined(POSIX)

#include "areg/base/private/posix/SyncObjectPosix.hpp"
#include "areg/base/private/posix/SpinLockPosix.hpp"
#include <pthread.h>

//////////////////////////////////////////////////////////////////////////
// CriticalSectionPosix class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   POSIX critical section wrapper for POSIX spin-lock. Provides synchronization for
 *          inter-thread communication.
 **/
class CriticalSectionPosix   : protected SyncObjectPosix
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor.
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Creates and initializes the critical section with optional initial lock state.
     *
     * \param   initLock    If true, the critical section is initially locked.
     **/
    explicit CriticalSectionPosix( bool initLock = false );

    /**
     * \brief   Destroy the object, free resources.
     **/
    virtual ~CriticalSectionPosix() = default;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Acquires the critical section, preventing other threads from accessing the protected
     *          shared data.
     *
     * \return  Returns true if the operation succeeded.
     **/
    bool lock() const;

    /**
     * \brief   Releases the critical section, allowing waiting threads to acquire it.
     **/
    void unlock() const;

    /**
     * \brief   Attempts to acquire the critical section without blocking.
     *
     * \return  Returns true if the calling thread acquired ownership; false if another thread owns
     *          it.
     **/
    bool try_lock() const;

protected:
/************************************************************************/
// SyncObjectPosix overrides.
/************************************************************************/
    
    /**
     * \brief   Returns true if the synchronization object is valid.
     **/
    bool is_valid() const override;

    /**
     * \brief   Releases all critical section resources before deletion.
     **/
    void free_resources() override;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The Critical Section object, which has implementation of recursive spin lock.
     **/
    mutable SpinLockPosix  m_spin_lock;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( CriticalSectionPosix );
};

//////////////////////////////////////////////////////////////////////////
// CriticalSectionPosix inline methods.
//////////////////////////////////////////////////////////////////////////

#endif  //  defined(_POSIX) || defined(POSIX)

#endif  // AREG_BASE_PRIVATE_POSIX_CRITICALSECTIONPOSIX_HPP
