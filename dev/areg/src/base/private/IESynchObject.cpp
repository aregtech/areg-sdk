/************************************************************************
 * \file        areg/src/base/private/IESynchObject.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Base Synchronization object interface.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "areg/src/base/IESynchObject.hpp"

//////////////////////////////////////////////////////////////////////////
// IESynchObject class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// IESynchObject class, constants
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Constant, define infinite waiting time
 **/
const unsigned int  IESynchObject::WAIT_INFINITE    = static_cast<unsigned int>(~0);

/**
 * \brief   Constant, define infinite waiting time
 **/
const unsigned int  IESynchObject::DO_NOT_WAIT      = static_cast<unsigned int>(0);

/**
 * \brief   Waiting time 500 milliseconds
 **/
const unsigned int  IESynchObject::WAIT_1_MS        = static_cast<unsigned int>(1);

/**
 * \brief   Waiting time 1 second
 **/
const unsigned int  IESynchObject::WAIT_1_SEC       = static_cast<unsigned int>(1000);

/**
 * \brief   Waiting time 1 second
 **/
const unsigned int  IESynchObject::WAIT_1_MIN       = static_cast<unsigned int>(60000);

//////////////////////////////////////////////////////////////////////////
// IESynchObject class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////

IESynchObject::IESynchObject(IESynchObject::eSyncObject synchObjectType)
    : mSynchObjectType  (synchObjectType)
    , mSynchObject      (NULL)
{
    ASSERT(synchObjectType != SO_UNKNOWN);  // do nothing
}

IESynchObject::~IESynchObject( void )
{
    _destroySynchObject();
}

//////////////////////////////////////////////////////////////////////////
// IESynchObject class, Operators
//////////////////////////////////////////////////////////////////////////
IESynchObject::operator void* ()
{
    return mSynchObject;
}

//////////////////////////////////////////////////////////////////////////
// IESynchObject class, Methods
//////////////////////////////////////////////////////////////////////////
void* IESynchObject::GetHandle( void ) const
{
    return mSynchObject;
}

IESynchObject::eSyncObject IESynchObject::GetSyncObjectType( void ) const
{
    return mSynchObjectType;
}

bool IESynchObject::Lock(unsigned int /*timeout*/ /*= WAIT_INFINITE */)
{
    return false;
}

bool IESynchObject::Unlock(void)
{
    return false;
}

