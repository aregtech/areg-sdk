/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ServiceItem.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Service Item object
 ************************************************************************/
#include "areg/component/ServiceItem.hpp"

#include "areg/base/IOStream.hpp"
#include "areg/base/UtilityDefs.hpp"

#include <utility>
namespace areg {

String ServiceItem::to_path( const ServiceItem & service )
{
    return service.to_string();
}

ServiceItem ServiceItem::from_path( const char* pathService, const char** out_nextPart /*= nullptr */ )
{
    ServiceItem result;
    result.conv_from_string(pathService, out_nextPart);
    return result;
}

ServiceItem::ServiceItem()
    : mServiceName      ( ServiceItem::INVALID_SERVICE )
    , mServiceVersion   ( Version::invalid_version() )
    , mServiceType      ( areg::ServiceType::Local )
    , mMagicNum         ( areg::CHECKSUM_IGNORE )
{
}

ServiceItem::ServiceItem(const String & serviceName)
    : mServiceName      ( serviceName )
    , mServiceVersion   ( Version::invalid_version() )
    , mServiceType      ( areg::ServiceType::Local )
    , mMagicNum         ( areg::CHECKSUM_IGNORE )
{
    mServiceName.truncate(areg::ITEM_NAMES_MAX_LENGTH);
    mMagicNum = ServiceItem::_magic_number(*this);
}

ServiceItem::ServiceItem( const String & serviceName, const Version & serviceVersion, areg::ServiceType serviceType )
    : mServiceName      ( serviceName )
    , mServiceVersion   ( serviceVersion )
    , mServiceType      ( serviceType )
    , mMagicNum         ( areg::CHECKSUM_IGNORE )
{
    mServiceName.truncate(areg::ITEM_NAMES_MAX_LENGTH);
    mMagicNum = ServiceItem::_magic_number(*this);
}

ServiceItem::ServiceItem( const InStream & stream )
    : mServiceName      ( stream )
    , mServiceVersion   ( stream )
    , mServiceType      ( areg::ServiceType::Local )
    , mMagicNum         ( areg::CHECKSUM_IGNORE )
{
    stream >> mServiceType;
    mMagicNum = ServiceItem::_magic_number(*this);
}

ServiceItem::ServiceItem( const ServiceItem & source )
    : mServiceName      ( source.mServiceName )
    , mServiceVersion   ( source.mServiceVersion )
    , mServiceType      ( source.mServiceType )
    , mMagicNum         ( source.mMagicNum )
{
}

ServiceItem::ServiceItem( ServiceItem && source ) noexcept
    : mServiceName      ( std::move(source.mServiceName) )
    , mServiceVersion   ( std::move(source.mServiceVersion) )
    , mServiceType      ( source.mServiceType )
    , mMagicNum         ( source.mMagicNum )
{
}

String ServiceItem::to_string() const
{
    String result(static_cast<uint32_t>(0xFF));

    result.append(mServiceName)
          .append(areg::COMPONENT_PATH_SEPARATOR)
          .append(mServiceVersion.to_string())
          .append(areg::COMPONENT_PATH_SEPARATOR)
          .append(String::make_string(static_cast<int32_t>(mServiceType), areg::Radix::Decimal));

    return result;
}

void ServiceItem::conv_from_string(  const char* pathService, const char** out_nextPart /*= nullptr*/ )
{
    const char* strSource   = pathService;
    mServiceName        = String::substr(strSource, areg::COMPONENT_PATH_SEPARATOR.data( ), &strSource);
    mServiceVersion     = String::substr(strSource, areg::COMPONENT_PATH_SEPARATOR.data( ), &strSource);
    String serviceType  = String::substr(strSource, areg::COMPONENT_PATH_SEPARATOR.data( ), &strSource);
    mServiceType        = static_cast<areg::ServiceType>(serviceType.to_int32());
    mMagicNum           = ServiceItem::_magic_number(*this);

    if (out_nextPart != nullptr)
        *out_nextPart = strSource;
}

uint32_t ServiceItem::_magic_number(const ServiceItem svcItem)
{
    uint32_t result = areg::CHECKSUM_IGNORE;

    if (svcItem.is_validated())
    {
        result = areg::crc32_init();
        result = areg::crc32_start(result, svcItem.mServiceName.as_string());
        result = areg::crc32_start(result, static_cast<uint8_t>(svcItem.mServiceType));
        result = areg::crc32_finish(result);
    }

    return result;
}

} // namespace areg
