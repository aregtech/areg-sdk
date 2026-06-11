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
namespace areg {

//////////////////////////////////////////////////////////////////////////
// ServerInfo class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
ServerInfo::ServerInfo()
    : mServerAddress( StubAddress::invalid_stub_address() )
    , mServerState  ( areg::ServiceConnectionState::Unknown )
{
}

ServerInfo::ServerInfo( const StubAddress & server )
    : mServerAddress( server )
    , mServerState  (  )
{
    set_connection_status( areg::ServiceConnectionState::Connected );
}

ServerInfo::ServerInfo( StubAddress && server )
    : mServerAddress( server )
    , mServerState  ( )
{
    set_connection_status( areg::ServiceConnectionState::Connected );
}

ServerInfo::ServerInfo( const ProxyAddress & proxy )
    : mServerAddress( proxy )
    , mServerState  ( areg::ServiceConnectionState::Pending )
{
    mServerAddress.invalidate_channel();
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
    set_connection_status( areg::ServiceConnectionState::Connected );
    
    return (*this);
}

ServerInfo & ServerInfo::operator = ( StubAddress && server ) noexcept
{
    mServerAddress  = std::move(server);
    set_connection_status( areg::ServiceConnectionState::Connected );
    
    return (*this);
}

ServerInfo & ServerInfo::operator = (const ServiceAddress & addService)
{
    mServerAddress = addService;
    set_connection_status( areg::ServiceConnectionState::Pending );

    return (*this);
}

ServerInfo & ServerInfo::operator = ( ServiceAddress && addService ) noexcept
{
    mServerAddress = std::move( addService );
    set_connection_status( areg::ServiceConnectionState::Pending );

    return (*this);
}

bool ServerInfo::operator == ( const ServerInfo & other ) const
{
    return (static_cast<const ServiceAddress&>(other.mServerAddress) == static_cast<const ServiceAddress&>(mServerAddress));
}

bool ServerInfo::operator == ( const StubAddress & server ) const
{
    return static_cast<const ServiceAddress &>(server) == static_cast<const ServiceAddress&>(mServerAddress) && server.is_service_compatible(mServerAddress.service());
}

bool ServerInfo::operator == ( const ProxyAddress & proxy ) const
{
    return mServerAddress == proxy;
}

ServerInfo::operator uint32_t () const
{
    return static_cast<uint32_t>(mServerAddress);
}

void ServerInfo::set_connection_status(areg::ServiceConnectionState newConnection)
{
    if ( mServerAddress.source() != areg::SOURCE_UNKNOWN )
        mServerState = newConnection;
    else if ( static_cast<const ServiceItem &>(mServerAddress).is_valid() )
        mServerState = areg::ServiceConnectionState::Pending;
    else
        mServerState = areg::ServiceConnectionState::Unknown;
}

} // namespace areg
