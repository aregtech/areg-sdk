/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        mtrouter/service/private/ServiceProxy.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform Proxy Service object
 ************************************************************************/
#include "mtrouter/service/private/ServiceProxy.hpp"
#include "areg/component/StubAddress.hpp"

#include <utility>

ServiceProxy::ServiceProxy()
    : mProxyAddress ( )
    , mConnectStatus( areg::ServiceConnectionState::Unknown )
{
}

ServiceProxy::ServiceProxy( const areg::ProxyAddress & addrProxy )
    : mProxyAddress ( addrProxy )
    , mConnectStatus( addrProxy.isValid() ? areg::ServiceConnectionState::Pending : areg::ServiceConnectionState::Unknown )
{
}

ServiceProxy::ServiceProxy( areg::ProxyAddress && addrProxy ) noexcept
    : mProxyAddress ( std::move(addrProxy) )
    , mConnectStatus( mProxyAddress.isValid( ) ? areg::ServiceConnectionState::Pending : areg::ServiceConnectionState::Unknown )
{
}

ServiceProxy::ServiceProxy(const areg::StubAddress & addrStub)
    : mProxyAddress (static_cast<const areg::ServiceAddress&>(addrStub))
    , mConnectStatus(areg::ServiceConnectionState::Unknown)
{
}

ServiceProxy::ServiceProxy( areg::StubAddress && addrStub) noexcept
    : mProxyAddress (std::move(addrStub))
    , mConnectStatus(areg::ServiceConnectionState::Unknown)
{
}

ServiceProxy::ServiceProxy( const ServiceProxy & serviceProxy )
    : mProxyAddress ( serviceProxy.mProxyAddress )
    , mConnectStatus( serviceProxy.mConnectStatus )
{
}

ServiceProxy::ServiceProxy( ServiceProxy && serviceProxy ) noexcept
    : mProxyAddress ( std::move(serviceProxy.mProxyAddress) )
    , mConnectStatus( serviceProxy.mConnectStatus )
{
}

ServiceProxy & ServiceProxy::operator = ( const ServiceProxy & serviceProxy )
{
    mProxyAddress   = serviceProxy.mProxyAddress;
    mConnectStatus  = serviceProxy.mConnectStatus;
    
    return (*this);
}

ServiceProxy & ServiceProxy::operator = ( ServiceProxy && serviceProxy ) noexcept
{
    mProxyAddress   = std::move(serviceProxy.mProxyAddress);
    mConnectStatus  = serviceProxy.mConnectStatus;
    
    return (*this);
}

ServiceProxy & ServiceProxy::operator = ( const areg::ProxyAddress & addrProxy )
{
    mProxyAddress   = addrProxy;
    mConnectStatus  = addrProxy.isValid() ? areg::ServiceConnectionState::Pending : areg::ServiceConnectionState::Unknown;
    
    return (*this);
}

ServiceProxy & ServiceProxy::operator = ( areg::ProxyAddress && addrProxy ) noexcept
{
    mProxyAddress   = std::move(addrProxy);
    mConnectStatus  = mProxyAddress.isValid( ) ? areg::ServiceConnectionState::Pending : areg::ServiceConnectionState::Unknown;
    
    return (*this);
}

bool ServiceProxy::operator == ( const ServiceProxy & serviceProxy ) const
{
    return ServiceProxy::operator == (serviceProxy.mProxyAddress);
}

bool ServiceProxy::operator == ( const areg::ProxyAddress & addrProxy ) const
{
    return  (static_cast<const areg::ServiceAddress &>(mProxyAddress) == static_cast<const areg::ServiceAddress &>(addrProxy))  && 
            (mProxyAddress.getThread() == addrProxy.getThread()) && 
            (mProxyAddress.getCookie() == addrProxy.getCookie());
}

bool ServiceProxy::operator == ( const areg::StubAddress & addrStub ) const
{
    return static_cast<const areg::ServiceAddress &>(mProxyAddress) == static_cast<const areg::ServiceAddress &>(addrStub);
}

void ServiceProxy::_setServiceStatus( areg::ServiceConnectionState newStatus )
{
    mConnectStatus = areg::ServiceConnectionState::Unknown;
    if ( mProxyAddress.isValid() )
    {
        mConnectStatus = newStatus;
        if ( newStatus == areg::ServiceConnectionState::Connected )
        {
            mConnectStatus = mProxyAddress.getTarget( ) != areg::TARGET_UNKNOWN ? areg::ServiceConnectionState::Connected : areg::ServiceConnectionState::Pending;
        }
    }
}

void ServiceProxy::_setService( const areg::ProxyAddress & addrProxy, areg::ServiceConnectionState connectStatus /*= areg::ServiceConnectionState::Pending*/ )
{
    mProxyAddress   = addrProxy;
    _setServiceStatus(connectStatus);
}

bool ServiceProxy::stubAvailable( const areg::StubAddress & addrStub )
{
    mConnectStatus = areg::ServiceConnectionState::Unknown;
    if ( mProxyAddress.isValid() )
    {
        if ( addrStub.isValid() )
        {
            mProxyAddress.setTarget( addrStub.getSource() );
            mConnectStatus = areg::ServiceConnectionState::Connected;
        }
        else
        {
            mConnectStatus = areg::ServiceConnectionState::Pending;
        }
    }
    return isConnected();
}

bool ServiceProxy::stubUnavailable()
{
    mConnectStatus = areg::ServiceConnectionState::Unknown;
    if ( mProxyAddress.isValid() )
    {
        mProxyAddress.setTarget( areg::TARGET_UNKNOWN );
        mConnectStatus = areg::ServiceConnectionState::Pending;
    }
    return isWaiting();
}
