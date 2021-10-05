#pragma once
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/CriticalSectionIX.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, POSIX Spin Lock wrapper class.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/base/GEGlobal.h"

#if  defined(_POSIX) || defined(POSIX)

#include "areg/base/private/posix/IESynchObjectBaseIX.hpp"
#include "areg/base/private/posix/SpinLockIX.hpp"
#include <pthread.h>

//////////////////////////////////////////////////////////////////////////
// CriticalSectionIX class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   POSIX Critical Section is a wrapper of POSIX spin lock, since
 *          it is specified as the  fastest locking / synchronization object.
 *          The Critical Section can be used only for the communication between threads.
 *          The Critical Section can be owned only by one thread at a time.
 **/
class CriticalSectionIX   : protected IESynchObjectBaseIX
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor.
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Creates Critical Section object and initializes POSIX spin lock.
     * \param   initLock    If true, the critical section (spin lock) is initially locked.
     **/
    explicit CriticalSectionIX( bool initLock = false );

    /**
     * \brief   Destroy the object, free resources.
     **/
    virtual ~CriticalSectionIX( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Locks the critical section and prevents other threads to
     *          access shared data until the locking thread does not release
     *          the critical section.
     * \return  Returns true if operation succeeded.
     **/
    bool lock( void ) const;

    /**
     * \brief   Unlocks critical section so that, one of waiting threads is released
     *          to own critical section.
     **/
    void unlock( void ) const;

    /**
     * \brief   Tests whether the critical section can be locked or not.
     *          It is locked if returned value is 'true'.
     * \return  Returns true if calling thread have got the ownership.
     *          Returns false if calling thread could not get the ownership.
     **/
    bool tryLock( void ) const;

protected:
/************************************************************************/
// IESynchObjectBaseIX overrides.
/************************************************************************/
    
    /**
     * \brief   Returns true if synchronization object is valid.
     **/
    virtual bool isValid( void ) const override;

    /**
     * \brief   Triggered when synchronization object is going to be deleted.
     *          This should free all resources.
     **/
    virtual void freeResources( void ) override;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The Critical Section object, which has implementation of recursive spin lock.
     **/
    mutable SpinLockIX  mSpinLock;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( CriticalSectionIX );
};

//////////////////////////////////////////////////////////////////////////
// CriticalSectionIX inline methods.
//////////////////////////////////////////////////////////////////////////

#endif  //  defined(_POSIX) || defined(POSIX)
