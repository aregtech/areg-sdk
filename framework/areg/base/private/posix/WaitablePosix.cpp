/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/WaitablePosix.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, POSIX Mutex wrapper class.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/base/private/posix/WaitablePosix.hpp"

#if   defined(_POSIX) || defined(POSIX)

#include "areg/base/private/posix/SyncLockAndWaitPosix.hpp"

namespace areg::os
{
    //////////////////////////////////////////////////////////////////////////
    // SyncWaitable class implementation
    //////////////////////////////////////////////////////////////////////////

    WaitablePosix::WaitablePosix( areg::os::SyncKind syncType, bool isRecursive, const char* asciiName /* = nullptr */ )
        : areg::os::MutexPosix     ( syncType, isRecursive, asciiName )
    {
    }

    WaitablePosix::~WaitablePosix()
    {
        ASSERT(areg::os::SyncLockAndWaitPosix::isWaitableRegistered(*this) == false);
    }

    void WaitablePosix::freeResources()
    {
        areg::os::SyncLockAndWaitPosix::eventRemove(*this);
    }

} // namespace areg::os

#endif  //  defined(_POSIX) || defined(POSIX)
