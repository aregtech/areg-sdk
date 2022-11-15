/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/WaitableSemaphoreIX.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, POSIX Waitable Event class.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/base/private/posix/WaitableSemaphoreIX.hpp"

#if defined(_POSIX) || defined(POSIX)

#include "areg/base/private/posix/SynchLockAndWaitIX.hpp"

//////////////////////////////////////////////////////////////////////////
// WaitableSemaphoreIX class implementation.
//////////////////////////////////////////////////////////////////////////

WaitableSemaphoreIX::WaitableSemaphoreIX(int maxCount, int initCount /*= 0*/, const char * asciiName /*= nullptr */)
    : IEWaitableBaseIX  ( NESynchTypesIX::eSynchObject::SoWaitSemaphore, true, asciiName )

    , mMaxCount         ( maxCount  )
    , mCurCount         ( initCount )
{
}

bool WaitableSemaphoreIX::releaseSemaphore(void)
{
    bool sendSignal = false;

    do 
    {
        ObjectLockIX lock(*this);
        if (mCurCount < mMaxCount)
        {
            ++ mCurCount;
            sendSignal  = true;
        }
    } while (false);

    if (sendSignal)
    {
        SynchLockAndWaitIX::eventSignaled(*this);
    }

    return sendSignal;
}

bool WaitableSemaphoreIX::checkSignaled(pthread_t /*contextThread*/) const
{
    ObjectLockIX lock(*this);
    return (mCurCount > 0);
}

bool WaitableSemaphoreIX::notifyRequestOwnership(pthread_t ownerThread)
{
    ObjectLockIX lock(*this);
    bool result = false;

    if ((mCurCount > 0) && (ownerThread != static_cast<pthread_t>(0)))
    {
        -- mCurCount;
        result = true;

        OUTPUT_DBG("Waitable Semaphore [ %s ] has given ownership to thread [ %p ], there are still [ %d ] locks available", getName(), reinterpret_cast<id_type>(ownerThread), mCurCount);
    }

    return result;
}

bool WaitableSemaphoreIX::checkCanSignalMultipleThreads(void) const
{
    return true;
}

void WaitableSemaphoreIX::notifyReleasedThreads(int /* numThreads */)
{
}

#endif  // defined(_POSIX) || defined(POSIX)
