/************************************************************************
 * \file        areg/component/private/ServiceItem.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Service Item object
 ************************************************************************/
#include "areg/component/ServiceItem.hpp"
#include "areg/base/IEIOStream.hpp"
#include "areg/base/NEUtilities.hpp"

const char * const      ServiceItem::INVALID_SERVICE      = "INVALID_SERVICE";

String ServiceItem::convAddressToPath( const ServiceItem & service )
{
    return service.convToString();
}

ServiceItem ServiceItem::convPathToAddress( const char* pathService, const char** out_nextPart /*= NULL */ )
{
    ServiceItem result;
    result.convFromString(pathService, out_nextPart);
    return result;
}

ServiceItem::ServiceItem( void )
    : mServiceName      ( ServiceItem::INVALID_SERVICE )
    , mServiceVersion   ( Version::INVALID_VERSION )
    , mServiceType      ( NEService::ServiceLocal )
    , mMagicNum         ( NEMath::CHECKSUM_IGNORE )
{
}

ServiceItem::ServiceItem(const char * serviceName)
    : mServiceName      ( serviceName != NULL ? serviceName : "")
    , mServiceVersion   ( Version::INVALID_VERSION )
    , mServiceType      ( NEService::ServiceLocal )
    , mMagicNum         ( NEMath::CHECKSUM_IGNORE )
{
    mServiceName.truncate(NEUtilities::ITEM_NAMES_MAX_LENGTH);
    mMagicNum = ServiceItem::_magicNumber(*this);
}

ServiceItem::ServiceItem( const char * serviceName, const Version & serviceVersion, NEService::eServiceType serviceType )
    : mServiceName      ( serviceName != NULL ? serviceName : "")
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
    , mServiceType      ( NEService::ServiceLocal )
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
    ; // do nothing
}

ServiceItem::~ServiceItem(void)
{
    ; // do nothing
}

String ServiceItem::convToString(void) const
{
    String result = String::EmptyString;

    result += mServiceName;
    result += NEUtilities::COMPONENT_PATH_SEPARATOR;
    result += mServiceVersion.convToString();
    result += NEUtilities::COMPONENT_PATH_SEPARATOR;
    result += String::int32ToString(static_cast<int>(mServiceType), NEString::RadixDecimal);

    return result;
}

void ServiceItem::convFromString(  const char* pathService, const char** out_nextPart /*= NULL*/ )
{
    const char* strSource   = pathService;
    mServiceName        = String::getSubstring(strSource, NEUtilities::COMPONENT_PATH_SEPARATOR, &strSource);
    mServiceVersion     = String::getSubstring(strSource, NEUtilities::COMPONENT_PATH_SEPARATOR, &strSource);
    String serviceType  = String::getSubstring(strSource, NEUtilities::COMPONENT_PATH_SEPARATOR, &strSource);
    mServiceType        = static_cast<NEService::eServiceType>(serviceType.convToInt32());
    mMagicNum           = ServiceItem::_magicNumber(*this);

    if (out_nextPart != NULL)
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

AREG_API const IEInStream & operator >> ( const IEInStream & stream, ServiceItem & input )
{
    stream >> input.mServiceName;
    stream >> input.mServiceVersion;
    stream >> input.mServiceType;
    
    input.mMagicNum = ServiceItem::_magicNumber(input);

    return stream;
}

AREG_API IEOutStream & operator << ( IEOutStream & stream, const ServiceItem & output )
{
    stream << output.mServiceName;
    stream << output.mServiceVersion;
    stream << output.mServiceType;
    return stream;
}
