/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ComponentAddress.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Component Address class implementation.
 *
 ************************************************************************/
#include "areg/component/ComponentAddress.hpp"

#include "areg/component/Component.hpp"
#include "areg/component/DispatcherThread.hpp"
#include "areg/base/IOStream.hpp"
#include "areg/base/CommonDefs.hpp"
#include "areg/base/MathDefs.hpp"

#include <string_view>
#include <utility>

//////////////////////////////////////////////////////////////////////////
// ComponentAddress class implementation
//////////////////////////////////////////////////////////////////////////

namespace {
    /**
     * \brief   The name of invalid component.
     *          Do not use this name as role name for component.
     **/
    constexpr std::string_view   INVALID_COMPONENT_NAME     { "INVALID_COMPONENT_NAME" };
} // namespace

namespace areg {

//////////////////////////////////////////////////////////////////////////
// Static variables
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The address of invalid component
 **/
const ComponentAddress & ComponentAddress::invalid_component_address() noexcept
{
    static const ComponentAddress _invalidComponentAddress(ThreadAddress::invalid_thread_address(), String(INVALID_COMPONENT_NAME));
    return _invalidComponentAddress;
}

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
ComponentAddress::ComponentAddress()
    : mRoleName     ( INVALID_COMPONENT_NAME )
    , mThreadAddress( ThreadAddress::invalid_thread_address() )
    , mMagicNum     ( areg::CHECKSUM_IGNORE )
{
}

ComponentAddress::ComponentAddress( const ThreadAddress & threadAddress )
    : mRoleName     ( INVALID_COMPONENT_NAME )
    , mThreadAddress( threadAddress )
    , mMagicNum     ( areg::CHECKSUM_IGNORE )
{
}

ComponentAddress::ComponentAddress( const ThreadAddress & threadAddress, const String & roleName )
    : mRoleName     ( roleName.is_empty() ? INVALID_COMPONENT_NAME : roleName)
    , mThreadAddress( threadAddress )
    , mMagicNum     ( areg::CHECKSUM_IGNORE )
{
    mRoleName.truncate(areg::ITEM_NAMES_MAX_LENGTH);
    mMagicNum   = ComponentAddress::_magic_number(*this);
}

ComponentAddress::ComponentAddress( const String & roleName )
    : mRoleName     ( roleName.is_empty() ? INVALID_COMPONENT_NAME : roleName)
    , mThreadAddress( DispatcherThread::current_dispatcher_thread().address() )
    , mMagicNum     ( areg::CHECKSUM_IGNORE )
{
    mRoleName.truncate(areg::ITEM_NAMES_MAX_LENGTH);
    Component* comp = Component::find_by_name(roleName);
    if (comp != nullptr)
    {
        mThreadAddress = comp->address( ).thread_address( );
    }

    mMagicNum   = ComponentAddress::_magic_number(*this);
}

ComponentAddress::ComponentAddress( const String & roleName, const String & nameThread )
    : mRoleName     ( roleName.is_empty() ? INVALID_COMPONENT_NAME : roleName)
    , mThreadAddress( nameThread.is_empty() ? ThreadAddress::invalid_thread_address() : DispatcherThread::dispatcher_thread(areg::crc32_calculate(nameThread.as_string())).address())
    , mMagicNum     ( areg::CHECKSUM_IGNORE )
{
    mRoleName.truncate(areg::ITEM_NAMES_MAX_LENGTH);
    mMagicNum   = ComponentAddress::_magic_number(*this);
}

ComponentAddress::ComponentAddress( ComponentAddress && src ) noexcept
    : mRoleName     ( std::move(src.mRoleName) )
    , mThreadAddress( std::move(src.mThreadAddress) )
    , mMagicNum     ( src.mMagicNum )
{
}

String ComponentAddress::to_string() const
{
    String result;

    result.append(mRoleName)
          .append(areg::COMPONENT_PATH_SEPARATOR)
          .append(ThreadAddress::to_path(mThreadAddress));

    return result;
}

void ComponentAddress::from_string(const char * pathComponent, const char** nextPart /*= nullptr*/)
{
    const char* strSource = pathComponent;

    mRoleName       = String::substr(strSource, areg::COMPONENT_PATH_SEPARATOR.data(), &strSource);
    mThreadAddress  = ThreadAddress::from_path(strSource, &strSource);
    mMagicNum       = ComponentAddress::_magic_number(*this);

    if (nextPart != nullptr)
        *nextPart = strSource;
}

uint32_t ComponentAddress::_magic_number(const ComponentAddress & addrComp) noexcept
{
    uint32_t result{ areg::CHECKSUM_IGNORE };
    if (addrComp.mThreadAddress.is_valid() && (addrComp.mRoleName.is_empty() == false) && (addrComp.mRoleName != INVALID_COMPONENT_NAME))
    {
        result = areg::crc32_init();
        result = areg::crc32_start(result, addrComp.mThreadAddress.name().as_string());
        result = areg::crc32_start(result, addrComp.mRoleName.as_string());
        result = areg::crc32_finish(result);
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////
String ComponentAddress::to_path( const ComponentAddress& componentAddress )
{
    return componentAddress.to_string();
}

ComponentAddress ComponentAddress::from_path( const char* componentPath, const char** out_nextPart /*= nullptr*/ )
{
    ComponentAddress result;
    result.from_string(componentPath, out_nextPart);
    return result;
}

} // namespace areg
