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

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/base/private/posix/WaitableSemaphorePosix.hpp"


#if defined(_POSIX) || defined(POSIX)

#include "areg/base/private/posix/SyncLockAndWaitPosix.hpp"

namespace areg::os
{
    //////////////////////////////////////////////////////////////////////////
    // WaitableSemaphorePosix class implementation.
    //////////////////////////////////////////////////////////////////////////

    WaitableSemaphorePosix::WaitableSemaphorePosix(int32_t maxCount, int32_t initCount /*= 0*/, const char * asciiName /*= nullptr */)
        : areg::os::WaitablePosix  ( areg::os::SyncKind::SoWaitSemaphore, true, asciiName )

        , mMaxCount         ( maxCount  )
        , mCurCount         ( initCount )
    {
    }

    bool WaitableSemaphorePosix::releaseSemaphore()
    {
        bool sendSignal = false;

        do 
        {
            ObjectLockPosix lock(*this);
            if (mCurCount < mMaxCount)
            {
                ++ mCurCount;
                sendSignal  = true;
            }
        } while (false);

        if (sendSignal)
        {
            SyncLockAndWaitPosix::eventSignaled(*this);
        }

        return sendSignal;
    }

    bool WaitableSemaphorePosix::checkSignaled(pthread_t /*contextThread*/) const
    {
        ObjectLockPosix lock(*this);
        return (mCurCount > 0);
    }

    bool WaitableSemaphorePosix::notifyRequestOwnership(pthread_t ownerThread)
    {
        ObjectLockPosix lock(*this);
        bool result = false;

        if ((mCurCount > 0) && (ownerThread != static_cast<pthread_t>(0)))
        {
            -- mCurCount;
            result = true;

            AREG_OUTPUT_DBG("Waitable Semaphore [ %s ] has given ownership to thread [ %p ], there are still [ %d ] locks available"
                            , getName().getString( )
                            , reinterpret_cast<id_type>(ownerThread), mCurCount);
        }

        return result;
    }

    bool WaitableSemaphorePosix::checkCanSignalMultipleThreads() const
    {
        return true;
    }

    void WaitableSemaphorePosix::notifyReleasedThreads(int32_t /* numThreads */)
    {
    }


} // namespace areg::os
#endif  // defined(_POSIX) || defined(POSIX)