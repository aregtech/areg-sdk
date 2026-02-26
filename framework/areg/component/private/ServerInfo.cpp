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
    : mServerAddress( areg::StubAddress::getInvalidStubAddress() )
    , mServerState  ( areg::ServiceConnectionState::Unknown )
{
}

ServerInfo::ServerInfo( const areg::StubAddress & server )
    : mServerAddress( server )
    , mServerState  (  )
{
    setConnectionStatus( areg::ServiceConnectionState::Connected );
}

ServerInfo::ServerInfo( areg::StubAddress && server )
    : mServerAddress( server )
    , mServerState  ( )
{
    setConnectionStatus( areg::ServiceConnectionState::Connected );
}

ServerInfo::ServerInfo( const ProxyAddress & proxy )
    : mServerAddress( proxy.getServiceName(), proxy.getServiceVersion(), proxy.getServiceType(), proxy.getRoleName(), areg::String::getEmptyString() )
    , mServerState  ( areg::ServiceConnectionState::Pending )
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

ServerInfo & ServerInfo::operator = ( const areg::StubAddress & server )
{
    mServerAddress  = server;
    setConnectionStatus( areg::ServiceConnectionState::Connected );
    
    return (*this);
}

ServerInfo & ServerInfo::operator = ( areg::StubAddress && server ) noexcept
{
    mServerAddress  = std::move(server);
    setConnectionStatus( areg::ServiceConnectionState::Connected );
    
    return (*this);
}

ServerInfo & ServerInfo::operator = (const areg::ServiceAddress & addService)
{
    mServerAddress = addService;
    setConnectionStatus( areg::ServiceConnectionState::Pending );

    return (*this);
}

ServerInfo & ServerInfo::operator = ( areg::ServiceAddress && addService ) noexcept
{
    mServerAddress = std::move( addService );
    setConnectionStatus( areg::ServiceConnectionState::Pending );

    return (*this);
}

bool ServerInfo::operator == ( const ServerInfo & other ) const
{
    return (mServerAddress == other.mServerAddress);
}

bool ServerInfo::operator == ( const areg::StubAddress & server ) const
{
    return server.getRoleName() == mServerAddress.getRoleName() && server.isServiceCompatible(mServerAddress.getService());
}

bool ServerInfo::operator == ( const ProxyAddress & proxy ) const
{
    return mServerAddress == proxy;
}

ServerInfo::operator uint32_t () const
{
    const areg::ServiceAddress & addrService = static_cast<const areg::ServiceAddress &>(mServerAddress);
    return static_cast<uint32_t>( addrService );
}

void ServerInfo::setConnectionStatus(areg::ServiceConnectionState newConnection)
{
    if ( mServerAddress.getSource() != areg::SOURCE_UNKNOWN )
        mServerState = newConnection;
    else if ( static_cast<const areg::ServiceItem &>(mServerAddress).isValid() )
        mServerState = areg::ServiceConnectionState::Pending;
    else
        mServerState = areg::ServiceConnectionState::Unknown;
}
