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

namespace areg
{

    //////////////////////////////////////////////////////////////////////////
    // ClientList class implementation
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    // Methods
    //////////////////////////////////////////////////////////////////////////
    bool ClientList::existClient( const areg::ProxyAddress & client ) const
    {
        return contains(areg::ClientInfo(client));
    }

    const areg::ClientInfo & ClientList::getClient( const areg::ProxyAddress & whichClient ) const
    {
        LISTPOS pos = find(areg::ClientInfo(whichClient));
        return (isValidPosition(pos) ? *pos : areg::ClientInfo::getInvalidClientInfo());
    }

    const areg::ClientInfo & ClientList::registerClient( const areg::ProxyAddress & whichClient, const areg::ServerInfo & server )
    {
        areg::ClientInfo clInfo(whichClient);
        LISTPOS pos = find(clInfo);
        if (isInvalidPosition(pos))
        {
            pushLast(std::move(clInfo));
            pos = lastPosition();
        }

        areg::ClientInfo & client = valueAtPosition(pos);
        client.setTargetServer( server.getAddress() );
        client.setConnectionStatus( server.getConnectionStatus() );

        return client;
    }

    bool ClientList::unregisterClient( const areg::ProxyAddress & whichClient, areg::ClientInfo & out_client )
    {
        bool result{ false };

        LISTPOS pos = find( areg::ClientInfo(whichClient) );
        if (isValidPosition(pos))
        {
            removeAt(pos, out_client);
            result = true;
        }

        return result;
    }

    void ClientList::serverAvailable( const areg::ServerInfo & whichServer, ClientList & out_clientList )
    {
        areg::ServiceConnectionState state = whichServer.getConnectionStatus();
        const areg::StubAddress & addrStub = whichServer.getAddress();

        for ( LISTPOS pos = firstPosition(); isValidPosition(pos); ++ pos)
        {
            areg::ClientInfo & client = *pos;
            client.setTargetServer(addrStub);
            client.setConnectionStatus( state );
            out_clientList.pushFirst(client);
        }
    }

    void ClientList::serverUnavailable( ClientList & out_clientList )
    {
        for (LISTPOS pos = firstPosition(); isValidPosition(pos); ++pos )
        {
            areg::ClientInfo & client = valueAtPosition( pos );
            out_clientList.pushLast( client );
            client.setTargetServer( areg::StubAddress::getInvalidStubAddress() );
            client.setConnectionStatus( areg::ServiceConnectionState::Pending );
        }
    }
} // namespace areg
