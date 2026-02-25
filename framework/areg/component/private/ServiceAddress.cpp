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

String ServiceAddress::to_path( const ServiceAddress & addService )
{
    return addService.to_string();
}

ServiceAddress ServiceAddress::from_path( const char * pathService, const char ** out_nextPart /*= nullptr */ )
{
    ServiceAddress result;
    result.conv_from_string(pathService, out_nextPart);
    return result;
}

ServiceAddress::ServiceAddress()
    : ServiceItem   ( )
    , mRoleName     ( String::empty_string(), 0 )
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
}

ServiceAddress::ServiceAddress( const String & serviceName
                              , const Version & serviceVersion
                              , NEService::ServiceType serviceType
                              , const String & roleName )
    : ServiceItem   ( serviceName, serviceVersion, serviceType )
    , mRoleName     ( roleName )
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
    mRoleName.truncate( NEUtilities::ITEM_NAMES_MAX_LENGTH );
    mMagicNum = ServiceAddress::_magic_number(*this);
}

ServiceAddress::ServiceAddress( const ServiceItem & serviceItem, const String & roleName )
    : ServiceItem   ( serviceItem )
    , mRoleName     ( roleName )
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
    mRoleName.truncate( NEUtilities::ITEM_NAMES_MAX_LENGTH );
    mMagicNum = ServiceAddress::_magic_number(*this);
}

ServiceAddress::ServiceAddress( const StubAddress & addrStub )
    : ServiceItem   ( static_cast<const ServiceItem &>(addrStub) )
    , mRoleName     ( addrStub.role_name() )
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
    mMagicNum = ServiceAddress::_magic_number(*this);
}

ServiceAddress::ServiceAddress( const ProxyAddress & addrProxy )
    : ServiceItem   ( static_cast<const ServiceItem &>(addrProxy) )
    , mRoleName     ( addrProxy.role_name() )
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
    mMagicNum = ServiceAddress::_magic_number(*this);
}

ServiceAddress::ServiceAddress( const InStream & stream )
    : ServiceItem   ( stream )
    , mRoleName     ( stream )
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
    mMagicNum = ServiceAddress::_magic_number(*this);
}

ServiceAddress::ServiceAddress( const ServiceAddress & source )
    : ServiceItem   ( static_cast<const ServiceItem &>(source) )
    , mRoleName     ( source.mRoleName )
    , mMagicNum     ( source.mMagicNum )
{
}

ServiceAddress::ServiceAddress( ServiceAddress && source ) noexcept
    : ServiceItem   ( static_cast<ServiceItem &&>(source) )
    , mRoleName     ( std::move(source.mRoleName) )
    , mMagicNum     ( source.mMagicNum )
{
}

String ServiceAddress::to_string() const
{
    String result( ServiceItem::to_string() );
    result.append(NECommon::COMPONENT_PATH_SEPARATOR).append(mRoleName);

    return result;
}

void ServiceAddress::conv_from_string(const char * pathService, const char** out_nextPart /*= nullptr */)
{
    const char* strSource   = pathService;
    ServiceItem::conv_from_string(pathService, &strSource);
    mRoleName   = String::substr(strSource, NECommon::COMPONENT_PATH_SEPARATOR.data(), &strSource);
    mMagicNum   = ServiceAddress::_magic_number(*this);

    if (out_nextPart != nullptr)
        *out_nextPart = strSource;
}

uint32_t ServiceAddress::_magic_number(const ServiceAddress addrService)
{
    uint32_t result = NEMath::CHECKSUM_IGNORE;
    if ( addrService.is_validated() )
    {
        result = NEMath::crc32_init();
        result = NEMath::crc32_start( result, addrService.mServiceName.as_string() );
        result = NEMath::crc32_start( result, static_cast<uint8_t>(addrService.mServiceType));
        result = NEMath::crc32_start( result, addrService.mRoleName.as_string() );
        result = NEMath::crc32_finish(result);
    }

    return result;
}
