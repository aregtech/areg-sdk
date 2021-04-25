/************************************************************************
 * \file        areg/src/base/private/CERuntimeBase.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Runtime Object class
 *
 ************************************************************************/
#include "areg/src/base/private/CERuntimeBase.hpp"
#include "areg/src/base/CERuntimeClassID.hpp"

//////////////////////////////////////////////////////////////////////////
// CERuntimeBase class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Hidden static calls.
//////////////////////////////////////////////////////////////////////////
const CERuntimeClassID & CERuntimeBase::_getClassId( void )
{
    static const CERuntimeClassID _classId("CERuntimeBase");
    return _classId;
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CERuntimeBase::CERuntimeBase( void )
{
    ; // do nothing
}

CERuntimeBase::CERuntimeBase( const CERuntimeBase & /*src*/ )
{
    ; // do nothing
}

CERuntimeBase::~CERuntimeBase( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
const CERuntimeBase & CERuntimeBase::operator = ( const CERuntimeBase & /*src*/ )
{
    return (*this);
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////
const CERuntimeClassID & CERuntimeBase::GetRuntimeClassId( void ) const
{
    return _getClassId();
}

const char * CERuntimeBase::GetRuntimeClassName( void ) const
{
    return _getClassId().GetName();
}

bool CERuntimeBase::IsInstanceOfRuntimeClass( const char * className ) const
{
    return (_getClassId() == className);
}

bool CERuntimeBase::IsInstanceOfRuntimeClass( const CERuntimeClassID & classId ) const
{
    return (_getClassId() == classId);
}
