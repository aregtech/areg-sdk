/************************************************************************
 * \file        areg/src/component/private/CEServiceAddress.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Generic Service Address object
 *
 ************************************************************************/
#include "areg/src/component/CEServiceAddress.hpp"
#include "areg/src/component/CEProxyAddress.hpp"
#include "areg/src/component/CEStubAddress.hpp"
#include "areg/src/base/IEIOStream.hpp"
#include "areg/src/base/NEUtilities.hpp"
#include "areg/src/base/NEMath.hpp"
#include "areg/src/component/CEDispatcherThread.hpp"

CEString CEServiceAddress::ConvertAddressToPath( const CEServiceAddress & addService )
{
    CEString result = CEServiceItem::ConvertAddressToPath( static_cast<const CEServiceItem &>(addService) );
    result += NEUtilities::COMPONENT_PATH_SEPARATOR;
    result += addService.mRoleName;

    return result;
}

CEServiceAddress CEServiceAddress::ConvertPathToAddress( const char * pathService, const char ** out_nextPart /*= NULL */ )
{
    CEServiceAddress result;
    const char* strSource   = pathService;
    
    static_cast<CEServiceItem &>(result) = CEServiceItem::ConvertPathToAddress( pathService, out_nextPart);
    result.mRoleName    = CEString::GetSubstring(strSource, NEUtilities::COMPONENT_PATH_SEPARATOR, &strSource);

    if (out_nextPart != NULL)
        *out_nextPart = strSource;

    return result;
}

CEServiceAddress::CEServiceAddress( void )
    : CEServiceItem ( )
    , mRoleName     ( "" )
{
    ; // do nothing
}

CEServiceAddress::CEServiceAddress( const char * serviceName
                                  , const CEVersion & serviceVersion
                                  , NEService::eServiceType serviceType
                                  , const char * roleName )
    : CEServiceItem ( serviceName, serviceVersion, serviceType )
    , mRoleName     ( roleName )
{
    mRoleName.Truncate( NEUtilities::ITEM_NAMES_MAX_LENGTH );
}

CEServiceAddress::CEServiceAddress( const CEServiceItem & serviceItem, const char * roleName )
    : CEServiceItem ( serviceItem )
    , mRoleName     ( roleName )
{
    mRoleName.Truncate( NEUtilities::ITEM_NAMES_MAX_LENGTH );
}

CEServiceAddress::CEServiceAddress( const CEStubAddress & addrStub )
    : CEServiceItem ( static_cast<const CEServiceItem &>(addrStub) )
    , mRoleName     ( addrStub.GetRoleName() )
{
    ; // do nothing
}

CEServiceAddress::CEServiceAddress( const CEProxyAddress & addrProxy )
    : CEServiceItem ( static_cast<const CEServiceItem &>(addrProxy) )
    , mRoleName     ( addrProxy.GetRoleName() )
{
    ; // do nothing
}

CEServiceAddress::CEServiceAddress( const IEInStream & stream )
    : CEServiceItem ( stream )
    , mRoleName     ( stream )
{
    ; // do nothing
}

CEServiceAddress::CEServiceAddress( const CEServiceAddress & source )
    : CEServiceItem ( static_cast<const CEServiceItem &>(source) )
    , mRoleName     ( source.mRoleName )
{
    ; // do nothing
}

CEServiceAddress::~CEServiceAddress(void)
{
    ; // do nothing
}

const CEServiceAddress & CEServiceAddress::operator = ( const CEServiceAddress & source )
{
    if ( static_cast<const CEServiceAddress *>(this) != &source )
    {
        static_cast<CEServiceItem &>(*this) = static_cast<const CEServiceItem &>(source);
        mRoleName   = source.mRoleName;
    }

    return (*this);
}

bool CEServiceAddress::operator == ( const CEServiceAddress & other ) const
{
    return (this != &other ? static_cast<const CEServiceItem &>(*this) == static_cast<const CEServiceItem &>(other) && mRoleName == other.mRoleName : true);
}

bool CEServiceAddress::operator != (const CEServiceAddress & other) const
{
    return (this != &other ? static_cast<const CEServiceItem &>(*this) != static_cast<const CEServiceItem &>(other) || mRoleName != other.mRoleName : false);
}

CEServiceAddress::operator unsigned int( void ) const
{
    unsigned int result = NEMath::CHECKSUM_IGNORE;
    if ( IsValid() )
    {
        result = NEMath::Crc32Init();
        result = NEMath::Crc32Start( result, static_cast<const char *>(mRoleName) );
        result = NEMath::Crc32Start(result, static_cast<const char *>(mServiceName));
        result = NEMath::Crc32Finish(result);
    }
    return result;
}
