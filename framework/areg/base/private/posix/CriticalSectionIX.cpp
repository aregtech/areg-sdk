/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/CriticalSectionIX.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, POSIX Mutex wrapper class.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/base/private/posix/CriticalSectionIX.hpp"

#if defined(_POSIX) || defined(POSIX)

#include <pthread.h>

//////////////////////////////////////////////////////////////////////////
// CriticalSectionIX class implementation.
//////////////////////////////////////////////////////////////////////////
CriticalSectionIX::CriticalSectionIX( bool initLock /*= false*/ )
    : IESynchObjectBaseIX   ( NESynchTypesIX::eSynchObject::SoSpinLock, "CriticalSection" )

    , mSpinLock             ( )
{
    if (initLock)
    {
        mSpinLock.lock();
    }
}

bool CriticalSectionIX::lock(void) const
{
    return mSpinLock.lock();
}

void CriticalSectionIX::unlock(void) const
{
    mSpinLock.unlock();
}

bool CriticalSectionIX::tryLock(void) const
{
    return mSpinLock.tryLock();
}

bool CriticalSectionIX::isValid(void) const
{
    return mSpinLock.isValid();
}

void CriticalSectionIX::freeResources(void)
{
    mSpinLock.freeResources();
}

#endif  //  defined(_POSIX) || defined(POSIX)
