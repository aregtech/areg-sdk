/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
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

namespace
{
    //!< The FNV-1a 32-bit offset basis.
    constexpr uint32_t  FNV1A_INIT  { 2'166'136'261u };

    //!< The FNV-1a 32-bit prime.
    constexpr uint32_t  FNV1A_PRIME { 16'777'619u };

    //!< Adds one byte to the FNV-1a hash.
    inline uint32_t _fnv1a(uint32_t hash, uint8_t value) noexcept
    {
        return (hash ^ value) * FNV1A_PRIME;
    }

    //!< Adds the characters of the text to the FNV-1a hash.
    inline uint32_t _fnv1a(uint32_t hash, const areg::String & text) noexcept
    {
        const char * data{ text.as_string() };
        const areg::CharCount count{ text.length() };
        for (areg::CharCount i = 0; i < count; ++ i)
        {
            hash = _fnv1a(hash, static_cast<uint8_t>(data[i]));
        }

        return hash;
    }
}

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
    areg::cut_item_name(mRoleName);
    mMagicNum = areg::crc32_calculate(mRoleName.as_string());
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
    areg::cut_item_name(mRoleName);
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
    , mNameCheck    ( ServiceAddress::NAME_CHECK_UNKNOWN )
{
}

ServiceAddress::ServiceAddress( const String & serviceName, const Version & serviceVersion, areg::ServiceType serviceType, const String & roleName )
    : ServiceItem   ( serviceName, serviceVersion, serviceType )
    , mRoleName     ( roleName )
    , mMagicNum     ( areg::CHECKSUM_IGNORE )
    , mNameCheck    ( ServiceAddress::NAME_CHECK_UNKNOWN )
{
    areg::cut_item_name(mRoleName);
    mMagicNum  = ServiceAddress::_magic_number(*this);
    mNameCheck = ServiceAddress::_name_check(*this);
}

ServiceAddress::ServiceAddress(const UniqueNumber serviceNum, const Version& serviceVersion, areg::ServiceType serviceType, const UniqueNumber roleNum)
    : ServiceItem   (serviceNum, serviceVersion, serviceType)
    , mRoleName     ( )
    , mMagicNum     (roleNum)
    , mNameCheck    ( ServiceAddress::NAME_CHECK_UNKNOWN )
{
}

ServiceAddress::ServiceAddress( const ServiceItem & serviceItem, const String & roleName )
    : ServiceItem   ( serviceItem )
    , mRoleName     ( roleName )
    , mMagicNum     ( areg::CHECKSUM_IGNORE )
    , mNameCheck    ( ServiceAddress::NAME_CHECK_UNKNOWN )
{
    areg::cut_item_name(mRoleName);
    if (areg::parse_address_number(mRoleName.data(), mMagicNum))
        mRoleName.clear();
    else
        mMagicNum = ServiceAddress::_magic_number(*this);

    mNameCheck = ServiceAddress::_name_check(*this);
}

ServiceAddress::ServiceAddress(const ServiceItem& serviceItem, const UniqueNumber roleNum)
    : ServiceItem   ( serviceItem )
    , mRoleName     ( )
    , mMagicNum     ( roleNum )
    , mNameCheck    ( ServiceAddress::NAME_CHECK_UNKNOWN )
{
}

ServiceAddress::ServiceAddress( const StubAddress & addrStub )
    : ServiceItem   ( static_cast<const ServiceItem &>(addrStub) )
    , mRoleName     ( addrStub.role_name() )
    , mMagicNum     ( static_cast<uint32_t>(static_cast<const ServiceAddress&>(addrStub)) )
    , mNameCheck    ( addrStub.name_check() )
{
}

ServiceAddress::ServiceAddress( const ProxyAddress & addrProxy )
    : ServiceItem   ( static_cast<const ServiceItem &>(addrProxy) )
    , mRoleName     ( addrProxy.role_name() )
    , mMagicNum     ( static_cast<uint32_t>(static_cast<const ServiceAddress&>(addrProxy)) )
    , mNameCheck    ( addrProxy.name_check() )
{
}

ServiceAddress::ServiceAddress(const areg::RawService& rawService, const areg::Endpoint& endPoint)
    : ServiceItem   ( rawService, endPoint )
    , mRoleName     ( )
    , mMagicNum     ( rawService.role )
    , mNameCheck    ( ServiceAddress::NAME_CHECK_UNKNOWN )
{
}

ServiceAddress::ServiceAddress( ServiceAddress && source ) noexcept
    : ServiceItem   ( static_cast<ServiceItem &&>(source) )
    , mRoleName     ( std::move(source.mRoleName) )
    , mMagicNum     ( source.mMagicNum )
    , mNameCheck    ( source.mNameCheck )
{
    source.mMagicNum  = areg::CHECKSUM_IGNORE;
    source.mNameCheck = ServiceAddress::NAME_CHECK_UNKNOWN;
}

String ServiceAddress::to_string() const
{
    String result{ ServiceItem::to_string() };
    result.append(areg::COMPONENT_PATH_SEPARATOR);
    if (mRoleName.is_empty())
        result.append(areg::ADDRESS_NUMBER_MARKER).append(String::make_string(mMagicNum));
    else
        result.append(mRoleName);

    return result;
}

void ServiceAddress::from_string(const char * pathService, const char** nextPart /*= nullptr */)
{
    const char* strSource   = pathService;
    ServiceItem::from_string(pathService, &strSource);
    mRoleName   = String::substr(strSource, areg::COMPONENT_PATH_SEPARATOR.data(), &strSource);
    mRoleName.truncate(areg::ITEM_NAMES_MAX_LENGTH);
    if (areg::parse_address_number(mRoleName.data(), mMagicNum))
        mRoleName.clear();
    else
        mMagicNum = ServiceAddress::_magic_number(*this);

    mNameCheck = ServiceAddress::_name_check(*this);

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

uint32_t ServiceAddress::_name_check(const ServiceAddress & addrService) noexcept
{
    uint32_t result{ ServiceAddress::NAME_CHECK_UNKNOWN };
    if (!addrService.mServiceName.is_empty() && (addrService.mServiceName != ServiceItem::INVALID_SERVICE.data()) &&
        !addrService.mRoleName.is_empty()    && (addrService.mServiceType != areg::ServiceType::Invalid))
    {
        const uint16_t type{ static_cast<uint16_t>(addrService.mServiceType) };
        result = _fnv1a(FNV1A_INIT, addrService.mServiceName);
        result = _fnv1a(result, areg::NAME_SEPARATOR);
        result = _fnv1a(result, static_cast<uint8_t>(type & 0xFFu));
        result = _fnv1a(result, static_cast<uint8_t>(type >> 8));
        result = _fnv1a(result, areg::NAME_SEPARATOR);
        result = _fnv1a(result, addrService.mRoleName);
        result += static_cast<uint32_t>(result == ServiceAddress::NAME_CHECK_UNKNOWN);
    }

    return result;
}

} // namespace areg
