/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ServiceItem.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Service Item object
 ************************************************************************/
#include "areg/component/ServiceItem.hpp"

#include "areg/base/IEIOStream.hpp"
#include "areg/base/NEUtilities.hpp"

#include <utility>

String ServiceItem::convAddressToPath( const ServiceItem & service )
{
    return service.convToString();
}

ServiceItem ServiceItem::convPathToAddress( const char* pathService, const char** out_nextPart /*= nullptr */ )
{
    ServiceItem result;
    result.convFromString(pathService, out_nextPart);
    return result;
}

ServiceItem::ServiceItem( void )
    : mServiceName      ( ServiceItem::INVALID_SERVICE )
    , mServiceVersion   ( Version::getInvalidVersion() )
    , mServiceType      ( NEService::eServiceType::ServiceLocal )
    , mMagicNum         ( NEMath::CHECKSUM_IGNORE )
{
}

ServiceItem::ServiceItem(const String & serviceName)
    : mServiceName      ( serviceName )
    , mServiceVersion   ( Version::getInvalidVersion() )
    , mServiceType      ( NEService::eServiceType::ServiceLocal )
    , mMagicNum         ( NEMath::CHECKSUM_IGNORE )
{
    mServiceName.truncate(NEUtilities::ITEM_NAMES_MAX_LENGTH);
    mMagicNum = ServiceItem::_magicNumber(*this);
}

ServiceItem::ServiceItem( const String & serviceName, const Version & serviceVersion, NEService::eServiceType serviceType )
    : mServiceName      ( serviceName )
    , mServiceVersion   ( serviceVersion )
    , mServiceType      ( serviceType )
    , mMagicNum         ( NEMath::CHECKSUM_IGNORE )
{
    mServiceName.truncate(NEUtilities::ITEM_NAMES_MAX_LENGTH);
    mMagicNum = ServiceItem::_magicNumber(*this);
}

ServiceItem::ServiceItem( const IEInStream & stream )
    : mServiceName      ( stream )
    , mServiceVersion   ( stream )
    , mServiceType      ( NEService::eServiceType::ServiceLocal )
    , mMagicNum         ( NEMath::CHECKSUM_IGNORE )
{
    stream >> mServiceType;
    mMagicNum = ServiceItem::_magicNumber(*this);
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

String ServiceItem::convToString(void) const
{
    String result(static_cast<uint32_t>(0xFF));

    result.append(mServiceName)
          .append(NECommon::COMPONENT_PATH_SEPARATOR)
          .append(mServiceVersion.convToString())
          .append(NECommon::COMPONENT_PATH_SEPARATOR)
          .append(String::toString(static_cast<int>(mServiceType), NEString::eRadix::RadixDecimal));

    return result;
}

void ServiceItem::convFromString(  const char* pathService, const char** out_nextPart /*= nullptr*/ )
{
    const char* strSource   = pathService;
    mServiceName        = String::getSubstring(strSource, NECommon::COMPONENT_PATH_SEPARATOR.data( ), &strSource);
    mServiceVersion     = String::getSubstring(strSource, NECommon::COMPONENT_PATH_SEPARATOR.data( ), &strSource);
    String serviceType  = String::getSubstring(strSource, NECommon::COMPONENT_PATH_SEPARATOR.data( ), &strSource);
    mServiceType        = static_cast<NEService::eServiceType>(serviceType.toInt32());
    mMagicNum           = ServiceItem::_magicNumber(*this);

    if (out_nextPart != nullptr)
        *out_nextPart = strSource;
}

unsigned int ServiceItem::_magicNumber(const ServiceItem svcItem)
{
    unsigned int result = NEMath::CHECKSUM_IGNORE;

    if (svcItem.isValidated())
    {
        result = NEMath::crc32Init();
        result = NEMath::crc32Start(result, svcItem.mServiceName.getString());
        result = NEMath::crc32Start(result, static_cast<unsigned char>(svcItem.mServiceType));
        result = NEMath::crc32Finish(result);
    }

    return result;
}
