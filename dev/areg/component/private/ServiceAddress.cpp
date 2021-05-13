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
    String result = ServiceItem::convAddressToPath( static_cast<const ServiceItem &>(addService) );
    result += NEUtilities::COMPONENT_PATH_SEPARATOR;
    result += addService.mRoleName;

    return result;
}

ServiceAddress ServiceAddress::convPathToAddress( const char * pathService, const char ** out_nextPart /*= NULL */ )
{
    ServiceAddress result;
    const char* strSource   = pathService;
    
    static_cast<ServiceItem &>(result) = ServiceItem::convPathToAddress( pathService, out_nextPart);
    result.mRoleName    = String::getSubstring(strSource, NEUtilities::COMPONENT_PATH_SEPARATOR, &strSource);

    if (out_nextPart != NULL)
        *out_nextPart = strSource;

    return result;
}

ServiceAddress::ServiceAddress( void )
    : ServiceItem   ( )
    , mRoleName     ( String::EmptyString )
{
    ; // do nothing
}

ServiceAddress::ServiceAddress( const char * serviceName
                                  , const Version & serviceVersion
                                  , NEService::eServiceType serviceType
                                  , const char * roleName )
    : ServiceItem   ( serviceName, serviceVersion, serviceType )
    , mRoleName     ( roleName )
{
    mRoleName.truncate( NEUtilities::ITEM_NAMES_MAX_LENGTH );
}

ServiceAddress::ServiceAddress( const ServiceItem & serviceItem, const char * roleName )
    : ServiceItem   ( serviceItem )
    , mRoleName     ( roleName )
{
    mRoleName.truncate( NEUtilities::ITEM_NAMES_MAX_LENGTH );
}

ServiceAddress::ServiceAddress( const StubAddress & addrStub )
    : ServiceItem   ( static_cast<const ServiceItem &>(addrStub) )
    , mRoleName     ( addrStub.getRoleName() )
{
    ; // do nothing
}

ServiceAddress::ServiceAddress( const ProxyAddress & addrProxy )
    : ServiceItem   ( static_cast<const ServiceItem &>(addrProxy) )
    , mRoleName     ( addrProxy.getRoleName() )
{
    ; // do nothing
}

ServiceAddress::ServiceAddress( const IEInStream & stream )
    : ServiceItem   ( stream )
    , mRoleName     ( stream )
{
    ; // do nothing
}

ServiceAddress::ServiceAddress( const ServiceAddress & source )
    : ServiceItem   ( static_cast<const ServiceItem &>(source) )
    , mRoleName     ( source.mRoleName )
{
    ; // do nothing
}

ServiceAddress::~ServiceAddress(void)
{
    ; // do nothing
}

const ServiceAddress & ServiceAddress::operator = ( const ServiceAddress & source )
{
    if ( static_cast<const ServiceAddress *>(this) != &source )
    {
        static_cast<ServiceItem &>(*this) = static_cast<const ServiceItem &>(source);
        mRoleName   = source.mRoleName;
    }

    return (*this);
}

bool ServiceAddress::operator == ( const ServiceAddress & other ) const
{
    return (this != &other ? static_cast<const ServiceItem &>(*this) == static_cast<const ServiceItem &>(other) && mRoleName == other.mRoleName : true);
}

bool ServiceAddress::operator != (const ServiceAddress & other) const
{
    return (this != &other ? static_cast<const ServiceItem &>(*this) != static_cast<const ServiceItem &>(other) || mRoleName != other.mRoleName : false);
}

ServiceAddress::operator unsigned int( void ) const
{
    unsigned int result = NEMath::CHECKSUM_IGNORE;
    if ( isValid() )
    {
        result = NEMath::crc32Init();
        result = NEMath::crc32Start( result, static_cast<const char *>(mRoleName) );
        result = NEMath::crc32Start(result, static_cast<const char *>(mServiceName));
        result = NEMath::crc32Finish(result);
    }
    return result;
}
