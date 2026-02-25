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

namespace
{
    /**
     * \brief   Constant. The name of invalid component.
     *          Do not use this name as role name for component.
     **/
    constexpr std::string_view   INVALID_COMPONENT_NAME     { "INVALID_COMPONENT_NAME" };
}

//////////////////////////////////////////////////////////////////////////
// Static variables
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The address of invalid component
 **/
const ComponentAddress & ComponentAddress::invalid_component_address()
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
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
}

ComponentAddress::ComponentAddress( const ThreadAddress & threadAddress )
    : mRoleName     ( INVALID_COMPONENT_NAME )
    , mThreadAddress( threadAddress )
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
}

ComponentAddress::ComponentAddress( const ThreadAddress & threadAddress, const String & roleName )
    : mRoleName     ( roleName.is_empty() ? INVALID_COMPONENT_NAME : roleName)
    , mThreadAddress( threadAddress )
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
    mRoleName.truncate(NEUtilities::ITEM_NAMES_MAX_LENGTH);
    mMagicNum   = ComponentAddress::_magic_number(*this);
}

ComponentAddress::ComponentAddress( const String & roleName )
    : mRoleName     ( roleName.is_empty() ? INVALID_COMPONENT_NAME : roleName)
    , mThreadAddress( DispatcherThread::current_dispatcher_thread().address() )
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
    mRoleName.truncate(NEUtilities::ITEM_NAMES_MAX_LENGTH);
    Component* comp = Component::find_by_name(roleName);
    if (comp != nullptr)
    {
        mThreadAddress = comp->address( ).thread_address( );
    }

    mMagicNum   = ComponentAddress::_magic_number(*this);
}

ComponentAddress::ComponentAddress( const String & roleName, const String & nameThread )
    : mRoleName     ( roleName.is_empty() ? INVALID_COMPONENT_NAME : roleName)
    , mThreadAddress( nameThread.is_empty() != false ? DispatcherThread::dispatcher_thread(nameThread).address() : ThreadAddress::invalid_thread_address())
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
    mRoleName.truncate(NEUtilities::ITEM_NAMES_MAX_LENGTH);
    mMagicNum   = ComponentAddress::_magic_number(*this);
}

ComponentAddress::ComponentAddress( const ComponentAddress & src )
    : mRoleName     ( src.mRoleName )
    , mThreadAddress( src.mThreadAddress )
    , mMagicNum     ( src.mMagicNum )
{
}

ComponentAddress::ComponentAddress( ComponentAddress && src ) noexcept
    : mRoleName     ( std::move(src.mRoleName) )
    , mThreadAddress( std::move(src.mThreadAddress) )
    , mMagicNum     ( src.mMagicNum )
{
}

ComponentAddress::ComponentAddress( const InStream & stream )
    : mRoleName     ( stream )
    , mThreadAddress( stream )
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
    mMagicNum   = ComponentAddress::_magic_number(*this);
}

bool ComponentAddress::is_valid() const
{
    return (mMagicNum != NEMath::CHECKSUM_IGNORE) && mThreadAddress.is_valid();
}

String ComponentAddress::to_string() const
{
    String result;

    result.append(mRoleName)
          .append(NECommon::COMPONENT_PATH_SEPARATOR)
          .append(ThreadAddress::to_path(mThreadAddress));

    return result;
}

void ComponentAddress::conv_from_string(const char * pathComponent, const char** out_nextPart /*= nullptr*/)
{
    const char* strSource = pathComponent;

    mRoleName       = String::substr(strSource, NECommon::COMPONENT_PATH_SEPARATOR.data(), &strSource);
    mThreadAddress  = ThreadAddress::from_path(strSource, &strSource);
    mMagicNum       = ComponentAddress::_magic_number(*this);

    if (out_nextPart != nullptr)
        *out_nextPart = strSource;
}

uint32_t ComponentAddress::_magic_number(const ComponentAddress & addrComp)
{
    uint32_t result = NEMath::CHECKSUM_IGNORE;
    if (addrComp.mThreadAddress.is_valid() && (addrComp.mRoleName.is_empty() == false) && (addrComp.mRoleName != INVALID_COMPONENT_NAME))
    {
        result = NEMath::crc32_init();
        result = NEMath::crc32_start(result, addrComp.mThreadAddress.thread_name().as_string());
        result = NEMath::crc32_start(result, addrComp.mRoleName.as_string());
        result = NEMath::crc32_finish(result);
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
    result.conv_from_string(componentPath, out_nextPart);
    return result;
}
