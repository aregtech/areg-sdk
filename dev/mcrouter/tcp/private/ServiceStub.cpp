/************************************************************************
 * \file        mcrouter/tcp/private/ServiceStub.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Stub Service object
 ************************************************************************/
#include "mcrouter/tcp/private/ServiceStub.hpp"
#include "areg/component/ProxyAddress.hpp"

ServiceStub::ServiceStub( void )
    : mStubAddress  ( )
    , mConnectStatus( NEService::ServiceConnectionUnknown )
{
    ; // do nothing
}

ServiceStub::ServiceStub( const StubAddress & addrStub )
    : mStubAddress  ( addrStub )
    , mConnectStatus( addrStub.isValid() ? NEService::ServicePending : NEService::ServiceConnectionUnknown )
{
    ; // do nothing
}

ServiceStub::ServiceStub( const ProxyAddress & addrProxy )
    : mStubAddress  ( static_cast<const ServiceItem &>(addrProxy), addrProxy.getRoleName(), "" )
    , mConnectStatus( addrProxy.isValid() ? NEService::ServicePending : NEService::ServiceConnectionUnknown )
{
    ; // do nothing
}

ServiceStub::ServiceStub( const ServiceStub & stubService )
    : mStubAddress  ( stubService.mStubAddress )
    , mConnectStatus( stubService.mConnectStatus )
{
    ; // do nothing
}

ServiceStub::~ServiceStub( void )
{
    ; // do nothing
}

const ServiceStub & ServiceStub::operator = ( const StubAddress & addrStub )
{
    mStubAddress    = addrStub;
    mConnectStatus  = addrStub.isValid() ? NEService::ServicePending : NEService::ServiceConnectionUnknown;
    return (*this);
}

const ServiceStub & ServiceStub::operator = ( const ProxyAddress & addrProxy )
{
    mStubAddress    = static_cast<const ServiceAddress &>(addrProxy);
    mConnectStatus  = addrProxy.isValid() ? NEService::ServicePending : NEService::ServiceConnectionUnknown;
    return (*this);
}

const ServiceStub & ServiceStub::operator = ( const ServiceStub & stubService )
{
    mStubAddress    = stubService.mStubAddress;
    mConnectStatus  = stubService.mConnectStatus;
    return (*this);
}

bool ServiceStub::operator == ( const StubAddress & addrStub ) const
{
    return static_cast<const ServiceAddress &>(mStubAddress) == static_cast<const ServiceAddress &>(addrStub);
}

bool ServiceStub::operator == (const ProxyAddress & addrProxy) const
{
    return static_cast<const ServiceAddress &>(mStubAddress) == static_cast<const ServiceAddress &>(addrProxy);
}

bool ServiceStub::operator == ( const ServiceStub & stubService ) const
{
    return static_cast<const ServiceAddress &>(mStubAddress) == static_cast<const ServiceAddress &>(stubService.mStubAddress);
}

ServiceStub::operator unsigned int ( void ) const
{
    const ServiceAddress & addrService = static_cast<const ServiceAddress &>(mStubAddress);
    return static_cast<unsigned int>(addrService);
}

bool ServiceStub::isValid( void ) const
{
    return mStubAddress.isValid() && (mStubAddress.getSource() != NEService::SOURCE_UNKNOWN) && (mStubAddress.getCookie() != NEService::COOKIE_UNKNOWN);
}

void ServiceStub::setService( const StubAddress & addrStub, NEService::eServiceConnection connectStatus /*= NEService::ServiceConnected */ )
{
    mStubAddress    = addrStub;
    setServiceStatus( connectStatus );
}

void ServiceStub::setServiceStatus( NEService::eServiceConnection newStatus )
{
    mConnectStatus  = NEService::ServiceConnectionUnknown;
    if ( mStubAddress.isValid() )
    {
        mConnectStatus = newStatus;
        if ( newStatus == NEService::ServiceConnected )
            mConnectStatus = mStubAddress.getSource() != NEService::SOURCE_UNKNOWN ? NEService::ServiceConnected : NEService::ServicePending;
        else
            mStubAddress.setSource( NEService::SOURCE_UNKNOWN );
    }
}
