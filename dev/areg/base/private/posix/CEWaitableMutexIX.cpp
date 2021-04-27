/************************************************************************
 * \file        areg/base/private/posix/CEWaitableMutexIX.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, POSIX Waitable Event class.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/base/private/posix/CEWaitableMutexIX.hpp"

#ifdef  _POSIX
#include "areg/base/private/posix/CESynchLockAndWaitIX.hpp"

//////////////////////////////////////////////////////////////////////////
// CEWaitableMutexIX class implementation.
//////////////////////////////////////////////////////////////////////////

CEWaitableMutexIX::CEWaitableMutexIX(bool initOwned /*= false*/, const char * asciiName /*= NULL*/)
    : IEWaitableBaseIX  ( NESynchTypesIX::SoWaitMutex, true, asciiName             )

    , mOwnerThread      ( initOwned ? pthread_self() : static_cast<pthread_t>(NULL) )
    , mLockCount        ( initOwned ? 1 : 0                                         )
{
    ; // do nothing
}

CEWaitableMutexIX::~CEWaitableMutexIX(void)
{
    ; // do nothing
}


bool CEWaitableMutexIX::ReleaseMutex(void)
{
    bool result     = false;
    bool sendSignal = false;

    do 
    {
        CEObjectLockIX lock(*this);
        if (mOwnerThread == pthread_self())
        {
            ASSERT(mLockCount > 0);
            result = true;

            if ( mLockCount == 1 )
            {
                mOwnerThread= static_cast<pthread_t>(NULL);
                mLockCount  = 0;
                sendSignal  = true;

                OUTPUT_DBG("Released waitable mutex [ %s ], reached lock count 0.", GetName());
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
        CESynchLockAndWaitIX::EventSignaled(*this);
    }
    return result;
}


bool CEWaitableMutexIX::IsSignaled(void) const
{
    CEObjectLockIX lock(*this);
    return (mOwnerThread == static_cast<pthread_t>(NULL));
}

bool CEWaitableMutexIX::RequestsOwnership(pthread_t ownerThread)
{
    bool result = false;
    if (ownerThread != static_cast<pthread_t>(NULL))
    {
        CEObjectLockIX lock(*this);
        if (mOwnerThread == static_cast<pthread_t>(NULL))
        {
            ASSERT(mLockCount == 0);

            result      = true;
            mLockCount  = 1;
            mOwnerThread= ownerThread;

            OUTPUT_DBG("Waitable Mutex [ %s ] gave ownership to thread [ %p ]. It is not signaled anymore", GetName(), ownerThread);
        }
        else if ( mOwnerThread == ownerThread )
        {
            result = true;
            ++ mLockCount;
        }
    }

    return result;
}

bool CEWaitableMutexIX::CanSignalMultipleThreads(void) const
{
    return false;
}

#ifdef DEBUG
void CEWaitableMutexIX::ThreadsReleased(int numThreads)
{
    ASSERT((numThreads == 1) || (numThreads == 0));
}
#else
void CEWaitableMutexIX::ThreadsReleased(int /*numThreads*/)
{
}
#endif

#endif  // _POSIX
