/************************************************************************
 * \file        areg/src/ipc/private/CEServiceProxy.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Proxy Service object
 ************************************************************************/
#include "areg/src/ipc/private/CEServiceProxy.hpp"
#include "areg/src/component/CEStubAddress.hpp"

CEServiceProxy::CEServiceProxy( void )
    : mProxyAddress ( )
    , mConnectStatus( NEService::ServiceConnectionUnknown )
{
    ; // do nothing
}

CEServiceProxy::CEServiceProxy( const CEProxyAddress & addrProxy )
    : mProxyAddress ( addrProxy )
    , mConnectStatus( addrProxy.IsValid() ? NEService::ServicePending : NEService::ServiceConnectionUnknown )
{
    ; // do nothing
}

CEServiceProxy::CEServiceProxy( const CEServiceProxy & serviceProxy )
    : mProxyAddress ( serviceProxy.mProxyAddress )
    , mConnectStatus( serviceProxy.mConnectStatus )
{
    ; // do nothing
}

CEServiceProxy::~CEServiceProxy(void)
{
    ; // do nothing
}

const CEServiceProxy & CEServiceProxy::operator = ( const CEServiceProxy & serviceProxy )
{
    mProxyAddress   = serviceProxy.mProxyAddress;
    mConnectStatus  = serviceProxy.mConnectStatus;
    return (*this);
}

const CEServiceProxy & CEServiceProxy::operator = ( const CEProxyAddress & addrProxy )
{
    mProxyAddress   = addrProxy;
    mConnectStatus  = addrProxy.IsValid() ? NEService::ServicePending : NEService::ServiceConnectionUnknown;
    return (*this);
}

bool CEServiceProxy::operator == ( const CEServiceProxy & serviceProxy ) const
{
    return operator == (serviceProxy.mProxyAddress);
}

bool CEServiceProxy::operator == ( const CEProxyAddress & addrProxy ) const
{
    return  (static_cast<const CEServiceAddress &>(mProxyAddress) == static_cast<const CEServiceAddress &>(addrProxy))  && 
            (mProxyAddress.GetThread() == addrProxy.GetThread()) && 
            (mProxyAddress.GetCookie() == addrProxy.GetCookie());
}

bool CEServiceProxy::operator == ( const CEStubAddress & addrStub ) const
{
    return static_cast<const CEServiceAddress &>(mProxyAddress) == static_cast<const CEServiceAddress &>(addrStub);
}

void CEServiceProxy::_setServiceStatus( NEService::eServiceConnection newStatus )
{
    mConnectStatus = NEService::ServiceConnectionUnknown;
    if ( mProxyAddress.IsValid() )
    {
        mConnectStatus = newStatus;
        if ( newStatus == NEService::ServiceConnected )
            mConnectStatus = mProxyAddress.GetTarget() != NEService::TARGET_UNKNOWN ? NEService::ServiceConnected : NEService::ServicePending;
    }
}

void CEServiceProxy::_setService( const CEProxyAddress & addrProxy, NEService::eServiceConnection connectStatus /*= NEService::ServicePending*/ )
{
    mProxyAddress   = addrProxy;
    _setServiceStatus(connectStatus);
}

bool CEServiceProxy::StubAvailable( const CEStubAddress & addrStub )
{
    mConnectStatus = NEService::ServiceConnectionUnknown;
    if ( mProxyAddress.IsValid() )
    {
        if ( addrStub.IsValid() )
        {
            mProxyAddress.SetTarget( addrStub.GetSource() );
            mConnectStatus = NEService::ServiceConnected;
        }
        else
        {
            mConnectStatus = NEService::ServicePending;
        }
    }
    return IsConnected();
}

bool CEServiceProxy::StubUnavailable(void)
{
    mConnectStatus = NEService::ServiceConnectionUnknown;
    if ( mProxyAddress.IsValid() )
    {
        mProxyAddress.SetTarget( NEService::TARGET_UNKNOWN );
        mConnectStatus = NEService::ServicePending;
    }
    return IsWaiting();
}
