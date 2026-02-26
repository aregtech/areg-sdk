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
    , mConnectStatus( areg::ServiceConnectionState::Unknown )
{
}

ServiceStub::ServiceStub( const areg::StubAddress & addrStub )
    : mStubAddress  ( addrStub )
    , mConnectStatus( addrStub.isValid() ? areg::ServiceConnectionState::Pending : areg::ServiceConnectionState::Unknown )
{
}

ServiceStub::ServiceStub( areg::StubAddress && addrStub ) noexcept
    : mStubAddress  ( std::move(addrStub) )
    , mConnectStatus(mStubAddress.isValid( ) ? areg::ServiceConnectionState::Pending : areg::ServiceConnectionState::Unknown )
{
}

ServiceStub::ServiceStub( const areg::ProxyAddress & addrProxy )
    : mStubAddress  ( static_cast<const areg::ServiceItem &>(addrProxy), addrProxy.getRoleName(), "" )
    , mConnectStatus( addrProxy.isValid() ? areg::ServiceConnectionState::Pending : areg::ServiceConnectionState::Unknown )
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

ServiceStub::ServiceStub(const areg::ServiceAddress& addrService)
    : mStubAddress  (addrService)
    , mConnectStatus(areg::ServiceConnectionState::Unknown)
{
}

ServiceStub::ServiceStub( areg::ServiceAddress && addrService)
    : mStubAddress  (std::move(addrService))
    , mConnectStatus(areg::ServiceConnectionState::Unknown)
{
}

ServiceStub & ServiceStub::operator = ( const areg::StubAddress & addrStub )
{
    mStubAddress    = addrStub;
    mConnectStatus  = addrStub.isValid() ? areg::ServiceConnectionState::Pending : areg::ServiceConnectionState::Unknown;

    return (*this);
}

ServiceStub & ServiceStub::operator = ( areg::StubAddress && addrStub ) noexcept
{
    mStubAddress    = std::move(addrStub);
    mConnectStatus  = mStubAddress.isValid( ) ? areg::ServiceConnectionState::Pending : areg::ServiceConnectionState::Unknown;
    
    return (*this);
}

ServiceStub & ServiceStub::operator = ( const areg::ProxyAddress & addrProxy )
{
    mStubAddress    = static_cast<const areg::ServiceAddress &>(addrProxy);
    mConnectStatus  = addrProxy.isValid() ? areg::ServiceConnectionState::Pending : areg::ServiceConnectionState::Unknown;

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

bool ServiceStub::operator == ( const areg::StubAddress & addrStub ) const
{
    return static_cast<const areg::ServiceAddress &>(mStubAddress) == static_cast<const areg::ServiceAddress &>(addrStub);
}

bool ServiceStub::operator == (const areg::ProxyAddress & addrProxy) const
{
    return static_cast<const areg::ServiceAddress &>(mStubAddress) == static_cast<const areg::ServiceAddress &>(addrProxy);
}

bool ServiceStub::operator == ( const ServiceStub & stubService ) const
{
    return static_cast<const areg::ServiceAddress &>(mStubAddress) == static_cast<const areg::ServiceAddress &>(stubService.mStubAddress);
}

ServiceStub::operator uint32_t () const
{
    const areg::ServiceAddress & addrService = static_cast<const areg::ServiceAddress &>(mStubAddress);
    return static_cast<uint32_t>(addrService);
}

bool ServiceStub::isValid() const
{
    return mStubAddress.isValid() && (mStubAddress.getSource() != areg::SOURCE_UNKNOWN) && (mStubAddress.getCookie() != areg::COOKIE_UNKNOWN);
}

void ServiceStub::setService( const areg::StubAddress & addrStub, areg::ServiceConnectionState connectStatus /*= areg::ServiceConnectionState::Connected */ )
{
    mStubAddress    = addrStub;
    setServiceStatus( connectStatus );
}

void ServiceStub::setServiceStatus( areg::ServiceConnectionState newStatus )
{
    mConnectStatus  = areg::ServiceConnectionState::Unknown;
    if ( mStubAddress.isValid() )
    {
        mConnectStatus = newStatus;
        if ( newStatus == areg::ServiceConnectionState::Connected )
            mConnectStatus = mStubAddress.getSource() != areg::SOURCE_UNKNOWN ? areg::ServiceConnectionState::Connected : areg::ServiceConnectionState::Pending;
        else
            mStubAddress.setSource( areg::SOURCE_UNKNOWN );
    }
}
