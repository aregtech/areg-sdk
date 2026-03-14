/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ClientInfo.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Client Info implementation
 *
 ************************************************************************/
#include "areg/component/private/ClientInfo.hpp"
#include "areg/component/StubAddress.hpp"

#include <utility>
namespace areg {

//////////////////////////////////////////////////////////////////////////
// ClientInfo class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constants
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Invalid client info object
 **/
const ClientInfo & ClientInfo::invalid_client_info() noexcept
{
    static const ClientInfo _invalidClientInfo;
    return _invalidClientInfo;
}

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

ClientInfo::ClientInfo()
    : mClientAddress( )
    , mClientState  ( areg::ServiceConnectionState::Unknown )
{
}

ClientInfo::ClientInfo( const ProxyAddress & client )
    : mClientAddress( client )
    , mClientState  ( areg::ServiceConnectionState::Unknown )
{
    set_connection_status( areg::ServiceConnectionState::Pending);
}

ClientInfo::ClientInfo( ProxyAddress && client ) noexcept
    : mClientAddress( static_cast<ProxyAddress &&>(client) )
    , mClientState  ( areg::ServiceConnectionState::Unknown )
{
    set_connection_status( areg::ServiceConnectionState::Pending );
}

ClientInfo::ClientInfo( ClientInfo && src ) noexcept
    : mClientAddress( std::move(src.mClientAddress) )
    , mClientState  ( src.mClientState )
{
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////
void ClientInfo::set_connection_status( areg::ServiceConnectionState newConnection ) noexcept
{
    if ( mClientAddress.source() != areg::SOURCE_UNKNOWN )
    {
        mClientState = mClientAddress.target() != areg::TARGET_UNKNOWN ? newConnection : areg::ServiceConnectionState::Pending;
    }
    else
    {
        mClientState = areg::ServiceConnectionState::Unknown;
    }
}

void ClientInfo::set_target(const StubAddress & addrStub)
{
    if ( mClientAddress.source() != areg::SOURCE_UNKNOWN )
    {
        mClientAddress.set_target( addrStub.source() );
    }
}

} // namespace areg
