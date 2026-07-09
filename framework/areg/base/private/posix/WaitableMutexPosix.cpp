/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/base/private/posix/WaitableMutexPosix.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, POSIX Waitable Event class.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/

#if defined(_POSIX) || defined(POSIX)

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/private/posix/WaitableMutexPosix.hpp"
#include "areg/base/private/posix/SyncLockAndWaitPosix.hpp"
namespace areg::os {

//////////////////////////////////////////////////////////////////////////
// WaitableMutexPosix class implementation.
//////////////////////////////////////////////////////////////////////////

WaitableMutexPosix::WaitableMutexPosix(bool initOwned /*= false*/)
    : WaitablePosix  ( areg::os::SyncKind::SoWaitMutex )
    , mObjectLock    ( areg::os::SyncKind::SoMutex, true )
    , mOwnerThread   ( initOwned ? pthread_self() : static_cast<pthread_t>(0) )
    , mLockCount     ( initOwned ? 1 : 0 )
{
}

bool WaitableMutexPosix::release_mutex() noexcept
{
    bool result     = false;
    bool sendSignal = false;

    do 
    {
        ObjectLockPosix lock(mObjectLock);
        if (mOwnerThread == pthread_self())
        {
            ASSERT(mLockCount > 0);
            result = true;

            if ( mLockCount == 1 )
            {
                mOwnerThread= static_cast<pthread_t>(0);
                mLockCount  = 0;
                sendSignal  = true;

                AREG_OUTPUT_DBG("Released waitable mutex [ %s ], reached lock count 0.", name().as_string( ));
            }
            else if (mLockCount > 1)
            {
                ASSERT(mOwnerThread != static_cast<pthread_t>(0));
                AREG_OUTPUT_INFO("Waitable Mutex remains locked, the lock count is still [ %d ], the owner thead [ %p ]"
                            , mLockCount - 1
                            , reinterpret_cast<id_type>(mOwnerThread));

                -- mLockCount;
            }
#ifdef DEBUG
            else
            {
                ASSERT(mOwnerThread == static_cast<pthread_t>(0));
                AREG_OUTPUT_DBG("Ignoring to unlock the waitable mutex. The lock count is 0, nothing to release, owner thread [ %p ].", reinterpret_cast<id_type>(mOwnerThread));
            }
#endif // DEBUG
        }
    } while (false);

    if (sendSignal)
    {
        SyncLockAndWaitPosix::event_signaled(*this);
    }
    return result;
}

bool WaitableMutexPosix::notify_request_ownership(pthread_t ownerThread)
{
    bool result = false;
    if (ownerThread != static_cast<pthread_t>(0))
    {
        ObjectLockPosix lock(mObjectLock);
        if (mOwnerThread == static_cast<pthread_t>(0))
        {
            ASSERT(mLockCount == 0);

            result      = true;
            mLockCount  = 1;
            mOwnerThread= ownerThread;

            AREG_OUTPUT_DBG("Waitable Mutex [ %s ] gave ownership to thread [ %p ]. It is not signaled anymore"
                            , name().as_string( )
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

} // namespace areg::os
#endif  // defined(_POSIX) || defined(POSIX)
