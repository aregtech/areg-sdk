/************************************************************************
 * \file        areg/base/private/IEGenericObject.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Generic Object Interface
 *              
 *
 ************************************************************************/

#include "areg/base/IEGenericObject.hpp"

////////////////////////////////////////////////////////////////////////
// IEGenericObject interface, constructor / destructor
////////////////////////////////////////////////////////////////////////

IEGenericObject::IEGenericObject( void )
    : mObjData  (static_cast<void *>(NULL))
{
    ; // do nothing
}

IEGenericObject::IEGenericObject(void* objData)
    : mObjData  (objData)
{
    ; // do nothing
}

IEGenericObject::~IEGenericObject( void )
{
    mObjData  = static_cast<void *>(NULL);
}
