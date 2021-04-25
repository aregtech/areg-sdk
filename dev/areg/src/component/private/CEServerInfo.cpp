/************************************************************************
 * \file        areg/src/component/private/CEServerInfo.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, 
 *
 ************************************************************************/
#include "areg/src/component/private/CEServerInfo.hpp"
#include "areg/src/component/CEProxyAddress.hpp"

//////////////////////////////////////////////////////////////////////////
// CEServerInfo class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CEServerInfo::CEServerInfo( void )
    : mServerAddress( CEStubAddress::INVALID_STUB_ADDRESS )
    , mServerState  ( NEService::ServiceConnectionUnknown )
{
    ; // do nothing
}

CEServerInfo::CEServerInfo( const CEStubAddress & server )
    : mServerAddress( server )
    , mServerState  (  )
{
    SetConnectionStatus( NEService::ServiceConnected );
}

CEServerInfo::CEServerInfo( const CEProxyAddress & proxy )
    : mServerAddress( proxy.GetServiceName(), proxy.GetServiceVersion(), proxy.GetServiceType(), proxy.GetRoleName(), "" )
    , mServerState  ( NEService::ServicePending )
{
    mServerAddress.InvalidateChannel();
}

CEServerInfo::CEServerInfo( const CEServerInfo & src )
    : mServerAddress( src.mServerAddress )
    , mServerState  ( src.mServerState )
{
    ; // do nothing
}

CEServerInfo::~CEServerInfo( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
const CEServerInfo & CEServerInfo::operator = ( const CEServerInfo & src )
{
    mServerAddress  = src.mServerAddress;
    mServerState    = src.mServerState;
    return (*this);
}

const CEServerInfo & CEServerInfo::operator = ( const CEStubAddress & server )
{
    mServerAddress  = server;
    SetConnectionStatus( NEService::ServiceConnected );
    return (*this);
}

const CEServerInfo & CEServerInfo::operator = (const CEServiceAddress & addService)
{
    mServerAddress = addService;
    SetConnectionStatus( NEService::ServicePending );
    return (*this);
}

bool CEServerInfo::operator == ( const CEServerInfo & other ) const
{
    return (mServerAddress == other.mServerAddress);
}

bool CEServerInfo::operator == ( const CEStubAddress & server ) const
{
    return server.GetRoleName() == mServerAddress.GetRoleName() && server.IsServiceCompatible(mServerAddress.GetService());
}

bool CEServerInfo::operator == ( const CEProxyAddress & proxy ) const
{
    return mServerAddress == proxy;
}

CEServerInfo::operator unsigned int ( void ) const
{
    return static_cast<unsigned int>( mServerAddress );
}

void CEServerInfo::SetConnectionStatus(NEService::eServiceConnection newConnection)
{
    if ( mServerAddress.GetSource() != NEService::SOURCE_UNKNOWN )
        mServerState = newConnection;
    else if ( static_cast<const CEServiceItem &>(mServerAddress).IsValid() )
        mServerState = NEService::ServicePending;
    else
        mServerState = NEService::ServiceConnectionUnknown;
}
