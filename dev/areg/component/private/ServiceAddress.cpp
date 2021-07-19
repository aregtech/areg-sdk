/************************************************************************
 * \file        areg/component/private/ServiceAddress.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
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

String ServiceAddress::convAddressToPath( const ServiceAddress & addService )
{
    return addService.convToString();
}

ServiceAddress ServiceAddress::convPathToAddress( const char * pathService, const char ** out_nextPart /*= NULL */ )
{
    ServiceAddress result;
    result.convFromString(pathService, out_nextPart);
    return result;
}

ServiceAddress::ServiceAddress( void )
    : ServiceItem   ( )
    , mRoleName     ( String::EmptyString )
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
    ; // do nothing
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
    : ServiceItem   ( static_cast<const ServiceItem >(source) )
    , mRoleName     ( source.mRoleName )
    , mMagicNum     ( source.mMagicNum )
{
    ; // do nothing
}

ServiceAddress::~ServiceAddress(void)
{
    ; // do nothing
}

String ServiceAddress::convToString(void) const
{
    String result = ServiceItem::convToString();
    result += NEUtilities::COMPONENT_PATH_SEPARATOR;
    result += mRoleName;

    return result;
}

void ServiceAddress::convFromString(const char * pathService, const char** out_nextPart /*= NULL */)
{
    const char* strSource   = pathService;
    ServiceItem::convFromString(pathService, &strSource);
    mRoleName   = String::getSubstring(strSource, NEUtilities::COMPONENT_PATH_SEPARATOR, &strSource);
    mMagicNum   = ServiceAddress::_magicNumber(*this);

    if (out_nextPart != NULL)
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
