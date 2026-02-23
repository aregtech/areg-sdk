/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/CriticalSectionPosix.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, POSIX Mutex wrapper class.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/base/private/posix/CriticalSectionPosix.hpp"

#if defined(_POSIX) || defined(POSIX)

#include <pthread.h>

//////////////////////////////////////////////////////////////////////////
// CriticalSectionPosix class implementation.
//////////////////////////////////////////////////////////////////////////
CriticalSectionPosix::CriticalSectionPosix( bool initLock /*= false*/ )
    : SyncObjectPosix   ( areg::os::SyncKind::SoSpinLock, "CriticalSection" )

    , mSpinLock             ( )
{
    if (initLock)
    {
        mSpinLock.lock();
    }
}

bool CriticalSectionPosix::lock() const
{
    return mSpinLock.lock();
}

void CriticalSectionPosix::unlock() const
{
    mSpinLock.unlock();
}

bool CriticalSectionPosix::tryLock() const
{
    return mSpinLock.tryLock();
}

bool CriticalSectionPosix::isValid() const
{
    return mSpinLock.isValid();
}

void CriticalSectionPosix::freeResources()
{
    mSpinLock.freeResources();
}

#endif  //  defined(_POSIX) || defined(POSIX)
