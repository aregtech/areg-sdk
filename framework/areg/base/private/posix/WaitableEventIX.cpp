/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/WaitableEventIX.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, POSIX Waitable Event class.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/base/private/posix/WaitableEventIX.hpp"

#if defined(_POSIX) || defined(POSIX)

#include "areg/base/private/posix/SyncLockAndWaitIX.hpp"

//////////////////////////////////////////////////////////////////////////
// WaitableEventIX class implementation
//////////////////////////////////////////////////////////////////////////

WaitableEventIX::WaitableEventIX( bool isInitSignaled, bool isAutoReset, const char * asciiName /* = nullptr */ )
    : IEWaitableBaseIX  ( NESyncTypesIX::eSyncObject::SoWaitEvent, true, asciiName )

    , mEventReset       ( isAutoReset ? NESyncTypesIX::eEventResetInfo::EventResetAutomatic : NESyncTypesIX::eEventResetInfo::EventResetManual )
    , mIsSignaled       ( isInitSignaled )
{
}

bool WaitableEventIX::setEvent(void)
{
    bool result     = false;
    bool sendSignal = false;

    do 
    {
        ObjectLockIX lock(*this);

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
                // OUTPUT_DBG("The waitable event [ %s ] was already in signal state. Ignoring call to set event", getName());
            }
#endif // DEBUG

        }
    } while (false);

    if (sendSignal)
    {
        SyncLockAndWaitIX::eventSignaled(*this);
    }

    return result;
}

bool WaitableEventIX::resetEvent(void)
{
    bool result = false;
    ObjectLockIX lock(*this);
    if ( isValid() )
    {
#ifdef DEBUG
        if (mIsSignaled)
        {
            if (NESyncTypesIX::eEventResetInfo::EventResetAutomatic == mEventReset)
            {
                OUTPUT_WARN("Manually reseting auto-reset waitable event [ %s ].", getName().getString());
            }
            else
            {
                OUTPUT_DBG("Manually reseting event [ %s ]", getName().getString());
            }
        }
#endif // DEBUG

        mIsSignaled = false;
        result      = true;
    }

    return result;
}


void WaitableEventIX::pulseEvent(void)
{
    do 
    {
        ObjectLockIX lock(*this);
        if (isValid())
        {
            if (mIsSignaled == false)
            {
                OUTPUT_DBG("Pulsing event [ %s ]", getName().getString( ));

                mIsSignaled = true;
                lock.unlock();

                SyncLockAndWaitIX::eventSignaled(*this);

                lock.lock();
                mIsSignaled = false;
            }
        }
    } while (false);
}

bool WaitableEventIX::checkSignaled(pthread_t /*contextThread*/) const
{
    ObjectLockIX lock(*this);
    return mIsSignaled;
}

bool WaitableEventIX::notifyRequestOwnership( pthread_t /* ownerThread */ )
{
    return true;
}

bool WaitableEventIX::checkCanSignalMultipleThreads( void ) const
{
    return true;
}

void WaitableEventIX::notifyReleasedThreads(int numThreads)
{
    ObjectLockIX lock(*this);

    if ((mEventReset == NESyncTypesIX::eEventResetInfo::EventResetAutomatic) && (numThreads > 0))
    {
        OUTPUT_DBG("There were [ %d ] released threads, automatically resetting waitable event [ %p ].", numThreads, this);
        mIsSignaled = false;
    }
}

#endif  // defined(_POSIX) || defined(POSIX)
