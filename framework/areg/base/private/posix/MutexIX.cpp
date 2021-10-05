/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/MutexIX.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, POSIX Mutex wrapper class.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/

#include "areg/base/private/posix/MutexIX.hpp"

#if defined(_POSIX) || defined(POSIX)

//////////////////////////////////////////////////////////////////////////
// MutexIX class implementation
//////////////////////////////////////////////////////////////////////////

MutexIX::MutexIX( bool initLocked /*= false*/, const char * asciiName /* = nullptr */)
    : IESynchObjectBaseIX   ( NESynchTypesIX::eSynchObject::SoMutex, asciiName)

    , mPosixMutex           ( )
    , mMutexValid           ( false )
    , mPosixMutexAttr       ( )
    , mMutexAttrValid       ( false )
{
    _initPosixMutex( true );
    if (initLocked)
    {
        pthread_mutex_lock( &mPosixMutex );
    }
}

MutexIX::MutexIX( NESynchTypesIX::eSynchObject synchType, bool isRecursive, const char * asciiName /* = nullptr */ )
    : IESynchObjectBaseIX   ( synchType, asciiName )

    , mPosixMutex           ( )
    , mMutexValid           ( false )
    , mPosixMutexAttr       ( )
    , mMutexAttrValid       ( false )
{
    _initPosixMutex( isRecursive );
}

MutexIX::~MutexIX( void )
{
    if (mMutexValid)
    {
        pthread_mutex_destroy(&mPosixMutex);
    }

    if (mMutexAttrValid)
    {
        pthread_mutexattr_destroy(&mPosixMutexAttr);
    }

    mMutexValid     = false;
    mMutexAttrValid = false;
}

inline void MutexIX::_initPosixMutex( bool isRecursive )
{
    if ( RETURNED_OK == pthread_mutexattr_init( &mPosixMutexAttr ) )
    {
        mMutexValid = true;
        if ( RETURNED_OK == pthread_mutexattr_settype( &mPosixMutexAttr, isRecursive ? PTHREAD_MUTEX_RECURSIVE : PTHREAD_MUTEX_DEFAULT ) )
        {
            if ( RETURNED_OK == pthread_mutex_init( &mPosixMutex, &mPosixMutexAttr ) )
            {
                mMutexAttrValid = true;
            }
            else
            {
                pthread_mutexattr_destroy( &mPosixMutexAttr );
                mMutexAttrValid = false;
                mMutexValid     = false;

            }
        }
        else
        {
            pthread_mutexattr_destroy( &mPosixMutexAttr );
            mMutexAttrValid = false;
        }
    }
}

bool MutexIX::lock( unsigned int msTimeout /*= NECommon::WAIT_INFINITE*/ ) const
{
    bool result = false;
    if ( mMutexValid )
    {
        if ( NECommon::WAIT_INFINITE == msTimeout )
        {
            result = RETURNED_OK == pthread_mutex_lock( &mPosixMutex );
        }
        else
        {
            timespec now;
            NESynchTypesIX::timeoutFromNow(now, msTimeout);
            result = RETURNED_OK == pthread_mutex_timedlock( &mPosixMutex, &now );
        }
    }

    return result;
}

bool MutexIX::tryLock( void ) const
{
    return (RETURNED_OK == pthread_mutex_trylock( &mPosixMutex ) );
}

void MutexIX::unlock( void ) const
{
    pthread_mutex_unlock( &mPosixMutex );
}

bool MutexIX::isValid( void ) const
{
    return (mMutexValid && mMutexAttrValid);
}

void MutexIX::freeResources(void)
{
    pthread_mutex_unlock( &mPosixMutex );
}

#endif // defined(_POSIX) || defined(POSIX)
