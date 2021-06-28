/************************************************************************
 * \file        areg/component/private/ServerInfo.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, 
 *
 ************************************************************************/
#include "areg/component/private/ServerInfo.hpp"
#include "areg/component/ProxyAddress.hpp"

//////////////////////////////////////////////////////////////////////////
// ServerInfo class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
ServerInfo::ServerInfo( void )
    : mServerAddress( StubAddress::INVALID_STUB_ADDRESS )
    , mServerState  ( NEService::ServiceConnectionUnknown )
{
    ; // do nothing
}

ServerInfo::ServerInfo( const StubAddress & server )
    : mServerAddress( server )
    , mServerState  (  )
{
    setConnectionStatus( NEService::ServiceConnected );
}

ServerInfo::ServerInfo( const ProxyAddress & proxy )
    : mServerAddress( proxy.getServiceName(), proxy.getServiceVersion(), proxy.getServiceType(), proxy.getRoleName(), "" )
    , mServerState  ( NEService::ServicePending )
{
    mServerAddress.invalidateChannel();
}

ServerInfo::ServerInfo( const ServerInfo & src )
    : mServerAddress( src.mServerAddress )
    , mServerState  ( src.mServerState )
{
    ; // do nothing
}

ServerInfo::~ServerInfo( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
const ServerInfo & ServerInfo::operator = ( const ServerInfo & src )
{
    mServerAddress  = src.mServerAddress;
    mServerState    = src.mServerState;
    return (*this);
}

const ServerInfo & ServerInfo::operator = ( const StubAddress & server )
{
    mServerAddress  = server;
    setConnectionStatus( NEService::ServiceConnected );
    return (*this);
}

const ServerInfo & ServerInfo::operator = (const ServiceAddress & addService)
{
    mServerAddress = addService;
    setConnectionStatus( NEService::ServicePending );
    return (*this);
}

bool ServerInfo::operator == ( const ServerInfo & other ) const
{
    return (mServerAddress == other.mServerAddress);
}

bool ServerInfo::operator == ( const StubAddress & server ) const
{
    return server.getRoleName() == mServerAddress.getRoleName() && server.isServiceCompatible(mServerAddress.getService());
}

bool ServerInfo::operator == ( const ProxyAddress & proxy ) const
{
    return mServerAddress == proxy;
}

ServerInfo::operator unsigned int ( void ) const
{
    const ServiceAddress & addrService = static_cast<const ServiceAddress &>(mServerAddress);
    return static_cast<unsigned int>( addrService );
}

void ServerInfo::setConnectionStatus(NEService::eServiceConnection newConnection)
{
    if ( mServerAddress.getSource() != NEService::SOURCE_UNKNOWN )
        mServerState = newConnection;
    else if ( static_cast<const ServiceItem &>(mServerAddress).isValid() )
        mServerState = NEService::ServicePending;
    else
        mServerState = NEService::ServiceConnectionUnknown;
}
