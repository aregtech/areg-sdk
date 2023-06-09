/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ClientInfo.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Client Info implementation
 *
 ************************************************************************/
#include "areg/component/private/ClientInfo.hpp"
#include "areg/component/StubAddress.hpp"

#include <utility>

//////////////////////////////////////////////////////////////////////////
// ClientInfo class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constants
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Invalid client info object
 **/
const ClientInfo & ClientInfo::getInvalidClientInfo( void )
{
    static const ClientInfo _invalidClientInfo;
    return _invalidClientInfo;
}

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

ClientInfo::ClientInfo( void )
    : mClientAddress( )
    , mClientState  ( NEService::eServiceConnection::ServiceConnectionUnknown )
{
}

ClientInfo::ClientInfo( const ProxyAddress & client )
    : mClientAddress( client )
    , mClientState  ( NEService::eServiceConnection::ServiceConnectionUnknown )
{
    setConnectionStatus( NEService::eServiceConnection::ServicePending);
}

ClientInfo::ClientInfo( ProxyAddress && client ) noexcept
    : mClientAddress( static_cast<ProxyAddress &&>(client) )
    , mClientState  ( NEService::eServiceConnection::ServiceConnectionUnknown )
{
    setConnectionStatus( NEService::eServiceConnection::ServicePending );
}

ClientInfo::ClientInfo( const ClientInfo & src )
    : mClientAddress( src.mClientAddress )
    , mClientState  ( src.mClientState )
{
}

ClientInfo::ClientInfo( ClientInfo && src ) noexcept
    : mClientAddress( std::move(src.mClientAddress) )
    , mClientState  ( src.mClientState )
{
}

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////

ClientInfo & ClientInfo::operator = ( const ClientInfo & src )
{
    mClientAddress  = src.mClientAddress;
    mClientState    = src.mClientState;
    
    return (*this);
}

ClientInfo & ClientInfo::operator = ( ClientInfo && src ) noexcept
{
    mClientAddress  = std::move(src.mClientAddress);
    mClientState    = src.mClientState;

    return (*this);
}

ClientInfo & ClientInfo::operator = ( const ProxyAddress & client )
{
    mClientAddress  = client;
    setConnectionStatus(NEService::eServiceConnection::ServicePending);
    return (*this);
}

ClientInfo & ClientInfo::operator = ( ProxyAddress && client ) noexcept
{
    mClientAddress  = std::move(client);
    setConnectionStatus( NEService::eServiceConnection::ServicePending );
    return (*this);
}

bool ClientInfo::operator == (const ProxyAddress & client) const
{
    return mClientAddress == client;
}

bool ClientInfo::operator == (const ClientInfo & other) const
{
    return (this != &other ? mClientAddress == other.mClientAddress : true);
}

bool ClientInfo::operator == (const StubAddress & server) const
{
    return mClientAddress == server;
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////
void ClientInfo::setConnectionStatus( NEService::eServiceConnection newConnection )
{
    if ( mClientAddress.getSource() != NEService::SOURCE_UNKNOWN )
    {
        mClientState = mClientAddress.getTarget() != NEService::TARGET_UNKNOWN ? newConnection : NEService::eServiceConnection::ServicePending;
    }
    else
    {
        mClientState = NEService::eServiceConnection::ServiceConnectionUnknown;
    }
}

ClientInfo::operator unsigned int (void) const
{
    const ServiceAddress & addrService = static_cast<const ServiceAddress &>(mClientAddress);
    return static_cast<unsigned int>( addrService );
}

void ClientInfo::setTargetServer(const StubAddress & addrStub)
{
    if ( mClientAddress.getSource() != NEService::SOURCE_UNKNOWN )
    {
        mClientAddress.setTarget( addrStub.getSource() );
    }
}
