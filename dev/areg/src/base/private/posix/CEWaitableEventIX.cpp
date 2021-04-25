/************************************************************************
 * \file        areg/src/base/private/posix/CEWaitableEventIX.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, POSIX Waitable Event class.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/src/base/private/posix/CEWaitableEventIX.hpp"
#ifdef  _POSIX
#include "areg/src/base/private/posix/CESynchLockAndWaitIX.hpp"

//////////////////////////////////////////////////////////////////////////
// CEWaitableEventIX class implementation
//////////////////////////////////////////////////////////////////////////

CEWaitableEventIX::CEWaitableEventIX( bool isInitSignaled, bool isAutoReset, const char * asciiName /* = NULL */ )
    : IEWaitableBaseIX  ( NESynchTypesIX::SoWaitEvent, true, asciiName )

    , mEventReset       ( isAutoReset ? NESynchTypesIX::EventResetAutomatic : NESynchTypesIX::EventResetManual )
    , mIsSignaled       ( isInitSignaled )
{
    ; // do nothing
}

CEWaitableEventIX::~CEWaitableEventIX( void )
{
    ; // do nothing
}

bool CEWaitableEventIX::SetEvent(void)
{
    bool result     = false;
    bool sendSignal = false;

    do 
    {
        CEObjectLockIX lock(*this);

        if (IsValid())
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
                // OUTPUT_DBG("The waitable event [ %s ] was already in signal state. Ignoring call to set event", GetName());
            }
#endif // DEBUG

        }
    } while (false);

    if (sendSignal)
    {
        CESynchLockAndWaitIX::EventSignaled(*this);
    }

    return result;
}

bool CEWaitableEventIX::ResetEvent(void)
{
    bool result = false;
    CEObjectLockIX lock(*this);
    if ( IsValid() )
    {
#ifdef DEBUG
        if (mIsSignaled)
        {
            if (NESynchTypesIX::EventResetAutomatic == mEventReset)
            {
                OUTPUT_WARN("Manually reseting auto-reset waitable event [ %s ].", GetName());
            }
            else
            {
                OUTPUT_DBG("Manually reseting event [ %s ]", GetName());
            }
        }
#endif // DEBUG

        mIsSignaled = false;
        result      = true;
    }

    return result;
}


void CEWaitableEventIX::PulseEvent(void)
{
    do 
    {
        CEObjectLockIX lock(*this);
        if (IsValid())
        {
            if (mIsSignaled == false)
            {
                OUTPUT_DBG("Pulsing event [ %s ]", GetName());

                mIsSignaled = true;
                lock.Unlock();

                CESynchLockAndWaitIX::EventSignaled(*this);

                lock.Lock();
                mIsSignaled = false;
            }
        }
    } while (false);
}

bool CEWaitableEventIX::IsSignaled(void) const
{
    CEObjectLockIX lock(*this);
    return mIsSignaled;
}

bool CEWaitableEventIX::RequestsOwnership( pthread_t /* ownerThread */ )
{
    return true;
}

bool CEWaitableEventIX::CanSignalMultipleThreads( void ) const
{
    return true;
}

void CEWaitableEventIX::ThreadsReleased(int numThreads)
{
    CEObjectLockIX lock(*this);

    if ((mEventReset == NESynchTypesIX::EventResetAutomatic) && (numThreads > 0))
    {
        OUTPUT_DBG("There were [ %d ] released threads, automatically reseting waitable event [ %p ].", numThreads, this);
        mIsSignaled = false;
    }
#ifdef DEBUG
    else
    {
        // OUTPUT_INFO("The waitable event [ %s / %p ] cannot be reset. Type = [ %s ], threads released [ %d ]", GetName(), this, NESynchTypesIX::GetString(mEventReset), numThreads);
    }
#endif // DEBUG
}

#endif  // _POSIX
