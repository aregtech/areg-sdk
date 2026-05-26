/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/WaitableSemaphorePosix.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, POSIX Waitable Event class.
 *
 ************************************************************************/

#if defined(_POSIX) || defined(POSIX)

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/private/posix/WaitableSemaphorePosix.hpp"
#include "areg/base/private/posix/SyncLockAndWaitPosix.hpp"

namespace areg::os {

//////////////////////////////////////////////////////////////////////////
// WaitableSemaphorePosix class implementation.
//////////////////////////////////////////////////////////////////////////

WaitableSemaphorePosix::WaitableSemaphorePosix(int32_t maxCount, int32_t initCount /*= 0*/)
    : WaitablePosix  ( areg::os::SyncKind::SoWaitSemaphore )
    , mObjectLock    ( areg::os::SyncKind::SoMutex, true )
    , mMaxCount      ( maxCount  )
    , mCurCount      ( initCount )
{
}

bool WaitableSemaphorePosix::release_semaphore() noexcept
{
    bool sendSignal = false;

    do 
    {
        ObjectLockPosix lock(mObjectLock);
        if (mCurCount < mMaxCount)
        {
            ++ mCurCount;
            sendSignal  = true;
        }
    } while (false);

    if (sendSignal)
    {
        SyncLockAndWaitPosix::event_signaled(*this);
    }

    return sendSignal;
}

bool WaitableSemaphorePosix::check_signaled(pthread_t /*contextThread*/) const
{
    ObjectLockPosix lock(mObjectLock);
    return (mCurCount > 0);
}

bool WaitableSemaphorePosix::notify_request_ownership(pthread_t ownerThread)
{
    ObjectLockPosix lock(mObjectLock);
    bool result = false;

    if ((mCurCount > 0) && (ownerThread != static_cast<pthread_t>(0)))
    {
        -- mCurCount;
        result = true;

        AREG_OUTPUT_DBG("Waitable Semaphore [ %s ] has given ownership to thread [ %p ], there are still [ %d ] locks available"
                        , name().as_string( )
                        , reinterpret_cast<id_type>(ownerThread), mCurCount);
    }

    return result;
}

bool WaitableSemaphorePosix::can_signal_threads() const noexcept
{
    return true;
}

void WaitableSemaphorePosix::notify_released_threads(int32_t /* numThreads */)
{
}

bool WaitableSemaphorePosix::is_valid() const noexcept
{
    return mObjectLock.is_valid();
}

} // namespace areg::os
#endif  // defined(_POSIX) || defined(POSIX)
