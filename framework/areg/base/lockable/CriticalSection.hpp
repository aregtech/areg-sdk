#ifndef AREG_BASE_CRITICALSECTION_HPP
#define AREG_BASE_CRITICALSECTION_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/lockable/CriticalSection.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Synchronization objects
 *              Declared following synchronization objects:
 *              CriticalSection     - Critical Section synchronization object.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/lockable/lockable.h"

namespace areg {
//////////////////////////////////////////////////////////////////////////
// class CriticalSection declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Single-process synchronization object providing mutual exclusion similar to mutex but
 *          slightly more efficient. Can only be used by threads within the same process.
 **/
class AREG_API CriticalSection  final   : public Lockable
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    CriticalSection();

    virtual ~CriticalSection();

//////////////////////////////////////////////////////////////////////////
// Override operations, SyncObject interface
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Waits for and acquires ownership of the critical section. Blocks indefinitely if
     *          owned by another thread.
     *
     * \return  Always returns true for critical sections.
     **/
    inline bool lock();

    /**
     * \brief   Waits for and acquires ownership of the critical section. The timeout parameter is
     *          ignored; always blocks indefinitely.
     *
     * \return  Always returns true.
     **/
    bool lock( uint32_t /*timeout = areg::WAIT_INFINITE*/) final;

    /**
     * \brief   Releases ownership of the critical section.
     *
     * \return  Always returns true.
     **/
    bool unlock() final;

    /**
     * \brief   Attempts to acquire critical section ownership without blocking.
     *
     * \return  Returns true if acquired or already owned by current thread; false if owned by
     *          another thread.
     **/
    bool try_lock() final;

//////////////////////////////////////////////////////////////////////////
// Hidden or OS specific calls
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   OS-specific creation of critical section.
     **/
    void _os_create_critical_section();

    /**
     * \brief   OS-specific cleanup and release of critical section.
     **/
    void _os_release_critical_section();

    /**
     * \brief   OS-specific implementation to enter and lock the critical section.
     **/
    bool _os_lock();

    /**
     * \brief   OS-specific implementation to exit and unlock the critical section.
     **/
    bool _os_unlock();

    /**
     * \brief   OS-specific implementation to attempt locking without blocking.
     **/
    bool _os_try_lock();

    //////////////////////////////////////////////////////////////////////////
// Forbidden method calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( CriticalSection );
};

//////////////////////////////////////////////////////////////////////////
// CriticalSection class inline functions
//////////////////////////////////////////////////////////////////////////

inline bool CriticalSection::lock( uint32_t  /*timeout = areg::WAIT_INFINITE */ )
{
    ASSERT( mSyncObject != nullptr );
    return _os_lock( );
}

inline bool CriticalSection::unlock()
{
    ASSERT( mSyncObject != nullptr );
    return _os_unlock( );
}

inline bool CriticalSection::try_lock()
{
    ASSERT( mSyncObject != nullptr );
    return _os_try_lock( );
}

inline bool CriticalSection::lock()
{
    return lock( areg::WAIT_INFINITE );
}
} // namespace areq

#endif // AREG_BASE_CRITICALSECTION_HPP
