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
const ComponentAddress & ComponentAddress::getInvalidComponentAddress()
{
    static const ComponentAddress _invalidComponentAddress(areg::ThreadAddress::getInvalidThreadAddress(), areg::String(INVALID_COMPONENT_NAME));
    return _invalidComponentAddress;
}

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
ComponentAddress::ComponentAddress()
    : mRoleName     ( INVALID_COMPONENT_NAME )
    , mThreadAddress( areg::ThreadAddress::getInvalidThreadAddress() )
    , mMagicNum     ( areg::CHECKSUM_IGNORE )
{
}

ComponentAddress::ComponentAddress( const areg::ThreadAddress & threadAddress )
    : mRoleName     ( INVALID_COMPONENT_NAME )
    , mThreadAddress( threadAddress )
    , mMagicNum     ( areg::CHECKSUM_IGNORE )
{
}

ComponentAddress::ComponentAddress( const areg::ThreadAddress & threadAddress, const areg::String & roleName )
    : mRoleName     ( roleName.isEmpty() ? INVALID_COMPONENT_NAME : roleName)
    , mThreadAddress( threadAddress )
    , mMagicNum     ( areg::CHECKSUM_IGNORE )
{
    mRoleName.truncate(areg::ITEM_NAMES_MAX_LENGTH);
    mMagicNum   = ComponentAddress::_magicNumber(*this);
}

ComponentAddress::ComponentAddress( const areg::String & roleName )
    : mRoleName     ( roleName.isEmpty() ? INVALID_COMPONENT_NAME : roleName)
    , mThreadAddress( DispatcherThread::getCurrentDispatcherThread().getAddress() )
    , mMagicNum     ( areg::CHECKSUM_IGNORE )
{
    mRoleName.truncate(areg::ITEM_NAMES_MAX_LENGTH);
    Component* comp = Component::findComponentByName(roleName);
    if (comp != nullptr)
    {
        mThreadAddress = comp->getAddress( ).getThreadAddress( );
    }

    mMagicNum   = ComponentAddress::_magicNumber(*this);
}

ComponentAddress::ComponentAddress( const areg::String & roleName, const areg::String & nameThread )
    : mRoleName     ( roleName.isEmpty() ? INVALID_COMPONENT_NAME : roleName)
    , mThreadAddress( nameThread.isEmpty() != false ? DispatcherThread::getDispatcherThread(nameThread).getAddress() : areg::ThreadAddress::getInvalidThreadAddress())
    , mMagicNum     ( areg::CHECKSUM_IGNORE )
{
    mRoleName.truncate(areg::ITEM_NAMES_MAX_LENGTH);
    mMagicNum   = ComponentAddress::_magicNumber(*this);
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

ComponentAddress::ComponentAddress( const areg::InStream & stream )
    : mRoleName     ( stream )
    , mThreadAddress( stream )
    , mMagicNum     ( areg::CHECKSUM_IGNORE )
{
    mMagicNum   = ComponentAddress::_magicNumber(*this);
}

bool ComponentAddress::isValid() const
{
    return (mMagicNum != areg::CHECKSUM_IGNORE) && mThreadAddress.isValid();
}

areg::String ComponentAddress::convToString() const
{
    areg::String result;

    result.append(mRoleName)
          .append(areg::COMPONENT_PATH_SEPARATOR)
            .append(areg::ThreadAddress::convAddressToPath(mThreadAddress));

    return result;
}

void ComponentAddress::convFromString(const char * pathComponent, const char** out_nextPart /*= nullptr*/)
{
    const char* strSource = pathComponent;

    mRoleName       = areg::String::getSubstring(strSource, areg::COMPONENT_PATH_SEPARATOR.data(), &strSource);
    mThreadAddress  = areg::ThreadAddress::convPathToAddress(strSource, &strSource);
    mMagicNum       = ComponentAddress::_magicNumber(*this);

    if (out_nextPart != nullptr)
        *out_nextPart = strSource;
}

uint32_t ComponentAddress::_magicNumber(const ComponentAddress & addrComp)
{
    uint32_t result = areg::CHECKSUM_IGNORE;
    if (addrComp.mThreadAddress.isValid() && (addrComp.mRoleName.isEmpty() == false) && (addrComp.mRoleName != INVALID_COMPONENT_NAME))
    {
        result = areg::crc32Init();
        result = areg::crc32Start(result, addrComp.mThreadAddress.getThreadName().getString());
        result = areg::crc32Start(result, addrComp.mRoleName.getString());
        result = areg::crc32Finish(result);
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////
areg::String ComponentAddress::convAddressToPath( const ComponentAddress& componentAddress )
{
    return componentAddress.convToString();
}

ComponentAddress ComponentAddress::convPathToAddress( const char* componentPath, const char** out_nextPart /*= nullptr*/ )
{
    ComponentAddress result;
    result.convFromString(componentPath, out_nextPart);
    return result;
}
