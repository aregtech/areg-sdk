/************************************************************************
 * \file        areg/component/private/CEServiceItem.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Service Item object
 ************************************************************************/
#include "areg/component/CEServiceItem.hpp"
#include "areg/base/IEIOStream.hpp"
#include "areg/base/NEUtilities.hpp"

const char * const      CEServiceItem::INVALID_SERVICE      = "INVALID_SERVICE";

CEString CEServiceItem::ConvertAddressToPath( const CEServiceItem & service )
{
    CEString result = CEString::EmptyString;
    
    result += service.mServiceName;
    result += NEUtilities::COMPONENT_PATH_SEPARATOR;
    result += service.mServiceVersion.ToString();
    result += NEUtilities::COMPONENT_PATH_SEPARATOR;
    result += CEString::Int32ToString(static_cast<int>(service.mServiceType), NEString::RadixDecimal);

    return result;
}

CEServiceItem CEServiceItem::ConvertPathToAddress( const char* pathService, const char** out_nextPart /*= NULL */ )
{
    CEServiceItem result;
    CEString serviceType;

    const char* strSource   = pathService;

    result.mServiceName     = CEString::GetSubstring(strSource, NEUtilities::COMPONENT_PATH_SEPARATOR, &strSource);
    result.mServiceVersion  = CEString::GetSubstring(strSource, NEUtilities::COMPONENT_PATH_SEPARATOR, &strSource);
    serviceType             = CEString::GetSubstring(strSource, NEUtilities::COMPONENT_PATH_SEPARATOR, &strSource);
    result.mServiceType     = static_cast<NEService::eServiceType>(serviceType.ToInt32());

    if (out_nextPart != NULL)
        *out_nextPart = strSource;

    return result;
}

CEServiceItem::CEServiceItem( void )
    : mServiceName      ( CEServiceItem::INVALID_SERVICE )
    , mServiceVersion   ( CEVersion::INVALID_VERSION )
    , mServiceType      ( NEService::ServiceLocal )
{
    ; // do nothing
}

CEServiceItem::CEServiceItem(const char * serviceName)
    : mServiceName      ( serviceName != NULL ? serviceName : "")
    , mServiceVersion   ( CEVersion::INVALID_VERSION )
    , mServiceType      ( NEService::ServiceLocal )
{
    mServiceName.Truncate(NEUtilities::ITEM_NAMES_MAX_LENGTH);
}

CEServiceItem::CEServiceItem( const char * serviceName, const CEVersion & serviceVersion, NEService::eServiceType serviceType )
    : mServiceName      ( serviceName != NULL ? serviceName : "")
    , mServiceVersion   ( serviceVersion )
    , mServiceType      ( serviceType )
{
    mServiceName.Truncate(NEUtilities::ITEM_NAMES_MAX_LENGTH);
}

CEServiceItem::CEServiceItem( const IEInStream & stream )
    : mServiceName      ( stream )
    , mServiceVersion   ( stream )
    , mServiceType      ( NEService::ServiceLocal )
{
    stream >> mServiceType;
}

CEServiceItem::CEServiceItem( const CEServiceItem & source )
    : mServiceName      ( source.mServiceName )
    , mServiceVersion   ( source.mServiceVersion )
    , mServiceType      ( source.mServiceType )
{
    ; // do nothing
}

CEServiceItem::~CEServiceItem(void)
{
    ; // do nothing
}

const CEServiceItem & CEServiceItem::operator = ( const CEServiceItem & source )
{
    if ( static_cast<const CEServiceItem *>(this) != &source )
    {
        mServiceName    = source.mServiceName;
        mServiceVersion = source.mServiceVersion;
        mServiceType    = source.mServiceType;
    }

    return (*this);
}

bool CEServiceItem::operator == ( const CEServiceItem & other ) const
{
    return (this != &other ? mServiceName == other.mServiceName && mServiceVersion == other.mServiceVersion && mServiceType == other.mServiceType : true);
}

bool CEServiceItem::operator != (const CEServiceItem & other ) const
{
    return (this != &other ? mServiceName != other.mServiceName || mServiceVersion != other.mServiceVersion || mServiceType != other.mServiceType : false);
}

CEServiceItem::operator unsigned int( void ) const
{
    return static_cast<unsigned int>(mServiceName);
}

bool CEServiceItem::IsServiceCompatible( const CEServiceItem & other ) const
{
    return ( this != &other ?   mServiceName == other.mServiceName && 
                                mServiceType == other.mServiceType &&
                                mServiceVersion.IsCompatible(other.mServiceVersion)
                             : true );
}

AREG_API const IEInStream & operator >> ( const IEInStream & stream, CEServiceItem & input )
{
    stream >> input.mServiceName;
    stream >> input.mServiceVersion;
    stream >> input.mServiceType;
    return stream;
}

AREG_API IEOutStream & operator << ( IEOutStream & stream, const CEServiceItem & output )
{
    stream << output.mServiceName;
    stream << output.mServiceVersion;
    stream << output.mServiceType;
    return stream;
}
