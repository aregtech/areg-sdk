/************************************************************************
 * \file        areg/base/private/posix/WaitableSemaphoreIX.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, POSIX Waitable Event class.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/base/private/posix/WaitableSemaphoreIX.hpp"

#ifdef  _POSIX
#include "areg/base/private/posix/SynchLockAndWaitIX.hpp"

//////////////////////////////////////////////////////////////////////////
// WaitableSemaphoreIX class implementation.
//////////////////////////////////////////////////////////////////////////

WaitableSemaphoreIX::WaitableSemaphoreIX(int maxCount, int initCount /*= 0*/, const char * asciiName /*= NULL */)
    : IEWaitableBaseIX  ( NESynchTypesIX::SoWaitSemaphore, true, asciiName )

    , mMaxCount         ( maxCount  )
    , mCurCount         ( initCount )
{
    ; // do nothing
}

WaitableSemaphoreIX::~WaitableSemaphoreIX(void)
{
    ; // do nothing
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

    if ((mCurCount > 0) && (ownerThread != static_cast<pthread_t>(NULL)))
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

#endif  // _POSIX
