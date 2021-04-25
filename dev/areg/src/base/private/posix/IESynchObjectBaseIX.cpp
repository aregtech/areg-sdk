/************************************************************************
 * \file        areg/src/base/private/IESynchObjectBaseIX.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, POSIX based Synchronization object base.
 *
 ************************************************************************/

#include "areg/src/base/private/posix/IESynchObjectBaseIX.hpp"

#ifdef _POSIX
#include "areg/src/base/private/posix/CESynchLockAndWaitIX.hpp"
#include "areg/src/base/NEString.hpp"
#include <string.h>

//////////////////////////////////////////////////////////////////////////
// IESynchObjectBaseIX class implementation
//////////////////////////////////////////////////////////////////////////

// const int    IESynchObjectBaseIX::_MAX_NAME_LENGTH      = 32;

IESynchObjectBaseIX::IESynchObjectBaseIX( NESynchTypesIX::eSynchObject synchType, const char* asciiName /* = NULL */ )
    : mSynchType    ( synchType )
{
    NEString::copyString<char, char>(mSynchName, IESynchObjectBaseIX::_MAX_NAME_LENGTH, asciiName != NULL ? asciiName : NEString::EmptyStringA);
}

IESynchObjectBaseIX::~IESynchObjectBaseIX( void )
{
}

#endif // _POSIX
