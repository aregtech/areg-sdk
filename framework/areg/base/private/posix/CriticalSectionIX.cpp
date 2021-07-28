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
    mSpin.spinOwner     = 0;
    mSpin.lockCounter   = 0;
    mSpin.spinLock      = 0;
    mSpin.internLock    = 0;

    if ( (RETURNED_OK == pthread_spin_init(&mSpin.spinLock, PTHREAD_PROCESS_PRIVATE  )) &&
         (RETURNED_OK == pthread_spin_init(&mSpin.internLock, PTHREAD_PROCESS_PRIVATE)) )
    {
        mSpinValid = true;
    }

    if (initLock && mSpinValid)
    {
        mSpin.spinOwner     = pthread_self();
        mSpin.lockCounter   = 1;
        lockSpin();
    }
}

CriticalSectionIX::~CriticalSectionIX(void)
{
    if (mSpinValid)
    {
        pthread_spin_destroy(&mSpin.spinLock);
        pthread_spin_destroy(&mSpin.internLock);

        mSpin.spinOwner     = 0;
        mSpin.lockCounter   = 0;
        mSpin.spinLock      = 0;
        mSpin.internLock    = 0;
        mSpinValid = false;
    }
}

bool CriticalSectionIX::lock(void) const
{
    bool result = false;
    if ( mSpinValid )
    {
        pthread_t newOwner = pthread_self();

        lockIntern();

        if ( mSpin.spinOwner == newOwner )
        {
            mSpin.lockCounter += 1;
            result = true;
            unlockIntern();
        }
        else
        {
            unlockIntern();

            if ( lockSpin() )
            {
                lockIntern();
                mSpin.spinOwner     = newOwner;
                mSpin.lockCounter   = 1;
                result = true;
                unlockIntern();
            }
        }
    }

    return result;
}

void CriticalSectionIX::unlock(void) const
{
    if ( mSpinValid )
    {
        lockIntern();

        if ( mSpin.spinOwner == pthread_self() )
        {
            if ( mSpin.lockCounter > 1 )
            {
                mSpin.lockCounter -= 1;
            }
            else if (mSpin.lockCounter == 1 )
            {
                mSpin.lockCounter   = 0;
                mSpin.spinOwner     = 0;

                unlockSpin();
            }
        }

        unlockIntern();
    }
}

bool CriticalSectionIX::tryLock(void) const
{
    bool result = false;
    pthread_t newOwner = pthread_self();

    lockIntern();

    if ( mSpin.spinOwner == newOwner )
    {
        mSpin.lockCounter += 1;
        result = true;
        unlockIntern();
    }
    else
    {
        unlockIntern();

        if ( RETURNED_OK == pthread_spin_trylock(&mSpin.spinLock) )
        {
            lockIntern();
            mSpin.spinOwner     = newOwner;
            mSpin.lockCounter   = 1;
            result = true;
            unlockIntern();
        }
    }

    return result;
}

inline bool CriticalSectionIX::lockSpin( void ) const
{
    return (RETURNED_OK == pthread_spin_lock(&mSpin.spinLock));
}

inline void CriticalSectionIX::unlockSpin( void ) const
{
    pthread_spin_unlock(&mSpin.spinLock);
}

inline void CriticalSectionIX::lockIntern( void ) const
{
    pthread_spin_lock(&mSpin.internLock);
}

inline void CriticalSectionIX::unlockIntern( void ) const
{
    pthread_spin_unlock(&mSpin.internLock);
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
        pthread_spin_destroy(&mSpin.spinLock);
        pthread_spin_destroy(&mSpin.internLock);

        mSpin.spinOwner     = 0;
        mSpin.lockCounter   = 0;
        mSpin.spinLock      = 0;
        mSpin.internLock    = 0;
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
