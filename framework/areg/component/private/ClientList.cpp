/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ClientList.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Client List implementation
 *
 ************************************************************************/
#include "areg/component/private/ClientList.hpp"

#include "areg/component/private/ServerInfo.hpp"
#include "areg/component/ProxyAddress.hpp"
namespace areg {

//////////////////////////////////////////////////////////////////////////
// ClientList class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////
bool ClientList::exist( const ProxyAddress & client ) const noexcept
{
    return contains(ClientInfo(client));
}

const ClientInfo & ClientList::consumer( const ProxyAddress & whichClient ) const noexcept
{
    LISTPOS pos = find(ClientInfo(whichClient));
    return (is_valid_position(pos) ? *pos : ClientInfo::invalid_client_info());
}

const ClientInfo & ClientList::register_consumer( const ProxyAddress & whichClient, const ServerInfo & server )
{
    ClientInfo clInfo(whichClient);
    LISTPOS pos = find(clInfo);
    if (!is_valid_position(pos))
    {
        push_last(std::move(clInfo));
        pos = last_position();
    }

    ClientInfo & client = value_at(pos);
    client.set_target( server.address() );
    client.set_connection_status( server.connection_status() );

    return client;
}

bool ClientList::unregister_consumer( const ProxyAddress & whichClient, ClientInfo & clientInfo )
{
    bool result{ false };

    LISTPOS pos = find( ClientInfo(whichClient) );
    if (is_valid_position(pos))
    {
        remove_at(pos, clientInfo);
        result = true;
    }

    return result;
}

void ClientList::provider_available( const ServerInfo & whichServer, ClientList & clientInfoList )
{
    areg::ServiceConnectionState state = whichServer.connection_status();
    const StubAddress & addrStub = whichServer.address();

    for ( LISTPOS pos = first_position(); is_valid_position(pos); ++ pos)
    {
        ClientInfo & client = *pos;
        client.set_target(addrStub);
        client.set_connection_status( state );
        clientInfoList.push_first(client);
    }
}

void ClientList::provider_unavailable( ClientList & clientInfoList )
{
    for (LISTPOS pos = first_position(); is_valid_position(pos); ++pos )
    {
        ClientInfo & client = value_at( pos );
        clientInfoList.push_last( client );
        client.set_target( StubAddress::invalid_stub_address() );
        client.set_connection_status( areg::ServiceConnectionState::Pending );
    }
}

} // namespace areg
