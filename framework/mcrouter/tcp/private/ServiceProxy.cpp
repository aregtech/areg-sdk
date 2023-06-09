/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        mcrouter/tcp/private/ServiceProxy.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform Proxy Service object
 ************************************************************************/
#include "mcrouter/tcp/private/ServiceProxy.hpp"
#include "areg/component/StubAddress.hpp"

#include <utility>

ServiceProxy::ServiceProxy( void )
    : mProxyAddress ( )
    , mConnectStatus( NEService::eServiceConnection::ServiceConnectionUnknown )
{
}

ServiceProxy::ServiceProxy( const ProxyAddress & addrProxy )
    : mProxyAddress ( addrProxy )
    , mConnectStatus( addrProxy.isValid() ? NEService::eServiceConnection::ServicePending : NEService::eServiceConnection::ServiceConnectionUnknown )
{
}

ServiceProxy::ServiceProxy( ProxyAddress && addrProxy ) noexcept
    : mProxyAddress ( std::move(addrProxy) )
    , mConnectStatus( mProxyAddress.isValid( ) ? NEService::eServiceConnection::ServicePending : NEService::eServiceConnection::ServiceConnectionUnknown )
{
}

ServiceProxy::ServiceProxy(const StubAddress & addrStub)
    : mProxyAddress (static_cast<const ServiceAddress&>(addrStub))
    , mConnectStatus(NEService::eServiceConnection::ServiceConnectionUnknown)
{
}

ServiceProxy::ServiceProxy( StubAddress && addrStub) noexcept
    : mProxyAddress (std::move(addrStub))
    , mConnectStatus(NEService::eServiceConnection::ServiceConnectionUnknown)
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
    mConnectStatus  = addrProxy.isValid() ? NEService::eServiceConnection::ServicePending : NEService::eServiceConnection::ServiceConnectionUnknown;
    
    return (*this);
}

ServiceProxy & ServiceProxy::operator = ( ProxyAddress && addrProxy ) noexcept
{
    mProxyAddress   = std::move(addrProxy);
    mConnectStatus  = mProxyAddress.isValid( ) ? NEService::eServiceConnection::ServicePending : NEService::eServiceConnection::ServiceConnectionUnknown;
    
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

void ServiceProxy::_setServiceStatus( NEService::eServiceConnection newStatus )
{
    mConnectStatus = NEService::eServiceConnection::ServiceConnectionUnknown;
    if ( mProxyAddress.isValid() )
    {
        mConnectStatus = newStatus;
        if ( newStatus == NEService::eServiceConnection::ServiceConnected )
        {
            mConnectStatus = mProxyAddress.getTarget( ) != NEService::TARGET_UNKNOWN ? NEService::eServiceConnection::ServiceConnected : NEService::eServiceConnection::ServicePending;
        }
    }
}

void ServiceProxy::_setService( const ProxyAddress & addrProxy, NEService::eServiceConnection connectStatus /*= NEService::eServiceConnection::ServicePending*/ )
{
    mProxyAddress   = addrProxy;
    _setServiceStatus(connectStatus);
}

bool ServiceProxy::stubAvailable( const StubAddress & addrStub )
{
    mConnectStatus = NEService::eServiceConnection::ServiceConnectionUnknown;
    if ( mProxyAddress.isValid() )
    {
        if ( addrStub.isValid() )
        {
            mProxyAddress.setTarget( addrStub.getSource() );
            mConnectStatus = NEService::eServiceConnection::ServiceConnected;
        }
        else
        {
            mConnectStatus = NEService::eServiceConnection::ServicePending;
        }
    }
    return isConnected();
}

bool ServiceProxy::stubUnavailable(void)
{
    mConnectStatus = NEService::eServiceConnection::ServiceConnectionUnknown;
    if ( mProxyAddress.isValid() )
    {
        mProxyAddress.setTarget( NEService::TARGET_UNKNOWN );
        mConnectStatus = NEService::eServiceConnection::ServicePending;
    }
    return isWaiting();
}
