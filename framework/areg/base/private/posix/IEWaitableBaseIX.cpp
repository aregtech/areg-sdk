/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/IEWaitableBaseIX.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, POSIX Mutex wrapper class.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/base/private/posix/IEWaitableBaseIX.hpp"

#if   defined(_POSIX) || defined(POSIX)

#include "areg/base/private/posix/SynchLockAndWaitIX.hpp"

//////////////////////////////////////////////////////////////////////////
// SynchWaitable class implementation
//////////////////////////////////////////////////////////////////////////

IEWaitableBaseIX::IEWaitableBaseIX( NESynchTypesIX::eSynchObject synchType, bool isRecursive, const char* asciiName /* = nullptr */ )
    : MutexIX     ( synchType, isRecursive, asciiName )
{
}

IEWaitableBaseIX::~IEWaitableBaseIX( void )
{
    ASSERT(SynchLockAndWaitIX::isWaitableRegistered(*this) == false);
}

void IEWaitableBaseIX::freeResources(void)
{
    SynchLockAndWaitIX::eventRemove(*this);
}

#endif  //  defined(_POSIX) || defined(POSIX)
