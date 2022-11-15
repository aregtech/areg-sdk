/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ClientList.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Client List implementation
 *
 ************************************************************************/
#include "areg/component/private/ClientList.hpp"

#include "areg/component/private/ServerInfo.hpp"
#include "areg/component/ProxyAddress.hpp"

//////////////////////////////////////////////////////////////////////////
// ClientList class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////
bool ClientList::existClient( const ProxyAddress & client ) const
{
    return contains(ClientInfo(client));
}

const ClientInfo & ClientList::getClient( const ProxyAddress & whichClient ) const
{
    LISTPOS pos = find(ClientInfo(whichClient));
    return (isValidPosition(pos) ? *pos : ClientInfo::INVALID_CLIENT_INFO);
}

const ClientInfo & ClientList::registerClient( const ProxyAddress & whichClient, const ServerInfo & server )
{
    ClientInfo clInfo(whichClient);
    LISTPOS pos = find(clInfo);
    if (isInvalidPosition(pos))
    {
        pushLast(std::move(clInfo));
        pos = lastPosition();
    }

    ClientInfo & client = valueAtPosition(pos);
    client.setTargetServer( server.getAddress() );
    client.setConnectionStatus( server.getConnectionStatus() );

    return client;
}

bool ClientList::unregisterClient( const ProxyAddress & whichClient, ClientInfo & out_client )
{
    bool result = false;

    ClientInfo client(whichClient);
    LISTPOS pos = find(client);
    if (isValidPosition(pos))
    {
        ClientInfo& client = valueAtPosition(pos);
        client.setTargetServer(StubAddress::INVALID_STUB_ADDRESS);
        client.setConnectionStatus(NEService::eServiceConnection::ServicePending);
        removeAt(pos);
        result = true;
    }

    return result;
}

void ClientList::serverAvailable( const ServerInfo & whichServer, ClientList & out_clientList )
{
    NEService::eServiceConnection state = whichServer.getConnectionStatus();
    const StubAddress & addrStub = whichServer.getAddress();

    for ( LISTPOS pos = firstPosition(); isValidPosition(pos); ++ pos)
    {
        ClientInfo & client = *pos;
        client.setTargetServer(addrStub);
        client.setConnectionStatus( state );
        out_clientList.pushFirst(client);
    }
}

void ClientList::serverUnavailable( ClientList & out_clientList )
{
    for (LISTPOS pos = firstPosition(); isValidPosition(pos); ++pos )
    {
        ClientInfo & client = valueAtPosition( pos );
        client.setTargetServer( StubAddress::INVALID_STUB_ADDRESS );
        client.setConnectionStatus( NEService::eServiceConnection::ServicePending );
        out_clientList.pushLast(client);
    }
}
