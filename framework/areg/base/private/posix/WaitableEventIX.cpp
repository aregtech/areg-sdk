/************************************************************************
 * \file        areg/base/private/posix/WaitableEventIX.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, POSIX Waitable Event class.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/base/private/posix/WaitableEventIX.hpp"
#ifdef  _POSIX
#include "areg/base/private/posix/SynchLockAndWaitIX.hpp"

//////////////////////////////////////////////////////////////////////////
// WaitableEventIX class implementation
//////////////////////////////////////////////////////////////////////////

WaitableEventIX::WaitableEventIX( bool isInitSignaled, bool isAutoReset, const char * asciiName /* = NULL */ )
    : IEWaitableBaseIX  ( NESynchTypesIX::SoWaitEvent, true, asciiName )

    , mEventReset       ( isAutoReset ? NESynchTypesIX::EventResetAutomatic : NESynchTypesIX::EventResetManual )
    , mIsSignaled       ( isInitSignaled )
{
    ; // do nothing
}

WaitableEventIX::~WaitableEventIX( void )
{
    ; // do nothing
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
        SynchLockAndWaitIX::eventSignaled(*this);
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
            if (NESynchTypesIX::EventResetAutomatic == mEventReset)
            {
                OUTPUT_WARN("Manually reseting auto-reset waitable event [ %s ].", getName());
            }
            else
            {
                OUTPUT_DBG("Manually reseting event [ %s ]", getName());
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
                OUTPUT_DBG("Pulsing event [ %s ]", getName());

                mIsSignaled = true;
                lock.unlock();

                SynchLockAndWaitIX::eventSignaled(*this);

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

    if ((mEventReset == NESynchTypesIX::EventResetAutomatic) && (numThreads > 0))
    {
        OUTPUT_DBG("There were [ %d ] released threads, automatically reseting waitable event [ %p ].", numThreads, this);
        mIsSignaled = false;
    }
}

#endif  // _POSIX
