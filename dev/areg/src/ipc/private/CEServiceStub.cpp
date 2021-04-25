/************************************************************************
 * \file        areg/src/ipc/private/CEServiceStub.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Stub Service object
 ************************************************************************/
#include "areg/src/ipc/private/CEServiceStub.hpp"
#include "areg/src/component/CEProxyAddress.hpp"

CEServiceStub::CEServiceStub( void )
    : mStubAddress  ( )
    , mConnectStatus( NEService::ServiceConnectionUnknown )
{
    ; // do nothing
}

CEServiceStub::CEServiceStub( const CEStubAddress & addrStub )
    : mStubAddress  ( addrStub )
    , mConnectStatus( addrStub.IsValid() ? NEService::ServicePending : NEService::ServiceConnectionUnknown )
{
    ; // do nothing
}

CEServiceStub::CEServiceStub( const CEProxyAddress & addrProxy )
    : mStubAddress  ( static_cast<const CEServiceItem &>(addrProxy), addrProxy.GetRoleName(), "" )
    , mConnectStatus( addrProxy.IsValid() ? NEService::ServicePending : NEService::ServiceConnectionUnknown )
{
    ; // do nothing
}

CEServiceStub::CEServiceStub( const CEServiceStub & stubService )
    : mStubAddress  ( stubService.mStubAddress )
    , mConnectStatus( stubService.mConnectStatus )
{
    ; // do nothing
}

CEServiceStub::~CEServiceStub( void )
{
    ; // do nothing
}

const CEServiceStub & CEServiceStub::operator = ( const CEStubAddress & addrStub )
{
    mStubAddress    = addrStub;
    mConnectStatus  = addrStub.IsValid() ? NEService::ServicePending : NEService::ServiceConnectionUnknown;
    return (*this);
}

const CEServiceStub & CEServiceStub::operator = ( const CEProxyAddress & addrProxy )
{
    mStubAddress    = static_cast<const CEServiceAddress &>(addrProxy);
    mConnectStatus  = addrProxy.IsValid() ? NEService::ServicePending : NEService::ServiceConnectionUnknown;
    return (*this);
}

const CEServiceStub & CEServiceStub::operator = ( const CEServiceStub & stubService )
{
    mStubAddress    = stubService.mStubAddress;
    mConnectStatus  = stubService.mConnectStatus;
    return (*this);
}

bool CEServiceStub::operator == ( const CEStubAddress & addrStub ) const
{
    return static_cast<const CEServiceAddress &>(mStubAddress) == static_cast<const CEServiceAddress &>(addrStub);
}

bool CEServiceStub::operator == (const CEProxyAddress & addrProxy) const
{
    return static_cast<const CEServiceAddress &>(mStubAddress) == static_cast<const CEServiceAddress &>(addrProxy);
}

bool CEServiceStub::operator == ( const CEServiceStub & stubService ) const
{
    return static_cast<const CEServiceAddress &>(mStubAddress) == static_cast<const CEServiceAddress &>(stubService.mStubAddress);
}

CEServiceStub::operator unsigned int ( void ) const
{
    return static_cast<unsigned int>(mStubAddress);
}

bool CEServiceStub::IsValid( void ) const
{
    return mStubAddress.IsValid() && (mStubAddress.GetSource() != NEService::SOURCE_UNKNOWN) && (mStubAddress.GetCookie() != NEService::COOKIE_UNKNOWN);
}

void CEServiceStub::SetService( const CEStubAddress & addrStub, NEService::eServiceConnection connectStatus /*= NEService::ServiceConnected */ )
{
    mStubAddress    = addrStub;
    SetServiceStatus( connectStatus );
}

void CEServiceStub::SetServiceStatus( NEService::eServiceConnection newStatus )
{
    mConnectStatus  = NEService::ServiceConnectionUnknown;
    if ( mStubAddress.IsValid() )
    {
        mConnectStatus = newStatus;
        if ( newStatus == NEService::ServiceConnected )
            mConnectStatus = mStubAddress.GetSource() != NEService::SOURCE_UNKNOWN ? NEService::ServiceConnected : NEService::ServicePending;
        else
            mStubAddress.SetSource( NEService::SOURCE_UNKNOWN );
    }
}
