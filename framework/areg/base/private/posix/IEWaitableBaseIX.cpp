/************************************************************************
 * \file        areg/base/private/posix/IEWaitableBaseIX.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, POSIX Mutex wrapper class.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/base/private/posix/IEWaitableBaseIX.hpp"

#ifdef  _POSIX
#include "areg/base/private/posix/SynchLockAndWaitIX.hpp"

//////////////////////////////////////////////////////////////////////////
// SynchWaitable class implementation
//////////////////////////////////////////////////////////////////////////

IEWaitableBaseIX::IEWaitableBaseIX( NESynchTypesIX::eSynchObject synchType, bool isRecursive, const char* asciiName /* = NULL */ )
    : MutexIX     ( synchType, isRecursive, asciiName )
{
    ; // do nothing
}

IEWaitableBaseIX::~IEWaitableBaseIX( void )
{
    ASSERT(SynchLockAndWaitIX::isWaitableRegistered(*this) == false);
}

void IEWaitableBaseIX::freeResources(void)
{
    SynchLockAndWaitIX::eventRemove(*this);
}

#endif  // _POSIX
