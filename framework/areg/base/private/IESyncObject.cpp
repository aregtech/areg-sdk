/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/IESyncObject.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Base Synchronization object interface.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "areg/base/IESyncObject.hpp"

//////////////////////////////////////////////////////////////////////////
// IESyncObject class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// IESyncObject class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////

IESyncObject::IESyncObject(IESyncObject::eSyncObject syncObjectType)
    : mSyncObject      ( nullptr )
    , mSyncObjectType  (syncObjectType)
{
    ASSERT( mSyncObjectType != IESyncObject::eSyncObject::SoUnknown );
}

IESyncObject::~IESyncObject( void )
{
    _osDestroySyncObject();
}

//////////////////////////////////////////////////////////////////////////
// IESyncObject class, Methods
//////////////////////////////////////////////////////////////////////////
bool IESyncObject::lock(unsigned int /*timeout*/ /*= WAIT_INFINITE */)
{
    return false;
}

bool IESyncObject::unlock(void)
{
    return false;
}
