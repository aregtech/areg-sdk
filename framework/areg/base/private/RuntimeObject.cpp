/************************************************************************
 * \file        areg/base/private/RuntimeObject.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Runtime Object class
 *
 ************************************************************************/
#include "areg/base/RuntimeObject.hpp"

//////////////////////////////////////////////////////////////////////////
// RuntimeObject class implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// Implement runtime standard functions and initialize variables
/************************************************************************/
IMPLEMENT_RUNTIME(RuntimeObject, RuntimeBase)

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
RuntimeObject::RuntimeObject( void )
    : RuntimeBase ( )
    , Object      ( )
{
    ; // do nothing
}

RuntimeObject::~RuntimeObject( void )
{
    ; // do nothing
}
