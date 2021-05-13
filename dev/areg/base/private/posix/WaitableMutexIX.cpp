/************************************************************************
 * \file        areg/base/private/posix/WaitableMutexIX.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, POSIX Waitable Event class.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/base/private/posix/WaitableMutexIX.hpp"

#ifdef  _POSIX
#include "areg/base/private/posix/SynchLockAndWaitIX.hpp"

//////////////////////////////////////////////////////////////////////////
// WaitableMutexIX class implementation.
//////////////////////////////////////////////////////////////////////////

WaitableMutexIX::WaitableMutexIX(bool initOwned /*= false*/, const char * asciiName /*= NULL*/)
    : IEWaitableBaseIX  ( NESynchTypesIX::SoWaitMutex, true, asciiName             )

    , mOwnerThread      ( initOwned ? pthread_self() : static_cast<pthread_t>(NULL) )
    , mLockCount        ( initOwned ? 1 : 0                                         )
{
    ; // do nothing
}

WaitableMutexIX::~WaitableMutexIX(void)
{
    ; // do nothing
}


bool WaitableMutexIX::releaseMutex(void)
{
    bool result     = false;
    bool sendSignal = false;

    do 
    {
        ObjectLockIX lock(*this);
        if (mOwnerThread == pthread_self())
        {
            ASSERT(mLockCount > 0);
            result = true;

            if ( mLockCount == 1 )
            {
                mOwnerThread= static_cast<pthread_t>(NULL);
                mLockCount  = 0;
                sendSignal  = true;

                OUTPUT_DBG("Released waitable mutex [ %s ], reached lock count 0.", getName());
            }
            else if (mLockCount > 1)
            {
                ASSERT(mOwnerThread != static_cast<pthread_t>(NULL));
                OUTPUT_INFO("Waitable Mutex remains locked, the lock count is still [ %d ], the owner thead [ %p ]"
                            , mLockCount - 1
                            , mOwnerThread);

                -- mLockCount;
            }
#ifdef DEBUG
            else
            {
                ASSERT(mOwnerThread == static_cast<pthread_t>(NULL));
                OUTPUT_DBG("Ignoring to unlock the waitable mutex. The lock count is 0, nothing to release, owner thread [ %p ].", mOwnerThread);
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


bool WaitableMutexIX::checkSignaled(void) const
{
    ObjectLockIX lock(*this);
    return (mOwnerThread == static_cast<pthread_t>(NULL));
}

bool WaitableMutexIX::notifyRequestOwnership(pthread_t ownerThread)
{
    bool result = false;
    if (ownerThread != static_cast<pthread_t>(NULL))
    {
        ObjectLockIX lock(*this);
        if (mOwnerThread == static_cast<pthread_t>(NULL))
        {
            ASSERT(mLockCount == 0);

            result      = true;
            mLockCount  = 1;
            mOwnerThread= ownerThread;

            OUTPUT_DBG("Waitable Mutex [ %s ] gave ownership to thread [ %p ]. It is not signaled anymore", getName(), ownerThread);
        }
        else if ( mOwnerThread == ownerThread )
        {
            result = true;
            ++ mLockCount;
        }
    }

    return result;
}

bool WaitableMutexIX::checkCanSignalMultipleThreads(void) const
{
    return false;
}

#ifdef DEBUG
void WaitableMutexIX::notifyReleasedThreads(int numThreads)
{
    ASSERT((numThreads == 1) || (numThreads == 0));
}
#else
void WaitableMutexIX::notifyReleasedThreads(int /*numThreads*/)
{
}
#endif

#endif  // _POSIX
