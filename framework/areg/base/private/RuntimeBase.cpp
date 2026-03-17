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
 * \brief       Areg Platform, Runtime Object base class implementation.
 *
 ************************************************************************/
#include "areg/base/private/RuntimeBase.hpp"
#include "areg/base/RuntimeClassID.hpp"
#include "areg/base/String.hpp"

namespace areg {

//////////////////////////////////////////////////////////////////////////
// RuntimeBase — static class identity
//////////////////////////////////////////////////////////////////////////

const RuntimeClassID & RuntimeBase::_class_id() noexcept
{
    // constexpr: both mClassName and mMagicNum are evaluated at compile time.
    // The static object is zero-runtime-cost initialized (placed in .rodata).
    static constexpr RuntimeClassID _classId { "RuntimeBase" };
    return _classId;
}

//////////////////////////////////////////////////////////////////////////
// RuntimeBase — virtual method implementations
//////////////////////////////////////////////////////////////////////////

const RuntimeClassID & RuntimeBase::class_id() const noexcept
{
    return RuntimeBase::_class_id();
}

std::string_view RuntimeBase::class_name() const noexcept
{
    return RuntimeBase::_class_id().name();
}

const char* RuntimeBase::class_string() const noexcept
{
    return RuntimeBase::_class_id().name().data();
}

uint32_t RuntimeBase::class_number() const noexcept
{
    return RuntimeBase::_class_id().magic();
}

bool RuntimeBase::is_runtime( const char * className ) const noexcept
{
    return (RuntimeBase::_class_id().name() == className);
}

bool RuntimeBase::is_runtime( std::string_view className ) const noexcept
{
    return (RuntimeBase::_class_id().name() == className);
}

bool RuntimeBase::is_runtime( const String & className ) const noexcept
{
    return (className == RuntimeBase::_class_id().name());
}

bool RuntimeBase::is_runtime( const RuntimeClassID & classId ) const noexcept
{
    return (RuntimeBase::_class_id().magic() == classId.magic());
}

bool RuntimeBase::is_runtime( uint32_t classMagic ) const noexcept
{
    return (RuntimeBase::_class_id().magic() == classMagic);
}

} // namespace areg
