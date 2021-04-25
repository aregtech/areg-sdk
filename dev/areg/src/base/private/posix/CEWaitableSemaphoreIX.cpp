/************************************************************************
 * \file        areg/src/base/private/posix/CEWaitableSemaphoreIX.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, POSIX Waitable Event class.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/src/base/private/posix/CEWaitableSemaphoreIX.hpp"

#ifdef  _POSIX
#include "areg/src/base/private/posix/CESynchLockAndWaitIX.hpp"

//////////////////////////////////////////////////////////////////////////
// CEWaitableSemaphoreIX class implementation.
//////////////////////////////////////////////////////////////////////////

CEWaitableSemaphoreIX::CEWaitableSemaphoreIX(int maxCount, int initCount /*= 0*/, const char * asciiName /*= NULL */)
    : IEWaitableBaseIX  ( NESynchTypesIX::SoWaitSemaphore, true, asciiName )

    , mMaxCount         ( maxCount  )
    , mCurCount         ( initCount )
{
    ; // do nothing
}

CEWaitableSemaphoreIX::~CEWaitableSemaphoreIX(void)
{
    ; // do nothing
}

bool CEWaitableSemaphoreIX::ReleaseSemaphore(void)
{
    bool sendSignal = false;

    do 
    {
        CEObjectLockIX lock(*this);
        if (mCurCount < mMaxCount)
        {
            ++ mCurCount;
            sendSignal  = true;
        }
    } while (false);

    if (sendSignal)
    {
        CESynchLockAndWaitIX::EventSignaled(*this);
    }

    return sendSignal;
}

bool CEWaitableSemaphoreIX::IsSignaled(void) const
{
    CEObjectLockIX lock(*this);
    return (mCurCount > 0);
}

bool CEWaitableSemaphoreIX::RequestsOwnership(pthread_t ownerThread)
{
    CEObjectLockIX lock(*this);
    bool result = false;

    if ((mCurCount > 0) && (ownerThread != static_cast<pthread_t>(NULL)))
    {
        -- mCurCount;
        result = true;

        OUTPUT_DBG("Waitable Semaphore [ %s ] has given ownership to thread [ %p ], there are still [ %d ] locks available", GetName(), ownerThread, mCurCount);
    }

    return result;
}

bool CEWaitableSemaphoreIX::CanSignalMultipleThreads(void) const
{
    return true;
}

void CEWaitableSemaphoreIX::ThreadsReleased(int /* numThreads */)
{
}

#endif  // _POSIX
