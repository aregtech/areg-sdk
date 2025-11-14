#ifndef AREG_BASE_PRIVATE_POSIX_IEWAITABLEBASEIX_HPP
#define AREG_BASE_PRIVATE_POSIX_IEWAITABLEBASEIX_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/IEWaitableBaseIX.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, waitable base object.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/base/GEGlobal.h"

#if defined(_POSIX) || defined(POSIX)

#include "areg/base/private/posix/MutexIX.hpp"
#include <pthread.h>

//////////////////////////////////////////////////////////////////////////
// SynchWaitable class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Base class for all waitable objects.
 *          The waitable objects are locked and released by signal.
 *          The waitable objects can be combined in the group, so that
 *          the waiting thread can wait for one or more objects to be
 *          signaled. This object cannot be directly instantiated.
 *          Instead, instantiate one of child classes.
 **/
class IEWaitableBaseIX : public MutexIX
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Protected instantiation constructor. Call to set the synchronization
     *          object type, recursive flag and the name.
     **/
    IEWaitableBaseIX( NESynchTypesIX::eSynchObject synchType, bool isRecursive, const char* asciiName = nullptr );

public:
    /**
     * \brief   Destructor.
     **/
    virtual ~IEWaitableBaseIX( void );

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns true if the object is signaled. Otherwise, returns false.
     * \param   contextThread   The thread context where lock and wait happened.
     **/
    virtual bool checkSignaled( pthread_t contextThread ) const = 0;

    /**
     * \brief   This callback is triggered when a waiting thread is released to continue to run.
     * \param   ownerThread     Indicates the POSIX thread ID that completed to wait.
     * \return  Returns true if waitable successfully has taken thread ownership.
     **/
    virtual bool notifyRequestOwnership( pthread_t ownerThread ) = 0;

    /**
     * \brief   This callback is triggered to when a system needs to know whether waitable
     *          can signal multiple threads. Returned 'true' value indicates that there can be
     *          multiple threads can get waitable signaled state. For example, waitable Mutex 
     *          signals only one thread, when waitable Event can signal multiple threads.
     **/
    virtual bool checkCanSignalMultipleThreads( void ) const = 0;

    /**
     * \brief   This callback is called to notify the object the amount of
     *          threads that were leased when the object is in signaled state.
     * \param   numThreads  The number of threads that where released when the
     *                      object is in signaled state. 0 means that no thread
     *                      was released by the object.
     **/
    virtual void notifyReleasedThreads( int numThreads ) = 0;

    /**
     * \brief   Call when synchronization object is going to be deleted.
     *          This releases all waiting threads with failure code.
     **/
    virtual void freeResources( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    IEWaitableBaseIX( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( IEWaitableBaseIX );
};

//////////////////////////////////////////////////////////////////////////
// IEWaitableBaseIX class inline implementation
//////////////////////////////////////////////////////////////////////////

#endif  // defined(_POSIX) || defined(POSIX)

#endif  // AREG_BASE_PRIVATE_POSIX_IEWAITABLEBASEIX_HPP
