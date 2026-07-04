/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/base/private/posix/CriticalSectionPosix.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, POSIX Mutex wrapper class.
 *
 ************************************************************************/

#if defined(_POSIX) || defined(POSIX)

/************************************************************************
 * Includes
 ************************************************************************/

#include "areg/base/private/posix/CriticalSectionPosix.hpp"
#include <pthread.h>

namespace areg::os {

//////////////////////////////////////////////////////////////////////////
// CriticalSectionPosix class implementation.
//////////////////////////////////////////////////////////////////////////
CriticalSectionPosix::CriticalSectionPosix( bool initLock /*= false*/ )
    : SyncObjectPosix   ( areg::os::SyncKind::SoSpinLock )

    , mSpinLock         ( )
{
    if (initLock)
    {
        mSpinLock.lock();
    }
}

bool CriticalSectionPosix::lock() const noexcept
{
    return mSpinLock.lock();
}

void CriticalSectionPosix::unlock() const noexcept
{
    mSpinLock.unlock();
}

bool CriticalSectionPosix::try_lock() const noexcept
{
    return mSpinLock.try_lock();
}

bool CriticalSectionPosix::is_valid() const noexcept
{
    return mSpinLock.is_valid();
}

void CriticalSectionPosix::free_resources()
{
    mSpinLock.free_resources();
}

} // namespace areg::os
#endif  //  defined(_POSIX) || defined(POSIX)
