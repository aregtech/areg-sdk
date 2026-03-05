#ifndef AREG_BASE_PRIVATE_POSIX_WAITABLEPOSIX_HPP
#define AREG_BASE_PRIVATE_POSIX_WAITABLEPOSIX_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/WaitablePosix.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, waitable base object.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/base/areg_global.h"

#if defined(_POSIX) || defined(POSIX)

#include "areg/base/private/posix/MutexPosix.hpp"
#include <pthread.h>
namespace areg::os {

//////////////////////////////////////////////////////////////////////////
// SyncWaitable class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Base class for POSIX waitable synchronization objects. Supports single or group waiting.
 *          Cannot be instantiated directly; use derived classes.
 **/
class WaitablePosix : public MutexPosix
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Protected constructor. Sets synchronization type, recursive flag, and name.
     **/
    WaitablePosix( areg::os::SyncKind syncType, bool isRecursive, const char* asciiName = nullptr );

public:
    virtual ~WaitablePosix();

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns true if the object is signaled.
     *
     * \param   contextThread       The thread context where signaling is checked.
     **/
    [[nodiscard]]
    virtual bool check_signaled( pthread_t contextThread ) const = 0;

    /**
     * \brief   Callback when a waiting thread is released to continue execution.
     *
     * \param   ownerThread     The thread ID that was released from waiting.
     * \return  Returns true if ownership was successfully acquired.
     **/
    virtual bool notify_request_ownership( pthread_t ownerThread ) = 0;

    /**
     * \brief   Returns true if this object can signal multiple threads simultaneously (e.g.,
     *          Event). Returns false for single-threaded objects (e.g., Mutex).
     **/
    [[nodiscard]]
    virtual bool can_signal_threads() const noexcept = 0;

    /**
     * \brief   Callback to notify the object how many threads were released when signaled.
     *
     * \param   numThreads      The number of released threads; 0 if none.
     **/
    virtual void notify_released_threads( int32_t numThreads ) = 0;

    /**
     * \brief   Called before deletion. Releases all waiting threads with failure code.
     **/
    virtual void free_resources();

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Default constructor is deleted.
     **/
    WaitablePosix() = delete;
    AREG_NOCOPY_NOMOVE( WaitablePosix );
};

//////////////////////////////////////////////////////////////////////////
// WaitablePosix class inline implementation
//////////////////////////////////////////////////////////////////////////

} // namespace areg::os

#endif  // defined(_POSIX) || defined(POSIX)

#endif  // AREG_BASE_PRIVATE_POSIX_WAITABLEPOSIX_HPP
