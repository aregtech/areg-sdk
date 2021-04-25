/************************************************************************
 * \file        areg/src/base/private/posix/IEWaitableBaseIX.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, POSIX Mutex wrapper class.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/src/base/private/posix/IEWaitableBaseIX.hpp"

#ifdef  _POSIX
#include "areg/src/base/private/posix/CESynchLockAndWaitIX.hpp"

//////////////////////////////////////////////////////////////////////////
// CESynchWaitable class implementation
//////////////////////////////////////////////////////////////////////////

IEWaitableBaseIX::IEWaitableBaseIX( NESynchTypesIX::eSynchObject synchType, bool isRecursive, const char* asciiName /* = NULL */ )
    : CEMutexIX     ( synchType, isRecursive, asciiName )
{
    ; // do nothing
}

IEWaitableBaseIX::~IEWaitableBaseIX( void )
{
    ASSERT(CESynchLockAndWaitIX::IsResourceRegistered(*this) == false);
}

void IEWaitableBaseIX::FreeResources(void)
{
    CESynchLockAndWaitIX::EventRemove(*this);
}

#endif  // _POSIX
