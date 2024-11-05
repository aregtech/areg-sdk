/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        mcrouter/service/private/ServiceStub.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform Stub Service object
 ************************************************************************/
#include "mcrouter/service/private/ServiceStub.hpp"
#include "areg/component/ProxyAddress.hpp"

#include <utility>

ServiceStub::ServiceStub( void )
    : mStubAddress  ( )
    , mConnectStatus( NEService::eServiceConnection::ServiceConnectionUnknown )
{
}

ServiceStub::ServiceStub( const StubAddress & addrStub )
    : mStubAddress  ( addrStub )
    , mConnectStatus( addrStub.isValid() ? NEService::eServiceConnection::ServicePending : NEService::eServiceConnection::ServiceConnectionUnknown )
{
}

ServiceStub::ServiceStub( StubAddress && addrStub ) noexcept
    : mStubAddress  ( std::move(addrStub) )
    , mConnectStatus(mStubAddress.isValid( ) ? NEService::eServiceConnection::ServicePending : NEService::eServiceConnection::ServiceConnectionUnknown )
{
}

ServiceStub::ServiceStub( const ProxyAddress & addrProxy )
    : mStubAddress  ( static_cast<const ServiceItem &>(addrProxy), addrProxy.getRoleName(), "" )
    , mConnectStatus( addrProxy.isValid() ? NEService::eServiceConnection::ServicePending : NEService::eServiceConnection::ServiceConnectionUnknown )
{
}

ServiceStub::ServiceStub( const ServiceStub & stubService )
    : mStubAddress  ( stubService.mStubAddress )
    , mConnectStatus( stubService.mConnectStatus )
{
}

ServiceStub::ServiceStub( ServiceStub && stubService ) noexcept
    : mStubAddress  ( std::move(stubService.mStubAddress) )
    , mConnectStatus( stubService.mConnectStatus )
{
}

ServiceStub::ServiceStub(const ServiceAddress& addrService)
    : mStubAddress  (addrService)
    , mConnectStatus(NEService::eServiceConnection::ServiceConnectionUnknown)
{
}

ServiceStub::ServiceStub( ServiceAddress && addrService)
    : mStubAddress  (std::move(addrService))
    , mConnectStatus(NEService::eServiceConnection::ServiceConnectionUnknown)
{
}

ServiceStub & ServiceStub::operator = ( const StubAddress & addrStub )
{
    mStubAddress    = addrStub;
    mConnectStatus  = addrStub.isValid() ? NEService::eServiceConnection::ServicePending : NEService::eServiceConnection::ServiceConnectionUnknown;

    return (*this);
}

ServiceStub & ServiceStub::operator = ( StubAddress && addrStub ) noexcept
{
    mStubAddress    = std::move(addrStub);
    mConnectStatus  = mStubAddress.isValid( ) ? NEService::eServiceConnection::ServicePending : NEService::eServiceConnection::ServiceConnectionUnknown;
    
    return (*this);
}

ServiceStub & ServiceStub::operator = ( const ProxyAddress & addrProxy )
{
    mStubAddress    = static_cast<const ServiceAddress &>(addrProxy);
    mConnectStatus  = addrProxy.isValid() ? NEService::eServiceConnection::ServicePending : NEService::eServiceConnection::ServiceConnectionUnknown;

    return (*this);
}

ServiceStub & ServiceStub::operator = ( const ServiceStub & stubService )
{
    mStubAddress    = stubService.mStubAddress;
    mConnectStatus  = stubService.mConnectStatus;
    
    return (*this);
}

ServiceStub & ServiceStub::operator = ( ServiceStub && stubService ) noexcept
{
    mStubAddress    = std::move(stubService.mStubAddress);
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

void ServiceStub::setService( const StubAddress & addrStub, NEService::eServiceConnection connectStatus /*= NEService::eServiceConnection::ServiceConnected */ )
{
    mStubAddress    = addrStub;
    setServiceStatus( connectStatus );
}

void ServiceStub::setServiceStatus( NEService::eServiceConnection newStatus )
{
    mConnectStatus  = NEService::eServiceConnection::ServiceConnectionUnknown;
    if ( mStubAddress.isValid() )
    {
        mConnectStatus = newStatus;
        if ( newStatus == NEService::eServiceConnection::ServiceConnected )
            mConnectStatus = mStubAddress.getSource() != NEService::SOURCE_UNKNOWN ? NEService::eServiceConnection::ServiceConnected : NEService::eServiceConnection::ServicePending;
        else
            mStubAddress.setSource( NEService::SOURCE_UNKNOWN );
    }
}
