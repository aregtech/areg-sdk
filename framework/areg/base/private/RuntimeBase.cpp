/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/base/private/RuntimeBase.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
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

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////
const RuntimeClassID & RuntimeBase::getRuntimeClassId( void ) const
{
    return _getClassId();
}

const String & RuntimeBase::getRuntimeClassName( void ) const
{
    return RuntimeBase::_getClassId().getName();
}

bool RuntimeBase::isInstanceOfRuntimeClass( const char * className ) const
{
    return (RuntimeBase::_getClassId() == className);
}

bool RuntimeBase::isInstanceOfRuntimeClass( const String & className ) const
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
