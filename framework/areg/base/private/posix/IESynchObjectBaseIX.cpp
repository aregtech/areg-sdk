/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/private/IESynchObjectBaseIX.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, POSIX based Synchronization object base.
 *
 ************************************************************************/

#include "areg/base/private/posix/IESynchObjectBaseIX.hpp"

#if  defined(_POSIX) || defined(POSIX)

#include "areg/base/private/posix/SynchLockAndWaitIX.hpp"
#include "areg/base/NEString.hpp"
#include <string.h>

//////////////////////////////////////////////////////////////////////////
// IESynchObjectBaseIX class implementation
//////////////////////////////////////////////////////////////////////////

// const int    IESynchObjectBaseIX::_MAX_NAME_LENGTH      = 32;

IESynchObjectBaseIX::IESynchObjectBaseIX( NESynchTypesIX::eSynchObject synchType, const char* asciiName /* = nullptr */ )
    : mSynchType    ( synchType )
    , mSynchName    ( asciiName != nullptr ? asciiName : NEString::EmptyStringA )
{
}

#endif //  defined(_POSIX) || defined(POSIX)
