/************************************************************************
 * \file        areg/src/base/private/CERuntimeObject.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Runtime Object class
 *
 ************************************************************************/
#include "areg/src/base/CERuntimeObject.hpp"

//////////////////////////////////////////////////////////////////////////
// CERuntimeObject class implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// Implement runtime standard functions and initialize variables
/************************************************************************/
IMPLEMENT_RUNTIME(CERuntimeObject, CERuntimeBase)

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CERuntimeObject::CERuntimeObject( void )
    : CERuntimeBase ( )
    , CEObject      ( )
{
    ; // do nothing
}

CERuntimeObject::~CERuntimeObject( void )
{
    ; // do nothing
}
