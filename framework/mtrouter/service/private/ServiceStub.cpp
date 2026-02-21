/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        mtrouter/service/private/ServiceStub.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform Stub Service object
 ************************************************************************/
#include "mtrouter/service/private/ServiceStub.hpp"
#include "areg/component/ProxyAddress.hpp"

#include <utility>

ServiceStub::ServiceStub()
    : mStubAddress  ( )
    , mConnectStatus( NEService::ServiceConnectionState::Unknown )
{
}

ServiceStub::ServiceStub( const StubAddress & addrStub )
    : mStubAddress  ( addrStub )
    , mConnectStatus( addrStub.isValid() ? NEService::ServiceConnectionState::Pending : NEService::ServiceConnectionState::Unknown )
{
}

ServiceStub::ServiceStub( StubAddress && addrStub ) noexcept
    : mStubAddress  ( std::move(addrStub) )
    , mConnectStatus(mStubAddress.isValid( ) ? NEService::ServiceConnectionState::Pending : NEService::ServiceConnectionState::Unknown )
{
}

ServiceStub::ServiceStub( const ProxyAddress & addrProxy )
    : mStubAddress  ( static_cast<const ServiceItem &>(addrProxy), addrProxy.getRoleName(), "" )
    , mConnectStatus( addrProxy.isValid() ? NEService::ServiceConnectionState::Pending : NEService::ServiceConnectionState::Unknown )
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
    , mConnectStatus(NEService::ServiceConnectionState::Unknown)
{
}

ServiceStub::ServiceStub( ServiceAddress && addrService)
    : mStubAddress  (std::move(addrService))
    , mConnectStatus(NEService::ServiceConnectionState::Unknown)
{
}

ServiceStub & ServiceStub::operator = ( const StubAddress & addrStub )
{
    mStubAddress    = addrStub;
    mConnectStatus  = addrStub.isValid() ? NEService::ServiceConnectionState::Pending : NEService::ServiceConnectionState::Unknown;

    return (*this);
}

ServiceStub & ServiceStub::operator = ( StubAddress && addrStub ) noexcept
{
    mStubAddress    = std::move(addrStub);
    mConnectStatus  = mStubAddress.isValid( ) ? NEService::ServiceConnectionState::Pending : NEService::ServiceConnectionState::Unknown;
    
    return (*this);
}

ServiceStub & ServiceStub::operator = ( const ProxyAddress & addrProxy )
{
    mStubAddress    = static_cast<const ServiceAddress &>(addrProxy);
    mConnectStatus  = addrProxy.isValid() ? NEService::ServiceConnectionState::Pending : NEService::ServiceConnectionState::Unknown;

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

ServiceStub::operator uint32_t () const
{
    const ServiceAddress & addrService = static_cast<const ServiceAddress &>(mStubAddress);
    return static_cast<uint32_t>(addrService);
}

bool ServiceStub::isValid() const
{
    return mStubAddress.isValid() && (mStubAddress.getSource() != NEService::SOURCE_UNKNOWN) && (mStubAddress.getCookie() != NEService::COOKIE_UNKNOWN);
}

void ServiceStub::setService( const StubAddress & addrStub, NEService::ServiceConnectionState connectStatus /*= NEService::ServiceConnectionState::Connected */ )
{
    mStubAddress    = addrStub;
    setServiceStatus( connectStatus );
}

void ServiceStub::setServiceStatus( NEService::ServiceConnectionState newStatus )
{
    mConnectStatus  = NEService::ServiceConnectionState::Unknown;
    if ( mStubAddress.isValid() )
    {
        mConnectStatus = newStatus;
        if ( newStatus == NEService::ServiceConnectionState::Connected )
            mConnectStatus = mStubAddress.getSource() != NEService::SOURCE_UNKNOWN ? NEService::ServiceConnectionState::Connected : NEService::ServiceConnectionState::Pending;
        else
            mStubAddress.setSource( NEService::SOURCE_UNKNOWN );
    }
}
