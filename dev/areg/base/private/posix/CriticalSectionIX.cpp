/************************************************************************
 * Copyright    (C) 2018-2021, Artak Avetyan (mailto:artak@aregtech.com).
 *              All rights reserved.
 *
 *              This program is distributed to companies and individuals
 *              with opened sources. Optionally, the binaries can be
 *              distributed as well. 
 *
 ************************************************************************/
/************************************************************************
 * \file        areg/base/private/posix/CriticalSectionIX.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, POSIX Mutex wrapper class.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/base/private/posix/CriticalSectionIX.hpp"

#ifdef  _POSIX

#include <pthread.h>

//////////////////////////////////////////////////////////////////////////
// CriticalSectionIX class implementation.
//////////////////////////////////////////////////////////////////////////
CriticalSectionIX::CriticalSectionIX( bool initLock /*= false*/ )
    : IESynchObjectBaseIX( NESynchTypesIX::SoSpinLock, "CriticalSection" )

    , mSpin              ( )
    , mSpinValid         ( false )
{
    mSpinValid = (RETURNED_OK == pthread_spin_init(&mSpin, PTHREAD_PROCESS_PRIVATE));
    if (initLock && mSpinValid)
    {
        pthread_spin_lock(&mSpin);
    }
}

CriticalSectionIX::~CriticalSectionIX(void)
{
    if (mSpinValid)
    {
        pthread_spin_destroy(&mSpin);
        mSpinValid = false;
    }
}

bool CriticalSectionIX::isValid(void) const
{
    return mSpinValid;
}

void CriticalSectionIX::freeResources(void)
{
    unlock();

    if (mSpinValid)
    {
        pthread_spin_destroy(&mSpin);
        mSpinValid = false;
    }
}

//////////////////////////////////////////////////////////////////////////
// SpinLockIX class implementation.
//////////////////////////////////////////////////////////////////////////

SpinLockIX::SpinLockIX(CriticalSectionIX & lock)
    : mLocker   ( lock )
{
    mLocker.lock();
}

SpinLockIX::~SpinLockIX(void)
{
    mLocker.unlock();
}

#endif  // _POSIX
