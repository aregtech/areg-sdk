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

areg::String ServiceAddress::convAddressToPath( const ServiceAddress & addService )
{
    return addService.convToString();
}

ServiceAddress ServiceAddress::convPathToAddress( const char * pathService, const char ** out_nextPart /*= nullptr */ )
{
    ServiceAddress result;
    result.convFromString(pathService, out_nextPart);
    return result;
}

ServiceAddress::ServiceAddress()
    : ServiceItem   ( )
    , mRoleName     ( areg::String::getEmptyString(), 0 )
    , mMagicNum     ( areg::CHECKSUM_IGNORE )
{
}

ServiceAddress::ServiceAddress( const areg::String & serviceName
                              , const Version & serviceVersion
                              , areg::ServiceType serviceType
                              , const areg::String & roleName )
    : ServiceItem   ( serviceName, serviceVersion, serviceType )
    , mRoleName     ( roleName )
    , mMagicNum     ( areg::CHECKSUM_IGNORE )
{
    mRoleName.truncate( areg::ITEM_NAMES_MAX_LENGTH );
    mMagicNum = ServiceAddress::_magicNumber(*this);
}

ServiceAddress::ServiceAddress( const ServiceItem & serviceItem, const areg::String & roleName )
    : ServiceItem   ( serviceItem )
    , mRoleName     ( roleName )
    , mMagicNum     ( areg::CHECKSUM_IGNORE )
{
    mRoleName.truncate( areg::ITEM_NAMES_MAX_LENGTH );
    mMagicNum = ServiceAddress::_magicNumber(*this);
}

ServiceAddress::ServiceAddress( const StubAddress & addrStub )
    : ServiceItem   ( static_cast<const ServiceItem &>(addrStub) )
    , mRoleName     ( addrStub.getRoleName() )
    , mMagicNum     ( areg::CHECKSUM_IGNORE )
{
    mMagicNum = ServiceAddress::_magicNumber(*this);
}

ServiceAddress::ServiceAddress( const ProxyAddress & addrProxy )
    : ServiceItem   ( static_cast<const ServiceItem &>(addrProxy) )
    , mRoleName     ( addrProxy.getRoleName() )
    , mMagicNum     ( areg::CHECKSUM_IGNORE )
{
    mMagicNum = ServiceAddress::_magicNumber(*this);
}

ServiceAddress::ServiceAddress( const InStream & stream )
    : ServiceItem   ( stream )
    , mRoleName     ( stream )
    , mMagicNum     ( areg::CHECKSUM_IGNORE )
{
    mMagicNum = ServiceAddress::_magicNumber(*this);
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

areg::String ServiceAddress::convToString() const
{
    areg::String result( ServiceItem::convToString() );
    result.append(areg::COMPONENT_PATH_SEPARATOR).append(mRoleName);

    return result;
}

void ServiceAddress::convFromString(const char * pathService, const char** out_nextPart /*= nullptr */)
{
    const char* strSource   = pathService;
    ServiceItem::convFromString(pathService, &strSource);
    mRoleName   = areg::String::getSubstring(strSource, areg::COMPONENT_PATH_SEPARATOR.data(), &strSource);
    mMagicNum   = ServiceAddress::_magicNumber(*this);

    if (out_nextPart != nullptr)
        *out_nextPart = strSource;
}

uint32_t ServiceAddress::_magicNumber(const ServiceAddress addrService)
{
    uint32_t result = areg::CHECKSUM_IGNORE;
    if ( addrService.isValidated() )
    {
        result = areg::crc32Init();
        result = areg::crc32Start( result, addrService.mServiceName.getString() );
        result = areg::crc32Start( result, static_cast<uint8_t>(addrService.mServiceType));
        result = areg::crc32Start( result, addrService.mRoleName.getString() );
        result = areg::crc32Finish(result);
    }

    return result;
}
