/************************************************************************
 * \file        areg/ipc/private/ServiceProxy.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Proxy Service object
 ************************************************************************/
#include "areg/ipc/private/ServiceProxy.hpp"
#include "areg/component/StubAddress.hpp"

ServiceProxy::ServiceProxy( void )
    : mProxyAddress ( )
    , mConnectStatus( NEService::ServiceConnectionUnknown )
{
    ; // do nothing
}

ServiceProxy::ServiceProxy( const ProxyAddress & addrProxy )
    : mProxyAddress ( addrProxy )
    , mConnectStatus( addrProxy.isValid() ? NEService::ServicePending : NEService::ServiceConnectionUnknown )
{
    ; // do nothing
}

ServiceProxy::ServiceProxy( const ServiceProxy & serviceProxy )
    : mProxyAddress ( serviceProxy.mProxyAddress )
    , mConnectStatus( serviceProxy.mConnectStatus )
{
    ; // do nothing
}

ServiceProxy::~ServiceProxy(void)
{
    ; // do nothing
}

const ServiceProxy & ServiceProxy::operator = ( const ServiceProxy & serviceProxy )
{
    mProxyAddress   = serviceProxy.mProxyAddress;
    mConnectStatus  = serviceProxy.mConnectStatus;
    return (*this);
}

const ServiceProxy & ServiceProxy::operator = ( const ProxyAddress & addrProxy )
{
    mProxyAddress   = addrProxy;
    mConnectStatus  = addrProxy.isValid() ? NEService::ServicePending : NEService::ServiceConnectionUnknown;
    return (*this);
}

bool ServiceProxy::operator == ( const ServiceProxy & serviceProxy ) const
{
    return operator == (serviceProxy.mProxyAddress);
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
    mConnectStatus = NEService::ServiceConnectionUnknown;
    if ( mProxyAddress.isValid() )
    {
        mConnectStatus = newStatus;
        if ( newStatus == NEService::ServiceConnected )
            mConnectStatus = mProxyAddress.getTarget() != NEService::TARGET_UNKNOWN ? NEService::ServiceConnected : NEService::ServicePending;
    }
}

void ServiceProxy::_setService( const ProxyAddress & addrProxy, NEService::eServiceConnection connectStatus /*= NEService::ServicePending*/ )
{
    mProxyAddress   = addrProxy;
    _setServiceStatus(connectStatus);
}

bool ServiceProxy::stubAvailable( const StubAddress & addrStub )
{
    mConnectStatus = NEService::ServiceConnectionUnknown;
    if ( mProxyAddress.isValid() )
    {
        if ( addrStub.isValid() )
        {
            mProxyAddress.setTarget( addrStub.getSource() );
            mConnectStatus = NEService::ServiceConnected;
        }
        else
        {
            mConnectStatus = NEService::ServicePending;
        }
    }
    return isConnected();
}

bool ServiceProxy::stubUnavailable(void)
{
    mConnectStatus = NEService::ServiceConnectionUnknown;
    if ( mProxyAddress.isValid() )
    {
        mProxyAddress.setTarget( NEService::TARGET_UNKNOWN );
        mConnectStatus = NEService::ServicePending;
    }
    return isWaiting();
}
