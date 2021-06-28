/************************************************************************
 * \file        areg/base/private/RuntimeBase.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Runtime Object class
 *
 ************************************************************************/
#include "areg/base/private/RuntimeBase.hpp"
#include "areg/base/RuntimeClassID.hpp"

//////////////////////////////////////////////////////////////////////////
// RuntimeBase class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Hidden static calls.
//////////////////////////////////////////////////////////////////////////
inline const RuntimeClassID & RuntimeBase::_getClassId( void )
{
    static const RuntimeClassID _classId("RuntimeBase");
    return _classId;
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
RuntimeBase::RuntimeBase( void )
{
    ; // do nothing
}

RuntimeBase::RuntimeBase( const RuntimeBase & /*src*/ )
{
    ; // do nothing
}

RuntimeBase::~RuntimeBase( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
const RuntimeBase & RuntimeBase::operator = ( const RuntimeBase & /*src*/ )
{
    return (*this);
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////
const RuntimeClassID & RuntimeBase::getRuntimeClassId( void ) const
{
    return _getClassId();
}

const char * RuntimeBase::getRuntimeClassName( void ) const
{
    return RuntimeBase::_getClassId().getName();
}

bool RuntimeBase::isInstanceOfRuntimeClass( const char * className ) const
{
    return (RuntimeBase::_getClassId() == className);
}

bool RuntimeBase::isInstanceOfRuntimeClass( const RuntimeClassID & classId ) const
{
    return (RuntimeBase::_getClassId() == classId);
}

bool RuntimeBase::isInstanceOfRuntimeClass(unsigned int classMagic) const
{
    return (RuntimeBase::_getClassId().getMagic() == classMagic);
}

unsigned int RuntimeBase::getRuntimeClassNumber(void) const
{
    return RuntimeBase::_getClassId().getMagic();
}
