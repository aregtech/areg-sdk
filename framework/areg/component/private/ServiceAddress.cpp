/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ServiceAddress.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Generic Service Address object
 *
 ************************************************************************/
#include "areg/component/ServiceAddress.hpp"

#include "areg/component/ProxyAddress.hpp"
#include "areg/component/StubAddress.hpp"
#include "areg/base/IOStream.hpp"
#include "areg/base/UtilityDefs.hpp"
#include "areg/base/MathDefs.hpp"
#include "areg/component/DispatcherThread.hpp"

#include <utility>
namespace areg {

//////////////////////////////////////////////////////////////////////////
// ServiceRole class implementation
//////////////////////////////////////////////////////////////////////////

ServiceRole::ServiceRole()
    : mRoleName ()
    , mMagicNum (areg::CHECKSUM_IGNORE)
{
}

ServiceRole::ServiceRole(const String& roleName)
    : mRoleName (roleName)
    , mMagicNum (areg::CHECKSUM_IGNORE)
{
    mRoleName.truncate(areg::ITEM_NAMES_MAX_LENGTH);
    mMagicNum = areg::crc32_calculate(roleName.as_string());
}

ServiceRole::ServiceRole(const UniqueNumber roleNum)
    : mRoleName ()
    , mMagicNum (roleNum)
{
}

ServiceRole::ServiceRole(const String& roleName, const UniqueNumber roleNum)
    : mRoleName (roleName)
    , mMagicNum (roleNum)
{
}

ServiceRole::ServiceRole(const ServiceRole& src)
    : mRoleName (src.mRoleName)
    , mMagicNum (src.mMagicNum)
{
}

ServiceRole::ServiceRole(ServiceRole&& src) noexcept
    : mRoleName (std::move(src.mRoleName))
    , mMagicNum (src.mMagicNum)
{
    src.mMagicNum = areg::CHECKSUM_IGNORE;
}

//////////////////////////////////////////////////////////////////////////
// ServiceAddress class implementation
//////////////////////////////////////////////////////////////////////////

String ServiceAddress::to_path( const ServiceAddress & addService )
{
    return addService.to_string();
}

ServiceAddress ServiceAddress::from_path( const char * pathService, const char ** nextPart /*= nullptr */ )
{
    ServiceAddress result;
    result.from_string(pathService, nextPart);
    return result;
}

ServiceAddress::ServiceAddress()
    : ServiceItem   ( )
    , mRoleName     ( String::empty_string(), 0 )
    , mMagicNum     ( areg::CHECKSUM_IGNORE )
{
}

ServiceAddress::ServiceAddress( const String & serviceName, const Version & serviceVersion, areg::ServiceType serviceType, const String & roleName )
    : ServiceItem   ( serviceName, serviceVersion, serviceType )
    , mRoleName     ( roleName )
    , mMagicNum     ( areg::CHECKSUM_IGNORE )
{
    mMagicNum = ServiceAddress::_magic_number(*this);
}

ServiceAddress::ServiceAddress(const UniqueNumber serviceNum, const Version& serviceVersion, areg::ServiceType serviceType, const UniqueNumber roleNum)
    : ServiceItem   (serviceNum, serviceVersion, serviceType)
    , mRoleName     ( )
    , mMagicNum     (roleNum)
{
}

ServiceAddress::ServiceAddress( const ServiceItem & serviceItem, const String & roleName )
    : ServiceItem   ( serviceItem )
    , mRoleName     ( roleName )
    , mMagicNum     ( areg::CHECKSUM_IGNORE )
{
    mMagicNum = roleName.is_numeric() ? roleName.to_uint32() : ServiceAddress::_magic_number(*this);
}

ServiceAddress::ServiceAddress(const ServiceItem& serviceItem, const UniqueNumber roleNum)
    : ServiceItem   ( serviceItem )
    , mRoleName     ( )
    , mMagicNum     ( roleNum )
{
}

ServiceAddress::ServiceAddress( const StubAddress & addrStub )
    : ServiceItem   ( static_cast<const ServiceItem &>(addrStub) )
    , mRoleName     ( addrStub.role_name() )
    , mMagicNum     ( static_cast<uint32_t>(static_cast<const ServiceAddress&>(addrStub)) )
{
}

ServiceAddress::ServiceAddress( const ProxyAddress & addrProxy )
    : ServiceItem   ( static_cast<const ServiceItem &>(addrProxy) )
    , mRoleName     ( addrProxy.role_name() )
    , mMagicNum     ( static_cast<uint32_t>(static_cast<const ServiceAddress&>(addrProxy)) )
{
}

ServiceAddress::ServiceAddress(const areg::RawService& rawService, const areg::Endpoint& endPoint)
    : ServiceItem   ( rawService, endPoint )
    , mRoleName     ( )
    , mMagicNum     ( rawService.role )
{
}

ServiceAddress::ServiceAddress( ServiceAddress && source ) noexcept
    : ServiceItem   ( static_cast<ServiceItem &&>(source) )
    , mRoleName     ( std::move(source.mRoleName) )
    , mMagicNum     ( source.mMagicNum )
{
    source.mMagicNum = areg::CHECKSUM_IGNORE;
}

String ServiceAddress::to_string() const
{
    return ServiceItem::to_string()
                        .append(areg::COMPONENT_PATH_SEPARATOR)
                        .append(mRoleName.is_empty() ? String::make_string(mMagicNum) : mRoleName);
}

void ServiceAddress::from_string(const char * pathService, const char** nextPart /*= nullptr */)
{
    const char* strSource   = pathService;
    ServiceItem::from_string(pathService, &strSource);
    mRoleName   = String::substr(strSource, areg::COMPONENT_PATH_SEPARATOR.data(), &strSource);
    mMagicNum   = mRoleName.is_numeric() ? static_cast<uint32_t>(mRoleName) : ServiceAddress::_magic_number(*this);

    if (nextPart != nullptr)
        *nextPart = strSource;
}

uint32_t ServiceAddress::_magic_number(const ServiceAddress addrService) noexcept
{
    uint32_t result{ areg::CHECKSUM_IGNORE };
    if (!addrService.mServiceName.is_empty() && (addrService.mServiceName != ServiceItem::INVALID_SERVICE.data()) &&
        !addrService.mRoleName.is_empty()    && (addrService.mServiceType != areg::ServiceType::Invalid))
    {
        result = areg::crc32_init();
        result = areg::crc32_start( result, addrService.mServiceName.as_string() );
        result = areg::crc32_start( result, static_cast<uint8_t>(addrService.mServiceType));
        result = areg::crc32_start( result, addrService.mRoleName.as_string() );
        result = areg::crc32_finish(result);
    }

    return result;
}

} // namespace areg
