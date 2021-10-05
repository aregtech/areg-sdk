/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ServiceAddress.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Generic Service Address object
 *
 ************************************************************************/
#include "areg/component/ServiceAddress.hpp"

#include "areg/component/ProxyAddress.hpp"
#include "areg/component/StubAddress.hpp"
#include "areg/base/IEIOStream.hpp"
#include "areg/base/NEUtilities.hpp"
#include "areg/base/NEMath.hpp"
#include "areg/component/DispatcherThread.hpp"

#include <utility>

String ServiceAddress::convAddressToPath( const ServiceAddress & addService )
{
    return addService.convToString();
}

ServiceAddress ServiceAddress::convPathToAddress( const char * pathService, const char ** out_nextPart /*= nullptr */ )
{
    ServiceAddress result;
    result.convFromString(pathService, out_nextPart);
    return result;
}

ServiceAddress::ServiceAddress( void )
    : ServiceItem   ( )
    , mRoleName     ( String::EmptyString.data(), 0 )
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
}

ServiceAddress::ServiceAddress( const char * serviceName
                              , const Version & serviceVersion
                              , NEService::eServiceType serviceType
                              , const char * roleName )
    : ServiceItem   ( serviceName, serviceVersion, serviceType )
    , mRoleName     ( roleName )
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
    mRoleName.truncate( NEUtilities::ITEM_NAMES_MAX_LENGTH );
    mMagicNum = ServiceAddress::_magicNumber(*this);
}

ServiceAddress::ServiceAddress( const ServiceItem & serviceItem, const char * roleName )
    : ServiceItem   ( serviceItem )
    , mRoleName     ( roleName )
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
    mRoleName.truncate( NEUtilities::ITEM_NAMES_MAX_LENGTH );
    mMagicNum = ServiceAddress::_magicNumber(*this);
}

ServiceAddress::ServiceAddress( const StubAddress & addrStub )
    : ServiceItem   ( static_cast<const ServiceItem &>(addrStub) )
    , mRoleName     ( addrStub.getRoleName() )
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
    mMagicNum = ServiceAddress::_magicNumber(*this);
}

ServiceAddress::ServiceAddress( const ProxyAddress & addrProxy )
    : ServiceItem   ( static_cast<const ServiceItem &>(addrProxy) )
    , mRoleName     ( addrProxy.getRoleName() )
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
    mMagicNum = ServiceAddress::_magicNumber(*this);
}

ServiceAddress::ServiceAddress( const IEInStream & stream )
    : ServiceItem   ( stream )
    , mRoleName     ( stream )
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
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

String ServiceAddress::convToString(void) const
{
    String result = ServiceItem::convToString();
    result += NECommon::COMPONENT_PATH_SEPARATOR.data();
    result += mRoleName;

    return result;
}

void ServiceAddress::convFromString(const char * pathService, const char** out_nextPart /*= nullptr */)
{
    const char* strSource   = pathService;
    ServiceItem::convFromString(pathService, &strSource);
    mRoleName   = String::getSubstring(strSource, NECommon::COMPONENT_PATH_SEPARATOR.data(), &strSource);
    mMagicNum   = ServiceAddress::_magicNumber(*this);

    if (out_nextPart != nullptr)
        *out_nextPart = strSource;
}

unsigned int ServiceAddress::_magicNumber(const ServiceAddress addrService)
{
    unsigned int result = NEMath::CHECKSUM_IGNORE;
    if ( addrService.isValidated() )
    {
        result = NEMath::crc32Init();
        result = NEMath::crc32Start( result, addrService.mServiceName.getString() );
        result = NEMath::crc32Start( result, static_cast<unsigned char>(addrService.mServiceType));
        result = NEMath::crc32Start( result, addrService.mRoleName.getString() );
        result = NEMath::crc32Finish(result);
    }

    return result;
}
