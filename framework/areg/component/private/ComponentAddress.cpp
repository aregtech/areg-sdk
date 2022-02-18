/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ComponentAddress.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Component Address class implementation.
 *
 ************************************************************************/
#include "areg/component/ComponentAddress.hpp"

#include "areg/component/Component.hpp"
#include "areg/component/DispatcherThread.hpp"
#include "areg/base/IEIOStream.hpp"
#include "areg/base/NECommon.hpp"
#include "areg/base/NEMath.hpp"

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
const ComponentAddress    ComponentAddress::INVALID_COMPONENT_ADDRESS(ThreadAddress::INVALID_THREAD_ADDRESS, INVALID_COMPONENT_NAME.data());

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
ComponentAddress::ComponentAddress( void )
    : mRoleName     ( INVALID_COMPONENT_NAME.data(), static_cast<int>(INVALID_COMPONENT_NAME.length( )) )
    , mThreadAddress( ThreadAddress::INVALID_THREAD_ADDRESS )
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
}

ComponentAddress::ComponentAddress( const ThreadAddress & threadAddress )
    : mRoleName     ( INVALID_COMPONENT_NAME.data( ), static_cast<int>(INVALID_COMPONENT_NAME.length( )) )
    , mThreadAddress( threadAddress )
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
}

ComponentAddress::ComponentAddress( const ThreadAddress & threadAddress, const char * roleName )
    : mRoleName     ( NEString::isEmpty<char>(roleName) ?  INVALID_COMPONENT_NAME.data() : roleName )
    , mThreadAddress( threadAddress )
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
    mRoleName.truncate(NEUtilities::ITEM_NAMES_MAX_LENGTH);
    mMagicNum   = ComponentAddress::_magicNumber(*this);
}

ComponentAddress::ComponentAddress( const char * roleName )
    : mRoleName     ( NEString::isEmpty<char>(roleName) ? INVALID_COMPONENT_NAME.data() : roleName )
    , mThreadAddress( DispatcherThread::getCurrentDispatcherThread().getAddress() )
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
    mRoleName.truncate(NEUtilities::ITEM_NAMES_MAX_LENGTH);
    Component* comp = Component::findComponentByName(roleName);
    if (comp != nullptr)
    {
        mThreadAddress = comp->getAddress( ).getThreadAddress( );
    }

    mMagicNum   = ComponentAddress::_magicNumber(*this);
}

ComponentAddress::ComponentAddress( const char*  roleName, const char * nameThread )
    : mRoleName     ( NEString::isEmpty<char>(roleName) ? INVALID_COMPONENT_NAME.data() : roleName )
    , mThreadAddress( nameThread != nullptr ? DispatcherThread::getDispatcherThread(nameThread).getAddress() : ThreadAddress::INVALID_THREAD_ADDRESS )
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
    mRoleName.truncate(NEUtilities::ITEM_NAMES_MAX_LENGTH);
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

ComponentAddress::ComponentAddress( const IEInStream & stream )
    : mRoleName     ( stream )
    , mThreadAddress( stream )
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
    mMagicNum   = ComponentAddress::_magicNumber(*this);
}

bool ComponentAddress::isValid( void ) const
{
    return (mMagicNum != NEMath::CHECKSUM_IGNORE) && mThreadAddress.isValid();
}

String ComponentAddress::convToString(void) const
{
    String result = "";

    result += mRoleName;
    result += NECommon::COMPONENT_PATH_SEPARATOR.data();
    result += ThreadAddress::convAddressToPath(mThreadAddress).c_str();

    return result;
}

void ComponentAddress::convFromString(const char * pathComponent, const char** out_nextPart /*= nullptr*/)
{
    const char* strSource = pathComponent;

    mRoleName       = String::getSubstring(strSource, NECommon::COMPONENT_PATH_SEPARATOR.data(), &strSource);
    mThreadAddress  = ThreadAddress::convPathToAddress(strSource, &strSource);
    mMagicNum       = ComponentAddress::_magicNumber(*this);

    if (out_nextPart != nullptr)
        *out_nextPart = strSource;
}

unsigned int ComponentAddress::_magicNumber(const ComponentAddress & addrComp)
{
    unsigned int result = NEMath::CHECKSUM_IGNORE;
    if (addrComp.mThreadAddress.isValid() && (addrComp.mRoleName.isEmpty() == false) && (addrComp.mRoleName != INVALID_COMPONENT_NAME.data()))
    {
        result = NEMath::crc32Init();
        result = NEMath::crc32Start(result, addrComp.mThreadAddress.getThreadName().c_str());
        result = NEMath::crc32Start(result, addrComp.mRoleName.getString());
        result = NEMath::crc32Finish(result);
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////
String ComponentAddress::convAddressToPath( const ComponentAddress& componentAddress )
{
    return componentAddress.convToString();
}

ComponentAddress ComponentAddress::convPathToAddress( const char* componentPath, const char** out_nextPart /*= nullptr*/ )
{
    ComponentAddress result;
    result.convFromString(componentPath, out_nextPart);
    return result;
}
