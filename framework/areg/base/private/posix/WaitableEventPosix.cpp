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
namespace areg::os {

//////////////////////////////////////////////////////////////////////////
// WaitableEventPosix class implementation
//////////////////////////////////////////////////////////////////////////

WaitableEventPosix::WaitableEventPosix( bool isInitSignaled, bool is_auto_reset, const char * asciiName /* = nullptr */ )
    : WaitablePosix  ( areg::os::SyncKind::SoWaitEvent, true, asciiName )

    , mEventReset       ( is_auto_reset ? areg::os::ResetMode::Automatic : areg::os::ResetMode::Manual )
    , mIsSignaled       ( isInitSignaled )
{
}

bool WaitableEventPosix::set_event()
{
    bool result     = false;
    bool sendSignal = false;

    do 
    {
        ObjectLockPosix lock(*this);

        if (is_valid())
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
                // AREG_OUTPUT_DBG("The waitable event [ %s ] was already in signal state. Ignoring call to set event", name());
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

bool WaitableEventPosix::reset()
{
    bool result = false;
    ObjectLockPosix lock(*this);
    if ( is_valid() )
    {
#ifdef DEBUG
        if (mIsSignaled)
        {
            if (areg::os::ResetMode::Automatic == mEventReset)
            {
                AREG_OUTPUT_WARN("Manually reseting auto-reset waitable event [ %s ].", name().as_string());
            }
            else
            {
                AREG_OUTPUT_DBG("Manually reseting event [ %s ]", name().as_string());
            }
        }
#endif // DEBUG

        mIsSignaled = false;
        result      = true;
    }

    return result;
}


void WaitableEventPosix::pulse_event()
{
    do 
    {
        ObjectLockPosix lock(*this);
        if (is_valid())
        {
            if (mIsSignaled == false)
            {
                AREG_OUTPUT_DBG("Pulsing event [ %s ]", name().as_string( ));

                mIsSignaled = true;
                lock.unlock();

                SyncLockAndWaitPosix::event_signaled(*this);

                lock.lock();
                mIsSignaled = false;
            }
        }
    } while (false);
}

bool WaitableEventPosix::check_signaled(pthread_t /*contextThread*/) const
{
    ObjectLockPosix lock(*this);
    return mIsSignaled;
}

bool WaitableEventPosix::notify_request_ownership( pthread_t /* ownerThread */ )
{
    return true;
}

bool WaitableEventPosix::can_signal_threads() const
{
    return true;
}

void WaitableEventPosix::notify_released_threads(int32_t numThreads)
{
    ObjectLockPosix lock(*this);

    if ((mEventReset == areg::os::ResetMode::Automatic) && (numThreads > 0))
    {
        AREG_OUTPUT_DBG("There were [ %d ] released threads, automatically resetting waitable event [ %p ].", numThreads, this);
        mIsSignaled = false;
    }
}

} // namespace areg::os
#endif  // defined(_POSIX) || defined(POSIX)
