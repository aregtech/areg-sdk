/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/RuntimeBase.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform Runtime Object class
 *
 ************************************************************************/
#include "areg/base/private/RuntimeBase.hpp"
#include "areg/base/RuntimeClassID.hpp"
namespace areg {

//////////////////////////////////////////////////////////////////////////
// RuntimeBase class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Hidden static calls.
//////////////////////////////////////////////////////////////////////////
inline const RuntimeClassID & RuntimeBase::_class_id()
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
const RuntimeClassID & RuntimeBase::class_id() const
{
    return _class_id();
}

const String & RuntimeBase::class_name() const
{
    return RuntimeBase::_class_id().name();
}

bool RuntimeBase::is_runtime( const char * className ) const
{
    return (RuntimeBase::_class_id() == className);
}

bool RuntimeBase::is_runtime( const String & className ) const
{
    return (RuntimeBase::_class_id() == className);
}

bool RuntimeBase::is_runtime( const RuntimeClassID & classId ) const
{
    return (RuntimeBase::_class_id() == classId);
}

bool RuntimeBase::is_runtime(uint32_t classMagic) const
{
    return (RuntimeBase::_class_id().magic() == classMagic);
}

uint32_t RuntimeBase::class_number() const
{
    return RuntimeBase::_class_id().magic();
}

} // namespace areg
