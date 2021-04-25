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
 * \file        areg/src/base/private/posix/CECriticalSectionIX.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, POSIX Mutex wrapper class.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/src/base/private/posix/CECriticalSectionIX.hpp"

#ifdef  _POSIX


//////////////////////////////////////////////////////////////////////////
// CECriticalSectionIX class implementation.
//////////////////////////////////////////////////////////////////////////
CECriticalSectionIX::CECriticalSectionIX( bool initLock /*= false*/ )
    : IESynchObjectBaseIX( NESynchTypesIX::SoSpinLock, "CriticalSection" )

    , mSpin ( static_cast<pthread_spinlock_t>(NULL) )
{
    pthread_spin_init(&mSpin, PTHREAD_PROCESS_PRIVATE);
    if (initLock)
    {
        pthread_spin_lock(&mSpin);
    }
}

CECriticalSectionIX::~CECriticalSectionIX(void)
{
    pthread_spin_destroy(&mSpin);
    mSpin = static_cast<pthread_spinlock_t>(NULL);
}

bool CECriticalSectionIX::IsValid(void) const
{
    return (mSpin != static_cast<pthread_spinlock_t>(NULL));
}

void CECriticalSectionIX::FreeResources(void)
{
    pthread_spin_destroy(&mSpin);
    mSpin = static_cast<pthread_spinlock_t>(NULL);
}

//////////////////////////////////////////////////////////////////////////
// CESpinLockIX class implementation.
//////////////////////////////////////////////////////////////////////////

CESpinLockIX::CESpinLockIX(CECriticalSectionIX & lock)
    : mLocker   ( lock )
{
    mLocker.Lock();
}

CESpinLockIX::~CESpinLockIX(void)
{
    mLocker.Unlock();
}

#endif  // _POSIX
