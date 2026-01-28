/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/IEWaitableBaseIX.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, POSIX Mutex wrapper class.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/base/private/posix/IEWaitableBaseIX.hpp"

#if   defined(_POSIX) || defined(POSIX)

#include "areg/base/private/posix/SyncLockAndWaitIX.hpp"

//////////////////////////////////////////////////////////////////////////
// SyncWaitable class implementation
//////////////////////////////////////////////////////////////////////////

IEWaitableBaseIX::IEWaitableBaseIX( NESyncTypesIX::eSyncObject syncType, bool isRecursive, const char* asciiName /* = nullptr */ )
    : MutexIX     ( syncType, isRecursive, asciiName )
{
}

IEWaitableBaseIX::~IEWaitableBaseIX( void )
{
    ASSERT(SyncLockAndWaitIX::isWaitableRegistered(*this) == false);
}

void IEWaitableBaseIX::freeResources(void)
{
    SyncLockAndWaitIX::eventRemove(*this);
}

#endif  //  defined(_POSIX) || defined(POSIX)
