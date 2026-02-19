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
    , mConnectStatus( NEService::ServiceConnectionState::Unknown )
{
}

ServiceProxy::ServiceProxy( const ProxyAddress & addrProxy )
    : mProxyAddress ( addrProxy )
    , mConnectStatus( addrProxy.isValid() ? NEService::ServiceConnectionState::Pending : NEService::ServiceConnectionState::Unknown )
{
}

ServiceProxy::ServiceProxy( ProxyAddress && addrProxy ) noexcept
    : mProxyAddress ( std::move(addrProxy) )
    , mConnectStatus( mProxyAddress.isValid( ) ? NEService::ServiceConnectionState::Pending : NEService::ServiceConnectionState::Unknown )
{
}

ServiceProxy::ServiceProxy(const StubAddress & addrStub)
    : mProxyAddress (static_cast<const ServiceAddress&>(addrStub))
    , mConnectStatus(NEService::ServiceConnectionState::Unknown)
{
}

ServiceProxy::ServiceProxy( StubAddress && addrStub) noexcept
    : mProxyAddress (std::move(addrStub))
    , mConnectStatus(NEService::ServiceConnectionState::Unknown)
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

ServiceProxy & ServiceProxy::operator = ( const ProxyAddress & addrProxy )
{
    mProxyAddress   = addrProxy;
    mConnectStatus  = addrProxy.isValid() ? NEService::ServiceConnectionState::Pending : NEService::ServiceConnectionState::Unknown;
    
    return (*this);
}

ServiceProxy & ServiceProxy::operator = ( ProxyAddress && addrProxy ) noexcept
{
    mProxyAddress   = std::move(addrProxy);
    mConnectStatus  = mProxyAddress.isValid( ) ? NEService::ServiceConnectionState::Pending : NEService::ServiceConnectionState::Unknown;
    
    return (*this);
}

bool ServiceProxy::operator == ( const ServiceProxy & serviceProxy ) const
{
    return ServiceProxy::operator == (serviceProxy.mProxyAddress);
}

bool ServiceProxy::operator == ( const ProxyAddress & addrProxy ) const
{
    return  (static_cast<const ServiceAddress &>(mProxyAddress) == static_cast<const ServiceAddress &>(addrProxy))  && 
            (mProxyAddress.getThread() == addrProxy.getThread()) && 
            (mProxyAddress.getCookie() == addrProxy.getCookie());
}

bool ServiceProxy::operator == ( const StubAddress & addrStub ) const
{
    return static_cast<const ServiceAddress &>(mProxyAddress) == static_cast<const ServiceAddress &>(addrStub);
}

void ServiceProxy::_setServiceStatus( NEService::ServiceConnectionState newStatus )
{
    mConnectStatus = NEService::ServiceConnectionState::Unknown;
    if ( mProxyAddress.isValid() )
    {
        mConnectStatus = newStatus;
        if ( newStatus == NEService::ServiceConnectionState::Connected )
        {
            mConnectStatus = mProxyAddress.getTarget( ) != NEService::TARGET_UNKNOWN ? NEService::ServiceConnectionState::Connected : NEService::ServiceConnectionState::Pending;
        }
    }
}

void ServiceProxy::_setService( const ProxyAddress & addrProxy, NEService::ServiceConnectionState connectStatus /*= NEService::ServiceConnectionState::Pending*/ )
{
    mProxyAddress   = addrProxy;
    _setServiceStatus(connectStatus);
}

bool ServiceProxy::stubAvailable( const StubAddress & addrStub )
{
    mConnectStatus = NEService::ServiceConnectionState::Unknown;
    if ( mProxyAddress.isValid() )
    {
        if ( addrStub.isValid() )
        {
            mProxyAddress.setTarget( addrStub.getSource() );
            mConnectStatus = NEService::ServiceConnectionState::Connected;
        }
        else
        {
            mConnectStatus = NEService::ServiceConnectionState::Pending;
        }
    }
    return isConnected();
}

bool ServiceProxy::stubUnavailable()
{
    mConnectStatus = NEService::ServiceConnectionState::Unknown;
    if ( mProxyAddress.isValid() )
    {
        mProxyAddress.setTarget( NEService::TARGET_UNKNOWN );
        mConnectStatus = NEService::ServiceConnectionState::Pending;
    }
    return isWaiting();
}
