/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/MutexPosix.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, POSIX Mutex wrapper class.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/

#include "areg/base/private/posix/MutexPosix.hpp"

#if defined(_POSIX) || defined(POSIX)

#ifdef __APPLE__
    #include <unistd.h>
#endif  // __APPLE__

namespace areg::os {

//////////////////////////////////////////////////////////////////////////
// MutexPosix class implementation
//////////////////////////////////////////////////////////////////////////

MutexPosix::MutexPosix( bool initLocked /*= false*/, const char * asciiName /* = nullptr */)
    : SyncObjectPosix( areg::os::SyncKind::SoMutex, asciiName)

    , mPosixMutex       ( )
    , mMutexValid       ( false )
    , mPosixMutexAttr   ( )
    , mMutexAttrValid   ( false )
{
    _init_posix_mutex( true );
    if (initLocked)
    {
        ::pthread_mutex_lock( &mPosixMutex );
    }
}

MutexPosix::MutexPosix( areg::os::SyncKind syncType, bool isRecursive, const char * asciiName /* = nullptr */ )
    : SyncObjectPosix( syncType, asciiName )

    , mPosixMutex       ( )
    , mMutexValid       ( false )
    , mPosixMutexAttr   ( )
    , mMutexAttrValid   ( false )
{
    _init_posix_mutex( isRecursive );
}

MutexPosix::~MutexPosix()
{
    if (mMutexValid)
    {
        ::pthread_mutex_destroy(&mPosixMutex);
    }

    if (mMutexAttrValid)
    {
        ::pthread_mutexattr_destroy(&mPosixMutexAttr);
    }

    mMutexValid     = false;
    mMutexAttrValid = false;
}

inline void MutexPosix::_init_posix_mutex( bool isRecursive )
{
    if ( areg::RETURNED_OK == ::pthread_mutexattr_init( &mPosixMutexAttr ) )
    {
        mMutexAttrValid = true;
        if ( areg::RETURNED_OK == ::pthread_mutexattr_settype( &mPosixMutexAttr, isRecursive ? PTHREAD_MUTEX_RECURSIVE : PTHREAD_MUTEX_DEFAULT ) )
        {
            if ( areg::RETURNED_OK == ::pthread_mutex_init( &mPosixMutex, &mPosixMutexAttr ) )
            {
                mMutexValid = true;
            }
            else
            {
                ::pthread_mutexattr_destroy( &mPosixMutexAttr );
                mMutexAttrValid = false;
                // mMutexValid stays false — mutex was never created
            }
        }
        else
        {
            ::pthread_mutexattr_destroy( &mPosixMutexAttr );
            mMutexAttrValid = false;
        }
    }
}

bool MutexPosix::lock( uint32_t msTimeout /*= areg::WAIT_INFINITE*/ ) const noexcept
{
    bool result = false;
    if ( is_valid() )
    {
        if ( areg::WAIT_INFINITE == msTimeout )
        {
            result = areg::RETURNED_OK == ::pthread_mutex_lock( &mPosixMutex );
        }
        else
        {
            timespec deadline;
            areg::os::timeout_from_now(deadline, msTimeout);
#ifdef __APPLE__
            // macOS doesn't have pthread_mutex_timedlock
            // Use exponential backoff to reduce CPU usage while maintaining responsiveness
            useconds_t sleepTime = 100;  // Start with 100 microseconds
            constexpr useconds_t maxSleep = 10000;  // Cap at 10 milliseconds

            while (!result)
            {
                if (areg::RETURNED_OK == ::pthread_mutex_trylock(&mPosixMutex))
                {
                    result = true;
                }
                else
                {
                    timespec current;
                    clock_gettime(CLOCK_REALTIME, &current);
                    if ((current.tv_sec > deadline.tv_sec) ||
                        ((current.tv_sec == deadline.tv_sec) && (current.tv_nsec >= deadline.tv_nsec)))
                    {
                        break; // timeout expired
                    }

                    usleep(sleepTime);
                    // Exponential backoff: double sleep time up to max
                    if (sleepTime < maxSleep)
                    {
                        sleepTime *= 2;
                    }
                }
            }
#else   // !__APPLE__
            result = areg::RETURNED_OK == ::pthread_mutex_timedlock( &mPosixMutex, &deadline );
#endif  // __APPLE__
        }
    }

    return result;
}

bool MutexPosix::try_lock() const noexcept
{
    return (areg::RETURNED_OK == ::pthread_mutex_trylock( &mPosixMutex ) );
}

void MutexPosix::unlock() const noexcept
{
    pthread_mutex_unlock( &mPosixMutex );
}

bool MutexPosix::is_valid() const noexcept
{
    return (mMutexValid && mMutexAttrValid);
}

void MutexPosix::free_resources()
{
    pthread_mutex_unlock( &mPosixMutex );
}

} // namespace areg::os

#endif // defined(_POSIX) || defined(POSIX)
