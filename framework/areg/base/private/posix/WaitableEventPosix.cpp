/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/WaitableEventPosix.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, POSIX Waitable Event class.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/base/private/posix/WaitableEventPosix.hpp"

#if defined(_POSIX) || defined(POSIX)

#include "areg/base/private/posix/SyncLockAndWaitPosix.hpp"

//////////////////////////////////////////////////////////////////////////
// WaitableEventPosix class implementation
//////////////////////////////////////////////////////////////////////////

WaitableEventPosix::WaitableEventPosix( bool isInitSignaled, bool isAutoReset, const char * asciiName /* = nullptr */ )
    : WaitablePosix  ( areg::os::eSyncObject::SoWaitEvent, true, asciiName )

    , mEventReset       ( isAutoReset ? areg::os::eEventResetInfo::EventResetAutomatic : areg::os::eEventResetInfo::EventResetManual )
    , mIsSignaled       ( isInitSignaled )
{
}

bool WaitableEventPosix::setEvent()
{
    bool result     = false;
    bool sendSignal = false;

    do 
    {
        ObjectLockPosix lock(*this);

        if (isValid())
        {
            result = true;
            if ( mIsSignaled == false)
            {
                mIsSignaled = true;
                sendSignal  = true;
            }
#ifdef DEBUG
            else
            {
                // AREG_OUTPUT_DBG("The waitable event [ %s ] was already in signal state. Ignoring call to set event", getName());
            }
#endif // DEBUG

        }
    } while (false);

    if (sendSignal)
    {
        SyncLockAndWaitPosix::eventSignaled(*this);
    }

    return result;
}

bool WaitableEventPosix::resetEvent()
{
    bool result = false;
    ObjectLockPosix lock(*this);
    if ( isValid() )
    {
#ifdef DEBUG
        if (mIsSignaled)
        {
            if (areg::os::eEventResetInfo::EventResetAutomatic == mEventReset)
            {
                AREG_OUTPUT_WARN("Manually reseting auto-reset waitable event [ %s ].", getName().getString());
            }
            else
            {
                AREG_OUTPUT_DBG("Manually reseting event [ %s ]", getName().getString());
            }
        }
#endif // DEBUG

        mIsSignaled = false;
        result      = true;
    }

    return result;
}


void WaitableEventPosix::pulseEvent()
{
    do 
    {
        ObjectLockPosix lock(*this);
        if (isValid())
        {
            if (mIsSignaled == false)
            {
                AREG_OUTPUT_DBG("Pulsing event [ %s ]", getName().getString( ));

                mIsSignaled = true;
                lock.unlock();

                SyncLockAndWaitPosix::eventSignaled(*this);

                lock.lock();
                mIsSignaled = false;
            }
        }
    } while (false);
}

bool WaitableEventPosix::checkSignaled(pthread_t /*contextThread*/) const
{
    ObjectLockPosix lock(*this);
    return mIsSignaled;
}

bool WaitableEventPosix::notifyRequestOwnership( pthread_t /* ownerThread */ )
{
    return true;
}

bool WaitableEventPosix::checkCanSignalMultipleThreads() const
{
    return true;
}

void WaitableEventPosix::notifyReleasedThreads(int numThreads)
{
    ObjectLockPosix lock(*this);

    if ((mEventReset == areg::os::eEventResetInfo::EventResetAutomatic) && (numThreads > 0))
    {
        AREG_OUTPUT_DBG("There were [ %d ] released threads, automatically resetting waitable event [ %p ].", numThreads, this);
        mIsSignaled = false;
    }
}

#endif  // defined(_POSIX) || defined(POSIX)
