/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/WaitableMutexIX.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, POSIX Waitable Event class.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/base/private/posix/WaitableMutexIX.hpp"

#if defined(_POSIX) || defined(POSIX)

#include "areg/base/private/posix/SynchLockAndWaitIX.hpp"

//////////////////////////////////////////////////////////////////////////
// WaitableMutexIX class implementation.
//////////////////////////////////////////////////////////////////////////

WaitableMutexIX::WaitableMutexIX(bool initOwned /*= false*/, const char * asciiName /*= nullptr*/)
    : IEWaitableBaseIX  ( NESynchTypesIX::eSynchObject::SoWaitMutex, true, asciiName             )

    , mOwnerThread      ( initOwned ? pthread_self() : static_cast<pthread_t>(0) )
    , mLockCount        ( initOwned ? 1 : 0                                      )
{
}

bool WaitableMutexIX::releaseMutex(void)
{
    bool result     = false;
    bool sendSignal = false;

    do 
    {
        ObjectLockIX lock(*this);
        if (mOwnerThread == pthread_self())
        {
            ASSERT(mLockCount > 0);
            result = true;

            if ( mLockCount == 1 )
            {
                mOwnerThread= static_cast<pthread_t>(0);
                mLockCount  = 0;
                sendSignal  = true;

                OUTPUT_DBG("Released waitable mutex [ %s ], reached lock count 0.", getName().getString( ));
            }
            else if (mLockCount > 1)
            {
                ASSERT(mOwnerThread != static_cast<pthread_t>(0));
                OUTPUT_INFO("Waitable Mutex remains locked, the lock count is still [ %d ], the owner thead [ %p ]"
                            , mLockCount - 1
                            , reinterpret_cast<id_type>(mOwnerThread));

                -- mLockCount;
            }
#ifdef DEBUG
            else
            {
                ASSERT(mOwnerThread == static_cast<pthread_t>(0));
                OUTPUT_DBG("Ignoring to unlock the waitable mutex. The lock count is 0, nothing to release, owner thread [ %p ].", reinterpret_cast<id_type>(mOwnerThread));
            }
#endif // DEBUG
        }
    } while (false);

    if (sendSignal)
    {
        SynchLockAndWaitIX::eventSignaled(*this);
    }
    return result;
}


bool WaitableMutexIX::checkSignaled(pthread_t contextThread) const
{
    ObjectLockIX lock(*this);
    return (mOwnerThread == static_cast<pthread_t>(0)) || (mOwnerThread == contextThread);
}

bool WaitableMutexIX::notifyRequestOwnership(pthread_t ownerThread)
{
    bool result = false;
    if (ownerThread != static_cast<pthread_t>(0))
    {
        ObjectLockIX lock(*this);
        if (mOwnerThread == static_cast<pthread_t>(0))
        {
            ASSERT(mLockCount == 0);

            result      = true;
            mLockCount  = 1;
            mOwnerThread= ownerThread;

            OUTPUT_DBG("Waitable Mutex [ %s ] gave ownership to thread [ %p ]. It is not signaled anymore"
                            , getName().getString( )
                            , reinterpret_cast<id_type>(ownerThread));
        }
        else if ( mOwnerThread == ownerThread )
        {
            result = true;
            ++ mLockCount;
        }
    }

    return result;
}

bool WaitableMutexIX::checkCanSignalMultipleThreads(void) const
{
    return false;
}

#ifdef  DEBUG

void WaitableMutexIX::notifyReleasedThreads(int numThreads)
{
    ASSERT((numThreads == 1) || (numThreads == 0));
}

#else   // DEBUG

void WaitableMutexIX::notifyReleasedThreads(int /*numThreads*/)
{
}

#endif  // DEBUG

#endif  // defined(_POSIX) || defined(POSIX)
