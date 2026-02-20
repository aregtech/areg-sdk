/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ServerInfo.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, 
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
ServerInfo::ServerInfo()
    : mServerAddress( StubAddress::getInvalidStubAddress() )
    , mServerState  ( NEService::ServiceConnectionState::Unknown )
{
}

ServerInfo::ServerInfo( const StubAddress & server )
    : mServerAddress( server )
    , mServerState  (  )
{
    setConnectionStatus( NEService::ServiceConnectionState::Connected );
}

ServerInfo::ServerInfo( StubAddress && server )
    : mServerAddress( server )
    , mServerState  ( )
{
    setConnectionStatus( NEService::ServiceConnectionState::Connected );
}

ServerInfo::ServerInfo( const ProxyAddress & proxy )
    : mServerAddress( proxy.getServiceName(), proxy.getServiceVersion(), proxy.getServiceType(), proxy.getRoleName(), String::getEmptyString() )
    , mServerState  ( NEService::ServiceConnectionState::Pending )
{
    mServerAddress.invalidateChannel();
}

ServerInfo::ServerInfo( const ServerInfo & src )
    : mServerAddress( src.mServerAddress )
    , mServerState  ( src.mServerState )
{
}

ServerInfo::ServerInfo( ServerInfo && src ) noexcept
    : mServerAddress( std::move(src.mServerAddress) )
    , mServerState  ( std::move(src.mServerState) )
{
}

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
ServerInfo & ServerInfo::operator = ( const ServerInfo & src )
{
    mServerAddress  = src.mServerAddress;
    mServerState    = src.mServerState;

    return (*this);
}

ServerInfo & ServerInfo::operator = ( ServerInfo && src ) noexcept
{
    mServerAddress  = std::move(src.mServerAddress);
    mServerState    = std::move(src.mServerState);
    
    return (*this);
}

ServerInfo & ServerInfo::operator = ( const StubAddress & server )
{
    mServerAddress  = server;
    setConnectionStatus( NEService::ServiceConnectionState::Connected );
    
    return (*this);
}

ServerInfo & ServerInfo::operator = ( StubAddress && server ) noexcept
{
    mServerAddress  = std::move(server);
    setConnectionStatus( NEService::ServiceConnectionState::Connected );
    
    return (*this);
}

ServerInfo & ServerInfo::operator = (const ServiceAddress & addService)
{
    mServerAddress = addService;
    setConnectionStatus( NEService::ServiceConnectionState::Pending );

    return (*this);
}

ServerInfo & ServerInfo::operator = ( ServiceAddress && addService ) noexcept
{
    mServerAddress = std::move( addService );
    setConnectionStatus( NEService::ServiceConnectionState::Pending );

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

ServerInfo::operator unsigned int () const
{
    const ServiceAddress & addrService = static_cast<const ServiceAddress &>(mServerAddress);
    return static_cast<unsigned int>( addrService );
}

void ServerInfo::setConnectionStatus(NEService::ServiceConnectionState newConnection)
{
    if ( mServerAddress.getSource() != NEService::SOURCE_UNKNOWN )
        mServerState = newConnection;
    else if ( static_cast<const ServiceItem &>(mServerAddress).isValid() )
        mServerState = NEService::ServiceConnectionState::Pending;
    else
        mServerState = NEService::ServiceConnectionState::Unknown;
}
