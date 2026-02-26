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

namespace areg
{
    areg::String ServiceItem::convAddressToPath( const ServiceItem & service )
    {
        return service.convToString();
    }

    ServiceItem ServiceItem::convPathToAddress( const char* pathService, const char** out_nextPart /*= nullptr */ )
    {
        ServiceItem result;
        result.convFromString(pathService, out_nextPart);
        return result;
    }

    ServiceItem::ServiceItem()
        : mServiceName      ( ServiceItem::INVALID_SERVICE )
        , mServiceVersion   ( areg::Version::getInvalidVersion() )
        , mServiceType      ( areg::ServiceType::Local )
        , mMagicNum         ( areg::CHECKSUM_IGNORE )
    {
    }

    ServiceItem::ServiceItem(const areg::String & serviceName)
        : mServiceName      ( serviceName )
        , mServiceVersion   ( areg::Version::getInvalidVersion() )
        , mServiceType      ( areg::ServiceType::Local )
        , mMagicNum         ( areg::CHECKSUM_IGNORE )
    {
        mServiceName.truncate(areg::ITEM_NAMES_MAX_LENGTH);
        mMagicNum = ServiceItem::_magicNumber(*this);
    }

    ServiceItem::ServiceItem( const areg::String & serviceName, const areg::Version & serviceVersion, areg::ServiceType serviceType )
        : mServiceName      ( serviceName )
        , mServiceVersion   ( serviceVersion )
        , mServiceType      ( serviceType )
        , mMagicNum         ( areg::CHECKSUM_IGNORE )
    {
        mServiceName.truncate(areg::ITEM_NAMES_MAX_LENGTH);
        mMagicNum = ServiceItem::_magicNumber(*this);
    }

    ServiceItem::ServiceItem( const areg::InStream & stream )
        : mServiceName      ( stream )
        , mServiceVersion   ( stream )
        , mServiceType      ( areg::ServiceType::Local )
        , mMagicNum         ( areg::CHECKSUM_IGNORE )
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

    areg::String ServiceItem::convToString() const
    {
        areg::String result(static_cast<uint32_t>(0xFF));

        result.append(mServiceName)
            .append(areg::COMPONENT_PATH_SEPARATOR)
            .append(mServiceVersion.convToString())
            .append(areg::COMPONENT_PATH_SEPARATOR)
            .append(areg::String::makeString(static_cast<int32_t>(mServiceType), areg::Radix::Decimal));

        return result;
    }

    void ServiceItem::convFromString(  const char* pathService, const char** out_nextPart /*= nullptr*/ )
    {
        const char* strSource   = pathService;
        mServiceName        = areg::String::getSubstring(strSource, areg::COMPONENT_PATH_SEPARATOR.data( ), &strSource);
        mServiceVersion     = areg::String::getSubstring(strSource, areg::COMPONENT_PATH_SEPARATOR.data( ), &strSource);
        areg::String serviceType  = areg::String::getSubstring(strSource, areg::COMPONENT_PATH_SEPARATOR.data( ), &strSource);
        mServiceType        = static_cast<areg::ServiceType>(serviceType.toInt32());
        mMagicNum           = ServiceItem::_magicNumber(*this);

        if (out_nextPart != nullptr)
            *out_nextPart = strSource;
    }

    uint32_t ServiceItem::_magicNumber(const ServiceItem svcItem)
    {
        uint32_t result = areg::CHECKSUM_IGNORE;

        if (svcItem.isValidated())
        {
            result = areg::crc32Init();
            result = areg::crc32Start(result, svcItem.mServiceName.getString());
            result = areg::crc32Start(result, static_cast<uint8_t>(svcItem.mServiceType));
            result = areg::crc32Finish(result);
        }

        return result;
    }

} // namespace areg