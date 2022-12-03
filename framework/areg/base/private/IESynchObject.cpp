/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/base/private/IESynchObject.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Base Synchronization object interface.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "areg/base/IESynchObject.hpp"

//////////////////////////////////////////////////////////////////////////
// IESynchObject class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// IESynchObject class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////

IESynchObject::IESynchObject(IESynchObject::eSyncObject synchObjectType)
    : mSynchObject      ( nullptr )
    , mSynchObjectType  (synchObjectType)
{
    ASSERT( mSynchObjectType != IESynchObject::eSyncObject::SoUnknown );
}

IESynchObject::~IESynchObject( void )
{
    _osDestroySynchObject();
}

//////////////////////////////////////////////////////////////////////////
// IESynchObject class, Methods
//////////////////////////////////////////////////////////////////////////
bool IESynchObject::lock(unsigned int /*timeout*/ /*= WAIT_INFINITE */)
{
    return false;
}

bool IESynchObject::unlock(void)
{
    return false;
}
