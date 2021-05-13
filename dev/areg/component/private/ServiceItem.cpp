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
    String result = String::EmptyString;
    
    result += service.mServiceName;
    result += NEUtilities::COMPONENT_PATH_SEPARATOR;
    result += service.mServiceVersion.convToString();
    result += NEUtilities::COMPONENT_PATH_SEPARATOR;
    result += String::int32ToString(static_cast<int>(service.mServiceType), NEString::RadixDecimal);

    return result;
}

ServiceItem ServiceItem::convPathToAddress( const char* pathService, const char** out_nextPart /*= NULL */ )
{
    ServiceItem result;
    String serviceType;

    const char* strSource   = pathService;

    result.mServiceName     = String::getSubstring(strSource, NEUtilities::COMPONENT_PATH_SEPARATOR, &strSource);
    result.mServiceVersion  = String::getSubstring(strSource, NEUtilities::COMPONENT_PATH_SEPARATOR, &strSource);
    serviceType             = String::getSubstring(strSource, NEUtilities::COMPONENT_PATH_SEPARATOR, &strSource);
    result.mServiceType     = static_cast<NEService::eServiceType>(serviceType.convToInt32());

    if (out_nextPart != NULL)
        *out_nextPart = strSource;

    return result;
}

ServiceItem::ServiceItem( void )
    : mServiceName      ( ServiceItem::INVALID_SERVICE )
    , mServiceVersion   ( Version::INVALID_VERSION )
    , mServiceType      ( NEService::ServiceLocal )
{
    ; // do nothing
}

ServiceItem::ServiceItem(const char * serviceName)
    : mServiceName      ( serviceName != NULL ? serviceName : "")
    , mServiceVersion   ( Version::INVALID_VERSION )
    , mServiceType      ( NEService::ServiceLocal )
{
    mServiceName.truncate(NEUtilities::ITEM_NAMES_MAX_LENGTH);
}

ServiceItem::ServiceItem( const char * serviceName, const Version & serviceVersion, NEService::eServiceType serviceType )
    : mServiceName      ( serviceName != NULL ? serviceName : "")
    , mServiceVersion   ( serviceVersion )
    , mServiceType      ( serviceType )
{
    mServiceName.truncate(NEUtilities::ITEM_NAMES_MAX_LENGTH);
}

ServiceItem::ServiceItem( const IEInStream & stream )
    : mServiceName      ( stream )
    , mServiceVersion   ( stream )
    , mServiceType      ( NEService::ServiceLocal )
{
    stream >> mServiceType;
}

ServiceItem::ServiceItem( const ServiceItem & source )
    : mServiceName      ( source.mServiceName )
    , mServiceVersion   ( source.mServiceVersion )
    , mServiceType      ( source.mServiceType )
{
    ; // do nothing
}

ServiceItem::~ServiceItem(void)
{
    ; // do nothing
}

const ServiceItem & ServiceItem::operator = ( const ServiceItem & source )
{
    if ( static_cast<const ServiceItem *>(this) != &source )
    {
        mServiceName    = source.mServiceName;
        mServiceVersion = source.mServiceVersion;
        mServiceType    = source.mServiceType;
    }

    return (*this);
}

bool ServiceItem::operator == ( const ServiceItem & other ) const
{
    return (this != &other ? mServiceName == other.mServiceName && mServiceVersion == other.mServiceVersion && mServiceType == other.mServiceType : true);
}

bool ServiceItem::operator != (const ServiceItem & other ) const
{
    return (this != &other ? mServiceName != other.mServiceName || mServiceVersion != other.mServiceVersion || mServiceType != other.mServiceType : false);
}

ServiceItem::operator unsigned int( void ) const
{
    return static_cast<unsigned int>(mServiceName);
}

bool ServiceItem::isServiceCompatible( const ServiceItem & other ) const
{
    return ( this != &other ?   mServiceName == other.mServiceName && 
                                mServiceType == other.mServiceType &&
                                mServiceVersion.isCompatible(other.mServiceVersion)
                             : true );
}

AREG_API const IEInStream & operator >> ( const IEInStream & stream, ServiceItem & input )
{
    stream >> input.mServiceName;
    stream >> input.mServiceVersion;
    stream >> input.mServiceType;
    return stream;
}

AREG_API IEOutStream & operator << ( IEOutStream & stream, const ServiceItem & output )
{
    stream << output.mServiceName;
    stream << output.mServiceVersion;
    stream << output.mServiceType;
    return stream;
}
